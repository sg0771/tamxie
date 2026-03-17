
#include "ortp/logging.h"
#include "ortp/port.h"
#include "ortp/str_utils.h"

char * ortp_strdup_vprintf(const char *fmt, va_list ap)
{
	/* Guess we need no more than 100 bytes. */
	int n, size = 200;
	char *p,*np;
#ifndef WIN32
	va_list cap;/*copy of our argument list: a va_list cannot be re-used (SIGSEGV on linux 64 bits)*/
#endif
	if ((p = (char *) ortp_malloc (size)) == NULL)
		return NULL;
	while (1)
	{
		/* Try to print in the allocated space. */
#ifndef WIN32
		va_copy(cap,ap);
		n = vsnprintf (p, size, fmt, cap);
		va_end(cap);
#else
		/*this works on 32 bits, luckily*/
		n = vsnprintf (p, size, fmt, ap);
#endif
		/* If that worked, return the string. */
		if (n > -1 && n < size)
			return p;
		//printf("Reallocing space.\n");
		/* Else try again with more space. */
		if (n > -1)	/* glibc 2.1 */
			size = n + 1;	/* precisely what is needed */
		else		/* glibc 2.0 */
			size *= 2;	/* twice the old size */
		if ((np = (char *) ortp_realloc (p, size)) == NULL)
		  {
		    free(p);
		    return NULL;
		  }
		else
		  {
		    p = np;
		  }
	}
}

char *ortp_strdup_printf(const char *fmt,...){
	char *ret;
	va_list args;
	va_start (args, fmt);
	ret=ortp_strdup_vprintf(fmt, args);
	va_end (args);
	return ret;
}



void* ortp_malloc(size_t sz){
	return malloc(sz);
}

void* ortp_realloc(void *ptr, size_t sz){
	return realloc(ptr,sz);
}

void ortp_free(void* ptr){
	free(ptr);
}

void * ortp_malloc0(size_t size){
	void *ptr=ortp_malloc(size);
	memset(ptr,0,size);
	return ptr;
}

char * ortp_strdup(const char *tmp){
	size_t sz;
	char *ret;
	if (tmp==NULL)
	  return NULL;
	sz=strlen(tmp)+1;
	ret=(char*)ortp_malloc(sz);
	strcpy(ret,tmp);
	ret[sz-1]='\0';
	return ret;
}

char *ortp_strndup(const char *str,int n){
	int min=MIN((int)strlen(str),n)+1;
	char *ret=(char*)ortp_malloc(min);
	strncpy(ret,str,min);
	ret[min-1]='\0';
	return ret;
}

int __ortp_thread_join(ortp_thread_t thread, void **ptr){
	int err=pthread_join(thread,ptr);
	if (err!=0) {
		ortp_error("pthread_join error: %s",strerror(err));
	}
	return err;
}

int __ortp_thread_create(pthread_t *thread, pthread_attr_t *attr, void * (*routine)(void*), void *arg){
	pthread_attr_t my_attr;
	pthread_attr_init(&my_attr);
	if (attr)my_attr = *attr;
	return pthread_create(thread, &my_attr, routine, arg);
}

void ortp_get_cur_time(ortpTimeSpec *ret){
	struct timeval tv;
	gettimeofday(&tv, NULL);
	ret->tv_sec=tv.tv_sec;
	ret->tv_nsec=tv.tv_usec*1000LL;	
}

void qinit(queue_t *q){
	mblk_init(&q->_q_stopper);
	q->_q_stopper.b_next=&q->_q_stopper;
	q->_q_stopper.b_prev=&q->_q_stopper;
	q->q_mcount=0;
}

void mblk_init(mblk_t *mp)
{
	mp->b_cont=mp->b_prev=mp->b_next=NULL;
	mp->b_rptr=mp->b_wptr=NULL;
	mp->reserved1=0;
	mp->reserved2=0;
#if defined(ORTP_TIMESTAMP)
	memset(&(mp->timestamp), 0, sizeof(struct timeval));
#endif
}

void mblk_meta_copy(const mblk_t *source, mblk_t *dest) {
	dest->reserved1 = source->reserved1;
	dest->reserved2 = source->reserved2;
#if defined(ORTP_TIMESTAMP)
	dest->timestamp = source->timestamp;
#endif
	#if V2_PHONE_SUPPORT
	dest->ssrc=source->ssrc;
	#endif
}

dblk_t *datab_alloc(int size){
	dblk_t *db;
	int total_size=sizeof(dblk_t)+size;
	db=(dblk_t *) ortp_malloc(total_size);
	db->db_base=(uint8_t*)db+sizeof(dblk_t);
	db->db_lim=db->db_base+size;
	db->db_ref=1;
	db->db_freefn=NULL;	/* the buffer pointed by db_base must never be freed !*/
	return db;
}

static inline void datab_ref(dblk_t *d){
	d->db_ref++;
}

static inline void datab_unref(dblk_t *d){
	d->db_ref--;
	if (d->db_ref==0){
		if (d->db_freefn!=NULL)
			d->db_freefn(d->db_base);
		ortp_free(d);
	}
}


mblk_t *allocb(int size, int pri)
{
	mblk_t *mp;
	dblk_t *datab;
	
	mp=(mblk_t *) ortp_malloc(sizeof(mblk_t));
	mblk_init(mp);
	datab=datab_alloc(size);
	
	mp->b_datap=datab;
	mp->b_rptr=mp->b_wptr=datab->db_base;
	mp->b_next=mp->b_prev=mp->b_cont=NULL;
	return mp;
}

mblk_t *esballoc(uint8_t *buf, int size, int pri, void (*freefn)(void*) )
{
	mblk_t *mp;
	dblk_t *datab;
	
	mp=(mblk_t *) ortp_malloc(sizeof(mblk_t));
	mblk_init(mp);
	datab=(dblk_t *) ortp_malloc(sizeof(dblk_t));
	

	datab->db_base=buf;
	datab->db_lim=buf+size;
	datab->db_ref=1;
	datab->db_freefn=freefn;
	
	mp->b_datap=datab;
	mp->b_rptr=mp->b_wptr=buf;
	mp->b_next=mp->b_prev=mp->b_cont=NULL;
	return mp;
}

	
void freeb(mblk_t *mp)
{
	return_if_fail(mp->b_datap!=NULL);
	return_if_fail(mp->b_datap->db_base!=NULL);
	
	datab_unref(mp->b_datap);
	ortp_free(mp);
}

void freemsg(mblk_t *mp)
{
	mblk_t *tmp1,*tmp2;
	tmp1=mp;
	while(tmp1!=NULL)
	{
		tmp2=tmp1->b_cont;
		freeb(tmp1);
		tmp1=tmp2;
	}
}

mblk_t *dupb(mblk_t *mp)
{
	mblk_t *newm;
	return_val_if_fail(mp->b_datap!=NULL,NULL);
	return_val_if_fail(mp->b_datap->db_base!=NULL,NULL);
	
	datab_ref(mp->b_datap);
	newm=(mblk_t *) ortp_malloc(sizeof(mblk_t));
	mblk_init(newm);
	mblk_meta_copy(mp, newm);
	newm->b_datap=mp->b_datap;
	newm->b_rptr=mp->b_rptr;
	newm->b_wptr=mp->b_wptr;
	return newm;
}

/* duplicates a complex mblk_t */
mblk_t	*dupmsg(mblk_t* m)
{
	mblk_t *newm=NULL,*mp,*prev;
	prev=newm=dupb(m);
	m=m->b_cont;
	while (m!=NULL){
		mp=dupb(m);
		prev->b_cont=mp;
		prev=mp;
		m=m->b_cont;
	}
	return newm;
}

void putq(queue_t *q,mblk_t *mp)
{
	q->_q_stopper.b_prev->b_next=mp;
	mp->b_prev=q->_q_stopper.b_prev;
	mp->b_next=&q->_q_stopper;
	q->_q_stopper.b_prev=mp;
	q->q_mcount++;
}

mblk_t *getq(queue_t *q)
{
	mblk_t *tmp;
	tmp=q->_q_stopper.b_next;
	if (tmp==&q->_q_stopper) return NULL;
	q->_q_stopper.b_next=tmp->b_next;
	tmp->b_next->b_prev=&q->_q_stopper;
	tmp->b_prev=NULL;
	tmp->b_next=NULL;
	q->q_mcount--;
	return tmp;
}

mblk_t * peekq(queue_t *q){
	mblk_t *tmp;
	tmp=q->_q_stopper.b_next;
	if (tmp==&q->_q_stopper) return NULL;
	return tmp;
}

/* insert mp in q just before emp */
void insq(queue_t *q,mblk_t *emp, mblk_t *mp)
{
	if (emp==NULL){
		putq(q,mp);
		return;
	}
	q->q_mcount++;
	emp->b_prev->b_next=mp;
	mp->b_prev=emp->b_prev;
	emp->b_prev=mp;
	mp->b_next=emp;	
}

void remq(queue_t *q, mblk_t *mp){
	q->q_mcount--;
	mp->b_prev->b_next=mp->b_next;
	mp->b_next->b_prev=mp->b_prev;
	mp->b_next=NULL;
	mp->b_prev=NULL;
}

/* remove and free all messages in the q */
void flushq(queue_t *q, int how)
{
	mblk_t *mp;
	
	while ((mp=getq(q))!=NULL)
	{
		freemsg(mp);
	}
}

int msgdsize(const mblk_t *mp)
{
	int msgsize=0;
	while(mp!=NULL){
		msgsize+=(int) (mp->b_wptr-mp->b_rptr);
		mp=mp->b_cont;
	}
	return msgsize;
}

void msgpullup(mblk_t *mp,int len)
{
	mblk_t *firstm=mp;
	dblk_t *db;
	int wlen=0;

	if (mp->b_cont==NULL && len==-1) return;	/*nothing to do, message is not fragmented */

	if (len==-1) len=msgdsize(mp);
	db=datab_alloc(len);
	while(wlen<len && mp!=NULL){
		int remain=len-wlen;
		int mlen=mp->b_wptr-mp->b_rptr;
		if (mlen<=remain){
			memcpy(&db->db_base[wlen],mp->b_rptr,mlen);
			wlen+=mlen;
			mp=mp->b_cont;
		}else{
			memcpy(&db->db_base[wlen],mp->b_rptr,remain);
			wlen+=remain;
		}
	}
	/*set firstm to point to the new datab */
	freemsg(firstm->b_cont);
	firstm->b_cont=NULL;
	datab_unref(firstm->b_datap);
	firstm->b_datap=db;
	firstm->b_rptr=db->db_base;
	firstm->b_wptr=firstm->b_rptr+wlen;
}


mblk_t *copyb(mblk_t *mp)
{
	mblk_t *newm;
	int len=(int) (mp->b_wptr-mp->b_rptr);
	newm=allocb(len,BPRI_MED);
	memcpy(newm->b_wptr,mp->b_rptr,len);
	newm->b_wptr+=len;
	memcpy(&newm->recv_addr,&mp->recv_addr,sizeof(newm->recv_addr));
	return newm;
}

mblk_t *copymsg(mblk_t *mp)
{
	mblk_t *newm=0,*m;
	m=newm=copyb(mp);
	mp=mp->b_cont;
	while(mp!=NULL){
		m->b_cont=copyb(mp);
		m=m->b_cont;
		mp=mp->b_cont;
	}
	return newm;
}

mblk_t * appendb(mblk_t *mp, const char *data, int size, bool_t pad){
	int padcnt=0;
	int i;
	if (pad){
		padcnt= (int)(4L-( (long)(((long)mp->b_wptr)+size) % 4L)) % 4L;
	}
	if ((mp->b_wptr + size +padcnt) > mp->b_datap->db_lim){
		/* buffer is not large enough: append a new block (with the same size ?)*/
		int plen=(int)((char*)mp->b_datap->db_lim - (char*) mp->b_datap->db_base);
		mp->b_cont=allocb(MAX(plen,size),0);
		mp=mp->b_cont;
	}
	if (size) memcpy(mp->b_wptr,data,size);
	mp->b_wptr+=size;
	for (i=0;i<padcnt;i++){
		mp->b_wptr[0]=0;
		mp->b_wptr++;
	}
	return mp;
}

void msgappend(mblk_t *mp, const char *data, int size, bool_t pad){
	while(mp->b_cont!=NULL) mp=mp->b_cont;
	appendb(mp,data,size,pad);
}

mblk_t *concatb(mblk_t *mp, mblk_t *newm){
	while (mp->b_cont!=NULL) mp=mp->b_cont;
	mp->b_cont=newm;
	while(newm->b_cont!=NULL) newm=newm->b_cont;
	return newm;
}
