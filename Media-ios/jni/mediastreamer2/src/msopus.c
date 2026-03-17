#include "mediastreamer2/msfilter.h"
#include "mediastreamer2/mscodecutils.h"
#include "mediastreamer2/msfilter.h"
#include "mediastreamer2/msticker.h"
#include <stdint.h>
#include <opus.h>

extern int ms2_LocalMax;

#define  TIME_PRE_FRAME  10  //每帧10ms
#define  FRAME_PRE_PACKET 5 //每个包5帧
typedef struct _OpusEncData {
	MSBufferizer *bufferizer; 
	OpusEncoder *encoder;
	uint32_t ts;
	int samplerate;
	int channels;
	int application;
	int bitrate;
	uint8_t* m_buf;
	int   m_bufsize;
} OpusEncData;


static void ms_opus_enc_init(MSFilter *f) {
	OpusEncData *d = (OpusEncData *)ms_new(OpusEncData, 1);

	d->m_buf = ms_malloc0(1600);
	d->m_bufsize = 0;
	ms2_LocalMax = 0;
	d->ts = 0;
	d->bufferizer = ms_bufferizer_new();
	d->encoder = NULL;//Opus encoder
	d->samplerate = 16000;
	d->channels = 1;
	d->application = OPUS_APPLICATION_VOIP;
	d->bitrate = 16000;//16kbits
	
	f->data = d;
}

static void ms_opus_enc_preprocess(MSFilter *f) {
	int error;

	OpusEncData *d = (OpusEncData *)f->data;
	/* create the encoder */
	int err = 0;
	d->encoder = opus_encoder_create(d->samplerate, d->channels, d->application, &error);
	if (error != OPUS_OK) {
		ms_warning("Create Opus Encoder error");
		return;
	}
	err = opus_encoder_ctl(d->encoder, OPUS_SET_BITRATE(d->bitrate));//16kbits
	opus_encoder_ctl(d->encoder, OPUS_SET_FORCE_CHANNELS(OPUS_AUTO));
	opus_int32	enc_bw;
	opus_encoder_ctl(d->encoder, OPUS_GET_BANDWIDTH(&enc_bw));
	opus_int32	complexity;
	opus_encoder_ctl(d->encoder, OPUS_GET_COMPLEXITY(&complexity));
	opus_int32	lostperc = 0;
	opus_encoder_ctl(d->encoder, OPUS_SET_PACKET_LOSS_PERC(0));
	opus_encoder_ctl(d->encoder, OPUS_SET_EXPERT_FRAME_DURATION(OPUS_FRAMESIZE_10_MS));//10ms 一帧
	opus_encoder_ctl(d->encoder, OPUS_SET_INBAND_FEC(0));
	/* set complexity to 0 for arm devices */
#ifdef __arm__
	opus_encoder_ctl(d->encoder, OPUS_SET_COMPLEXITY(0));
#endif

}

//获取最大值
static int GetMaxAudio(int16_t *pcm, int len){
	int MaxValue = 0;int i;
	for(i = 0; i < len ;i ++){
		if(abs(pcm[i] > MaxValue))
			MaxValue = abs(pcm[i]);
	}
	return MaxValue;
}

static void ms_opus_enc_process(MSFilter *f) {
	OpusEncData *d = (OpusEncData *)f->data;

	//从上层filter获取PCM数据
	mblk_t *im = NULL;
	while ((im = ms_queue_get(f->inputs[0])) != NULL) {
		ms_bufferizer_put(d->bufferizer, im);
	}
	
	int frame_size = d->samplerate * TIME_PRE_FRAME / 1000; /* in samples */
	int nbytes = frame_size * 2;
	int total_bytes = nbytes * FRAME_PRE_PACKET;
	uint8_t *buf_pcm = alloca(nbytes);
	//缓冲区有50ms的数据
	while (ms_bufferizer_get_avail(d->bufferizer) >= (total_bytes)) {
		memset(d->m_buf, 0 , 1600);
		
		d->m_buf[0]  = FRAME_PRE_PACKET;
		d->m_bufsize = 1 + FRAME_PRE_PACKET;
		int i;
		for (i = 0; i < FRAME_PRE_PACKET; i++) { /* encode 20ms by 20ms and repacketize all of them together */
			ms_bufferizer_read(d->bufferizer, buf_pcm, nbytes);
			
			ms2_LocalMax = GetMaxAudio((int16_t *)buf_pcm, nbytes/2);
			
			opus_int32 ret = opus_encode(d->encoder, (opus_int16 *)buf_pcm, frame_size, d->m_buf + d->m_bufsize, 500);
			if (ret < 0) {
				ms_error("Opus encoder error: %s", opus_strerror(ret));
				break;
			}
			if (ret > 0) { //编码OK
				d->m_buf[i + 1] = ret;
				d->m_bufsize += ret;
			}
		}

		if (d->m_bufsize > 1 + FRAME_PRE_PACKET) {
			mblk_t *om = allocb(d->m_bufsize, 0);
			memcpy(om->b_wptr, d->m_buf, d->m_bufsize);
			om->b_wptr += d->m_bufsize;
			d->ts += FRAME_PRE_PACKET;
			mblk_set_timestamp_info(om, d->ts);
			ms_queue_put(f->outputs[0], om);
		}
	}
}

static void ms_opus_enc_postprocess(MSFilter *f) {
	OpusEncData *d = (OpusEncData *)f->data;
	opus_encoder_destroy(d->encoder);
	d->encoder = NULL;

	if (d->m_buf) {
		ms_free(d->m_buf);
		d->m_buf = NULL;
		d->m_bufsize = 0;
	}
}

static void ms_opus_enc_uninit(MSFilter *f) {
	OpusEncData *d = (OpusEncData *)f->data;
	if (d == NULL) return;
	if (d->encoder) {
		opus_encoder_destroy(d->encoder);
		d->encoder = NULL;
	}
	ms_bufferizer_destroy(d->bufferizer);
	d->bufferizer = NULL;

	if (d->m_buf) {
		ms_free(d->m_buf);
		d->m_buf = NULL;
		d->m_bufsize = 0;
	}

	ms_free(d);
}


static int ms_opus_enc_set_sample_rate(MSFilter *f, void *arg) {
	OpusEncData *d = (OpusEncData *)f->data;
	int samplerate = *((int *)arg);
	/* check values: supported are 8, 12, 16, 24 and 48 kHz */
	switch (samplerate) {
		case 8000:case 12000:case 16000:case 24000:case 48000:
			d->samplerate=samplerate;
			break;
		default:
			ms_error("Opus encoder got unsupported sample rate of %d, switch to default 48kHz",samplerate);
			d->samplerate=16000;
	}
	return 0;
}

static int ms_opus_enc_get_sample_rate(MSFilter *f, void *arg) {
	OpusEncData *d = (OpusEncData *)f->data;
	*((int *)arg) = d->samplerate;
	return 0;
}

static int ms_opus_enc_set_bitrate(MSFilter *f, void *arg) {
	OpusEncData *d = (OpusEncData *)f->data;
	d->bitrate = *((int *)arg);
	return 0;
}

static int ms_opus_enc_get_bitrate(MSFilter *f, void *arg) {
	OpusEncData *d = (OpusEncData *)f->data;
	*((int *)arg) = d->bitrate;
	return 0;
}



static int ms_opus_enc_set_nchannels(MSFilter *f, void *arg) {
	OpusEncData *d = (OpusEncData *)f->data;
	d->channels = *(int*)arg;
	return 0;
}

static MSFilterMethod ms_opus_enc_methods[] = {
	{	MS_FILTER_SET_SAMPLE_RATE,	ms_opus_enc_set_sample_rate	},
	{	MS_FILTER_GET_SAMPLE_RATE,	ms_opus_enc_get_sample_rate	},
	{	MS_FILTER_SET_BITRATE,		ms_opus_enc_set_bitrate		},
	{	MS_FILTER_GET_BITRATE,		ms_opus_enc_get_bitrate		},
	{	MS_FILTER_SET_NCHANNELS		,	ms_opus_enc_set_nchannels},
	{	0,				NULL				}
};

MSFilterDesc ms_opus_enc_desc = {
	MS_OPUS_ENC_ID,
	"MSOpusEnc",
	"An opus encoder.",
	MS_FILTER_ENCODER,
	"opus",
	1,//input
	1,//output
	ms_opus_enc_init,
	ms_opus_enc_preprocess,
	ms_opus_enc_process,
	ms_opus_enc_postprocess,
	ms_opus_enc_uninit,
	ms_opus_enc_methods,
	0
};
MS_FILTER_DESC_EXPORT(ms_opus_enc_desc)


/**
 * Definition of the private data structure of the opus decoder.
 */
typedef struct _OpusDecData {
	OpusDecoder *decoder;
	int samplerate;
	int channels;
} OpusDecData;


static void ms_opus_dec_init(MSFilter *f) {
	OpusDecData *d = (OpusDecData *)ms_new(OpusDecData, 1);
	d->decoder = NULL;
	d->samplerate = 16000;
	d->channels = 1;
	f->data = d;
}

static void ms_opus_dec_preprocess(MSFilter *f) {
	int error;
	OpusDecData *d = (OpusDecData *)f->data;
	d->decoder = opus_decoder_create(d->samplerate, d->channels, &error);
	if (error != OPUS_OK) {
		ms_error("Opus decoder creation failed: %s", opus_strerror(error));
	}
	int value = 0;
	opus_decoder_ctl(d->decoder, OPUS_GET_GAIN(&value));
}


static mblk_t *Wh_Decoder(OpusDecData *d, mblk_t * im) {
	uint8_t *buf = im->b_rptr;
	int bufsize = im->b_wptr - im->b_rptr;
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
	ms_warning("pos = %d %d %d %d %d", pos[0], pos[1], pos[2], pos[3], pos[4]);

	int frame_size = d->samplerate * TIME_PRE_FRAME / 1000; /* in samples */
	int nbytes = frame_size * 2;
	int total_bytes = nbytes * FRAME_PRE_PACKET;
	mblk_t *om = allocb(total_bytes, 0);
	int i;
	for (i = 0; i < FRAME_PRE_PACKET; i++) {
		uint8_t *p = buf + pos[i];
		int   size = buf[i + 1];
		opus_decode(d->decoder, p, size, (opus_int16 *)(om->b_wptr + i * nbytes), frame_size, 0);
	}
	om->b_wptr += total_bytes;
	return om;
}
static void ms_opus_dec_process(MSFilter *f) {
	OpusDecData *d = (OpusDecData *)f->data;
	mblk_t *im = NULL;
	while ((im = ms_queue_get(f->inputs[0])) != NULL) {
		mblk_t *om = Wh_Decoder(d, im);
		freemsg(im);
		if(om)
			ms_queue_put(f->outputs[0], om);
	}
}

static void ms_opus_dec_postprocess(MSFilter *f) {
	OpusDecData *d = (OpusDecData *)f->data;
	opus_decoder_destroy(d->decoder);
	d->decoder = NULL;
}

static void ms_opus_dec_uninit(MSFilter *f) {
	OpusDecData *d = (OpusDecData *)f->data;
	if (d == NULL) return;
	if (d->decoder) {
		opus_decoder_destroy(d->decoder);
		d->decoder = NULL;
	}
	ms_free(d);
}

static int ms_opus_dec_set_sample_rate(MSFilter *f, void *arg) {
	OpusDecData *d = (OpusDecData *)f->data;
	d->samplerate = *((int *)arg);
	return 0;
}

static int ms_opus_dec_get_sample_rate(MSFilter *f, void *arg) {
	OpusDecData *d = (OpusDecData *)f->data;
	*((int *)arg) = d->samplerate;
	return 0;
}

static int ms_opus_dec_set_nchannels(MSFilter *f, void *arg) {
	OpusDecData *d = (OpusDecData *)f->data;
	d->channels=*(int*)arg;
	return 0;
}

static MSFilterMethod ms_opus_dec_methods[] = {
	{	MS_FILTER_SET_SAMPLE_RATE,	ms_opus_dec_set_sample_rate	},
	{	MS_FILTER_GET_SAMPLE_RATE,	ms_opus_dec_get_sample_rate	},
	{	MS_FILTER_SET_NCHANNELS		,	ms_opus_dec_set_nchannels},
	{	0,				NULL				}
};

MSFilterDesc ms_opus_dec_desc = {
	MS_OPUS_DEC_ID,
	"MSOpusDec",
	"An opus decoder.",
	MS_FILTER_DECODER,
	"opus",
	1,
	1,
	ms_opus_dec_init,
	ms_opus_dec_preprocess,
	ms_opus_dec_process,
	ms_opus_dec_postprocess,
	ms_opus_dec_uninit,
	ms_opus_dec_methods,
	0
};
MS_FILTER_DESC_EXPORT(ms_opus_dec_desc)
