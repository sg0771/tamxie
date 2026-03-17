//
//  AQAudioFilePlay.h
//  Media
//
//  Created by ftanx on 2017/6/14.
//  Copyright © 2017年 com.wxx.2007. All rights reserved.
//  基于AudioQueue的PCM播放测试类

#import <Foundation/Foundation.h>

@interface AQAudioFilePlay : NSObject

@property (readonly,assign) BOOL isOpen;

- (BOOL)Start:(NSString*)filename;
- (void)Stop;

@end

