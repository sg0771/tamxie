//
//  ViewController.m
//  VA
//
//  Created by TenXie on 2017/2/16.
//  Copyright © 2017年 TenXie. All rights reserved.
//

#import "ViewController.h"
#import <AudioToolbox/AudioToolbox.h>
#import <AVFoundation/AVFoundation.h>
#include <ifaddrs.h>
#include <arpa/inet.h>

#include "RtmpSender.h"
#include "MSVideo.h"

class RTMP_Sender_Test : public IVideoCaptureSink, public IAudioCaptureSink{

    int m_iSampleRate = 48000;
    int m_iChannel = 2;
    void *m_cap = nullptr;
    void *m_rtmp = nullptr;
    int m_num = 0;
    std::string m_strURL;
    IMSDraw *m_draw = nullptr;
    void *m_view = nullptr;
    
    void* m_audio = nullptr;
public:
    virtual void OnAudioData(uint8_t *buf, int buf_size){
        if(m_rtmp)
            RtmpSenderSendPcm(m_rtmp, buf, buf_size);
    }
    virtual void OnVideoData(uint8_t *buf, int width, int height){
        if(m_rtmp == nullptr){
            m_rtmp = RtmpSenderCreate(m_strURL.c_str(), width, height, m_iSampleRate, m_iChannel);
        }
        if(m_draw == nullptr){
            m_draw = IMSDraw::Create();
            m_draw->SetView(m_view);
            m_draw->SetSize(width, height);
        }
        if(m_rtmp){
            m_draw->Draw(buf);
            RtmpSenderSendI420(m_rtmp, buf);
        }
    }
    int Open(const char *szURL, UIView *view){
        m_strURL = szURL;
        m_cap = IVideoCaptureCreate((IVideoCaptureSink*)this);
        m_view = (__bridge void*) view;
        m_audio = IAudioCaptureCreate(this, m_iSampleRate, m_iChannel);
        return 0;
    }
    void Close(){
        IVideoCaptureDestroy(m_cap);
        m_cap = nullptr;
        IAudioCaptureDestroy(m_audio);
        m_audio = nullptr;
    }
};

@interface ViewController (){
    RTMP_Sender_Test *m_test;
}
@end

@implementation ViewController


- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
    //
    
    [[[NSURLSession sharedSession] dataTaskWithURL:[NSURL URLWithString:@"https://www.baidu.com"] completionHandler:^(NSData * _Nullable data, NSURLResponse * _Nullable response, NSError * _Nullable error) {
          
      }] resume];
    
    signal(SIGPIPE, SIG_IGN);
}


- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}


- (IBAction)onStart:(id)sender {
     if(nullptr == m_test){
         m_test = new RTMP_Sender_Test;
        NSString *strIP = m_label.text;
         m_test->Open([strIP UTF8String], m_va);
    }
}

- (IBAction)onStop:(id)sender {
    if(nullptr != m_test){
        m_test->Close();
        m_test = nullptr;
    }

}

@end
