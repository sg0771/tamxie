//
//  MSAUSession.m
//  Media
//
//  Created by ftanx on 2017/6/14.
//  Copyright © 2017年 com.wxx.2007. All rights reserved.
//
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#include <AudioToolbox/AudioToolbox.h>
#import  <AVFoundation/AVFoundation.h>
#import "MSAUSession.h"

#define kOutputBus 0
#define kInputBus 1
#define SAMPLE_RATE  16000
#define MAX_CHANNEL  100
#define WH_FRAMESIZE 1600
#define PCM_SIZE     800

static void GetAudioSt(int16_t *pcm, int count, int* iMax, int* iAvg){
    int max_v = 0;
    int64_t sum2 = 0.0;
    for(int i = 0; i < count; i++){
        if(abs(pcm[i]) > max_v)max_v = abs(pcm[i]);
        sum2 += abs(pcm[i]) ;
    }
    *iMax = max_v;
    *iAvg = (int)(sum2 / count);
}

static int16_t Mix_add_short(int16_t a, int16_t b){
    int sum = a + b;
    if(sum > 32767)sum = 32767;
    if(sum < -32767)sum = -32767;
    return (int16_t)sum;
}

static void Mixer_Process(int16_t *dst, int16_t*src, int length){
    for (int i = 0; i < length; i++) {
        dst[i] = Mix_add_short(src[i],dst[i]);
    }
}

@interface MSAUSession(){
@public
    //read_cb 缓存
    wh_buffer m_readBuf;
    
    //write_cb 缓存
    wh_buffer m_writeBuf;
    
    wh_mempool m_pool[MAX_CHANNEL];
    wh_queue   m_audioChannel[MAX_CHANNEL]; //音频通道
    int        m_iMaxC[MAX_CHANNEL];
    
    int16_t    mix_data[PCM_SIZE]; //混音数据
    wh_mutex_t m_mutexMixer; //混音锁
    NSThread   *mThreadMixer;//混音线程
    
    AudioUnit	m_unit;
}
@end

//static void au_interruption_listener (void  *inClientData, UInt32   inInterruptionState){}

#pragma mark Recording Callback
static OSStatus read_cb(void *inRefCon,
                                  AudioUnitRenderActionFlags *ioActionFlags,
                                  const AudioTimeStamp *inTimeStamp,
                                  UInt32 inBusNumber,
                                  UInt32 inNumberFrames,
                                  AudioBufferList *ioData) {
    MSAUSession*session = (__bridge MSAUSession*)inRefCon;
    int size = inNumberFrames * 2;
    AudioBufferList readAudioBufferList;
    readAudioBufferList.mBuffers[0].mDataByteSize = size;
    readAudioBufferList.mNumberBuffers=1;
    readAudioBufferList.mBuffers[0].mNumberChannels = 1;
    readAudioBufferList.mBuffers[0].mData = alloca(size);
    AudioTimeStamp ts;
    OSStatus err = AudioUnitRender(session->m_unit, ioActionFlags, &ts, inBusNumber,inNumberFrames, &readAudioBufferList);
    //NSLog(@"xxxReadCB = %d",size);
    uint8_t *buf = (uint8_t*)readAudioBufferList.mBuffers[0].mData;

    if (err == 0 && session.delegate != nil) {
        buffer_put(&session->m_readBuf, buf,size); //输出缓冲区        
        while(session->m_readBuf.size >= WH_FRAMESIZE){
            uint8_t *pcm = alloca(WH_FRAMESIZE);//for Wanghui
            buffer_read(&session->m_readBuf, pcm, WH_FRAMESIZE);
            [session.delegate onAudioData:pcm Size:WH_FRAMESIZE];
        }
    }

    return noErr;
}


static OSStatus write_cb(void *inRefCon,
                                 AudioUnitRenderActionFlags *ioActionFlags,
                                 const AudioTimeStamp *inTimeStamp,
                                 UInt32 inBusNumber,
                                 UInt32 inNumberFrames,
                                 AudioBufferList *ioData) {
    MSAUSession*session = (__bridge MSAUSession*)inRefCon;

    int size = inNumberFrames * 2;
    if(session->m_writeBuf.size >= size){
        buffer_read(&session->m_writeBuf, ioData->mBuffers[0].mData, size);
    }else{
        memset(ioData->mBuffers[0].mData, 0, size);
    }
    return noErr;
}



@implementation MSAUSession
@synthesize isOpen;
@synthesize delegate;


- (int)  AddChannel{
    if(!isOpen)return -1;
    @synchronized (self) {
        int channel = -1;
        for (int i = 0; i < MAX_CHANNEL; i++) {
            if(!m_audioChannel[i].use){
                queue_init(&m_audioChannel[i]);
                m_audioChannel[i].use = YES;
                mempool_init(&m_pool[i]);
                channel = i;
                break;
            }
        }
        return channel;
    }
}

- (void) RemoveChannel:(int)channel{
    if(!isOpen)return;
    @synchronized (self) {
        if(m_audioChannel[channel].use){
            queue_flush(&m_audioChannel[channel]);
            mempool_destroy(&m_pool[channel]);
        }
    }
}

- (void) WriteDate:(int)channel Data:(uint8_t*)buf Size:(int)size{
    if(!isOpen)return;
    wh_mutex_lock(&m_mutexMixer);
    if(m_audioChannel[channel].use){
        wh_packet *m = mempool_allocp(&m_pool[channel], buf, size);
        queue_push(&m_audioChannel[channel], m);
    }
    wh_mutex_unlock(&m_mutexMixer);
}

- (BOOL) GetMixerData{
    memset(mix_data, 0, WH_FRAMESIZE);
    if(!isOpen)return NO;
    wh_mutex_lock(&m_mutexMixer);
    BOOL bHave = NO;
    for (int channel = 0; channel < MAX_CHANNEL;channel++) {
        int packet = m_audioChannel[channel].q_mcount;
        if(m_audioChannel[channel].use && packet > 2){
            wh_packet *m = queue_pop(&m_audioChannel[channel]);
            if(packet > 20){ //该队列数据太多，
                mempool_free(&m_pool[channel], m);
                continue;
            }
            
            int avg = 0;
            GetAudioSt((int16_t*)m->pBuf, 800, &m_iMaxC[channel], &avg);//256
            if(packet > 5 && avg < 512){
                //队列数据多，删除静音包加快播放
                mempool_free(&m_pool[channel], m);
                continue;
            }

            Mixer_Process(mix_data, (int16_t*)m->pBuf, PCM_SIZE);//混音计算，只适合2-3个人吧
            bHave = YES;
            mempool_free(&m_pool[channel], m);
        }
    }
    wh_mutex_unlock(&m_mutexMixer);
    if(!bHave)return NO;
    return YES;
}

//混音线程函数
-(void)threadMixerPorocess{
    NSLog(@"Startxxxx");
    while (isOpen) {
        if(m_writeBuf.size < 1600 * 2){
            BOOL bMix = [self GetMixerData];
            if(bMix)
                buffer_put(&m_writeBuf, (uint8_t*)mix_data, WH_FRAMESIZE);
        }
        sleepMs(10);
    }
}

-(MSAUSession*)init{
    self = [super init];
    if(self){
        isOpen = NO;
        delegate = nil;
        buffer_init(&m_readBuf);
        buffer_init(&m_writeBuf);
        mThreadMixer = [[NSThread alloc] initWithTarget:self selector:@selector(threadMixerPorocess) object:nil];
    }
    return self;
}

- (BOOL)Open{
    if(!isOpen)[self Close];
    @synchronized (self) {
    
        for (int i = 0; i < MAX_CHANNEL; i++) {
            queue_init(&m_audioChannel[i]);
        }
        wh_mutex_init(&m_mutexMixer,NULL);

        
        OSStatus status = 0;
        UInt32 doSetProperty = 1;
        UInt32 doNotSetProperty = 0;
        
       // AudioSessionInitialize(NULL, NULL, au_interruption_listener, (__bridge void*)self); //去掉之后不能选择扬声器/听筒模式
        
        AudioSessionSetActive(true);
        UInt32 category = kAudioSessionCategory_PlayAndRecord;
        AudioSessionSetProperty(kAudioSessionProperty_AudioCategory, sizeof(category), &category);//采集模式
        AudioSessionSetProperty(kAudioSessionProperty_OverrideCategoryDefaultToSpeaker,sizeof (doSetProperty),&doSetProperty);//支持扬声器
        
        Float32 preferredBufferSize = 0.15;
        AudioSessionSetProperty(kAudioSessionProperty_PreferredHardwareIOBufferDuration,sizeof(preferredBufferSize), &preferredBufferSize);
        
        AudioComponentDescription au_description;
        au_description.componentType          = kAudioUnitType_Output;
        au_description.componentSubType       = kAudioUnitSubType_VoiceProcessingIO;
        au_description.componentManufacturer  = kAudioUnitManufacturer_Apple;
        au_description.componentFlags         = 0;
        au_description.componentFlagsMask     = 0;
        AudioComponent foundComponent = AudioComponentFindNext (NULL,&au_description);
        AudioComponentInstanceNew (foundComponent, &m_unit);

        //Set Hardware Samplerate
        Float64 hwsamplerate = SAMPLE_RATE;
        AudioSessionSetProperty(kAudioSessionProperty_PreferredHardwareSampleRate,sizeof(hwsamplerate), &hwsamplerate);
        
        // Enable IO for playback
       AudioUnitSetProperty (m_unit, kAudioOutputUnitProperty_EnableIO,kAudioUnitScope_Output ,kOutputBus,&doSetProperty,sizeof (doSetProperty));
        
        // Enable IO for recording
        AudioUnitSetProperty(m_unit,kAudioOutputUnitProperty_EnableIO,kAudioUnitScope_Input,kInputBus,&doSetProperty,sizeof(doSetProperty));

        //disable unit buffer allocation
        AudioUnitSetProperty(m_unit,  kAudioUnitProperty_ShouldAllocateBuffer, kAudioUnitScope_Output,  kInputBus, &doNotSetProperty,sizeof(doNotSetProperty));
    
        // Describe format
        AudioStreamBasicDescription audioFormat;
        audioFormat.mSampleRate			= SAMPLE_RATE;
        audioFormat.mFormatID			= kAudioFormatLinearPCM;
        audioFormat.mFormatFlags		= kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
        audioFormat.mFramesPerPacket	= 1;
        audioFormat.mChannelsPerFrame	= 1;
        audioFormat.mBitsPerChannel		= 16;
        audioFormat.mBytesPerPacket		= 2;
        audioFormat.mBytesPerFrame		= 2;
        
        // Apply format
        AudioUnitSetProperty(m_unit,kAudioUnitProperty_StreamFormat,kAudioUnitScope_Output,kInputBus, &audioFormat,sizeof(audioFormat));
        AudioUnitSetProperty(m_unit,kAudioUnitProperty_StreamFormat,kAudioUnitScope_Input, kOutputBus,&audioFormat,sizeof(audioFormat));
        

        // Set input callback
        AURenderCallbackStruct callbackStruct;
        callbackStruct.inputProc = read_cb;
        callbackStruct.inputProcRefCon = (__bridge void*)self;
        status = AudioUnitSetProperty(m_unit,kAudioOutputUnitProperty_SetInputCallback, kAudioUnitScope_Global,kInputBus,&callbackStruct,sizeof(callbackStruct));
        
        // Set output callback
        callbackStruct.inputProc = write_cb;
        callbackStruct.inputProcRefCon =  (__bridge void*)self;
        status = AudioUnitSetProperty(m_unit,kAudioUnitProperty_SetRenderCallback, kAudioUnitScope_Global, kOutputBus,&callbackStruct,sizeof(callbackStruct));

        AudioUnitInitialize(m_unit);
        AudioOutputUnitStart(m_unit);
        isOpen = YES;
        [mThreadMixer start];
        return isOpen;
    }
}

- (void)Close{
    @synchronized (self) {
        if(isOpen){
            isOpen = NO;
            [mThreadMixer cancel];
            for (int i = 0; i < MAX_CHANNEL; i++) {
                queue_flush(&m_audioChannel[i]);
            }
            wh_mutex_destroy(&m_mutexMixer);
            buffer_flush(&m_readBuf);
            buffer_flush(&m_writeBuf);
            AudioOutputUnitStop(m_unit);
            AudioComponentInstanceDispose(m_unit);
            AudioUnitUninitialize(m_unit);
            AudioSessionSetActiveWithFlags(false, kAudioSessionSetActiveFlag_NotifyOthersOnDeactivation);
        }
    }
}

- (int) getWriteSize:(int)chanel{
    if(m_audioChannel[chanel].use){
        return m_audioChannel[chanel].q_mcount * WH_FRAMESIZE;
    }
    return 0;
}

@end

