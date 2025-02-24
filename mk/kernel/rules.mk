# Copyright 2025 Mist Tecnologia Ltda
# Copyright (c) 2008-2015 Travis Geiselbrecht
#
# Use of this source code is governed by a MIT-style
# license that can be found in the LICENSE file or at
# https://opensource.org/licenses/MIT

LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_DEPS += lib/libc
MODULE_DEPS += lib/heap
MODULE_DEPS += lib/ktl

MODULE_SRCS += $(LOCAL_DIR)/debug.cc
MODULE_SRCS += $(LOCAL_DIR)/event.cc
MODULE_SRCS += $(LOCAL_DIR)/init.cc
MODULE_SRCS += $(LOCAL_DIR)/mutex.cc
MODULE_SRCS += $(LOCAL_DIR)/thread.cc
MODULE_SRCS += $(LOCAL_DIR)/timer.cc
MODULE_SRCS += $(LOCAL_DIR)/semaphore.cc
MODULE_SRCS += $(LOCAL_DIR)/mp.cc
MODULE_SRCS += $(LOCAL_DIR)/port.cc

ifeq ($(WITH_KERNEL_VM),1)
MODULE_DEPS += kernel/vm
else
MODULE_DEPS += kernel/novm
endif

MODULE_COMPILEFLAGS += -include hidden.h

MODULE_OPTIONS := extra_warnings

# Visibility annotations conflict with kernel/hidden.h.
MODULE_COMPILEFLAGS += -D_LIBCPP_DISABLE_VISIBILITY_ANNOTATIONS

include make/module.mk
