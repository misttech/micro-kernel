# Copyright 2025 Mist Tecnologia Ltda. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += $(LOCAL_DIR)/copy.cc

MODULE_DEPS += lib/fxt
MODULE_DEPS += lib/arch
MODULE_DEPS += lib/arch/$(ARCH)
MODULE_DEPS += lib/stdcompat

include make/module.mk
