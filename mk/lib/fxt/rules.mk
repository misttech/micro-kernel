# Copyright 2025 Mist Tecnologia Ltda. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += $(LOCAL_DIR)/fxt.cc

MODULE_DEPS += lib/fit
MODULE_DEPS += lib/stdcompat
MODULE_DEPS += lib/special-sections

# <fxt/writer.h> has #include <lib/zx/result.h>
# <fxt/serializer.h> has #include <lib/zx/result.h>
MODULE_DEPS += lib/zx

# Silence warning in Clang about GNU extension used in interned_string.h.
GLOBAL_CPPFLAGS += -Wno-gnu-string-literal-operator-template

include make/module.mk
