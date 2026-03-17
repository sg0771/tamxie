
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)


LOCAL_C_INCLUDES += $(LOCAL_PATH)
LOCAL_C_INCLUDES += $(realpath $(LOCAL_PATH)/..)
LOCAL_C_INCLUDES += $(realpath $(LOCAL_PATH)/include/)

LOCAL_SRC_FILES += source/SoundTouch/AAFilter.cpp
LOCAL_SRC_FILES += source/SoundTouch/FIFOSampleBuffer.cpp
LOCAL_SRC_FILES += source/SoundTouch/FIRFilter.cpp
LOCAL_SRC_FILES += source/SoundTouch/cpu_detect_x86.cpp
LOCAL_SRC_FILES += source/SoundTouch/sse_optimized.cpp
LOCAL_SRC_FILES += source/SoundTouch/RateTransposer.cpp
LOCAL_SRC_FILES += source/SoundTouch/InterpolateCubic.cpp
LOCAL_SRC_FILES += source/SoundTouch/InterpolateLinear.cpp
LOCAL_SRC_FILES += source/SoundTouch/InterpolateShannon.cpp
LOCAL_SRC_FILES += source/SoundTouch/TDStretch.cpp
LOCAL_SRC_FILES += source/SoundTouch/BPMDetect.cpp
LOCAL_SRC_FILES += source/SoundTouch/PeakFinder.cpp
LOCAL_SRC_FILES += source/SoundTouch/SoundTouch.cpp
LOCAL_SRC_FILES += source/SoundTouch/mmx_optimized.cpp
LOCAL_SRC_FILES += ijksoundtouch_wrap.cpp

LOCAL_MODULE := ijksoundtouch
include $(BUILD_STATIC_LIBRARY)

$(call import-module,android/cpufeatures)
