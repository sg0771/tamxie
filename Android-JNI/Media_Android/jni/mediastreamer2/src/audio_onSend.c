#include "mediastreamer2/msticker.h"

typedef struct _SendData{
	void   *sink;//回调句柄
	cb_onSend func;//回调函数
}SendData;

static void sender_init(MSFilter *f){
	SendData *s = (SendData*)ms_malloc(sizeof(SendData));
	s->func = NULL;
	s->sink = NULL;
	f->data = s;
}

static void sender_uninit(MSFilter *f){
	SendData *s = (SendData *)f->data;
	s->func = NULL;
	ms_free(s);
}

//这个filter直接连接发送，没有output
static void sender_process(MSFilter *f){
	ms_filter_lock(f);
	SendData *s = (SendData *)f->data;
	mblk_t *im = NULL;
	while ((im = ms_queue_get(f->inputs[0])) != NULL) {
		int len = im->b_wptr - im->b_rptr;
		if (s->func) {
			s->func(s->sink, im->b_rptr, len);//发送回调函数
		}
		freemsg(im);
	}
	ms_filter_unlock(f);
}


//回调对象
static int sender_setsink(MSFilter *f, void* arg) {
	SendData  *s = (SendData *)f->data;
	s->sink = arg;
	return 0;
}
//回调函数
static int sender_setcb(MSFilter *f,  void* func) {
	SendData  *s = (SendData *)f->data;
	s->func = (cb_onSend)func;
	return 0;
}
static MSFilterMethod sender_methods[] = {
	{ MS_SENDER_SETSINK	,	sender_setsink },
	{ MS_SENDER_SETCB	,	sender_setcb },
	{ 0		        ,	NULL }
};

MSFilterDesc ms_audio_sender_desc ={
	MS_AUDIO_SENDER_ID,
	"MSAudioSender",
	N_("A filter that Audio Data to Send"),
	MS_FILTER_OTHER,
	NULL,
	1,
	0,
	sender_init,
	NULL,
	sender_process,
	NULL,
	sender_uninit,
	sender_methods,
	MS_FILTER_IS_PUMP
};

MS_FILTER_DESC_EXPORT(ms_audio_sender_desc)
