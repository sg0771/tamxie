//
//  MSOpusDecoder.m
//  media
//
//  Created by ftanx on 2017/6/16.
//  Copyright © 2017年 TenXie. All rights reserved.
//

#import "MSOpusDecoder.h"
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

@interface MSOpusDecoder(){
	OpusDecoder *decoder;
}
@end

@implementation MSOpusDecoder


-(MSOpusDecoder*)init{
    self = [super init];
    if(self){
        int error = 0;
        decoder = opus_decoder_create(OPUS_SAMPLERATE, OPUS_CHANNEL, &error);
        int value = 0;
        opus_decoder_ctl(decoder, OPUS_GET_GAIN(&value));
    }
    return self;
}
-(void)dealloc{
	opus_decoder_destroy(decoder);
}
-(int)Decode:(uint8_t*)inBuf decodeData:(uint8_t*)outBuf{
    @synchronized (self) {
        int pos[5] = {0};
        pos[0] = 6;
        pos[1] = pos[0] + inBuf[1];
        pos[2] = pos[1] + inBuf[2];
        pos[3] = pos[2] + inBuf[3];
        pos[4] = pos[3] + inBuf[4];
        int16_t *pcm = (int16_t*)outBuf;
        for (int i = 0; i < FRAME_PRE_PACKET; i++) {
            uint8_t *enc_data = inBuf + pos[i];//包地址
            int  enc_size = inBuf[i + 1];//包长度
            opus_decode(decoder, enc_data, enc_size, pcm + i * FRAME_SIZE, FRAME_SIZE, 0);
        }
        return FRAME_DATASIZE;
    }
}
@end
