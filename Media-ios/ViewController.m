//
//  ViewController.m
//  Media
//
//  Created by ftanx on 2017/6/14.
//  Copyright © 2017年 com.wxx.2007. All rights reserved.
//

#import "ViewController.h"
#import "AUTest.h"
#import "VideoTest.h"

@interface ViewController (){
    //AQAudioFilePlay *fileplay;
   // AQAudioCapture *capture;
    AUTest    *audio_test;
    VideoTest *video_test;
}
@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
   // fileplay = nil;
   // capture = [[AQAudioCapture alloc] init];
    //m_view.frame = CGRectMake(0, 0, self.view.frame.size.width/3, self.view.frame.size.height/3);
    
    video_test = [[VideoTest alloc] init];
    [video_test Start:self.view];
    //audio_test = [[AUTest alloc] init];[audio_test Start];
}


- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (IBAction)onStart:(id)sender {
    //fileplay = [[AQAudioFilePlay alloc] init];
    //[fileplay Start:@"1.pcm"];
    //[capture Open];
    
    //[test Start];

}
- (IBAction)onStop:(id)sender {
    

}

@end
