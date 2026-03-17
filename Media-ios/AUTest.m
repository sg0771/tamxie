//
//  AUTest.m
//  Media
//
//  Created by ftanx on 2017/6/14.
//  Copyright © 2017年 com.wxx.2007. All rights reserved.
//

#import "AUTest.h"
#import "MSAUSession.h"
#import "MSOpusEncoder.h"
#import "MSOpusDecoder.h"

#define FRAME_SIZE 1600

@interface AUTest()<DataDelegate>{
    BOOL isOpen;
    MSAUSession *session;
    int channel;
    
    FILE* m_f1;
    NSThread *mTh1;
    int ch1;
    
    FILE* m_f2;
    NSThread *mTh2;
    int ch2;
    
    FILE* m_f3;
    NSThread *mTh3;
    int ch3;
}
@end


@implementation AUTest

-(void)func1{
    uint8_t *pcm = alloca(FRAME_SIZE);
    MSOpusEncoder *encoder = [[MSOpusEncoder alloc] init];
    MSOpusDecoder *decoder = [[MSOpusDecoder alloc] init];
    uint8_t *enc = alloca(FRAME_SIZE);
    uint8_t *dec = alloca(FRAME_SIZE);
    int ts = 40;
    while (isOpen) {
        if(!fread(pcm, FRAME_SIZE, 1, m_f1)){
            fseek(m_f1, 0, 0);
            fread(pcm, FRAME_SIZE, 1, m_f1);
        }
        
        [encoder Encode:pcm encodeData:enc];
        //NSLog(@"Opus Enc length = %d",enc_length);
        [decoder Decode:enc decodeData:dec];
        
        
        [session WriteDate:ch1 Data:dec Size:FRAME_SIZE];
        int writesize = [session getWriteSize:ch1];
        if(writesize > FRAME_SIZE * 5){
            ts = 60;
           // NSLog(@"CH1 Slow Now");
        }else{
            ts = 40;
           // NSLog(@"CH1 Fast Now");
        }
        sleepMs(ts);
    }
}

-(void)func2{
    uint8_t *pcm = alloca(FRAME_SIZE);
    int ts = 40;
    while (isOpen) {
        if(!fread(pcm, FRAME_SIZE, 1, m_f2)){
            fseek(m_f2, 0, 0);
            fread(pcm, FRAME_SIZE, 1, m_f2);
        }
        [session WriteDate:ch2 Data:pcm Size:FRAME_SIZE];
        int writesize = [session getWriteSize:ch2];
        if(writesize > FRAME_SIZE * 5){
            ts = 60;
           // NSLog(@"CH2 Slow Now");
        }else{
            ts = 40;
           // NSLog(@"CH2 Fast Now");
        }
        sleepMs(ts);
    }
}


-(void)func3{
    uint8_t *pcm = alloca(FRAME_SIZE);
    int ts = 40;
    while (isOpen) {
        if(!fread(pcm, FRAME_SIZE, 1, m_f3)){
            fseek(m_f3, 0, 0);
            fread(pcm, FRAME_SIZE, 1, m_f3);
        }
        [session WriteDate:ch3 Data:pcm Size:FRAME_SIZE];
        int writesize = [session getWriteSize:ch3];
        if(writesize > FRAME_SIZE * 5){
            ts = 60;
            //NSLog(@"CH3 Slow Now");
        }else{
            ts = 40;
            //NSLog(@"CH3 Fast Now");
        }
        sleepMs(ts);
    }
}



-(void)Start{
    session = [[MSAUSession alloc] init];
    session.delegate = self;
    [session Open];
    isOpen = YES;
    channel = [session AddChannel];
    
    m_f1 = fopen(PathRes("1.pcm"),"rb");
    if(0 && m_f1 != NULL){
        ch1 = [session AddChannel];
        mTh1 = [[NSThread alloc] initWithTarget:self selector:@selector(func1) object:nil];
        [mTh1 start];
    }

    m_f2 = fopen(PathRes("2.pcm"),"rb");
    if(0 && m_f2 != NULL){
        ch2 = [session AddChannel];
        mTh2 = [[NSThread alloc] initWithTarget:self selector:@selector(func2) object:nil];
        [mTh2 start];
    }
    
    m_f3 = fopen(PathRes("3.pcm"),"rb");
    if(0 && m_f3 != NULL){
        ch3 = [session AddChannel];
        mTh3 = [[NSThread alloc] initWithTarget:self selector:@selector(func3) object:nil];
        [mTh3 start];
    }
}
-(void)Stop{
    isOpen = NO;
}

-(void)onAudioData:(uint8_t*)buf Size:(int)size{
    [session WriteDate:channel Data:buf Size:size];
}

@end
