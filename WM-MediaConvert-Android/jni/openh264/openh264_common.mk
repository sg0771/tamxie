LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_ARM_MODE := arm

LOCAL_C_INCLUDES +=$(LOCAL_PATH)/codec/api/svc/
LOCAL_C_INCLUDES +=$(LOCAL_PATH)/codec/common/
LOCAL_C_INCLUDES +=$(LOCAL_PATH)/codec/common/inc/


LOCAL_ASMFLAGS :=-DPIC=1  -fPIC  
LOCAL_CFLAGS :=-DPIC=1  -fomit-frame-pointer -fPIC -D_ISOC99_SOURCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -O3 -fno-math-errno -fno-signed-zeros -fno-tree-vectorize  -D__ANDROID=1 -Wno-deprecated-declarations  -fpermissive 



LOCAL_MODULE := openh264_common

ifeq ($(TARGET_ARCH_ABI), armeabi-v7a)
CFLAGS += -DHAVE_NEON
LOCAL_SRC_FILES+=  codec/common/arm/arm_arch_common_macro.S \
codec/common/arm/copy_mb_neon.S \
codec/common/arm/deblocking_neon.S \
codec/common/arm/expand_picture_neon.S \
codec/common/arm/intra_pred_common_neon.S \
codec/common/arm/mc_neon.S 
endif


ifeq ($(TARGET_ARCH_ABI), arm64)
CFLAGS += -DHAVE_NEON_AARCH64
LOCAL_SRC_FILES+=  codec/common/arm64/arm_arch64_common_macro.S \
 codec/common/arm64/copy_mb_aarch64_neon.S \
 codec/common/arm64/deblocking_aarch64_neon.S \
 codec/common/arm64/expand_picture_aarch64_neon.S \
 codec/common/arm64/intra_pred_common_aarch64_neon.S \
 codec/common/arm64/mc_aarch64_neon.S 
endif


LOCAL_SRC_FILES+=  codec/common/src/common_tables.cpp \
codec/common/src/copy_mb.cpp \
codec/common/src/cpu.cpp \
codec/common/src/crt_util_safe_x.cpp \
codec/common/src/deblocking_common.cpp \
codec/common/src/expand_pic.cpp \
codec/common/src/intra_pred_common.cpp \
codec/common/src/mc.cpp \
codec/common/src/memory_align.cpp \
codec/common/src/sad_common.cpp \
codec/common/src/utils.cpp \
codec/common/src/welsCodecTrace.cpp \
codec/common/src/WelsTaskThread.cpp \
codec/common/src/WelsThread.cpp \
codec/common/src/WelsThreadLib.cpp \
codec/common/src/WelsThreadPool.cpp \


include $(BUILD_STATIC_LIBRARY)