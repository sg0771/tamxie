LOCAL_PATH:= $(call my-dir)
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
# -mfloat-abi=soft is a workaround for FP register corruption on Exynos 4210
# http://www.spinics.net/lists/arm-kernel/msg368417.html
ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
LOCAL_CFLAGS += -mfloat-abi=soft
endif
#LOCAL_CFLAGS += -std=c99
LOCAL_LDLIBS += -llog -landroid

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

LOCAL_ASMFLAGS += -DPIC=1  -fPIC  
LOCAL_CFLAGS += -DPIC=1  -fomit-frame-pointer -fPIC -D_ISOC99_SOURCE -D_FILE_OFFSET_BITS=64 
LOCAL_CFLAGS += -D_LARGEFILE_SOURCE -O3 -fno-math-errno -fno-signed-zeros -fno-tree-vectorize  -D__ANDROID=1 -Wno-deprecated-declarations  -fpermissive 
LOCAL_C_INCLUDES += $(ffmpeg-root-dir)/ffmpeg
LOCAL_C_INCLUDES += $(ffmpeg-root-dir)/ijkyuv/include

ifeq ($(TARGET_ARCH_ABI), armeabi-v7a)
LOCAL_LDLIBS += -Wl,--no-warn-shared-textrel
endif

LOCAL_MODULE := MediaConvert

LOCAL_SRC_FILES +=  WXFfmpegImpl.c
LOCAL_SRC_FILES +=  FFmpegConvert.cpp
LOCAL_SRC_FILES +=  FFmpegDelogoConvert.cpp
LOCAL_SRC_FILES +=  LibDelogo.cpp
LOCAL_SRC_FILES +=  MediaConvert.cpp
LOCAL_SRC_FILES +=  WaterMarkRemoveAPI.cpp
LOCAL_SRC_FILES +=  WXFfmpeg.cpp
LOCAL_SRC_FILES +=  WXMediaConvert.cpp
LOCAL_SRC_FILES +=  WXMediaInfo.cpp


LOCAL_SHARED_LIBRARIES +=  wxffmpeg 

LOCAL_STATIC_LIBRARIES += yuv_static

LOCAL_LDLIBS += -llog -ldl -lz -lm -pthread -ljnigraphics    -landroid

include $(BUILD_SHARED_LIBRARY)