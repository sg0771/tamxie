#ifndef _FFMPEG_INCLUDES_H_
#define _FFMPEG_INCLUDES_H_

extern "C" {
#include "FfmpegDefines.h"
};

#ifdef __APPLE__
#include <TargetConditionals.h>
#define IS_MAC
#include <sys/_pthread/_pthread_mutex_t.h>
#include <sys/select.h>
#include <sys/time.h>
#endif

#ifdef __ANDROID__
#include <jni.h>
#include <android/log.h>  
#define   LOG_TAG  "wxmedia"
#define   LOGE(...)   __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)  
#else
#define   LOGE(...)
#endif

#include <string>
#include <vector>
#include <mutex>
#include <thread>

#include "WXFFmpegConvertAPI.h"
//#include "IWXVideo.h"

// in ffmpeg.c
WXDELOGO_CAPI WXCtx * avffmpeg_create();
WXDELOGO_CAPI void    avffmpeg_setEventOwner(WXCtx *octx, void *owner);
WXDELOGO_CAPI void    avffmpeg_setRotate(WXCtx *octx, int rotate);//设置旋转角度
WXDELOGO_CAPI void    avffmpeg_setEvent(WXCtx *octx, WXFfmpegOnEvent cb);

WXDELOGO_CAPI void    avffmpeg_setVideoCb(WXCtx *octx, onFfmpegVideoData cb);

WXDELOGO_CAPI void    avffmpeg_setEventID(WXCtx *octx, WXCTSTR szID);
WXDELOGO_CAPI void    avffmpeg_set_video_encode_mode(WXCtx *octx, int mode);
WXDELOGO_CAPI void    avffmpeg_destroy(WXCtx *octx);
WXDELOGO_CAPI int     avffmpeg_convert(WXCtx *octx, int argc, char **argv);
WXDELOGO_CAPI void    avffmpeg_interrupt(WXCtx *octx);
WXDELOGO_CAPI int64_t avffmpeg_getCurrTime(WXCtx *octx);
WXDELOGO_CAPI int64_t avffmpeg_getTotalTime(WXCtx *octx);
WXDELOGO_CAPI int     avffmpeg_getState(WXCtx *octx);



#endif
