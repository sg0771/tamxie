LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE :=x264
LOCAL_ARM_MODE := arm
LOCAL_C_INCLUDES +=	$(LOCAL_PATH) 

LOCAL_ASFLAGS :=-DPIC=1 -fPIC -fpic  -DSTACK_ALIGNMENT=4 -DHIGH_BIT_DEPTH=0 -DBIT_DEPTH=8
LOCAL_CFLAGS :=-DPIC=1 -fPIC -fpic  -Wshadow -O2 -ffast-math  -Wall -std=gnu99 -D_GNU_SOURCE  -fomit-frame-pointer -fno-tree-vectorize

ifeq ($(TARGET_ARCH_ABI), armeabi-v7a)
LOCAL_ASMFLAGS+=-march=armv7-a -mfloat-abi=softfp -mfpu=neon
LOCAL_CFLAGS+=-march=armv7-a  -mfloat-abi=softfp -mfpu=neon

LOCAL_SRC_FILES+=common/arm/bitstream-a.S \
common/arm/cpu-a.S \
common/arm/dct-a.S \
common/arm/deblock-a.S \
common/arm/mc-a.S \
common/arm/mc-c.c \
common/arm/predict-a.S \
common/arm/pixel-a.S \
common/arm/quant-a.S \
common/arm/predict-c.c 

else

LOCAL_SRC_FILES+=common/aarch64/bitstream-a.S \
common/aarch64/cabac-a.S \
common/aarch64/dct-a.S \
common/aarch64/deblock-a.S \
common/aarch64/mc-a.S \
common/aarch64/pixel-a.S \
common/aarch64/predict-a.S \
common/aarch64/quant-a.S \
common/aarch64/predict-c.c \
common/aarch64/mc-c.c  \
common/aarch64/asm-offsets.c

endif

LOCAL_SRC_FILES+=common/bitstream.c \
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
common/threadpool.c \
common/vlc.c \
encoder/analyse.c \
encoder/cabac.c \
encoder/cavlc.c \
encoder/encoder.c \
encoder/lookahead.c \
encoder/macroblock.c \
encoder/me.c \
encoder/ratecontrol.c \
encoder/set.c \
encoder/slicetype-cl.c \

include $(BUILD_STATIC_LIBRARY)