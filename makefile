# Copyright 2025 Mist Tecnologia Ltda
# Copyright 2016 The Fuchsia Authors
# Copyright (c) 2008-2015 Travis Geiselbrecht
#
# Use of this source code is governed by a MIT-style
# license that can be found in the LICENSE file or at
# https://opensource.org/licenses/MIT

LKMAKEROOT ?= .
LKROOT ?= .
LKINC ?=
BUILDROOT ?= .
DEFAULT_PROJECT ?= x64
TOOLCHAIN_PREFIX ?=

# check if LKROOT is already a part of LKINC list and add it only if it is not
ifeq ($(filter $(LKROOT),$(LKINC)), )
LKINC := $(LKROOT) $(LKINC)
endif

# add the external path to LKINC
ifneq ($(LKROOT),.)
LKINC += $(LKROOT)/external
else
LKINC += external
endif

export LKMAKEROOT
export LKROOT
export LKINC
export BUILDROOT
export DEFAULT_PROJECT
export TOOLCHAIN_PREFIX

# veneer makefile that calls into the engine with lk as the build root
# if we're the top level invocation, call ourselves with additional args
.PHONY: _top $(MAKECMDGOALS)
$(MAKECMDGOALS) _top:
	@$(MAKE) -C $(LKMAKEROOT) --no-print-directory -rR -f make/engine.mk $(addprefix -I,$(LKINC)) $(MAKECMDGOALS)
