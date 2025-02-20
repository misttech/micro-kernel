LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_OPTIONS := extra_warnings

MODULE_DEPS := \
	lib/libc

MODULE_SRCS += \
   $(LOCAL_DIR)/acpi_lite.cc

include make/module.mk
