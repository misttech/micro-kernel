// Copyright 2017 The Fuchsia Authors
//
// Use of this source code is governed by a MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT

#ifndef ZIRCON_KERNEL_LIB_COUNTERS_INCLUDE_LIB_COUNTERS_H_
#define ZIRCON_KERNEL_LIB_COUNTERS_INCLUDE_LIB_COUNTERS_H_

#include <lib/special-sections/special-sections.h>

#include <arch/ops.h>
#include <kernel/percpu.h>
#include <ktl/algorithm.h>
#include <ktl/atomic.h>
#include <ktl/limits.h>

#include "counter-vmo-abi.h"

// Kernel counters are a facility designed to help field diagnostics and
// to help devs properly dimension the load/clients/size of the kernel
// constructs. It answers questions like:
//   - after N seconds how many outstanding <x> things are allocated?
//   - up to this point has <Y> ever happened?
//   - up to this point what is min(<Z>) or max(<Z>)?
//
// Currently the only query interface to the counters is the kcounter command.
// Issue 'kcounter --help' to learn what it can do.
//
// Kernel counters public API:
//
// 1- Define a new counter in a .cc file. Do not define a counter in a header
// file as that may lead to the creation of multiple, unrelated counters. Do not
// define multiple counters with the same name.
//
//      KCOUNTER(counter_name, "<counter name>");
//      KCOUNTER_DECLARE(counter_name, "<counter name>", Sum);
//      KCOUNTER_DECLARE(counter_name, "<counter name>", Min);
//      KCOUNTER_DECLARE(counter_name, "<counter name>", Max);
//
// 2- Counters start at zero, but can be set to other values during startup.
//
// Update the counter using sum, min, or max operations:
//
//      kcounter_add(counter_name, 1);
//      kcounter_min(counter_name, 1);
//      kcounter_max(counter_name, 1);
//
// By default with KCOUNTER, the `kcounter` presentation will calculate a sum()
// across cores. The Min and Max presentation types can be specified by using
// KCOUNTER_DECLARE and will present with a min() or max() across cores,
// respectively.
//
// Naming the counters
// The naming convention is "subsystem.thing_or_action"
// for example "dispatcher.destroy"
//             "exceptions.fpu"
//             "handles.live"
//
// The counter update methods use relaxed atomic loads and stores only to avoid
// formal data races in the C++ memory model, not to ensure atomicity with
// respect to concurrent updates. Consistency under concurrent updates may
// require read-modify-write operations on some architectures, which can
// incur significant code gen complexity and runtime overhead, particularly
// when preemption/interrupts are not disabled during updates. Since counters
// are expected to be used in situations where concurrent updates are unlikely,
// and where performance outweighs absolute accuracy, the operations are not
// required to be atomic with respect to concurrent updates.
//
// The following provides some motivating comparisons of code gen on armv8-a:
// https://godbolt.org/z/osz68aKq6
//
// **NOTE**: Because the accuracy and consistency of the counter values is not
// guaranteed, care must be taken when reading the counter values in non-
// diagnostic code. In the appropriate diagnostic use cases, counter values may
// be read via |ValueCurrCpu|, |SumAcrossAllCpus|, |MinAcrossAllCpus|, or
// |MaxAcrossAllCpus|. When using these methods, please include comments to make
// it clear that the values are NOT reliable for production logic or non-
// diagnostic purposes.

class CounterDesc {
 public:
  constexpr const counters::Descriptor* begin() const { return begin_; }
  constexpr const counters::Descriptor* end() const { return end_; }
  size_t size() const { return end() - begin(); }

  constexpr auto VmoData() const { return &vmo_begin_; }

  size_t VmoDataSize() const {
    return (reinterpret_cast<uintptr_t>(vmo_end_) - reinterpret_cast<uintptr_t>(&vmo_begin_));
  }

  size_t VmoContentSize() const {
    return (reinterpret_cast<uintptr_t>(end_) - reinterpret_cast<uintptr_t>(&vmo_begin_));
  }

 private:
  // Via magic in kernel.ld, all the descriptors wind up in a contiguous
  // array bounded by these two symbols, sorted by name.
  static const counters::Descriptor begin_[] __asm__("kcountdesc_begin");
  static const counters::Descriptor end_[] __asm__("kcountdesc_end");

  // That array sits inside a region that's page-aligned and padded out to
  // page size.  The region as a whole has the DescriptorVmo layout.
  static const counters::DescriptorVmo vmo_begin_ __asm__("k_counter_desc_vmo_begin");
  static const counters::Descriptor vmo_end_[] __asm__("k_counter_desc_vmo_end");
};

class CounterArena {
 public:
  int64_t* CpuData(size_t idx) const { return &arena_[idx * CounterDesc().size()]; }

  constexpr int64_t* VmoData() const { return arena_; }

  size_t VmoDataSize() const {
    return (reinterpret_cast<uintptr_t>(arena_page_end_) - reinterpret_cast<uintptr_t>(arena_));
  }

  size_t VmoContentSize() const {
    return (reinterpret_cast<uintptr_t>(arena_end_) - reinterpret_cast<uintptr_t>(arena_));
  }

 private:
  // Parallel magic in kernel.ld allocates int64_t[SMP_MAX_CPUS] worth
  // of data space for each counter.
  static int64_t arena_[] __asm__("kcounters_arena");
  static int64_t arena_end_[] __asm__("kcounters_arena_end");
  // That's page-aligned and padded out to page size.
  static int64_t arena_page_end_[] __asm__("kcounters_arena_page_end");
};

class Counter {
 public:
  explicit constexpr Counter(const counters::Descriptor* desc) : desc_(desc) {}

  // Return the sum of the per-cpu slots for this counter.
  int64_t SumAcrossAllCpus() const {
    int64_t sum = 0;
    percpu::ForEach([&](cpu_num_t cpu_num, percpu* p) {
      ktl::atomic_ref<int64_t> slot(p->counters[Index()]);
      sum += slot.load(ktl::memory_order_relaxed);
    });
    return sum;
  }

  // Return the max of the per-cpu slots for this counter.
  int64_t MaxAcrossAllCpus() const {
    int64_t max_value = ktl::numeric_limits<int64_t>::min();
    percpu::ForEach([&](cpu_num_t cpu_num, percpu* p) {
      ktl::atomic_ref<int64_t> slot(p->counters[Index()]);
      max_value = ktl::max(max_value, slot.load(ktl::memory_order_relaxed));
    });
    return max_value;
  }

  // Return the min of the per-cpu slots for this counter.
  int64_t MinAcrossAllCpus() const {
    int64_t min_value = ktl::numeric_limits<int64_t>::max();
    percpu::ForEach([&](cpu_num_t cpu_num, percpu* p) {
      ktl::atomic_ref<int64_t> slot(p->counters[Index()]);
      min_value = ktl::min(min_value, slot.load(ktl::memory_order_relaxed));
    });
    return min_value;
  }

  // Return the value of the calling cpu's slot for this counter.
  int64_t ValueCurrCpu() const {
    ktl::atomic_ref<int64_t> slot(*Slot());
    return slot.load(ktl::memory_order_relaxed);
  }

  // Set the value of calling cpu's slot to |value|. No memory order is implied.
  void Set(uint64_t value) const {
    ktl::atomic_ref<int64_t> slot(*Slot());
    slot.store(value, ktl::memory_order_relaxed);
  }

  void Add(int64_t delta) const {
    ktl::atomic_ref<int64_t> slot(*Slot());
    slot.store(slot.load(ktl::memory_order_relaxed) + delta, ktl::memory_order_relaxed);
  }

  void Max(int64_t value) const {
    ktl::atomic_ref<int64_t> slot(*Slot());
    const int64_t current = slot.load(ktl::memory_order_relaxed);
    if (value > current) {
      slot.store(value, ktl::memory_order_relaxed);
    }
  }

  // Note: Since counters are defined in zero-initialized storage, a min counter
  // must be set to the maximum value using Set() at some time during boot.
  void Min(int64_t value) const {
    ktl::atomic_ref<int64_t> slot(*Slot());
    const int64_t current = slot.load(ktl::memory_order_relaxed);
    if (value < current) {
      slot.store(value, ktl::memory_order_relaxed);
    }
  }

 private:
  int64_t* Slot() const { return &percpu::GetCurrent().counters[Index()]; }

  // The order of the descriptors is the order of the slots in each per-CPU
  // array.
  constexpr size_t Index() const { return desc_ - CounterDesc().begin(); }

  const counters::Descriptor* desc_;
};

// Define the descriptor and reserve the arena space for the counters.  Place
// each kcounter_arena_* array in a .bss.kcounter.* section; kernel.ld
// recognizes those names and places them all together to become the contiguous
// kcounters_arena array.  Note that each kcounter_arena_* does not correspond
// with the slots used for this particular counter (that would have terrible
// cache effects); it just reserves enough space for counters_init() to dole
// out in per-CPU chunks.
#define KCOUNTER_DECLARE(var, name, type)                                                     \
  namespace {                                                                                 \
  static_assert(__INCLUDE_LEVEL__ == 0,                                                       \
                "kcounters should not be defined in a header as doing so may result in the "  \
                "creation of multiple unrelated counters with the same name");                \
  int64_t kcounter_arena_##var SPECIAL_SECTION(".bss.kcounter." name, int64_t)[SMP_MAX_CPUS]; \
  const counters::Descriptor kcounter_desc_##var SPECIAL_SECTION("kcountdesc." name,          \
                                                                 counters::Descriptor) = {    \
      name,                                                                                   \
      counters::Type::k##type,                                                                \
  };                                                                                          \
  constexpr Counter var(&kcounter_desc_##var);                                                \
  }  // anonymous namespace

#define KCOUNTER(var, name) KCOUNTER_DECLARE(var, name, Sum)

inline void kcounter_add(const Counter& counter, int64_t delta) { counter.Add(delta); }
inline void kcounter_min(const Counter& counter, int64_t value) { counter.Min(value); }
inline void kcounter_max(const Counter& counter, int64_t value) { counter.Max(value); }

#endif  // ZIRCON_KERNEL_LIB_COUNTERS_INCLUDE_LIB_COUNTERS_H_
