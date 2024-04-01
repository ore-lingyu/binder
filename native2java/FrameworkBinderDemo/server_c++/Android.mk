LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= server_binder.cpp

LOCAL_SHARED_LIBRARIES := liblog \
    libcutils \
    libbinder \
    libutils \
    libhardware

LOCAL_MODULE:= binder_for_java

include $(BUILD_EXECUTABLE)
