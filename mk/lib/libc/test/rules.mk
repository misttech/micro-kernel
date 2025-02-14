LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += $(LOCAL_DIR)/printf_tests.cc

MODULE_DEPS += lib/libc
MODULE_DEPS += lib/unittest

include make/module.mk

