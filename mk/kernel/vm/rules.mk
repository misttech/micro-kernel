LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += $(LOCAL_DIR)/bootalloc.c
MODULE_SRCS += $(LOCAL_DIR)/pmm.c
MODULE_SRCS += $(LOCAL_DIR)/vm.c
MODULE_SRCS += $(LOCAL_DIR)/vmm.c

MODULE_OPTIONS := extra_warnings

include make/module.mk
