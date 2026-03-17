/*
ffmpeg header files
*/

#ifndef _FFMPEG_DEFINES_H_
#define _FFMPEG_DEFINES_H_

#ifdef _WIN32
#include <Windows.h>
#include <tchar.h>
#include <psapi.h>
#else
#define stricmp strcasecmp
#define _T(str) str
#define WXCTSTR const char*
#define WXTSTR  char*
#endif

#include <stdint.h>
#include <stdlib.h>
#include <setjmp.h>  
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <limits.h>
#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <wchar.h>
#include <assert.h>
#if HAVE_IO_H
#include <io.h>
#endif

#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#if HAVE_SYS_RESOURCE_H
#include <sys/time.h>
#include <sys/types.h>
#include <sys/resource.h>
#endif

#if HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif

#if HAVE_TERMIOS_H
#include <fcntl.h>
#include <sys/ioctl.h>
#include <termios.h>
#elif HAVE_KBHIT
#include <conio.h>
#endif

#ifdef __cplusplus
extern "C"{
#endif

#include "ffmpeg-config.h"


#include <libavutil/threadmessage.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavfilter/avfilter.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
#include <libavutil/fifo.h>
#include <libavutil/opt.h>
#include <libavfilter/buffersrc.h>
#include <libavfilter/buffersink.h>
#include <libavutil/display.h>
#include <libavutil/eval.h>
#include <libavutil/bprint.h>
#include <libavutil/pixdesc.h>
#include <libavutil/parseutils.h>
#include <libavutil/avassert.h>
#include <libavutil/intreadwrite.h>
#include <libavutil/time.h>
#include <libavcodec/avcodec.h>
#include <libavfilter/avfilter.h>
#include <libavformat/avformat.h>

#ifdef __cplusplus
};
#endif

#ifndef mid_pred
#define mid_pred mid_pred_c
static int mid_pred_c(int a, int b, int c) {
	if (a>b) {
		if (c>b) {
			if (c>a) b = a;
			else    b = c;
		}
	}
	else {
		if (b>c) {
			if (c>a) b = c;
			else    b = a;
		}
	}
	return b;
}
#endif

#define INSERT_FILT(name, arg) do {                                          \
    AVFilterContext *filt_ctx;                                               \
    ret = avfilter_graph_create_filter(&filt_ctx, avfilter_get_by_name(name), "m_" name, arg, NULL, graph);    \
    if (ret < 0)goto fail;                                                   \
    ret = avfilter_link(filt_ctx, 0, last_filter, 0);                        \
    if (ret < 0) goto fail;                                                  \
    last_filter = filt_ctx;                                                  \
} while (0)

#define HAS_ARG    0x0001
#define OPT_BOOL   0x0002
#define OPT_EXPERT 0x0004
#define OPT_STRING 0x0008
#define OPT_VIDEO  0x0010
#define OPT_AUDIO  0x0020
#define OPT_INT    0x0080
#define OPT_FLOAT  0x0100
#define OPT_SUBTITLE 0x0200
#define OPT_INT64  0x0400
#define OPT_EXIT   0x0800
#define OPT_DATA   0x1000
#define OPT_PERFILE  0x2000 
#define OPT_OFFSET 0x4000
#define OPT_SPEC   0x8000
#define OPT_TIME  0x10000
#define OPT_DOUBLE 0x20000
#define OPT_INPUT  0x40000
#define OPT_OUTPUT 0x80000
#define VSYNC_AUTO       -1
#define VSYNC_PASSTHROUGH 0
#define VSYNC_CFR         1
#define VSYNC_VFR         2
#define VSYNC_VSCFR       0xfe
#define VSYNC_DROP        0xff
#define DECODING_FOR_OST    1
#define DECODING_FOR_FILTER 2
#define MAX_STREAMS 1024    /* arbitrary sanity check value */

#define MAX_QUEUE_SIZE (15 * 1024 * 1024)
#define MIN_FRAMES 25
#define EXTERNAL_CLOCK_MIN_FRAMES 2
#define EXTERNAL_CLOCK_MAX_FRAMES 10
#define SDL_AUDIO_MIN_BUFFER_SIZE 512
#define SDL_AUDIO_MAX_CALLBACKS_PER_SEC 30
#define AV_SYNC_THRESHOLD_MIN 0.04  //40ms
#define AV_SYNC_THRESHOLD_MAX 0.1   //100ms
#define AV_SYNC_FRAMEDUP_THRESHOLD 0.1 //100ms
#define AV_NOSYNC_THRESHOLD 10.0
#define SAMPLE_CORRECTION_PERCENT_MAX 10
#define VIDEO_PICTURE_QUEUE_SIZE 3
#define SUBPICTURE_QUEUE_SIZE 16
#define SAMPLE_QUEUE_SIZE 9
#define FRAME_QUEUE_SIZE FFMAX(SAMPLE_QUEUE_SIZE, FFMAX(VIDEO_PICTURE_QUEUE_SIZE, SUBPICTURE_QUEUE_SIZE))
#define AUDIO_DIFF_AVG_NB   20
#define REFRESH_RATE 0.01 //10ms 刷新

#endif
