LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_ASMFLAGS := -DPIC=1  -fPIC  
LOCAL_CFLAGS := -DPIC=1  -fomit-frame-pointer -fPIC -D_ISOC99_SOURCE -D_FILE_OFFSET_BITS=64 
LOCAL_CFLAGS += -D_LARGEFILE_SOURCE -O3 -fno-math-errno -fno-signed-zeros -fno-tree-vectorize  -D__ANDROID=1 -Wno-deprecated-declarations  -fpermissive 

ifeq ($(TARGET_ARCH_ABI), arm64-v8a)
	LOCAL_ASMFLAGS+= -DFFMPEG_ARM64
	LOCAL_CFLAGS +=  -DFFMPEG_ARM64
	LOCAL_SRC_FILES += $(ARM64_FILES)
else  ifeq ($(TARGET_ARCH_ABI), armeabi-v7a)
	LOCAL_LDLIBS += -Wl,--no-warn-shared-textrel
	LOCAL_CFLAGS += -march=armv7-a -mcpu=cortex-a8 -mfpu=vfpv3-d16 -mfloat-abi=softfp -mthumb -mcpu=cortex-a8 -fPIC
	LOCAL_ASMFLAGS   += -DFFMPEG_ARMV7
	LOCAL_CFLAGS     += -DFFMPEG_ARMV7
	LOCAL_SRC_FILES += $(ARMV7_FILES) 	
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

LOCAL_MODULE := MediaConvert

LOCAL_C_INCLUDES :=$(ffmpeg-root-dir)/ffmpeg
LOCAL_C_INCLUDES +=$(ffmpeg-root-dir)/ijkyuv/include

LOCAL_STATIC_LIBRARIES += libyuv_static
LOCAL_SRC_FILES  :=  FFmpegConvert.cpp   MediaConvert.cpp WXFfmpegImpl.c FFmpegDelogoConvert.cpp  WaterMarkRemoveAPI.cpp WXMediaConvert.cpp LibDelogo.cpp    WXFfmpeg.cpp   WXMediaInfo.cpp

LOCAL_SHARED_LIBRARIES := wxffmpeg
LOCAL_LDLIBS := -llog -ldl -lz -lm -pthread -ljnigraphics    -landroid
ifeq ($(TARGET_ARCH_ABI), armeabi-v7a)
LOCAL_LDLIBS += -Wl,--no-warn-shared-textrel
endif

include $(BUILD_SHARED_LIBRARY)