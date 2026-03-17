#include <mediastreamer2/msfilter.h>
#include <mediastreamer2/msticker.h>
#include <mediastreamer2/mssndcard.h>

#include <sys/types.h>
#include <string.h>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <SLES/OpenSLES_AndroidConfiguration.h>
#include <jni.h>
#include <dlfcn.h>

static  MSFilter *filter_read =NULL;
static  MSFilter *filter_write =NULL;

static MSSndCard *android_snd_card_new(void);
static MSFilter *ms_android_snd_read_new(void);
static MSFilter *ms_android_snd_write_new(void);

struct OpenSLESContext {
	OpenSLESContext() {
		samplerate = 16000;
		nchannels = 1;
	}
	int samplerate;
	int nchannels;
	SLObjectItf engineObject;
	SLEngineItf engineEngine;
};

struct SLRead {
	SLRead() {
		streamType = SL_ANDROID_RECORDING_PRESET_VOICE_COMMUNICATION;
		inBufSize = 1600;
		qinit(&read_queue);
		ms_mutex_init(&read_mutex,NULL);
		mTickerSynchronizer = NULL;
		started = FALSE;
		currentBuffer = 0;
		recBuffer[0] = (uint8_t *) calloc(inBufSize, sizeof(uint8_t));
		recBuffer[1] = (uint8_t *) calloc(inBufSize, sizeof(uint8_t));
	}
	~SLRead() {
		free(recBuffer[0]);
		free(recBuffer[1]);
		flushq(&read_queue,0);
		ms_mutex_destroy(&read_mutex);
	}
	void setContext(OpenSLESContext *context) {
		opensles_context = context;
	}
	OpenSLESContext *opensles_context;
	SLObjectItf recorderObject;
	SLRecordItf recorderRecord;
	SLAndroidSimpleBufferQueueItf recorderBufferQueue;
	SLAndroidConfigurationItf recorderConfig;
	SLint32 streamType;
	queue_t       read_queue;
	ms_mutex_t read_mutex;
	MSTickerSynchronizer *mTickerSynchronizer;
	MSFilter *mFilter;
	int64_t read_samples;
	uint8_t *recBuffer[2];
	int inBufSize;
	int currentBuffer;
	bool_t started;
};

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

#define PACK_NUM 800
struct SLWrite {
	//缓存通道
	struct WriteNode{
		MSBufferizer bufferizer;
		int16_t *input;
		int       m_max;
		WriteNode(){
			ms_bufferizer_init(&bufferizer);
			input=(int16_t*)ms_malloc0(PACK_NUM*sizeof(int16_t));
		}
		~WriteNode(){
			ms_free(input);
			ms_bufferizer_uninit(&bufferizer);
		}
		int channel_process_in(int32_t *sum, int nsamples){
			memset(input,0,nsamples*2);
			m_max = 0;
			if (ms_bufferizer_read(&bufferizer, (uint8_t*)input, nsamples*2 )!=0){
				int i = 0;
				for(i = 0; i < nsamples; i++){
					if(abs(input[i]) > m_max)m_max = abs(input[i]);
				}
				if(m_max > 200){
					//ms_error("Mixer Channel [%p] MaxValue = %d",input,m_max);
					accumulate(sum, input, nsamples);
					return nsamples;
				}else{
					m_max = 0;
				}
			}
			return 0;
		}
	} ;
	WriteNode m_play[MIXER_MAX_CHANNELS];
	int32_t *m_sum;
	//再取数据的时候回调
	bool_t MixerProcess(){
		bool_t got_something = FALSE;
		memset(m_sum, 0, PACK_NUM*sizeof(int32_t));
		for(int i=0; i < MIXER_MAX_CHANNELS;++i){
			if (m_play[i].channel_process_in(m_sum, PACK_NUM)){
				got_something=TRUE; 
			}
		}
		if(got_something){
			//ms_error("Mixer Audio Data!!!");
			int16_t *dest_pcm = (int16_t*)playBuffer[currentBuffer];
			for(int i =0; i < PACK_NUM; i++ ){
				dest_pcm[i] = saturate(m_sum[i]);
			}
		}else{
			//ms_error("Silence Audio Data!!!");
			memset(playBuffer[currentBuffer],0,PACK_NUM*2);
		}
		return got_something;
	}
	SLWrite() {
		streamType = SL_ANDROID_STREAM_VOICE;
		nbufs = 0;
		outBufSize = PACK_NUM * sizeof(int16_t);
		ms_mutex_init(&write_mutex,NULL);
		currentBuffer = 0;
		m_sum = (int32_t*)ms_malloc0(PACK_NUM*sizeof(int32_t));
		playBuffer[0] = (uint8_t *) ms_malloc0(outBufSize);
		playBuffer[1] = (uint8_t *) ms_malloc0(outBufSize);
	}
	~SLWrite() {
		ms_free(playBuffer[0]);
		ms_free(playBuffer[1]);
		ms_free(m_sum);
		ms_mutex_destroy(&write_mutex);
	}
	void setContext(OpenSLESContext *context) {
		opensles_context = context;
	}
	OpenSLESContext *opensles_context;
	SLObjectItf outputMixObject;
	SLObjectItf playerObject;
	SLPlayItf playerPlay;
	SLAndroidSimpleBufferQueueItf playerBufferQueue;
	SLAndroidConfigurationItf playerConfig;
	SLint32 streamType;
	
	ms_mutex_t write_mutex;
	
	int nbufs;
	uint8_t *playBuffer[2];
	int outBufSize;
	int currentBuffer;
};

MS2_PUBLIC int SLGetMax(int channel){
	if(filter_write == NULL)return 0;
	SLWrite *octx = (SLWrite*)filter_write->data;
	return octx->m_play[channel].m_max;
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

MS2_PUBLIC extern "C" void SLInputAudio(int channel, mblk_t*m){
	if(filter_write == NULL)return;
	SLWrite *octx = (SLWrite*)filter_write->data;
	int avg = ABS_AVG((int16_t*)m->b_rptr,800);
	ms_mutex_lock(&octx->write_mutex);
	int packet = octx->m_play[channel].bufferizer.q.q_mcount;
	if(packet > 15){
		ms_error("++++++ %d Packet in the channel  [%d]",packet ,channel);
	}
	if(packet > 40){
		ms_error("AAAA %d Packet in the channel  [%d] , Do not input Audio Data",packet ,channel);
		ms_mutex_unlock(&octx->write_mutex);
		return ;
	}
	if(packet > 10 && avg < 256){
		ms_error("BBBB %d Packet in the channel  [%d] , Do not input Sience Audio Data, avg = %d",packet ,channel,avg);
		ms_mutex_unlock(&octx->write_mutex);
		return;
	}
	ms_bufferizer_put(&octx->m_play[channel].bufferizer,m);
	ms_mutex_unlock(&octx->write_mutex);	
}

static void android_snd_card_detect(MSSndCardManager *m) {
	MSSndCard *card = android_snd_card_new();
	ms_snd_card_manager_add_card(m, card);
}

static SLresult opensles_engine_init(OpenSLESContext *ctx) {
	SLresult result;
	result = slCreateEngine(&(ctx->engineObject), 0, NULL, 0, NULL, NULL);
	result = (*ctx->engineObject)->Realize(ctx->engineObject, SL_BOOLEAN_FALSE);
	result = (*ctx->engineObject)->GetInterface(ctx->engineObject, SL_IID_ENGINE, &(ctx->engineEngine));
	return result;
}

static OpenSLESContext* opensles_context_init() {
	OpenSLESContext* ctx = new OpenSLESContext();
	opensles_engine_init(ctx);
	return ctx;
} 

static void android_native_snd_card_init(MSSndCard *card) {
}

static void android_native_snd_card_uninit(MSSndCard *card) {
	OpenSLESContext *ctx = (OpenSLESContext*)card->data;
	ms_warning("Deletion of OpenSLES context [%p]", ctx);
	if (ctx->engineObject != NULL) {
                (*ctx->engineObject)->Destroy(ctx->engineObject);
                ctx->engineObject = NULL;
                ctx->engineEngine = NULL;
        }
}

static SLresult opensles_recorder_init(SLRead *ictx) {
	SLresult result;
	SLDataLocator_IODevice loc_dev = {
		SL_DATALOCATOR_IODEVICE,
		SL_IODEVICE_AUDIOINPUT,
		SL_DEFAULTDEVICEID_AUDIOINPUT,
		NULL
	};
	SLDataSource audio_src = {&loc_dev,NULL};
	SLDataLocator_AndroidSimpleBufferQueue loc_bq = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,2};
	SLDataFormat_PCM format_pcm = {
		SL_DATAFORMAT_PCM,
		1,
		SL_SAMPLINGRATE_16,
		SL_PCMSAMPLEFORMAT_FIXED_16,
		SL_PCMSAMPLEFORMAT_FIXED_16,
		SL_SPEAKER_FRONT_CENTER,
		SL_BYTEORDER_LITTLEENDIAN
	};
	SLDataSink audio_sink = {&loc_bq,&format_pcm};
	const SLInterfaceID ids[] = {SL_IID_ANDROIDSIMPLEBUFFERQUEUE,SL_IID_ANDROIDCONFIGURATION};
	const SLboolean req[] = {SL_BOOLEAN_TRUE,SL_BOOLEAN_TRUE};
	result = (*ictx->opensles_context->engineEngine)->CreateAudioRecorder(ictx->opensles_context->engineEngine, &ictx->recorderObject, &audio_src, &audio_sink, 2, ids, req);
	result = (*ictx->recorderObject)->GetInterface(ictx->recorderObject, SL_IID_ANDROIDCONFIGURATION, &ictx->recorderConfig);
	result = (*ictx->recorderConfig)->SetConfiguration(ictx->recorderConfig, SL_ANDROID_KEY_RECORDING_PRESET, &ictx->streamType, sizeof(SLint32));
	result = (*ictx->recorderObject)->Realize(ictx->recorderObject, SL_BOOLEAN_FALSE);
	result = (*ictx->recorderObject)->GetInterface(ictx->recorderObject, SL_IID_RECORD, &ictx->recorderRecord);
	result = (*ictx->recorderObject)->GetInterface(ictx->recorderObject, SL_IID_ANDROIDSIMPLEBUFFERQUEUE, &ictx->recorderBufferQueue);
	return result;
}

static void compute_timespec(SLRead *ictx) {
	uint64_t ns = ((1000 * ictx->read_samples) / (uint64_t) ictx->opensles_context->samplerate) * 1000000;
	MSTimeSpec ts;
	ts.tv_nsec = ns % 1000000000;
	ts.tv_sec = ns / 1000000000;
	ictx->mTickerSynchronizer != NULL ? ms_ticker_synchronizer_set_external_time(ictx->mTickerSynchronizer, &ts) : 0.0;
}

static void opensles_recorder_callback(SLAndroidSimpleBufferQueueItf bq, void *context) {
	SLRead *ictx = (SLRead *)context;
	if (!ictx->started)return;
	if (ictx->mTickerSynchronizer == NULL) {
		MSFilter *obj = ictx->mFilter;
		ictx->mTickerSynchronizer = ms_ticker_synchronizer_new();
		ms_ticker_set_time_func(obj->ticker, (uint64_t (*)(void*))ms_ticker_synchronizer_get_corrected_time, ictx->mTickerSynchronizer);
	}
	ictx->read_samples += ictx->inBufSize / sizeof(int16_t);
	mblk_t *m = allocb(ictx->inBufSize, 0);
	memcpy(m->b_wptr, ictx->recBuffer[ictx->currentBuffer], ictx->inBufSize);
	m->b_wptr += ictx->inBufSize;
	ms_mutex_lock(&ictx->read_mutex);
	int packet_num = ictx->read_queue.q_mcount;
	if(packet_num < 20){
		putq(&ictx->read_queue, m);
 	}
 	compute_timespec(ictx);
 	ms_mutex_unlock(&ictx->read_mutex);
 	(*ictx->recorderBufferQueue)->Enqueue(ictx->recorderBufferQueue, ictx->recBuffer[ictx->currentBuffer], ictx->inBufSize);
	ictx->currentBuffer = ictx->currentBuffer == 1 ? 0 : 1;
}

static SLresult opensles_recorder_callback_init(SLRead *ictx) {
	SLresult result;
	result = (*ictx->recorderBufferQueue)->RegisterCallback(ictx->recorderBufferQueue, opensles_recorder_callback, ictx);
	result = (*ictx->recorderRecord)->SetRecordState(ictx->recorderRecord, SL_RECORDSTATE_STOPPED);
	result = (*ictx->recorderBufferQueue)->Clear(ictx->recorderBufferQueue);
	result = (*ictx->recorderRecord)->SetRecordState(ictx->recorderRecord, SL_RECORDSTATE_RECORDING);
	result = (*ictx->recorderBufferQueue)->Enqueue(ictx->recorderBufferQueue, ictx->recBuffer[0], ictx->inBufSize);
	result = (*ictx->recorderBufferQueue)->Enqueue(ictx->recorderBufferQueue, ictx->recBuffer[1], ictx->inBufSize);
	return result;
}

static SLRead* opensles_input_context_init() {
	SLRead* ictx = new SLRead();
	return ictx;
}

static void android_snd_read_init(MSFilter *obj) {
	SLRead *ictx = opensles_input_context_init();
	obj->data = ictx;
	filter_read = obj;
}

static void android_snd_read_preprocess(MSFilter *obj) {
	SLRead *ictx = (SLRead*) obj->data;
	ictx->mFilter = obj;
	ictx->read_samples = 0;
	ictx->started = TRUE;
	opensles_recorder_init(ictx);
	opensles_recorder_callback_init(ictx);
	filter_read = obj;
}

static void android_snd_read_process(MSFilter *obj) {
//无输出
}

static void android_snd_read_postprocess(MSFilter *obj) {
	filter_read = NULL;
	SLRead *ictx = (SLRead*)obj->data;
	if (ictx)ictx->started = FALSE;
	(*ictx->recorderRecord)->SetRecordState(ictx->recorderRecord, SL_RECORDSTATE_STOPPED);
	(*ictx->recorderBufferQueue)->Enqueue(ictx->recorderBufferQueue, ictx->recBuffer[0], 0);
	(*ictx->recorderBufferQueue)->Enqueue(ictx->recorderBufferQueue, ictx->recBuffer[1], 0);
	 (*ictx->recorderBufferQueue)->Clear(ictx->recorderBufferQueue);
	if (ictx->recorderObject != NULL) {
		(*ictx->recorderObject)->Destroy(ictx->recorderObject);
		ictx->recorderObject = NULL;
		ictx->recorderRecord = NULL;
		ictx->recorderBufferQueue = NULL;
	}
	ms_ticker_set_time_func(obj->ticker, NULL, NULL);
	ms_mutex_lock(&ictx->read_mutex);
	ms_ticker_synchronizer_destroy(ictx->mTickerSynchronizer);
	ictx->mTickerSynchronizer = NULL;
	ms_mutex_unlock(&ictx->read_mutex);
}

static void android_snd_read_uninit(MSFilter *obj) {
	filter_read = NULL;
	SLRead *ictx = (SLRead*)obj->data;
	delete ictx;
}

static MSFilterMethod android_snd_read_methods[] = {
	{0,NULL}
};

MSFilterDesc android_snd_opensles_read_desc = {
	MS_FILTER_PLUGIN_ID,
	"MSOpenSLESRecorder",
	"android sound source",
	MS_FILTER_OTHER,
	NULL,
	0,
	1,//虚拟输出端
	android_snd_read_init,
	android_snd_read_preprocess,
	android_snd_read_process,
	android_snd_read_postprocess,
	android_snd_read_uninit,
	android_snd_read_methods
};

MS2_PUBLIC extern "C"   mblk_t* GetCaptureData(){
	if(filter_read == NULL)return NULL;
	SLRead *ictx = (SLRead*)filter_read->data;
	mblk_t *m = NULL;
	ms_mutex_lock(&ictx->read_mutex);
	m = getq(&ictx->read_queue);
	ms_mutex_unlock(&ictx->read_mutex);
	return m;
}

static MSFilter* ms_android_snd_read_new() {
	MSFilter *f = ms_filter_new_from_desc(&android_snd_opensles_read_desc);
	return f;
}

static MSFilter *android_snd_card_create_reader(MSSndCard *card) {
	MSFilter *f = ms_android_snd_read_new();
	SLRead *ictx = static_cast<SLRead*>(f->data);
	ictx->setContext((OpenSLESContext*)card->data);
	return f;
}

static SLresult opensles_mixer_init(SLWrite *octx) {
	SLresult result;
        const SLuint32 nbInterface = 0;
        const SLInterfaceID ids[] = {};
        const SLboolean req[] = {};
        result = (*octx->opensles_context->engineEngine)->CreateOutputMix( octx->opensles_context->engineEngine,  &(octx->outputMixObject),  nbInterface,ids, req);
        result = (*octx->outputMixObject)->Realize(octx->outputMixObject, SL_BOOLEAN_FALSE);
        return result;
}

static SLresult opensles_sink_init(SLWrite *octx) {
	SLresult result;
        SLDataFormat_PCM format_pcm = {
		SL_DATAFORMAT_PCM,
		1,
		 SL_SAMPLINGRATE_16,
		SL_PCMSAMPLEFORMAT_FIXED_16,
		SL_PCMSAMPLEFORMAT_FIXED_16,
		SL_SPEAKER_FRONT_CENTER,
		SL_BYTEORDER_LITTLEENDIAN
	};
        SLDataLocator_AndroidSimpleBufferQueue loc_bufq = { SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2   };
        SLDataSource audio_src = {&loc_bufq,  &format_pcm   };
        SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, octx->outputMixObject};
        SLDataSink audio_sink = {&loc_outmix,NULL};
        const SLuint32 nbInterface = 3;
        const SLInterfaceID ids[] = { SL_IID_VOLUME, SL_IID_ANDROIDSIMPLEBUFFERQUEUE, SL_IID_ANDROIDCONFIGURATION };
        const SLboolean req[] = { SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE };
        result = (*octx->opensles_context->engineEngine)->CreateAudioPlayer(octx->opensles_context->engineEngine,&(octx->playerObject),&audio_src, &audio_sink, nbInterface, ids,req );
	result = (*octx->playerObject)->GetInterface(octx->playerObject, SL_IID_ANDROIDCONFIGURATION, &octx->playerConfig);
	result = (*octx->playerConfig)->SetConfiguration(octx->playerConfig, SL_ANDROID_KEY_STREAM_TYPE, &octx->streamType, sizeof(SLint32));
	result = (*octx->playerObject)->Realize(octx->playerObject, SL_BOOLEAN_FALSE);
	result = (*octx->playerObject)->GetInterface(octx->playerObject, SL_IID_PLAY, &(octx->playerPlay));
	result = (*octx->playerObject)->GetInterface(octx->playerObject, SL_IID_ANDROIDSIMPLEBUFFERQUEUE, &(octx->playerBufferQueue));
	return result;
}

static void opensles_player_callback(SLAndroidSimpleBufferQueueItf bq, void* context) {
	SLWrite *octx = (SLWrite*)context;
	ms_mutex_lock(&octx->write_mutex);
	bool_t bMixer = octx->MixerProcess();	//取混音数据到octx->playBuffer[octx->currentBuffer] ，没有就memset0
	if(bMixer && octx->nbufs == 0)(*octx->playerBufferQueue)->Clear(octx->playerBufferQueue); //第一个有效数据，清空队列
	int ask = octx->outBufSize;
	static  int64_t g_ts_silence = 0;
	if (bMixer) {
		if(g_ts_silence >  600)
			(*octx->playerBufferQueue)->Clear(octx->playerBufferQueue);  //长期静音之后忽然来了数据，强制把数据输入到声卡的最前端
		g_ts_silence = 0;
		octx->nbufs++;
	} else{
		ask /= 5; 
		g_ts_silence += 10;
	}
	ms_mutex_unlock(&octx->write_mutex);
 	(*octx->playerBufferQueue)->Enqueue(octx->playerBufferQueue, octx->playBuffer[octx->currentBuffer], ask);
	octx->currentBuffer = octx->currentBuffer == 1 ? 0 : 1;
}

static SLresult opensles_player_callback_init(SLWrite *octx) {
	SLresult result;
	result = (*octx->playerPlay)->SetPlayState(octx->playerPlay, SL_PLAYSTATE_STOPPED);
	result = (*octx->playerBufferQueue)->Clear(octx->playerBufferQueue);
        result = (*octx->playerBufferQueue)->RegisterCallback(octx->playerBufferQueue, opensles_player_callback, octx);
	result = (*octx->playerBufferQueue)->Enqueue(octx->playerBufferQueue, octx->playBuffer[0], octx->outBufSize);
	result = (*octx->playerBufferQueue)->Enqueue(octx->playerBufferQueue, octx->playBuffer[1], octx->outBufSize);
        result = (*octx->playerPlay)->SetPlayState(octx->playerPlay, SL_PLAYSTATE_PLAYING);
        return result;
}

static SLWrite* opensles_output_context_init() {
	SLWrite* octx = new SLWrite();
	return octx;
}

static MSFilter *android_snd_card_create_writer(MSSndCard *card) {
	MSFilter *f = ms_android_snd_write_new();
	SLWrite *octx = static_cast<SLWrite*>(f->data);
	octx->setContext((OpenSLESContext*)card->data);
	return f;
}

static void android_snd_write_init(MSFilter *obj){
	filter_write = obj;
	SLWrite *octx = opensles_output_context_init();
	obj->data = octx;
}

static void android_snd_write_uninit(MSFilter *obj){
	filter_write = NULL;
	SLWrite *octx = (SLWrite*)obj->data;
	delete octx;
}


static void android_snd_write_preprocess(MSFilter *obj) {
	filter_write = obj;
	SLWrite *octx = (SLWrite*)obj->data;
        octx->currentBuffer = 0;
	opensles_mixer_init(octx);
	opensles_sink_init(octx);
	opensles_player_callback_init(octx);
	octx->nbufs = 0;
}

static void android_snd_write_process(MSFilter *obj) {
	//外部输入
}

static void android_snd_write_postprocess(MSFilter *obj) {
	filter_write = NULL;
	SLWrite *octx = (SLWrite*)obj->data;
	 (*octx->playerPlay)->SetPlayState(octx->playerPlay, SL_PLAYSTATE_STOPPED);
	(*octx->playerBufferQueue)->Clear(octx->playerBufferQueue);
	if (octx->playerObject != NULL){
                (*octx->playerObject)->Destroy(octx->playerObject);
                octx->playerObject = NULL;
                octx->playerPlay = NULL;
                octx->playerBufferQueue = NULL;
        }
	if (octx->outputMixObject != NULL) {
                (*octx->outputMixObject)->Destroy(octx->outputMixObject);
                octx->outputMixObject = NULL;
        }
}

static MSFilterMethod android_snd_write_methods[] = {
	{0,NULL}
};

MSFilterDesc android_snd_opensles_write_desc = {
	MS_FILTER_PLUGIN_ID,
	"MSOpenSLESPlayer",
	"android sound output",
	MS_FILTER_OTHER,
	NULL,
	1,
	0,
	android_snd_write_init,
	android_snd_write_preprocess,
	android_snd_write_process,
	android_snd_write_postprocess,
	android_snd_write_uninit,
	android_snd_write_methods
};

static MSFilter* ms_android_snd_write_new(void) {
	MSFilter *f = ms_filter_new_from_desc(&android_snd_opensles_write_desc);
	return f;
}

MSSndCardDesc android_native_snd_opensles_card_desc = {
	"openSLES",
	android_snd_card_detect,
	android_native_snd_card_init,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	android_snd_card_create_reader,
	android_snd_card_create_writer,
	android_native_snd_card_uninit
};

static MSSndCard* android_snd_card_new(void) {
	MSSndCard* obj;
	obj = ms_snd_card_new(&android_native_snd_opensles_card_desc);
	obj->name = ms_strdup("android opensles sound card");
	OpenSLESContext *context = opensles_context_init();
	obj->capabilities |= MS_SND_CARD_CAP_BUILTIN_ECHO_CANCELLER;
	obj->latency = 0; 
	obj->data = context;
	return obj;
}