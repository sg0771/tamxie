#include "mediastreamer2/mscommon.h"
#include "mediastreamer2/mscodecutils.h"
#include "mediastreamer2/msfilter.h"

static unsigned int cpu_count = 1;
unsigned int ms_get_cpu_count() {
	return cpu_count;
}

void ms_set_cpu_count(unsigned int c) {
	ms_message("CPU count set to %d", c);
	cpu_count = c;
}

MSList *ms_list_new(void *data){
	MSList *new_elem=(MSList *)ms_new(MSList,1);
	new_elem->prev=new_elem->next=NULL;
	new_elem->data=data;
	return new_elem;
}

MSList *ms_list_append_link(MSList *elem, MSList *new_elem){
	MSList *it=elem;
	if (elem==NULL) return new_elem;
	while (it->next!=NULL) it=ms_list_next(it);
	it->next=new_elem;
	new_elem->prev=it;
	return elem;
}

MSList * ms_list_append(MSList *elem, void * data){
	MSList *new_elem=ms_list_new(data);
	return ms_list_append_link(elem,new_elem);
}

MSList * ms_list_prepend(MSList *elem, void *data){
	MSList *new_elem=ms_list_new(data);
	if (elem!=NULL) {
		new_elem->next=elem;
		elem->prev=new_elem;
	}
	return new_elem;
}


MSList * ms_list_concat(MSList *first, MSList *second){
	MSList *it=first;
	if (it==NULL) return second;
	while(it->next!=NULL) it=ms_list_next(it);
	it->next=second;
	second->prev=it;
	return first;
}

MSList * ms_list_free(MSList *list){
	MSList *elem = list;
	MSList *tmp;
	if (list==NULL) return NULL;
	while(elem->next!=NULL) {
		tmp = elem;
		elem = elem->next;
		ms_free(tmp);
	}
	ms_free(elem);
	return NULL;
}

MSList * ms_list_remove(MSList *first, void *data){
	MSList *it;
	it=ms_list_find(first,data);
	if (it) return ms_list_remove_link(first,it);
	else {
		ms_warning("ms_list_remove: no element with %p data was in the list", data);
		return first;
	}
}

int ms_list_size(const MSList *first){
	int n=0;
	while(first!=NULL){
		++n;
		first=first->next;
	}
	return n;
}

void ms_list_for_each(const MSList *list, void (*func)(void *)){
	for(;list!=NULL;list=list->next){
		func(list->data);
	}
}

void ms_list_for_each2(const MSList *list, void (*func)(void *, void *), void *user_data){
	for(;list!=NULL;list=list->next){
		func(list->data,user_data);
	}
}

MSList *ms_list_remove_link(MSList *list, MSList *elem){
	MSList *ret;
	if (elem==list){
		ret=elem->next;
		elem->prev=NULL;
		elem->next=NULL;
		if (ret!=NULL) ret->prev=NULL;
		ms_free(elem);
		return ret;
	}
	elem->prev->next=elem->next;
	if (elem->next!=NULL) elem->next->prev=elem->prev;
	elem->next=NULL;
	elem->prev=NULL;
	ms_free(elem);
	return list;
}

MSList *ms_list_find(MSList *list, void *data){
	for(;list!=NULL;list=list->next){
		if (list->data==data) return list;
	}
	return NULL;
}

MSList *ms_list_find_custom(MSList *list, int (*compare_func)(const void *, const void*), const void *user_data){
	for(;list!=NULL;list=list->next){
		if (compare_func(list->data,user_data)==0) return list;
	}
	return NULL;
}

void * ms_list_nth_data(const MSList *list, int index){
	int i;
	for(i=0;list!=NULL;list=list->next,++i){
		if (i==index) return list->data;
	}
	ms_error("ms_list_nth_data: no such index in list.");
	return NULL;
}

int ms_list_position(const MSList *list, MSList *elem){
	int i;
	for(i=0;list!=NULL;list=list->next,++i){
		if (elem==list) return i;
	}
	ms_error("ms_list_position: no such element in list.");
	return -1;
}

int ms_list_index(const MSList *list, void *data){
	int i;
	for(i=0;list!=NULL;list=list->next,++i){
		if (data==list->data) return i;
	}
	ms_error("ms_list_index: no such element in list.");
	return -1;
}

MSList *ms_list_insert_sorted(MSList *list, void *data, int (*compare_func)(const void *, const void*)){
	MSList *it,*previt=NULL;
	MSList *nelem;
	MSList *ret=list;
	if (list==NULL) return ms_list_append(list,data);
	else{
		nelem=ms_list_new(data);
		for(it=list;it!=NULL;it=it->next){
			previt=it;
			if (compare_func(data,it->data)<=0){
				nelem->prev=it->prev;
				nelem->next=it;
				if (it->prev!=NULL)
					it->prev->next=nelem;
				else{
					ret=nelem;
				}
				it->prev=nelem;
				return ret;
			}
		}
		previt->next=nelem;
		nelem->prev=previt;
	}
	return ret;
}

MSList *ms_list_insert(MSList *list, MSList *before, void *data){
	MSList *elem;
	if (list==NULL || before==NULL) return ms_list_append(list,data);
	for(elem=list;elem!=NULL;elem=ms_list_next(elem)){
		if (elem==before){
			if (elem->prev==NULL)
				return ms_list_prepend(list,data);
			else{
				MSList *nelem=ms_list_new(data);
				nelem->prev=elem->prev;
				nelem->next=elem;
				elem->prev->next=nelem;
				elem->prev=nelem;
			}
		}
	}
	return list;
}

MSList *ms_list_copy(const MSList *list){
	MSList *copy=NULL;
	const MSList *iter;
	for(iter=list;iter!=NULL;iter=ms_list_next(iter)){
		copy=ms_list_append(copy,iter->data);
	}
	return copy;
}

void ms_sleep(int seconds){
	struct timespec ts,rem;
	int err;
	ts.tv_sec=seconds;
	ts.tv_nsec=0;
	do {
		err=nanosleep(&ts,&rem);
		ts=rem;
	}while(err==-1 && errno==EINTR);
}

void ms_usleep(uint64_t usec){
	struct timespec ts,rem;
	int err;
	ts.tv_sec=usec/1000000LL;
	ts.tv_nsec=(usec%1000000LL)*1000;
	do {
		err=nanosleep(&ts,&rem);
		ts=rem;
	}while(err==-1 && errno==EINTR);
}


extern void _android_key_cleanup(void*);
void ms_thread_exit(void* ref_val) {
	_android_key_cleanup(NULL);
	ortp_thread_exit(ref_val); 
}
//============================================================

struct _MSConcealerContext {
	int64_t sample_time;
	int64_t plc_start_time;
	unsigned long total_number_for_plc;
	unsigned int max_plc_time;
};

/*** plc context begin***/
unsigned long ms_concealer_context_get_total_number_of_plc(MSConcealerContext* obj) {
	return obj->total_number_for_plc;
}

MSConcealerContext* ms_concealer_context_new(unsigned int max_plc_time){
	MSConcealerContext *obj=(MSConcealerContext *) ms_new(MSConcealerContext,1);
	obj->sample_time=-1;
	obj->plc_start_time=-1;
	obj->total_number_for_plc=0;
	obj->max_plc_time=max_plc_time;
	return obj;
}

void ms_concealer_context_destroy(MSConcealerContext* context) {
	ms_free(context);
}

int ms_concealer_inc_sample_time(MSConcealerContext* obj, uint64_t current_time, int time_increment, int got_packet){
	int plc_duration=0;
	if (obj->sample_time==-1){
		obj->sample_time=(int64_t)current_time;
	}
	obj->sample_time+=time_increment;
	if (obj->plc_start_time!=-1 && got_packet){
		plc_duration=current_time-obj->plc_start_time;
		obj->plc_start_time=-1;
		if (plc_duration>obj->max_plc_time) plc_duration=obj->max_plc_time;
	}
	return plc_duration;
}

unsigned int ms_concealer_context_is_concealement_required(MSConcealerContext* obj,uint64_t current_time) {
	
	if(obj->sample_time == -1) return 0; /*no valid value*/
	
	if (obj->sample_time < current_time){
		int plc_duration;
		if (obj->plc_start_time==-1)
			obj->plc_start_time=obj->sample_time;
		plc_duration=current_time-obj->plc_start_time;
		if (plc_duration<obj->max_plc_time) {
			obj->total_number_for_plc++;
			return 1;
		}else{
			/*reset sample time, so that we don't do PLC anymore and can resync properly when the stream restarts*/
			obj->sample_time=-1;
			return 0;
		}
	}
	return 0;
}


struct _MSConcealerTsContext {
	int64_t sample_ts;
	int64_t plc_start_ts;
	unsigned long total_number_for_plc;
	unsigned int max_plc_ts;
};

/*** plc context begin***/
unsigned long ms_concealer_ts_context_get_total_number_of_plc(MSConcealerTsContext* obj) {
	return obj->total_number_for_plc;
}

MSConcealerTsContext* ms_concealer_ts_context_new(unsigned int max_plc_ts){
	MSConcealerTsContext *obj=(MSConcealerTsContext *) ms_new(MSConcealerTsContext,1);
	obj->sample_ts=-1;
	obj->plc_start_ts=-1;
	obj->total_number_for_plc=0;
	obj->max_plc_ts=max_plc_ts;
	return obj;
}

void ms_concealer_ts_context_destroy(MSConcealerTsContext* context) {
	ms_free(context);
}

int ms_concealer_ts_context_inc_sample_ts(MSConcealerTsContext* obj, uint64_t current_ts, int ts_increment, int got_packet){
	int plc_duration=0;
	if (obj->sample_ts==-1){
		obj->sample_ts=(int64_t)current_ts;
	}
	obj->sample_ts+=ts_increment;
	if (obj->plc_start_ts!=-1 && got_packet){
		plc_duration=current_ts-obj->plc_start_ts;
		obj->plc_start_ts=-1;
		if (plc_duration>obj->max_plc_ts) plc_duration=obj->max_plc_ts;
	}
	return plc_duration;
}

unsigned int ms_concealer_ts_context_is_concealement_required(MSConcealerTsContext* obj, uint64_t current_ts) {
	if(obj->sample_ts == -1) return 0; /*no valid value*/
	
	if (obj->sample_ts < current_ts){
		int plc_duration;
		if (obj->plc_start_ts==-1)
			obj->plc_start_ts=obj->sample_ts;
		plc_duration=current_ts-obj->plc_start_ts;
		if (plc_duration<obj->plc_start_ts) {
			obj->total_number_for_plc++;
			return 1;
		}else{
			/*reset sample time, so that we don't do PLC anymore and can resync properly when the stream restarts*/
			obj->sample_ts=-1;
			return 0;
		}
	}
	return 0;
}

/*** plc context end***/
