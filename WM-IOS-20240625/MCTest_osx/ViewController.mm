//
//  ViewController.m
//  MacPlayer
//
//  Created by tam on 2019/12/11.
//  Copyright © 2019年 xkt.spacexkt.space. All rights reserved.
//

#import "ViewController.h"
#include "../MediaConvert/MediaConvert/MediaConvert.h" //Base API

#include "../MediaConvert/MediaConvert/WXFfmpegConvertAPI.h" //播放器API
#include "../MediaConvert/MediaConvert/WaterMarkRemoveAPI.h" //去水印API

@implementation ViewController{
    void *m_play1;

    void *m_pCapture;
    NSString *m_strPath;
    NSString *m_strLog;
}

@synthesize m_btnPlay;
@synthesize m_textFileName;
@synthesize m_view;
@synthesize m_strInput;

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    m_strPath = [paths objectAtIndex:0];
    m_strLog = [m_strPath stringByAppendingString:@"/WXMedia.log"];
    NSLog(@"m_strLog=%@",m_strLog);
    //WXDeviceInit([m_strLog UTF8String]);
    m_play1 = NULL;
   // WXSetStreamRecord(1);
    m_pCapture = nullptr;//录屏
}

- (void)setRepresentedObject:(id)representedObject {
    [super setRepresentedObject:representedObject];

    // Update the view, if already loaded.
}

- (IBAction)onSelectFile:(id)sender {
    NSOpenPanel *openPanel = [NSOpenPanel openPanel];
    if([openPanel runModal] == NSFileHandlingPanelOKButton){
        m_strInput = [[openPanel URL] path];
        
        m_textFileName.stringValue = m_strInput ;
    }
}


- (IBAction)onPlayFile:(id)sender {
    if(m_play1 == NULL){
        if([m_strInput length] > 0){
            NSLog(@"Start FFPLAY  %@ ! !!!",m_strInput);
            m_play1 = WXFfplayCreate("FFPLAY", [m_strInput UTF8String], 100, 0);
            if(m_play1){
                m_btnPlay.title = @"Stop";
                WXFfplaySetView(m_play1, (__bridge void*)m_view);
                //WXFfplaySetVolume(m_play1,100);
                WXFfplayStart(m_play1);
                NSLog(@"Start FFPLAY !!!!");
            }
        }
    }else{
        WXFfplayStop(m_play1);
        WXFfplayDestroy(m_play1);
        m_play1 = NULL;
        m_btnPlay.title = @"Play";
    }
}




@end
