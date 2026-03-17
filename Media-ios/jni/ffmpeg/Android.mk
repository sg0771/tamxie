LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_C_INCLUDES := $(LOCAL_PATH)

LOCAL_MODULE    := ffmpeg
LOCAL_SRC_FILES := libavcodec/allcodecs.c \
	libavcodec/audioconvert.c \
	libavcodec/avpacket.c \
	libavcodec/bitstream.c \
	libavcodec/bitstream_filter.c \
	libavcodec/cabac.c \
	libavcodec/dsputil.c \
	libavcodec/error_resilience.c \
	libavcodec/faanidct.c \
	libavcodec/golomb.c \
	libavcodec/h264.c \
	libavcodec/h264_cabac.c \
	libavcodec/h264_cavlc.c \
	libavcodec/h264_direct.c \
	libavcodec/h264_loopfilter.c \
	libavcodec/h264_ps.c \
	libavcodec/h264_refs.c \
	libavcodec/h264_sei.c \
	libavcodec/h264dsp.c \
	libavcodec/h264idct.c \
	libavcodec/h264pred.c \
	libavcodec/imgconvert.c \
	libavcodec/inverse.c \
	libavcodec/jrevdct.c \
	libavcodec/mpegvideo.c \
	libavcodec/opt.c \
	libavcodec/options.c \
	libavcodec/parser.c \
	libavcodec/pthread.c \
	libavcodec/raw.c \
	libavcodec/resample2.c \
	libavcodec/resample.c \
	libavcodec/simple_idct.c \
	libavcodec/svq3.c \
	libavcodec/utils.c \

LOCAL_SRC_FILES += libavcore/audioconvert.c \
	libavcore/imgutils.c \
	libavcore/parseutils.c \
	libavcore/samplefmt.c \
	libavcore/utils.c \

LOCAL_SRC_FILES += libavutil/avstring.c \
	libavutil/cpu.c \
	libavutil/error.c \
	libavutil/inverse.c \
	libavutil/mathematics.c \
	libavutil/mem.c \
	libavutil/opt.c \
	libavutil/pixdesc.c \
	libavutil/random_seed.c \
	libavutil/rational.c

LOCAL_CFLAGS := -std=gnu99 -fno-short-enums -fstrict-aliasing -O2 -fpic  -fPIC -include $(LOCAL_PATH)/ffconfig.h 
include $(BUILD_STATIC_LIBRARY)
