# Copyright 2025 Mist Tecnologia Ltda
# Copyright 2017 The Fuchsia Authors
#
# Use of this source code is governed by a MIT-style
# license that can be found in the LICENSE file or at
# https://opensource.org/licenses/MIT

LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_DEPS += lib/zircon-internal

MODULE_SRCS += $(LOCAL_DIR)/hexdump.cc
MODULE_SRCS += $(LOCAL_DIR)/sizes.cc

include make/module.mk
