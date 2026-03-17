#include <jni.h>
#include <android/log.h>  
#include <stdint.h>
#include <thread>
#include <queue>
#include <mutex>
#include <unistd.h>
#include <faac.h>
#include <rtmp.h>
#include <libyuv.h>
extern "C" {
#include  <x264.h>
}; 

#define   LOG_TAG  "WXRtmp"
#define   LOGE(...)   __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)  
#define   WXLocker    std::recursive_mutex
#define   WXAutoLock  std::lock_guard<WXLocker>
#define   SAVC(x)    static const AVal av_ ## x = AVC(#x)

static const AVal av_setDataFrame = AVC("@setDataFrame");
static const AVal av_SDKVersion = AVC("LFLiveKit 2.4.0");
SAVC(onMetaData);
SAVC(duration);
SAVC(width);
SAVC(height);
SAVC(videocodecid);
SAVC(videodatarate);
SAVC(framerate);
SAVC(audiocodecid);
SAVC(audiodatarate);
SAVC(audiosamplerate);
SAVC(audiosamplesize);
SAVC(stereo);
SAVC(encoder);
SAVC(fileSize);
SAVC(avc1);
SAVC(mp4a);

static int s_nFps = 15;//视频帧率
static int s_nVideoBitrate = 1350;//视频码率
static int s_nAudioBitrate = 128;//音频码率码率
class RTMP_Sender;
static RTMP_Sender *s_obj = nullptr;

static WXLocker s_lckHandle;//全局锁
static jlong s_handle = 0;//只能有一个对象

volatile int s_nStop = 1;//线程结束标记！

static jlong s_nA = 0;
static jlong s_nV = 0;

static  int s_bSetMeta = 0;
static  int s_nMetaWidth = 0;
static  int s_nMetaHeight = 0;
static  int s_nMetaVideoBitrate = 0;
static  int s_nMetaAudioBitrate = 0;

static int64_t  WXGetTimeMs() {
#ifdef _WIN32
	return ::timeGetTime();
#else
	struct timeval tv;
	gettimeofday(&tv, nullptr);
	return tv.tv_sec * 1000 + tv.tv_usec / 1000;
#endif
}

struct  AudioBuffer{
	uint8_t* m_pData = nullptr;
	int m_nSize;
	AudioBuffer(int data_size, uint8_t* data) {
		m_nSize = data_size;
		m_pData =(uint8_t*)malloc(m_nSize);
		memcpy(m_pData, data, m_nSize);
	}
	virtual ~AudioBuffer() {
		if (m_pData) {
			free(m_pData);
			m_pData = nullptr;
		}
	}
};
static WXLocker s_lckAudio;
static std::queue<AudioBuffer*>s_queueAudio;
static void AudioPush(int data_size, uint8_t* data) {

	{
		WXAutoLock al(s_lckHandle);
		if (s_handle == 0) {
			LOGE("kkkkkk No RTMP OBJ %s Error", __FUNCTION__);
			return;
		}
	}

	WXAutoLock al(s_lckAudio);
	if (s_queueAudio.size() > 10)
		return;
	AudioBuffer* obj = new AudioBuffer(data_size, data);
	s_queueAudio.push(obj);
}

static AudioBuffer* AudioPop() {
	WXAutoLock al(s_lckAudio);
	if (s_queueAudio.empty())
		return nullptr;
	AudioBuffer* obj = s_queueAudio.front();
	s_queueAudio.pop();
	return obj;
}

static void AudioClean() {
	WXAutoLock al(s_lckAudio);
	LOGE("%s Start", __FUNCTION__);
	while (!s_queueAudio.empty()) {
		AudioBuffer* obj = s_queueAudio.front();
		s_queueAudio.pop();
		delete obj;
	}
	LOGE("%s Stop", __FUNCTION__);
}

struct VideoBuffer{
public:
	uint8_t* m_pData = nullptr;
	int m_nWidth = 0;
	int m_nHeight = 0;

	int m_nClipWidth = 0;
	int m_nClipHeight = 0;
	int64_t m_pts = 0;
	VideoBuffer(int64_t pts, uint8_t* data, int src_w, int src_h,int dst_w,int dst_h) { //裁剪模式
		m_pts = pts;
		m_nWidth = src_w;
		m_nHeight = src_h;
		m_nClipWidth = dst_w;
		m_nClipHeight = dst_h;
		if (data) {
			int size = src_w * src_h * 3 / 2;
			m_pData = (uint8_t*)malloc(size);
			memcpy(m_pData, data, size);
		}
	}
	virtual ~VideoBuffer() {
		if (m_pData) {
			free(m_pData);
			m_pData = nullptr;
		}
	}
};
static WXLocker s_lckVideo;
static std::queue<VideoBuffer*>s_queueVideo;

static void VideoPush(int64_t pts, uint8_t* data, int width, int height, int dst_w, int dst_h) {

	{
		WXAutoLock al(s_lckHandle);
		if (s_handle == 0) {
			LOGE("kkkkkk No RTMP OBJ %s Error", __FUNCTION__);
			return;
		}
	}

    {
        WXAutoLock al(s_lckVideo);
        if (s_queueVideo.size() > 10) {
            LOGE("VideoQueue is too big %s Error", __FUNCTION__);
            return;
        }
    }

	VideoBuffer* obj = new VideoBuffer(pts, data, width, height,dst_w,dst_h);

    WXAutoLock al(s_lckVideo);
	s_queueVideo.push(obj);
}

static VideoBuffer* VideoPop() {
	WXAutoLock al(s_lckVideo);
	if (s_queueVideo.empty()) {
		return nullptr;
	}
	VideoBuffer* obj = s_queueVideo.front();
	s_queueVideo.pop();
	return obj;
}

static void VideoClean() {
	WXAutoLock al(s_lckVideo);
	LOGE("%s Start", __FUNCTION__);
	while (!s_queueVideo.empty()){
		VideoBuffer* obj = s_queueVideo.front();
		s_queueVideo.pop();
		delete obj;
	}
	LOGE("%s Stop", __FUNCTION__);
}


class AVCEncoder {
	x264_t *m_pCtx = NULL;

	x264_picture_t m_pic;
	int m_iWidth = 0;
	int m_iHeight = 0;
	int m_iFps = 0;

	uint8_t *m_pAvcBuffer = NULL;
	uint8_t *m_extradata = nullptr;
	int m_extrasize = 0;
public:
	int Open(int iWidth, int iHeight, uint8_t*& pOut, int& outlen) {
		pOut = NULL;
		outlen = 0;
		LOGE("H264 %s [%dx%d]", __FUNCTION__, iWidth, iHeight);

		x264_param_t x264_param;
		x264_param_default(&x264_param);
		x264_param_default_preset(&x264_param, "superfast", "zerolatency");
		m_iWidth = x264_param.i_width = iWidth;
		m_iHeight = x264_param.i_height = iHeight;
		x264_param.i_csp = X264_CSP_I420;//
		x264_param.i_threads = 1;
		if (s_nVideoBitrate == 0) {
			x264_param.rc.i_rc_method = X264_RC_CQP;
			x264_param.rc.i_qp_constant = 25;
		}
		else {
			x264_param.rc.i_rc_method = X264_RC_ABR;
			x264_param.rc.i_bitrate = s_nVideoBitrate;
		}
		x264_param.b_repeat_headers = 0;//Not SPS PPS in NAL
		x264_param.b_annexb = 0;//Not 00 00 00 01 / 00 00 01 StartCode  MP4 Format

		x264_param.i_timebase_num = 1;
		x264_param.i_timebase_den = 1000;//ms time

		m_pCtx = x264_encoder_open(&x264_param);
		if (m_pCtx == NULL) {
			LOGE("H264 %s [%dx%d] x264_encoder_open ERROR", __FUNCTION__, iWidth, iHeight);
			return 0;
		}

		m_pAvcBuffer = (uint8_t*)malloc(iWidth * iHeight);
		memset(m_pAvcBuffer, 0, iWidth * iHeight);

		m_pic.img.plane[0] = NULL;
		m_pic.img.plane[1] = NULL;
		m_pic.img.plane[2] = NULL;
		m_pic.img.plane[3] = NULL;
		x264_picture_alloc(&m_pic, X264_CSP_I420, iWidth, iHeight);
		//SPS PPS

		x264_nal_t *nal = NULL;
		int nnal = 0;
		uint8_t *pSPS = NULL;
		int iSPS = 0;
		uint8_t *pPPS = NULL;
		int iPPS = 0;
		x264_encoder_headers(m_pCtx, &nal, &nnal);
		for (int i = 0; i < nnal; i++) {
			if (nal[i].i_type == 7) {
				iSPS = nal[i].i_payload - 4;
				pSPS = nal[i].p_payload + 4;
			}
			if (nal[i].i_type == 8) {
				iPPS = nal[i].i_payload - 4;
				pPPS = nal[i].p_payload + 4;
			}
		}

		m_extrasize = iSPS + iPPS + 16;
		m_extradata = (uint8_t*)malloc(m_extrasize);
		m_extradata[0] = 0x17;
		m_extradata[1] = 0x00;
		m_extradata[2] = 0x00;
		m_extradata[3] = 0x00;
		m_extradata[4] = 0x00;
		m_extradata[5] = 0x01;
		m_extradata[6] = pSPS[1];
		m_extradata[7] = pSPS[2];
		m_extradata[8] = pSPS[3];
		m_extradata[9] = 0xff;
		m_extradata[10] = 0xe1;
		m_extradata[11] = (iSPS >> 8) & 0xff;
		m_extradata[12] = iSPS & 0xff;
		memcpy(m_extradata + 13, pSPS, iSPS);
		m_extradata[13 + iSPS] = 1;
		m_extradata[14 + iSPS] = (iPPS >> 8) & 0xff;
		m_extradata[15 + iSPS] = iPPS & 0xff;
		memcpy(m_extradata + 16 + iSPS, pPPS, iPPS);

		LOGE("H264 %s [%dx%d] OK", __FUNCTION__, iWidth, iHeight);

		pOut = m_extradata;
		outlen = m_extrasize;
		return 1;
	}

	//裁剪编码
	int EncodeNV21(int64_t pts, uint8_t* data, int src_w, int src_h, uint8_t*& pOut, int& outlen) {
		pOut = nullptr;
		outlen = 0;
		if (!m_pCtx || !data) {
			return -1;
		}
		int dx = (src_w - m_iWidth) / 2;
		int dy = (src_h - m_iHeight) / 2;
		uint8_t* pY = data;
		uint8_t* pNV21 = data + src_w * src_h;
		libyuv::NV21ToI420(
			pY + (dy   * src_w) + dx, src_w,
			pNV21 + (dy / 2 * src_w) + dx, src_w,
			m_pic.img.plane[0], m_pic.img.i_stride[0],
			m_pic.img.plane[1], m_pic.img.i_stride[1],
			m_pic.img.plane[2], m_pic.img.i_stride[2],
			m_iWidth, m_iHeight
		);
		x264_nal_t *nal = NULL;
		int i_nal = 0;
		x264_picture_t pic_out;
		m_pic.i_pts = pts;
		int i_frame_size = x264_encoder_encode(m_pCtx, &nal, &i_nal, &m_pic, &pic_out);
		if (i_frame_size < 0) {
			return -2;
		}

		//FLV
		m_pAvcBuffer[0] = pic_out.i_type == X264_TYPE_IDR ? 0x17 : 0x27;
		m_pAvcBuffer[1] = 0x01;//
		m_pAvcBuffer[2] = 0x00;
		m_pAvcBuffer[3] = 0x00;
		m_pAvcBuffer[4] = 0x00;//
		memcpy(m_pAvcBuffer + 5, nal[0].p_payload, i_frame_size);
		pOut = m_pAvcBuffer;
		outlen = i_frame_size + 5;
		return outlen;
	}

	void Close() {
		LOGE("AVCEncoder %s Start",__FUNCTION__);
		if (m_pCtx) {
			LOGE("AVCEncoder %s x264_picture_clean", __FUNCTION__);
			x264_picture_clean(&m_pic);
			LOGE("AVCEncoder %s x264_encoder_close", __FUNCTION__);
			x264_encoder_close(m_pCtx);
			m_pCtx = NULL;
		}
		if (m_pAvcBuffer) {
			LOGE("AVCEncoder %s delete m_pAvcBuffer", __FUNCTION__);
			free(m_pAvcBuffer);
			m_pAvcBuffer = nullptr;
		}
		if (m_extradata) {
			LOGE("AVCEncoder %s delete m_extradata", __FUNCTION__);
			free(m_extradata);
			m_extradata = nullptr;
		}
		LOGE("AVCEncoder %s Stop", __FUNCTION__);
	}
};

class AACEncoder
{
	faacEncHandle m_h = NULL;//
	int *m_chanmap = NULL;
	unsigned long m_nMax = 0;//
	unsigned long m_samplesInput = 0;
	int m_samplerate = 48000;
	int m_channel = 2;
	uint8_t *m_pout = NULL;
	int m_nOutsize = 0; //
	short m_ex = 0;
private:

	int GetSRIndex(unsigned int sampleRate)
	{
		if (92017 <= sampleRate) return 0;
		if (75132 <= sampleRate) return 1;
		if (55426 <= sampleRate) return 2;
		if (46009 <= sampleRate) return 3;
		if (37566 <= sampleRate) return 4;
		if (27713 <= sampleRate) return 5;
		if (23004 <= sampleRate) return 6;
		if (18783 <= sampleRate) return 7;
		if (13856 <= sampleRate) return 8;
		if (11502 <= sampleRate) return 9;
		if (9391 <= sampleRate) return 10;
		return 11;
	}

public:
	int   Open(int samplerate, int channels, uint8_t *&pOut, int &iOutSize)
	{
		LOGE("FAAC Open [%dx%d]", samplerate, channels);
		pOut = NULL;
		iOutSize = 0;
		m_h = faacEncOpen(samplerate, channels, &m_samplesInput, &m_nMax);
		if (m_h == NULL)
		{
			return 0;
		}
		m_samplerate = samplerate;
		m_channel = channels;
		m_pout = (uint8_t*)malloc(m_nMax);

		faacEncConfigurationPtr cfg = faacEncGetCurrentConfiguration(m_h);
		cfg->aacObjectType = LOW;
		cfg->mpegVersion = MPEG4;
		cfg->useTns = 0;
		cfg->allowMidside = 1;
		cfg->bandWidth = samplerate / 2;

		cfg->bitRate = s_nAudioBitrate ? s_nAudioBitrate * 1000 / channels : 128 * 1000 / channels;
		//cfg->quantqual = 80;

		cfg->outputFormat = 0;// FLV

		cfg->inputFormat = FAAC_INPUT_16BIT;
		if (!faacEncSetConfiguration(m_h, cfg))
		{
			return 0;
		}

		int SRindex = GetSRIndex(samplerate);
		m_ex = (cfg->aacObjectType << 11) | (SRindex << 7) | (m_channel << 3);
		m_pout[0] = 0xAF;
		m_pout[1] = 0;
		m_pout[2] = (uint8_t)(m_ex >> 8);
		m_pout[3] = (uint8_t)m_ex;
		pOut = m_pout;
		iOutSize = 4;
		LOGE("FAAC Open OK [%dx%d]", samplerate, channels);
		return 1;
	}

	void  Encode(short *pbuf, uint8_t *&pOut, int &iOutSize){
		pOut = NULL;
		iOutSize = 0;
		m_pout[0] = 0xAF;
		m_pout[1] = 1;
		int encsize = faacEncEncode(m_h, (int *)pbuf, m_samplesInput, m_pout + 2, m_nMax);//������
		pOut = m_pout;
		iOutSize = encsize + 2;
	}

	void  Close(){
		LOGE("FAAC Close Start");
		if (m_h){
			faacEncClose(m_h);
			m_h = NULL;
		}
		if (m_pout) {
			free(m_pout);
			m_pout = nullptr;
		}
		LOGE("FAAC Close Stop");
	}
};

extern "C" JNIEXPORT void JNICALL Java_com_apowersoft_WXMedia_RtmpSender_SetMetadata(JNIEnv *env, jobject _obj,
	jint bSetMeta, jint width, jint height, jint nVideoBitrate,jint nAudioBitrate) {

	LOGE("%s Size=[%dx%d] VideoBitrate=[%d] AudioBitrate=[%d]",
		__FUNCTION__, width, height, nVideoBitrate, nAudioBitrate);

	if (bSetMeta) {
		s_bSetMeta = 1;
		s_nMetaWidth = width;
		s_nMetaHeight = height;
		s_nMetaVideoBitrate = nVideoBitrate;
		s_nMetaAudioBitrate = nAudioBitrate;
	}else {
		s_bSetMeta = 0;
		s_nMetaWidth = 0;
		s_nMetaHeight = 0;
		s_nMetaVideoBitrate = 0;
		s_nMetaAudioBitrate = 0;
	}
}


extern "C" JNIEXPORT void JNICALL Java_com_apowersoft_WXMedia_RtmpSender_SetMetadata2(JNIEnv *env, jobject _obj,
	jint bSetMeta, jint width, jint height,  jint fps, jint nVideoBitrate,jint nAudioBitrate) {

	LOGE("%s Size=[%dx%d] VideoBitrate=[%d] AudioBitrate=[%d]",
		__FUNCTION__, width, height, nVideoBitrate, nAudioBitrate);

	if (bSetMeta) {
		s_bSetMeta = 1;
		s_nMetaWidth = width;
		s_nMetaHeight = height;
		s_nFps = fps;
		s_nMetaVideoBitrate = nVideoBitrate;
		s_nMetaAudioBitrate = nAudioBitrate;
	}else {
		s_bSetMeta = 0;
		s_nMetaWidth = 0;
		s_nMetaHeight = 0;
		s_nFps = 25;
		s_nMetaVideoBitrate = 0;
		s_nMetaAudioBitrate = 0;
	}
}

class RTMP_Sender{
	WXLocker m_mutexRTMP;
	RTMP *m_r = NULL;
	AVCEncoder *m_avc = nullptr;
	AACEncoder *m_aac = nullptr;

	int64_t m_ptsStart = 0;
	int64_t m_num = 0;
	int m_iWidth = 0;
	int m_iHeight = 0;
	int m_nSampleRate = 0;
	int m_nChannel = 0;

	void SendMeta() {
		WXAutoLock al(m_mutexRTMP);
		LOGE("SetMetaData Begin");
		RTMPPacket packet;
		RTMPPacket_Reset(&packet);
		memset(&packet, 0, sizeof(RTMPPacket));
		char pbuf[2048], *pend = pbuf + sizeof(pbuf);
		packet.m_nChannel = 0x03;                   // control channel (invoke)
		packet.m_headerType = RTMP_PACKET_SIZE_MEDIUM;
		packet.m_packetType = RTMP_PACKET_TYPE_INFO;
		packet.m_nTimeStamp = 0;
		packet.m_nInfoField2 = m_r->m_stream_id;
		packet.m_hasAbsTimestamp = TRUE;
		packet.m_body = pbuf + RTMP_MAX_HEADER_SIZE;

		char *enc = packet.m_body;
		enc = AMF_EncodeString(enc, pend, &av_setDataFrame);
		enc = AMF_EncodeString(enc, pend, &av_onMetaData);

		*enc++ = AMF_OBJECT;

		enc = AMF_EncodeNamedNumber(enc, pend, &av_duration, 0.0);
		enc = AMF_EncodeNamedNumber(enc, pend, &av_fileSize, 0.0);

		// videosize
		// video
		enc = AMF_EncodeNamedString(enc, pend, &av_videocodecid, &av_avc1);

		if (s_bSetMeta) {
			enc = AMF_EncodeNamedNumber(enc, pend, &av_width, s_nMetaWidth);
			enc = AMF_EncodeNamedNumber(enc, pend, &av_height, s_nMetaHeight);
			enc = AMF_EncodeNamedNumber(enc, pend, &av_videodatarate, s_nMetaVideoBitrate ? float(s_nMetaVideoBitrate) : 1350.0);
			enc = AMF_EncodeNamedNumber(enc, pend, &av_framerate, float(s_nFps));
		}
		else {
			enc = AMF_EncodeNamedNumber(enc, pend, &av_width, m_iWidth);
			enc = AMF_EncodeNamedNumber(enc, pend, &av_height, m_iHeight);
			enc = AMF_EncodeNamedNumber(enc, pend, &av_videodatarate, s_nVideoBitrate ? float(s_nVideoBitrate) : 1350.0);
			enc = AMF_EncodeNamedNumber(enc, pend, &av_framerate, float(s_nFps));
		}

;

		// audio
		enc = AMF_EncodeNamedString(enc, pend, &av_audiocodecid, &av_mp4a);
		
		if (s_bSetMeta) {
			enc = AMF_EncodeNamedNumber(enc, pend, &av_audiodatarate, s_nMetaAudioBitrate ? float(s_nMetaAudioBitrate) : 128.0);
		}else {
			enc = AMF_EncodeNamedNumber(enc, pend, &av_audiodatarate, s_nAudioBitrate ? float(s_nAudioBitrate) : 128.0);
		}


		enc = AMF_EncodeNamedNumber(enc, pend, &av_audiosamplerate, m_nSampleRate);
		enc = AMF_EncodeNamedNumber(enc, pend, &av_audiosamplesize, 16.0);
		enc = AMF_EncodeNamedBoolean(enc, pend, &av_stereo, m_nChannel == 2);

		// sdk version
		enc = AMF_EncodeNamedString(enc, pend, &av_encoder, &av_SDKVersion);

		*enc++ = 0;
		*enc++ = 0;
		*enc++ = AMF_OBJECT_END;

		packet.m_nBodySize = (uint32_t)(enc - packet.m_body);
		RTMP_SendPacket(m_r, &packet, 0);
		LOGE("SetMetaData End");
	}

	void SendData(uint8_t *buf, int buf_size, int64_t dwTime, int bVideo){
		WXAutoLock al(m_mutexRTMP);
		RTMPPacket packet;
		RTMPPacket_Reset(&packet);
		memset(&packet, 0, sizeof(RTMPPacket));
		packet.m_nChannel = 0x04;
		packet.m_headerType = RTMP_PACKET_SIZE_LARGE;
		packet.m_nTimeStamp = 0;
		packet.m_nInfoField2 = m_r->m_stream_id;
		packet.m_hasAbsTimestamp = 0;
		packet.m_packetType = bVideo ? RTMP_PACKET_TYPE_VIDEO : RTMP_PACKET_TYPE_AUDIO;   /* VIDEO */
		packet.m_body = (char*)buf;
		packet.m_nBodySize = buf_size;
		packet.m_nTimeStamp = (uint32_t)dwTime;
		RTMP_SendPacket(m_r, &packet, 0);
		if (dwTime < 3000) {
			LOGE("%s ts=[%d] video=[%d] Size=[%d]",__FUNCTION__,(int)dwTime, bVideo, buf_size);
		}
	}

	void SetAudioParam(int sample_rate, int channel) {
		if (m_aac == nullptr) {
			m_aac = new AACEncoder;
			uint8_t *pOut = NULL;
			int outsize = 0;
			int ret = m_aac->Open(sample_rate, channel, pOut, outsize);
			if (ret) {
				m_nSampleRate = sample_rate;
				m_nChannel = channel;
				SendData(pOut, outsize, 0, false);
				LOGE("Send AAC  Extra sample_rate=%d channel=%d", sample_rate, channel);
			}else {
				delete m_aac;
				m_aac = nullptr;
			}
		}
	}

	void EncodeAudio(uint8_t *buf, int data_size) {
		if (m_aac) {
			uint8_t *pOut = NULL;
			int iOutSize = 0;
			int64_t tsAudio = 1024 * m_num * 1000 / 48000;
			m_num++;
			m_aac->Encode((short*)buf, pOut, iOutSize);
			if (iOutSize) {
				SendData(pOut, iOutSize, tsAudio, 0);
			}
		}
	}

	void SetVideoParam(int width, int height) {
		if (m_iWidth != width || m_iHeight != height) {
			if (m_avc) {
				LOGE("Destroy Old H264 Encoder [%dx%d]", m_iWidth, m_iHeight);
				m_avc->Close();
				delete m_avc;
				m_avc = nullptr;
			}
		}
		if (m_avc == nullptr) {
			LOGE("Create New H264 Encoder [%dx%d]", width, height);
			m_avc = new AVCEncoder;
			uint8_t *pOut = NULL;
			int outsize = 0;
			int ret = m_avc->Open(width, height, pOut, outsize);
			if (ret) {
				m_iWidth = width;
				m_iHeight = height;
				SendMeta();
				SendData(pOut, outsize, 0, true);
				LOGE("Create New H264 Encoder [%dx%d] OK", width, height);
				LOGE("Send H264  Extra [%dx%d]", width, height);
			}else {
				LOGE("Create New H264 Encoder [%dx%d] Error", width, height);
				delete m_avc;
				m_avc = nullptr;
			}
		}
	}

	void EncodeNV21(uint8_t* data, int src_w, int src_h, int dst_w, int dst_h, int64_t pts) {
		SetVideoParam(dst_w, dst_h);
		if (m_avc) {
			uint8_t* pOut = nullptr;
			int iOutSize = 0;
			m_avc->EncodeNV21(pts, data, src_w,src_h, pOut, iOutSize);
			if (iOutSize) {
				SendData(pOut, iOutSize, pts - m_ptsStart, 1);
			}
		}
	}

	void AudioProcess() {
		LOGE("%s Begin", __FUNCTION__ );
		while (!s_nStop){
			AudioBuffer *audio_obj = AudioPop();
			if (nullptr == audio_obj) {
				usleep(5000);
				continue;
			}else {
				EncodeAudio(audio_obj->m_pData, audio_obj->m_nSize);
				delete audio_obj;
			}
		}
		s_nA = 0;
		CloseImpl();
	}

	void VideoProcess() {
		LOGE("%s Begin", __FUNCTION__);
		while (!s_nStop){
			VideoBuffer *video_obj = VideoPop();
			if (nullptr == video_obj) {
				usleep(5 * 1000);
				continue;
			}else {
				EncodeNV21(video_obj->m_pData, video_obj->m_nWidth, video_obj->m_nHeight,video_obj->m_nClipWidth,video_obj->m_nClipHeight, video_obj->m_pts);
				delete video_obj;
			}
		}
		s_nV = 0;
		CloseImpl();
	}

	void CloseImpl() {
		WXAutoLock al(m_mutexRTMP);
		LOGE("%s !!!", __FUNCTION__);
		if (s_handle && s_nA == 0 && s_nV == 0) {

			LOGE("%s AudioStop ", __FUNCTION__);
			if (m_aac) {
				m_aac->Close();
				delete m_aac;
				m_aac = nullptr;
				m_nSampleRate = 0;
				m_nChannel = 0;
			}
			LOGE("%s AudioClean ", __FUNCTION__);
			AudioClean();

			LOGE("%s VideoStop ", __FUNCTION__);
			if (m_avc) {
				LOGE("%s AvcClose ", __FUNCTION__);
				m_avc->Close();
				LOGE("%s AvcDelete ", __FUNCTION__);
				delete m_avc;
				m_avc = nullptr;
				m_iWidth = 0;
				m_iHeight = 0;
			}
			LOGE("%s VideoClean ", __FUNCTION__);
			VideoClean();

			LOGE("%s RtmpClean ", __FUNCTION__);
			if (m_r) {
				LOGE("%s RTMP_Close ", __FUNCTION__);
				RTMP_Close(m_r);
				LOGE("%s RTMP_Free ", __FUNCTION__);
				RTMP_Free(m_r);
				m_r = NULL;
			}
			WXAutoLock al(s_lckHandle);
			s_handle = 0;
			LOGE("%s Thread Video Audio Stop!!! OK", __FUNCTION__);
		}
	}
public:
	int Open(const char *URL, int width, int height, int sample_rate, int channel){
		LOGE("%s %s begin",__FUNCTION__, URL);
		LOGE("URL = %s", URL);
		LOGE("%dx%d",width,height);
		LOGE("%dx%d",sample_rate,channel);
		m_r = RTMP_Alloc();
		RTMP_Init(m_r);
		int err = RTMP_SetupURL(m_r, (char*)URL);
		if (err <= 0){
		    LOGE("%s RTMP_SetupURL Error", __FUNCTION__);
			return 0;
		}
		RTMP_EnableWrite(m_r);
		err = RTMP_Connect(m_r, NULL);
		if (err <= 0){
		    LOGE("%s RTMP_Connect Error", __FUNCTION__);
			return 0;
		}
		err = RTMP_ConnectStream(m_r, 0);
		if (err <= 0){
		    LOGE("%s RTMP_ConnectStream Error", __FUNCTION__);
			return 0;
		}
		m_ptsStart = WXGetTimeMs();
		SetAudioParam(sample_rate, channel);
		SetVideoParam(width, height);

		s_nStop = 0;//

		//音频线程
		s_nA = 1;
		std::thread threadAudio(&RTMP_Sender::AudioProcess, this);
		threadAudio.detach();

		//视频线程
		s_nV = 1;
		std::thread threadVideo(&RTMP_Sender::VideoProcess, this);
		threadVideo.detach();

		LOGE("%s %s OK", __FUNCTION__, URL);
		return 1;
	}
};


extern "C" JNIEXPORT void JNICALL Java_com_apowersoft_WXMedia_RtmpSender_SetVideoBitrate(JNIEnv *env, jobject _obj, jint nBitrate) {
	s_nVideoBitrate =nBitrate;
}
extern "C" JNIEXPORT void JNICALL Java_com_apowersoft_WXMedia_RtmpSender_SetAudioBitrate(JNIEnv *env, jobject _obj, jint nBitrate) {
	s_nAudioBitrate =nBitrate;
}

extern "C" JNIEXPORT jint JNICALL Java_com_apowersoft_WXMedia_RtmpSender_IsRunning(JNIEnv *env, jobject _obj) {
	WXAutoLock al(s_lckHandle);
	return s_handle != 0;
}

extern "C" JNIEXPORT jlong JNICALL Java_com_apowersoft_WXMedia_RtmpSender_Connect(JNIEnv *env, jobject _obj,  jstring strURL, jint width, jint height, jint sample_rate,jint channel){
	LOGE("%s AAAA!!!",__FUNCTION__);

	WXAutoLock al(s_lckHandle);

	char*   szURL = (char*)env->GetStringUTFChars(strURL, 0);

	s_obj = new RTMP_Sender;
	int ret = s_obj->Open(szURL, width, height, sample_rate, channel);
	if (ret) {
		WXAutoLock al(s_lckHandle);
		s_handle = (jlong)s_obj;
		LOGE("%s OK !!!", __FUNCTION__);
	}
	else {
		s_obj = nullptr;
		LOGE("%s ERROR !!!", __FUNCTION__);
	}
	return s_handle;
}


extern "C" JNIEXPORT void JNICALL Java_com_apowersoft_WXMedia_RtmpSender_SendNV21(JNIEnv * env, jobject _obj, jlong handle, jbyteArray data, jint width, jint height) {
	if (!s_nStop) {
		uint8_t* pData = (uint8_t*)(env->GetByteArrayElements(data, 0));
		VideoPush(WXGetTimeMs(), pData, width, height, width, height);
		env->ReleaseByteArrayElements(data, (jbyte*)pData, 0);
	}
}

extern "C" JNIEXPORT void JNICALL Java_com_apowersoft_WXMedia_RtmpSender_ClipNV21(JNIEnv * env, jobject _obj, jlong handle, jbyteArray data, jint width, jint height, jint dst_w, jint dst_h) {
	if (!s_nStop) {
		uint8_t* pData = (uint8_t*)(env->GetByteArrayElements(data, 0));
		VideoPush(WXGetTimeMs(), pData, width, height, dst_w, dst_h);
		env->ReleaseByteArrayElements(data, (jbyte*)pData, 0);
	}
}

extern "C" JNIEXPORT void JNICALL Java_com_apowersoft_WXMedia_RtmpSender_SendPCM(JNIEnv *env, jobject _obj,  jlong handle, jbyteArray data, jint data_size){
	if (!s_nStop) {
		uint8_t *pData = (uint8_t *)(env->GetByteArrayElements(data, 0));
		AudioPush(data_size, pData);
		env->ReleaseByteArrayElements(data, (jbyte*)pData, 0);
	}
}

extern "C" JNIEXPORT void JNICALL Java_com_apowersoft_WXMedia_RtmpSender_Disconnect(JNIEnv *env, jobject _obj,  jlong handle){
	LOGE("%s Start!!!", __FUNCTION__);
	if (!s_nStop) {
		s_nStop = 1;
		while (true) {
			WXAutoLock al(s_lckHandle);
			if (s_handle == 0) {
				if (s_obj) {
					delete s_obj;
					s_obj = nullptr;
					LOGE("---- %s delete s_obj   OK!!!", __FUNCTION__);
				}
				break;
			}else {
				usleep(1000 * 10);
			}
		}
	}
	LOGE("%s Stop!!!", __FUNCTION__);
}