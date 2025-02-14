
// Copyright 2025 Mist Tecnologia Ltda. All rights reserved.
// Copyright 2016 The Fuchsia Authors. All rights reserved.
// Copyright (c) 2008-2013 Travis Geiselbrecht
//
// Use of this source code is governed by a MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT
//

#ifndef TOP_INCLUDE_LK_COMPILER_H_
#define TOP_INCLUDE_LK_COMPILER_H_

#include <zircon/compiler.h>

#define __UNUSED __attribute__((__unused__))
#if __clang__
// Per https://clang.llvm.org/docs/AttributeReference.html#used
// __used__ does not prevent linkers from removing unused sections
// (if --gc-sections is passed). Need to specify "retain" as well.
#define __USED __attribute__((__used__, retain))
#else
#define __USED __attribute__((__used__))
#endif

#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6) || defined(__clang__)
#ifdef __cplusplus
#define STATIC_ASSERT(e) static_assert(e, #e)
#else
#define STATIC_ASSERT(e) _Static_assert(e, #e)
#endif
#else
#define STATIC_ASSERT(e) extern char (*ct_assert(void))[sizeof(char[1 - 2 * !(e)])]
#define STATIC_ASSERT(e) extern char (*ct_assert(void))[sizeof(char[1 - 2 * !(e)])]
#endif

/* compiler fence */
#define CF                             \
  do {                                 \
    __asm__ volatile("" ::: "memory"); \
  } while (0)

/* TODO: add type check */
#if !defined(countof)
#define countof(a) (sizeof(a) / sizeof((a)[0]))
#endif

#endif  // TOP_INCLUDE_LK_COMPILER_H_
