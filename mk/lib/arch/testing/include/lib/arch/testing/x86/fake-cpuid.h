// Copyright 2020 The Fuchsia Authors
//
// Use of this source code is governed by a MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT

#ifndef ZIRCON_KERNEL_LIB_ARCH_TESTING_INCLUDE_LIB_ARCH_TESTING_X86_FAKE_CPUID_H_
#define ZIRCON_KERNEL_LIB_ARCH_TESTING_INCLUDE_LIB_ARCH_TESTING_X86_FAKE_CPUID_H_

#include <lib/arch/x86/cpuid.h>

#include <memory>

#include <fbl/intrusive_hash_table.h>

namespace arch::testing {

// Represents an x86 microprocessor, except where we stretch the term to also
// include hypervisors running atop them.
// Underscores are used to avoid the ambiguitity of otherwise concatenated
// alphanumeric identifiers.
// Entries are given chronologically within specific manufacture lines.
enum class X86Microprocessor {
  kIntelAtom330,
  kIntelAtomD510,
  kIntelAtomX5_Z8350,
  kIntelCeleron3855u,
  kIntelCore2_6300,
  kIntelCoreI3_3240,
  kIntelCoreI3_6100,
  kIntelCoreI5_7300u,
  kIntelCoreI7_2600k,
  kIntelCoreI7_6500u,
  kIntelCoreI7_6700k,
  kIntelCoreM3_7y30,
  kIntelPentiumN4200,
  kIntelXeonE5520,
  kIntelXeonE5_2690_V3,
  kIntelXeonE5_2690_V4,
  kAmdA10_7870k,
  kAmdRyzen5_1500x,
  kAmdRyzen7_1700,
  kAmdRyzen7_2700x,
  kAmdRyzen9_3950x,
  kAmdRyzen9_3950xVirtualBoxHyperv,
  kAmdRyzen9_3950xVirtualBoxKvm,
  kAmdRyzen9_3950xVmware,
  kAmdRyzen9_3950xWsl2,
  kAmdRyzenThreadripper1950x,
  kAmdRyzenThreadripper2970wx,
};

// FakeCpuidIo is a fake analogue to arch::BootCpuidIo, which may be provided
// in its place for tests - in the kernel and on host - for logic templated
// on any type the interface contract of the latter. A "CPUID I/O provider",
// FakeCpuidIo's methods are expected to be instantiated by "CPUID value
// types", defined in <lib/arch/x86/cpuid.h>.
//
// Using `Populate`, test authors can provide fake data for specific
// (sub)leaves.
//
// FakeCpuidIo is immovable and non-copyable; it is expected to be passed
// around by const reference.
class FakeCpuidIo {
 public:
  FakeCpuidIo() = default;

  // Constructs an FakeCpuidIo populated from the CPUID data collected from the
  // provided mircoprocessor.
  explicit FakeCpuidIo(X86Microprocessor microprocessor);

  // Returns the cached CpuidIo object corresponding to the particular CPUID
  // register type.
  template <typename CpuidValue>
  const CpuidIo* Get() const {
    return Get(CpuidValue::kLeaf, CpuidValue::kSubleaf);
  }

  // A convenience method to directly read a particular CPUID register type in
  // consultation with the associated cached CpuidIo objects.
  template <typename CpuidValue>
  auto Read() const {
    return CpuidValue::Get().ReadFrom(Get<CpuidValue>());
  }

  // Provides fake data for a given leaf. Subsequent calls can overwrite
  // previously populated data.
  FakeCpuidIo& Populate(uint32_t leaf, uint32_t subleaf, uint32_t eax, uint32_t ebx, uint32_t ecx,
                        uint32_t edx);
  FakeCpuidIo& Populate(uint32_t leaf, uint32_t subleaf, CpuidIo::Register reg, uint32_t value);

 private:
  // An intrusive data structure wrapping a CpuidIo object, required be stored
  // in a fbl::HashTable.
  //
  // This would be simpler using something more along the lines of
  // std::map<std::pair<uint32_t, uint32_t>, CpuidIo>, but we need to
  // support in-kernel testing where only fbl containers are available.
  struct Hashable : public fbl::SinglyLinkedListable<std::unique_ptr<Hashable>> {
    // Required to instantiate fbl::DefaultKeyedObjectTraits.
    uint64_t GetKey() const { return key_; }

    // Required to instantiate fbl::DefaultHashTraits.
    static size_t GetHash(uint64_t key) { return static_cast<size_t>(key); }

    uint64_t key_;
    CpuidIo cpuid_;
  };

  static uint64_t Key(uint32_t leaf, uint32_t subleaf) {
    return (static_cast<uint64_t>(subleaf) << 32) | static_cast<uint64_t>(leaf);
  }

  const CpuidIo* Get(uint32_t leaf, uint32_t subleaf) const;

  const CpuidIo empty_ = {};
  fbl::HashTable<uint64_t, std::unique_ptr<Hashable>> map_;
};

}  // namespace arch::testing

#endif  // ZIRCON_KERNEL_LIB_ARCH_TESTING_INCLUDE_LIB_ARCH_TESTING_X86_FAKE_CPUID_H_
