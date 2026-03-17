//
//  VideoTest.m
//  MediaTest
//
//  Created by ftanx on 2017/6/16.
//  Copyright © 2017年 com.wxx.2007. All rights reserved.
//

#import "VideoTest.h"
#import "MSVideoCapture.h"
#import "MSVideoDraw.h"


@interface VideoTest()<DataDelegate>{
    MSVideoCapture *Capture;
    MSVideoDraw    *Draw;
    UIView *m_view;
    BOOL bDraw;
}
@end

@implementation VideoTest

-(void)onVideoData:(uint8_t*)buf withWidth:(int)width withHeight:(int)height{
    //NSLog(@"On Video %dx%d %x %x %x %x",width,height,buf[0],buf[1],buf[2],buf[3]);
    if(!bDraw){
        bDraw = YES;
        Draw.m_iVideoWidth = width;
        Draw.m_iVideoHeight = height;
        Draw.m_bShowFull = NO;
        [Draw Open];
    }
    [Draw DrawVideoData:(void*)buf];
}

-(void)ResizeThread{
    while (1) {
            dispatch_async(dispatch_get_main_queue(), ^{
                    m_view.frame = CGRectMake(0, 0, 200 + rand() % 50, 300 + rand() % 60);
                [Draw DrawAudioLevel:(rand() % 256 * 32767 / 256)];
            });
        sleepMs(200);
    }
}

-(void)Start:(UIView*)view{
    bDraw = NO;
    Draw = [[MSVideoDraw alloc] initWithParent:view doAction:nil];

    Capture = [[MSVideoCapture alloc] init];
    Capture.delegate = self;
    Capture.m_rotate = 90;
    [Capture Start];
    
   NSThread *t = [[NSThread alloc] initWithTarget:self selector:@selector(ResizeThread) object:nil];
    [t start];
}

-(void)Stop{
    [Capture Stop];
}

@end
