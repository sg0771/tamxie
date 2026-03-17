LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE :=wxrtmp
LOCAL_ARM_MODE := arm

LOCAL_C_INCLUDES +=	$(LOCAL_PATH)
LOCAL_C_INCLUDES +=	$(LOCAL_PATH)/../x264
LOCAL_C_INCLUDES +=	$(LOCAL_PATH)/../faac
LOCAL_C_INCLUDES +=	$(LOCAL_PATH)/../libyuv/include
LOCAL_C_INCLUDES +=	$(LOCAL_PATH)/../librtmp

LOCAL_CFLAGS :=-DPIC=1 -fPIC   -D_FILE_OFFSET_BITS=64  
LOCAL_CPP_FEATURES += exceptions
LOCAL_SRC_FILES:=RtmpSender.cpp 


LOCAL_LDLIBS += -llog -ldl -lz -lm -pthread
ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
LOCAL_LDLIBS += -Wl,--no-warn-shared-textrel
endif
LOCAL_STATIC_LIBRARIES +=faac x264  libyuv rtmp

include $(BUILD_SHARED_LIBRARY)
