//
//  wh_utils.m
//  Media
//
//  Created by ftanx on 2017/6/16.
//  Copyright © 2017年 com.wxx.2007. All rights reserved.
//  OBJC  的utils

#import "wh_utils_objc.h"
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#import  <AVFoundation/AVFoundation.h>

const char *string_NS2C(NSString * str){
    return [str UTF8String];
}

NSString* string_C2NS(const char *sz){
    NSString *str=[NSString stringWithCString:sz  encoding:NSUTF8StringEncoding];
    return str;
}

const char * PathDoc(const char *sz){
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *docDir = [paths objectAtIndex:0];
    NSString *str = [NSString stringWithFormat:@"%@/%s",docDir,sz];
    return [str UTF8String];
}

const char * PathRes(const char *sz){
    NSString *str = string_C2NS(sz);
    NSString *filePath = [[[NSBundle mainBundle] bundlePath] stringByAppendingPathComponent:str];
    return [filePath UTF8String];
}

UIImage* ImageScaleFromImage(UIImage *image, int width, int height){
    // 创建一个bitmap的context
    // 并把它设置成为当前正在使用的context
    CGSize s;
    s.width = width;
    s.height = height;
    UIGraphicsBeginImageContext(s);
    
    // 绘制改变大小的图片
    [image drawInRect:CGRectMake(0, 0, width, height)];
    
    // 从当前context中创建一个改变大小后的图片
    UIImage* scaledImage = UIGraphicsGetImageFromCurrentImageContext();
    
    // 使当前的context出堆栈
    UIGraphicsEndImageContext();
    // 返回新的改变大小后的图片
    return scaledImage;
}
UIImage* ImageScaleFromName(NSString *imageName, int width, int height){
    UIImage *image = [UIImage imageNamed:imageName];
    return ImageScaleFromImage(image,width,height);
}


//声音外放模式
void setSpeakerEnabled(BOOL enable) {
    if(enable) {
        UInt32 audioRouteOverride = kAudioSessionOverrideAudioRoute_Speaker;
        AudioSessionSetProperty (kAudioSessionProperty_OverrideAudioRoute , sizeof (audioRouteOverride), &audioRouteOverride);
    } else {
        UInt32 audioRouteOverride = kAudioSessionOverrideAudioRoute_None;
        AudioSessionSetProperty (kAudioSessionProperty_OverrideAudioRoute, sizeof (audioRouteOverride), &audioRouteOverride);
    }
}
