LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := soomla_core_internal
LOCAL_SRC_FILES := ${shell find $(LOCAL_PATH)/src -name "*.cpp" -print}
LOCAL_C_INCLUDES := $(LOCAL_PATH)/src
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/src
LOCAL_STATIC_LIBRARIES := ee_x jansson

include $(BUILD_STATIC_LIBRARY)