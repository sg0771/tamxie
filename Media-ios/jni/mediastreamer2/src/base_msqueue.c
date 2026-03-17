#include "mediastreamer2/msqueue.h"
#include <string.h>

MSQueue * ms_queue_new(struct _MSFilter *f1, int pin1, struct _MSFilter *f2, int pin2 ){
	MSQueue *q=(MSQueue*)ms_new(MSQueue,1);
	qinit(&q->q);
	q->prev.filter=f1;
	q->prev.pin=pin1;
	q->next.filter=f2;
	q->next.pin=pin2;
	return q;
}

void ms_queue_init(MSQueue *q){
	q->prev.filter=0;
	q->prev.pin=0;
	q->next.filter=0;
	q->next.pin=0;
	qinit(&q->q);
}

void ms_queue_destroy(MSQueue *q){
	flushq(&q->q,0);
	ms_free(q);
}

void ms_queue_flush(MSQueue *q){
	flushq(&q->q,0);
}


void ms_bufferizer_init(MSBufferizer *obj){
	qinit(&obj->q);
	obj->size=0;
}

MSBufferizer * ms_bufferizer_new(){
	MSBufferizer *obj=(MSBufferizer *)ms_new(MSBufferizer,1);
	ms_bufferizer_init(obj);
	return obj;
}

void ms_bufferizer_put(MSBufferizer *obj, mblk_t *m){
	obj->size+=msgdsize(m);
	putq(&obj->q,m);
}

void ms_bufferizer_put_from_queue(MSBufferizer *obj, MSQueue *q){
	mblk_t *m;
	while((m=ms_queue_get(q))!=NULL){
		ms_bufferizer_put(obj,m);
	}
}

int ms_bufferizer_read(MSBufferizer *obj, uint8_t *data, int datalen){
	if (obj->size>=datalen){
		int sz=0;
		int cplen;
		mblk_t *m=peekq(&obj->q);
		/*we can return something */
		while(sz<datalen){
			cplen=MIN(m->b_wptr-m->b_rptr,datalen-sz);
			memcpy(data+sz,m->b_rptr,cplen);
			sz+=cplen;
			m->b_rptr+=cplen;
			if (m->b_rptr==m->b_wptr){
				/* check cont */
				if (m->b_cont!=NULL) {
					m=m->b_cont;
				}
				else{
					mblk_t *remove=getq(&obj->q);
					freemsg(remove);
					m=peekq(&obj->q);
				}
			}
		}
		obj->size-=datalen;
		return datalen;
	}
	return 0;
}

void ms_bufferizer_skip_bytes(MSBufferizer *obj, int bytes){
	uint8_t *tmp=(uint8_t*)alloca(bytes);
	ms_bufferizer_read(obj,tmp,bytes);
}

void ms_bufferizer_flush(MSBufferizer *obj){
	obj->size=0;
	flushq(&obj->q,0);
}

void ms_bufferizer_uninit(MSBufferizer *obj){
	flushq(&obj->q,0);
}

void ms_bufferizer_destroy(MSBufferizer *obj){
	ms_bufferizer_uninit(obj);
	ms_free(obj);
}
