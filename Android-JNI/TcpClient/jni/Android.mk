LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

#LOCAL_ARM_MODE := arm
LOCAL_MODULE := TcpClient

LOCAL_SRC_FILES := TcpClient.cpp

LOCAL_LDLIBS += -llog 

include $(BUILD_SHARED_LIBRARY)