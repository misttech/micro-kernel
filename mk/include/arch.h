// Copyright 2025 Mist Tecnologia Ltda
// Copyright (c) 2008-2014 Travis Geiselbrecht
//
// Use of this source code is governed by a MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT

#ifndef MK_INCLUDE_ARCH_H_
#define MK_INCLUDE_ARCH_H_

#include <sys/types.h>

#include <lk/compiler.h>

__BEGIN_CDECLS

void arch_early_init(void);
void arch_init(void);
void arch_quiesce(void);
void arch_chain_load(void *entry, ulong arg0, ulong arg1, ulong arg2, ulong arg3) __NO_RETURN;
void arch_enter_uspace(vaddr_t entry_point, vaddr_t user_stack_top) __NO_RETURN;

__END_CDECLS

/* arch specific bits */
#include <arch/defines.h>

#endif  // MK_INCLUDE_ARCH_H_
