# Copyright 2025 Mist Tecnologia Ltda
# Copyright (c) 2008-2015 Travis Geiselbrecht
#
# Use of this source code is governed by a MIT-style
# license that can be found in the LICENSE file or at
# https://opensource.org/licenses/MIT

LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_DEPS := \
	lib/libc \
	lib/debug \
	lib/heap \
	lib/ktl

MODULE_SRCS := \
	$(LOCAL_DIR)/debug.c \
	$(LOCAL_DIR)/event.c \
	$(LOCAL_DIR)/init.c \
	$(LOCAL_DIR)/mutex.c \
	$(LOCAL_DIR)/thread.c \
	$(LOCAL_DIR)/timer.c \
	$(LOCAL_DIR)/semaphore.c \
	$(LOCAL_DIR)/mp.c \
	$(LOCAL_DIR)/port.c

ifeq ($(WITH_KERNEL_VM),1)
MODULE_DEPS += kernel/vm
else
MODULE_DEPS += kernel/novm
endif

MODULE_COMPILEFLAGS += -include kernel/hidden.h

MODULE_OPTIONS := extra_warnings

# Visibility annotations conflict with kernel/include/hidden.h.
MODULE_COMPILEFLAGS += -D_LIBCPP_DISABLE_VISIBILITY_ANNOTATIONS

include make/module.mk
