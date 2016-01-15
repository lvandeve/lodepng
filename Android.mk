#                              Example usage
#
# Directory structure:
#   sample_project/
#       jni/
#           Android.mk          <- your Android.mk
#           my_sample.cpp          and sources
#       third_party/
#           lodepng/            <- cloned repository
#       AndroidManifest.xml
#       ...
#
# Contents of sample_project/jni/Android.mk:
#
#   LOCAL_PATH := $(call my-dir)
#   include $(CLEAR_VARS)
#   
#   LOCAL_MODULE     := my_sample
#   LOCAL_LDLIBS     := -landroid -llog -lGLESv2
#   LOCAL_STATIC_LIBRARIES := lodepng
#   LOCAL_SRC_FILES := my_sample.cpp
#   
#   include $(BUILD_SHARED_LIBRARY)
#   
#   
#   $(call import-add-path, ../../third_party)
#   $(call import-module, lodepng)


LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := lodepng
LOCAL_SRC_FILES := lodepng.cpp
LOCAL_C_INCLUDES := $(LOCAL_PATH)
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)

include $(BUILD_STATIC_LIBRARY)
