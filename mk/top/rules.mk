LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_DEPS := \
	app \
	arch \
	dev \
	kernel \
	platform \
	target

MODULE_SRCS += $(LOCAL_DIR)/init.c
MODULE_SRCS += $(LOCAL_DIR)/main.cc
MODULE_SRCS += $(LOCAL_DIR)/debug.cc

MODULE_OPTIONS := extra_warnings

MODULE_DEFINES += _LIBCPP_VERBOSE_ABORT_NOT_NOEXCEPT

include make/module.mk
