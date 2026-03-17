//
//  MSVideoCapture.m
//  VA
//
//  Created by ftanx on 2017/4/4.
//  Copyright © 2017年 TenXie. All rights reserved.
//


#include "MSVideo.h"
#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#import <UIKit/UIKit.h>

#include <libyuv.h>



@interface SC_VideoCapturer : NSObject <AVCaptureVideoDataOutputSampleBufferDelegate>{
@public
    AVCaptureDevice *m_FrontDevice;
    AVCaptureDevice *m_BackDevice;
    AVCaptureSession    *m_avSession;
    AVCaptureDevice     *m_avDevice;
    AVCaptureDeviceInput *m_avInput;
    AVCaptureVideoDataOutput *m_avOutput;
    BOOL                m_isFront;
    BOOL                m_isRunning;
    IVideoCaptureSink *m_pSink;//Sink data to C++ obj
    int m_vsize;
    int     m_iWidth;
    int     m_iHeight;
    int     m_iVideosize;
    int     m_rotate;
    uint8_t *m_pBuf;
    uint64_t m_tsStart;
    int32_t m_iFps;
    int32_t m_iFrame;
}
- (BOOL) startCapture;
- (BOOL) stopCapture;
@end

@implementation SC_VideoCapturer

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
        m_iFrame = 0;
        m_iFps = 10;
        m_vsize = 0;
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
- (BOOL) startCapture{
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
                          [NSNumber numberWithInteger:kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange], (id)kCVPixelBufferPixelFormatTypeKey, nil];
    [m_avOutput setVideoSettings:dict];
    
    if ([m_avOutput.connections count] > 0) {
        AVCaptureConnection *conn = [m_avOutput.connections objectAtIndex:0];
        [conn setVideoMinFrameDuration:CMTimeMake(1, m_iFps)];
        [conn setVideoMaxFrameDuration:CMTimeMake(1, m_iFps)];
    }
    
    dispatch_queue_t queue = dispatch_queue_create("com.WXMedia.VideoCapture", NULL);
    [m_avOutput setSampleBufferDelegate:self queue:queue];
    [m_avSession addOutput:m_avOutput];
    [m_avSession startRunning];
    m_isRunning = YES;
    return YES;
}

- (BOOL) stopCapture{
    if(!m_isRunning) return NO;
    if(m_avSession) {
        [m_avSession stopRunning];
        m_avSession = nil;
    }
    if(m_pBuf){
        free(m_pBuf);
        m_pBuf = NULL;
    }
    m_isRunning = NO;
    return YES;
}

//Video Delegate
- (void)captureOutput:(AVCaptureOutput *)captureOutput
        didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer
        fromConnection:(AVCaptureConnection *)connection{

    CVImageBufferRef frame = CMSampleBufferGetImageBuffer(sampleBuffer);
    if(CVPixelBufferLockBaseAddress(frame, 0) == kCVReturnSuccess) {
        int Width   = (int)CVPixelBufferGetWidth(frame);
        int Height  = (int)CVPixelBufferGetHeight(frame);
        if(m_pBuf == NULL){
            m_iVideosize = Width * Height * 3 / 2;
            m_pBuf = (uint8_t*)malloc(m_iVideosize);
        }
        
        //NSLog(@"WxH = %dx%d",Width,Height);
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
                libyuv::NV12ToI420Rotate(pY, PitchY,pUV, PitchUV, pYDst, m_iWidth,pUDst, m_iWidth/2,pVDst, m_iWidth/2,
                                         m_iWidth, m_iHeight, libyuv::kRotate0);
                break;
            case 90:
                m_iWidth  = Height;
                m_iHeight = Width;
                libyuv::NV12ToI420Rotate(pY, PitchY,pUV, PitchUV, pYDst, m_iWidth,pUDst, m_iWidth/2,pVDst, m_iWidth/2,
                                         Width, Height, libyuv::kRotate90);
                break;
            case 180:
                m_iWidth  = Width;
                m_iHeight = Height;
                libyuv::NV12ToI420Rotate(pY, PitchY,pUV, PitchUV, pYDst, m_iWidth,pUDst, m_iWidth/2,pVDst, m_iWidth/2,
                                         m_iWidth, m_iHeight, libyuv::kRotate180);
                break;
            case 270:
                m_iWidth  = Height;
                m_iHeight = Width;
                libyuv::NV12ToI420Rotate(pY, PitchY,pUV, PitchUV, pYDst, m_iWidth,pUDst, m_iWidth/2,pVDst, m_iWidth/2,
                                         Width, Height, libyuv::kRotate270);
                break;
            default:
                break;
        }
        m_pSink->OnVideoData(m_pBuf,m_iWidth, m_iHeight);
        CVPixelBufferUnlockBaseAddress(frame, 0);
    }
}

@end

//================================================================================
class VideoCapture{
    SC_VideoCapturer *m_capture = nil;
public:
    int  Open(IVideoCaptureSink* pSink, int vsize = 0, int isFront = 1, int fps = 10, int rotate = 0){
        m_capture = [[SC_VideoCapturer alloc] init];
        m_capture->m_pSink = pSink;
        m_capture->m_isFront = isFront ? YES : NO;
        m_capture->m_iFps = fps;
        m_capture->m_vsize = vsize;
        m_capture->m_rotate = 90;
        return [m_capture startCapture];
    }
    void Close(){

        [m_capture stopCapture];
    }

    virtual int  GetWidth(){
        return m_capture->m_iWidth;
    }
    virtual int  GetHeight(){
        return m_capture->m_iHeight;
    }
};

//摄像头初始化
void* IVideoCaptureCreate(IVideoCaptureSink* pSink){
    VideoCapture *obj = new VideoCapture;
    obj->Open(pSink);
    return obj;
}
//销毁摄像头
void  IVideoCaptureDestroy(void* ptr){
    VideoCapture *obj = (VideoCapture*)ptr;
    obj->Close();
}
