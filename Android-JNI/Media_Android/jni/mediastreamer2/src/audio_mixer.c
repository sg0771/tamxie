#include "mediastreamer2/mscommon.h"
#include "mediastreamer2/msticker.h"
#include <math.h>
#include "opus_impl.h"

#define PACK_NUM 800 //800 short

extern int gs_nBufferSize;

static void accumulate(int32_t *sum, int16_t* contrib, int nwords){
	int i;
	for(i=0;i<nwords;++i){
		sum[i]+=contrib[i];
	}
}

static inline int16_t saturate(int32_t s){
	if (s>32767) return 32767;
	if (s<-32767) return -32767;
	return (int16_t)s;
}

//缂撳瓨閫氶亾
typedef struct Channel{
	MSBufferizer bufferizer;
	int16_t *input;
	MSOpusDecoder *decoder;
	int m_max;
} Channel;

static void channel_init(Channel *chan){
	ms_bufferizer_init(&chan->bufferizer);
	chan->input=ms_malloc0(PACK_NUM * 2);
	chan->decoder = MSOpusDecoder_Create();
}

static void channel_uninit(Channel *chan){
	ms_free(chan->input);
	ms_bufferizer_uninit(&chan->bufferizer);
	MSOpusDecoder_Destroy(chan->decoder);
}

static int GetAudioMax(int16_t *pcm, int count){
	int i;
	int max_v = 0;
	for(i = 0; i < count; i++)
		if(abs(pcm[i]) > max_v)max_v = abs(pcm[i]);
	return max_v; 
}
static int channel_process_in(Channel *chan,  int32_t *sum, int nsamples){
	if (ms_bufferizer_read(&chan->bufferizer, (uint8_t*)chan->input, nsamples*2 )!=0){
		chan->m_max = GetAudioMax(chan->input, nsamples);
		//ms_error("Chan Max Value = %d" , chan->m_max);
		accumulate(sum,chan->input,nsamples);
		return nsamples;
	}else memset(chan->input,0,nsamples*2);
	return 0;
}

typedef struct MixerState{
	Channel channels[MIXER_MAX_CHANNELS];
	int32_t *sum;//输出缓冲区
	ms_mutex_t mutex;
} MixerState;

static MSFilter *filter_mixer = NULL;
static void mixer_init(MSFilter *f){
	filter_mixer = f;
	MixerState *s=ms_new0(MixerState,1);
	int i;
	for(i=0;i<MIXER_MAX_CHANNELS;++i){
		channel_init(&s->channels[i]);
	}
	s->sum=(int32_t*)ms_malloc0(PACK_NUM * 4);	
	ms_mutex_init(&s->mutex,NULL);
	f->data=s;
}

int GetChannelMax(int channel){
	if(filter_mixer == NULL)return 0;
	MixerState *s=(MixerState *)filter_mixer->data;
	int maxv =  s->channels[channel].m_max;
	 s->channels[channel].m_max = 0;
	 return maxv;
}

static void mixer_uninit(MSFilter *f){
	filter_mixer = NULL;
	int i;
	MixerState *s=(MixerState *)f->data;
	for(i=0;i<MIXER_MAX_CHANNELS;++i){
		channel_uninit(&s->channels[i]);
	}
	ms_mutex_destroy(&s->mutex);
	ms_free(s);
}

//考虑归一化算法
static mblk_t *make_output(MixerState *s,  int nwords){
	mblk_t *om=allocb(nwords*2,0);
	int i;
	for(i=0;i<nwords;++i,om->b_wptr+=2){
		*(int16_t*)om->b_wptr = saturate(s->sum[i]);
	}
	return om;
}

static void mixer_process(MSFilter *f){
	MixerState *s=(MixerState *)f->data;

	ms_mutex_lock(&s->mutex);
	if(gs_nBufferSize > 2){
		ms_mutex_unlock(&s->mutex);
		return;
	}
	// ms_error("mixer_process %d", gs_nBufferSize);
	ms_mutex_unlock(&s->mutex);
	int i;
	int nwords = PACK_NUM;
	bool_t got_something=FALSE;
	memset(s->sum,  0, nwords*sizeof(int32_t));
	
	ms_mutex_lock(&s->mutex);
	for(i=0;i<MIXER_MAX_CHANNELS;++i){
		if (channel_process_in(&s->channels[i], s->sum,nwords)){
			got_something=TRUE; 
			gs_nBufferSize++;
		}
	}
	ms_mutex_unlock(&s->mutex);
	
	if (got_something){
		mblk_t *om=make_output(s,nwords);
		ms_queue_put(f->outputs[0],om);
	}
}


static int ABS_AVG(int16_t *pcm, int count){
	int i;
	int64_t sum2 = 0.0;
	for(i = 0; i < count; i++){
		sum2 += abs(pcm[i]) ;
	}
	int avg = sum2 / count;
	return avg;
}

//浠庡閮ㄥ線鎸囧畾绠￠亾鍐欏叆鏁版嵁
static int mixer_put(MSFilter *f, void  *arg) {

	MixerState *s=(MixerState *)f->data;
	MSAudioNode *p = (MSAudioNode*)arg;
	mblk_t *m = NULL;
	if(p->pcm){
		m= allocb(p->size, 0);
		memcpy(m->b_wptr, p->buf, p->size);
		m->b_wptr += p->size;
	}else{
		//Opus data
		m = MSOpusDecoder_Decode(s->channels[p->channel].decoder, p->buf);
	}
	int avg = ABS_AVG((int16_t*)m->b_rptr,800);//256 一下可以认为是静音数据
	
	int packet = s->channels[p->channel].bufferizer.q.q_mcount;
	//ms_error("channel [%d] has %d packet, input packet ABS_AVG = %d", p->channel, packet,avg);
	
	if(packet > 20)return 0;//通道缓冲了20个数据包，不保存通道缓冲了20个数据包，不保存
	if(packet > 5 && avg < 512)return 0;//通道缓存了多个数据包，后面就跳过静音包
	
	ms_mutex_lock(&s->mutex);
	ms_bufferizer_put(&s->channels[p->channel].bufferizer, m);//往指定通道输入数据
	ms_mutex_unlock(&s->mutex);
	return 0;
}


static MSFilterMethod methods[] = {
	{ MS_MIXER_PUTDATA	,	mixer_put },
	{ 0 ,NULL }
};


MSFilterDesc ms_audio_mixer_desc={
	.id=MS_AUDIO_MIXER_ID,
	.name="MSAudioMixer",
	.text=N_("A filter that mixes down 16 bit sample audio streams"),
	.category=MS_FILTER_OTHER,
	.ninputs=0,
	.noutputs=1, 
	.init=mixer_init,
	.preprocess=NULL,
	.process=mixer_process,
	.postprocess=NULL,
	.uninit=mixer_uninit,
	.methods=methods,
	.flags=MS_FILTER_IS_PUMP
};

MS_FILTER_DESC_EXPORT(ms_audio_mixer_desc)
