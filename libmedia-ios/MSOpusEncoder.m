//
//  MSOpusEncoder.m
//  media
//
//  Created by ftanx on 2017/6/16.
//  Copyright © 2017年 TenXie. All rights reserved.
//

#import "MSOpusEncoder.h"
#include <opus.h>
#include "wh_utils_c.h"
#include <string.h>
#include <stdlib.h>

#define OPUS_SAMPLERATE 16000
#define OPUS_CHANNEL 1
#define OPUS_MODE  OPUS_APPLICATION_VOIP
#define OPUS_BITRATE  16000

#define FRAME_DATASIZE 1600
#define FRAME_PRE_PACKET 5  //A Packet has 5 Frame
#define FRAME_SIZE 160  //10ms

@interface MSOpusEncoder(){
    int m_bufsize;
    OpusEncoder *encoder;
    uint8_t *m_buf;
}
@end


@implementation MSOpusEncoder

-(MSOpusEncoder*)init{
    self = [super init];
    if(self){
        int error = 0;
        encoder = opus_encoder_create(OPUS_SAMPLERATE, OPUS_CHANNEL,  OPUS_MODE, &error);
        error = opus_encoder_ctl(encoder, OPUS_SET_BITRATE(OPUS_BITRATE));
        error = opus_encoder_ctl(encoder, OPUS_SET_EXPERT_FRAME_DURATION(OPUS_FRAMESIZE_10_MS));//10ms 一帧
        m_bufsize = 0;
        m_buf = wh_malloc0(FRAME_SIZE);
    }
    return self;
}

-(void)dealloc{
    wh_free(m_buf);
    opus_encoder_destroy(encoder);
}

-(int)Encode:(uint8_t*)inBuf encodeData:(uint8_t*)outBuf{
    if(inBuf == NULL || outBuf == NULL)return 0;
    @synchronized (self) {
        memset(m_buf,0,FRAME_DATASIZE);
        m_buf[0]  = FRAME_PRE_PACKET; //第一个字节表示 5个包
        m_bufsize = 1 + FRAME_PRE_PACKET; // 2-6字节表示数据5个pack的长度
        int16_t *pcm = (int16_t*)inBuf;
        for (int i = 0; i < FRAME_PRE_PACKET; i++) {
            opus_int32 ret = opus_encode(encoder, pcm + FRAME_SIZE * i, FRAME_SIZE, m_buf + m_bufsize, 500);
            m_buf[i + 1] = ret;//每个包的长度
            m_bufsize += ret;
            //NSLog(@"[%d] = %d total = %d",i,ret,m_bufsize);
        }
        memcpy(outBuf, m_buf, m_bufsize);
        return m_bufsize;
    }
}

@end
