PREFIX ?=/usr/local
SYSTEM ?= LINUX

AS=yasm
CC=gcc
AR=ar rcs
RANLIB=ranlib
STRIP=strip
RM=rm -rf

OBJS += CompressPNG8.o CompressSize.o CompressQuality.o WXPNG.o  WXJPG.o 

OBJS += pngquant/libimagequant.o 

OBJS += FreeImage/Background.o  FreeImage/BitmapAccess.o  FreeImage/BSplineRotate.o  FreeImage/CacheFile.o  FreeImage/Channels.o  FreeImage/ClassicRotate.o  FreeImage/ColorLookup.o  FreeImage/Colors.o  FreeImage/Conversion.o  FreeImage/Conversion16_555.o  FreeImage/Conversion16_565.o  FreeImage/Conversion24.o  FreeImage/Conversion32.o  FreeImage/Conversion4.o  FreeImage/Conversion8.o  FreeImage/ConversionFloat.o  FreeImage/ConversionRGB16.o  FreeImage/ConversionRGBA16.o  FreeImage/ConversionRGBAF.o  FreeImage/ConversionRGBF.o  FreeImage/ConversionType.o  FreeImage/ConversionUINT16.o  FreeImage/CopyPaste.o  FreeImage/Display.o  FreeImage/Exif.o  FreeImage/FIRational.o  FreeImage/Flip.o  FreeImage/FreeImage.o  FreeImage/FreeImageIO.o  FreeImage/FreeImageTag.o  FreeImage/GetType.o  FreeImage/Halftoning.o  FreeImage/IPTC.o  FreeImage/J2KHelper.o  FreeImage/JPEGTransform.o  FreeImage/LFPQuantizer.o  FreeImage/MemoryIO.o  FreeImage/MNGHelper.o  FreeImage/MultigridPoissonSolver.o  FreeImage/MultiPage.o  FreeImage/NNQuantizer.o  FreeImage/PixelAccess.o  FreeImage/Plugin.o  FreeImage/PluginBMP.o  FreeImage/PluginCUT.o  FreeImage/PluginDDS.o  FreeImage/PluginEXR.o  FreeImage/PluginG3.o  FreeImage/PluginGIF.o  FreeImage/PluginHDR.o  FreeImage/PluginICO.o  FreeImage/PluginIFF.o  FreeImage/PluginJ2K.o  FreeImage/PluginJNG.o  FreeImage/PluginJP2.o  FreeImage/PluginJPEG.o  FreeImage/PluginKOALA.o  FreeImage/PluginMNG.o  FreeImage/PluginPCD.o  FreeImage/PluginPCX.o  FreeImage/PluginPFM.o  FreeImage/PluginPICT.o  FreeImage/PluginPNG.o  FreeImage/PluginPNM.o  FreeImage/PluginPSD.o  FreeImage/PluginRAS.o  FreeImage/PluginRAW.o  FreeImage/PluginSGI.o  FreeImage/PluginTARGA.o  FreeImage/PluginTIFF.o  FreeImage/PluginWBMP.o  FreeImage/PluginWebP.o  FreeImage/PluginXBM.o  FreeImage/PluginXPM.o  FreeImage/PSDParser.o  FreeImage/Rescale.o  FreeImage/Resize.o  FreeImage/TagConversion.o  FreeImage/TagLib.o  FreeImage/TIFFLogLuv.o  FreeImage/tmoColorConvert.o  FreeImage/tmoDrago03.o  FreeImage/tmoFattal02.o  FreeImage/tmoReinhard05.o  FreeImage/ToneMapping.o  FreeImage/WuQuantizer.o  FreeImage/XTIFF.o  FreeImage/ZLibInterface.o   FreeImage/FreeImageC.o

OBJS += libyuv/compare.o  libyuv/compare_common.o  libyuv/compare_gcc.o  libyuv/compare_neon.o  libyuv/compare_neon64.o  libyuv/compare_win.o  libyuv/convert.o  libyuv/convert_argb.o  libyuv/convert_from.o  libyuv/convert_from_argb.o  libyuv/convert_to_argb.o  libyuv/convert_to_i420.o  libyuv/cpu_id.o  libyuv/planar_functions.o  libyuv/rotate.o  libyuv/rotate_argb.o  libyuv/rotate_neon.o  libyuv/rotate_neon64.o  libyuv/row_any.o  libyuv/row_common.o  libyuv/row_gcc.o  libyuv/row_neon.o  libyuv/row_neon64.o  libyuv/row_win.o  libyuv/scale.o  libyuv/scale_any.o  libyuv/scale_argb.o  libyuv/scale_common.o  libyuv/scale_gcc.o  libyuv/scale_neon.o  libyuv/scale_neon64.o  libyuv/scale_win.o  libyuv/video_common.o 

OBJS += LibRawLite/dcraw_common.o  LibRawLite/dcraw_fileio.o  LibRawLite/demosaic_packs.o  LibRawLite/libraw_c_api.o  LibRawLite/libraw_cxx.o  LibRawLite/libraw_datastream.o  
 
OBJS += OpenEXR/half.o  OpenEXR/IexBaseExc.o  OpenEXR/IexMathFloatExc.o  OpenEXR/IexMathFpu.o  OpenEXR/IexThrowErrnoExc.o  OpenEXR/IlmThreadMutexPosix.o  OpenEXR/IlmThreadMutexWin32.o  OpenEXR/IlmThreadPool.o  OpenEXR/IlmThreadPosix.o  OpenEXR/IlmThreadSemaphorePosix.o  OpenEXR/IlmThreadSemaphorePosixCompat.o  OpenEXR/IlmThreadSemaphoreWin32.o  OpenEXR/IlmThreadWin32.o  OpenEXR/ImathBox.o  OpenEXR/ImathColorAlgo.o  OpenEXR/ImathFun.o  OpenEXR/ImathMatrixAlgo.o  OpenEXR/ImathRandom.o  OpenEXR/ImathShear.o  OpenEXR/ImathVec.o  OpenEXR/ImfAcesFile.o  OpenEXR/ImfAttribute.o  OpenEXR/ImfB44Compressor.o  OpenEXR/ImfBoxAttribute.o  OpenEXR/ImfChannelList.o  OpenEXR/ImfChannelListAttribute.o  OpenEXR/ImfChromaticities.o  OpenEXR/ImfChromaticitiesAttribute.o  OpenEXR/ImfCompositeDeepScanLine.o  OpenEXR/ImfCompressionAttribute.o  OpenEXR/ImfCompressor.o  OpenEXR/ImfConvert.o  OpenEXR/ImfCRgbaFile.o  OpenEXR/ImfDeepCompositing.o  OpenEXR/ImfDeepFrameBuffer.o  OpenEXR/ImfDeepImageStateAttribute.o  OpenEXR/ImfDeepScanLineInputFile.o  OpenEXR/ImfDeepScanLineInputPart.o  OpenEXR/ImfDeepScanLineOutputFile.o  OpenEXR/ImfDeepScanLineOutputPart.o  OpenEXR/ImfDeepTiledInputFile.o  OpenEXR/ImfDeepTiledInputPart.o  OpenEXR/ImfDeepTiledOutputFile.o  OpenEXR/ImfDeepTiledOutputPart.o  OpenEXR/ImfDoubleAttribute.o  OpenEXR/ImfDwaCompressor.o  OpenEXR/ImfEnvmap.o  OpenEXR/ImfEnvmapAttribute.o  OpenEXR/ImfFastHuf.o  OpenEXR/ImfFloatAttribute.o  OpenEXR/ImfFloatVectorAttribute.o  OpenEXR/ImfFrameBuffer.o  OpenEXR/ImfFramesPerSecond.o  OpenEXR/ImfGenericInputFile.o  OpenEXR/ImfGenericOutputFile.o  OpenEXR/ImfHeader.o  OpenEXR/ImfHuf.o  OpenEXR/ImfInputFile.o  OpenEXR/ImfInputPart.o  OpenEXR/ImfInputPartData.o  OpenEXR/ImfIntAttribute.o  OpenEXR/ImfIO.o  OpenEXR/ImfKeyCode.o  OpenEXR/ImfKeyCodeAttribute.o  OpenEXR/ImfLineOrderAttribute.o  OpenEXR/ImfLut.o  OpenEXR/ImfMatrixAttribute.o  OpenEXR/ImfMisc.o  OpenEXR/ImfMultiPartInputFile.o  OpenEXR/ImfMultiPartOutputFile.o  OpenEXR/ImfMultiView.o  OpenEXR/ImfOpaqueAttribute.o  OpenEXR/ImfOutputFile.o  OpenEXR/ImfOutputPart.o  OpenEXR/ImfOutputPartData.o  OpenEXR/ImfPartType.o  OpenEXR/ImfPizCompressor.o  OpenEXR/ImfPreviewImage.o  OpenEXR/ImfPreviewImageAttribute.o  OpenEXR/ImfPxr24Compressor.o  OpenEXR/ImfRational.o  OpenEXR/ImfRationalAttribute.o  OpenEXR/ImfRgbaFile.o  OpenEXR/ImfRgbaYca.o  OpenEXR/ImfRle.o  OpenEXR/ImfRleCompressor.o  OpenEXR/ImfScanLineInputFile.o  OpenEXR/ImfStandardAttributes.o  OpenEXR/ImfStdIO.o  OpenEXR/ImfStringAttribute.o  OpenEXR/ImfStringVectorAttribute.o  OpenEXR/ImfSystemSpecific.o  OpenEXR/ImfTestFile.o  OpenEXR/ImfThreading.o  OpenEXR/ImfTileDescriptionAttribute.o  OpenEXR/ImfTiledInputFile.o  OpenEXR/ImfTiledInputPart.o  OpenEXR/ImfTiledMisc.o  OpenEXR/ImfTiledOutputFile.o  OpenEXR/ImfTiledOutputPart.o  OpenEXR/ImfTiledRgbaFile.o  OpenEXR/ImfTileOffsets.o  OpenEXR/ImfTimeCode.o  OpenEXR/ImfTimeCodeAttribute.o  OpenEXR/ImfVecAttribute.o  OpenEXR/ImfVersion.o  OpenEXR/ImfWav.o  OpenEXR/ImfZip.o  OpenEXR/ImfZipCompressor.o  
 
OBJS += libjpeg/cdjpeg.o  libjpeg/jaricom.o  libjpeg/jcapimin.o  libjpeg/jcapistd.o  libjpeg/jcarith.o  libjpeg/jccoefct.o  libjpeg/jccolor.o  libjpeg/jcdctmgr.o  libjpeg/jchuff.o  libjpeg/jcicc.o  libjpeg/jcinit.o  libjpeg/jcmainct.o  libjpeg/jcmarker.o  libjpeg/jcmaster.o  libjpeg/jcomapi.o  libjpeg/jcparam.o  libjpeg/jcphuff.o  libjpeg/jcprepct.o  libjpeg/jcsample.o  libjpeg/jctrans.o  libjpeg/jdapimin.o  libjpeg/jdapistd.o  libjpeg/jdarith.o  libjpeg/jdatadst.o  libjpeg/jdatasrc.o  libjpeg/jdcoefct.o  libjpeg/jdcolor.o  libjpeg/jddctmgr.o  libjpeg/jdhuff.o  libjpeg/jdicc.o  libjpeg/jdinput.o  libjpeg/jdmainct.o  libjpeg/jdmarker.o  libjpeg/jdmaster.o  libjpeg/jdmerge.o  libjpeg/jdphuff.o  libjpeg/jdpostct.o  libjpeg/jdsample.o  libjpeg/jdtrans.o  libjpeg/jerror.o  libjpeg/jfdctflt.o  libjpeg/jfdctfst.o  libjpeg/jfdctint.o  libjpeg/jidctflt.o  libjpeg/jidctfst.o  libjpeg/jidctint.o  libjpeg/jidctred.o  libjpeg/jmemmgr.o  libjpeg/jmemnobs.o  libjpeg/jquant1.o  libjpeg/jquant2.o  libjpeg/jutils.o  libjpeg/rdcolmap.o  libjpeg/rdswitch.o  libjpeg/transupp.o  

OBJS += libjpeg/simd/x86_64/jsimd_x86_64.o  

OBJS32 += libjpeg/simd/i386/jsimd_i386.o

OBJS += LibOpenJPEG/bio.o  LibOpenJPEG/cidx_manager.o  LibOpenJPEG/cio.o  LibOpenJPEG/dwt.o  LibOpenJPEG/event.o  LibOpenJPEG/function_list.o  LibOpenJPEG/image.o  LibOpenJPEG/invert.o  LibOpenJPEG/j2k.o  LibOpenJPEG/jp2.o  LibOpenJPEG/mct.o  LibOpenJPEG/mqc.o  LibOpenJPEG/openjpeg.o  LibOpenJPEG/opj_clock.o  LibOpenJPEG/phix_manager.o  LibOpenJPEG/pi.o  LibOpenJPEG/ppix_manager.o  LibOpenJPEG/raw.o  LibOpenJPEG/t1.o  LibOpenJPEG/t2.o  LibOpenJPEG/tcd.o  LibOpenJPEG/tgt.o  LibOpenJPEG/thix_manager.o  LibOpenJPEG/tpix_manager.o  
 
OBJS += LibTIFF4/tif_aux.o  LibTIFF4/tif_close.o  LibTIFF4/tif_codec.o  LibTIFF4/tif_color.o  LibTIFF4/tif_compress.o  LibTIFF4/tif_dir.o  LibTIFF4/tif_dirinfo.o  LibTIFF4/tif_dirread.o  LibTIFF4/tif_dirwrite.o  LibTIFF4/tif_dumpmode.o  LibTIFF4/tif_error.o  LibTIFF4/tif_extension.o  LibTIFF4/tif_fax3.o  LibTIFF4/tif_fax3sm.o  LibTIFF4/tif_flush.o  LibTIFF4/tif_getimage.o  LibTIFF4/tif_jbig.o  LibTIFF4/tif_jpeg.o  LibTIFF4/tif_jpeg_12.o  LibTIFF4/tif_luv.o  LibTIFF4/tif_lzma.o  LibTIFF4/tif_lzw.o  LibTIFF4/tif_next.o  LibTIFF4/tif_ojpeg.o  LibTIFF4/tif_open.o  LibTIFF4/tif_packbits.o  LibTIFF4/tif_pixarlog.o  LibTIFF4/tif_predict.o  LibTIFF4/tif_print.o  LibTIFF4/tif_read.o  LibTIFF4/tif_strip.o  LibTIFF4/tif_swab.o  LibTIFF4/tif_thunder.o  LibTIFF4/tif_tile.o  LibTIFF4/tif_unix.o  LibTIFF4/tif_version.o  LibTIFF4/tif_warning.o   LibTIFF4/tif_write.o  LibTIFF4/tif_zip.o  LibTIFF4/tif_zstd.o  
 
OBJS += libwebp/alpha_dec.o  libwebp/alpha_enc.o  libwebp/alpha_processing.o  libwebp/alpha_processing_mips_dsp_r2.o  libwebp/alpha_processing_neon.o  libwebp/alpha_processing_sse2.o  libwebp/alpha_processing_sse41.o  libwebp/analysis_enc.o  libwebp/anim_decode.o  libwebp/anim_encode.o  libwebp/backward_references_cost_enc.o  libwebp/backward_references_enc.o  libwebp/bit_reader_utils.o  libwebp/bit_writer_utils.o  libwebp/buffer_dec.o  libwebp/color_cache_utils.o  libwebp/config_enc.o  libwebp/cost.o  libwebp/cost_enc.o  libwebp/cost_mips_dsp_r2.o  libwebp/cost_mips32.o  libwebp/cost_neon.o  libwebp/cost_sse2.o  libwebp/cpu.o  libwebp/dec.o  libwebp/dec_clip_tables.o  libwebp/dec_mips_dsp_r2.o  libwebp/dec_mips32.o  libwebp/dec_msa.o  libwebp/dec_neon.o  libwebp/dec_sse2.o  libwebp/dec_sse41.o  libwebp/demux.o  libwebp/enc.o  libwebp/enc_mips_dsp_r2.o  libwebp/enc_mips32.o  libwebp/enc_msa.o  libwebp/enc_neon.o  libwebp/enc_sse2.o  libwebp/enc_sse41.o  libwebp/filter_enc.o  libwebp/filters.o  libwebp/filters_mips_dsp_r2.o  libwebp/filters_msa.o  libwebp/filters_neon.o  libwebp/filters_sse2.o  libwebp/filters_utils.o  libwebp/frame_dec.o  libwebp/frame_enc.o  libwebp/histogram_enc.o  libwebp/huffman_encode_utils.o  libwebp/huffman_utils.o  libwebp/idec_dec.o  libwebp/io_dec.o  libwebp/iterator_enc.o  libwebp/lossless.o  libwebp/lossless_enc.o  libwebp/lossless_enc_mips_dsp_r2.o  libwebp/lossless_enc_mips32.o  libwebp/lossless_enc_msa.o  libwebp/lossless_enc_neon.o  libwebp/lossless_enc_sse2.o  libwebp/lossless_enc_sse41.o  libwebp/lossless_mips_dsp_r2.o  libwebp/lossless_msa.o  libwebp/lossless_neon.o  libwebp/lossless_sse2.o  libwebp/lossless_sse41.o  libwebp/muxedit.o  libwebp/muxinternal.o  libwebp/muxread.o  libwebp/near_lossless_enc.o  libwebp/picture_csp_enc.o  libwebp/picture_enc.o  libwebp/picture_psnr_enc.o  libwebp/picture_rescale_enc.o  libwebp/picture_tools_enc.o  libwebp/predictor_enc.o  libwebp/quant_dec.o  libwebp/quant_enc.o  libwebp/quant_levels_dec_utils.o  libwebp/quant_levels_utils.o  libwebp/random_utils.o  libwebp/rescaler.o  libwebp/rescaler_mips_dsp_r2.o  libwebp/rescaler_mips32.o  libwebp/rescaler_msa.o  libwebp/rescaler_neon.o  libwebp/rescaler_sse2.o  libwebp/rescaler_utils.o  libwebp/ssim.o  libwebp/ssim_sse2.o  libwebp/syntax_enc.o  libwebp/thread_utils.o  libwebp/token_enc.o  libwebp/tree_dec.o  libwebp/tree_enc.o  libwebp/upsampling.o  libwebp/upsampling_mips_dsp_r2.o  libwebp/upsampling_msa.o  libwebp/upsampling_neon.o  libwebp/upsampling_sse2.o  libwebp/upsampling_sse41.o  libwebp/utils.o  libwebp/vp8_dec.o  libwebp/vp8l_dec.o  libwebp/vp8l_enc.o  libwebp/webp_dec.o  libwebp/webp_enc.o  libwebp/yuv.o  libwebp/yuv_mips_dsp_r2.o  libwebp/yuv_mips32.o  libwebp/yuv_neon.o  libwebp/yuv_sse2.o  libwebp/yuv_sse41.o  

OBJSASM := libjpeg/simd/x86_64/jccolor-avx2.o libjpeg/simd/x86_64/jccolor-sse2.o libjpeg/simd/x86_64/jcgray-avx2.o libjpeg/simd/x86_64/jcgray-sse2.o libjpeg/simd/x86_64/jchuff-sse2.o libjpeg/simd/x86_64/jcphuff-sse2.o libjpeg/simd/x86_64/jcsample-avx2.o libjpeg/simd/x86_64/jcsample-sse2.o libjpeg/simd/x86_64/jdcolor-avx2.o libjpeg/simd/x86_64/jdcolor-sse2.o libjpeg/simd/x86_64/jdmerge-avx2.o libjpeg/simd/x86_64/jdmerge-sse2.o libjpeg/simd/x86_64/jdsample-avx2.o libjpeg/simd/x86_64/jdsample-sse2.o libjpeg/simd/x86_64/jfdctflt-sse.o libjpeg/simd/x86_64/jfdctfst-sse2.o libjpeg/simd/x86_64/jfdctint-avx2.o libjpeg/simd/x86_64/jfdctint-sse2.o libjpeg/simd/x86_64/jidctflt-sse2.o libjpeg/simd/x86_64/jidctfst-sse2.o libjpeg/simd/x86_64/jidctint-avx2.o libjpeg/simd/x86_64/jidctint-sse2.o libjpeg/simd/x86_64/jidctred-sse2.o libjpeg/simd/x86_64/jquantf-sse2.o libjpeg/simd/x86_64/jquanti-avx2.o libjpeg/simd/x86_64/jquanti-sse2.o libjpeg/simd/x86_64/jsimdcpu.o 



ifeq ($(SYSTEM), WINDOWS) 
OBJS +=  FreeImage/PluginJXR.o  LibJXR/adapthuff.o  LibJXR/JXRdecode.o  LibJXR/JXRencode.o  LibJXR/JXRGlue.o  LibJXR/JXRGlueJxr.o  LibJXR/JXRGluePFC.o  LibJXR/JXRimage.o  LibJXR/JXRMeta.o  LibJXR/JXRTranscode.o  LibJXR/perfTimerANSI.o  LibJXR/postprocess.o  LibJXR/segdec.o  LibJXR/segenc.o  LibJXR/strcodec.o  LibJXR/strdec.o  LibJXR/strdec_x86.o  LibJXR/strenc.o  LibJXR/strenc_x86.o  LibJXR/strFwdTransform.o  LibJXR/strInvTransform.o  LibJXR/strPredQuant.o  LibJXR/strPredQuantDec.o  LibJXR/strPredQuantEnc.o  LibJXR/strTransform.o  
ASFLAGS    := -fwin64 -DWIN64 -D__x86_64__  -I./libjpeg/simd/nasm/   -I./libjpeg/simd/x86_64/ 
CFLAGS     := -I.  -I./libjpeg   -I./zlib  -c  -Wall -std=c99 -fPIC -Winline -O2 -D_FILE_OFFSET_BITS=64 -DWXPNG_EXPORTS  -mavx -msse2 -msse4.1 -mpclmul -fvisibility=hidden
CPPFLAGS   := -I. -I./libjpeg  -I./zlib  -c  -Wall -fPIC -Winline -O2 -D_FILE_OFFSET_BITS=64   -DWXPNG_EXPORTS     -mavx -msse2 -msse4.1 -mpclmul  -fvisibility=hidden
CXXFLAGS   := -I. -I./libjpeg  -I./zlib  -c  -Wall -fPIC -Winline -O2 -D_FILE_OFFSET_BITS=64   -DWXPNG_EXPORTS   -mavx -msse2 -msse4.1 -mpclmul -fvisibility=hidden
DYLIB_LIB  := WX_Image.dll
DEPEND_LIB := -lm  -lws2_32 -lstdc++
else 

ASFLAGS    := -f elf64 -D__x86_64__  -DELF -DPIC -I./libjpeg/simd/nasm/   -I./libjpeg/simd/x86_64/  

CFLAGS     := -I.  -I./libjpeg     -I./zlib  -c  -Wall -std=c99 -fPIC -Winline -O2 -D_FILE_OFFSET_BITS=64 -DWXPNG_EXPORTS  -mavx -msse2 -msse4.1 -mpclmul -fvisibility=hidden
CPPFLAGS   := -I. -I./libjpeg  -I./zlib  -c  -Wall -fPIC -Winline -O2 -D_FILE_OFFSET_BITS=64   -DWXPNG_EXPORTS   -mavx -msse2 -msse4.1 -mpclmul  -fvisibility=hidden
CXXFLAGS   := -I. -I./libjpeg  -I./zlib  -c  -Wall -fPIC -Winline -O2 -D_FILE_OFFSET_BITS=64   -DWXPNG_EXPORTS -mavx -msse2 -msse4.1 -mpclmul -fvisibility=hidden
DYLIB_LIB  := WX_Image.so
DEPEND_LIB := -lm   -lstdc++
endif

OBJS += libpng/libpng.o 

OBJS += zlib/zlib.o  


%.o: %.asm
	$(AS) $(ASFLAGS) -o $@ $< 

all:$(OBJS)  $(OBJSASM)
	$(CC) -shared -o $(DYLIB_LIB) $(OBJS)  $(OBJSASM) $(DEPEND_LIB) -fvisibility=hidden
	$(STRIP) $(DYLIB_LIB) 
	$(RM)  $(OBJS) $(OBJSASM) 
			
install: all
	mkdir -p $(PREFIX)/lib
	mkdir -p $(PREFIX)/include
	cp -f $(EXPORT_STATIC_LIB)  $(PREFIX)/lib
	cp -f bzlib.h $(PREFIX)/include
		
clean: 
	$(RM) $(OBJS) $(OBJSASM)    $(DYLIB_LIB) 

