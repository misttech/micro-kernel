// Copyright 2025 Mist Tecnologia Ltda. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TOP_INCLUDE_LK_DEBUG_H_
#define TOP_INCLUDE_LK_DEBUG_H_

#include_next <debug.h>

#include <pretty/hexdump.h>

__BEGIN_CDECLS

#if !DISABLE_DEBUG_OUTPUT

/* Obtain the panic file descriptor. */
FILE *get_panic_fd(void);

#else

/* Obtain the panic file descriptor. */
static inline FILE *get_panic_fd(void) { return NULL; }

#endif /* DISABLE_DEBUG_OUTPUT */

__END_CDECLS


#endif  // TOP_INCLUDE_LK_DEBUG_H_
