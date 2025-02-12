LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_DEPS :=

MODULE_SRCS += $(LOCAL_DIR)/new.cc
MODULE_SRCS += $(LOCAL_DIR)/pure_virtual.cc

include make/module.mk
