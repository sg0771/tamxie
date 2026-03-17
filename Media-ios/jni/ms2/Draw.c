#include <jni.h>  
#include <android/bitmap.h>  
#include <android/log.h>  
#include <math.h>  
#include <string.h>  
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include "yuv2rgb.neon.h"
#include <libyuv.h>

#define LOG_TAG  "Ftanx"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)  
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)  


//---------------------------------------------------------------------------------------------------------------------------------------------------------------
static void  xProcess(int src_w, int src_h, int dst_w, int dst_h, int *dw, int *dh) {
	*dw = 0;*dh = 0;
	if ((dst_w * src_h / src_w) > dst_h)
		*dh = ((src_h - src_w * dst_h / dst_w) + 3) / 4 * 2;
	else
		*dw = ((src_w - src_h * dst_w / dst_h) + 3) / 4 * 2;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef struct _AndroidRender{
	int m_src_w;
	int m_src_h;
	int m_win_w;
	int m_win_h;
	int m_dw;
	int m_dh;
	uint8_t *m_tmp;//I420Scale Data
}AndroidRender;

static void ConvertData(AndroidRender *render, uint8_t* buf){
	if(render ==NULL)return;
	//Dst Data
	int dst_size = render->m_win_w * render->m_win_h;
	unsigned char *pYd  = render->m_tmp;
	unsigned char *pUd = pYd + dst_size;
	unsigned char *pVd = pYd + dst_size * 5 / 4;
	//Src Data
	int src_size = render->m_src_w * render->m_src_h;
	unsigned char *pYs =  buf;
	unsigned char *pUs = pYs + src_size ;
	unsigned char *pVs = pYs + src_size * 5 / 4;
	int sY = render->m_src_w;
	int sU = render->m_src_w / 2;
	int sV = render->m_src_w / 2;
	
	//裁剪后的区域 比例大约等于窗口比例这样使图像看起来不失真
	I420Scale( pYs + render->m_dh * sY + render->m_dw,  sY, 
	pUs + (render->m_dh / 2) * sU + render->m_dw / 2,  sU, 
	pVs + (render->m_dh / 2) * sV + render->m_dw / 2,  sV, 
	render->m_src_w - 2 * render->m_dw, render->m_src_h - 2 * render->m_dh,
	pYd,  render->m_win_w, 
	pUd, render->m_win_w / 2, 
	pVd, render->m_win_w / 2, 
	render->m_win_w, render->m_win_h, 
	kFilterLinear);
	//LOGE("%p SRC=%dx%d DST=%dx%d DwXDh=%dx%d",render, render->m_src_w, render->m_src_h, render->m_win_w, render->m_win_h, render->m_dw, render->m_dh);
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------------
//SurfaceView 在XML里面的大小， 或者上层API获取的大小
JNIEXPORT jint   Java_com_mediastream_MSDraw_xCreate  ( JNIEnv * env, jobject activity, int src_w, int src_h, int win_w, int win_h){
	AndroidRender *render = malloc(sizeof(AndroidRender ));
	render->m_src_w = src_w;
	render->m_src_h  = src_h;
	render->m_win_w  = (win_w  + 3)/4*4; //显存大小
	render->m_win_h  = (win_h   + 3)/4*4;//显存大小
	render->m_tmp     = (uint8_t*)malloc(render->m_win_w * render->m_win_h * 3 /  2);
	xProcess(render->m_src_w, render->m_src_h, render->m_win_w, render->m_win_h, &render->m_dw, &render->m_dh);
	return (jint)render;
}

JNIEXPORT void Java_com_mediastream_MSDraw_xDestroy( JNIEnv * env, jobject activity, jint handle){
	if(handle == 0)return;
	AndroidRender *render = (AndroidRender*)handle;
	if(render->m_tmp){
		free(render->m_tmp);
		render->m_tmp = NULL;
	}
	free(render);
}

JNIEXPORT void Java_com_mediastream_MSDraw_xDraw   ( JNIEnv * env, jobject activity, jint handle, jobject surface, jbyteArray buf){
	if(handle == 0)return;
	AndroidRender *render = (AndroidRender*)handle;
	//Clip YV12 Data
	unsigned char* yuv_pixel= (unsigned char*)(*env)->GetByteArrayElements(env, buf, 0);
	ConvertData(render, yuv_pixel);
	(*env)->ReleaseByteArrayElements(env, buf, yuv_pixel, 0); 	
	//Draw To Srface
	ANativeWindow * mANativeWindow = (ANativeWindow *)ANativeWindow_fromSurface(env, surface); 
	if (mANativeWindow == NULL)return;  
	ANativeWindow_setBuffersGeometry(mANativeWindow,  render->m_win_w, render->m_win_h, WINDOW_FORMAT_RGBA_8888);
	ANativeWindow_Buffer nwBuffer;
	if (0 != ANativeWindow_lock(mANativeWindow, &nwBuffer, 0))return;  
	int imgsize = render->m_win_w * render->m_win_h;
	unsigned char * pYs  = render->m_tmp;
	unsigned char * pUs  = render->m_tmp + imgsize;
	unsigned char * pVs  = render->m_tmp + imgsize * 5 / 4;
	yuv420_2_rgb8888_neon ((uint8_t*)nwBuffer.bits, 
						pYs,  pUs,  pVs, 
						render->m_win_w,  render->m_win_h,  
						render->m_win_w,  render->m_win_w/2, 
						nwBuffer.stride*4);						
	//LOGE("Render = %dx%d nwBuffer=%dx%dx%d ",render->m_win_w,  render->m_win_h, nwBuffer.width, nwBuffer.height, nwBuffer.format);
	if(0 !=ANativeWindow_unlockAndPost(mANativeWindow))return;  
	ANativeWindow_release(mANativeWindow);  
}