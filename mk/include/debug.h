
// Copyright 2025 Mist Tecnologia Ltda
// Copyright 2016 The Fuchsia Authors
// Copyright (c) 2008-2012 Travis Geiselbrecht
//
// Use of this source code is governed by a MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT

#ifndef MK_INCLUDE_DEBUG_H_
#define MK_INCLUDE_DEBUG_H_

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <zircon/compiler.h>

#include <platform/debug.h>

#if !defined(LK_DEBUGLEVEL)
#define LK_DEBUGLEVEL 0
#endif

#if !defined(DEBUG_PRINT_LEVEL)
#define DEBUG_PRINT_LEVEL 0
#endif

/* debug print levels */
#define CRITICAL 0
#define ALWAYS 0
#define INFO 1
#define SPEW 2

__BEGIN_CDECLS

#define DPRINTF_ENABLED_FOR_LEVEL(level) ((level) <= (DEBUG_PRINT_LEVEL))

#define dprintf(level, x...)                \
  do {                                      \
    if (DPRINTF_ENABLED_FOR_LEVEL(level)) { \
      printf(x);                            \
    }                                       \
  } while (0)

/* systemwide halts */
void panic(const char *fmt, ...) __PRINTFLIKE(1, 2) __NO_RETURN __NO_INLINE;

#define PANIC_UNIMPLEMENTED panic("%s unimplemented\n", __PRETTY_FUNCTION__)

void __stack_chk_fail(void) __NO_RETURN;

uintptr_t choose_stack_guard(void);

/* spin the cpu for a period of (short) time */
void spin(uint32_t usecs);

/* spin the cpu for a certain number of cpu cycles */
void spin_cycles(uint32_t usecs);

// A printf-like macro which prepend's the user's message with the special
// "ZIRCON KERNEL OOPS" tag.  zbi_test bots look for tags like this and consider
// their presence to indicate test failures, even if the higher level test
// framework code thinks the test passed.
//
// A KERNEL_OOPS indicates the presence of a bug, however, the bug may not be in
// the kernel itself.
#define KERNEL_OOPS(fmt, ...) printf("\nZIRCON KERNEL OOPS\n" fmt, ##__VA_ARGS__)

__END_CDECLS

#endif  // MK_INCLUDE_DEBUG_H_
