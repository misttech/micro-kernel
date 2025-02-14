# Copyright 2025 Mist Tecnologia Ltda
# Copyright 2016 The Fuchsia Authors
# Copyright (c) 2008-2015 Travis Geiselbrecht
#
# Use of this source code is governed by a MIT-style
# license that can be found in the LICENSE file or at
# https://opensource.org/licenses/MIT

LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS += \
	$(LOCAL_DIR)/console.cc

MODULE_DEPS += lib/zircon-internal
MODULE_DEPS += lib/fbl_ulib

MODULE_DEFINES += BOOT_TEST_SUCCESS_STRING="\"***Boot-test-successful!-MDd7/O65SuVZ23yGAaQG4CedYQGH9E1/58r73pSAVK0=***\""

include make/module.mk
