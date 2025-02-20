/*
 * Copyright (c) 2008-2014 Travis Geiselbrecht
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */
#pragma once

#include <align.h>
#include <endian.h>
#include <malloc.h>
#include <rand.h>
#include <stddef.h>
#include <sys/types.h>

#include <arch/defines.h>
#include <lk/compiler.h>

__BEGIN_CDECLS

int atoi(const char *num);
double atof(const char *num);
unsigned int atoui(const char *num);
long atol(const char *num);
unsigned long atoul(const char *num);
unsigned long long atoull(const char *num);

long strtol(const char *nptr, char **endptr, int base);
long long strtoll(const char *nptr, char **endptr, int base);

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

/* allocate a buffer on the stack aligned and padded to the cpu's cache line size */
#define STACKBUF_DMA_ALIGN(var, size) uint8_t var[ROUNDUP(size, CACHE_LINE)] __ALIGNED(CACHE_LINE);
void abort(void) __attribute__((noreturn));
void qsort(void *aa, size_t n, size_t es, int (*cmp)(const void *, const void *));
void *bsearch(const void *key, const void *base, size_t num_elems, size_t size,
              int (*compare)(const void *, const void *));
unsigned long int strtoul(const char *nptr, char **endptr, int base);
char *getenv(const char *name);
int atexit(void (*func)(void));

__END_CDECLS
