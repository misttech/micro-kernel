LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

# two implementations, modern and legacy
# legacy implies older hardware, pre pentium, pre pci
CPU = modern

MODULE_DEPS += lib/acpi_lite 
MODULE_DEPS += lib/bio 
MODULE_DEPS += lib/cbuf

MODULE_DEPS += dev/bus/pci/drivers

MODULE_SRCS += $(LOCAL_DIR)/cmos.cc
MODULE_SRCS += $(LOCAL_DIR)/console.cc
MODULE_SRCS += $(LOCAL_DIR)/debug.cc
MODULE_SRCS += $(LOCAL_DIR)/ide.c
MODULE_SRCS += $(LOCAL_DIR)/interrupts.cc
MODULE_SRCS += $(LOCAL_DIR)/keyboard.cc
MODULE_SRCS += $(LOCAL_DIR)/lapic.cc
MODULE_SRCS += $(LOCAL_DIR)/pic.cc
MODULE_SRCS += $(LOCAL_DIR)/platform.cc
MODULE_SRCS += $(LOCAL_DIR)/timer.cc
MODULE_SRCS += $(LOCAL_DIR)/uart.cc

LK_HEAP_IMPLEMENTATION ?= cmpctmalloc

# Underlying kernel heap only has default alignment of 8 bytes, so pass
# this to the compiler as the default new alignment.
GLOBAL_COMPILEFLAGS += -faligned-new=8

include make/module.mk

