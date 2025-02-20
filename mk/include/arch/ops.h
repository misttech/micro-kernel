// Copyright 2025 Mist Tecnologia Ltda
// Copyright (c) 2008-2014 Travis Geiselbrecht
//
// Use of this source code is governed by a MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT

#ifndef MK_INCLUDE_ARCH_OPS_H_
#define MK_INCLUDE_ARCH_OPS_H_

#ifndef __ASSEMBLER__

#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>

#include <lk/compiler.h>

__BEGIN_CDECLS

/* fast routines that most arches will implement inline */
static void arch_enable_ints(void);
static void arch_disable_ints(void);
static bool arch_ints_disabled(void);
static bool arch_in_int_handler(void);

static ulong arch_cycle_count(void);

static uint arch_curr_cpu_num(void);

/* Use to align structures on cache lines to avoid cpu aliasing. */
#define __CPU_ALIGN __ALIGNED(MAX_CACHE_LINE)

void arch_disable_cache(uint flags);
void arch_enable_cache(uint flags);

void arch_clean_cache_range(addr_t start, size_t len);
void arch_clean_invalidate_cache_range(addr_t start, size_t len);
void arch_invalidate_cache_range(addr_t start, size_t len);
void arch_sync_cache_range(addr_t start, size_t len);

void arch_idle(void);

__END_CDECLS

#endif  // !__ASSEMBLER__

/* for the above arch enable/disable routines */
#define ARCH_CACHE_FLAG_ICACHE 1
#define ARCH_CACHE_FLAG_DCACHE 2
#define ARCH_CACHE_FLAG_UCACHE (ARCH_CACHE_FLAG_ICACHE | ARCH_CACHE_FLAG_DCACHE)

// Give the specific arch a chance to override some routines.
#include <arch/arch_ops.h>

#endif  // MK_INCLUDE_ARCH_OPS_H_
