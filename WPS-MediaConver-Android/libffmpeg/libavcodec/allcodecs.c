/*
 * Provide registration of all codecs, parsers and bitstream filters for libavcodec.
 * Copyright (c) 2002 Fabrice Bellard
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * @file
 * Provide registration of all codecs, parsers and bitstream filters for libavcodec.
 */

#include "config.h"
#include "libavutil/internal.h"
#include "libavutil/intmath.h"

#include "libavutil/thread.h"
#include "avcodec.h"
#include "version.h"

extern AVCodec ff_a64multi_encoder;
extern AVCodec ff_a64multi5_encoder;
extern AVCodec ff_aasc_decoder;
extern AVCodec ff_aic_decoder;
extern AVCodec ff_alias_pix_encoder;
extern AVCodec ff_alias_pix_decoder;
extern AVCodec ff_amv_encoder;
extern AVCodec ff_amv_decoder;
extern AVCodec ff_anm_decoder;
extern AVCodec ff_ansi_decoder;
extern AVCodec ff_apng_encoder;
extern AVCodec ff_apng_decoder;
extern AVCodec ff_arbc_decoder;
extern AVCodec ff_asv1_encoder;
extern AVCodec ff_asv1_decoder;
extern AVCodec ff_asv2_encoder;
extern AVCodec ff_asv2_decoder;
extern AVCodec ff_aura_decoder;
extern AVCodec ff_aura2_decoder;
extern AVCodec ff_avrp_encoder;
extern AVCodec ff_avrp_decoder;
extern AVCodec ff_avrn_decoder;
extern AVCodec ff_avs_decoder;
extern AVCodec ff_avui_encoder;
extern AVCodec ff_avui_decoder;
extern AVCodec ff_ayuv_encoder;
extern AVCodec ff_ayuv_decoder;
extern AVCodec ff_bethsoftvid_decoder;
extern AVCodec ff_bfi_decoder;
extern AVCodec ff_bink_decoder;
extern AVCodec ff_bitpacked_decoder;
extern AVCodec ff_bmp_encoder;
extern AVCodec ff_bmp_decoder;
extern AVCodec ff_bmv_video_decoder;
extern AVCodec ff_brender_pix_decoder;
extern AVCodec ff_c93_decoder;
extern AVCodec ff_cavs_decoder;
extern AVCodec ff_cdgraphics_decoder;
extern AVCodec ff_cdxl_decoder;
extern AVCodec ff_cfhd_decoder;
extern AVCodec ff_cinepak_encoder;
extern AVCodec ff_cinepak_decoder;
extern AVCodec ff_clearvideo_decoder;
extern AVCodec ff_cljr_encoder;
extern AVCodec ff_cljr_decoder;
extern AVCodec ff_cllc_decoder;
extern AVCodec ff_comfortnoise_encoder;
extern AVCodec ff_comfortnoise_decoder;
extern AVCodec ff_cpia_decoder;
extern AVCodec ff_cscd_decoder;
extern AVCodec ff_cyuv_decoder;
extern AVCodec ff_dds_decoder;
extern AVCodec ff_dfa_decoder;
extern AVCodec ff_dirac_decoder;
extern AVCodec ff_dnxhd_encoder;
extern AVCodec ff_dnxhd_decoder;
extern AVCodec ff_dpx_encoder;
extern AVCodec ff_dpx_decoder;
extern AVCodec ff_dsicinvideo_decoder;
extern AVCodec ff_dvaudio_decoder;
extern AVCodec ff_dvvideo_encoder;
extern AVCodec ff_dvvideo_decoder;
extern AVCodec ff_dxa_decoder;
extern AVCodec ff_dxtory_decoder;
extern AVCodec ff_dxv_decoder;
extern AVCodec ff_eacmv_decoder;
extern AVCodec ff_eamad_decoder;
extern AVCodec ff_eatgq_decoder;
extern AVCodec ff_eatgv_decoder;
extern AVCodec ff_eatqi_decoder;
extern AVCodec ff_eightbps_decoder;
extern AVCodec ff_eightsvx_exp_decoder;
extern AVCodec ff_eightsvx_fib_decoder;
extern AVCodec ff_escape124_decoder;
extern AVCodec ff_escape130_decoder;
extern AVCodec ff_exr_decoder;
extern AVCodec ff_ffv1_encoder;
extern AVCodec ff_ffv1_decoder;
extern AVCodec ff_ffvhuff_encoder;
extern AVCodec ff_ffvhuff_decoder;
extern AVCodec ff_fic_decoder;
extern AVCodec ff_fits_encoder;
extern AVCodec ff_fits_decoder;
extern AVCodec ff_flashsv_encoder;
extern AVCodec ff_flashsv_decoder;
extern AVCodec ff_flashsv2_encoder;
extern AVCodec ff_flashsv2_decoder;
extern AVCodec ff_flic_decoder;
extern AVCodec ff_flv_encoder;
extern AVCodec ff_flv_decoder;
extern AVCodec ff_fmvc_decoder;
extern AVCodec ff_fourxm_decoder;
extern AVCodec ff_fraps_decoder;
extern AVCodec ff_frwu_decoder;
extern AVCodec ff_g2m_decoder;
extern AVCodec ff_gdv_decoder;
extern AVCodec ff_gif_encoder;
extern AVCodec ff_gif_decoder;
extern AVCodec ff_h261_encoder;
extern AVCodec ff_h261_decoder;
extern AVCodec ff_h263_encoder;
extern AVCodec ff_h263_decoder;
extern AVCodec ff_h263i_decoder;
extern AVCodec ff_h263p_encoder;
extern AVCodec ff_h263p_decoder;
extern AVCodec ff_h263_v4l2m2m_decoder;
extern AVCodec ff_h264_decoder;
extern AVCodec ff_h264_crystalhd_decoder;
extern AVCodec ff_h264_v4l2m2m_decoder;
extern AVCodec ff_h264_mediacodec_decoder;
extern AVCodec ff_h264_mmal_decoder;
extern AVCodec ff_h264_qsv_decoder;
extern AVCodec ff_h264_rkmpp_decoder;
extern AVCodec ff_hap_encoder;
extern AVCodec ff_hap_decoder;
extern AVCodec ff_hevc_decoder;
extern AVCodec ff_hevc_qsv_decoder;
extern AVCodec ff_hevc_rkmpp_decoder;
extern AVCodec ff_hevc_v4l2m2m_decoder;
extern AVCodec ff_hnm4_video_decoder;
extern AVCodec ff_hq_hqa_decoder;
extern AVCodec ff_hqx_decoder;
extern AVCodec ff_huffyuv_encoder;
extern AVCodec ff_huffyuv_decoder;
extern AVCodec ff_hymt_decoder;
extern AVCodec ff_idcin_decoder;
extern AVCodec ff_iff_ilbm_decoder;
extern AVCodec ff_imm4_decoder;
extern AVCodec ff_indeo2_decoder;
extern AVCodec ff_indeo3_decoder;
extern AVCodec ff_indeo4_decoder;
extern AVCodec ff_indeo5_decoder;
extern AVCodec ff_interplay_video_decoder;
extern AVCodec ff_jpeg2000_encoder;
extern AVCodec ff_jpeg2000_decoder;
extern AVCodec ff_jpegls_encoder;
extern AVCodec ff_jpegls_decoder;
extern AVCodec ff_jv_decoder;
extern AVCodec ff_kgv1_decoder;
extern AVCodec ff_kmvc_decoder;
extern AVCodec ff_lagarith_decoder;
extern AVCodec ff_ljpeg_encoder;
extern AVCodec ff_loco_decoder;
extern AVCodec ff_m101_decoder;
extern AVCodec ff_magicyuv_encoder;
extern AVCodec ff_magicyuv_decoder;
extern AVCodec ff_mdec_decoder;
extern AVCodec ff_mimic_decoder;
extern AVCodec ff_mjpeg_encoder;
extern AVCodec ff_mjpeg_decoder;
extern AVCodec ff_mjpegb_decoder;
extern AVCodec ff_mmvideo_decoder;
extern AVCodec ff_motionpixels_decoder;
extern AVCodec ff_mpeg1video_encoder;
extern AVCodec ff_mpeg1video_decoder;
extern AVCodec ff_mpeg2video_encoder;
extern AVCodec ff_mpeg2video_decoder;
extern AVCodec ff_mpeg4_encoder;
extern AVCodec ff_mpeg4_decoder;
extern AVCodec ff_mpeg4_crystalhd_decoder;
extern AVCodec ff_mpeg4_v4l2m2m_decoder;
extern AVCodec ff_mpeg4_mmal_decoder;
extern AVCodec ff_mpegvideo_decoder;
extern AVCodec ff_mpeg1_v4l2m2m_decoder;
extern AVCodec ff_mpeg2_mmal_decoder;
extern AVCodec ff_mpeg2_crystalhd_decoder;
extern AVCodec ff_mpeg2_v4l2m2m_decoder;
extern AVCodec ff_mpeg2_qsv_decoder;
extern AVCodec ff_mpeg2_mediacodec_decoder;
extern AVCodec ff_msa1_decoder;
extern AVCodec ff_mscc_decoder;
extern AVCodec ff_msmpeg4v1_decoder;
extern AVCodec ff_msmpeg4v2_encoder;
extern AVCodec ff_msmpeg4v2_decoder;
extern AVCodec ff_msmpeg4v3_encoder;
extern AVCodec ff_msmpeg4v3_decoder;
extern AVCodec ff_msmpeg4_crystalhd_decoder;
extern AVCodec ff_msrle_decoder;
extern AVCodec ff_mss1_decoder;
extern AVCodec ff_mss2_decoder;
extern AVCodec ff_msvideo1_encoder;
extern AVCodec ff_msvideo1_decoder;
extern AVCodec ff_mszh_decoder;
extern AVCodec ff_mts2_decoder;
extern AVCodec ff_mvc1_decoder;
extern AVCodec ff_mvc2_decoder;
extern AVCodec ff_mwsc_decoder;
extern AVCodec ff_mxpeg_decoder;
extern AVCodec ff_nuv_decoder;
extern AVCodec ff_paf_video_decoder;
extern AVCodec ff_pam_encoder;
extern AVCodec ff_pam_decoder;
extern AVCodec ff_pbm_encoder;
extern AVCodec ff_pbm_decoder;
extern AVCodec ff_pcx_encoder;
extern AVCodec ff_pcx_decoder;
extern AVCodec ff_pgm_encoder;
extern AVCodec ff_pgm_decoder;
extern AVCodec ff_pgmyuv_encoder;
extern AVCodec ff_pgmyuv_decoder;
extern AVCodec ff_pictor_decoder;
extern AVCodec ff_pixlet_decoder;
extern AVCodec ff_png_encoder;
extern AVCodec ff_png_decoder;
extern AVCodec ff_ppm_encoder;
extern AVCodec ff_ppm_decoder;
extern AVCodec ff_prores_encoder;
extern AVCodec ff_prores_decoder;
extern AVCodec ff_prores_aw_encoder;
extern AVCodec ff_prores_ks_encoder;
extern AVCodec ff_prosumer_decoder;
extern AVCodec ff_psd_decoder;
extern AVCodec ff_ptx_decoder;
extern AVCodec ff_qdraw_decoder;
extern AVCodec ff_qpeg_decoder;
extern AVCodec ff_qtrle_encoder;
extern AVCodec ff_qtrle_decoder;
extern AVCodec ff_r10k_encoder;
extern AVCodec ff_r10k_decoder;
extern AVCodec ff_r210_encoder;
extern AVCodec ff_r210_decoder;
extern AVCodec ff_rasc_decoder;
extern AVCodec ff_rawvideo_encoder;
extern AVCodec ff_rawvideo_decoder;
extern AVCodec ff_rl2_decoder;
extern AVCodec ff_roq_encoder;
extern AVCodec ff_roq_decoder;
extern AVCodec ff_rpza_decoder;
extern AVCodec ff_rscc_decoder;
extern AVCodec ff_rv10_encoder;
extern AVCodec ff_rv10_decoder;
extern AVCodec ff_rv20_encoder;
extern AVCodec ff_rv20_decoder;
extern AVCodec ff_rv30_decoder;
extern AVCodec ff_rv40_decoder;
extern AVCodec ff_s302m_encoder;
extern AVCodec ff_s302m_decoder;
extern AVCodec ff_sanm_decoder;
extern AVCodec ff_scpr_decoder;
extern AVCodec ff_screenpresso_decoder;
extern AVCodec ff_sdx2_dpcm_decoder;
extern AVCodec ff_sgi_encoder;
extern AVCodec ff_sgi_decoder;
extern AVCodec ff_sgirle_decoder;
extern AVCodec ff_sheervideo_decoder;
extern AVCodec ff_smacker_decoder;
extern AVCodec ff_smc_decoder;
extern AVCodec ff_smvjpeg_decoder;
extern AVCodec ff_snow_encoder;
extern AVCodec ff_snow_decoder;
extern AVCodec ff_sp5x_decoder;
extern AVCodec ff_speedhq_decoder;
extern AVCodec ff_srgc_decoder;
extern AVCodec ff_sunrast_encoder;
extern AVCodec ff_sunrast_decoder;
extern AVCodec ff_svq1_encoder;
extern AVCodec ff_svq1_decoder;
extern AVCodec ff_svq3_decoder;
extern AVCodec ff_targa_encoder;
extern AVCodec ff_targa_decoder;
extern AVCodec ff_targa_y216_decoder;
extern AVCodec ff_tdsc_decoder;
extern AVCodec ff_theora_decoder;
extern AVCodec ff_thp_decoder;
extern AVCodec ff_tiertexseqvideo_decoder;
extern AVCodec ff_tiff_encoder;
extern AVCodec ff_tiff_decoder;
extern AVCodec ff_tmv_decoder;
extern AVCodec ff_truemotion1_decoder;
extern AVCodec ff_truemotion2_decoder;
extern AVCodec ff_truemotion2rt_decoder;
extern AVCodec ff_tscc_decoder;
extern AVCodec ff_tscc2_decoder;
extern AVCodec ff_txd_decoder;
extern AVCodec ff_ulti_decoder;
extern AVCodec ff_utvideo_encoder;
extern AVCodec ff_utvideo_decoder;
extern AVCodec ff_v210_encoder;
extern AVCodec ff_v210_decoder;
extern AVCodec ff_v210x_decoder;
extern AVCodec ff_v308_encoder;
extern AVCodec ff_v308_decoder;
extern AVCodec ff_v408_encoder;
extern AVCodec ff_v408_decoder;
extern AVCodec ff_v410_encoder;
extern AVCodec ff_v410_decoder;
extern AVCodec ff_vb_decoder;
extern AVCodec ff_vble_decoder;
extern AVCodec ff_vc1_decoder;
extern AVCodec ff_vc1_crystalhd_decoder;
extern AVCodec ff_vc1image_decoder;
extern AVCodec ff_vc1_mmal_decoder;
extern AVCodec ff_vc1_qsv_decoder;
extern AVCodec ff_vc1_v4l2m2m_decoder;
extern AVCodec ff_vc2_encoder;
extern AVCodec ff_vcr1_decoder;
extern AVCodec ff_vmdvideo_decoder;
extern AVCodec ff_vmnc_decoder;
extern AVCodec ff_vp3_decoder;
extern AVCodec ff_vp5_decoder;
extern AVCodec ff_vp6_decoder;
extern AVCodec ff_vp6a_decoder;
extern AVCodec ff_vp6f_decoder;
extern AVCodec ff_vp7_decoder;
extern AVCodec ff_vp8_decoder;
extern AVCodec ff_vp8_rkmpp_decoder;
extern AVCodec ff_vp8_v4l2m2m_decoder;
extern AVCodec ff_vp9_decoder;
extern AVCodec ff_vp9_rkmpp_decoder;
extern AVCodec ff_vp9_v4l2m2m_decoder;
extern AVCodec ff_vqa_decoder;
extern AVCodec ff_webp_decoder;
extern AVCodec ff_wcmv_decoder;
extern AVCodec ff_wrapped_avframe_encoder;
extern AVCodec ff_wrapped_avframe_decoder;
extern AVCodec ff_wmv1_encoder;
extern AVCodec ff_wmv1_decoder;
extern AVCodec ff_wmv2_encoder;
extern AVCodec ff_wmv2_decoder;
extern AVCodec ff_wmv3_decoder;
extern AVCodec ff_wmv3_crystalhd_decoder;
extern AVCodec ff_wmv3image_decoder;
extern AVCodec ff_wnv1_decoder;
extern AVCodec ff_xan_wc3_decoder;
extern AVCodec ff_xan_wc4_decoder;
extern AVCodec ff_xbm_encoder;
extern AVCodec ff_xbm_decoder;
extern AVCodec ff_xface_encoder;
extern AVCodec ff_xface_decoder;
extern AVCodec ff_xl_decoder;
extern AVCodec ff_xpm_decoder;
extern AVCodec ff_xwd_encoder;
extern AVCodec ff_xwd_decoder;
extern AVCodec ff_y41p_encoder;
extern AVCodec ff_y41p_decoder;
extern AVCodec ff_ylc_decoder;
extern AVCodec ff_yop_decoder;
extern AVCodec ff_yuv4_encoder;
extern AVCodec ff_yuv4_decoder;
extern AVCodec ff_zero12v_decoder;
extern AVCodec ff_zerocodec_decoder;
extern AVCodec ff_zlib_encoder;
extern AVCodec ff_zlib_decoder;
extern AVCodec ff_zmbv_encoder;
extern AVCodec ff_zmbv_decoder;

/* audio codecs */
extern AVCodec ff_aac_encoder;
extern AVCodec ff_aac_decoder;
extern AVCodec ff_aac_fixed_decoder;
extern AVCodec ff_aac_latm_decoder;
extern AVCodec ff_ac3_encoder;
extern AVCodec ff_ac3_decoder;
extern AVCodec ff_ac3_fixed_encoder;
extern AVCodec ff_ac3_fixed_decoder;
extern AVCodec ff_alac_encoder;
extern AVCodec ff_alac_decoder;
extern AVCodec ff_als_decoder;
extern AVCodec ff_amrnb_decoder;
extern AVCodec ff_amrwb_decoder;
extern AVCodec ff_ape_decoder;
extern AVCodec ff_aptx_encoder;
extern AVCodec ff_aptx_decoder;
extern AVCodec ff_aptx_hd_encoder;
#if CONFIG_APTX_HD_DECODER
extern AVCodec ff_aptx_hd_decoder;
#endif

extern AVCodec ff_atrac1_decoder;
extern AVCodec ff_atrac3_decoder;
extern AVCodec ff_atrac3al_decoder;
extern AVCodec ff_atrac3p_decoder;
extern AVCodec ff_atrac3pal_decoder;
extern AVCodec ff_atrac9_decoder;
extern AVCodec ff_binkaudio_dct_decoder;
extern AVCodec ff_binkaudio_rdft_decoder;
extern AVCodec ff_bmv_audio_decoder;
extern AVCodec ff_cook_decoder;
extern AVCodec ff_dca_encoder;
extern AVCodec ff_dca_decoder;
extern AVCodec ff_dolby_e_decoder;
extern AVCodec ff_dsd_lsbf_decoder;
extern AVCodec ff_dsd_msbf_decoder;
extern AVCodec ff_dsd_lsbf_planar_decoder;
extern AVCodec ff_dsd_msbf_planar_decoder;
extern AVCodec ff_dsicinaudio_decoder;
extern AVCodec ff_dss_sp_decoder;
extern AVCodec ff_dst_decoder;
extern AVCodec ff_eac3_encoder;
extern AVCodec ff_eac3_decoder;
extern AVCodec ff_evrc_decoder;
extern AVCodec ff_ffwavesynth_decoder;
extern AVCodec ff_flac_encoder;
extern AVCodec ff_flac_decoder;
extern AVCodec ff_g723_1_encoder;
extern AVCodec ff_g723_1_decoder;
extern AVCodec ff_g729_decoder;
extern AVCodec ff_gsm_decoder;
extern AVCodec ff_gsm_ms_decoder;
extern AVCodec ff_hcom_decoder;
extern AVCodec ff_iac_decoder;
extern AVCodec ff_ilbc_decoder;
extern AVCodec ff_imc_decoder;
extern AVCodec ff_interplay_acm_decoder;
extern AVCodec ff_mace3_decoder;
extern AVCodec ff_mace6_decoder;
extern AVCodec ff_metasound_decoder;
extern AVCodec ff_mlp_encoder;
extern AVCodec ff_mlp_decoder;
extern AVCodec ff_mp1_decoder;
extern AVCodec ff_mp1float_decoder;
extern AVCodec ff_mp2_encoder;
extern AVCodec ff_mp2_decoder;
extern AVCodec ff_mp2float_decoder;
extern AVCodec ff_mp2fixed_encoder;
extern AVCodec ff_mp3float_decoder;
extern AVCodec ff_mp3_decoder;
extern AVCodec ff_mp3adufloat_decoder;
extern AVCodec ff_mp3adu_decoder;
extern AVCodec ff_mp3on4float_decoder;
extern AVCodec ff_mp3on4_decoder;
extern AVCodec ff_mpc7_decoder;
extern AVCodec ff_mpc8_decoder;
extern AVCodec ff_nellymoser_encoder;
extern AVCodec ff_nellymoser_decoder;
extern AVCodec ff_on2avc_decoder;
extern AVCodec ff_opus_encoder;
extern AVCodec ff_opus_decoder;
extern AVCodec ff_paf_audio_decoder;
extern AVCodec ff_qcelp_decoder;
extern AVCodec ff_qdm2_decoder;
extern AVCodec ff_qdmc_decoder;
extern AVCodec ff_ra_144_encoder;
extern AVCodec ff_ra_144_decoder;
extern AVCodec ff_ra_288_decoder;
extern AVCodec ff_ralf_decoder;
extern AVCodec ff_sbc_encoder;
extern AVCodec ff_sbc_decoder;
extern AVCodec ff_shorten_decoder;
extern AVCodec ff_sipr_decoder;
extern AVCodec ff_smackaud_decoder;
extern AVCodec ff_sonic_encoder;
extern AVCodec ff_sonic_decoder;
extern AVCodec ff_sonic_ls_encoder;
extern AVCodec ff_tak_decoder;
extern AVCodec ff_truehd_encoder;
extern AVCodec ff_truehd_decoder;
extern AVCodec ff_truespeech_decoder;
extern AVCodec ff_tta_encoder;
extern AVCodec ff_tta_decoder;
extern AVCodec ff_twinvq_decoder;
extern AVCodec ff_vmdaudio_decoder;
extern AVCodec ff_vorbis_encoder;
extern AVCodec ff_vorbis_decoder;
extern AVCodec ff_wavpack_encoder;
extern AVCodec ff_wavpack_decoder;
extern AVCodec ff_wmalossless_decoder;
extern AVCodec ff_wmapro_decoder;
extern AVCodec ff_wmav1_encoder;
extern AVCodec ff_wmav1_decoder;
extern AVCodec ff_wmav2_encoder;
extern AVCodec ff_wmav2_decoder;
extern AVCodec ff_wmavoice_decoder;
extern AVCodec ff_ws_snd1_decoder;
extern AVCodec ff_xma1_decoder;
extern AVCodec ff_xma2_decoder;

/* PCM codecs */
extern AVCodec ff_pcm_alaw_encoder;
extern AVCodec ff_pcm_alaw_decoder;
extern AVCodec ff_pcm_bluray_decoder;
extern AVCodec ff_pcm_dvd_encoder;
extern AVCodec ff_pcm_dvd_decoder;
extern AVCodec ff_pcm_f16le_decoder;
extern AVCodec ff_pcm_f24le_decoder;
extern AVCodec ff_pcm_f32be_encoder;
extern AVCodec ff_pcm_f32be_decoder;
extern AVCodec ff_pcm_f32le_encoder;
extern AVCodec ff_pcm_f32le_decoder;
extern AVCodec ff_pcm_f64be_encoder;
extern AVCodec ff_pcm_f64be_decoder;
extern AVCodec ff_pcm_f64le_encoder;
extern AVCodec ff_pcm_f64le_decoder;
extern AVCodec ff_pcm_lxf_decoder;
extern AVCodec ff_pcm_mulaw_encoder;
extern AVCodec ff_pcm_mulaw_decoder;
extern AVCodec ff_pcm_s8_encoder;
extern AVCodec ff_pcm_s8_decoder;
extern AVCodec ff_pcm_s8_planar_encoder;
extern AVCodec ff_pcm_s8_planar_decoder;
extern AVCodec ff_pcm_s16be_encoder;
extern AVCodec ff_pcm_s16be_decoder;
extern AVCodec ff_pcm_s16be_planar_encoder;
extern AVCodec ff_pcm_s16be_planar_decoder;
extern AVCodec ff_pcm_s16le_encoder;
extern AVCodec ff_pcm_s16le_decoder;
extern AVCodec ff_pcm_s16le_planar_encoder;
extern AVCodec ff_pcm_s16le_planar_decoder;
extern AVCodec ff_pcm_s24be_encoder;
extern AVCodec ff_pcm_s24be_decoder;
extern AVCodec ff_pcm_s24daud_encoder;
extern AVCodec ff_pcm_s24daud_decoder;
extern AVCodec ff_pcm_s24le_encoder;
extern AVCodec ff_pcm_s24le_decoder;
extern AVCodec ff_pcm_s24le_planar_encoder;
extern AVCodec ff_pcm_s24le_planar_decoder;
extern AVCodec ff_pcm_s32be_encoder;
extern AVCodec ff_pcm_s32be_decoder;
extern AVCodec ff_pcm_s32le_encoder;
extern AVCodec ff_pcm_s32le_decoder;
extern AVCodec ff_pcm_s32le_planar_encoder;
extern AVCodec ff_pcm_s32le_planar_decoder;
extern AVCodec ff_pcm_s64be_encoder;
extern AVCodec ff_pcm_s64be_decoder;
extern AVCodec ff_pcm_s64le_encoder;
extern AVCodec ff_pcm_s64le_decoder;
extern AVCodec ff_pcm_u8_encoder;
extern AVCodec ff_pcm_u8_decoder;
extern AVCodec ff_pcm_u16be_encoder;
extern AVCodec ff_pcm_u16be_decoder;
extern AVCodec ff_pcm_u16le_encoder;
extern AVCodec ff_pcm_u16le_decoder;
extern AVCodec ff_pcm_u24be_encoder;
extern AVCodec ff_pcm_u24be_decoder;
extern AVCodec ff_pcm_u24le_encoder;
extern AVCodec ff_pcm_u24le_decoder;
extern AVCodec ff_pcm_u32be_encoder;
extern AVCodec ff_pcm_u32be_decoder;
extern AVCodec ff_pcm_u32le_encoder;
extern AVCodec ff_pcm_u32le_decoder;
extern AVCodec ff_pcm_vidc_encoder;
extern AVCodec ff_pcm_vidc_decoder;
extern AVCodec ff_pcm_zork_decoder;

/* DPCM codecs */
extern AVCodec ff_gremlin_dpcm_decoder;
extern AVCodec ff_interplay_dpcm_decoder;
extern AVCodec ff_roq_dpcm_encoder;
extern AVCodec ff_roq_dpcm_decoder;
extern AVCodec ff_sol_dpcm_decoder;
extern AVCodec ff_xan_dpcm_decoder;

/* ADPCM codecs */
extern AVCodec ff_adpcm_4xm_decoder;
extern AVCodec ff_adpcm_adx_encoder;
extern AVCodec ff_adpcm_adx_decoder;
extern AVCodec ff_adpcm_afc_decoder;
extern AVCodec ff_adpcm_aica_decoder;
extern AVCodec ff_adpcm_ct_decoder;
extern AVCodec ff_adpcm_dtk_decoder;
extern AVCodec ff_adpcm_ea_decoder;
extern AVCodec ff_adpcm_ea_maxis_xa_decoder;
extern AVCodec ff_adpcm_ea_r1_decoder;
extern AVCodec ff_adpcm_ea_r2_decoder;
extern AVCodec ff_adpcm_ea_r3_decoder;
extern AVCodec ff_adpcm_ea_xas_decoder;
extern AVCodec ff_adpcm_g722_encoder;
extern AVCodec ff_adpcm_g722_decoder;
extern AVCodec ff_adpcm_g726_encoder;
extern AVCodec ff_adpcm_g726_decoder;
extern AVCodec ff_adpcm_g726le_encoder;
extern AVCodec ff_adpcm_g726le_decoder;
extern AVCodec ff_adpcm_ima_amv_decoder;
extern AVCodec ff_adpcm_ima_apc_decoder;
extern AVCodec ff_adpcm_ima_dat4_decoder;
extern AVCodec ff_adpcm_ima_dk3_decoder;
extern AVCodec ff_adpcm_ima_dk4_decoder;
extern AVCodec ff_adpcm_ima_ea_eacs_decoder;
extern AVCodec ff_adpcm_ima_ea_sead_decoder;
extern AVCodec ff_adpcm_ima_iss_decoder;
extern AVCodec ff_adpcm_ima_oki_decoder;
extern AVCodec ff_adpcm_ima_qt_encoder;
extern AVCodec ff_adpcm_ima_qt_decoder;
extern AVCodec ff_adpcm_ima_rad_decoder;
extern AVCodec ff_adpcm_ima_smjpeg_decoder;
extern AVCodec ff_adpcm_ima_wav_encoder;
extern AVCodec ff_adpcm_ima_wav_decoder;
extern AVCodec ff_adpcm_ima_ws_decoder;
extern AVCodec ff_adpcm_ms_encoder;
extern AVCodec ff_adpcm_ms_decoder;
extern AVCodec ff_adpcm_mtaf_decoder;
extern AVCodec ff_adpcm_psx_decoder;
extern AVCodec ff_adpcm_sbpro_2_decoder;
extern AVCodec ff_adpcm_sbpro_3_decoder;
extern AVCodec ff_adpcm_sbpro_4_decoder;
extern AVCodec ff_adpcm_swf_encoder;
extern AVCodec ff_adpcm_swf_decoder;
extern AVCodec ff_adpcm_thp_decoder;
extern AVCodec ff_adpcm_thp_le_decoder;
extern AVCodec ff_adpcm_vima_decoder;
extern AVCodec ff_adpcm_xa_decoder;
extern AVCodec ff_adpcm_yamaha_encoder;
extern AVCodec ff_adpcm_yamaha_decoder;

/* subtitles */
extern AVCodec ff_ssa_encoder;
extern AVCodec ff_ssa_decoder;
extern AVCodec ff_ass_encoder;
extern AVCodec ff_ass_decoder;
extern AVCodec ff_ccaption_decoder;
extern AVCodec ff_dvbsub_encoder;
extern AVCodec ff_dvbsub_decoder;
extern AVCodec ff_dvdsub_encoder;
extern AVCodec ff_dvdsub_decoder;
extern AVCodec ff_jacosub_decoder;
extern AVCodec ff_microdvd_decoder;
extern AVCodec ff_movtext_encoder;
extern AVCodec ff_movtext_decoder;
extern AVCodec ff_mpl2_decoder;
extern AVCodec ff_pgssub_decoder;
#if CONFIG_PJS_DECODER
extern AVCodec ff_pjs_decoder;
#endif
extern AVCodec ff_realtext_decoder;
extern AVCodec ff_sami_decoder;
#if CONFIG_SRT_ENCODER
extern AVCodec ff_srt_encoder;
#endif
#if CONFIG_SRT_DECODER
extern AVCodec ff_srt_decoder;
#endif
#if CONFIG_STL_DECODER
extern AVCodec ff_stl_decoder;
#endif
#if CONFIG_SUBRIP_ENCODER
extern AVCodec ff_subrip_encoder;
#endif

#if CONFIG_SUBRIP_DECODER
extern AVCodec ff_subrip_decoder;
#endif

extern AVCodec ff_subviewer_decoder;

#if CONFIG_SUBVIEWER1_DECODER
extern AVCodec ff_subviewer1_decoder;
#endif

#if CONFIG_TEXT_ENCODER
extern AVCodec ff_text_encoder;
#endif

#if CONFIG_TEXT_DECODER
extern AVCodec ff_text_decoder;
#endif

#if CONFIG_VPLAYER_DECODER
extern AVCodec ff_vplayer_decoder;
#endif

extern AVCodec ff_webvtt_encoder;
extern AVCodec ff_webvtt_decoder;
extern AVCodec ff_xsub_encoder;
extern AVCodec ff_xsub_decoder;

/* external libraries */
extern AVCodec ff_aac_at_encoder;
extern AVCodec ff_aac_at_decoder;
extern AVCodec ff_ac3_at_decoder;
extern AVCodec ff_adpcm_ima_qt_at_decoder;
extern AVCodec ff_alac_at_encoder;
extern AVCodec ff_alac_at_decoder;
extern AVCodec ff_amr_nb_at_decoder;
extern AVCodec ff_eac3_at_decoder;
extern AVCodec ff_gsm_ms_at_decoder;
extern AVCodec ff_ilbc_at_encoder;
extern AVCodec ff_ilbc_at_decoder;
extern AVCodec ff_mp1_at_decoder;
extern AVCodec ff_mp2_at_decoder;
extern AVCodec ff_mp3_at_decoder;
extern AVCodec ff_pcm_alaw_at_encoder;
extern AVCodec ff_pcm_alaw_at_decoder;
extern AVCodec ff_pcm_mulaw_at_encoder;
extern AVCodec ff_pcm_mulaw_at_decoder;
extern AVCodec ff_qdmc_at_decoder;
extern AVCodec ff_qdm2_at_decoder;
extern AVCodec ff_libaom_av1_decoder;
extern AVCodec ff_libaom_av1_encoder;
extern AVCodec ff_libaribb24_decoder;
extern AVCodec ff_libcelt_decoder;
extern AVCodec ff_libcodec2_encoder;
extern AVCodec ff_libcodec2_decoder;
extern AVCodec ff_libdav1d_decoder;
extern AVCodec ff_libdavs2_decoder;
//extern AVCodec ff_libfdk_aac_encoder;
//extern AVCodec ff_libfdk_aac_decoder;
extern AVCodec ff_libgsm_encoder;
extern AVCodec ff_libgsm_decoder;
extern AVCodec ff_libgsm_ms_encoder;
extern AVCodec ff_libgsm_ms_decoder;
extern AVCodec ff_libilbc_encoder;
extern AVCodec ff_libilbc_decoder;
extern AVCodec ff_libmp3lame_encoder;
extern AVCodec ff_libopencore_amrnb_encoder;
extern AVCodec ff_libopencore_amrnb_decoder;
extern AVCodec ff_libopencore_amrwb_decoder;
extern AVCodec ff_libopenjpeg_encoder;
extern AVCodec ff_libopenjpeg_decoder;
extern AVCodec ff_libopus_encoder;
extern AVCodec ff_libopus_decoder;
extern AVCodec ff_librsvg_decoder;
extern AVCodec ff_libshine_encoder;
extern AVCodec ff_libspeex_encoder;
extern AVCodec ff_libspeex_decoder;
extern AVCodec ff_libtheora_encoder;
extern AVCodec ff_libtwolame_encoder;
extern AVCodec ff_libvo_amrwbenc_encoder;
extern AVCodec ff_libvorbis_encoder;
extern AVCodec ff_libvorbis_decoder;
extern AVCodec ff_libvpx_vp8_encoder;
extern AVCodec ff_libvpx_vp8_decoder;
extern AVCodec ff_libvpx_vp9_encoder;
extern AVCodec ff_libvpx_vp9_decoder;
extern AVCodec ff_libwavpack_encoder;
/* preferred over libwebp */
extern AVCodec ff_libwebp_anim_encoder;
extern AVCodec ff_libwebp_encoder;
extern AVCodec ff_libx262_encoder;
//extern AVCodec ff_libx264_encoder;
extern AVCodec ff_libx264rgb_encoder;
extern AVCodec ff_libx265_encoder;
extern AVCodec ff_libxavs_encoder;
extern AVCodec ff_libxavs2_encoder;
extern AVCodec ff_libxvid_encoder;
extern AVCodec ff_libzvbi_teletext_decoder;

/* text */
extern AVCodec ff_bintext_decoder;
extern AVCodec ff_xbin_decoder;
extern AVCodec ff_idf_decoder;

/* external libraries, that shouldn't be used by default if one of the
 * above is available */
extern AVCodec ff_h263_v4l2m2m_encoder;
extern AVCodec ff_libopenh264_encoder;
extern AVCodec ff_libopenh264_decoder;
extern AVCodec ff_h264_amf_encoder;
extern AVCodec ff_h264_cuvid_decoder;
extern AVCodec ff_h264_nvenc_encoder;
extern AVCodec ff_h264_omx_encoder;
extern AVCodec ff_h264_qsv_encoder;
extern AVCodec ff_h264_v4l2m2m_encoder;
extern AVCodec ff_h264_vaapi_encoder;
extern AVCodec ff_h264_videotoolbox_encoder;
#if FF_API_NVENC_OLD_NAME
extern AVCodec ff_nvenc_encoder;
extern AVCodec ff_nvenc_h264_encoder;
extern AVCodec ff_nvenc_hevc_encoder;
#endif
extern AVCodec ff_hevc_amf_encoder;
extern AVCodec ff_hevc_cuvid_decoder;
extern AVCodec ff_hevc_mediacodec_decoder;
extern AVCodec ff_hevc_nvenc_encoder;
extern AVCodec ff_hevc_qsv_encoder;
extern AVCodec ff_hevc_v4l2m2m_encoder;
extern AVCodec ff_hevc_vaapi_encoder;
extern AVCodec ff_hevc_videotoolbox_encoder;
extern AVCodec ff_libkvazaar_encoder;
extern AVCodec ff_mjpeg_cuvid_decoder;
extern AVCodec ff_mjpeg_qsv_encoder;
extern AVCodec ff_mjpeg_vaapi_encoder;
extern AVCodec ff_mpeg1_cuvid_decoder;
extern AVCodec ff_mpeg2_cuvid_decoder;
extern AVCodec ff_mpeg2_qsv_encoder;
extern AVCodec ff_mpeg2_vaapi_encoder;
extern AVCodec ff_mpeg4_cuvid_decoder;
extern AVCodec ff_mpeg4_mediacodec_decoder;
extern AVCodec ff_mpeg4_v4l2m2m_encoder;
extern AVCodec ff_vc1_cuvid_decoder;
extern AVCodec ff_vp8_cuvid_decoder;
extern AVCodec ff_vp8_mediacodec_decoder;
extern AVCodec ff_vp8_qsv_decoder;
extern AVCodec ff_vp8_v4l2m2m_encoder;
extern AVCodec ff_vp8_vaapi_encoder;
extern AVCodec ff_vp9_cuvid_decoder;
extern AVCodec ff_vp9_mediacodec_decoder;
extern AVCodec ff_vp9_vaapi_encoder;

static const AVCodec * const codec_list[] = {
    &ff_libopus_encoder,
    &ff_libopus_decoder,
    
    &ff_libvpx_vp8_encoder,
    &ff_libvpx_vp8_decoder,
    &ff_libvpx_vp9_encoder,
    &ff_libvpx_vp9_decoder,

    //&ff_libx264_encoder,
    &ff_libopenh264_encoder,

    &ff_libwebp_encoder,
    &ff_libwebp_anim_encoder,

    &ff_libmp3lame_encoder,

    //&ff_libfdk_aac_decoder,
    //&ff_libfdk_aac_encoder,

    &ff_a64multi_encoder,
    &ff_a64multi5_encoder,
    &ff_alias_pix_encoder,
    &ff_amv_encoder,
    &ff_apng_encoder,
    &ff_asv1_encoder,
    &ff_asv2_encoder,
    &ff_avrp_encoder,
    &ff_avui_encoder,
    &ff_ayuv_encoder,
    &ff_bmp_encoder,
    &ff_cinepak_encoder,
    &ff_cljr_encoder,
    &ff_comfortnoise_encoder,
    &ff_dnxhd_encoder,
    &ff_dpx_encoder,
    &ff_dvvideo_encoder,
    &ff_ffv1_encoder,
    &ff_ffvhuff_encoder,
    &ff_fits_encoder,
    &ff_flashsv_encoder,
    &ff_flashsv2_encoder,
    &ff_flv_encoder,
    &ff_gif_encoder,
    &ff_h261_encoder,
    &ff_h263_encoder,
    &ff_h263p_encoder,
    &ff_huffyuv_encoder,
    &ff_jpeg2000_encoder,
    &ff_jpegls_encoder,
    &ff_ljpeg_encoder,
    &ff_magicyuv_encoder,
    &ff_mjpeg_encoder,
    &ff_mpeg1video_encoder,
    &ff_mpeg2video_encoder,
    &ff_mpeg4_encoder,
    &ff_msmpeg4v2_encoder,
    &ff_msmpeg4v3_encoder,
    &ff_msvideo1_encoder,
    &ff_pam_encoder,
    &ff_pbm_encoder,
    &ff_pcx_encoder,
    &ff_pgm_encoder,
    &ff_pgmyuv_encoder,
    &ff_png_encoder,
    &ff_ppm_encoder,
    &ff_prores_encoder,
    &ff_prores_aw_encoder,
    &ff_prores_ks_encoder,
    &ff_qtrle_encoder,
    &ff_r10k_encoder,
    &ff_r210_encoder,
    &ff_rawvideo_encoder,
    &ff_roq_encoder,
    &ff_rv10_encoder,
    &ff_rv20_encoder,
    &ff_s302m_encoder,
    &ff_sgi_encoder,
    &ff_snow_encoder,
    &ff_sunrast_encoder,
    &ff_svq1_encoder,
    &ff_targa_encoder,
    &ff_tiff_encoder,
    &ff_utvideo_encoder,
    &ff_v210_encoder,
    &ff_v308_encoder,
    &ff_v408_encoder,
    &ff_v410_encoder,
    &ff_vc2_encoder,
    &ff_wrapped_avframe_encoder,
    &ff_wmv1_encoder,
    &ff_wmv2_encoder,
    &ff_xbm_encoder,
    &ff_xface_encoder,
    &ff_xwd_encoder,
    &ff_y41p_encoder,
    &ff_yuv4_encoder,
    &ff_zlib_encoder,
    &ff_zmbv_encoder,
    &ff_aac_encoder,
    &ff_ac3_encoder,
    &ff_ac3_fixed_encoder,
    &ff_alac_encoder,
    &ff_aptx_encoder,
    &ff_aptx_hd_encoder,
    &ff_dca_encoder,
    &ff_eac3_encoder,
    &ff_flac_encoder,
    &ff_g723_1_encoder,
    &ff_mlp_encoder,
    &ff_mp2_encoder,
    &ff_mp2fixed_encoder,
    &ff_nellymoser_encoder,
    &ff_opus_encoder,
    &ff_ra_144_encoder,
    &ff_sbc_encoder,
    &ff_sonic_encoder,
    &ff_sonic_ls_encoder,
    &ff_truehd_encoder,
    &ff_tta_encoder,
    &ff_vorbis_encoder,
    &ff_wavpack_encoder,
    &ff_wmav1_encoder,
    &ff_wmav2_encoder,
    &ff_pcm_alaw_encoder,
    &ff_pcm_dvd_encoder,
    &ff_pcm_f32be_encoder,
    &ff_pcm_f32le_encoder,
    &ff_pcm_f64be_encoder,
    &ff_pcm_f64le_encoder,
    &ff_pcm_mulaw_encoder,
    &ff_pcm_s8_encoder,
    &ff_pcm_s8_planar_encoder,
    &ff_pcm_s16be_encoder,
    &ff_pcm_s16be_planar_encoder,
    &ff_pcm_s16le_encoder,
    &ff_pcm_s16le_planar_encoder,
    &ff_pcm_s24be_encoder,
    &ff_pcm_s24daud_encoder,
    &ff_pcm_s24le_encoder,
    &ff_pcm_s24le_planar_encoder,
    &ff_pcm_s32be_encoder,
    &ff_pcm_s32le_encoder,
    &ff_pcm_s32le_planar_encoder,
    &ff_pcm_s64be_encoder,
    &ff_pcm_s64le_encoder,
    &ff_pcm_u8_encoder,
    &ff_pcm_u16be_encoder,
    &ff_pcm_u16le_encoder,
    &ff_pcm_u24be_encoder,
    &ff_pcm_u24le_encoder,
    &ff_pcm_u32be_encoder,
    &ff_pcm_u32le_encoder,
    &ff_pcm_vidc_encoder,
    &ff_roq_dpcm_encoder,
    &ff_adpcm_adx_encoder,
    &ff_adpcm_g722_encoder,
    &ff_adpcm_g726_encoder,
    &ff_adpcm_g726le_encoder,
    &ff_adpcm_ima_qt_encoder,
    &ff_adpcm_ima_wav_encoder,
    &ff_adpcm_ms_encoder,
    &ff_adpcm_swf_encoder,
    &ff_adpcm_yamaha_encoder,
    &ff_ssa_encoder,
    &ff_ass_encoder,
    &ff_dvbsub_encoder,
    &ff_dvdsub_encoder,
    &ff_movtext_encoder,
#if CONFIG_SRT_ENCODER
    &ff_srt_encoder,
#endif
#if CONFIG_SUBRIP_ENCODER
    &ff_subrip_encoder,
#endif
#if CONFIG_TEXT_ENCODER
    &ff_text_encoder,
#endif
    &ff_webvtt_encoder,
    &ff_xsub_encoder,
    &ff_aasc_decoder,
    &ff_aic_decoder,
    &ff_alias_pix_decoder,
    &ff_amv_decoder,
    &ff_anm_decoder,
    &ff_ansi_decoder,
    &ff_apng_decoder,
    &ff_arbc_decoder,
    &ff_asv1_decoder,
    &ff_asv2_decoder,
    &ff_aura_decoder,
    &ff_aura2_decoder,
    &ff_avrp_decoder,
    &ff_avrn_decoder,
    &ff_avs_decoder,
    &ff_avui_decoder,
    &ff_ayuv_decoder,
    &ff_bethsoftvid_decoder,
    &ff_bfi_decoder,
    &ff_bink_decoder,
    &ff_bitpacked_decoder,
    &ff_bmp_decoder,
    &ff_bmv_video_decoder,
    &ff_brender_pix_decoder,
    &ff_c93_decoder,
    &ff_cavs_decoder,
    &ff_cdgraphics_decoder,
    &ff_cdxl_decoder,
    &ff_cfhd_decoder,
    &ff_cinepak_decoder,
    &ff_clearvideo_decoder,
    &ff_cljr_decoder,
    &ff_cllc_decoder,
    &ff_comfortnoise_decoder,
    &ff_cpia_decoder,
    &ff_cscd_decoder,
    &ff_cyuv_decoder,
    &ff_dds_decoder,
    &ff_dfa_decoder,
    &ff_dirac_decoder,
    &ff_dnxhd_decoder,
    &ff_dpx_decoder,
    &ff_dsicinvideo_decoder,
    &ff_dvaudio_decoder,
    &ff_dvvideo_decoder,
    &ff_dxa_decoder,
    &ff_dxtory_decoder,
    &ff_dxv_decoder,
    &ff_eacmv_decoder,
    &ff_eamad_decoder,
    &ff_eatgq_decoder,
    &ff_eatgv_decoder,
    &ff_eatqi_decoder,
    &ff_eightbps_decoder,
    &ff_eightsvx_exp_decoder,
    &ff_eightsvx_fib_decoder,
    &ff_escape124_decoder,
    &ff_escape130_decoder,
    &ff_exr_decoder,
    &ff_ffv1_decoder,
    &ff_ffvhuff_decoder,
    &ff_fic_decoder,
    &ff_fits_decoder,
    &ff_flashsv_decoder,
    &ff_flashsv2_decoder,
    &ff_flic_decoder,
    &ff_flv_decoder,
    &ff_fmvc_decoder,
    &ff_fourxm_decoder,
    &ff_fraps_decoder,
    &ff_frwu_decoder,
    &ff_g2m_decoder,
    &ff_gdv_decoder,
    &ff_gif_decoder,
    &ff_h261_decoder,
    &ff_h263_decoder,
    &ff_h263i_decoder,
    &ff_h263p_decoder,
    &ff_h264_decoder,
    &ff_hap_decoder,
    &ff_hevc_decoder,
    &ff_hnm4_video_decoder,
    &ff_hq_hqa_decoder,
    &ff_hqx_decoder,
    &ff_huffyuv_decoder,
    &ff_hymt_decoder,
    &ff_idcin_decoder,
    &ff_iff_ilbm_decoder,
    &ff_imm4_decoder,
    &ff_indeo2_decoder,
    &ff_indeo3_decoder,
    &ff_indeo4_decoder,
    &ff_indeo5_decoder,
    &ff_interplay_video_decoder,
    &ff_jpeg2000_decoder,
    &ff_jpegls_decoder,
    &ff_jv_decoder,
    &ff_kgv1_decoder,
    &ff_kmvc_decoder,
    &ff_lagarith_decoder,
    &ff_loco_decoder,
    &ff_m101_decoder,
    &ff_magicyuv_decoder,
    &ff_mdec_decoder,
    &ff_mimic_decoder,
    &ff_mjpeg_decoder,
    &ff_mjpegb_decoder,
    &ff_mmvideo_decoder,
    &ff_motionpixels_decoder,
    &ff_mpeg1video_decoder,
    &ff_mpeg2video_decoder,
    &ff_mpeg4_decoder,
    &ff_mpegvideo_decoder,
    &ff_msa1_decoder,
    &ff_mscc_decoder,
    &ff_msmpeg4v1_decoder,
    &ff_msmpeg4v2_decoder,
    &ff_msmpeg4v3_decoder,
    &ff_msrle_decoder,
    &ff_mss1_decoder,
    &ff_mss2_decoder,
    &ff_msvideo1_decoder,
    &ff_mszh_decoder,
    &ff_mts2_decoder,
    &ff_mvc1_decoder,
    &ff_mvc2_decoder,
    &ff_mwsc_decoder,
    &ff_mxpeg_decoder,
    &ff_nuv_decoder,
    &ff_paf_video_decoder,
    &ff_pam_decoder,
    &ff_pbm_decoder,
    &ff_pcx_decoder,
    &ff_pgm_decoder,
    &ff_pgmyuv_decoder,
    &ff_pictor_decoder,
    &ff_pixlet_decoder,
    &ff_png_decoder,
    &ff_ppm_decoder,
    &ff_prores_decoder,
    &ff_prosumer_decoder,
    &ff_psd_decoder,
    &ff_ptx_decoder,
    &ff_qdraw_decoder,
    &ff_qpeg_decoder,
    &ff_qtrle_decoder,
    &ff_r10k_decoder,
    &ff_r210_decoder,
    &ff_rasc_decoder,
    &ff_rawvideo_decoder,
    &ff_rl2_decoder,
    &ff_roq_decoder,
    &ff_rpza_decoder,
    &ff_rscc_decoder,
    &ff_rv10_decoder,
    &ff_rv20_decoder,
    &ff_rv30_decoder,
    &ff_rv40_decoder,
    &ff_s302m_decoder,
    &ff_sanm_decoder,
    &ff_scpr_decoder,
    &ff_screenpresso_decoder,
    &ff_sdx2_dpcm_decoder,
    &ff_sgi_decoder,
    &ff_sgirle_decoder,
    &ff_sheervideo_decoder,
    &ff_smacker_decoder,
    &ff_smc_decoder,
    &ff_smvjpeg_decoder,
    &ff_snow_decoder,
    &ff_sp5x_decoder,
    &ff_speedhq_decoder,
    &ff_srgc_decoder,
    &ff_sunrast_decoder,
    &ff_svq1_decoder,
    &ff_svq3_decoder,
    &ff_targa_decoder,
    &ff_targa_y216_decoder,
    &ff_tdsc_decoder,
    &ff_theora_decoder,
    &ff_thp_decoder,
    &ff_tiertexseqvideo_decoder,
    &ff_tiff_decoder,
    &ff_tmv_decoder,
    &ff_truemotion1_decoder,
    &ff_truemotion2_decoder,
    &ff_truemotion2rt_decoder,
    &ff_tscc_decoder,
    &ff_tscc2_decoder,
    &ff_txd_decoder,
    &ff_ulti_decoder,
    &ff_utvideo_decoder,
    &ff_v210_decoder,
    &ff_v210x_decoder,
    &ff_v308_decoder,
    &ff_v408_decoder,
    &ff_v410_decoder,
    &ff_vb_decoder,
    &ff_vble_decoder,
    &ff_vc1_decoder,
    &ff_vc1image_decoder,
    &ff_vcr1_decoder,
    &ff_vmdvideo_decoder,
    &ff_vmnc_decoder,
    &ff_vp3_decoder,
    &ff_vp5_decoder,
    &ff_vp6_decoder,
    &ff_vp6a_decoder,
    &ff_vp6f_decoder,
    &ff_vp7_decoder,
    &ff_vp8_decoder,
    &ff_vp9_decoder,
    &ff_vqa_decoder,
    &ff_webp_decoder,
    &ff_wcmv_decoder,
    &ff_wrapped_avframe_decoder,
    &ff_wmv1_decoder,
    &ff_wmv2_decoder,
    &ff_wmv3_decoder,
    &ff_wmv3image_decoder,
    &ff_wnv1_decoder,
    &ff_xan_wc3_decoder,
    &ff_xan_wc4_decoder,
    &ff_xbm_decoder,
    &ff_xface_decoder,
    &ff_xl_decoder,
    &ff_xpm_decoder,
    &ff_xwd_decoder,
    &ff_y41p_decoder,
    &ff_ylc_decoder,
    &ff_yop_decoder,
    &ff_yuv4_decoder,
    &ff_zero12v_decoder,
    &ff_zerocodec_decoder,
    &ff_zlib_decoder,
    &ff_zmbv_decoder,
    &ff_aac_decoder,
    &ff_aac_fixed_decoder,
    &ff_aac_latm_decoder,
    &ff_ac3_decoder,
    &ff_ac3_fixed_decoder,
    &ff_alac_decoder,
    &ff_als_decoder,
    &ff_amrnb_decoder,
    &ff_amrwb_decoder,
    &ff_ape_decoder,
    &ff_aptx_decoder,
    &ff_aptx_hd_decoder,
    &ff_atrac1_decoder,
    &ff_atrac3_decoder,
    &ff_atrac3al_decoder,
    &ff_atrac3p_decoder,
    &ff_atrac3pal_decoder,
    &ff_atrac9_decoder,
    &ff_binkaudio_dct_decoder,
    &ff_binkaudio_rdft_decoder,
    &ff_bmv_audio_decoder,
    &ff_cook_decoder,
    &ff_dca_decoder,
    &ff_dolby_e_decoder,
    &ff_dsd_lsbf_decoder,
    &ff_dsd_msbf_decoder,
    &ff_dsd_lsbf_planar_decoder,
    &ff_dsd_msbf_planar_decoder,
    &ff_dsicinaudio_decoder,
    &ff_dss_sp_decoder,
    &ff_dst_decoder,
    &ff_eac3_decoder,
    &ff_evrc_decoder,
    &ff_ffwavesynth_decoder,
    &ff_flac_decoder,
    &ff_g723_1_decoder,
    &ff_g729_decoder,
    &ff_gsm_decoder,
    &ff_gsm_ms_decoder,
    &ff_hcom_decoder,
    &ff_iac_decoder,
    &ff_ilbc_decoder,
    &ff_imc_decoder,
    &ff_interplay_acm_decoder,
    &ff_mace3_decoder,
    &ff_mace6_decoder,
    &ff_metasound_decoder,
    &ff_mlp_decoder,
    &ff_mp1_decoder,
    &ff_mp1float_decoder,
    &ff_mp2_decoder,
    &ff_mp2float_decoder,
    &ff_mp3float_decoder,
    &ff_mp3_decoder,
    &ff_mp3adufloat_decoder,
    &ff_mp3adu_decoder,
    &ff_mp3on4float_decoder,
    &ff_mp3on4_decoder,
    &ff_mpc7_decoder,
    &ff_mpc8_decoder,
    &ff_nellymoser_decoder,
    &ff_on2avc_decoder,
    &ff_opus_decoder,
    &ff_paf_audio_decoder,
    &ff_qcelp_decoder,
    &ff_qdm2_decoder,
    &ff_qdmc_decoder,
    &ff_ra_144_decoder,
    &ff_ra_288_decoder,
    &ff_ralf_decoder,
    &ff_sbc_decoder,
    &ff_shorten_decoder,
    &ff_sipr_decoder,
    &ff_smackaud_decoder,
    &ff_sonic_decoder,
    &ff_tak_decoder,
    &ff_truehd_decoder,
    &ff_truespeech_decoder,
    &ff_tta_decoder,
    &ff_twinvq_decoder,
    &ff_vmdaudio_decoder,
    &ff_vorbis_decoder,
    &ff_wavpack_decoder,
    &ff_wmalossless_decoder,
    &ff_wmapro_decoder,
    &ff_wmav1_decoder,
    &ff_wmav2_decoder,
    &ff_wmavoice_decoder,
    &ff_ws_snd1_decoder,
    &ff_xma1_decoder,
    &ff_xma2_decoder,
    &ff_pcm_alaw_decoder,
    &ff_pcm_bluray_decoder,
    &ff_pcm_dvd_decoder,
    &ff_pcm_f16le_decoder,
    &ff_pcm_f24le_decoder,
    &ff_pcm_f32be_decoder,
    &ff_pcm_f32le_decoder,
    &ff_pcm_f64be_decoder,
    &ff_pcm_f64le_decoder,
    &ff_pcm_lxf_decoder,
    &ff_pcm_mulaw_decoder,
    &ff_pcm_s8_decoder,
    &ff_pcm_s8_planar_decoder,
    &ff_pcm_s16be_decoder,
    &ff_pcm_s16be_planar_decoder,
    &ff_pcm_s16le_decoder,
    &ff_pcm_s16le_planar_decoder,
    &ff_pcm_s24be_decoder,
    &ff_pcm_s24daud_decoder,
    &ff_pcm_s24le_decoder,
    &ff_pcm_s24le_planar_decoder,
    &ff_pcm_s32be_decoder,
    &ff_pcm_s32le_decoder,
    &ff_pcm_s32le_planar_decoder,
    &ff_pcm_s64be_decoder,
    &ff_pcm_s64le_decoder,
    &ff_pcm_u8_decoder,
    &ff_pcm_u16be_decoder,
    &ff_pcm_u16le_decoder,
    &ff_pcm_u24be_decoder,
    &ff_pcm_u24le_decoder,
    &ff_pcm_u32be_decoder,
    &ff_pcm_u32le_decoder,
    &ff_pcm_vidc_decoder,
    &ff_pcm_zork_decoder,
    &ff_gremlin_dpcm_decoder,
    &ff_interplay_dpcm_decoder,
    &ff_roq_dpcm_decoder,
    &ff_sol_dpcm_decoder,
    &ff_xan_dpcm_decoder,
    &ff_adpcm_4xm_decoder,
    &ff_adpcm_adx_decoder,
    &ff_adpcm_afc_decoder,
    &ff_adpcm_aica_decoder,
    &ff_adpcm_ct_decoder,
    &ff_adpcm_dtk_decoder,
    &ff_adpcm_ea_decoder,
    &ff_adpcm_ea_maxis_xa_decoder,
    &ff_adpcm_ea_r1_decoder,
    &ff_adpcm_ea_r2_decoder,
    &ff_adpcm_ea_r3_decoder,
    &ff_adpcm_ea_xas_decoder,
    &ff_adpcm_g722_decoder,
    &ff_adpcm_g726_decoder,
    &ff_adpcm_g726le_decoder,
    &ff_adpcm_ima_amv_decoder,
    &ff_adpcm_ima_apc_decoder,
    &ff_adpcm_ima_dat4_decoder,
    &ff_adpcm_ima_dk3_decoder,
    &ff_adpcm_ima_dk4_decoder,
    &ff_adpcm_ima_ea_eacs_decoder,
    &ff_adpcm_ima_ea_sead_decoder,
    &ff_adpcm_ima_iss_decoder,
    &ff_adpcm_ima_oki_decoder,
    &ff_adpcm_ima_qt_decoder,
    &ff_adpcm_ima_rad_decoder,
    &ff_adpcm_ima_smjpeg_decoder,
    &ff_adpcm_ima_wav_decoder,
    &ff_adpcm_ima_ws_decoder,
    &ff_adpcm_ms_decoder,
    &ff_adpcm_mtaf_decoder,
    &ff_adpcm_psx_decoder,
    &ff_adpcm_sbpro_2_decoder,
    &ff_adpcm_sbpro_3_decoder,
    &ff_adpcm_sbpro_4_decoder,
    &ff_adpcm_swf_decoder,
    &ff_adpcm_thp_decoder,
    &ff_adpcm_thp_le_decoder,
    &ff_adpcm_vima_decoder,
    &ff_adpcm_xa_decoder,
    &ff_adpcm_yamaha_decoder,
    &ff_ssa_decoder,
    &ff_ass_decoder,
    &ff_ccaption_decoder,
    &ff_dvbsub_decoder,
    &ff_dvdsub_decoder,
    &ff_jacosub_decoder,
    &ff_microdvd_decoder,
    &ff_movtext_decoder,
    &ff_mpl2_decoder,
    &ff_pgssub_decoder,
#if CONFIG_PJS_DECODER
    &ff_pjs_decoder,
#endif
    &ff_realtext_decoder,
    &ff_sami_decoder,
#if CONFIG_SRT_DECODER
    &ff_srt_decoder,
#endif
#if CONFIG_STL_DECODER
    &ff_stl_decoder,
#endif
#if CONFIG_SUBRIP_DECODER
    &ff_subrip_decoder,
#endif
#if CONFIG_SUBVIEWER1_ENCODER
    &ff_subviewer_decoder,
#endif
#if CONFIG_SUBVIEWER1_DECODER
    &ff_subviewer1_decoder,
#endif
#if CONFIG_TEXT_DECODER
    &ff_text_decoder,
#endif
#if CONFIG_VPLAYER_DECODER
    &ff_vplayer_decoder,
#endif
    &ff_webvtt_decoder,
    &ff_xsub_decoder,
    &ff_bintext_decoder,
    &ff_xbin_decoder,
    &ff_idf_decoder,
    NULL };


static AVOnce av_codec_static_init = AV_ONCE_INIT;
static void av_codec_init_static(void)
{
    for (int i = 0; codec_list[i]; i++) {
        if (codec_list[i]->init_static_data)
            codec_list[i]->init_static_data((AVCodec*)codec_list[i]);
    }
}

const AVCodec *av_codec_iterate(void **opaque)
{
    uintptr_t i = (uintptr_t)*opaque;
    const AVCodec *c = codec_list[i];

    ff_thread_once(&av_codec_static_init, av_codec_init_static);

    if (c)
        *opaque = (void*)(i + 1);

    return c;
}

#if FF_API_NEXT
FF_DISABLE_DEPRECATION_WARNINGS
static AVOnce av_codec_next_init = AV_ONCE_INIT;

static void av_codec_init_next(void)
{
    AVCodec *prev = NULL, *p;
    void *i = 0;
    while ((p = (AVCodec*)av_codec_iterate(&i))) {
        if (prev)
            prev->next = p;
        prev = p;
    }
}



av_cold void avcodec_register(AVCodec *codec)
{
    ff_thread_once(&av_codec_next_init, av_codec_init_next);
}

AVCodec *av_codec_next(const AVCodec *c)
{
    ff_thread_once(&av_codec_next_init, av_codec_init_next);

    if (c)
        return c->next;
    else
        return (AVCodec*)codec_list[0];
}

void avcodec_register_all(void)
{
    ff_thread_once(&av_codec_next_init, av_codec_init_next);
}
FF_ENABLE_DEPRECATION_WARNINGS
#endif

static enum AVCodecID remap_deprecated_codec_id(enum AVCodecID id)
{
    switch(id){
        //This is for future deprecatec codec ids, its empty since
        //last major bump but will fill up again over time, please don't remove it
        default                                         : return id;
    }
}

static AVCodec *find_codec(enum AVCodecID id, int (*x)(const AVCodec *))
{
    const AVCodec *p, *experimental = NULL;
    void *i = 0;

    id = remap_deprecated_codec_id(id);

    while ((p = av_codec_iterate(&i))) {
        if (!x(p))
            continue;
        if (p->id == id) {
            if (p->capabilities & AV_CODEC_CAP_EXPERIMENTAL && !experimental) {
                experimental = p;
            } else
                return (AVCodec*)p;
        }
    }

    return (AVCodec*)experimental;
}

AVCodec *avcodec_find_encoder(enum AVCodecID id)
{
    return find_codec(id, av_codec_is_encoder);
}

AVCodec *avcodec_find_decoder(enum AVCodecID id)
{
    return find_codec(id, av_codec_is_decoder);
}

static AVCodec *find_codec_by_name(const char *name, int (*x)(const AVCodec *))
{
    void *i = 0;
    const AVCodec *p;

    if (!name)
        return NULL;

    while ((p = av_codec_iterate(&i))) {
        if (!x(p))
            continue;
        if (strcmp(name, p->name) == 0)
            return (AVCodec*)p;
    }

    return NULL;
}

AVCodec *avcodec_find_encoder_by_name(const char *name)
{
    return find_codec_by_name(name, av_codec_is_encoder);
}

AVCodec *avcodec_find_decoder_by_name(const char *name)
{
    return find_codec_by_name(name, av_codec_is_decoder);
}
