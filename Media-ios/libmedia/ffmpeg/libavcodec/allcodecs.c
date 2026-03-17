/*
 注册各种编解码器
 */
#include "avcodec.h"
void avcodec_register_all(void){
    static int initialized;
    if (initialized)
        return;
    initialized = 1;
    extern AVCodec ff_h264_decoder;
    avcodec_register(&ff_h264_decoder);
}
