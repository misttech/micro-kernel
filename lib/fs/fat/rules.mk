LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_DEPS += lib/bcache
MODULE_DEPS += lib/bio
MODULE_DEPS += lib/fs
MODULE_DEPS += lib/libcpp

ifeq ($(call TOBOOL,WITH_TESTS),true)
MODULE_DEPS += $(LOCAL_DIR)/test
endif

MODULE_SRCS += $(LOCAL_DIR)/dir.cc
MODULE_SRCS += $(LOCAL_DIR)/fat.cc
MODULE_SRCS += $(LOCAL_DIR)/file.cc
MODULE_SRCS += $(LOCAL_DIR)/file_iterator.cc
MODULE_SRCS += $(LOCAL_DIR)/fs.cc

MODULE_COMPILEFLAGS += -Wmissing-declarations
MODULE_CPPFLAGS += -Wno-invalid-offsetof

include make/module.mk
