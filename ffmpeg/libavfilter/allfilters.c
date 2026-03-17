/*
 * filter registration
 * Copyright (c) 2008 Vitor Sessak
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

#include "libavutil/thread.h"
#include "avfilter.h"
#include "ffmpeg-config.h"

extern AVFilter ff_af_abench;
extern AVFilter ff_af_acompressor;
extern AVFilter ff_af_acontrast;
extern AVFilter ff_af_acopy;
extern AVFilter ff_af_acrossfade;
extern AVFilter ff_af_acrusher;
extern AVFilter ff_af_adelay;
extern AVFilter ff_af_aecho;
extern AVFilter ff_af_aemphasis;
extern AVFilter ff_af_aeval;
extern AVFilter ff_af_afade;
extern AVFilter ff_af_afftfilt;
extern AVFilter ff_af_afir;
extern AVFilter ff_af_aformat;
extern AVFilter ff_af_agate;
extern AVFilter ff_af_aiir;
extern AVFilter ff_af_ainterleave;
extern AVFilter ff_af_alimiter;
extern AVFilter ff_af_allpass;
extern AVFilter ff_af_aloop;
extern AVFilter ff_af_amerge;
extern AVFilter ff_af_ametadata;
extern AVFilter ff_af_amix;
extern AVFilter ff_af_anequalizer;
extern AVFilter ff_af_anull;
extern AVFilter ff_af_apad;
extern AVFilter ff_af_aperms;
extern AVFilter ff_af_aphaser;
extern AVFilter ff_af_apulsator;
extern AVFilter ff_af_arealtime;
extern AVFilter ff_af_aresample;
extern AVFilter ff_af_areverse;
extern AVFilter ff_af_aselect;
extern AVFilter ff_af_asendcmd;
extern AVFilter ff_af_asetnsamples;
extern AVFilter ff_af_asetpts;
extern AVFilter ff_af_asetrate;
extern AVFilter ff_af_asettb;
extern AVFilter ff_af_ashowinfo;
extern AVFilter ff_af_asidedata;
extern AVFilter ff_af_asplit;
extern AVFilter ff_af_astats;
extern AVFilter ff_af_astreamselect;
extern AVFilter ff_af_atempo;
extern AVFilter ff_af_atrim;
extern AVFilter ff_af_azmq;
extern AVFilter ff_af_bandpass;
extern AVFilter ff_af_bandreject;
extern AVFilter ff_af_bass;
extern AVFilter ff_af_biquad;
extern AVFilter ff_af_bs2b;
extern AVFilter ff_af_channelmap;
extern AVFilter ff_af_channelsplit;
extern AVFilter ff_af_chorus;
extern AVFilter ff_af_compand;
extern AVFilter ff_af_compensationdelay;
extern AVFilter ff_af_crossfeed;
extern AVFilter ff_af_crystalizer;
extern AVFilter ff_af_dcshift;
extern AVFilter ff_af_drmeter;
extern AVFilter ff_af_dynaudnorm;
extern AVFilter ff_af_earwax;
extern AVFilter ff_af_ebur128;
extern AVFilter ff_af_equalizer;
extern AVFilter ff_af_extrastereo;
extern AVFilter ff_af_firequalizer;
extern AVFilter ff_af_flanger;
extern AVFilter ff_af_haas;
extern AVFilter ff_af_hdcd;
extern AVFilter ff_af_headphone;
extern AVFilter ff_af_highpass;
extern AVFilter ff_af_join;
extern AVFilter ff_af_ladspa;
extern AVFilter ff_af_loudnorm;
extern AVFilter ff_af_lowpass;
extern AVFilter ff_af_lv2;
extern AVFilter ff_af_mcompand;
extern AVFilter ff_af_pan;
extern AVFilter ff_af_replaygain;
extern AVFilter ff_af_resample;
extern AVFilter ff_af_rubberband;
extern AVFilter ff_af_sidechaincompress;
extern AVFilter ff_af_sidechaingate;
extern AVFilter ff_af_silencedetect;
extern AVFilter ff_af_silenceremove;
extern AVFilter ff_af_sofalizer;
extern AVFilter ff_af_stereotools;
extern AVFilter ff_af_stereowiden;
extern AVFilter ff_af_superequalizer;
extern AVFilter ff_af_surround;
extern AVFilter ff_af_treble;
extern AVFilter ff_af_tremolo;
extern AVFilter ff_af_vibrato;
extern AVFilter ff_af_volume;
extern AVFilter ff_af_volumedetect;

extern AVFilter ff_asrc_aevalsrc;
extern AVFilter ff_asrc_anoisesrc;
extern AVFilter ff_asrc_anullsrc;
extern AVFilter ff_asrc_flite;
extern AVFilter ff_asrc_hilbert;
extern AVFilter ff_asrc_sine;

extern AVFilter ff_asink_anullsink;

extern AVFilter ff_vf_alphaextract;
extern AVFilter ff_vf_alphamerge;
extern AVFilter ff_vf_ass;
extern AVFilter ff_vf_atadenoise;
extern AVFilter ff_vf_avgblur;
extern AVFilter ff_vf_avgblur_opencl;
extern AVFilter ff_vf_bbox;
extern AVFilter ff_vf_bench;
extern AVFilter ff_vf_bitplanenoise;
extern AVFilter ff_vf_blackdetect;
extern AVFilter ff_vf_blackframe;
extern AVFilter ff_vf_blend;
extern AVFilter ff_vf_boxblur;
extern AVFilter ff_vf_bwdif;
extern AVFilter ff_vf_chromakey;
extern AVFilter ff_vf_ciescope;
extern AVFilter ff_vf_codecview;
extern AVFilter ff_vf_colorbalance;
extern AVFilter ff_vf_colorchannelmixer;
extern AVFilter ff_vf_colorkey;
extern AVFilter ff_vf_colorlevels;
extern AVFilter ff_vf_colormatrix;
extern AVFilter ff_vf_colorspace;
extern AVFilter ff_vf_convolution;
extern AVFilter ff_vf_convolution_opencl;
extern AVFilter ff_vf_convolve;
extern AVFilter ff_vf_copy;
extern AVFilter ff_vf_coreimage;
extern AVFilter ff_vf_cover_rect;
extern AVFilter ff_vf_crop;
extern AVFilter ff_vf_cropdetect;
extern AVFilter ff_vf_curves;
extern AVFilter ff_vf_datascope;
extern AVFilter ff_vf_dctdnoiz;
extern AVFilter ff_vf_deband;
extern AVFilter ff_vf_decimate;
extern AVFilter ff_vf_deconvolve;
extern AVFilter ff_vf_deflate;
extern AVFilter ff_vf_deflicker;
extern AVFilter ff_vf_deinterlace_qsv;
extern AVFilter ff_vf_deinterlace_vaapi;
extern AVFilter ff_vf_dejudder;
extern AVFilter ff_vf_delogo;
extern AVFilter ff_vf_denoise_vaapi;
extern AVFilter ff_vf_deshake;
extern AVFilter ff_vf_despill;
extern AVFilter ff_vf_detelecine;
extern AVFilter ff_vf_dilation;
extern AVFilter ff_vf_displace;
extern AVFilter ff_vf_doubleweave;
extern AVFilter ff_vf_drawbox;
extern AVFilter ff_vf_drawgraph;
extern AVFilter ff_vf_drawgrid;
extern AVFilter ff_vf_drawtext;
extern AVFilter ff_vf_edgedetect;
extern AVFilter ff_vf_elbg;
extern AVFilter ff_vf_entropy;
extern AVFilter ff_vf_eq;
extern AVFilter ff_vf_erosion;
extern AVFilter ff_vf_extractplanes;
extern AVFilter ff_vf_fade;
extern AVFilter ff_vf_fftfilt;
extern AVFilter ff_vf_field;
extern AVFilter ff_vf_fieldhint;
extern AVFilter ff_vf_fieldmatch;
extern AVFilter ff_vf_fieldorder;
extern AVFilter ff_vf_fillborders;
extern AVFilter ff_vf_find_rect;
extern AVFilter ff_vf_floodfill;
extern AVFilter ff_vf_format;
extern AVFilter ff_vf_fps;
extern AVFilter ff_vf_framepack;
extern AVFilter ff_vf_framerate;
extern AVFilter ff_vf_framestep;
extern AVFilter ff_vf_frei0r;
extern AVFilter ff_vf_fspp;
extern AVFilter ff_vf_gblur;
extern AVFilter ff_vf_geq;
extern AVFilter ff_vf_gradfun;
extern AVFilter ff_vf_haldclut;
extern AVFilter ff_vf_hflip;
extern AVFilter ff_vf_histeq;
extern AVFilter ff_vf_histogram;
extern AVFilter ff_vf_hqdn3d;
extern AVFilter ff_vf_hqx;
extern AVFilter ff_vf_hstack;
extern AVFilter ff_vf_hue;
extern AVFilter ff_vf_hwdownload;
extern AVFilter ff_vf_hwmap;
extern AVFilter ff_vf_hwupload;
extern AVFilter ff_vf_hwupload_cuda;
extern AVFilter ff_vf_hysteresis;
extern AVFilter ff_vf_idet;
extern AVFilter ff_vf_il;
extern AVFilter ff_vf_inflate;
extern AVFilter ff_vf_interlace;
extern AVFilter ff_vf_interleave;
extern AVFilter ff_vf_kerndeint;
extern AVFilter ff_vf_lenscorrection;
extern AVFilter ff_vf_libvmaf;
extern AVFilter ff_vf_limiter;
extern AVFilter ff_vf_loop;
extern AVFilter ff_vf_lumakey;
extern AVFilter ff_vf_lut;
extern AVFilter ff_vf_lut2;
extern AVFilter ff_vf_lut3d;
extern AVFilter ff_vf_lutrgb;
extern AVFilter ff_vf_lutyuv;
extern AVFilter ff_vf_maskedclamp;
extern AVFilter ff_vf_maskedmerge;
extern AVFilter ff_vf_mcdeint;
extern AVFilter ff_vf_mergeplanes;
extern AVFilter ff_vf_mestimate;
extern AVFilter ff_vf_metadata;
extern AVFilter ff_vf_midequalizer;
extern AVFilter ff_vf_minterpolate;
extern AVFilter ff_vf_mix;
extern AVFilter ff_vf_mpdecimate;
extern AVFilter ff_vf_negate;
extern AVFilter ff_vf_nlmeans;
extern AVFilter ff_vf_nnedi;
extern AVFilter ff_vf_noformat;
extern AVFilter ff_vf_noise;
extern AVFilter ff_vf_normalize;
extern AVFilter ff_vf_null;
extern AVFilter ff_vf_ocr;
extern AVFilter ff_vf_ocv;
extern AVFilter ff_vf_oscilloscope;
extern AVFilter ff_vf_overlay;
extern AVFilter ff_vf_overlay_opencl;
extern AVFilter ff_vf_overlay_qsv;
extern AVFilter ff_vf_owdenoise;
extern AVFilter ff_vf_pad;
extern AVFilter ff_vf_palettegen;
extern AVFilter ff_vf_paletteuse;
extern AVFilter ff_vf_perms;
extern AVFilter ff_vf_perspective;
extern AVFilter ff_vf_phase;
extern AVFilter ff_vf_pixdesctest;
extern AVFilter ff_vf_pixscope;
extern AVFilter ff_vf_pp;
extern AVFilter ff_vf_pp7;
extern AVFilter ff_vf_premultiply;
extern AVFilter ff_vf_prewitt;
extern AVFilter ff_vf_procamp_vaapi;
extern AVFilter ff_vf_program_opencl;
extern AVFilter ff_vf_pseudocolor;
extern AVFilter ff_vf_psnr;
extern AVFilter ff_vf_pullup;
extern AVFilter ff_vf_qp;
extern AVFilter ff_vf_random;
extern AVFilter ff_vf_readeia608;
extern AVFilter ff_vf_readvitc;
extern AVFilter ff_vf_realtime;
extern AVFilter ff_vf_remap;
extern AVFilter ff_vf_removegrain;
extern AVFilter ff_vf_removelogo;
extern AVFilter ff_vf_repeatfields;
extern AVFilter ff_vf_reverse;
extern AVFilter ff_vf_roberts;
extern AVFilter ff_vf_rotate;
extern AVFilter ff_vf_sab;
extern AVFilter ff_vf_scale;
extern AVFilter ff_vf_scale_cuda;
extern AVFilter ff_vf_scale_npp;
extern AVFilter ff_vf_scale_qsv;
extern AVFilter ff_vf_scale_vaapi;
extern AVFilter ff_vf_scale2ref;
extern AVFilter ff_vf_select;
extern AVFilter ff_vf_selectivecolor;
extern AVFilter ff_vf_sendcmd;
extern AVFilter ff_vf_separatefields;
extern AVFilter ff_vf_setdar;
extern AVFilter ff_vf_setfield;
extern AVFilter ff_vf_setpts;
extern AVFilter ff_vf_setrange;
extern AVFilter ff_vf_setsar;
extern AVFilter ff_vf_settb;
extern AVFilter ff_vf_sharpness_vaapi;
extern AVFilter ff_vf_showinfo;
extern AVFilter ff_vf_showpalette;
extern AVFilter ff_vf_shuffleframes;
extern AVFilter ff_vf_shuffleplanes;
extern AVFilter ff_vf_sidedata;
extern AVFilter ff_vf_signalstats;
extern AVFilter ff_vf_signature;
extern AVFilter ff_vf_smartblur;
extern AVFilter ff_vf_sobel;
extern AVFilter ff_vf_split;
extern AVFilter ff_vf_spp;
extern AVFilter ff_vf_ssim;
extern AVFilter ff_vf_stereo3d;
extern AVFilter ff_vf_streamselect;
extern AVFilter ff_vf_subtitles;
extern AVFilter ff_vf_super2xsai;
extern AVFilter ff_vf_swaprect;
extern AVFilter ff_vf_swapuv;
extern AVFilter ff_vf_tblend;
extern AVFilter ff_vf_telecine;
extern AVFilter ff_vf_threshold;
extern AVFilter ff_vf_thumbnail;
extern AVFilter ff_vf_thumbnail_cuda;
extern AVFilter ff_vf_tile;
extern AVFilter ff_vf_tinterlace;
extern AVFilter ff_vf_tlut2;
extern AVFilter ff_vf_tonemap;
extern AVFilter ff_vf_transpose;
extern AVFilter ff_vf_trim;
extern AVFilter ff_vf_unpremultiply;
extern AVFilter ff_vf_unsharp;
extern AVFilter ff_vf_unsharp_opencl;
extern AVFilter ff_vf_uspp;
extern AVFilter ff_vf_vaguedenoiser;
extern AVFilter ff_vf_vectorscope;
extern AVFilter ff_vf_vflip;
extern AVFilter ff_vf_vfrdet;
extern AVFilter ff_vf_vidstabdetect;
extern AVFilter ff_vf_vidstabtransform;
extern AVFilter ff_vf_vignette;
extern AVFilter ff_vf_vmafmotion;
extern AVFilter ff_vf_vpp_qsv;
extern AVFilter ff_vf_vstack;
extern AVFilter ff_vf_w3fdif;
extern AVFilter ff_vf_waveform;
extern AVFilter ff_vf_weave;
extern AVFilter ff_vf_xbr;
extern AVFilter ff_vf_yadif;
extern AVFilter ff_vf_zmq;
extern AVFilter ff_vf_zoompan;
extern AVFilter ff_vf_zscale;

extern AVFilter ff_vsrc_allrgb;
extern AVFilter ff_vsrc_allyuv;
extern AVFilter ff_vsrc_cellauto;
extern AVFilter ff_vsrc_color;
extern AVFilter ff_vsrc_coreimagesrc;
extern AVFilter ff_vsrc_frei0r_src;
extern AVFilter ff_vsrc_haldclutsrc;
extern AVFilter ff_vsrc_life;
extern AVFilter ff_vsrc_mandelbrot;
extern AVFilter ff_vsrc_mptestsrc;
extern AVFilter ff_vsrc_nullsrc;
extern AVFilter ff_vsrc_openclsrc;
extern AVFilter ff_vsrc_rgbtestsrc;
extern AVFilter ff_vsrc_smptebars;
extern AVFilter ff_vsrc_smptehdbars;
extern AVFilter ff_vsrc_testsrc;
extern AVFilter ff_vsrc_testsrc2;
extern AVFilter ff_vsrc_yuvtestsrc;

extern AVFilter ff_vsink_nullsink;

/* multimedia filters */
extern AVFilter ff_avf_abitscope;
extern AVFilter ff_avf_adrawgraph;
extern AVFilter ff_avf_ahistogram;
extern AVFilter ff_avf_aphasemeter;
extern AVFilter ff_avf_avectorscope;
extern AVFilter ff_avf_concat;
extern AVFilter ff_avf_showcqt;
extern AVFilter ff_avf_showfreqs;
extern AVFilter ff_avf_showspectrum;
extern AVFilter ff_avf_showspectrumpic;
extern AVFilter ff_avf_showvolume;
extern AVFilter ff_avf_showwaves;
extern AVFilter ff_avf_showwavespic;
extern AVFilter ff_vaf_spectrumsynth;

/* multimedia sources */
extern AVFilter ff_avsrc_amovie;
extern AVFilter ff_avsrc_movie;

/* those filters are part of public or internal API,
 * they are formatted to not be found by the grep
 * as they are manually added again (due to their 'names'
 * being the same while having different 'types'). */
extern  AVFilter ff_asrc_abuffer;
extern  AVFilter ff_vsrc_buffer;
extern  AVFilter ff_asink_abuffer;
extern  AVFilter ff_vsink_buffer;
extern AVFilter ff_af_afifo;
extern AVFilter ff_vf_fifo;

/**
static const AVFilter * const filter_list[] = {
    &ff_asrc_abuffer,
    &ff_vsrc_buffer,
    &ff_asink_abuffer,
    &ff_vsink_buffer,
    NULL };
*/

static const AVFilter * const filter_list[] = {
    &ff_af_abench,
    &ff_af_acompressor,
    &ff_af_acontrast,
    &ff_af_acopy,
   // &ff_af_acue,
    &ff_af_acrossfade,
   // &ff_af_acrossover,
    &ff_af_acrusher,
  //  &ff_af_adeclick,
  //  &ff_af_adeclip,
    &ff_af_adelay,
   // &ff_af_aderivative,
    &ff_af_aecho,
    &ff_af_aemphasis,
    &ff_af_aeval,
    &ff_af_afade,
  //  &ff_af_afftdn,
    &ff_af_afftfilt,
    &ff_af_afir,
    &ff_af_aformat,
    &ff_af_agate,
    &ff_af_aiir,
  //  &ff_af_aintegral,
    &ff_af_ainterleave,
    &ff_af_alimiter,
    &ff_af_allpass,
    &ff_af_aloop,
    &ff_af_amerge,
    &ff_af_ametadata,
    &ff_af_amix,
   // &ff_af_amultiply,
    &ff_af_anequalizer,
  //  &ff_af_anlmdn,
    &ff_af_anull,
    &ff_af_apad,
    &ff_af_aperms,
    &ff_af_aphaser,
    &ff_af_apulsator,
    &ff_af_arealtime,
    &ff_af_aresample,
    &ff_af_areverse,
    &ff_af_aselect,
    &ff_af_asendcmd,
    &ff_af_asetnsamples,
    &ff_af_asetpts,
    &ff_af_asetrate,
    &ff_af_asettb,
    &ff_af_ashowinfo,
    &ff_af_asidedata,
    &ff_af_asplit,
    &ff_af_astats,
    &ff_af_astreamselect,
    &ff_af_atempo,
    &ff_af_atrim,
    &ff_af_bandpass,
    &ff_af_bandreject,
    &ff_af_bass,
    &ff_af_biquad,
    &ff_af_channelmap,
    &ff_af_channelsplit,
    &ff_af_chorus,
    &ff_af_compand,
    &ff_af_compensationdelay,
    &ff_af_crossfeed,
    &ff_af_crystalizer,
    &ff_af_dcshift,
    &ff_af_drmeter,
    &ff_af_dynaudnorm,
    &ff_af_earwax,
    &ff_af_ebur128,
    &ff_af_equalizer,
    &ff_af_extrastereo,
    &ff_af_firequalizer,
    &ff_af_flanger,
    &ff_af_haas,
    &ff_af_hdcd,
    &ff_af_headphone,
    &ff_af_highpass,
  //  &ff_af_highshelf,
    &ff_af_join,
    &ff_af_loudnorm,
    &ff_af_lowpass,
   // &ff_af_lowshelf,
    &ff_af_mcompand,
    &ff_af_pan,
    &ff_af_replaygain,
    &ff_af_sidechaincompress,
    &ff_af_sidechaingate,
    &ff_af_silencedetect,
    &ff_af_silenceremove,
    &ff_af_stereotools,
    &ff_af_stereowiden,
    &ff_af_superequalizer,
    &ff_af_surround,
    &ff_af_treble,
    &ff_af_tremolo,
    &ff_af_vibrato,
    &ff_af_volume,
    &ff_af_volumedetect,
    &ff_asrc_aevalsrc,
    &ff_asrc_anoisesrc,
    &ff_asrc_anullsrc,
    &ff_asrc_hilbert,
   // &ff_asrc_sinc,
    &ff_asrc_sine,
    &ff_asink_anullsink,
    &ff_vf_alphaextract,
    &ff_vf_alphamerge,
  //  &ff_vf_amplify,
    &ff_vf_atadenoise,
    &ff_vf_avgblur,
    &ff_vf_bbox,
    &ff_vf_bench,
    &ff_vf_bitplanenoise,
    &ff_vf_blackdetect,
    &ff_vf_blend,
   // &ff_vf_bm3d,
    &ff_vf_bwdif,
   // &ff_vf_chromahold,
    &ff_vf_chromakey,
    //&ff_vf_chromashift,
    &ff_vf_ciescope,
    &ff_vf_codecview,
    &ff_vf_colorbalance,
    &ff_vf_colorchannelmixer,
    &ff_vf_colorkey,
    &ff_vf_colorlevels,
    &ff_vf_colorspace,
    &ff_vf_convolution,
    &ff_vf_convolve,
    &ff_vf_copy,
    &ff_vf_crop,
    //&ff_vf_cue,
    &ff_vf_curves,
    &ff_vf_datascope,
    &ff_vf_dctdnoiz,
    &ff_vf_deband,
   // &ff_vf_deblock,
    &ff_vf_decimate,
    &ff_vf_deconvolve,
    //&ff_vf_dedot,
    &ff_vf_deflate,
    &ff_vf_deflicker,
    &ff_vf_dejudder,
    &ff_vf_deshake,
    &ff_vf_despill,
    &ff_vf_detelecine,
    &ff_vf_dilation,
    &ff_vf_displace,
    &ff_vf_doubleweave,
    &ff_vf_drawbox,
    &ff_vf_drawgraph,
    &ff_vf_drawgrid,
    &ff_vf_edgedetect,
    &ff_vf_elbg,
    &ff_vf_entropy,
    &ff_vf_erosion,
    &ff_vf_extractplanes,
    &ff_vf_fade,
    ///&ff_vf_fftdnoiz,
    &ff_vf_fftfilt,
    &ff_vf_field,
    &ff_vf_fieldhint,
    &ff_vf_fieldmatch,
    &ff_vf_fieldorder,
    &ff_vf_fillborders,
    &ff_vf_floodfill,
    &ff_vf_format,
    &ff_vf_fps,
    &ff_vf_framepack,
    &ff_vf_framerate,
    &ff_vf_framestep,
  //  &ff_vf_freezedetect,
    &ff_vf_gblur,
    &ff_vf_gradfun,
   // &ff_vf_graphmonitor,
   // &ff_vf_greyedge,
    &ff_vf_haldclut,
    &ff_vf_hflip,
    &ff_vf_histogram,
    &ff_vf_hqx,
    &ff_vf_hstack,
    &ff_vf_hue,
    &ff_vf_hwdownload,
    &ff_vf_hwmap,
    &ff_vf_hwupload,
    &ff_vf_hysteresis,
    &ff_vf_idet,
    &ff_vf_il,
    &ff_vf_inflate,
    &ff_vf_interleave,
    &ff_vf_lenscorrection,
    &ff_vf_limiter,
    &ff_vf_loop,
    &ff_vf_lumakey,
    &ff_vf_lut,
   // &ff_vf_lut1d,
    &ff_vf_lut2,
    &ff_vf_lut3d,
    &ff_vf_lutrgb,
    &ff_vf_lutyuv,
    &ff_vf_maskedclamp,
    &ff_vf_maskedmerge,
    //&ff_vf_maskfun,
    &ff_vf_mergeplanes,
    &ff_vf_mestimate,
    &ff_vf_metadata,
    &ff_vf_midequalizer,
    &ff_vf_minterpolate,
    &ff_vf_mix,
    &ff_vf_negate,
    &ff_vf_nlmeans,
    &ff_vf_noformat,
    &ff_vf_noise,
    &ff_vf_normalize,
    &ff_vf_null,
    &ff_vf_oscilloscope,
    &ff_vf_overlay,
    &ff_vf_pad,
    &ff_vf_palettegen,
    &ff_vf_paletteuse,
    &ff_vf_perms,
    &ff_vf_pixdesctest,
    &ff_vf_pixscope,
    &ff_vf_premultiply,
    &ff_vf_prewitt,
    &ff_vf_pseudocolor,
    &ff_vf_psnr,
    &ff_vf_qp,
    &ff_vf_random,
    &ff_vf_readeia608,
    &ff_vf_readvitc,
    &ff_vf_realtime,
    &ff_vf_remap,
    &ff_vf_removegrain,
    &ff_vf_removelogo,
    &ff_vf_reverse,
    //&ff_vf_rgbashift,
    &ff_vf_roberts,
    &ff_vf_rotate,
    &ff_vf_scale,
    &ff_vf_scale2ref,
    &ff_vf_select,
    &ff_vf_selectivecolor,
    &ff_vf_sendcmd,
    &ff_vf_separatefields,
    &ff_vf_setdar,
    &ff_vf_setfield,
    //&ff_vf_setparams,
    &ff_vf_setpts,
    &ff_vf_setrange,
    &ff_vf_setsar,
    &ff_vf_settb,
    &ff_vf_showinfo,
    &ff_vf_showpalette,
    &ff_vf_shuffleframes,
    &ff_vf_shuffleplanes,
    &ff_vf_sidedata,
    &ff_vf_signalstats,
    &ff_vf_sobel,
    &ff_vf_split,
 //   &ff_vf_sr,
    &ff_vf_ssim,
    &ff_vf_streamselect,
    &ff_vf_swaprect,
    &ff_vf_swapuv,
    &ff_vf_tblend,
    &ff_vf_telecine,
    &ff_vf_threshold,
    &ff_vf_thumbnail,
    &ff_vf_tile,
    &ff_vf_tlut2,
   // &ff_vf_tmix,
    &ff_vf_tonemap,
   // &ff_vf_tpad,
    &ff_vf_transpose,
    &ff_vf_trim,
    &ff_vf_unpremultiply,
    &ff_vf_unsharp,
    &ff_vf_vectorscope,
    &ff_vf_vflip,
    &ff_vf_vfrdet,
  //  &ff_vf_vibrance,
    &ff_vf_vignette,
    &ff_vf_vmafmotion,
    &ff_vf_vstack,
    &ff_vf_w3fdif,
    &ff_vf_waveform,
    &ff_vf_weave,
    &ff_vf_xbr,
   // &ff_vf_xstack,
    &ff_vf_yadif,
    &ff_vf_zoompan,
    &ff_vsrc_allrgb,
    &ff_vsrc_allyuv,
    &ff_vsrc_cellauto,
    &ff_vsrc_color,
    &ff_vsrc_haldclutsrc,
    &ff_vsrc_life,
    &ff_vsrc_mandelbrot,
    &ff_vsrc_nullsrc,
   // &ff_vsrc_pal75bars,
   // &ff_vsrc_pal100bars,
    &ff_vsrc_rgbtestsrc,
    &ff_vsrc_smptebars,
    &ff_vsrc_smptehdbars,
    &ff_vsrc_testsrc,
    &ff_vsrc_testsrc2,
    &ff_vsrc_yuvtestsrc,
    &ff_vsink_nullsink,
    &ff_avf_abitscope,
    &ff_avf_adrawgraph,
   // &ff_avf_agraphmonitor,
    &ff_avf_ahistogram,
    &ff_avf_aphasemeter,
    &ff_avf_avectorscope,
    &ff_avf_concat,
    &ff_avf_showcqt,
    &ff_avf_showfreqs,
    &ff_avf_showspectrum,
    &ff_avf_showspectrumpic,
    &ff_avf_showvolume,
    &ff_avf_showwaves,
    &ff_avf_showwavespic,
    &ff_vaf_spectrumsynth,
    &ff_avsrc_amovie,
    &ff_avsrc_movie,
    &ff_af_afifo,
    &ff_vf_fifo,
    &ff_asrc_abuffer,
    &ff_vsrc_buffer,
    &ff_asink_abuffer,
    &ff_vsink_buffer,
    NULL };



const AVFilter *av_filter_iterate(void **opaque)
{
    uintptr_t i = (uintptr_t)*opaque;
    const AVFilter *f = filter_list[i];

    if (f)
        *opaque = (void*)(i + 1);

    return f;
}

const AVFilter *avfilter_get_by_name(const char *name)
{
    const AVFilter *f = NULL;
    void *opaque = 0;

    if (!name)
        return NULL;

    while ((f = av_filter_iterate(&opaque)))
        if (!strcmp(f->name, name))
            return (AVFilter *)f;

    return NULL;
}


#if FF_API_NEXT
FF_DISABLE_DEPRECATION_WARNINGS
static AVOnce av_filter_next_init = AV_ONCE_INIT;

static void av_filter_init_next(void)
{
    AVFilter *prev = NULL, *p;
    void *i = 0;
    while ((p = (AVFilter*)av_filter_iterate(&i))) {
        if (prev)
            prev->next = p;
        prev = p;
    }
}

void avfilter_register_all(void)
{
    ff_thread_once(&av_filter_next_init, av_filter_init_next);
}

int avfilter_register(AVFilter *filter)
{
    ff_thread_once(&av_filter_next_init, av_filter_init_next);

    return 0;
}

const AVFilter *avfilter_next(const AVFilter *prev)
{
    ff_thread_once(&av_filter_next_init, av_filter_init_next);

    return prev ? prev->next : filter_list[0];
}

FF_ENABLE_DEPRECATION_WARNINGS
#endif
