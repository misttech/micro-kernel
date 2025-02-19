/*
 * Copyright (c) 2014 Travis Geiselbrecht
 *
 * Use of this source code is governed by a MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 */

#ifndef MK_INCLUDE_ARCH_MP_H_
#define MK_INCLUDE_ARCH_MP_H_

#include <sys/types.h>

#include <kernel/mp.h>
#include <lk/compiler.h>

__BEGIN_CDECLS

/* send inter processor interrupt, if supported */
status_t arch_mp_send_ipi(mp_cpu_mask_t target, mp_ipi_t ipi);

void arch_mp_init_percpu(void);

__END_CDECLS

#endif  // MK_INCLUDE_ARCH_MP_H_
