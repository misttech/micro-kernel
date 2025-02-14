/*
 * Copyright (c) 2008-2015 Travis Geiselbrecht
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#pragma once

#include <stddef.h>
#include <sys/types.h>

#include <lk/compiler.h>

__BEGIN_CDECLS

// The underlying cmpctmalloc allocator defaults to 8 byte alignment.
#define HEAP_DEFAULT_ALIGNMENT 8

#ifdef _KERNEL

// define this to enable collection of all unique call sites with unique sizes
#define HEAP_COLLECT_STATS 0

// standard heap definitions
void* malloc(size_t size) __MALLOC __ALLOC_SIZE(1);
void* memalign(size_t alignment, size_t size) __MALLOC __ALLOC_SIZE(2);
void* calloc(size_t count, size_t size) __MALLOC __ALLOC_SIZE(1, 2);
void free(void* ptr);
void sized_free(void* ptr, size_t s);

/* critical section time delayed free */
void heap_delayed_free(void*);

/* tell the heap to return any free pages it can find */
void heap_trim(void);

// called once at kernel initialization
void heap_init(void);

#endif  // _KERNEL

__END_CDECLS
