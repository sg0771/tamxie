//
//  MSH264Encoder.h
//  media
//
//  Created by ftanx on 2017/6/3.
//  Copyright © 2017年 TenXie. All rights reserved.
//

//  X264 视频编码器

#import "MSH264Encoder.h"
#import <x264.h>

#import <stdint.h>
#import <stdio.h>
#import <stdlib.h>
#import <string.h>
#import "wh_utils_objc.h"

@interface MSH264Encoder(){
    x264_t *m_h;
    x264_param_t m_param;
    x264_picture_t m_pic;
}
@end

@implementation MSH264Encoder

//配置参数
@synthesize  m_iWidth;
@synthesize  m_iHeight;

//编码器状态
@synthesize  m_bOpen;

//编码结果
@synthesize  m_bKey;
@synthesize  m_iEncLength;
@synthesize  m_encData;


#pragma mark - 使用  [[MSH264Encoder alloc ] init]
-(MSH264Encoder*)init{
    self = [super init];
    if(self){
        m_iWidth = 0;
        m_iHeight = 0;
        m_bOpen = NO;
        m_bKey = 0;
        m_iEncLength = 0;
        m_h = NULL;
        m_encData = NULL;
    }
    return self;
}

-(BOOL)Open{
@synchronized (self) {
    if(m_iWidth == 0 || m_iHeight == 0)return NO;
    x264_param_default( &m_param );
    m_param.i_threads = 1;
    m_param.i_frame_total = 0;
    m_param.i_keyint_max = 200;
    m_param.i_width = m_iWidth;
    m_param.i_height = m_iHeight;
    m_param.i_fps_num = 30;
    m_param.i_fps_den = 1;
    m_param.rc.i_rc_method = X264_RC_CQP;
    m_param.rc.i_qp_constant = 34;//图像质量
    m_param.analyse.i_me_method = X264_ME_DIA;//me dia
    m_param.analyse.i_subpel_refine = 2;//subme 2
    m_param.rc.i_qp_min = 10;
    m_param.rc.i_qp_max = 51;
    
    //zero delay
    m_param.rc.i_lookahead = 0;
    m_param.i_sync_lookahead = 0;
    m_param.i_bframe = 0;
    m_param.b_sliced_threads = 1;
    m_param.b_vfr_input = 0;
    m_param.rc.b_mb_tree = 0;
    
    //BASELINE
    m_param.i_frame_reference = 1;//
    m_param.analyse.b_transform_8x8 = 0;
    m_param.b_cabac = 1;//0;
    m_param.i_cqm_preset = X264_CQM_FLAT;
    m_param.psz_cqm_file = NULL;
    m_param.i_bframe = 0;
    m_param.analyse.i_weighted_pred = X264_WEIGHTP_NONE;
    m_param.b_interlaced = 0;
    m_param.b_fake_interlaced = 0;
    
    m_param.b_cabac = 1;
    m_param.analyse.b_mixed_references = 0;
    m_param.analyse.i_trellis = 0;
    
    m_param.b_repeat_headers = 1;
    m_param.b_annexb = 1;//
    
    m_encData = (uint8_t*)wh_malloc(m_iWidth * m_iHeight);
    //open
    m_h = x264_encoder_open( &m_param );
    if(m_h == NULL)return 0;
    x264_picture_alloc(&m_pic, X264_CSP_I420, m_iWidth, m_iHeight);
    m_bOpen = YES;
    return m_bOpen;
}
}
-(void) Close{
     @synchronized (self) {
         if(m_bOpen){
             x264_picture_clean(&m_pic);
             x264_encoder_close(m_h);
             m_h = NULL;
             m_iWidth = 0;
             m_iHeight = 0;
             m_bOpen = NO;
             free(m_encData);
             m_encData = NULL;
        }
    }
}

//编码后获取结果
-(BOOL) EncodeFrame:(uint8_t*)buf forceKeyFrame:(BOOL)b {
    @synchronized (self) {
        if(!m_bOpen)return NO;
        int ImgSize = m_iWidth * m_iHeight;
        uint8_t* pY = buf;
        uint8_t* pU = buf + ImgSize;
        uint8_t* pV = pU   + ImgSize / 4;
        memcpy(m_pic.img.plane[0], pY, ImgSize);
        memcpy(m_pic.img.plane[1], pU, ImgSize / 4);
        memcpy(m_pic.img.plane[2], pV, ImgSize / 4);
        x264_nal_t *nal = NULL;
        int i_nal = 0;

        if(b)m_pic.i_type = X264_TYPE_IDR;//Key Frame
        
        x264_picture_t pic_out;
        int length = x264_encoder_encode(m_h, &nal, &i_nal, &m_pic, &pic_out);
        m_pic.i_type = X264_TYPE_AUTO;

        if(length < 0)return NO;
        m_bKey = (pic_out.i_type == X264_TYPE_IDR);
        m_iEncLength = length;
        memcpy(m_encData, nal[0].p_payload, length);
        return YES;
    }
}

@end


