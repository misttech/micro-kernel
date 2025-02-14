# Copyright 2025 Mist Tecnologia LTDA. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_DEPS += lib/stdcompat
MODULE_DEPS += lib/libc

MODULE_SRCS += $(LOCAL_DIR)/align.cc
MODULE_SRCS += $(LOCAL_DIR)/popcount.cc
MODULE_SRCS += $(LOCAL_DIR)/stub-new.cc

# ktl headers come before libc++ headers so they can override a few.  But
# some libc++ headers use #include_next, so we need another directory that
# comes after the libc++ headers where ktl can provide the headers libc++
# expects to come from libc.
GLOBAL_CPPFLAGS += -idirafter $(LOCAL_DIR)/include-after

include make/module.mk