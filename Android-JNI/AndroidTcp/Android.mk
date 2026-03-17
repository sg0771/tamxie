LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_ARM_MODE := arm



# -march=armv7-a -mfpu=neon -mfloat-abi=softfp

LOCAL_C_INCLUDES +=$(ffmpeg-root-dir)
LOCAL_ASMFLAGS :=-DPIC=1 -fPIC  
LOCAL_CFLAGS :=-DPIC=1 -std=c99 -fomit-frame-pointer -fPIC -marm -D_ISOC99_SOURCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -O3 -fno-math-errno -fno-signed-zeros -fno-tree-vectorize  -D__ANDROID=1

LOCAL_LDLIBS += -llog -ldl -lz -lm -pthread
LOCAL_LDLIBS += -Wl,--no-warn-shared-textrel

LOCAL_MODULE := H264Decoder
LOCAL_SRC_FILES += H264Decoder.c

LOCAL_STATIC_LIBRARIES +=wxffmpeg_static 

include $(BUILD_SHARED_LIBRARY)