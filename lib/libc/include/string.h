// Copyright 2025 Mist Tecnologia Ltda
// Copyright 2016 The Fuchsia Authors
// Copyright (c) 2008 Travis Geiselbrecht
//
// Use of this source code is governed by a MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT

#ifndef LIB_LIBC_INCLUDE_STRING_H_
#define LIB_LIBC_INCLUDE_STRING_H_

// Other libc++ headers use <cstring> but libc++'s <cstring> complains if it
// thinks <string.h> is not its own wrapper, which we do not need.  We always
// get the kernel's <string.h> before the libc++ with the way the include paths
// are set up, which is necessary because of other headers where preempting
// libc++'s header is unavoidable for the kernel environment.  So just we
// define the macro that <cstring> checks for to keep it happy.
#define _LIBCPP_STRING_H

#include <stddef.h>

#include <lk/compiler.h>

__BEGIN_CDECLS

int memcmp(void const *, const void *, size_t) __PURE;
void *memcpy(void *, void const *, size_t);
void *memmove(void *, void const *, size_t);
void *memset(void *, int, size_t);

char *strcat(char *, char const *);
int strcmp(char const *, char const *) __PURE;
int strcasecmp(char const *, char const *) __PURE;
char *strcpy(char *, char const *);
char *strerror(int) __CONST;
size_t strlen(char const *) __PURE;
char *strncat(char *, char const *, size_t);
int strncmp(char const *, char const *, size_t) __PURE;
char *strncpy(char *, char const *, size_t);
size_t strspn(char const *, char const *) __PURE;
size_t strcspn(const char *s, const char *) __PURE;
char *strtok(char *, char const *);
int strcoll(const char *s1, const char *s2) __PURE;
size_t strxfrm(char *dest, const char *src, size_t n) __PURE;
char *strdup(const char *str) __MALLOC;

/* non standard */
void bcopy(void const *, void *, size_t);
void bzero(void *, size_t);
size_t strlcat(char *, char const *, size_t);
size_t strlcpy(char *, char const *, size_t);
int strncasecmp(char const *, char const *, size_t) __PURE;
int strnicmp(char const *, char const *, size_t) __PURE;
size_t strnlen(char const *s, size_t count) __PURE;

// Used by address sanitizer.
__typeof(memcpy) __unsanitized_memcpy;
__typeof(memmove) __unsanitized_memmove;
__typeof(memset) __unsanitized_memset;

__END_CDECLS
// C++ wants overloads for const and non-const versions of some functions.
#ifdef __cplusplus

const char *strchr(const char *, int) __asm__("strchr") __PURE;
inline char *strchr(char *s, int c) {
  return const_cast<char *>(strchr(const_cast<const char *>(s), c));
}

const char *strpbrk(const char *, const char *) __asm__("strpbrk") __PURE;
inline char *strpbrk(char *s1, const char *s2) {
  return const_cast<char *>(strpbrk(const_cast<const char *>(s1), s2));
}

const char *strrchr(const char *, int) __asm__("strrchr") __PURE;
inline char *strrchr(char *s, int c) {
  return const_cast<char *>(strrchr(const_cast<const char *>(s), c));
}

const void *memchr(const void *, int, size_t) __asm__("memchr") __PURE;
inline void *memchr(void *s, int c, size_t n) {
  return const_cast<void *>(memchr(const_cast<const void *>(s), c, n));
}

const char *strstr(const char *, const char *) __asm__("strstr") __PURE;
inline char *strstr(char *s1, const char *s2) {
  return const_cast<char *>(strstr(const_cast<const char *>(s1), s2));
}

#else

void *memchr(const void *, int, size_t);
char *strchr(const char *, int) __PURE;
char *strpbrk(const char *, const char *) __PURE;
char *strrchr(const char *, int) __PURE;
char *strstr(const char *, const char *) __PURE;

#endif  // __cplusplus

#endif  // LIB_LIBC_INCLUDE_STRING_H_
