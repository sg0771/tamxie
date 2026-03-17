LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)


LOCAL_ARM_MODE := arm

LOCAL_SRC_FILES+=     src/dsp/alpha_processing_neon.c \
    src/dsp/alpha_processing.c \
    src/dsp/alpha_processing_mips_dsp_r2.c \
    src/dsp/alpha_processing_sse2.c \
    src/dsp/alpha_processing_sse41.c \
    src/dsp/cpu.c \
    src/dsp/dec.c \
    src/dsp/dec_clip_tables.c \
    src/dsp/dec_mips32.c \
    src/dsp/dec_mips_dsp_r2.c \
    src/dsp/dec_msa.c \
    src/dsp/dec_neon.c \
    src/dsp/dec_sse2.c \
    src/dsp/dec_sse41.c \
    src/dsp/filters.c \
    src/dsp/filters_mips_dsp_r2.c \
    src/dsp/filters_msa.c \
    src/dsp/filters_neon.c \
    src/dsp/filters_sse2.c \
    src/dsp/lossless.c \
    src/dsp/lossless_mips_dsp_r2.c \
    src/dsp/lossless_msa.c \
    src/dsp/lossless_neon.c \
    src/dsp/lossless_sse2.c \
    src/dsp/rescaler.c \
    src/dsp/rescaler_mips32.c \
    src/dsp/rescaler_mips_dsp_r2.c \
    src/dsp/rescaler_msa.c \
    src/dsp/rescaler_neon.c \
    src/dsp/rescaler_sse2.c \
    src/dsp/upsampling.c \
    src/dsp/upsampling_mips_dsp_r2.c \
    src/dsp/upsampling_msa.c \
    src/dsp/upsampling_neon.c \
    src/dsp/upsampling_sse2.c \
    src/dsp/yuv.c \
    src/dsp/yuv_mips32.c \
    src/dsp/yuv_mips_dsp_r2.c \
    src/dsp/yuv_neon.c \
    src/dsp/yuv_sse2.c \


LOCAL_SRC_FILES+= \
    src/dec/alpha_dec.c \
    src/dec/buffer_dec.c \
    src/dec/frame_dec.c \
    src/dec/idec_dec.c \
    src/dec/io_dec.c \
    src/dec/quant_dec.c \
    src/dec/tree_dec.c \
    src/dec/vp8_dec.c \
    src/dec/vp8l_dec.c \
    src/dec/webp_dec.c \

LOCAL_SRC_FILES+=  \
    src/demux/anim_decode.c \
    src/demux/demux.c \

LOCAL_SRC_FILES+= \
    src/dsp/cost.c \
    src/dsp/cost_mips32.c \
    src/dsp/cost_mips_dsp_r2.c \
    src/dsp/cost_sse2.c \
    src/dsp/enc.c \
    src/dsp/enc_avx2.c \
    src/dsp/enc_mips32.c \
    src/dsp/enc_mips_dsp_r2.c \
    src/dsp/enc_msa.c \
    src/dsp/enc_neon.c \
    src/dsp/enc_sse2.c \
    src/dsp/enc_sse41.c \
    src/dsp/lossless_enc.c \
    src/dsp/lossless_enc_mips32.c \
    src/dsp/lossless_enc_mips_dsp_r2.c \
    src/dsp/lossless_enc_msa.c \
    src/dsp/lossless_enc_neon.c \
    src/dsp/lossless_enc_sse2.c \
    src/dsp/lossless_enc_sse41.c \
    src/dsp/ssim.c \
    src/dsp/ssim_sse2.c \

LOCAL_SRC_FILES+= \
    src/enc/alpha_enc.c \
    src/enc/analysis_enc.c \
    src/enc/backward_references_cost_enc.c \
    src/enc/backward_references_enc.c \
    src/enc/config_enc.c \
    src/enc/cost_enc.c \
    src/enc/delta_palettization_enc.c \
    src/enc/filter_enc.c \
    src/enc/frame_enc.c \
    src/enc/histogram_enc.c \
    src/enc/iterator_enc.c \
    src/enc/near_lossless_enc.c \
    src/enc/picture_enc.c \
    src/enc/picture_csp_enc.c \
    src/enc/picture_psnr_enc.c \
    src/enc/picture_rescale_enc.c \
    src/enc/picture_tools_enc.c \
    src/enc/predictor_enc.c \
    src/enc/quant_enc.c \
    src/enc/syntax_enc.c \
    src/enc/token_enc.c \
    src/enc/tree_enc.c \
    src/enc/vp8l_enc.c \
    src/enc/webp_enc.c \

LOCAL_SRC_FILES+=  \
    src/mux/anim_encode.c \
    src/mux/muxedit.c \
    src/mux/muxinternal.c \
    src/mux/muxread.c \

LOCAL_SRC_FILES+= \
    src/utils/bit_reader_utils.c \
    src/utils/color_cache_utils.c \
    src/utils/filters_utils.c \
    src/utils/huffman_utils.c \
    src/utils/quant_levels_dec_utils.c \
    src/utils/random_utils.c \
    src/utils/rescaler_utils.c \
    src/utils/thread_utils.c \
    src/utils/utils.c \

LOCAL_SRC_FILES+=  \
    src/utils/bit_writer_utils.c \
    src/utils/huffman_encode_utils.c \
    src/utils/quant_levels_utils.c \


LOCAL_CFLAGS := -Wall -DANDROID -DHAVE_MALLOC_H -DHAVE_PTHREAD -DWEBP_USE_THREAD 
LOCAL_CFLAGS += -fvisibility=hidden -DWEBP_SWAP_16BIT_CSP
LOCAL_CFLAGS += -finline-functions -ffast-math  -ffunction-sections -fdata-sections
LOCAL_CFLAGS += -frename-registers -s
LOCAL_CFLAGS += -DHAVE_CPU_FEATURES_H 


ifeq ($(TARGET_ARCH_ABI), armeabi-v7a)

LOCAL_ASMFLAGS+=-march=armv7-a -mfloat-abi=softfp -mfpu=neon
LOCAL_CFLAGS     +=-march=armv7-a  -mfloat-abi=softfp -mfpu=neon

else

LOCAL_ASMFLAGS+=-DPIC=1 -march=armv8-a -fPIC  
LOCAL_CFLAGS +=-DPIC=1 -march=armv8-a

endif



LOCAL_C_INCLUDES += $(LOCAL_PATH)/src


LOCAL_STATIC_LIBRARIES := cpufeatures
LOCAL_MODULE := webp
include $(BUILD_STATIC_LIBRARY)

 $(call import-module,android/cpufeatures)