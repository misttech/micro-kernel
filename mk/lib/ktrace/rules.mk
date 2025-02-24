# Copyright 2025 Mist Tecnologia Ltda. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += $(LOCAL_DIR)/ktrace.cc

MODULE_DEPS += lib/fxt
MODULE_DEPS += lib/ktl
MODULE_DEPS += lib/zircon-internal

# Silence warning in Clang about GNU extension used in string_ref.h.
MODULE_COMPILEFLAGS += -Wno-gnu-string-literal-operator-template

include make/module.mk
