//
//  MSH264Encoder.h
//  media
//
//  Created by ftanx on 2017/6/3.
//  Copyright © 2017年 TenXie. All rights reserved.
//

//  X264 视频编码器

#import <Foundation/Foundation.h>


@interface MSH264Encoder : NSObject;

//配置参数，可以执行get set操作
@property (assign) int m_iWidth;
@property (assign) int m_iHeight;


//readonly 只能执行get 操作
//编码器状态
@property (readonly,assign) BOOL m_bOpen;

//编码结果
@property (readonly,assign) BOOL m_bKey;
@property (readonly,assign) int m_iEncLength;
@property (atomic)          uint8_t* m_encData;

//打开
-(BOOL)     Open;
//关闭
-(void)     Close;

//编码后获取结果,forceKeyFrame表示强制输出关键帧
-(BOOL)     EncodeFrame:(uint8_t*)buf forceKeyFrame:(BOOL)b;

@end
