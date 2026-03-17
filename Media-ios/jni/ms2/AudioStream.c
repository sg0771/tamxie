#include "ms2.h"
#include <mediastreamer2/mscommon.h>
#include <mediastreamer2/msticker.h>
#include <mediastreamer2/msfilter.h>
#include <mediastreamer2/mssndcard.h>

#define MAX_CHANNEL 100

int ms2_LocalMax = 0;

extern int gCaptureMax;//采集最大值
struct MSAudioStream {
	//采集部分
	MSFilter *read_cap;      //声音采集Filter  source
	MSFilter *encoder;			  //opus编码器Filter
	MSFilter *sender;             //输出到Filter
	MSTicker *ticker1;               //定时器
		
	 //接收端
	MSFilter *  mixer;                //解码后的混音器 filter
	MSFilter *  write_cap;            //播放端
	MSTicker *ticker2;               //定时器
	
	bool_t *used;
	bool_t bPcm;

	queue_t *q;
	ms_mutex_t mutex;
};

//ms2回调函数
void onReadData(void *param, uint8_t *buf, int size) {
	MSAudioStream *stream = (MSAudioStream *)param;
	//if(stream->q.q_mcount >= 20)return;
	ms_mutex_lock(&stream->mutex);
	mblk_t *im = allocb(size, 0);
	memcpy(im->b_wptr, buf, size);
	im->b_wptr += size;
	putq(stream->q, im);
	ms_mutex_unlock(&stream->mutex);
}

int  MsAudioStream_Sender_GetData(MSAudioStream *stream, uint8_t *buf) {
	int len = 0;
	ms_mutex_lock(&stream->mutex);
	mblk_t *m = getq(stream->q);
	if (m) {
		len = m->b_wptr - m->b_rptr;
		memcpy(buf, m->b_rptr, len);
	}
	ms_mutex_unlock(&stream->mutex);
	return len;
}
//========================================================
MSAudioStream* MSAudioStream_Create(int bPcm) {
	ms_warning("MSAudioStream_Create");
	MSAudioStream *stream = ms_new0(MSAudioStream,1);;
	int i;
	stream->used = ms_new0(bool_t, MAX_CHANNEL);
	for (i = 0; i < MAX_CHANNEL; i++) {
		stream->used[i] = FALSE;
	}
	
	MSSndCardManager *manager = ms_snd_card_manager_get();
	//本地采集编码
	MSSndCard * card_capture = ms_snd_card_manager_get_default_capture_card(manager);//声卡对象
	stream->read_cap = ms_snd_card_create_reader(card_capture);//声卡filter
	stream->sender = ms_filter_new(MS_AUDIO_SENDER_ID);//发送filter
	stream->mixer = ms_filter_new(MS_AUDIO_MIXER_ID);//混音器
	
	ms_error("Read Filter");
	MSSndCard *card_play = ms_snd_card_manager_get_default_playback_card(manager);//播放声卡
	stream->write_cap = ms_snd_card_create_writer(card_play);//播放filter
	ms_error("Write Filter");
	
	if (bPcm) {  //PCM 测试
		stream->bPcm = TRUE;
		ms_filter_link(stream->read_cap, 0, stream->sender, 0);//录音
	}else { //OPUS 多路解码并混音
		stream->bPcm = FALSE;
		stream->encoder = ms_filter_new(MS_OPUS_ENC_ID);
		ms_filter_link(stream->read_cap, 0, stream->encoder, 0);
		ms_filter_link(stream->encoder, 0, stream->sender, 0);
	}
	ms_filter_link(stream->mixer, 0, stream->write_cap, 0);
		ms_error("LINK");
			
	stream->q = ms_new0(queue_t, 1);
	qinit(stream->q);
	ms_mutex_init(&stream->mutex, NULL);//初始化锁
	ms_filter_call_method(stream->sender, MS_SENDER_SETSINK, (void*)stream);
	ms_filter_call_method(stream->sender, MS_SENDER_SETCB, (void*)onReadData);
	
	stream->ticker1 = ms_ticker_new(10);//定时器
	ms_ticker_attach(stream->ticker1, stream->read_cap);
	ms_error("tick1");
	stream->ticker2 = ms_ticker_new(5);//定时器
	ms_ticker_attach(stream->ticker2, stream->mixer);
	ms_error("tick2");	
	return stream;
}

void MSAudioStream_Destroy(MSAudioStream *stream) {
	ms_ticker_detach(stream->ticker1, stream->read_cap);
	ms_ticker_detach(stream->ticker2, stream->mixer);	
	ms_ticker_destroy(stream->ticker1);
	ms_ticker_destroy(stream->ticker2);	
	
	flushq(stream->q,0);
	ms_free(stream->q);

	if (stream->bPcm) {
		ms_filter_unlink(stream->read_cap, 0, stream->sender, 0);//录音
	}else {
		ms_filter_unlink(stream->read_cap, 0, stream->encoder, 0);
		ms_filter_unlink(stream->encoder, 0, stream->sender, 0);
		ms_filter_destroy(stream->encoder);
	}
	ms_filter_unlink(stream->mixer, 0, stream->write_cap, 0);

	ms_free(stream->used);
	ms_filter_destroy(stream->read_cap);
	ms_filter_destroy(stream->sender);
	ms_filter_destroy(stream->mixer);
	ms_filter_destroy(stream->write_cap);
	ms_free(stream);
	stream = NULL;
	return;
}

int MSAudioStream_Receive_Add(MSAudioStream *stream) {
	int channel = -1;
	int i;
	for (i = 0; i < MAX_CHANNEL; i++) {
		if (!stream->used[i]) {
			channel = i;
			stream->used[channel] = TRUE;
			break;
		}
	}
	return channel;
}

void MSAudioStream_Receive_Remove(MSAudioStream *stream, int channel) {
	if (channel > -1 && channel < MAX_CHANNEL) {
		stream->used[channel] = FALSE;
	}
}

void MSAudioStream_Receive_WriteData(MSAudioStream *stream, int channel, uint8_t *buf, int size) {
	if (channel > -1 && channel < MAX_CHANNEL && stream->used[channel]) {
		MSAudioNode arg;
		arg.size = size;
		arg.buf = buf;
		arg.channel = channel;
		arg.pcm = stream->bPcm;
		ms_filter_call_method(stream->mixer, MS_MIXER_PUTDATA, (void*)&arg);//填充数据
	}
}

//某一路的最大值
extern int GetChannelMax(int channel);// form mixer
int MSAudioStream_Receive_GetMax(MSAudioStream *stream,int channel){
	if (channel > -1 && channel < MAX_CHANNEL && stream->used[channel]) 
		return GetChannelMax(channel);
	return 0;
}

//本地采集最大值
int    MSAudioStream_Receive_GetLocalMax     (MSAudioStream *stream){
	return gCaptureMax;
}


