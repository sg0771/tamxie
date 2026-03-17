#ifndef _AVC_ENCODER_H_
#define _AVC_ENCODER_H_

#include <string.h>
#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include "common/x264.h"

struct _AVCEncoder{
	x264_t *m_h;
	x264_param_t m_param;
	x264_picture_t m_pic;
	int m_iWidth;
	int m_iHeight;
	int m_iFps;
};
typedef struct _AVCEncoder AVCEncoder;

// 创建编码器
static AVCEncoder* Open(int iWidth, int iHeight, int fram_interval, int qp, int iFps){
	AVCEncoder* pEnc = malloc(sizeof(AVCEncoder));
	memset(pEnc,0,sizeof(AVCEncoder));
	x264_param_default( &pEnc->m_param );
	pEnc->m_param.i_threads = 1;
	pEnc->m_param.i_frame_total = 0;
	pEnc->m_param.i_keyint_max = fram_interval;//100;//-i 100
	pEnc->m_iWidth  = pEnc->m_param.i_width = iWidth;
	pEnc->m_iHeight = pEnc->m_param.i_height = iHeight;
	pEnc->m_iFps    = pEnc->m_param.i_fps_num = iFps;
 	pEnc->m_param.rc.i_rc_method = X264_RC_CQP;// 指定qp
	pEnc->m_param.rc.i_qp_constant = qp; //34;	// 28
	pEnc->m_param.analyse.i_me_method = X264_ME_DIA;//-me dia
	pEnc->m_param.analyse.i_subpel_refine = 2;//-m --subme 2
 	pEnc->m_param.rc.i_qp_min = 10;
 	pEnc->m_param.rc.i_qp_max = 51;	

	//零延时
	pEnc->m_param.rc.i_lookahead = 0;
	pEnc->m_param.i_sync_lookahead = 0;
	pEnc->m_param.i_bframe = 0;
	pEnc->m_param.b_sliced_threads = 1;
	pEnc->m_param.b_vfr_input = 0;
       pEnc->m_param.rc.b_mb_tree = 0;

	//BASELINE
	pEnc->m_param.i_frame_reference = 1;//单参考帧
	pEnc->m_param.analyse.b_transform_8x8 = 0;
	pEnc->m_param.b_cabac = 1;//0;
	pEnc->m_param.i_cqm_preset = X264_CQM_FLAT;
	pEnc->m_param.psz_cqm_file = NULL;
	pEnc->m_param.i_bframe = 0;
	pEnc->m_param.analyse.i_weighted_pred = X264_WEIGHTP_NONE;
	pEnc->m_param.b_interlaced = 0;
	pEnc->m_param.b_fake_interlaced = 0;
		
	pEnc->m_h = x264_encoder_open( &pEnc->m_param );
	if(pEnc->m_h == NULL){
		return 0;
	}
	pEnc->m_pic.img.plane[0] = NULL;
	pEnc->m_pic.img.plane[1] = NULL;
	pEnc->m_pic.img.plane[2] = NULL;
	pEnc->m_pic.img.plane[3] = NULL;
	x264_picture_alloc(&pEnc->m_pic, X264_CSP_I420, iWidth, iHeight);
	return pEnc;		
}

static int Close(AVCEncoder *pEnc){
	if(pEnc){
		x264_picture_clean(&pEnc->m_pic);
		x264_encoder_close(pEnc->m_h);
		pEnc->m_h = NULL;
		free(pEnc);
		pEnc = NULL;
		return 1;
	}
	return 0;
}

// 标准编码，返回编码长度，假设缓冲区大小足够
//YV12 和 I420 的UV位置相反
//进来的数据有90度或者270度的旋转，需要处理一下
int GetWidth(AVCEncoder *pEnc){
	if(pEnc == 0)return 0;
	return pEnc->m_iWidth;
}
int GetHeight(AVCEncoder *pEnc){
	if(pEnc == 0)return 0;
	return pEnc->m_iHeight;
}

int Encode(AVCEncoder *pEnc, unsigned char *pBuf, unsigned char *pOutBuf, int force_key){
	if(pEnc == 0)return 0;
	// 编码结构
	int iDataLen  = 0;
	
	x264_nal_t *nal;
	int i_nal, i;
	x264_picture_t pic_out;
	int i_frame_size;
	int  bKey;
	int ImgSize = pEnc->m_iWidth * pEnc->m_iHeight;
	unsigned char *pYs = pBuf;
	unsigned char *pUs = pBuf + ImgSize;
	unsigned char *pVs = pBuf + ImgSize * 5 / 4;	
	memcpy(pEnc->m_pic.img.plane[0], pYs,   ImgSize);
	memcpy(pEnc->m_pic.img.plane[1], pVs,  ImgSize / 4);
	memcpy(pEnc->m_pic.img.plane[2], pUs,  ImgSize / 4);	

	if(force_key != 0)
		pEnc->m_pic.i_type = X264_TYPE_IDR;// 指定输出帧为关键帧
	i_frame_size = x264_encoder_encode( pEnc->m_h, &nal, &i_nal, &pEnc->m_pic, &pic_out);
	pEnc->m_pic.i_type = X264_TYPE_AUTO;
	
	if(i_frame_size < 0)return 0;	
	memcpy(pOutBuf,nal[0].p_payload, i_frame_size);
	bKey = pic_out.i_type == X264_TYPE_IDR;//是否关键帧
	i_frame_size |= (bKey << 24);// 把Key 标记加到第一位
	return i_frame_size;
}

jint Java_com_mediastream_H264Encoder_Open(JNIEnv* env, jobject thiz, jint width, jint height, jint fram_interval, jint qp, jint fps){
	AVCEncoder *pEnc = Open(width, height, fram_interval, qp, fps);
	return (jint)pEnc;
}

jint Java_com_mediastream_H264Encoder_Close(JNIEnv* env, jobject thiz, jint HandelEncoder){
	AVCEncoder *pEnc = (AVCEncoder*)HandelEncoder;	
	return Close(pEnc);
}

// 编码输入输出，缓冲在JAVA层分配
jint Java_com_mediastream_H264Encoder_Encode(JNIEnv* env, jobject thiz, jint HandelEncoder, jbyteArray in, jbyteArray out,  jint force_key){
	AVCEncoder *pEnc = (AVCEncoder*)HandelEncoder;	
	jbyte * pBuf    = (jbyte*)(*env)->GetByteArrayElements(env, in, 0);
	jbyte * pOutBuf = (jbyte*)(*env)->GetByteArrayElements(env, out, 0);
	int iOutLen = Encode(pEnc, pBuf, pOutBuf, force_key);
    	(*env)->ReleaseByteArrayElements(env, in , pBuf,    0); 
    	(*env)->ReleaseByteArrayElements(env, out, pOutBuf, 0); 
	return iOutLen;
}

jint Java_com_mediastream_H264Encoder_GetWidth(JNIEnv* env, jobject thiz, jint HandelEncoder){
	AVCEncoder *pEnc = (AVCEncoder*)HandelEncoder;
	return GetWidth(pEnc);
}

jint Java_com_mediastream_H264Encoder_GetHeight(JNIEnv* env, jobject thiz, jint HandelEncoder){
	AVCEncoder *pEnc = (AVCEncoder*)HandelEncoder;
	return GetHeight(pEnc);
}
#endif // _AVC_ENDECOR_H_
