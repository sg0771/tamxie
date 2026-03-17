#include "opus_impl.h"

#define OPUS_SAMPLERATE 16000
#define OPUS_CHANNEL 1
#define OPUS_MODE  OPUS_APPLICATION_VOIP
#define OPUS_BITRATE  16000

#define FRAME_DATASIZE 1600
#define FRAME_PRE_PACKET 5  //A Packet has 5 Frame
#define FRAME_SIZE 160  //10ms

struct MSOpusEncoder {
	OpusEncoder *encoder;
	uint8_t* m_buf;
	int m_bufsize;
};
struct MSOpusEncoder* MSOpusEncoder_Create(){
	MSOpusEncoder *enc = ms_new0(MSOpusEncoder,1);
	int error = 0;
	enc->encoder = opus_encoder_create(OPUS_SAMPLERATE, OPUS_CHANNEL,  OPUS_MODE, &error);
	error = opus_encoder_ctl(enc->encoder, OPUS_SET_BITRATE(OPUS_BITRATE));
	error = opus_encoder_ctl(enc->encoder, OPUS_SET_FORCE_CHANNELS(OPUS_AUTO));
	opus_int32	enc_bw;
	error = opus_encoder_ctl(enc->encoder, OPUS_GET_BANDWIDTH(&enc_bw));
	opus_int32	complexity;
	error = opus_encoder_ctl(enc->encoder, OPUS_GET_COMPLEXITY(&complexity));
	opus_int32	lostperc = 0;
	error = opus_encoder_ctl(enc->encoder, OPUS_SET_PACKET_LOSS_PERC(0));
	error = opus_encoder_ctl(enc->encoder, OPUS_SET_EXPERT_FRAME_DURATION(OPUS_FRAMESIZE_10_MS));//10ms 一帧
	error = opus_encoder_ctl(enc->encoder, OPUS_SET_INBAND_FEC(0));
#ifdef __arm__
	error = opus_encoder_ctl(enc->encoder, OPUS_SET_COMPLEXITY(0));
#endif
	enc->m_buf = ms_malloc0(FRAME_DATASIZE);
	return enc;
}
void MSOpusEncoder_Destroy(struct MSOpusEncoder *enc){
	opus_encoder_destroy(enc->encoder);
	ms_free(enc->m_buf);
	ms_free(enc);
}
mblk_t *MSOpusEncoder_Encode(struct MSOpusEncoder *enc, int16_t *pcm){
	if(enc == NULL || pcm == NULL)return NULL;
	memset(enc->m_buf,0,1600);
	enc->m_buf[0]  = FRAME_PRE_PACKET;
	enc->m_bufsize = 1 + FRAME_PRE_PACKET;
	int i;
	for (i = 0; i < FRAME_PRE_PACKET; i++) { 
		opus_int32 ret = opus_encode(enc->encoder, pcm + FRAME_SIZE * i, FRAME_SIZE, enc->m_buf + enc->m_bufsize, 500);
		enc->m_buf[i + 1] = ret;
		enc->m_bufsize += ret;
	}
	mblk_t *om = allocb(enc->m_bufsize, 0);
	memcpy(om->b_wptr, enc->m_buf, enc->m_bufsize);
	om->b_wptr += enc->m_bufsize;
	return om;
}

int  MSOpusEncoder_Encode2(MSOpusEncoder *enc, int16_t *pcm, uint8_t *encbuf){
	if(enc == NULL || pcm == NULL || encbuf == NULL)return 0;
	memset(enc->m_buf,0,1600);
	enc->m_buf[0]  = FRAME_PRE_PACKET;
	enc->m_bufsize = 1 + FRAME_PRE_PACKET;
	int i;
	for (i = 0; i < FRAME_PRE_PACKET; i++) { 
		opus_int32 ret = opus_encode(enc->encoder, pcm + FRAME_SIZE * i, FRAME_SIZE, enc->m_buf + enc->m_bufsize, 500);
		enc->m_buf[i + 1] = ret;
		enc->m_bufsize += ret;
	}
	memcpy(encbuf, enc->m_buf, enc->m_bufsize);
	return enc->m_bufsize;
}

//=========================================================================================
struct MSOpusDecoder {
	OpusDecoder *decoder;
} ;
struct MSOpusDecoder* MSOpusDecoder_Create(){
	MSOpusDecoder *dec = ms_new0(MSOpusDecoder, 1);
	int error = 0;
	dec->decoder = opus_decoder_create(OPUS_SAMPLERATE, OPUS_CHANNEL, &error);
	int value = 0;
	int res = opus_decoder_ctl(dec->decoder, OPUS_GET_GAIN(&value));
	return dec;
}
void MSOpusDecoder_Destroy(struct MSOpusDecoder *dec){
	opus_decoder_destroy(dec->decoder);
	ms_free(dec);
}

mblk_t *MSOpusDecoder_Decode(struct MSOpusDecoder *dec, uint8_t *buf){
	int packets = buf[0];
	if (packets != FRAME_PRE_PACKET) {
		ms_warning("数据帧不对！！ %d", packets);
		return NULL;
	}
	int pos[5] = {0};
	pos[0] = 1 + FRAME_PRE_PACKET;
	pos[1] = pos[0] + buf[1];
	pos[2] = pos[1] + buf[2];
	pos[3] = pos[2] + buf[3];
	pos[4] = pos[3] + buf[4];

	mblk_t *om = allocb(FRAME_DATASIZE, 0);
	int16_t *pcm = (int16_t*)om->b_wptr;
	int i;
	for (i = 0; i < FRAME_PRE_PACKET; i++) {
		uint8_t *enc_data = buf + pos[i];
		int   enc_size = buf[i + 1];
		int res = opus_decode(dec->decoder, enc_data, enc_size, pcm + i * FRAME_SIZE, FRAME_SIZE, 0);
	}
	om->b_wptr += FRAME_DATASIZE;
	return om;
}

int  MSOpusDecoder_Decode2(MSOpusDecoder *dec, uint8_t *encbuf, int16_t *decpcm){
	int packets = encbuf[0];
	if (packets != FRAME_PRE_PACKET) {
		ms_warning("数据帧不对！！ %d", packets);
		return 0;
	}
	int pos[5] = {0};
	pos[0] = 1 + FRAME_PRE_PACKET;
	pos[1] = pos[0] + encbuf[1];
	pos[2] = pos[1] + encbuf[2];
	pos[3] = pos[2] + encbuf[3];
	pos[4] = pos[3] + encbuf[4];
	int i;
	for (i = 0; i < FRAME_PRE_PACKET; i++) {
		uint8_t *enc_data = encbuf + pos[i];
		int   enc_size = encbuf[i + 1];
		int res = opus_decode(dec->decoder, enc_data, enc_size, decpcm + i * FRAME_SIZE, FRAME_SIZE, 0);
	}
	return 1;
}