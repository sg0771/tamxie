#ifndef _MS_OPUS_IMPL_H_
#define _MS_OPUS_IMPL_H_

#include <stdint.h>
#include <opus.h>
#include "mediastreamer2/msfilter.h"

//输入 16KHz 16Bit 50ms  
//800 short
//1600 byte
//5 * 10ms
typedef struct MSOpusEncoder  MSOpusEncoder;
MSOpusEncoder* MSOpusEncoder_Create();
void MSOpusEncoder_Destroy(MSOpusEncoder *enc);
mblk_t *MSOpusEncoder_Encode(MSOpusEncoder *enc, int16_t *pcm);
int  MSOpusEncoder_Encode2(MSOpusEncoder *enc, int16_t *pcm, uint8_t *encbuf);

typedef struct MSOpusDecoder  MSOpusDecoder;
MSOpusDecoder* MSOpusDecoder_Create();
void MSOpusDecoder_Destroy(MSOpusDecoder *dec);
mblk_t *MSOpusDecoder_Decode(MSOpusDecoder *dec, uint8_t *encbuf);
int  MSOpusDecoder_Decode2(MSOpusDecoder *dec, uint8_t *encbuf, int16_t *decpcm);

#endif