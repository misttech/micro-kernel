# Copyright 2025 Mist Tecnologia Ltda. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += $(LOCAL_DIR)/chainlock_transaction.cc
MODULE_SRCS += $(LOCAL_DIR)/seqlock.cc

MODULE_DEPS += lib/arch
MODULE_DEPS += lib/arch/$(ARCH)
MODULE_DEPS += ulib/affine
MODULE_DEPS += ulib/concurrent
MODULE_DEPS += ulib/lockdep

include make/module.mk
