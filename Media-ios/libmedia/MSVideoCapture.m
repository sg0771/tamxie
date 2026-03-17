//
//  MSVideoCapture.m
//  media
//
//  Created by ftanx on 2017/5/19.
//  Copyright © 2017年 TenXie. All rights reserved.
//

#import <AVFoundation/AVFoundation.h>
#import "MSVideoCapture.h"
#import "wh_utils_objc.h"
#include <libyuv.h>

@interface MSVideoCapture() <AVCaptureVideoDataOutputSampleBufferDelegate>{
@public
    AVCaptureDevice *m_FrontDevice;
    AVCaptureDevice *m_BackDevice;
    AVCaptureSession    *m_avSession;
    AVCaptureDevice     *m_avDevice;
    AVCaptureDeviceInput *m_avInput;
    AVCaptureVideoDataOutput *m_avOutput;
    
    uint8_t *m_pBuf;
    uint64_t m_tsStart;
    int32_t m_iFrameNum;
}
@end

@implementation MSVideoCapture

@synthesize  m_vsize;
@synthesize  m_rotate;
@synthesize  m_iFps;
@synthesize  m_isFront;
@synthesize  m_isRunning;
@synthesize  m_iWidth;
@synthesize  m_iHeight;
@synthesize  delegate;
- (id) init{
    if((self = [super init]) != nil) {
        m_avSession = nil;
        m_avDevice = nil;
        m_isRunning = NO;
        m_iWidth = 0;
        m_iHeight = 0;
        m_isFront = YES;
        m_FrontDevice = nil;
        m_BackDevice = nil;
        m_tsStart = 0;
        m_iFrameNum = 0;
        m_iFps = 10;
        m_vsize = 0;
        delegate = nil;
        NSArray *cameras = [AVCaptureDevice devicesWithMediaType:AVMediaTypeVideo];
        if(cameras != nil){
            for(AVCaptureDevice *device in cameras) {
                if(device.position == AVCaptureDevicePositionFront) {
                    m_FrontDevice = device;
                }
                if(device.position == AVCaptureDevicePositionBack) {
                    m_BackDevice = device;
                }
            }
        }
    }
    return self;
}

//some param set other
- (BOOL) Start{
    @synchronized (self) {
        m_avDevice = m_isFront ? m_FrontDevice : m_BackDevice;
        NSError *error = nil;
        
        m_avSession = [[AVCaptureSession alloc] init];
        switch (m_vsize) {
            case 0:
                m_avSession.sessionPreset = AVCaptureSessionPreset352x288;
                break;
            case 1:
                m_avSession.sessionPreset = AVCaptureSessionPreset640x480;
                break;
            case 2:
                m_avSession.sessionPreset = AVCaptureSessionPreset1280x720;
                break;
            default:
                m_avSession.sessionPreset = AVCaptureSessionPreset352x288;
                break;
        }
        m_avInput = [AVCaptureDeviceInput deviceInputWithDevice:m_avDevice error:&error];
        [m_avSession addInput:m_avInput];
        
        m_avOutput= [[AVCaptureVideoDataOutput alloc] init];
        NSDictionary* dict = [NSDictionary dictionaryWithObjectsAndKeys:
                              [NSNumber numberWithInteger:kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange],
                              (id)kCVPixelBufferPixelFormatTypeKey, nil];
        [m_avOutput setVideoSettings:dict];
        dispatch_queue_t queue = dispatch_queue_create("com.wh2007.VideoCapture", NULL);
        [m_avOutput setSampleBufferDelegate:self queue:queue];
        [m_avSession addOutput:m_avOutput];
        [m_avSession startRunning];
        m_isRunning = YES;
    }
    return YES;
}

- (BOOL) Stop{
    @synchronized (self) {
        if(!m_isRunning) return NO;
        if(m_avSession) {
            [m_avSession stopRunning];
        }
        if(m_pBuf){
            free(m_pBuf);
            m_pBuf = NULL;
        }
        m_isRunning = NO;
    }
    return YES;
}


- (BOOL)Switch{
    [self Stop];
    m_isFront = !m_isFront;
    return [self Start];
}
//Video Delegate
- (void)captureOutput:(AVCaptureOutput *)captureOutput
didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer
       fromConnection:(AVCaptureConnection *)connection{
    if(m_iFrameNum == 0){
        m_tsStart = GetTimeStamp();
    }
    uint64_t time = GetTimeStamp() - m_tsStart;
    uint64_t time2 = m_iFrameNum * 1000 / m_iFps;
    //NSLog(@"time = %lld time2 = %lld",time, time2);
    if(time >= time2){
        m_iFrameNum++;//有效帧
    }else{
        return;//不采集数据
    }
    CVImageBufferRef frame = CMSampleBufferGetImageBuffer(sampleBuffer);
    if(CVPixelBufferLockBaseAddress(frame, 0) == kCVReturnSuccess) {
        int Width   = (int)CVPixelBufferGetWidth(frame);
        int Height  = (int)CVPixelBufferGetHeight(frame);
        if(m_pBuf == NULL){
            int Videosize = Width * Height * 3 / 2;
            m_pBuf = (uint8_t*)wh_malloc(Videosize);
        }
        uint8_t* pY  = (uint8_t*)CVPixelBufferGetBaseAddressOfPlane(frame, 0);
        int PitchY   = (int)CVPixelBufferGetBytesPerRowOfPlane(frame, 0);
        uint8_t* pUV = (uint8_t*)CVPixelBufferGetBaseAddressOfPlane(frame, 1);
        int PitchUV  = (int)CVPixelBufferGetBytesPerRowOfPlane(frame, 1);
        uint8_t* pYDst = m_pBuf;
        uint8_t* pUDst = m_pBuf + Width * Height;
        uint8_t* pVDst = m_pBuf + Width * Height * 5 / 4;
        switch (m_rotate) {
            case 0:
                m_iWidth  = Width;
                m_iHeight = Height;
                NV12ToI420Rotate(pY, PitchY,pUV, PitchUV, pYDst, m_iWidth,pUDst, m_iWidth/2,pVDst, m_iWidth/2,
                                 m_iWidth, m_iHeight, kRotate0);
                break;
            case 90:
                m_iWidth  = Height;
                m_iHeight = Width;
                NV12ToI420Rotate(pY, PitchY,pUV, PitchUV, pYDst, m_iWidth,pUDst, m_iWidth/2,pVDst, m_iWidth/2,
                                 Width, Height, kRotate90);
                break;
            case 180:
                m_iWidth  = Width;
                m_iHeight = Height;
                NV12ToI420Rotate(pY, PitchY,pUV, PitchUV, pYDst, m_iWidth,pUDst, m_iWidth/2,pVDst, m_iWidth/2,
                                 m_iWidth, m_iHeight, kRotate180);
                break;
            case 270:
                m_iWidth  = Height;
                m_iHeight = Width;
                NV12ToI420Rotate(pY, PitchY,pUV, PitchUV, pYDst, m_iWidth,pUDst, m_iWidth/2,pVDst, m_iWidth/2,
                                 Width, Height, kRotate270);
                break;
            default:
                break;
        }
        CVPixelBufferUnlockBaseAddress(frame, 0);
        if(delegate != nil)[delegate onVideoData:m_pBuf withWidth:m_iWidth withHeight:m_iHeight];
    }
}

@end

