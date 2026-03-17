#include <string.h>
#include <jni.h>
#include <stdio.h>
#include <stdlib.h>

#include "libyuv.h" //NV21->YV12
#include "yuv2rgb.neon.h"

//旋转处理
static void ImageRotate(unsigned char *src, unsigned char *dst, int dst_width, int dst_height, int rotate){
	int imgsize = dst_width * dst_height;
	unsigned char * pYs  = src;
	unsigned char * pUs  = src + imgsize;
	unsigned char * pVs  = src + imgsize* 5 / 4;
	unsigned char * pYd  = dst;
	unsigned char * pUd =  dst + imgsize;
	unsigned char * pVd  = dst + imgsize* 5 / 4;	
	if(rotate == 0)
		I420Rotate(	pYs,  dst_width,    pUs,   dst_width / 2, pVs,  dst_width / 2, 
				pYd,  dst_width,   pUd,   dst_width / 2, pVd,  dst_width / 2,  
				dst_width, dst_height, kRotate0);
	else if(rotate == 90)
		I420Rotate(	pYs,  dst_height, pUs,  dst_height  / 2, pVs,  dst_height / 2, 
				pYd,  dst_width,  pUd,  dst_width / 2,  pVd, dst_width / 2,  
				dst_height, dst_width, kRotate90);
	else if(rotate == 80)		
		I420Rotate(	pYs,  dst_width,   pUs,   dst_width  / 2, pVs, dst_width / 2, 
				pYd,  dst_width,  pUd,   dst_width / 2,  pVd, dst_width / 2,  
				dst_width, dst_height, kRotate180);
	else if(rotate == 270)
		I420Rotate(	pYs,  dst_height,    pUs,  dst_height  / 2, pVs,  dst_height / 2, 
				pYd,  dst_width,  pUd,  dst_width / 2, pVd, dst_width / 2,  
				dst_height, dst_width, kRotate270);			
}
// 编码输入输出，缓冲在JAVA层分配
// YV12 I420 图像旋转
void Java_com_mediastream_ImageConvert_I420Rotate(JNIEnv* env, jobject thiz, jbyteArray in, jbyteArray out, jint dst_width, jint dst_height , jint rotate){
	jbyte * pBuf = (jbyte*)(*env)->GetByteArrayElements(env, in, 0);
	jbyte * pOutBuf = (jbyte*)(*env)->GetByteArrayElements(env, out, 0);
	ImageRotate(pBuf, pOutBuf, dst_width, dst_height,rotate);
	(*env)->ReleaseByteArrayElements(env, in, pBuf, 0);
	(*env)->ReleaseByteArrayElements(env, out, pOutBuf, 0);
}


static void ImageNV21ToI420(unsigned char *src, unsigned char *dst, int dst_width, int dst_height){
	int imgsize = dst_width * dst_height;
	unsigned char * pYs  = src;
	unsigned char * pUVs  = src + imgsize;
	unsigned char * pYd  = dst;
	unsigned char * pUd = dst + imgsize;
	unsigned char * pVd  = dst + imgsize* 5 / 4;
	NV12ToI420(	pYs, dst_width,pUVs, dst_width,
				pYd,  dst_width, pUd,  dst_width / 2, pVd, dst_width / 2,
				dst_width, dst_height);
}
// 编码输入输出，缓冲在JAVA层分配
// NV21 To YV12
void Java_com_mediastream_ImageConvert_NV21ToI420(JNIEnv* env, jobject thiz,  jbyteArray in, jbyteArray out,  jint dst_width, jint dst_height){
	jbyte * pBuf = (jbyte*)(*env)->GetByteArrayElements(env, in, 0);
	jbyte * pOutBuf = (jbyte*)(*env)->GetByteArrayElements(env, out, 0);
	ImageNV21ToI420(pBuf, pOutBuf, dst_width, dst_height);
	(*env)->ReleaseByteArrayElements(env, in, pBuf, 0);
	(*env)->ReleaseByteArrayElements(env, out, pOutBuf, 0);
}


//需要转为YV12格式统一处理
static void ImageNV21ToI420Rotate(unsigned char *src, unsigned char *dst, int dst_width, int dst_height, int rotate){
	int imgsize = dst_width * dst_height;
	unsigned char * pYs  = src;
	unsigned char * pUVs  = src + imgsize;
	unsigned char * pYd  = dst;
	unsigned char * pUd = dst + imgsize;
	unsigned char * pVd  = dst + imgsize* 5 / 4;
	if(rotate == 0){
		NV12ToI420Rotate(pYs, dst_width, pUVs, dst_width,
                     pYd, dst_width,pUd, dst_width/2,pVd, dst_width/2,
                     dst_width, dst_height, kRotate0);
	}else if(rotate == 90){
		NV12ToI420Rotate(pYs, dst_height, pUVs, dst_height,
                     pYd, dst_width,pUd, dst_width/2,pVd, dst_width/2,
                     dst_height, dst_width, kRotate90);
	}else if(rotate == 180){
		NV12ToI420Rotate(pYs, dst_width, pUVs, dst_width,
                     pYd, dst_width,pUd, dst_width/2,pVd, dst_width/2,
                     dst_width, dst_height, kRotate180);	
	}else if(rotate == 270){
		NV12ToI420Rotate(pYs, dst_height, pUVs, dst_height,
                     pYd, dst_width,pUd, dst_width/2,pVd, dst_width/2,
                     dst_height, dst_width, kRotate270);
	}
}
// 编码输入输出，缓冲在JAVA层分配
// NV21 To YV12 Rotate
void Java_com_mediastream_ImageConvert_NV21ToI420Rotate(JNIEnv* env, jobject thiz,  jbyteArray in, jbyteArray out,  jint dst_width, jint dst_height, jint rotate){
	jbyte * pBuf = (jbyte*)(*env)->GetByteArrayElements(env, in, 0);
	jbyte * pOutBuf = (jbyte*)(*env)->GetByteArrayElements(env, out, 0);
	ImageNV21ToI420Rotate(pBuf, pOutBuf, dst_width, dst_height, rotate);
	(*env)->ReleaseByteArrayElements(env, in, pBuf, 0);
	(*env)->ReleaseByteArrayElements(env, out, pOutBuf, 0);
}


//解码格式是I420   Y：U：V 格式
// 编码输入输出，缓冲在JAVA层分配
// YV12 To ARGB8888 
//libyuv 格式不对
static void ImageI420ToARGB(unsigned char *src, unsigned char *dst, int width, int height){
	int imgsize = width * height;
	unsigned char * pYs  = src;
	unsigned char * pUs  = src + imgsize;
	unsigned char * pVs  = src + imgsize * 5 / 4;	
	yuv420_2_rgb8888_neon (dst, pYs, pVs, pUs, width, height,  width,  width/2, width * 4);   
}
void Java_com_mediastream_ImageConvert_I420ToARGB(JNIEnv* env, jobject thiz,  jbyteArray in, jbyteArray out,  jint dst_width, jint dst_height, jint rotate){
	jbyte * pBuf = (jbyte*)(*env)->GetByteArrayElements(env, in, 0);
	jbyte * pOutBuf = (jbyte*)(*env)->GetByteArrayElements(env, out, 0);
	ImageI420ToARGB(pBuf, pOutBuf, dst_width, dst_height);
	(*env)->ReleaseByteArrayElements(env, in, pBuf, 0);
	(*env)->ReleaseByteArrayElements(env, out, pOutBuf, 0);
}

//手机常用的 YV12 格式 是  y:v:u 排列 
static void ImageYV12ToARGB(unsigned char *src, unsigned char *dst, int width, int height){
	int imgsize = width * height;
	unsigned char * pYs  = src;
	unsigned char * pVs  = src + imgsize;
	unsigned char * pUs  = src + imgsize * 5 / 4;	
	yuv420_2_rgb8888_neon (dst, pYs, pVs, pUs, width, height,  width,  width/2, width * 4);   
}
void Java_com_mediastream_ImageConvert_YV12ToARGB(JNIEnv* env, jobject thiz,  jbyteArray in, jbyteArray out,  jint dst_width, jint dst_height, jint rotate){
	jbyte * pBuf = (jbyte*)(*env)->GetByteArrayElements(env, in, 0);
	jbyte * pOutBuf = (jbyte*)(*env)->GetByteArrayElements(env, out, 0);
	ImageYV12ToARGB(pBuf, pOutBuf, dst_width, dst_height);
	(*env)->ReleaseByteArrayElements(env, in, pBuf, 0);
	(*env)->ReleaseByteArrayElements(env, out, pOutBuf, 0);
}

static void  xProcess(int src_w, int src_h, int dst_w, int dst_h, int *dw, int *dh) {
	*dw = 0;*dh = 0;
	if ((dst_w * src_h / src_w) > dst_h)
		*dh = ((src_h - src_w * dst_h / dst_w) + 3) / 4 * 2;
	else
		*dw = ((src_w - src_h * dst_w / dst_h) + 3) / 4 * 2;
}

//手机常用的 YV12 格式 是  y:v:u 排列 
//紧凑格式的YV12数据裁剪后缩放到dst比例的数据
static void ImageYV12Clip(unsigned char *src,  int width, int height, unsigned char *dst, int dst_width, int dst_height){
	int dw  = 0; //裁剪起始位置
	int dh  = 0;	
	xProcess(width,height,dst_width,dst_height,&dw,&dh);
	
	unsigned char *pYd  = dst;
	unsigned char *pUd = dst + dst_width * dst_height;
	unsigned char *pVd = dst + dst_width * dst_height * 5 / 4;
	
	unsigned char *pYs =  src;
	unsigned char *pUs = src + width * height ;
	unsigned char *pVs = src + width * height * 5 / 4;
	int sY = width;
	int sU = width / 2;
	int sV = width / 2;
	
	//裁剪后的区域 比例大约等于dst_w:dst_h
	I420Scale( pYs + dh * sY + dw,  sY, 
	pUs + (dh / 2) * sU + dw / 2,  sU, 
	pVs + (dh / 2) * sV + dw / 2,  sV, 
	width - 2 * dw, height - 2 * dh,
	pYd,  dst_width, 
	pUd, dst_width / 2, 
	pVd, dst_width / 2, 
	dst_width, dst_height, 
	kFilterLinear);
}
void Java_com_mediastream_ImageConvert_YV12Clip(JNIEnv* env, jobject thiz,  jbyteArray in,jint width, jint height,  jbyteArray out,   jint dst_width, jint dst_height){
	jbyte * pBuf = (jbyte*)(*env)->GetByteArrayElements(env, in, 0);
	jbyte * pOutBuf = (jbyte*)(*env)->GetByteArrayElements(env, out, 0);
	ImageYV12Clip(pBuf, width, height, pOutBuf, dst_width, dst_height);
	(*env)->ReleaseByteArrayElements(env, in, pBuf, 0);
	(*env)->ReleaseByteArrayElements(env, out, pOutBuf, 0);
}

int  Java_com_mediastream_ImageConvert_GetClipWidth(JNIEnv* env, jobject thiz, int src_w, int src_h, int dst_w, int dst_h) {
	if ((dst_w * src_h / src_w) > dst_h)
		return src_w;
	else
		return  (src_h * dst_w / dst_h + 3) / 4 * 4;
}

int  Java_com_mediastream_ImageConvert_GetClipHeight(JNIEnv* env, jobject thiz, int src_w, int src_h, int dst_w, int dst_h) {
	if ((dst_w * src_h / src_w) > dst_h)
		return  (src_w * dst_h / dst_w + 3) / 4 * 4;
	else
		return src_h;
}