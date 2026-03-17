//
//  ViewController.m
//  Media
//
//  Created by momo on 2018/11/29.
//  Copyright © 2018年 Tam. All rights reserved.
//

#import "ViewController.h"

#import "../libffmpeg/WXBase.h"
#import "../libffmpeg/WXMediaAPI.h"

#include <time.h>
#include <sys/time.h>
#include <stdint.h>
#include <mach/mach_time.h>


@implementation ViewController
{
    void *m_play1;
    int64_t ptsTotol;
    int64_t ptsCurr;
    
    //串行队列
    dispatch_queue_t m_serialQueueProcess;
    //串行队列
    dispatch_queue_t m_serialQueueLog;
}

@synthesize m_btnPlay;
@synthesize m_textFileName;

@synthesize m_strInput;

- (void)viewDidLoad {
    [super viewDidLoad];

    // Do any additional setup after loading the view.
    WXFfmpegInit();
    m_play1 = NULL;
    ptsTotol = 0;
    ptsCurr = 0;
    //队列
    m_serialQueueProcess = dispatch_queue_create("WXMedia.process", DISPATCH_QUEUE_SERIAL);
    
    //Log队列
    m_serialQueueLog = dispatch_queue_create("WXMedia.log", DISPATCH_QUEUE_SERIAL);

    dispatch_async(m_serialQueueLog, ^{
        while(1){

            if(ptsTotol != 0 && ptsCurr!= 0){
                NSLog(@"PtsCurr=%lld  PtsTotal=%lld",ptsCurr, ptsTotol);
            }
            WXSleepMs(5);
        }
    });
    
   // int64_t ts = clock_gettime();
}

- (void)setRepresentedObject:(id)representedObject {
    [super setRepresentedObject:representedObject];
}

- (IBAction)onSelectFile:(id)sender {

    NSOpenPanel *openPanel = [NSOpenPanel openPanel];
    if([openPanel runModal] == NSFileHandlingPanelOKButton){
        m_strInput = [[openPanel URL] path];

        m_textFileName.stringValue = m_strInput ;
    }
}

- (IBAction)onPlay:(id)sender {
    
    if(m_play1 == NULL){
        if([m_strInput length] > 0){
            NSLog(@"Start FFPLAY  %@ ! !!!",m_strInput);
            m_play1 = WXFfplayCreate("FFPLAY", [m_strInput UTF8String], 100, 0);
            if(m_play1){
                m_btnPlay.stringValue = @"Stop";
                WXFfplaySetView(m_play1, (__bridge void*)_m_viewDisplay);
                WXFfplaySetVolume(m_play1,100);
                WXFfplayStart(m_play1);
                NSLog(@"Start FFPLAY !!!!");
            }
        }
    }else{
        WXFfplayStop(m_play1);
        m_play1 = NULL;
        m_btnPlay.stringValue = @"Play";
    }

}
@end
