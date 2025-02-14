# Copyright 2025 Mist Tecnologia Ltda
# Copyright 2016 The Fuchsia Authors
# Copyright (c) 2008-2015 Travis Geiselbrecht
#
# Use of this source code is governed by a MIT-style
# license that can be found in the LICENSE file or at
# https://opensource.org/licenses/MIT

LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_OPTIONS := extra_warnings

# x86 code always runs with the mmu enabled
WITH_KERNEL_VM := 1

GLOBAL_DEFINES += \
	IS_64BIT=1 \

MEMBASE ?= 0
KERNEL_BASE ?= 0xffffffff80000000
KERNEL_LOAD_OFFSET ?= 0x00200000
KERNEL_ASPACE_BASE ?= 0xffffff8000000000UL # -512GB
KERNEL_ASPACE_SIZE ?= 0x0000008000000000UL
USER_ASPACE_BASE   ?= 0x0000000000000000UL
USER_ASPACE_SIZE   ?= 0x0000800000000000UL

GLOBAL_DEFINES += \
	ARCH_X86_64=1 \
	MEMBASE=$(MEMBASE) \
	KERNEL_BASE=$(KERNEL_BASE) \
	KERNEL_LOAD_OFFSET=$(KERNEL_LOAD_OFFSET) \
	KERNEL_ASPACE_BASE=$(KERNEL_ASPACE_BASE) \
	KERNEL_ASPACE_SIZE=$(KERNEL_ASPACE_SIZE) \
	SMP_MAX_CPUS=1 \
	ARCH_HAS_MMU=1

MODULE_SRCS += \
	$(LOCAL_DIR)/start.S \
	$(LOCAL_DIR)/asm.S \
	$(LOCAL_DIR)/exceptions.S \
	$(LOCAL_DIR)/mmu.c \
	$(LOCAL_DIR)/ops.S \
	$(LOCAL_DIR)/arch.c \
	$(LOCAL_DIR)/cache.c \
	$(LOCAL_DIR)/descriptor.c \
	$(LOCAL_DIR)/faults.c \
	$(LOCAL_DIR)/feature.c \
	$(LOCAL_DIR)/gdt.S \
	$(LOCAL_DIR)/thread.c \
	$(LOCAL_DIR)/fpu.c

GLOBAL_DEFINES += \
	X86_WITH_FPU=1

MODULE_DEPS += lib/pretty

include $(LOCAL_DIR)/toolchain.mk

# set the default toolchain to x86 elf and set a #define
ifndef TOOLCHAIN_PREFIX
TOOLCHAIN_PREFIX := $(ARCH_x86_64_TOOLCHAIN_PREFIX)
endif

#$(warning ARCH_x86_TOOLCHAIN_PREFIX = $(ARCH_x86_TOOLCHAIN_PREFIX))
#$(warning ARCH_x86_64_TOOLCHAIN_PREFIX = $(ARCH_x86_64_TOOLCHAIN_PREFIX))
#$(warning TOOLCHAIN_PREFIX = $(TOOLCHAIN_PREFIX))

cc-option = $(shell if test -z "`$(1) $(2) -S -o /dev/null -xc /dev/null 2>&1`"; \
	then echo "$(2)"; else echo "$(3)"; fi ;)

# disable SSP if the compiler supports it; it will break stuff
GLOBAL_CFLAGS += $(call cc-option,$(CC),-fno-stack-protector,)

CLANG_ARCH := x86_64
ARCH_LDFLAGS += -m elf_x86_64

ARCH_COMPILEFLAGS += -fasynchronous-unwind-tables
ARCH_COMPILEFLAGS += -gdwarf-2
ARCH_COMPILEFLAGS += -fno-pic
ARCH_LDFLAGS += -z max-page-size=4096

ARCH_COMPILEFLAGS += $(NO_SAFESTACK) $(NO_SANITIZERS)
ARCH_COMPILEFLAGS += -mcmodel=kernel
ARCH_COMPILEFLAGS += -mno-red-zone

# set switches to generate/not generate fpu code
ARCH_COMPILEFLAGS_FLOAT +=
# hard disable floating point in the kernel
ARCH_COMPILEFLAGS_NOFLOAT += -msoft-float -mno-mmx -mno-sse -mno-sse2 -mno-3dnow -mno-avx -mno-avx2

# select default optimizations for different target cpu levels
ifeq ($(call TOBOOL,$(USE_ZIG_CC)),true)
ARCH_COMPILEFLAGS += -march=x86_64 -mcx16
else
ARCH_COMPILEFLAGS += -march=x86-64 -mcx16
endif
ARCH_OPTFLAGS :=
GLOBAL_DEFINES += X86_LEGACY=0

LINKER_SCRIPT += $(BUILDDIR)/kernel.ld

# potentially generated files that should be cleaned out with clean make rule
GENERATED += $(BUILDDIR)/kernel.ld

# rules for generating the linker scripts
$(BUILDDIR)/kernel.ld: $(LOCAL_DIR)/kernel.ld $(wildcard arch/*.ld)
	@echo generating $@
	@$(MKDIR)
	$(NOECHO)sed "s/%MEMBASE%/$(MEMBASE)/;s/%MEMSIZE%/$(MEMSIZE)/;s/%KERNEL_BASE%/$(KERNEL_BASE)/;s/%KERNEL_LOAD_OFFSET%/$(KERNEL_LOAD_OFFSET)/" < $< > $@.tmp
	@$(call TESTANDREPLACEFILE,$@.tmp,$@)

include make/module.mk
