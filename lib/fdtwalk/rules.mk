LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_DEPS := lib/fdt

MODULE_SRCS := $(LOCAL_DIR)/fdtwalk.cc
MODULE_SRCS += $(LOCAL_DIR)/helpers.cc

include make/module.mk
