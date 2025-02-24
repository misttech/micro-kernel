# Copyright 2025 Mist Tecnologia Ltda. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += $(LOCAL_DIR)/energy-model.cc
MODULE_SRCS += $(LOCAL_DIR)/power-state.cc
MODULE_SRCS += $(LOCAL_DIR)/kernel-registry.cc
#MODULE_SRCS += $(LOCAL_DIR)/port-power-level-controller.cc

MODULE_DEPS += ulib/fbl
MODULE_DEPS += lib/ktl

include make/module.mk
