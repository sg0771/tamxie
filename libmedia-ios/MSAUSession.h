//
//  MSAUSession.h
//  Media
//
//  Created by ftanx on 2017/6/14.
//  Copyright © 2017年 com.wxx.2007. All rights reserved.
//  基于AudioUnit的音频流，数据包都是 800个short

#import <Foundation/Foundation.h>
#import "wh_utils_objc.h"

@interface MSAUSession : NSObject

@property (readonly,assign) BOOL isOpen;
@property id<DataDelegate> delegate;

- (BOOL) Open;
- (void) Close;
- (int)  AddChannel;
- (void) RemoveChannel:(int)channel;
- (void) WriteDate:(int)channel Data:(uint8_t*)buf Size:(int)size;
- (int)  getWriteSize:(int)chanel;

@end
