#include "mediastreamer2/mscommon.h"
#include "mediastreamer2/msfilter.h"

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
