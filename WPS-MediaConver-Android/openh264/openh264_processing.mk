LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_ARM_MODE := arm

LOCAL_C_INCLUDES +=$(LOCAL_PATH)/codec/api/svc/
LOCAL_C_INCLUDES +=$(LOCAL_PATH)/codec/common/
LOCAL_C_INCLUDES +=$(LOCAL_PATH)/codec/common/inc/
LOCAL_C_INCLUDES +=$(LOCAL_PATH)/codec/processing/interface/


LOCAL_ASMFLAGS :=-DPIC=1  -fPIC  
LOCAL_CFLAGS :=-DPIC=1  -fomit-frame-pointer -fPIC -D_ISOC99_SOURCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -O3 -fno-math-errno -fno-signed-zeros -fno-tree-vectorize  -D__ANDROID=1 -Wno-deprecated-declarations  -fpermissive 



LOCAL_MODULE := openh264_processing

ifeq ($(TARGET_ARCH_ABI), armeabi-v7a)
CFLAGS += -DHAVE_NEON
LOCAL_SRC_FILES+=  codec/processing/src/arm/adaptive_quantization.S\
codec/processing/src/arm/down_sample_neon.S\
codec/processing/src/arm/pixel_sad_neon.S\
codec/processing/src/arm/vaa_calc_neon.S 
endif


ifeq ($(TARGET_ARCH_ABI), arm64)
CFLAGS += -DHAVE_NEON_AARCH64
LOCAL_SRC_FILES+=  codec/processing/src/arm64/adaptive_quantization_aarch64_neon.S\
codec/processing/src/arm64/down_sample_aarch64_neon.S\
codec/processing/src/arm64/pixel_sad_aarch64_neon.S\
codec/processing/src/arm64/vaa_calc_aarch64_neon.S 
endif


LOCAL_SRC_FILES+= codec/processing/src/adaptivequantization/AdaptiveQuantization.cpp\
codec/processing/src/backgrounddetection/BackgroundDetection.cpp\
codec/processing/src/common/memory.cpp\
codec/processing/src/common/WelsFrameWork.cpp\
codec/processing/src/common/WelsFrameWorkEx.cpp\
codec/processing/src/complexityanalysis/ComplexityAnalysis.cpp\
codec/processing/src/denoise/denoise.cpp\
codec/processing/src/denoise/denoise_filter.cpp\
codec/processing/src/downsample/downsample.cpp\
codec/processing/src/downsample/downsamplefuncs.cpp\
codec/processing/src/imagerotate/imagerotate.cpp\
codec/processing/src/imagerotate/imagerotatefuncs.cpp\
codec/processing/src/scenechangedetection/SceneChangeDetection.cpp\
codec/processing/src/scrolldetection/ScrollDetection.cpp\
codec/processing/src/scrolldetection/ScrollDetectionFuncs.cpp\
codec/processing/src/vaacalc/vaacalcfuncs.cpp\
codec/processing/src/vaacalc/vaacalculation.cpp


include $(BUILD_STATIC_LIBRARY)