#ifndef _XKT_MS2_H_
#define _XKT_MS2_H_

#include <stdint.h>
typedef struct MSAudioStream MSAudioStream;
MSAudioStream* MSAudioStream_Create(int bPcm);//媒体流创建
void MSAudioStream_Destroy(MSAudioStream *stream);//销毁
int  MsAudioStream_Sender_GetData(MSAudioStream *stream, uint8_t *buf);//外部线程不断获取数据
int  MSAudioStream_Receive_Add(MSAudioStream *stream);//增加输入Node
void MSAudioStream_Receive_Remove    (MSAudioStream *stream, int channel);//删除输入Node
void MSAudioStream_Receive_WriteData(MSAudioStream *stream, int channel, uint8_t *buf, int size);//往Node写入数据
int    MSAudioStream_Receive_GetMax     (MSAudioStream *stream, int channel);//获取最大值
int    MSAudioStream_Receive_GetLocalMax     (MSAudioStream *stream);//获取最大值

#endif




