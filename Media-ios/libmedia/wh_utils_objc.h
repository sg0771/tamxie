//
//  wh_utils.h
//  Media
//
//  Created by ftanx on 2017/6/16.
//  Copyright © 2017年 com.wxx.2007. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import "wh_utils_c.h"

@protocol DataDelegate <NSObject>
@optional
    -(void)onVideoData:(uint8_t*)buf withWidth:(int)width withHeight:(int)height;
    -(void)onAudioData:(uint8_t*)buf Size:(int)size;
@end


const char * PathDoc(const char *sz);

const char * PathRes(const char *sz);

const char *string_NS2C(NSString * str);

NSString* string_C2NS(const char *sz);

UIImage* ImageScaleFromImage(UIImage *image, int width, int height);

UIImage* ImageScaleFromName(NSString *imageName, int width, int height);

void setSpeakerEnabled(BOOL enable);
