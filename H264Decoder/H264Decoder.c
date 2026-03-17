#include <jni.h>
#include <libyuv.h>
#include <android/bitmap.h>  
#include <android/log.h>  
#include <math.h>  
#include <string.h>  
#include <android/native_window.h>
#include <android/native_window_jni.h>

#include "libavcodec/avcodec.h"
#include "libavutil/mem.h"

#define  LOG_TAG  "Tam"
#define  LOGI(...)   __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)  
#define   LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)  

typedef struct FFDecoder {
	AVCodecContext *m_c;//解码器
	AVFrame *m_pFrame;//解码帧
	AVFrame *m_pTmpFrame;//和显示帧大小差不多的
	int m_iSrcWidth; //解码宽度
	int m_iSrcHeight;//解码高度
	int m_iWinWidth;
	int m_iWinHeight;
}FFDecoder;

static struct  FFDecoder* FFDecoder_Create(int srcWidth, int srcHeight) {
	FFDecoder* dec = (FFDecoder*)av_mallocz(sizeof(FFDecoder));
	avcodec_register_all();
	AVCodec* codec = avcodec_find_decoder(AV_CODEC_ID_H264);
	dec->m_c = avcodec_alloc_context3(codec);
	dec->m_c->width = srcWidth;
	dec->m_c->height = srcHeight;
	dec->m_c->coded_width = srcWidth;
	dec->m_c->coded_height = srcHeight;
	int iOpen = avcodec_open2(dec->m_c, codec, NULL);
	if (iOpen < 0) {
		// LOGE("%s Error",__FUNCTION__);
		return NULL;
	}
	dec->m_pFrame = av_frame_alloc();
	dec->m_iSrcWidth = srcWidth;
	dec->m_iSrcHeight = srcHeight;
	dec->m_pTmpFrame = NULL;
	dec->m_iWinWidth = 0;
	dec->m_iWinHeight = 0;
	// LOGE("%s OK Size=%dx%d", __FUNCTION__, srcWidth, srcHeight);
	return dec;
}

static void FFDecoder_ConvertData(struct  FFDecoder*dec, int winWidth, int winHeight) {
	if(dec->m_iWinWidth != winWidth / 4 * 4 || dec->m_iWinHeight != winHeight / 4 * 4){
		if (dec->m_pTmpFrame) {
			av_frame_free(&dec->m_pTmpFrame);
			dec->m_pTmpFrame = NULL;
		}
		dec->m_iWinWidth  = winWidth / 4 * 4;
		dec->m_iWinHeight = winHeight / 4 * 4;
		dec->m_pTmpFrame = av_frame_alloc();
		dec->m_pTmpFrame ->format = AV_PIX_FMT_YUV420P;
		dec->m_pTmpFrame ->width = dec->m_iWinWidth ;
		dec->m_pTmpFrame ->height = dec->m_iWinHeight;
		av_frame_get_buffer(dec->m_pTmpFrame , 1);
		// LOGE("%s Resize Display Rect =%dx%d",__FUNCTION__, dec->m_iWinWidth ,dec->m_iWinHeight );
	}
	I420Scale(dec->m_pFrame->data[0],  dec->m_pFrame->linesize[0], 
		dec->m_pFrame->data[1],  dec->m_pFrame->linesize[1], 
		dec->m_pFrame->data[2],  dec->m_pFrame->linesize[2], 
		dec->m_pFrame->width,  dec->m_pFrame->height, 
		dec->m_pTmpFrame->data[0],  dec->m_pTmpFrame->linesize[0], 
		dec->m_pTmpFrame->data[1],  dec->m_pTmpFrame->linesize[1], 
		dec->m_pTmpFrame->data[2],  dec->m_pTmpFrame->linesize[2], 
		dec->m_pTmpFrame->width,  dec->m_pTmpFrame->height, 
		kFilterLinear);
}

static void FFDecoder_Destroy(struct  FFDecoder*dec) {
	if (dec->m_c) {
		avcodec_close(dec->m_c);
		avcodec_free_context(&dec->m_c);
		dec->m_c = NULL;
	}
	if (dec->m_pFrame) {
		av_frame_free(&dec->m_pFrame);
		dec->m_pFrame = NULL;
	}
	if (dec->m_pTmpFrame) {
		av_frame_free(&dec->m_pTmpFrame);
		dec->m_pTmpFrame = NULL;
	}
	av_free(dec);
	// LOGE("%s OK",__FUNCTION__);
}

static void FFDecoder_GetARGB(struct  FFDecoder*dec, uint8_t* pOut) {
	I420ToABGR(
		dec->m_pFrame->data[0], dec->m_pFrame->linesize[0],
		dec->m_pFrame->data[1], dec->m_pFrame->linesize[1],
		dec->m_pFrame->data[2], dec->m_pFrame->linesize[2],
		pOut, dec->m_iSrcWidth * 4,
		dec->m_iSrcWidth, dec->m_iSrcHeight
	);
}

static void FFDecoder_GetYUY2(struct  FFDecoder*dec, uint8_t* pOut) {
	I420ToYUY2(
		dec->m_pFrame->data[0], dec->m_pFrame->linesize[0],
		dec->m_pFrame->data[1], dec->m_pFrame->linesize[1],
		dec->m_pFrame->data[2], dec->m_pFrame->linesize[2],
		pOut, dec->m_iSrcWidth * 2,
		dec->m_iSrcWidth, dec->m_iSrcHeight
	);
}

static void FFDecoder_GetI420(struct  FFDecoder*dec, uint8_t* pOutY, uint8_t* pOutU, uint8_t* pOutV,  int Pitch) {
	int size = dec->m_iSrcWidth*dec->m_iSrcHeight;
    if(Pitch == 0)
        Pitch = dec->m_iSrcWidth;
	I420Copy(
		dec->m_pFrame->data[0], dec->m_pFrame->linesize[0],
		dec->m_pFrame->data[1], dec->m_pFrame->linesize[1],
		dec->m_pFrame->data[2], dec->m_pFrame->linesize[2],
		pOutY, Pitch,
		pOutU, Pitch / 2,
		pOutV, Pitch / 2,
		dec->m_iSrcWidth,
		dec->m_iSrcHeight
	);
}

static int FFDecoder_Decode(struct  FFDecoder*dec, uint8_t *buf, int buf_size) {
	int got_picture = 0;
	AVPacket avpkt;
	av_init_packet(&avpkt);
	avpkt.data = buf;
	avpkt.size = buf_size;
	avpkt.flags = 0x0001;//#define AV_PKT_FLAG_KEY   0x0001
	int ret1 = avcodec_decode_video2(dec->m_c, dec->m_pFrame, &got_picture, &avpkt);
	if (got_picture == 0) {  //有的解码第一次仅仅配置了sps+pps
		int ret2 = avcodec_decode_video2(dec->m_c, dec->m_pFrame, &got_picture, &avpkt);
	}
	return got_picture > 0;
}

jlong Java_com_apowersoft_WXMedia_H264Decoder_Create(JNIEnv* env, jobject thiz, jint srcWidth, jint srcHeight){
	FFDecoder *pDec =  FFDecoder_Create(srcWidth, srcHeight);
	return (jlong)pDec;
}

// JAVA Close 后要把HandleDecoder 置空
jint Java_com_apowersoft_WXMedia_H264Decoder_Destroy(JNIEnv* env, jobject thiz, jlong HandleDecoder) {
	struct  FFDecoder *pDec = (struct  FFDecoder*)HandleDecoder;
	FFDecoder_Destroy(pDec);
	return 0;
}

jint Java_com_apowersoft_WXMedia_H264Decoder_Decode(JNIEnv* env, jobject thiz, jlong HandleDecoder,
	jbyteArray in, jint nalLen, jbyteArray out){
	struct FFDecoder *pDec = (struct FFDecoder*)HandleDecoder;
	uint8_t *pBuf      = (uint8_t *)(*env)->GetByteArrayElements(env, in, 0);
	int ret = FFDecoder_Decode(pDec, pBuf, nalLen);
	(*env)->ReleaseByteArrayElements(env, in, (jbyte*)pBuf, 0);
	return ret;	
}

void Java_com_apowersoft_WXMedia_H264Decoder_GetI420(JNIEnv* env, jobject thiz, jlong HandleDecoder, 
jbyteArray outY , jbyteArray outU, jbyteArray outV) {
	struct FFDecoder *pDec = (struct FFDecoder*)HandleDecoder;
	uint8_t *pBufOutY = (uint8_t *)(*env)->GetByteArrayElements(env, outY, 0);
	uint8_t *pBufOutU = (uint8_t *)(*env)->GetByteArrayElements(env, outU, 0);
	uint8_t *pBufOutV = (uint8_t *)(*env)->GetByteArrayElements(env, outV, 0);
	FFDecoder_GetI420(pDec, pBufOutY, pBufOutU, pBufOutV,0);
	(*env)->ReleaseByteArrayElements(env, outY, (jbyte*)pBufOutY, 0);
	(*env)->ReleaseByteArrayElements(env, outU, (jbyte*)pBufOutU, 0);
	(*env)->ReleaseByteArrayElements(env, outV, (jbyte*)pBufOutV, 0);
}

void Java_com_apowersoft_WXMedia_H264Decoder_GetYUV(JNIEnv* env, jobject thiz, jlong HandleDecoder, 
jbyteArray outY , jbyteArray outU, jbyteArray outV, jint Pitch) {
	struct FFDecoder *pDec = (struct FFDecoder*)HandleDecoder;
	uint8_t *pBufOutY = (uint8_t *)(*env)->GetByteArrayElements(env, outY, 0);
	uint8_t *pBufOutU = (uint8_t *)(*env)->GetByteArrayElements(env, outU, 0);
	uint8_t *pBufOutV = (uint8_t *)(*env)->GetByteArrayElements(env, outV, 0);
	FFDecoder_GetI420(pDec, pBufOutY, pBufOutU, pBufOutV, Pitch);
	(*env)->ReleaseByteArrayElements(env, outY, (jbyte*)pBufOutY, 0);
	(*env)->ReleaseByteArrayElements(env, outU, (jbyte*)pBufOutU, 0);
	(*env)->ReleaseByteArrayElements(env, outV, (jbyte*)pBufOutV, 0);
}

void Java_com_apowersoft_WXMedia_H264Decoder_GetYUY2(JNIEnv* env, jobject thiz, jlong HandleDecoder, jbyteArray out) {
	struct FFDecoder *pDec = (struct FFDecoder*)HandleDecoder; 
	uint8_t *pBufOut = (uint8_t *)(*env)->GetByteArrayElements(env, out, 0);
	FFDecoder_GetYUY2(pDec, pBufOut);
	(*env)->ReleaseByteArrayElements(env, out, (jbyte*)pBufOut, 0);
}


void Java_com_apowersoft_WXMedia_H264Decoder_GetARGB(JNIEnv* env, jobject thiz, jlong HandleDecoder, jbyteArray out) {
	struct FFDecoder *pDec = (struct FFDecoder*)HandleDecoder;
	uint8_t *pBufOut = (uint8_t *)(*env)->GetByteArrayElements(env, out, 0);
	FFDecoder_GetARGB(pDec, pBufOut);
	(*env)->ReleaseByteArrayElements(env, out, (jbyte*)pBufOut, 0);
}


void Java_com_apowersoft_WXMedia_H264Decoder_Display(JNIEnv* env, jobject thiz, jlong HandleDecoder,  jobject surface,  jint winWidth, jint winHeight) {
	struct FFDecoder *dec = (struct FFDecoder*)HandleDecoder;
	FFDecoder_ConvertData(dec, winWidth, winHeight);
	ANativeWindow * mANativeWindow = (ANativeWindow *)ANativeWindow_fromSurface(env, surface); 
	if (mANativeWindow == NULL){
		return;  
	}
	ANativeWindow_setBuffersGeometry(mANativeWindow,  dec->m_pTmpFrame->width,  dec->m_pTmpFrame->height, WINDOW_FORMAT_RGBA_8888);
	ANativeWindow_Buffer nwBuffer;
	if (0 != ANativeWindow_lock(mANativeWindow, &nwBuffer, 0))
		return;  	
	I420ToABGR(
		dec->m_pTmpFrame->data[0],  dec->m_pTmpFrame->linesize[0], 
		dec->m_pTmpFrame->data[1],  dec->m_pTmpFrame->linesize[1], 
		dec->m_pTmpFrame->data[2],  dec->m_pTmpFrame->linesize[2], 
		(uint8_t*)nwBuffer.bits, nwBuffer.stride*4,
		dec->m_pTmpFrame->width,  dec->m_pTmpFrame->height
	);
	if(0 !=ANativeWindow_unlockAndPost(mANativeWindow))
		return; 
	ANativeWindow_release(mANativeWindow);  
}


void Java_com_apowersoft_WXMedia_H264Decoder_Display2(JNIEnv* env, jobject thiz, jlong HandleDecoder,  jobject surface,  jint winWidth, jint winHeight) {
	AVFrame *m_pTmpFrame = av_frame_alloc();
	m_pTmpFrame ->format = AV_PIX_FMT_YUV420P;
	m_pTmpFrame ->width = winWidth / 4 * 4;
	m_pTmpFrame ->height = winHeight / 4 * 4;
	av_frame_get_buffer(m_pTmpFrame , 1);
	memset(m_pTmpFrame ->data[1],128,m_pTmpFrame ->linesize[1] * m_pTmpFrame ->height/2 );//U
	memset(m_pTmpFrame ->data[2],128,m_pTmpFrame ->linesize[2] * m_pTmpFrame ->height/2 );//V	
	for(int h = 0; h < m_pTmpFrame ->height ;h++){
		for(int w = 0 ; w < m_pTmpFrame ->width;w++){
			int  pos = w + h * m_pTmpFrame ->linesize[0];
			m_pTmpFrame ->data[0][pos] = (w + h) % 256;
		}
	}
		
	ANativeWindow * mANativeWindow = (ANativeWindow *)ANativeWindow_fromSurface(env, surface); 
	if (mANativeWindow == NULL){
		return;  
	}
	ANativeWindow_setBuffersGeometry(mANativeWindow,  m_pTmpFrame->width,  m_pTmpFrame->height, WINDOW_FORMAT_RGBA_8888);
	ANativeWindow_Buffer nwBuffer;
	if (0 != ANativeWindow_lock(mANativeWindow, &nwBuffer, 0))
		return;  	
	I420ToABGR(
		m_pTmpFrame->data[0],  m_pTmpFrame->linesize[0], 
		m_pTmpFrame->data[1],  m_pTmpFrame->linesize[1], 
		m_pTmpFrame->data[2], m_pTmpFrame->linesize[2], 
		(uint8_t*)nwBuffer.bits, nwBuffer.stride*4,
		m_pTmpFrame->width,  m_pTmpFrame->height
	);
	if(0 !=ANativeWindow_unlockAndPost(mANativeWindow))
		return; 
	ANativeWindow_release(mANativeWindow);  
}



struct sps_info_struct
{
	uint32_t profile_idc;// = 0;
	uint32_t level_idc;// = 0;

	uint32_t width;// = 0;
	uint32_t height;// = 0;
	uint32_t fps;// = 0;      //SPS中可能不包含FPS信息
};

struct sps_bit_stream
{
	const uint8_t *data;// = NULL; //sps数据
	int size;// = 0;          //sps数据大小
	int index;// = 0;         //当前计算位所在的位置标记
};

static void del_emulation_prevention(uint8_t *data, int *dataSize)
{
	uint32_t dataSizeTemp = *dataSize;
	for (uint32_t i = 0, j = 0; i<(dataSizeTemp - 2); i++) {
		int val = (data[i] ^ 0x0) + (data[i + 1] ^ 0x0) + (data[i + 2] ^ 0x3);    //检测是否是竞争码
		if (val == 0) {
			for (j = i + 2; j<dataSizeTemp - 1; j++) {    //移除竞争码
				data[j] = data[j + 1];
			}

			(*dataSize)--;      //data size 减1
		}
	}
}
static void sps_bs_init(struct sps_bit_stream *bs, const uint8_t *data, uint32_t size)
{
	if (bs) {
		bs->data = data;
		bs->size = size;
		bs->index = 0;
	}
}

static int32_t eof(struct sps_bit_stream *bs) {
	return (bs->index >= bs->size * 8);    //位偏移已经超出数据
}

static uint32_t u(struct sps_bit_stream *bs, uint8_t bitCount)
{
	uint32_t val = 0;
	for (uint8_t i = 0; i<bitCount; i++) {
		val <<= 1;
		if (eof(bs)) {
			val = 0;
			break;
		}
		else if (bs->data[bs->index / 8] & (0x80 >> (bs->index % 8))) {     //计算index所在的位是否为1
			val |= 1;
		}
		bs->index++;  //递增当前起始位(表示该位已经被计算，在后面的计算过程中不需要再次去计算所在的起始位索引，缺点：后面每个bit位都需要去位移)
	}

	return val;
}

static uint32_t ue(struct sps_bit_stream *bs)
{
	uint32_t zeroNum = 0;
	while (u(bs, 1) == 0 && !eof(bs) && zeroNum < 32) {
		zeroNum++;
	}

	return (uint32_t)((1 << zeroNum) - 1 + u(bs, zeroNum));
}

static int32_t se(struct sps_bit_stream *bs)
{
	int32_t ueVal = (int32_t)ue(bs);
	double k = ueVal;

	int32_t seVal = (int32_t)ceil(k / 2);     //ceil:返回大于或者等于指定表达式的最小整数
	if (ueVal % 2 == 0) {       //偶数取反，即(-1)^(k+1)
		seVal = -seVal;
	}

	return seVal;
}

static void vui_para_parse(struct sps_bit_stream *bs, struct sps_info_struct *info)
{
	uint32_t aspect_ratio_info_present_flag = u(bs, 1);
	if (aspect_ratio_info_present_flag) {
		uint32_t aspect_ratio_idc = u(bs, 8);
		if (aspect_ratio_idc == 255) {      //Extended_SAR
			u(bs, 16);      //sar_width
			u(bs, 16);      //sar_height
		}
	}

	uint32_t overscan_info_present_flag = u(bs, 1);
	if (overscan_info_present_flag) {
		u(bs, 1);       //overscan_appropriate_flag
	}

	uint32_t video_signal_type_present_flag = u(bs, 1);
	if (video_signal_type_present_flag) {
		u(bs, 3);       //video_format
		u(bs, 1);       //video_full_range_flag
		uint32_t colour_description_present_flag = u(bs, 1);
		if (colour_description_present_flag) {
			u(bs, 8);       //colour_primaries
			u(bs, 8);       //transfer_characteristics
			u(bs, 8);       //matrix_coefficients
		}
	}

	uint32_t chroma_loc_info_present_flag = u(bs, 1);
	if (chroma_loc_info_present_flag) {
		ue(bs);     //chroma_sample_loc_type_top_field
		ue(bs);     //chroma_sample_loc_type_bottom_field
	}

	uint32_t timing_info_present_flag = u(bs, 1);
	if (timing_info_present_flag) {
		uint32_t num_units_in_tick = u(bs, 32);
		uint32_t time_scale = u(bs, 32);
		uint32_t fixed_frame_rate_flag = u(bs, 1);

		info->fps = (uint32_t)((float)time_scale / (float)num_units_in_tick);
		if (fixed_frame_rate_flag) {
			info->fps = info->fps / 2;
		}
	}

	uint32_t nal_hrd_parameters_present_flag = u(bs, 1);
	if (nal_hrd_parameters_present_flag) {
		//hrd_parameters()  //see E.1.2 HRD parameters syntax
	}

	//后面代码需要hrd_parameters()函数接口实现才有用
	uint32_t vcl_hrd_parameters_present_flag = u(bs, 1);
	if (vcl_hrd_parameters_present_flag) {
		//hrd_parameters()
	}
	if (nal_hrd_parameters_present_flag || vcl_hrd_parameters_present_flag) {
		u(bs, 1);   //low_delay_hrd_flag
	}

	u(bs, 1);       //pic_struct_present_flag
	uint32_t bitstream_restriction_flag = u(bs, 1);
	if (bitstream_restriction_flag) {
		u(bs, 1);   //motion_vectors_over_pic_boundaries_flag
		ue(bs);     //max_bytes_per_pic_denom
		ue(bs);     //max_bits_per_mb_denom
		ue(bs);     //log2_max_mv_length_horizontal
		ue(bs);     //log2_max_mv_length_vertical
		ue(bs);     //max_num_reorder_frames
		ue(bs);     //max_dec_frame_buffering
	}
}

static void GetSize2(const uint8_t *pData, int dataSize, int *out_width, int *out_height)
{
	uint8_t *data = (uint8_t *)pData;
	int POS = 0;
	if (data[0] == 0 && data[1] == 0 && (data[2] == 1 || data[3] == 1)) {
		if (data[2] == 1)
			POS = 3;
		else
			POS = 4;
	}
	else {
		POS = 8;
	}
	data += POS;
	dataSize -= POS;

	*out_width = 0;
	*out_height = 0;
	uint8_t __x = data[0];
	uint8_t __y = (__x & 0x1F);
	if (!data || dataSize <= 0) return;
	if (__y != 0x07)return;

	struct  sps_info_struct info;

	int32_t ret = 0;

	uint8_t *dataBuf = (uint8_t *)malloc(dataSize);
	memcpy(dataBuf, data, dataSize);        //重新拷贝一份数据，防止移除竞争码时对原数据造成影响
	del_emulation_prevention(dataBuf, &dataSize);

	struct sps_bit_stream bs;
	sps_bs_init(&bs, dataBuf, dataSize);   //初始化SPS数据流结构体

	u(&bs, 1);      //forbidden_zero_bit
	u(&bs, 2);      //nal_ref_idc
	uint32_t nal_unit_type = u(&bs, 5);

	if (nal_unit_type == 0x7) {     //Nal SPS Flag
		info.profile_idc = u(&bs, 8);
		u(&bs, 1);      //constraint_set0_flag
		u(&bs, 1);      //constraint_set1_flag
		u(&bs, 1);      //constraint_set2_flag
		u(&bs, 1);      //constraint_set3_flag
		u(&bs, 1);      //constraint_set4_flag
		u(&bs, 1);      //constraint_set4_flag
		u(&bs, 2);      //reserved_zero_2bits
		info.level_idc = u(&bs, 8);

		ue(&bs);    //seq_parameter_set_id

		uint32_t chroma_format_idc = 1;     //摄像机出图大部分格式是4:2:0
		if (info.profile_idc == 100 || info.profile_idc == 110 || info.profile_idc == 122 ||
			info.profile_idc == 244 || info.profile_idc == 44 || info.profile_idc == 83 ||
			info.profile_idc == 86 || info.profile_idc == 118 || info.profile_idc == 128 ||
			info.profile_idc == 138 || info.profile_idc == 139 || info.profile_idc == 134 ||
			info.profile_idc == 135) {
			chroma_format_idc = ue(&bs);
			if (chroma_format_idc == 3) {
				u(&bs, 1);      //separate_colour_plane_flag
			}

			ue(&bs);        //bit_depth_luma_minus8
			ue(&bs);        //bit_depth_chroma_minus8
			u(&bs, 1);      //qpprime_y_zero_transform_bypass_flag
			uint32_t seq_scaling_matrix_present_flag = u(&bs, 1);
			if (seq_scaling_matrix_present_flag) {
				uint32_t seq_scaling_list_present_flag[8] = { 0 };
				for (int32_t i = 0; i<((chroma_format_idc != 3) ? 8 : 12); i++) {
					seq_scaling_list_present_flag[i] = u(&bs, 1);
					if (seq_scaling_list_present_flag[i]) {
						if (i < 6) {    //scaling_list(ScalingList4x4[i], 16, UseDefaultScalingMatrix4x4Flag[i])
						}
						else {    //scaling_list(ScalingList8x8[i ? 6], 64, UseDefaultScalingMatrix8x8Flag[i ? 6] )
						}
					}
				}
			}
		}

		ue(&bs);        //log2_max_frame_num_minus4
		uint32_t pic_order_cnt_type = ue(&bs);
		if (pic_order_cnt_type == 0) {
			ue(&bs);        //log2_max_pic_order_cnt_lsb_minus4
		}
		else if (pic_order_cnt_type == 1) {
			u(&bs, 1);      //delta_pic_order_always_zero_flag
			se(&bs);        //offset_for_non_ref_pic
			se(&bs);        //offset_for_top_to_bottom_field

			uint32_t num_ref_frames_in_pic_order_cnt_cycle = ue(&bs);
			int32_t *offset_for_ref_frame = (int32_t *)malloc((uint32_t)num_ref_frames_in_pic_order_cnt_cycle * sizeof(int32_t));
			for (int32_t i = 0; i<num_ref_frames_in_pic_order_cnt_cycle; i++) {
				offset_for_ref_frame[i] = se(&bs);
			}
			free(offset_for_ref_frame);
		}

		ue(&bs);      //max_num_ref_frames
		u(&bs, 1);      //gaps_in_frame_num_value_allowed_flag

		uint32_t pic_width_in_mbs_minus1 = ue(&bs);     //第36位开始
		uint32_t pic_height_in_map_units_minus1 = ue(&bs);      //47
		uint32_t frame_mbs_only_flag = u(&bs, 1);

		info.width = (int32_t)(pic_width_in_mbs_minus1 + 1) * 16;
		info.height = (int32_t)(2 - frame_mbs_only_flag) * (pic_height_in_map_units_minus1 + 1) * 16;

		if (!frame_mbs_only_flag) {
			u(&bs, 1);      //mb_adaptive_frame_field_flag
		}

		u(&bs, 1);     //direct_8x8_inference_flag
		uint32_t frame_cropping_flag = u(&bs, 1);
		if (frame_cropping_flag) {
			uint32_t frame_crop_left_offset = ue(&bs);
			uint32_t frame_crop_right_offset = ue(&bs);
			uint32_t frame_crop_top_offset = ue(&bs);
			uint32_t frame_crop_bottom_offset = ue(&bs);

			//See 6.2 Source, decoded, and output picture formats
			int32_t crop_unit_x = 1;
			int32_t crop_unit_y = 2 - frame_mbs_only_flag;      //monochrome or 4:4:4
			if (chroma_format_idc == 1) {   //4:2:0
				crop_unit_x = 2;
				crop_unit_y = 2 * (2 - frame_mbs_only_flag);
			}
			else if (chroma_format_idc == 2) {    //4:2:2
				crop_unit_x = 2;
				crop_unit_y = 2 - frame_mbs_only_flag;
			}

			info.width -= crop_unit_x * (frame_crop_left_offset + frame_crop_right_offset);
			info.height -= crop_unit_y * (frame_crop_top_offset + frame_crop_bottom_offset);
		}

		*out_width = info.width;
		*out_height = info.height;
		uint32_t vui_parameters_present_flag = u(&bs, 1);
		if (vui_parameters_present_flag) {
			vui_para_parse(&bs, &info);
		}

		ret = 1;
	}
	free(dataBuf);
	return;
}


jint Java_com_apowersoft_WXMedia_H264Decoder_GetWidth(JNIEnv* env, jobject thiz,jbyteArray data, jint size){
	uint8_t *pBuf      = (uint8_t *)(*env)->GetByteArrayElements(env, data, 0);
	int width  = 0;
	int height = 0;
	GetSize2(pBuf, size, &width, &height);
	(*env)->ReleaseByteArrayElements(env, data, (jbyte*)pBuf, 0);
	return width;	
}

jint Java_com_apowersoft_WXMedia_H264Decoder_GetHeight(JNIEnv* env, jobject thiz,jbyteArray data, jint size){
	uint8_t *pBuf      = (uint8_t *)(*env)->GetByteArrayElements(env, data, 0);
	int width  = 0;
	int height = 0;
	GetSize2(pBuf, size, &width, &height);
	(*env)->ReleaseByteArrayElements(env, data, (jbyte*)pBuf, 0);
	return height;	
}
