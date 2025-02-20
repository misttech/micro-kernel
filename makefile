# Copyright 2024 Mist Tecnologia LTDA. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

HOST_PLATFORM := $(shell uname -s | tr '[:upper:]' '[:lower:]')
HOST_ARCH := $(shell uname -m)

ifeq ($(HOST_ARCH),x86_64)
HOST_ARCH = x64
else ifeq ($(HOST_ARCH),aarch64)
HOST_ARCH = arm64
else
$(error Unsupported host architecture: $(HOST_ARCH))
endif

ifeq ($(HOST_PLATFORM),linux)
HOST_OS = linux
else ifeq ($(HOST_PLATFORM),darwin)
HOST_OS = mac
else
$(error Unsupported host platform: $(HOST_PLATFORM))
endif

# Build variables
MKROOT ?= $(PWD)
OUTPUT ?= build-$(HOST_ARCH)
NOECHO ?= @
GN ?= $(MKROOT)/prebuilt/third_party/gn/$(HOST_OS)-$(HOST_ARCH)/gn
NINJA ?= $(MKROOT)/prebuilt/third_party/ninja/$(HOST_OS)-$(HOST_ARCH)/ninja

# By default, also show the number of actively running actions.
export NINJA_STATUS="[%f/%t][%p/%w](%r) "
# By default, print the 4 oldest commands that are still running.
export NINJA_STATUS_MAX_COMMANDS=4
export NINJA_STATUS_REFRESH_MILLIS=100
export NINJA_PERSISTENT_MODE=0

gen: ## Generate ninja
	$(NOECHO)echo "Running:$(GN) gen $(OUTPUT)"
	$(NOECHO)$(GN) gen $(OUTPUT)
.PHONY: gen

spotless:
	rm -rf -- "$(MKROOT)/$(OUTPUT)"

%: ## Make any ninja target
	$(NOECHO)$(NINJA) -C $(OUTPUT) $@