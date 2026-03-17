LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

TARGET_ARCH_ABI := armeabi-v7a
TARGET_PLATFORM := android-14
LOCAL_ARCH      := arm
LOCAL_ARM_MODE  := arm
LOCAL_ARM_NEON  := true

LOCAL_C_INCLUDES :=  $(LOCAL_PATH)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../oRTP/include 
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../mediastreamer2/include 
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../x264
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../ffmpeg
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../libyuv/include 
	
LOCAL_SRC_FILES := MediaStream_jni.c  AudioStream.c
LOCAL_SRC_FILES += H264Decoder.c  yuv2rgb.neon.S
LOCAL_SRC_FILES += H264Encoder.c
LOCAL_SRC_FILES += ImageConvert.c
LOCAL_SRC_FILES += Draw.c Draw2.c DrawScale.c

LOCAL_MODULE := libms2

LOCAL_STATIC_LIBRARIES := cpufeatures 
LOCAL_STATIC_LIBRARIES += libmediastreamer2
LOCAL_STATIC_LIBRARIES += libortp
LOCAL_STATIC_LIBRARIES += libopus
LOCAL_STATIC_LIBRARIES += libffmpeg 
LOCAL_STATIC_LIBRARIES +=libx264 
LOCAL_STATIC_LIBRARIES +=libyuv
LOCAL_STATIC_LIBRARIES +=webrtc
LOCAL_STATIC_LIBRARIES +=openal
LOCAL_CFLAGS+= -std=c99
LOCAL_LDLIBS += -llog -ldl -lz -ljnigraphics    -landroid
include $(BUILD_SHARED_LIBRARY)
$(call import-module,android/cpufeatures)