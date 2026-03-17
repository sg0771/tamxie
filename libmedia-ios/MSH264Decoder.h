//
//  MSH264Decoder.h
//  media
//
//  Created by ftanx on 2017/6/5.
//  Copyright © 2017年 TenXie. All rights reserved.
//  FFMPEG  H264 解码器

#import <Foundation/Foundation.h>


@interface MSH264Decoder : NSObject;


//编码器状态
@property (readonly,assign) BOOL m_bOpen;
//解码参数
@property (readonly,assign) int m_iWidth;
@property (readonly,assign) int m_iHeight;
//解码结果，需要外部申请内存来保存
@property (atomic) uint8_t *m_decData;

-(BOOL)     Open;
-(void)     Close;
-(BOOL)     DecodeFrame:(uint8_t*)buf DataSize:(int)size; //编码后获取结果

@end
