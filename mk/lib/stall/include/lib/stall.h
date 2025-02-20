// Copyright 2024 The Fuchsia Authors
//
// Use of this source code is governed by a MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT
#ifndef ZIRCON_KERNEL_LIB_STALL_INCLUDE_LIB_STALL_H_
#define ZIRCON_KERNEL_LIB_STALL_INCLUDE_LIB_STALL_H_

#include <zircon/types.h>

#include <kernel/mutex.h>
#include <kernel/spinlock.h>
#include <kernel/thread.h>

// Maintains per-CPU stall timers in real time.
//
// Its counters are conceptually continuous. In fact, we update the saved state whenever the
// conditions change, so we can always compute the current value by extrapolating.
//
// With respect to stall contributions, threads are always in one of these three states:
//  - Not contributing at all to any accumulator.
//  - Contributing to an accumulator as a progressing thread.
//  - Contributing to an accumulator as a stalling thread.
class StallAccumulator {
 public:
  struct Stats {
    // Monotonic time spent with num_contributors_stalling > 0.
    zx_duration_mono_t total_time_stall_some = 0;

    // Monotonic time spent with num_contributors_stalling > 0 && num_contributors_progressing == 0.
    zx_duration_mono_t total_time_stall_full = 0;

    // Monotonic time spent with num_contributors_progressing > 0 || num_contributors_stalling > 0.
    zx_duration_mono_t total_time_active = 0;
  };

  // Alter contributor counts by the given amount.
  //
  // Only values between -1 and +1 are accepted.
  void Update(int op_contributors_progressing, int op_contributors_stalling);

  // Reads the current stats and resets them to zero.
  Stats Flush();

  // Internally called by the scheduler at every context switch.
  static void ApplyContextSwitch(Thread *current_thread, Thread *next_thread)
      TA_REQ(current_thread->get_lock(), next_thread->get_lock());

 private:
  void UpdateWithIrqDisabled(int op_contributors_progressing, int op_contributors_stalling)
      TA_EXCL(lock_);
  void Consolidate() TA_REQ(lock_);

  DECLARE_SPINLOCK(StallAccumulator) lock_;

  // Number of progressing threads currently tracked by this structure.
  size_t num_contributors_progressing_ TA_GUARDED(lock_) = 0;

  // Number of stalling threads currently tracked by this structure.
  size_t num_contributors_stalling_ TA_GUARDED(lock_) = 0;

  // Timestamp of the last Consolidate() call.
  zx_instant_mono_t last_consolidate_time_ TA_GUARDED(lock_) = 0;

  // Accumulated totals at the time of the last update.
  Stats accumulated_stats_ TA_GUARDED(lock_);
};

// Maintains system-wide stall stats by periodically aggregating measurements from per-CPU
// `StallAccumulator`s.
class StallAggregator {
 public:
  struct Stats {
    // Total monotonic time spent with at least one memory-stalled thread.
    zx_duration_mono_t stalled_time_some = 0;

    // Total monotonic time spent with all threads memory-stalled.
    zx_duration_mono_t stalled_time_full = 0;
  };

  // Gets this class' singleton instance.
  static StallAggregator *GetStallAggregator() { return &singleton_; }

  // Starts the sampling thread on the singleton instance.
  static void StartSamplingThread(uint level);

  // Returns the values of the aggregated stats.
  Stats ReadStats() const;

 private:
  friend class StallAggregatorTests;

  static StallAggregator singleton_;

  // Aggregates a new set of per-CPU samples. Called periodically by the sampling thread.
  //
  // The `iterate_per_cpu_stats` argument is a function that takes a callback and calls it for each
  // CPU, passing its per-CPU measurements collected since the previous call. It's always
  // `IteratePerCpuStats` at runtime, except for tests that inject a custom fake data provider.
  using PerCpuStatsCallback = fit::inline_function<void(const StallAccumulator::Stats &)>;
  void SampleOnce(
      fit::inline_function<void(PerCpuStatsCallback)> iterate_per_cpu_stats = IteratePerCpuStats);

  static void IteratePerCpuStats(PerCpuStatsCallback callback);

  mutable DECLARE_CRITICAL_MUTEX(StallAggregator) stats_lock_;
  Stats stats_ TA_GUARDED(stats_lock_) = {};
};

#endif  // ZIRCON_KERNEL_LIB_STALL_INCLUDE_LIB_STALL_H_
