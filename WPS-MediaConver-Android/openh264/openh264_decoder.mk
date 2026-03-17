LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_ARM_MODE := arm

LOCAL_C_INCLUDES +=$(LOCAL_PATH)/codec/api/svc/
LOCAL_C_INCLUDES +=$(LOCAL_PATH)/codec/common/
LOCAL_C_INCLUDES +=$(LOCAL_PATH)/codec/common/inc/
LOCAL_C_INCLUDES +=$(LOCAL_PATH)/codec/processing/interface/
LOCAL_C_INCLUDES +=$(LOCAL_PATH)/codec/decoder/core/inc/
LOCAL_C_INCLUDES +=$(LOCAL_PATH)/codec/decoder/plus/inc


LOCAL_ASMFLAGS :=-DPIC=1  -fPIC  
LOCAL_CFLAGS :=-DPIC=1  -fomit-frame-pointer -fPIC -D_ISOC99_SOURCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -O3 -fno-math-errno -fno-signed-zeros -fno-tree-vectorize  -D__ANDROID=1 -Wno-deprecated-declarations  -fpermissive 


LOCAL_MODULE := openh264_decoder

ifeq ($(TARGET_ARCH_ABI), armeabi-v7a)
CFLAGS += -DHAVE_NEON
LOCAL_SRC_FILES+= codec/decoder/core/arm/block_add_neon.S\
codec/decoder/core/arm/intra_pred_neon.S\
codec/encoder/core/arm/intra_pred_neon.S
endif


ifeq ($(TARGET_ARCH_ABI), arm64)
CFLAGS += -DHAVE_NEON_AARCH64
LOCAL_SRC_FILES+=  codec/decoder/core/arm64/block_add_aarch64_neon.S\
codec/decoder/core/arm64/intra_pred_aarch64_neon.S
endif


LOCAL_SRC_FILES+=codec/decoder/core/src/au_parser.cpp\
codec/decoder/core/src/bit_stream.cpp\
codec/decoder/core/src/cabac_decoder.cpp\
codec/decoder/core/src/deblocking.cpp\
codec/decoder/core/src/decode_mb_aux.cpp\
codec/decoder/core/src/decode_slice.cpp\
codec/decoder/core/src/decoder.cpp\
codec/decoder/core/src/decoder_core.cpp\
codec/decoder/core/src/decoder_data_tables.cpp\
codec/decoder/core/src/error_concealment.cpp\
codec/decoder/core/src/fmo.cpp\
codec/decoder/core/src/get_intra_predictor.cpp\
codec/decoder/core/src/manage_dec_ref.cpp\
codec/decoder/core/src/memmgr_nal_unit.cpp\
codec/decoder/core/src/mv_pred.cpp\
codec/decoder/core/src/parse_mb_syn_cabac.cpp\
codec/decoder/core/src/parse_mb_syn_cavlc.cpp\
codec/decoder/core/src/pic_queue.cpp\
codec/decoder/core/src/rec_mb.cpp


include $(BUILD_STATIC_LIBRARY)