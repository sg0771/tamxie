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
                      
                      
#ifndef max
#define max(a,b)       (a)>(b)?(a):(b)
#endif
 
 #ifndef min
#define min(a,b)       (a)<(b)?(a):(b)
#endif                                                                                                                                   
//---------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef struct _AndroidRender{                                                                                                                                   
	int m_src_w;                                                                                                                                             
	int m_src_h;                                                                                                                                             
	int m_win_w;                                                                                                                                             
	int m_win_h;                                                                                                                                             
	uint8_t *m_tmp;//RGBAData                                                                                                                                
}AndroidRender;                                                                                                                                                  
                                                                                                                                                                 
//SurfaceView 在XML里面的大小， 或者上层API获取的大小                                                                                                                            
JNIEXPORT jint   Java_com_mediastream_MSDrawScale_xCreate  ( JNIEnv * env, jobject activity, int src_w, int src_h, int win_w, int win_h){                        
	AndroidRender *render = malloc(sizeof(AndroidRender ));                                                                                                  
	render->m_src_w = src_w;                                                                                                                                 
	render->m_src_h  = src_h;                                                                                                                                
	render->m_win_w = (win_w + 3) / 4*4; //显存大小                                                                                                              
	render->m_win_h  = (win_h + 3) / 4*4;//显存大小                                                                                                              
	render->m_tmp     = (uint8_t*)malloc(render->m_src_w * render->m_src_h * 4);                                                                             
	LOGE("Java_com_mediastream_MSDrawScale_xCreate OK");                                                                                                     
	return (jint)render;                                                                                                                                     
}                                                                                                                                                                
                                                                                                                                                                 
JNIEXPORT void Java_com_mediastream_MSDrawScale_xDestroy( JNIEnv * env, jobject activity, jint handle){                                                          
	if(handle == 0)return;                                                                                                                                   
	AndroidRender *render = (AndroidRender*)handle;                                                                                                          
	if(render->m_tmp){                                                                                                                                       
		free(render->m_tmp);                                                                                                                             
		render->m_tmp = NULL;                                                                                                                            
	}                                                                                                                                                        
	free(render);                                                                                                                                            
}                                                                                                                                                                
                                                                                                                                                                 
JNIEXPORT void Java_com_mediastream_MSDrawScale_xDraw(JNIEnv * env, jobject activity, jint handle, jobject surface, jbyteArray buf, int dx, int dy, int scale){  
		//YUV to RGB Data                                                                                                                                        
	AndroidRender *render = (AndroidRender*)handle;      
	
	int dst_win_w = render->m_win_w * 100 / scale;                                                                                                           
	int dst_win_h = render->m_win_h * 100 / scale; //缩放后的窗口大小                                                                                                
	int dst_dx = dx * 100 / scale;                                                                                                                           
	int dst_dy = dy * 100 / scale;//缩放后的图像左上角坐标
	
	if(dst_dx < -render->m_src_w)return;
	if(dst_dy < -render->m_src_h)return;	
	if(dst_dx > dst_win_w + render->m_src_w)return;
	if(dst_dy > dst_win_h + render->m_src_h)return;	//偏移越界
	
	int hw_x1 = max(dst_dx, 0);                                                                                                                 \ 
	int hw_x2 = min(dst_dx + render->m_src_w,  dst_win_w);                                                                                                    
	int hw_size_x = hw_x2 - hw_x1;//图像在缩小的视窗中所占的长度                                                                                                             
	
	int hw_y1 = max(dst_dy,0);                                                                                                                               
	int hw_y2 = min(dst_dy + render->m_src_h,  dst_win_h);	                                                                                                 
	int hw_size_y = hw_y2 - hw_y1;//图像在缩小的视窗中所占的宽度                                                                                                             

	int img_x1 =max(-dst_dx,0);//图像数据起始x位置
	int img_y1 =max(-dst_dy,0);//图像数据起始y位置
	                                                                                                                               
                                                                                                    
	unsigned char* yuv_pixel= (unsigned char*)(*env)->GetByteArrayElements(env, buf, 0);                                                                     
	int imgsize = render->m_src_w * render->m_src_h;                                                                                                         
	unsigned char * pYs  = yuv_pixel;                                                                                                                        
	unsigned char * pUs  = yuv_pixel + imgsize;                                                                                                              
	unsigned char * pVs  = yuv_pixel + imgsize * 5 / 4;                                                                                                      
	yuv420_2_rgb8888_neon (render->m_tmp ,                                                                                                                   
				pYs,  pUs,  pVs,                                                                                                                 
				render->m_src_w,  render->m_src_h,                                                                                               
				render->m_src_w,  render->m_src_w/2,                                                                                             
				render->m_src_w*4);                                                                                                              
	(*env)->ReleaseByteArrayElements(env, buf, yuv_pixel, 0);  
	                                                                                              
	//Draw To Srface                                                                                                                              
	ANativeWindow * mANativeWindow = (ANativeWindow *)ANativeWindow_fromSurface(env, surface);                                                               
	if (mANativeWindow == NULL)return;                                                                          
	ANativeWindow_setBuffersGeometry(mANativeWindow, dst_win_w, dst_win_h, WINDOW_FORMAT_RGBA_8888);
	ANativeWindow_Buffer nwBuffer;                                                                                                                           
	if (ANativeWindow_lock(mANativeWindow, &nwBuffer, 0) == 0){                                                                                              
		//Copy RGB To HW                                                                                                                                 
		//拷贝数据到显存                                                                                                                                        
		//nwBuffer.bits,  nwBuffer.stride                                                                                                                
		memset(nwBuffer.bits,0,nwBuffer.height * nwBuffer.stride * 4 );
		for(int h = hw_y1; h < hw_y1 + hw_size_y; h++){                                                                                                              
			uint8_t *pSRC = (uint8_t *)(render->m_tmp + (img_y1 + h - hw_y1) * render->m_src_w * 4 +img_x1*4);                                                         
			uint8_t *pDST = (uint8_t *)nwBuffer.bits + (h) * nwBuffer.stride * 4 + hw_x1 * 4 ;		                                         
			memcpy(pDST, pSRC, hw_size_x * 4);	                                                                                                 
		}                                                                                                                                                
		ANativeWindow_unlockAndPost(mANativeWindow);                                                                                                     
		ANativeWindow_release(mANativeWindow);                                                                                                                                                 
//		LOGE("Java_com_mediastream_MSDrawScale_xDraw OK!!!");
	}                                                                                                                                                        
}                                                                                                                                                                
