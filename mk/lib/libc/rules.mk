LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_DEPS := \
	lib/heap \
	lib/io

MODULE_SRCS += \
	$(LOCAL_DIR)/abort.c \
	$(LOCAL_DIR)/atexit.c \
	$(LOCAL_DIR)/atoi.c \
	$(LOCAL_DIR)/bsearch.c \
	$(LOCAL_DIR)/ctype.c \
	$(LOCAL_DIR)/errno.c \
	$(LOCAL_DIR)/rand.c \
	$(LOCAL_DIR)/strtol.c \
	$(LOCAL_DIR)/strtoll.c \
	$(LOCAL_DIR)/stdio.c \
	$(LOCAL_DIR)/qsort.c \
	$(LOCAL_DIR)/eabi.c

MODULE_FLOAT_SRCS += \
	$(LOCAL_DIR)/printf.c \
	$(LOCAL_DIR)/atof.c \

MODULE_COMPILEFLAGS += -fno-builtin

MODULE_OPTIONS := extra_warnings

ifeq ($(call TOBOOL,WITH_TESTS),true)
MODULE_DEPS += $(LOCAL_DIR)/test
endif

# libc/include is before toolchain headers because it needs to be able to
# override some libc++ headers that won't work in the kernel context.
# However, libc/include/limits.h punts to the toolchain via #include_next
# <limits.h> and the toolchain's limits.h does the same to get the "system"
# libc <limits.h>, so we need another include directory after the toolchain
# headers that has a limits.h for that to find, even though in the kernel
# there is nothing to add to the toolchain's <limits.h> content.
GLOBAL_COMPILEFLAGS += -idirafter $(LOCAL_DIR)/include-after

include $(LOCAL_DIR)/string/rules.mk

include make/module.mk
