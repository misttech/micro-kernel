LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_INCLUDES += $(LOCAL_DIR)/local/include

MODULE_DEFINES=MSPACES=1

MODULE_SRCS += \
	$(LOCAL_DIR)/uefi.cc \
	$(LOCAL_DIR)/text_protocol.cc \
	$(LOCAL_DIR)/boot_service_provider.cc \
	$(LOCAL_DIR)/runtime_service_provider.cc \
	$(LOCAL_DIR)/switch_stack.S \
	$(LOCAL_DIR)/configuration_table.cc \

include make/module.mk
