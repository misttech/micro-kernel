// Copyright 2025 Mist Tecnologia Ltda
// Copyright (c) 2008 Travis Geiselbrecht
//
// Use of this source code is governed by a MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT

#ifndef MK_INCLUDE_PLATFORM_TIMER_H_
#define MK_INCLUDE_PLATFORM_TIMER_H_

#include <sys/types.h>

#include <lk/compiler.h>

__BEGIN_CDECLS

/* Routines the core kernel calls to set up and use hardware timer based callbacks at
 * interrupt context.
 *
 * Most of the time this is handled in the platform layer but a few standalone timer
 * implementations in other parts of the system may implement, depending on the
 * configuration of the build.
 */

typedef enum handler_return (*platform_timer_callback)(void *arg, lk_time_t now);

/* If the platform only supports a single monotonic (fixed rate) timer */
status_t platform_set_periodic_timer(platform_timer_callback callback, void *arg,
                                     lk_time_t interval);

/* If the platform implements a full dynamic (can be set to arbitary points in the future) timer */
status_t platform_set_oneshot_timer(platform_timer_callback callback, void *arg,
                                    lk_time_t interval);
void platform_stop_timer(void);

__END_CDECLS

#endif  // MK_INCLUDE_PLATFORM_TIMER_H_
