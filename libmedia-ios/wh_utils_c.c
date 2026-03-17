//
//  utils.m
//  Media
//
//  Created by ftanx on 2017/6/14.
//  Copyright © 2017年 com.wxx.2007. All rights reserved.
//
#include "wh_utils_c.h"
#ifndef _WIN32
#include <sys/param.h>
#include <alloca.h>
#else
#define alloca _alloca
#endif
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


#pragma mark - Malloc Function
void* wh_malloc(size_t size){
    return malloc(size);
}

void  wh_free(void*ptr){
    if(ptr)free(ptr);
}

void * wh_malloc0(size_t size){
    void *ptr=wh_malloc(size);
    memset(ptr,0,size);
    return ptr;
}

#pragma mark - Packet Function
wh_packet *packet_malloc(int size){
    wh_packet *m = (wh_packet *)wh_malloc0(sizeof(wh_packet));
    m->pBuf = wh_malloc(size);
    m->iBufsize = size;
    return m;
}

wh_packet *packet_mallocp(uint8_t *buf, int size){
    wh_packet *m = packet_malloc(size);
    memcpy(m->pBuf, buf, size);
    return m;
}

wh_packet *packet_clone(wh_packet *mp){
    wh_packet *m = (wh_packet *)wh_malloc0(sizeof(wh_packet));
    memcpy(m,mp,sizeof(wh_packet));
    m->pNext = NULL;
    m->pBuf = wh_malloc(mp->iBufsize);
    memcpy(m->pBuf,mp->pBuf,mp->iBufsize);
    return m;
}

void packet_free(wh_packet *mp){
    if(mp->pBuf){
        wh_free(mp->pBuf);
        mp->pBuf = NULL;
    }
    wh_free(mp);
}

#pragma mark - Queue Function
void queue_init(wh_queue *q){
    q->begin = NULL;
    q->trail = NULL;
    q->q_mcount = 0;
    q->use = 0;
}

void queue_push(wh_queue *q, wh_packet *mp){
    if(q == NULL)return;
    if(q->begin == NULL){
        q->begin = mp;
    }else{
        q->trail->pNext = mp;
    }
    q->trail = mp;
    q->q_mcount++;
}

//gethead but not pop
wh_packet * peekq(wh_queue *q){
    if (q == NULL) return NULL;
    return q->begin;
}

wh_packet *queue_pop(wh_queue *q){
    if(q == NULL || q->q_mcount == 0)return NULL;
    wh_packet *mp = q->begin;
    mp->iPos = 0;
    q->q_mcount--;
    if(q->q_mcount == 0){
        q->begin = NULL;
        q->trail = NULL;
        mp->pNext = NULL;
    }else{
        q->begin = mp->pNext;
        mp->pNext = NULL;
    }
    return mp;
}

void queue_flush(wh_queue *q){
    q->q_mcount = 0;
    q->use = 0;
    wh_packet *mp;
    while ((mp=queue_pop(q))!=NULL){
        packet_free(mp);
    }
}

#pragma mark - Memory Pool Function
void mempool_init(wh_mempool*pool){
    wh_mutex_init(&pool->mutex,NULL);
    queue_init(&pool->qMemory);
}

wh_packet *mempool_alloc(wh_mempool*pool, int size){
    wh_mutex_lock(&pool->mutex);
    if(pool->qMemory.q_mcount == 0){
        wh_packet *mp = packet_malloc(size);//申请一个新的
        queue_push(&pool->qMemory, mp);
    }
    wh_packet *m = queue_pop(&pool->qMemory);
    wh_mutex_unlock(&pool->mutex);
    return m;
}

wh_packet *mempool_allocp(wh_mempool*pool,uint8_t*buf, int size){
    wh_packet *m = mempool_alloc(pool,size);
    wh_mutex_lock(&pool->mutex);
    memcpy(m->pBuf, buf, size);
    wh_mutex_unlock(&pool->mutex);
    return m;
}

void mempool_free(wh_mempool*pool,wh_packet *mp){
    wh_mutex_lock(&pool->mutex);
    queue_push(&pool->qMemory, mp); //放回pool
    wh_mutex_unlock(&pool->mutex);
}

void mempool_destroy(wh_mempool*pool){
    queue_flush(&pool->qMemory);
    wh_mutex_destroy(&pool->mutex);
}

#pragma mark - Memory Buffer Function
void buffer_init(wh_buffer *obj){
    queue_init(&obj->qData);
    queue_init(&obj->qMemory);
    obj->size=0;
    wh_mutex_init(&obj->mutex,NULL);
}

void buffer_put(wh_buffer *obj, uint8_t *buf, int size){
    wh_mutex_lock(&obj->mutex);
    if(obj->qMemory.q_mcount == 0){
        wh_packet *mp = packet_malloc(size);
        queue_push(&obj->qMemory, mp);
        printf("New Pack to Mem, Data is %d\n",obj->qData.q_mcount);
    }
     wh_packet *mp = queue_pop(&obj->qMemory);
    memcpy(mp->pBuf, buf, size);
    queue_push(&obj->qData, mp);
    obj->size += size;
    wh_mutex_unlock(&obj->mutex);
}

int buffer_read(wh_buffer *obj, uint8_t *data, int datalen){
    wh_mutex_lock(&obj->mutex);
    if (obj->size >= datalen){
        wh_packet *m = obj->qData.begin;
        int sz = 0;
        while(sz < datalen){
            int cplen = MIN(m->iBufsize - m->iPos, datalen - sz);
            memcpy(data + sz, m->pBuf + m->iPos, cplen);
            sz      += cplen; //已经拷贝的数据长度
            m->iPos += cplen; //当前packet已经使用的长度
            if (m->iPos == m->iBufsize){
                wh_packet *remove = queue_pop(&obj->qData);
                queue_push(&obj->qMemory, remove);
                m = obj->qData.begin;
            }
        }
        obj->size -= datalen;
		wh_mutex_unlock(&obj->mutex);
        return datalen;
    }
    wh_mutex_unlock(&obj->mutex);
    return 0;
}

void buffer_skip_bytes(wh_buffer *obj, int bytes){
    uint8_t *tmp=(uint8_t*)alloca(bytes);
    buffer_read(obj,tmp,bytes);
}

void buffer_flush(wh_buffer *obj){
    wh_mutex_lock(&obj->mutex);
    obj->size=0;
    queue_flush(&obj->qData);
    queue_flush(&obj->qMemory);
    wh_mutex_unlock(&obj->mutex);
}

#pragma mark - Time Function
uint64_t GetTimeStamp(){
#ifndef _WIN32
	struct timeval tv;
	gettimeofday(&tv, (struct timezone *) NULL);
	return (uint64_t)(tv.tv_sec * 1000 + tv.tv_usec / 1000);//
#else
	return 0;
#endif // !WIN32
}

void sleepMs(int ms){
#ifndef _WIN32
    struct timespec ts;
    ts.tv_sec=0;
    ts.tv_nsec=ms*1000000LL;
    nanosleep(&ts,NULL);
#else
	Sleep(ms);
#endif
}
