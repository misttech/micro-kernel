/*
 * Copyright (c) 2006-2015 Travis Geiselbrecht
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#include <lib/heap.h>

#include <new>

#include <lk/debug.h>

void *operator new(size_t s) { return malloc(s); }

void *operator new[](size_t s) { return malloc(s); }

#if 0
void *operator new(size_t s, const std::nothrow_t &) noexcept { return malloc(s); }

void *operator new[](size_t s, const std::nothrow_t &) noexcept { return malloc(s); }
#endif

void operator delete(void *p) { return free(p); }

void operator delete[](void *p) { return free(p); }

void operator delete(void *p, size_t s) { return free(p); }

void operator delete[](void *p, size_t s) { return free(p); }
