LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

#LOCAL_ARM_MODE := arm
LOCAL_MODULE  :=  WXTcp
LOCAL_STATIC_LIBRARIES += libopus
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../opus/include

LOCAL_SRC_FILES := TcpClient.cpp  TcpServer.cpp

LOCAL_LDLIBS     += -llog 

include $(BUILD_SHARED_LIBRARY)