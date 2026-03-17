LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

#LOCAL_ARM_MODE := arm
LOCAL_MODULE  :=  PCCastRecv

LOCAL_SRC_FILES :=TSDemux.cpp  PCCastRecv.cpp

LOCAL_LDLIBS     += -llog 

include $(BUILD_SHARED_LIBRARY)