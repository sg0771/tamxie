#ifndef _FFMPEG_INCLUDES_H_
#define _FFMPEG_INCLUDES_H_

extern "C" {
#include "FfmpegDefines.h"
};

#ifdef __APPLE__
#include <TargetConditionals.h>
#define IS_MAC
#include <sys/_pthread/_pthread_mutex_t.h>
#endif

#ifdef ANDROID
#include <jni.h>
#include <android/log.h>
#define LOG    "libwxffmpeg"  
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG,__VA_ARGS__)
#define LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG,__VA_ARGS__)
#define LOGW(...)  __android_log_print(ANDROID_LOG_WARN,LOG,__VA_ARGS__)
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG,__VA_ARGS__)
#define LOGF(...)  __android_log_print(ANDROID_LOG_FATAL,LOG,__VA_ARGS__)
#endif

#include <string>
#include <vector>
#include <mutex>
#include <thread>

#include "WXMediaAPI.h"
#include "IWXVideo.h"

// in ffmpeg.c
WXFFMPEG_CAPI WXCtx * avffmpeg_create();
WXFFMPEG_CAPI void    avffmpeg_setEventOwner(WXCtx *octx, void *owner);
WXFFMPEG_CAPI void    avffmpeg_setRotate(WXCtx *octx, int rotate);//设置旋转角度
WXFFMPEG_CAPI void    avffmpeg_setEvent(WXCtx *octx, WXFfmpegOnEvent cb);
WXFFMPEG_CAPI void    avffmpeg_setEventID(WXCtx *octx, WXCTSTR szID);
WXFFMPEG_CAPI void    avffmpeg_set_video_encode_mode(WXCtx *octx, int mode);
WXFFMPEG_CAPI void    avffmpeg_destroy(WXCtx *octx);
WXFFMPEG_CAPI int     avffmpeg_convert(WXCtx *octx, int argc, char **argv);
WXFFMPEG_CAPI void    avffmpeg_interrupt(WXCtx *octx);
WXFFMPEG_CAPI int64_t avffmpeg_getCurrTime(WXCtx *octx);
WXFFMPEG_CAPI int64_t avffmpeg_getTotalTime(WXCtx *octx);
WXFFMPEG_CAPI int     avffmpeg_getState(WXCtx *octx);

#endif
