LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE :=libfaac
LOCAL_ARM_MODE := arm

LOCAL_CFLAGS :=-DPIC=1 -fPIC   -D_FILE_OFFSET_BITS=64   -DWEBRTC_POSIX=1

LOCAL_CPP_FEATURES += exceptions

LOCAL_SRC_FILES:= libfaac.cpp

LOCAL_LDLIBS += -llog -lm 

ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
LOCAL_LDLIBS += -Wl,--no-warn-shared-textrel
endif

include $(BUILD_SHARED_LIBRARY)
