#include "libavcodec/avcodec.h"
#include "libavutil/mem.h"
#include <jni.h>
#include <libyuv.h>

#ifndef SAFE_ARRAY_DELETE
#define SAFE_ARRAY_DELETE(p) if(p){delete []p;p = NULL;}
#endif

typedef struct _FFDecoder{
	AVCodec *m_pCodec;
	AVCodecContext *m_c;
	AVFrame *m_pFrame;
	int m_bOpen;
	int m_iWidth;
	int m_iHeight;
}FFDecoder;

static FFDecoder* Open(){
	FFDecoder *pDec = av_malloc(sizeof(FFDecoder));
	avcodec_init();
	avcodec_register_all();
	pDec->m_pFrame = avcodec_alloc_frame();
	pDec->m_c = avcodec_alloc_context();
	pDec->m_pCodec = avcodec_find_decoder(CODEC_ID_H264);
	pDec->m_bOpen = 0;
	int iOpen = avcodec_open(pDec->m_c,pDec->m_pCodec);
	if(iOpen < 0)return 0;
	pDec->m_bOpen = 1;
	pDec->m_iWidth = 0;
	pDec->m_iHeight = 0;
	return pDec;
}

static int Close(FFDecoder *pDec){
	if(pDec == 0)return 0;
	if(pDec->m_bOpen == 0)return 0;
	avcodec_close(pDec->m_c);
	av_free(pDec->m_c);
	av_free(pDec->m_pFrame);
	pDec->m_bOpen = 0;
	av_free(pDec);
	return 1;
}

jint Java_com_mediastream_H264Decoder_Open(JNIEnv* env, jobject thiz){
	FFDecoder *pDec = Open();	
	return (jint)pDec;
}

// JAVA Close 后要把HandleDecoder 置空
jint Java_com_mediastream_H264Decoder_Close(JNIEnv* env, jobject thiz, jint HandleDecoder) {
	FFDecoder *pDec = (FFDecoder*)HandleDecoder;
	return Close(pDec);
}

jint Java_com_mediastream_H264Decoder_Decode(JNIEnv* env, jobject thiz, jint HandleDecoder, jbyteArray in, jint nalLen){
	FFDecoder *pDec = (FFDecoder*)HandleDecoder;
	unsigned char *pBuf = (unsigned char *)(*env)->GetByteArrayElements(env, in, 0);	
	int got_picture = 0;
	AVPacket avpkt;
	av_init_packet(&avpkt);
	avpkt.data = pBuf;
	avpkt.size = nalLen;		
	avpkt.flags = 0x0001;//#define AV_PKT_FLAG_KEY   0x0001

	int consumed_bytes = avcodec_decode_video2(pDec->m_c, pDec->m_pFrame, &got_picture, &avpkt); 
    (*env)->ReleaseByteArrayElements(env, in, pBuf, 0);    
	if (got_picture > 0) {
		pDec->m_iWidth  = pDec->m_c->width;
		pDec->m_iHeight = pDec->m_c->height;
	}
	return consumed_bytes;	
}

jint Java_com_mediastream_H264Decoder_GetWidth(JNIEnv* env, jobject thiz, jint HandleDecoder){
	FFDecoder *pDec = (FFDecoder*)HandleDecoder;
	return pDec->m_iWidth;
}

jint Java_com_mediastream_H264Decoder_GetHeight(JNIEnv* env, jobject thiz, jint HandleDecoder){
	FFDecoder *pDec = (FFDecoder*)HandleDecoder;
	return pDec->m_iHeight;
}

//Get RGBA Data
//Java 上层先获取W H
//然后申请 W*H*4 的内存
jint Java_com_mediastream_H264Decoder_GetYUVData(JNIEnv *env, jobject thiz, jint HandleDecoder, jbyteArray pYUV){
	FFDecoder *pDec = (FFDecoder*)HandleDecoder;
	int width = pDec->m_iWidth;
	int height = pDec->m_iHeight;
	if (width == 0 || height == 0)
		return 0;
	unsigned char* PixelYUV= (unsigned char*)(*env)->GetByteArrayElements(env, pYUV, 0);
	unsigned char *pYd  = PixelYUV;
	unsigned char *pVd = PixelYUV + width * height;
	unsigned char *pUd = PixelYUV + width * height * 5 / 4;
	
	unsigned char *pYs = pDec->m_pFrame->data[0];
	unsigned char *pUs = pDec->m_pFrame->data[1];
	unsigned char *pVs = pDec->m_pFrame->data[2];
	int sY = pDec->m_pFrame->linesize[0];
	int sU = pDec->m_pFrame->linesize[1];
	int sV = pDec->m_pFrame->linesize[2];
	I420Copy(pYs, sY, pUs, sU, pVs, sV, pYd, width, pUd, width / 2, pVd, width / 2, width, height);
	(*env)->ReleaseByteArrayElements(env, pYUV, PixelYUV, 0); 
	return 1;
}

//Java 上层先获取W H
//然后申请 W*H*4 的内存
static void  xProcess(int src_w, int src_h, int dst_w, int dst_h, int *dw, int *dh) {
	*dw = 0;*dh = 0;
	if ((dst_w * src_h / src_w) > dst_h)
		*dh = ((src_h - src_w * dst_h / dst_w) + 3) / 4 * 2;
	else
		*dw = ((src_w - src_h * dst_w / dst_h) + 3) / 4 * 2;
}

jint Java_com_mediastream_H264Decoder_GetYUVData2(JNIEnv *env, jobject thiz,
 jint HandleDecoder, jbyteArray pYUV,
 jint dst_width, jint dst_height){
	FFDecoder *pDec = (FFDecoder*)HandleDecoder;
	int width = pDec->m_iWidth;
	int height = pDec->m_iHeight;
	if (width == 0 || height == 0)
		return 0;
		
	int dw  = 0; //裁剪起始位置
	int dh  = 0;	
	xProcess(width,height,dst_width,dst_height,&dw,&dh);
	
	unsigned char* PixelYUV= (unsigned char*)(*env)->GetByteArrayElements(env, pYUV, 0);
	unsigned char *pYd  = PixelYUV;
	unsigned char *pUd = PixelYUV + dst_width * dst_height;
	unsigned char *pVd = PixelYUV + dst_width * dst_height * 5 / 4;
	
	unsigned char *pYs = pDec->m_pFrame->data[0];
	unsigned char *pUs = pDec->m_pFrame->data[1];
	unsigned char *pVs = pDec->m_pFrame->data[2];
	int sY = pDec->m_pFrame->linesize[0];
	int sU = pDec->m_pFrame->linesize[1];
	int sV = pDec->m_pFrame->linesize[2];
	
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
	
	(*env)->ReleaseByteArrayElements(env, pYUV, PixelYUV, 0); 
	return 1;
}

