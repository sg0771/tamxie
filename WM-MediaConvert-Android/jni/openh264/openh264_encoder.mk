LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_ARM_MODE := arm

LOCAL_C_INCLUDES +=$(LOCAL_PATH)/codec/api/svc/
LOCAL_C_INCLUDES +=$(LOCAL_PATH)/codec/common/
LOCAL_C_INCLUDES +=$(LOCAL_PATH)/codec/common/inc/
LOCAL_C_INCLUDES +=$(LOCAL_PATH)/codec/processing/interface/
LOCAL_C_INCLUDES +=$(LOCAL_PATH)/codec/encoder/core/inc/
LOCAL_C_INCLUDES +=$(LOCAL_PATH)/codec/encoder/plus/inc/


LOCAL_ASMFLAGS :=-DPIC=1  -fPIC  
LOCAL_CFLAGS :=-DPIC=1  -fomit-frame-pointer -fPIC -D_ISOC99_SOURCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -O3 -fno-math-errno -fno-signed-zeros -fno-tree-vectorize  -D__ANDROID=1 -Wno-deprecated-declarations  -fpermissive 



LOCAL_MODULE := openh264_encoder

ifeq ($(TARGET_ARCH_ABI), armeabi-v7a)
CFLAGS += -DHAVE_NEON
LOCAL_SRC_FILES+=  codec/encoder/core/arm/intra_pred_neon.S\
codec/encoder/core/arm/intra_pred_sad_3_opt_neon.S\
codec/encoder/core/arm/memory_neon.S\
codec/encoder/core/arm/pixel_neon.S\
codec/encoder/core/arm/reconstruct_neon.S\
codec/encoder/core/arm/svc_motion_estimation.S
endif


ifeq ($(TARGET_ARCH_ABI), arm64)
CFLAGS += -DHAVE_NEON_AARCH64
LOCAL_SRC_FILES+= codec/encoder/core/arm64/intra_pred_aarch64_neon.S\
codec/encoder/core/arm64/intra_pred_sad_3_opt_aarch64_neon.S\
codec/encoder/core/arm64/memory_aarch64_neon.S\
codec/encoder/core/arm64/pixel_aarch64_neon.S\
codec/encoder/core/arm64/reconstruct_aarch64_neon.S\
codec/encoder/core/arm64/svc_motion_estimation_aarch64_neon.S
endif


LOCAL_SRC_FILES+=  codec/encoder/core/src/au_set.cpp\
codec/encoder/core/src/encode_mb_aux.cpp\
codec/encoder/core/src/encoder_data_tables.cpp\
codec/encoder/core/src/encoder_ext.cpp\
codec/encoder/core/src/get_intra_predictor.cpp\
codec/encoder/core/src/md.cpp\
codec/encoder/core/src/mv_pred.cpp\
codec/encoder/core/src/nal_encap.cpp\
codec/encoder/core/src/paraset_strategy.cpp\
codec/encoder/core/src/picture_handle.cpp\
codec/encoder/core/src/ratectl.cpp\
codec/encoder/core/src/ref_list_mgr_svc.cpp\
codec/encoder/core/src/sample.cpp\
codec/encoder/core/src/set_mb_syn_cabac.cpp\
codec/encoder/core/src/set_mb_syn_cavlc.cpp\
codec/encoder/core/src/slice_multi_threading.cpp\
codec/encoder/core/src/svc_base_layer_md.cpp\
codec/encoder/core/src/svc_enc_slice_segment.cpp\
codec/encoder/core/src/svc_encode_mb.cpp\
codec/encoder/core/src/svc_encode_slice.cpp\
codec/encoder/core/src/svc_mode_decision.cpp\
codec/encoder/core/src/svc_motion_estimate.cpp\
codec/encoder/core/src/svc_set_mb_syn_cabac.cpp\
codec/encoder/core/src/svc_set_mb_syn_cavlc.cpp\
codec/encoder/core/src/wels_preprocess.cpp\
codec/encoder/core/src/wels_task_base.cpp\
codec/encoder/core/src/wels_task_encoder.cpp\
codec/encoder/core/src/wels_task_management.cpp\
codec/encoder/plus/src/DllEntry.cpp\
codec/encoder/plus/src/welsEncoderExt.cpp\
codec/encoder/core/src/deblocking.cpp\
codec/encoder/core/src/decode_mb_aux.cpp\
codec/encoder/core/src/encoder.cpp




include $(BUILD_STATIC_LIBRARY)