//
//  MSVideoDraw.h
//
//
//  Created by ftanx  on 16/20/11.
//  Copyright (c) 2011 ZY.SYM. All rights reserved.
//  使用OpenGL 在UIView上 绘制 YUV420 数据
//  1.增加对父窗口改变大小的处理
//  2.增加对窗口被隐藏后的处理


/*
 使用例子，view是父窗口
 
 Draw = [[MSVideoDraw alloc] initWithFrame:CGRectMake(0, 0, view.frame.size.width, view.frame.size.height)];
 [view addSubview:Draw];
 
 Draw.m_iVideoWidth = width;
 Draw.m_iVideoHeight = height;
 Draw.m_bShowFull = NO;

 [Draw Open];
 
 // APP后台模式、View不在窗口上不绘制视频
 // 父窗口改变大小时也跟着改变
 [Draw DrawVideoData:(void*)buf];
 
 */
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

@interface MSVideoDraw : UIButton

@property (assign) int m_iVideoWidth;  //视频大小
@property (assign) int m_iVideoHeight; //视频大小
@property (assign) BOOL m_bShowFull;   //是否填充整个UIView


- (MSVideoDraw*)initWithParent:(UIView*)view doAction:(SEL)action ; //根据父窗口创建
- (BOOL)Open;  //启用
- (void)Close; //禁用
- (void)DrawVideoData:(uint8_t *)data; //绘制视频
- (void)ClearVideo;  //清屏
- (void)DrawAudioLevel:(int)level; // 绘制音波条

@end
