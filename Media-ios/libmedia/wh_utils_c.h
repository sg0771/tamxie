//
//  utils.h
//  Media
//
//  Created by ftanx on 2017/6/14.
//  Copyright © 2017年 com.wxx.2007. All rights reserved.
//  基础函数

#ifndef _WH_UTILS_C_H_
#define _WH_UTILS_C_H_

#include <pthread.h>

#ifndef WIN32
#include <sys/time.h>
#include <malloc/malloc.h> /* for alloca */
#else
#pragma warning(disable: 4068)  
#include <Windows.h>
#include <stdint.h>
#include <malloc.h> /* for alloca */
#endif

typedef unsigned char bool_t;

#ifndef MIN
#define MIN(a,b) ((a) < (b)) ? (a) : (b);
#endif

#ifndef MAX
#define MAX(a,b) ((a) > (b)) ? (a) : (b);
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifdef __cplusplus
extern "C" {
#endif

#pragma mark - 时间函数
void sleepMs(int ms);
uint64_t GetTimeStamp();

#pragma mark - 线程和锁
typedef pthread_t           wh_thread_t;
typedef pthread_mutex_t     wh_mutex_t;
#define wh_mutex_init		pthread_mutex_init
#define wh_mutex_lock		pthread_mutex_lock
#define wh_mutex_unlock	    pthread_mutex_unlock
#define wh_mutex_destroy	pthread_mutex_destroy

#pragma mark - 内存函数
#define wh_new(type,count)	(type*)wh_malloc(sizeof(type)*(count))
#define wh_new0(type,count)	(type*)wh_malloc0(sizeof(type)*(count))

void*   wh_malloc(size_t size);
void    wh_free(void*p);
void *  wh_malloc0(size_t size);

#pragma mark - 媒体类型
typedef enum _MediaType {
		Media_PCM = 0x00,
		Media_OPUS = 0x01,
		Media_YV12 = 0x10,
		Media_NV12 = 0x12,
		Media_H264 = 0x13
}MediaType;

#pragma mark - 数据包
typedef struct _wh_packet {
	struct _wh_packet *pNext;
	MediaType type;
	uint16_t  iWidth;
	uint16_t  iHeight;
	uint16_t  bkey;
	uint16_t  iChannel;
	uint32_t  iSamplerate;
	uint8_t   *pBuf;
	uint32_t  iBufsize;
	uint32_t  iPos;
} wh_packet;
wh_packet *packet_malloc(int size);
wh_packet *packet_mallocp(uint8_t *buf, int size);
wh_packet *packet_clone(wh_packet *mp);
void packet_free(wh_packet *mp);

#pragma mark - 队列
typedef struct _wh_squeue {
		wh_packet *begin;
		wh_packet *trail;
		int       q_mcount;
		uint8_t   use;
} wh_queue;
void queue_init(wh_queue *q);
void queue_push(wh_queue *q, wh_packet *m);
wh_packet * queue_pop(wh_queue *q);
void queue_flush(wh_queue *q);

#pragma mark - 内存池，适合用于内存大小一样的数据包，避免反复malloc/free操作
typedef struct _wh_mempool {
	wh_queue   qMemory;
	wh_mutex_t mutex;
}wh_mempool;
void mempool_init(wh_mempool*pool); //申请一个队列，长度为count，每个数据包为size
wh_packet *mempool_alloc(wh_mempool*pool, int size); //申请内存池中得内存
wh_packet *mempool_allocp(wh_mempool*pool, uint8_t*buf, int size); //申请内存池中得内存
void mempool_free(wh_mempool*pool, wh_packet *p);    //释放内存
void mempool_destroy(wh_mempool*pool);

#pragma mark - 由队列数据组成的缓存

typedef struct wh_buffer {
	wh_queue   qData;
	wh_mutex_t mutex;
	wh_queue   qMemory;
	int size;
}wh_buffer;
void buffer_init(wh_buffer *obj);
void buffer_put(wh_buffer *obj, uint8_t *buf, int size);
int  buffer_read(wh_buffer *obj, uint8_t *buf, int size);
void buffer_skip_bytes(wh_buffer *obj, int size);
void buffer_flush(wh_buffer *obj);

#ifdef __cplusplus
};
#endif

#endif

