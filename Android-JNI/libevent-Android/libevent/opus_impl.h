/*
Opus 音频编解码器封装
10ms 一个数据包
*/
#include <WXBase.h>
#include <opus.h>

class MSOpusEncoder {
	OpusEncoder *m_pEncoder = nullptr;
	WXDataBuffer m_buf;

	int m_nFrameSize = 0;
public:
	MSOpusEncoder(int nSampleRate, int nChannel){
		int bitrate = nSampleRate * nChannel * 16 / 12;
		m_nFrameSize = nSampleRate / 100;
		int error = 0;
		m_pEncoder = opus_encoder_create(nSampleRate, nChannel, OPUS_APPLICATION_AUDIO, &error);
		error = opus_encoder_ctl(m_pEncoder, OPUS_SET_BITRATE(bitrate));
		error = opus_encoder_ctl(m_pEncoder, OPUS_SET_FORCE_CHANNELS(OPUS_AUTO));
		opus_int32	enc_bw;
		error = opus_encoder_ctl(m_pEncoder, OPUS_GET_BANDWIDTH(&enc_bw));
		opus_int32	complexity;
		error = opus_encoder_ctl(m_pEncoder, OPUS_GET_COMPLEXITY(&complexity));
		opus_int32	lostperc = 0;
		error = opus_encoder_ctl(m_pEncoder, OPUS_SET_PACKET_LOSS_PERC(0));
		error = opus_encoder_ctl(m_pEncoder, OPUS_SET_EXPERT_FRAME_DURATION(OPUS_FRAMESIZE_10_MS));//10ms 一帧
		error = opus_encoder_ctl(m_pEncoder, OPUS_SET_INBAND_FEC(0));
#ifdef __arm__
		error = opus_encoder_ctl(m_pEncoder, OPUS_SET_COMPLEXITY(0));
#endif
		m_buf.Init(nullptr, 4096);
	}
	virtual  ~MSOpusEncoder() {
		if (m_pEncoder) {
			opus_encoder_destroy(m_pEncoder);
			m_pEncoder = nullptr;
		}
	}

	int  EncodeFrame(uint8_t *buf_in, uint8_t **buf_out, int *out_len) {
		*buf_out = nullptr;
		*out_len = 0;
		if (buf_in == NULL || m_pEncoder == NULL)return 0;
		opus_int32 ret = opus_encode(m_pEncoder, (int16_t*)buf_in, m_nFrameSize, m_buf.m_pBuf, 4096);
		if (ret < 0)return 0;
		*buf_out = m_buf.m_pBuf;
		*out_len = ret;
		return 1;
	}
};

static void* WXOpusEncoderCreate(int inSampleRate, int inChannel) {
	MSOpusEncoder *obj = new MSOpusEncoder(inSampleRate, inChannel);
	return (void*)obj;
}

//返回1 表示有编码输出
static int   WXOpusEncoderEncodeFrame(void* ptr, uint8_t *buf_in,  uint8_t **buf_out, int *out_len) {
	MSOpusEncoder *obj = (MSOpusEncoder*)ptr;
	if (obj) {
		return obj->EncodeFrame(buf_in, buf_out, out_len);
	}
	return 0;
}
static void  WXOpusEncoderDestroy(void* ptr) {
	MSOpusEncoder *obj =  (MSOpusEncoder*)ptr;
	delete obj;
}

//=========================================================================================
class MSOpusDecoder {
	OpusDecoder *m_pDecoder = nullptr;
	WXDataBuffer m_buf;
	int m_nFrameSize = 0;
public:
	MSOpusDecoder(int nSampleRate, int nChannel) {
		int error = 0;
		m_pDecoder = opus_decoder_create(nSampleRate, nChannel, &error);
		int value = 0;
		int ret = opus_decoder_ctl(m_pDecoder, OPUS_GET_GAIN(&value));
		if (ret >= 0) {
			int size = nSampleRate * nChannel * 2 / 100;//10ms 数据量
			m_nFrameSize = nSampleRate / 100;
			m_buf.Init(nullptr, size);
		}

	}

	virtual ~MSOpusDecoder() {
		if (m_pDecoder) {
			opus_decoder_destroy(m_pDecoder);
			m_pDecoder = nullptr;
		}
	}

	int  DecodeFrame(uint8_t *encbuf,int encsize, uint8_t** decpcm, int* nOutSize) {
		*decpcm = nullptr;
		*nOutSize = 0;
		if (m_pDecoder) {
			int ret = opus_decode(m_pDecoder, encbuf, encsize, (int16_t*)m_buf.m_pBuf, m_nFrameSize, 0);
			if (ret <= 0) {
				return 0;
			}
			*decpcm = (uint8_t*)m_buf.m_pBuf;
			*nOutSize = m_buf.m_iBufSize;
			return 1;
		}
		return 0;
	}
} ;

static void* WXOpusDecoderCreate(int inSampleRate, int inChannel) {
	MSOpusDecoder *obj = new MSOpusDecoder(inSampleRate, inChannel);
	return (void*)obj;
}

//返回1 表示有解码输出
static int   WXOpusDecoderEncodeFrame(void* ptr, uint8_t *encbuf, int encsize, uint8_t** decpcm, int* nOutSize) {
	MSOpusDecoder *obj = (MSOpusDecoder*)ptr;
	if (obj) {
		return obj->DecodeFrame(encbuf, encsize, decpcm, nOutSize);
	}
	return 0;
}
static void  WXOpusDecoderDestroy(void* ptr) {
	MSOpusDecoder *obj = (MSOpusDecoder*)ptr;
	delete obj;
}
