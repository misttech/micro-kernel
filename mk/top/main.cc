// Copyright 2025 Mist Tecnologia LTDA
// Copyright 2016 The Fuchsia Authors
// Copyright (c) 2013-2015 Travis Geiselbrecht
//
// Use of this source code is governed by a MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT

/*
 * Main entry point to the OS. Initializes modules in order and creates
 * the default thread.
 */
#include "lk/main.h"

#include <app.h>
#include <arch.h>
#include <assert.h>
#include <debug.h>
#include <lib/console.h>
#include <lib/heap.h>
#include <platform.h>
#include <target.h>
#include <zircon/compiler.h>

#include <kernel/init.h>
#include <kernel/mutex.h>
#include <kernel/thread.h>
#include <lk/init.h>

/* saved boot arguments from whoever loaded the system */
ulong lk_boot_args[4];

#if WITH_SMP
static thread_t *secondary_bootstrap_threads[SMP_MAX_CPUS - 1];
static uint secondary_bootstrap_thread_count;
#endif

static int bootstrap2(void *arg);

static bool lk_global_constructors_called_flag = false;

extern void (*const __ctor_list[])(void);
extern void (*const __ctor_end[])(void);

bool lk_global_constructors_called(void) { return lk_global_constructors_called_flag; }

static void call_constructors(void) {
  for (void (*const *a)(void) = __ctor_list; a != __ctor_end; a++) {
    (*a)();
  }

  lk_global_constructors_called_flag = true;
}

/* called from arch code */
void lk_main(ulong arg0, ulong arg1, ulong arg2, ulong arg3) {
  // save the boot args
  lk_boot_args[0] = arg0;
  lk_boot_args[1] = arg1;
  lk_boot_args[2] = arg2;
  lk_boot_args[3] = arg3;

  // get us into some sort of thread context
  thread_init_early();

  // bring the debuglog up early so we can safely printf
  // dlog_init_early();

  // deal with any static constructors
  call_constructors();

  // we can safely printf now since we have the debuglog, the current thread set
  // which holds (a per-line buffer), and global ctors finished (some of the
  // printf machinery depends on ctors right now).
  // NOTE: botanist depends on this string being printed to serial. If this changes,
  // that code must be changed as well. See https://fxbug.dev/42138089#c20.
  // dprintf(ALWAYS, "printing enabled\n");

  lk_primary_cpu_init_level(LK_INIT_LEVEL_EARLIEST, LK_INIT_LEVEL_ARCH_EARLY - 1);

  // Carry out any early architecture-specific and platform-specific init
  // required to get the boot CPU and platform into a known state.
  arch_early_init();

  lk_primary_cpu_init_level(LK_INIT_LEVEL_ARCH_EARLY, LK_INIT_LEVEL_PLATFORM_EARLY - 1);

  platform_early_init();

  // do any super early target initialization
  lk_primary_cpu_init_level(LK_INIT_LEVEL_PLATFORM_EARLY, LK_INIT_LEVEL_ARCH_PREVM - 1);
  target_early_init();

  // At this point, the kernel command line and serial are set up.

  dprintf(INFO, "\nwelcome to mk\n\n");

  // dprintf(SPEW, "KASLR: Kernel image at %p\n", __executable_start);

  dprintf(INFO, "boot args 0x%lx 0x%lx 0x%lx 0x%lx\n", lk_boot_args[0], lk_boot_args[1],
          lk_boot_args[2], lk_boot_args[3]);

  // Perform any additional arch and platform-specific set up that needs to be done
  // before virtual memory or the heap are set up.
  dprintf(SPEW, "initializing vm pre-heap\n");
  // arch_prevm_init();
  lk_primary_cpu_init_level(LK_INIT_LEVEL_ARCH_PREVM, LK_INIT_LEVEL_PLATFORM_PREVM - 1);
  dprintf(SPEW, "initializing platform pre-vm\n");
  // platform_prevm_init();
  lk_primary_cpu_init_level(LK_INIT_LEVEL_PLATFORM_PREVM, LK_INIT_LEVEL_VM_PREHEAP - 1);

  // perform basic virtual memory setup
  dprintf(SPEW, "initializing vm pre-heap\n");
  // vm_init_preheap();
  lk_primary_cpu_init_level(LK_INIT_LEVEL_VM_PREHEAP, LK_INIT_LEVEL_HEAP - 1);

  // bring up the kernel heap
  dprintf(SPEW, "initializing heap\n");
  heap_init();
  lk_primary_cpu_init_level(LK_INIT_LEVEL_HEAP, LK_INIT_LEVEL_VM - 1);

  // enable virtual memory
  dprintf(SPEW, "initializing vm\n");
  // vm_init();
  lk_primary_cpu_init_level(LK_INIT_LEVEL_VM, LK_INIT_LEVEL_TOPOLOGY - 1);

  // initialize other parts of the kernel
  dprintf(SPEW, "initializing kernel\n");
  kernel_init();
  lk_primary_cpu_init_level(LK_INIT_LEVEL_KERNEL, LK_INIT_LEVEL_THREADING - 1);

  // Mark the current CPU as being active, then create a thread to complete
  // system initialization
  dprintf(SPEW, "creating bootstrap completion thread\n");
  // Scheduler::SetCurrCpuActive(true);
  thread_t *t =
      thread_create("bootstrap2", &bootstrap2, NULL, DEFAULT_PRIORITY, DEFAULT_STACK_SIZE);
  thread_set_pinned_cpu(t, 0);
  thread_detach(t);
  thread_resume(t);

  // become the idle thread and enable interrupts to start the scheduler
  thread_become_idle();
}

static int bootstrap2(void *arg) {
  DEBUG_ASSERT(lk_arch_curr_cpu_num() == BOOT_CPU_ID);

  dprintf(SPEW, "top of bootstrap2()\n");

  lk_primary_cpu_init_level(LK_INIT_LEVEL_THREADING, LK_INIT_LEVEL_ARCH - 1);
  arch_init();
  lk_primary_cpu_init_level(LK_INIT_LEVEL_ARCH, LK_INIT_LEVEL_PLATFORM - 1);

  dprintf(SPEW, "initializing platform\n");
  platform_init();
  lk_primary_cpu_init_level(LK_INIT_LEVEL_PLATFORM, LK_INIT_LEVEL_ARCH_LATE - 1);

  // At this point, other cores in the system have been started (though may
  // not yet be online).  Signal that the boot CPU is ready.
  // mp_signal_curr_cpu_ready();

  // Perform per-CPU set up on the boot CPU.
  dprintf(SPEW, "initializing late arch\n");
  target_init();
  // arch_late_init_percpu();
  lk_primary_cpu_init_level(LK_INIT_LEVEL_ARCH_LATE, LK_INIT_LEVEL_USER - 1);

  // Give the kernel shell an opportunity to run. If it exits this function, continue booting.
  kernel_shell_init();

  dprintf(SPEW, "initializing apps\n");
  apps_init();

  dprintf(SPEW, "moving to last init level\n");
  lk_primary_cpu_init_level(LK_INIT_LEVEL_USER, LK_INIT_LEVEL_LAST);

  // timeline_init.Set(current_mono_ticks());
  return 0;
}

#if WITH_SMP
void lk_secondary_cpu_entry(void) {
  uint cpu = arch_curr_cpu_num();

  if (cpu > secondary_bootstrap_thread_count) {
    dprintf(CRITICAL,
            "Invalid secondary cpu num %d, SMP_MAX_CPUS %d, secondary_bootstrap_thread_count %d\n",
            cpu, SMP_MAX_CPUS, secondary_bootstrap_thread_count);
    return;
  }

  thread_secondary_cpu_init_early();
  thread_resume(secondary_bootstrap_threads[cpu - 1]);

  dprintf(SPEW, "entering scheduler on cpu %d\n", cpu);
  thread_secondary_cpu_entry();
}

static int secondary_cpu_bootstrap2(void *arg) {
  /* secondary cpu initialize from threading level up. 0 to threading was handled in arch */
  lk_init_level(LK_INIT_FLAG_SECONDARY_CPUS, LK_INIT_LEVEL_THREADING, LK_INIT_LEVEL_LAST);

  return 0;
}

void lk_init_secondary_cpus(uint secondary_cpu_count) {
  if (secondary_cpu_count >= SMP_MAX_CPUS) {
    dprintf(CRITICAL, "Invalid secondary_cpu_count %d, SMP_MAX_CPUS %d\n", secondary_cpu_count,
            SMP_MAX_CPUS);
    secondary_cpu_count = SMP_MAX_CPUS - 1;
  }
  for (uint i = 0; i < secondary_cpu_count; i++) {
    dprintf(SPEW, "creating bootstrap completion thread for cpu %d\n", i + 1);
    thread_t *t = thread_create("secondarybootstrap2", &secondary_cpu_bootstrap2, NULL,
                                DEFAULT_PRIORITY, DEFAULT_STACK_SIZE);
    t->pinned_cpu = i + 1;
    thread_detach(t);
    secondary_bootstrap_threads[i] = t;
  }
  secondary_bootstrap_thread_count = secondary_cpu_count;
}
#endif
