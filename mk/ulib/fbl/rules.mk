
# Copyright 2025 Mist Tecnologia Ltda
# Copyright 2016 The Fuchsia Authors
# Copyright (c) 2008-2015 Travis Geiselbrecht
#
# Use of this source code is governed by a MIT-style
# license that can be found in the LICENSE file or at
# https://opensource.org/licenses/MIT

LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_DEPS += lib/ktl
MODULE_DEPS += lib/heap

MODULE_SRCS := $(LOCAL_DIR)/alloc_checker.cc

include make/module.mk
