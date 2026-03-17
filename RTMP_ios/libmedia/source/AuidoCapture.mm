//
//  AuidoCapture.cpp
//  media
//
//  Created by momo on 2021/7/8.
//  Copyright © 2021 TenXie. All rights reserved.
//

#include "MSVideo.h"
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#import  <AVFoundation/AVFoundation.h>
#include <thread>
#include <mutex>

class ALCapture{
public:
    ALCdevice *m_pCapDev = NULL;
    ALchar*       m_pBuffer = NULL;
    int m_iBufsize = 0;
    int m_format = AL_FORMAT_STEREO16;
    int m_iSampleRate = 48000;
    int m_iChannel = 2;
    int m_time = 10;
    BOOL m_bStart = FALSE;
    std::thread *m_thread = nullptr;
    IAudioCaptureSink *m_pSink = nullptr;
    bool Satrt(int sample_rate, int channel, IAudioCaptureSink *sink){
        m_iSampleRate = sample_rate;
        m_iChannel = channel;
 
        m_format = m_iChannel == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
        
        m_iBufsize = m_time * m_iSampleRate * m_iChannel * 2 / 1000;
        m_pBuffer = new ALchar[m_iBufsize];
        
        const ALCchar *szCapDev = alcGetString(NULL, ALC_CAPTURE_DEFAULT_DEVICE_SPECIFIER);
        //缓冲区太小会导致没数据

        [[AVAudioSession sharedInstance] setCategory: AVAudioSessionCategoryPlayAndRecord withOptions:AVAudioSessionCategoryOptionMixWithOthers error: nil];
        [[AVAudioSession sharedInstance] setActive: YES error: nil];
        m_pSink = sink;
        m_pCapDev = alcCaptureOpenDevice(szCapDev, m_iSampleRate, m_format, m_iBufsize * 10);
        if(m_pCapDev){
            m_bStart = TRUE;
            m_thread = new std::thread(&ALCapture::threadFunc,this);
            return true;
        }
        return false;
    }
    void Stop(){
        if(m_thread){
            m_bStart = FALSE;
            m_thread->join();
            delete m_thread;
            m_thread = NULL;
        }
    }
    
    
    void threadFunc(){
        alcCaptureStart(m_pCapDev);

        while (m_bStart) {
            int iSamplesAvailable = 0;
            alcGetIntegerv(m_pCapDev, ALC_CAPTURE_SAMPLES, 1, &iSamplesAvailable);
            if (iSamplesAvailable > (m_iBufsize / 2)) {
                alcCaptureSamples(m_pCapDev, m_pBuffer, m_iBufsize / 2);
                //Write data  m_pBuffer,m_iBufsize
                m_pSink->OnAudioData((uint8_t*)m_pBuffer,m_iBufsize);
            }
            usleep(1);
        }
        alcCaptureStop(m_pCapDev);
        alcCaptureCloseDevice(m_pCapDev);
        m_pCapDev = NULL;
    }
};




//摄像头初始化
void* IAudioCaptureCreate(IAudioCaptureSink* pSink, int sample_rate, int channel){
    ALCapture * obj =  new ALCapture;
    obj->Satrt(sample_rate, channel, pSink);
    return obj;
}
//销毁摄像头
void  IAudioCaptureDestroy(void* ptr){
    ALCapture * obj =  (ALCapture*)ptr;
    obj->Stop();
}

