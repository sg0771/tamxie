//
//  AQAudioFilePlay.m
//  Media
//
//  Created by ftanx on 2017/6/14.
//  Copyright © 2017年 com.wxx.2007. All rights reserved.
//

#import "AQAudioFilePlay.h"
#import <AudioToolbox/AudioToolbox.h>
#import <AudioToolbox/AudioFile.h>

#define QUEUE_BUFFER_SIZE 4 //队列缓冲个数
#define EVERY_READ_LENGTH 1600 //每次从文件读取的长度

@interface AQAudioFilePlay(){
    AudioQueueRef audioQueue;//音频播放队列
    AudioQueueBufferRef audioQueueBuffers[QUEUE_BUFFER_SIZE];//音频缓存制
    FILE *file;//pcm源文件
}
@end

@implementation AQAudioFilePlay
@synthesize isOpen;

//回调函数，取数据填充到硬件
static void AudioPlayerAQInputCallback(void *input, AudioQueueRef outQ, AudioQueueBufferRef outQB){
    AQAudioFilePlay *aq_play = (__bridge AQAudioFilePlay *)(input);
    [aq_play readPCMAndPlay:outQ buffer:outQB]; //Read Data!!!
}

- (BOOL)Start:(NSString*)filename{
    if (isOpen) {
        [self Stop];
    }
    @synchronized (self) {
        NSString *filepath = [[[NSBundle mainBundle] bundlePath] stringByAppendingPathComponent:filename];
        NSLog(@"filepath = %@",filepath);
        file  = fopen([filepath UTF8String], "r");
        if(file){
            fseek(file, 0, SEEK_SET);
        } else{
            isOpen = NO;
            return NO;
        }
        ///设置音频参数
        AudioStreamBasicDescription audioDescription;///音频参数
        audioDescription.mSampleRate = 16000;//采样率
        audioDescription.mFormatID = kAudioFormatLinearPCM;
        audioDescription.mFormatFlags = kLinearPCMFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
        audioDescription.mChannelsPerFrame = 1;///单声道
        audioDescription.mFramesPerPacket = 1;//每一个packet一侦数据
        audioDescription.mBitsPerChannel = 16;//每个采样点16bit量化
        audioDescription.mBytesPerFrame = (audioDescription.mBitsPerChannel/8) * audioDescription.mChannelsPerFrame;
        audioDescription.mBytesPerPacket = audioDescription.mBytesPerFrame ;
        AudioQueueNewOutput(&audioDescription, AudioPlayerAQInputCallback, (__bridge  void*)self, nil, nil, 0, &audioQueue);//使用player的内部线程播
        ////添加buffer区
        for(int i = 0;i < QUEUE_BUFFER_SIZE; i++){
            AudioQueueAllocateBuffer(audioQueue, EVERY_READ_LENGTH, &audioQueueBuffers[i]);///创建buffer区
            [self readPCMAndPlay:audioQueue buffer:audioQueueBuffers[i]];
        }
        AudioQueueStart(audioQueue, NULL);
        isOpen = YES;
    }
    return YES;
}

-(void)readPCMAndPlay:(AudioQueueRef)outQ buffer:(AudioQueueBufferRef)outQB{
    @synchronized (self) {
        uint8_t *pcmDataBuffer = alloca(EVERY_READ_LENGTH);
        size_t readLength = fread(pcmDataBuffer, EVERY_READ_LENGTH, 1, file);//读取文件
        if(readLength){
            Byte *audiodata = (Byte *)outQB->mAudioData;
            memcpy(audiodata, pcmDataBuffer, EVERY_READ_LENGTH);
            outQB->mAudioDataByteSize = EVERY_READ_LENGTH;
        }else{
            NSLog(@"NO Data");
            Byte *audiodata = (Byte *)outQB->mAudioData;
            memset(audiodata, 0, EVERY_READ_LENGTH);
            outQB->mAudioDataByteSize = EVERY_READ_LENGTH;
        }
        AudioQueueEnqueueBuffer(outQ, outQB, 0, NULL);
    }
}

-(void)Stop{
    @synchronized (self) {
        if(isOpen){
            isOpen = NO;
            AudioQueueDispose(audioQueue,NO);
            AudioQueueStop(audioQueue, NO);
            fclose(file);
        }
    }
}


@end
