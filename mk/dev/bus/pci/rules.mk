LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += $(LOCAL_DIR)/debug.cc
MODULE_SRCS += $(LOCAL_DIR)/pci.cc

MODULE_SRCS += $(LOCAL_DIR)/bus_mgr/bridge.cc
MODULE_SRCS += $(LOCAL_DIR)/bus_mgr/bus.cc
MODULE_SRCS += $(LOCAL_DIR)/bus_mgr/bus_mgr.cc
MODULE_SRCS += $(LOCAL_DIR)/bus_mgr/device.cc
MODULE_SRCS += $(LOCAL_DIR)/bus_mgr/resource.cc

MODULE_SRCS += $(LOCAL_DIR)/backend/ecam.cc
MODULE_SRCS += $(LOCAL_DIR)/backend/bios32.cc
MODULE_SRCS += $(LOCAL_DIR)/backend/type1.cc

MODULE_DEPS += lib/libc

MODULE_CPPFLAGS += -Wno-invalid-offsetof
MODULE_COMPILEFLAGS += -Wmissing-declarations

include make/module.mk
