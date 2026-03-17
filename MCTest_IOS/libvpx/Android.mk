LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE :=vpx
LOCAL_ARM_MODE := arm
LOCAL_C_INCLUDES +=$(LOCAL_PATH)

LOCAL_ASMFLAGS :=-DPIC=1 -fPIC -fpic 
LOCAL_CFLAGS +=-DNDEBUG -O3 -fPIC -Wall -Wdeclaration-after-statement -Wdisabled-optimization -Wfloat-conversion -Wpointer-arith -Wtype-limits -Wcast-qual -Wvla -Wimplicit-function-declaration -Wuninitialized -Wunused -Wextra -Wundef 

LOCAL_SRC_FILES+=\
vp8/common/alloccommon.c \
vp8/common/blockd.c \
vp8/common/context.c \
vp8/common/copy_c.c \
vp8/common/debugmodes.c \
vp8/common/dequantize.c \
vp8/common/entropy.c \
vp8/common/entropymode.c \
vp8/common/entropymv.c \
vp8/common/extend.c \
vp8/common/filter.c \
vp8/common/findnearmv.c \
vp8/common/generic/systemdependent.c \
vp8/common/idct_blk.c \
vp8/common/idctllm.c \
vp8/common/loopfilter_filters.c \
vp8/common/mbpitch.c \
vp8/common/modecont.c \
vp8/common/postproc.c \
vp8/common/quant_common.c \
vp8/common/reconinter.c \
vp8/common/reconintra.c \
vp8/common/reconintra4x4.c \
vp8/common/rtcd.c \
vp8/common/setupintrarecon.c \
vp8/common/swapyv12buffer.c \
vp8/common/treecoder.c \
vp8/common/vp8_loopfilter.c \
vp8/decoder/dboolhuff.c \
vp8/decoder/decodeframe.c \
vp8/decoder/decodemv.c \
vp8/decoder/detokenize.c \
vp8/decoder/onyxd_if.c \
vp8/decoder/threading.c \
vp8/encoder/bitstream.c \
vp8/encoder/boolhuff.c \
vp8/encoder/dct.c \
vp8/encoder/denoising.c \
vp8/encoder/encodeframe.c \
vp8/encoder/encodeintra.c \
vp8/encoder/encodemb.c \
vp8/encoder/encodemv.c \
vp8/encoder/ethreading.c \
vp8/encoder/firstpass.c \
vp8/encoder/lookahead.c \
vp8/encoder/mcomp.c \
vp8/encoder/modecosts.c \
vp8/encoder/onyx_if.c \
vp8/encoder/pickinter.c \
vp8/encoder/picklpf.c \
vp8/encoder/ratectrl.c \
vp8/encoder/rdopt.c \
vp8/encoder/segmentation.c \
vp8/encoder/temporal_filter.c \
vp8/encoder/tokenize.c \
vp8/encoder/treewriter.c \
vp8/encoder/vp8_quantize.c \
vp8/vp8_cx_iface.c \
vp8/vp8_dx_iface.c \
vp9/common/vp9_alloccommon.c \
vp9/common/vp9_blockd.c \
vp9/common/vp9_common_data.c \
vp9/common/vp9_debugmodes.c \
vp9/common/vp9_entropy.c \
vp9/common/vp9_entropymode.c \
vp9/common/vp9_entropymv.c \
vp9/common/vp9_filter.c \
vp9/common/vp9_frame_buffers.c \
vp9/common/vp9_idct.c \
vp9/common/vp9_loopfilter.c \
vp9/common/vp9_mvref_common.c \
vp9/common/vp9_postproc.c \
vp9/common/vp9_pred_common.c \
vp9/common/vp9_quant_common.c \
vp9/common/vp9_reconinter.c \
vp9/common/vp9_reconintra.c \
vp9/common/vp9_rtcd.c \
vp9/common/vp9_scale.c \
vp9/common/vp9_scan.c \
vp9/common/vp9_seg_common.c \
vp9/common/vp9_thread_common.c \
vp9/common/vp9_tile_common.c \
vp9/decoder/vp9_decodeframe.c \
vp9/decoder/vp9_decodemv.c \
vp9/decoder/vp9_decoder.c \
vp9/decoder/vp9_detokenize.c \
vp9/decoder/vp9_dsubexp.c \
vp9/decoder/vp9_dthread.c \
vp9/encoder/vp9_alt_ref_aq.c \
vp9/encoder/vp9_aq_360.c \
vp9/encoder/vp9_aq_complexity.c \
vp9/encoder/vp9_aq_cyclicrefresh.c \
vp9/encoder/vp9_aq_variance.c \
vp9/encoder/vp9_bitstream.c \
vp9/encoder/vp9_blockiness.c \
vp9/encoder/vp9_context_tree.c \
vp9/encoder/vp9_cost.c \
vp9/encoder/vp9_dct.c \
vp9/encoder/vp9_encodeframe.c \
vp9/encoder/vp9_encodemb.c \
vp9/encoder/vp9_encodemv.c \
vp9/encoder/vp9_encoder.c \
vp9/encoder/vp9_ethread.c \
vp9/encoder/vp9_extend.c \
vp9/encoder/vp9_firstpass.c \
vp9/encoder/vp9_lookahead.c \
vp9/encoder/vp9_mbgraph.c \
vp9/encoder/vp9_mcomp.c \
vp9/encoder/vp9_noise_estimate.c \
vp9/encoder/vp9_picklpf.c \
vp9/encoder/vp9_pickmode.c \
vp9/encoder/vp9_quantize.c \
vp9/encoder/vp9_ratectrl.c \
vp9/encoder/vp9_rd.c \
vp9/encoder/vp9_rdopt.c \
vp9/encoder/vp9_resize.c \
vp9/encoder/vp9_segmentation.c \
vp9/encoder/vp9_skin_detection.c \
vp9/encoder/vp9_speed_features.c \
vp9/encoder/vp9_subexp.c \
vp9/encoder/vp9_svc_layercontext.c \
vp9/encoder/vp9_temporal_filter.c \
vp9/encoder/vp9_tokenize.c \
vp9/encoder/vp9_treewriter.c \
vp9/vp9_cx_iface.c \
vp9/vp9_dx_iface.c \
vpx/src/svc_encodeframe.c \
vpx/src/vpx_codec.c \
vpx/src/vpx_decoder.c \
vpx/src/vpx_encoder.c \
vpx/src/vpx_image.c \
vpx_config.c \
vpx_dsp/add_noise.c \
vpx_dsp/avg.c \
vpx_dsp/bitreader.c \
vpx_dsp/bitreader_buffer.c \
vpx_dsp/bitwriter.c \
vpx_dsp/bitwriter_buffer.c \
vpx_dsp/deblock.c \
vpx_dsp/fastssim.c \
vpx_dsp/fwd_txfm.c \
vpx_dsp/intrapred.c \
vpx_dsp/inv_txfm.c \
vpx_dsp/loopfilter.c \
vpx_dsp/prob.c \
vpx_dsp/psnr.c \
vpx_dsp/psnrhvs.c \
vpx_dsp/quantize.c \
vpx_dsp/sad.c \
vpx_dsp/tiny_ssim.c \
vpx_dsp/subtract.c \
vpx_dsp/sum_squares.c \
vpx_dsp/variance.c \
vpx_dsp/vpx_convolve.c \
vpx_dsp/vpx_dsp_rtcd.c \
vpx_mem/vpx_mem.c \
vpx_scale/generic/gen_scalers.c \
vpx_scale/generic/vpx_scale.c \
vpx_scale/generic/yv12config.c \
vpx_scale/generic/yv12extend.c \
vpx_scale/vpx_scale_rtcd.c \
vpx_util/vpx_thread.c 

LOCAL_SRC_FILES+=vpx_ports/arm_cpudetect.c \

LOCAL_SRC_FILES+=vp8/common/arm/bilinearpredict_neon.c \
vp8/common/arm/copymem_neon.c \
vp8/common/arm/dc_only_idct_add_neon.c \
vp8/common/arm/dequant_idct_neon.c \
vp8/common/arm/dequantizeb_neon.c \
vp8/common/arm/idct_blk_neon.c \
vp8/common/arm/idct_dequant_0_2x_neon.c \
vp8/common/arm/idct_dequant_full_2x_neon.c \
vp8/common/arm/iwalsh_neon.c \
vp8/common/arm/loopfilter_arm.c \
vp8/common/arm/loopfiltersimplehorizontaledge_neon.c \
vp8/common/arm/loopfiltersimpleverticaledge_neon.c \
vp8/common/arm/mbloopfilter_neon.c \
vp8/common/arm/shortidct4x4llm_neon.c \
vp8/common/arm/sixtappredict_neon.c \
vp8/common/arm/vp8_loopfilter_neon.c \
vp8/encoder/arm/denoising_neon.c \
vp8/encoder/arm/fastquantizeb_neon.c \
vp8/encoder/arm/shortfdct_neon.c \
vp8/encoder/arm/vp8_shortwalsh4x4_neon.c \
vp9/common/arm/vp9_iht4x4_add_neon.c \
vp9/common/arm/vp9_iht8x8_add_neon.c \
vp9/encoder/arm/vp9_dct_neon.c \
vp9/encoder/arm/vp9_error_neon.c \
vp9/encoder/arm/vp9_quantize_neon.c \
vpx_dsp/arm/avg_neon.c \
vpx_dsp/arm/deblock_neon.c \
vpx_dsp/arm/fwd_txfm_neon.c \
vpx_dsp/arm/hadamard_neon.c \
vpx_dsp/arm/idct16x16_neon.c \
vpx_dsp/arm/idct32x32_1_add_neon.c \
vpx_dsp/arm/idct32x32_135_add_neon.c \
vpx_dsp/arm/idct32x32_34_add_neon.c \
vpx_dsp/arm/idct32x32_add_neon.c \
vpx_dsp/arm/intrapred_neon.c \
vpx_dsp/arm/sad_neon.c \
vpx_dsp/arm/sad4d_neon.c \
vpx_dsp/arm/subpel_variance_neon.c \
vpx_dsp/arm/subtract_neon.c \
vpx_dsp/arm/variance_neon.c \
vpx_dsp/arm/vpx_convolve_neon.c \

ifeq ($(TARGET_ARCH_ABI), armeabi-v7a)

LOCAL_ASMFLAGS+=-march=armv7-a -mfloat-abi=softfp -mfpu=neon
LOCAL_CFLAGS+=-march=armv7-a  -mfloat-abi=softfp -mfpu=neon


LOCAL_SRC_FILES+=vpx_dsp/arm/idct_neon.asm.S \
vpx_dsp/arm/save_reg_neon.asm.S \
vpx_dsp/arm/idct16x16_1_add_neon.asm.S \
vpx_dsp/arm/idct16x16_add_neon.asm.S \
vpx_dsp/arm/idct4x4_1_add_neon.asm.S \
vpx_dsp/arm/idct4x4_add_neon.asm.S \
vpx_dsp/arm/idct8x8_1_add_neon.asm.S \
vpx_dsp/arm/idct8x8_add_neon.asm.S \
vpx_dsp/arm/intrapred_neon_asm.asm.S \
vpx_dsp/arm/loopfilter_16_neon.asm.S \
vpx_dsp/arm/loopfilter_4_neon.asm.S \
vpx_dsp/arm/loopfilter_8_neon.asm.S \
vpx_dsp/arm/vpx_convolve_avg_neon_asm.asm.S \
vpx_dsp/arm/vpx_convolve_copy_neon_asm.asm.S \
vpx_dsp/arm/vpx_convolve8_avg_neon_asm.asm.S \
vpx_dsp/arm/vpx_convolve8_neon_asm.asm.S 

else

endif


LOCAL_STATIC_LIBRARIES := cpufeatures
include $(BUILD_STATIC_LIBRARY)
$(call import-module,android/cpufeatures)
