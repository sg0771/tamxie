//
//  RtmpSender.h
//  media
//
//  Created by momo on 2021/7/3.
//  Copyright © 2021 TenXie. All rights reserved.
//

#ifndef _RTMP_SENDER_H_
#define _RTMP_SENDER_H_

void* RtmpSenderCreate(const char* szURL, int width, int height, int SampleRate, int channel);
void  RtmpSenderSendI420(void* ptr,  uint8_t* buf);
void  RtmpSenderSendRGB32(void* ptr, uint8_t* buf);
void  RtmpSenderSendPcm(void* ptr,   uint8_t* buf, int buf_size);
void  RtmpSenderDestroy(void* ptr);

#endif
