/*
 OpenGL Video Render on OpenGLES
 */

#ifndef _IWXVideoRender_H_
#define _IWXVideoRender_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

//视频数据回调函数
 typedef void(*WXFfmpegOnVideoData)(uint8_t** data, int* linesize, int width, int height);

//创建视频渲染器
//hwnd:UIView 对象
//width:视频宽度
//height:视频高度
//cbData:回调函数
 void* WXVideoRenderCreate(void* hwnd, int width, int height, WXFfmpegOnVideoData cbData);

 //显示解码的YUV420数据
 void WXVideoRenderDisplay(void* p, uint8_t ** data, int* linesize);//YUV420P AVFrame

 //销毁视频解码器
 void WXVideoRenderDestroy(void* p);

#ifdef __cplusplus
}
#endif

#endif
