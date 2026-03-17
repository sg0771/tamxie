//
//  MSH264Decoder.h
//  media
//
//  Created by ftanx on 2017/6/5.
//  Copyright © 2017年 TenXie. All rights reserved.
//  FFMPEG  H264 解码器


#import "MSH264Decoder.h"
#import "libavcodec/avcodec.h"

@interface MSH264Decoder(){
    struct AVCodec *m_pCodec;// = NULL;
    struct AVCodecContext *m_pCtx;// = NULL;
    struct AVFrame  *m_pFrame;// = NULL;
}
@end

@implementation  MSH264Decoder : NSObject;
@synthesize  m_bOpen;
@synthesize  m_iWidth;
@synthesize  m_iHeight;
@synthesize m_decData;

#pragma mark - 使用  [[MSH264Decoder alloc ] init]
- (MSH264Decoder*)init{
    self = [super init];
    if(self != nil){
        m_iWidth = 0;
        m_iHeight = 0;
        m_bOpen = NO;
        m_pCtx = NULL;
        m_pCodec = NULL;
        m_pFrame = NULL;
        m_decData = NULL;
        avcodec_register_all();
    }
    return self;
}

#pragma mark - 打开解码器
-(BOOL)  Open{
    if(m_bOpen)[self Close];
    m_pCodec = avcodec_find_decoder(AV_CODEC_ID_H264);
    m_pFrame = av_frame_alloc();
    m_pCtx = avcodec_alloc_context3(m_pCodec);
    if(avcodec_open2(m_pCtx, m_pCodec, NULL) < 0){
        av_free(m_pCtx);
        av_frame_free(&m_pFrame);
        m_pCodec = NULL;
        NSLog(@"创建解码器失败");
        return NO;
    }
    m_bOpen = YES;
    return m_bOpen;
}

#pragma mark - 关闭解码
-(void)  Close{
    if(!m_bOpen)return;
    avcodec_close(m_pCtx);
    av_free(m_pCtx);
    av_frame_free(&m_pFrame);
    m_bOpen = 0;
    m_pCtx = NULL;
    m_pCodec = NULL;
    m_pFrame = NULL;
    m_bOpen = NO;
    m_iWidth = 0;
    m_iHeight = 0;
}

#pragma mark - 解码后获取结果
-(BOOL) DecodeFrame:(uint8_t*)buf DataSize:(int)size{
    if(!m_bOpen || buf == NULL || size <= 0)return NO;
    AVPacket avpkt;
    av_init_packet(&avpkt);
    avpkt.data = buf;
    avpkt.size = size;
    avpkt.flags = 0x0001;
    int got_picture = 0;
    avcodec_decode_video2(m_pCtx, m_pFrame, &got_picture, &avpkt);
    if(got_picture <= 0){
        avcodec_decode_video2(m_pCtx, m_pFrame, &got_picture, &avpkt);//sometime need decode two
    }
    if(got_picture){ //Decode ok !
        if(m_iWidth == 0 || m_iHeight == 0 || m_decData == NULL){
            m_iWidth  = m_pCtx->width;
            m_iHeight = m_pCtx->height;
            m_decData = malloc(m_iWidth * m_iHeight * 3 / 2);
        }
        uint8_t *pYDst = m_decData;
        uint8_t *pUDst = pYDst + m_iWidth * m_iHeight;
        uint8_t *pVDst = pYDst + m_iWidth * m_iHeight * 5 / 4;
        uint8_t *pYSrc = m_pFrame->data[0];
        int      pitchY = m_pFrame->linesize[0];
        uint8_t *pUSrc = m_pFrame->data[1];
        int      pitchU = m_pFrame->linesize[1];
        uint8_t *pVSrc = m_pFrame->data[2];
        int      pitchV = m_pFrame->linesize[2];
        for (int i = 0; i < m_iHeight; i++) {
            memcpy(pYDst, pYSrc, m_iWidth);
            pYDst += m_iWidth;
            pYSrc += pitchY;
        }
        for (int j = 0; j < m_iHeight/2; j++) {
            memcpy(pUDst, pUSrc, m_iWidth/2);
            pUDst += m_iWidth / 2;
            pUSrc += pitchU;
            memcpy(pVDst, pVSrc, m_iWidth/2);
            pVDst += m_iWidth / 2;
            pVSrc += pitchV;
        }
        return YES;
    }
    return NO;
}

@end
