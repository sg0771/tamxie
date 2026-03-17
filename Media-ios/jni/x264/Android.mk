LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

TARGET_ARCH_ABI := armeabi-v7a
TARGET_PLATFORM := android-14
LOCAL_ARCH      := arm
LOCAL_ARM_MODE  := arm
LOCAL_ARM_NEON  := true


LOCAL_MODULE    := x264
LOCAL_SRC_FILES :=  common/arm/cpu-a.S \
     common/arm/dct-a.S \
     common/arm/deblock-a.S \
     common/arm/pixel-a.S \
     common/arm/predict-a.S \
     common/arm/predict-c.c \
     common/arm/mc-c.c \
     common/arm/mc-a.S \
     common/arm/quant-a.S \
     common/bitstream.c \
     common/cabac.c \
     common/common.c \
     common/cpu.c \
     common/dct.c \
     common/deblock.c \
     common/frame.c \
     common/macroblock.c \
     common/mc.c \
     common/mvpred.c \
     common/osdep.c \
     common/pixel.c \
     common/predict.c \
     common/quant.c \
     common/rectangle.c \
     common/set.c \
     common/vlc.c \
     encoder/analyse.c \
     encoder/cabac.c \
     encoder/cavlc.c \
     encoder/encoder.c \
     encoder/lookahead.c \
     encoder/macroblock.c \
     encoder/me.c \
     encoder/ratecontrol.c \
     encoder/set.c 
LOCAL_CFLAGS := -std=gnu99 -fno-short-enums -fstrict-aliasing -O2 -fpic  -fPIC
include $(BUILD_STATIC_LIBRARY)
