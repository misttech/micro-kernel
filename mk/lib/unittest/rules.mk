LOCAL_DIR := $(GET_LOCAL_DIR)

MODULE := $(LOCAL_DIR)

MODULE_SRCS := \
	$(LOCAL_DIR)/console.cc \
	$(LOCAL_DIR)/phys.cc \
  $(LOCAL_DIR)/unittest.cc \
  $(LOCAL_DIR)/user_memory_tests.cc \
  $(LOCAL_DIR)/user_memory.cc \

# MODULE_DEPS += lib/fit

include make/module.mk
