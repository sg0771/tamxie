LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_ARM_MODE := arm

LOCAL_C_INCLUDES += $(LOCAL_PATH)
LOCAL_C_INCLUDES += $(realpath $(LOCAL_PATH)/..)
LOCAL_C_INCLUDES += $(realpath $(LOCAL_PATH)/../ffmpeg)
LOCAL_C_INCLUDES += $(realpath $(LOCAL_PATH)/../ijkyuv/include)

LOCAL_ASMFLAGS :=-DPIC=1  -fPIC  
LOCAL_CFLAGS :=-DPIC=1 -std=c99 -fomit-frame-pointer -fPIC -D_ISOC99_SOURCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -O3 -fno-math-errno -fno-signed-zeros -fno-tree-vectorize  -D__ANDROID=1


ifeq ($(TARGET_ARCH_ABI), arm64-v8a)
	LOCAL_ASMFLAGS+= -DFFMPEG_ARM64
	LOCAL_CFLAGS +=  -DFFMPEG_ARM64
else  ifeq ($(TARGET_ARCH_ABI), armeabi-v7a)
	LOCAL_ASMFLAGS   += -DFFMPEG_ARMV7
	LOCAL_CFLAGS     += -DFFMPEG_ARMV7
else  ifeq ($(TARGET_ARCH_ABI), armeabi)
	LOCAL_ASMFLAGS   += -DFFMPEG_ARMV5
	LOCAL_CFLAGS     +=  -DFFMPEG_ARMV5
else  ifeq ($(TARGET_ARCH_ABI), x86_64)
	LOCAL_ASMFLAGS   +=  -DFFMPEG_X64  
	LOCAL_CFLAGS     +=  -DFFMPEG_X64 
else  ifeq ($(TARGET_ARCH_ABI), x86)
	LOCAL_ASMFLAGS+= -DFFMPEG_X86
	LOCAL_CFLAGS +=  -DFFMPEG_X86
endif

LOCAL_LDLIBS += -llog -ldl -lz -lm -pthread -ljnigraphics    -landroid

ifeq ($(TARGET_ARCH_ABI), armeabi-v7a)
LOCAL_LDLIBS += -Wl,--no-warn-shared-textrel
endif

LOCAL_SRC_FILES=  H264Decoder.c

LOCAL_SHARED_LIBRARIES := wxffmpeg
LOCAL_STATIC_LIBRARIES := cpufeatures yuv_static

LOCAL_MODULE := h264decoder
include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/cpufeatures)