//
//  ViewController.m
//  MCTest
//
//  Created by momo on 2022/3/23.
//

#import "ViewController.h"

@interface ViewController (){
    
    int m_bMCInit;
    void  *m_handle;
    int m_iWidth;
    int m_iHeight;
    int m_iFps;
    
}

@end


@implementation ViewController
@synthesize m_label;

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    
    m_handle = NULL;
    m_bMCInit = MediaConvertLibraryInit();
    
    if(m_bMCInit){
        NSTimer *timer = [NSTimer scheduledTimerWithTimeInterval:1.0 repeats:YES block:^(NSTimer * _Nonnull timer) {
            
            if(self->m_handle){
                
                int rate = MediaConvertGetState(self->m_handle);
                if(rate >=0 && rate < 100){
                    //正常进度
                    NSString *str = [[NSString alloc]initWithFormat:@"转换进度 %d",rate];
                    [self->m_label setText:str];
                }else if(rate == -2){
                    NSString *str = [[NSString alloc]initWithFormat:@"没有开始转换"];
                    [self->m_label setText:str];
                }else if(rate == -1){
                    NSString *str = [[NSString alloc]initWithFormat:@"转换失败"];
                    [self->m_label setText:str];
                    MediaConvertDestroy(self->m_handle);
                    self->m_handle = NULL;
                }else if(rate == 100){
                    NSString *str = [[NSString alloc]initWithFormat:@"转换成功"];
                    [self->m_label setText:str];
                    MediaConvertDestroy(self->m_handle);
                    self->m_handle = NULL;
                }
                
            }

        }];
    }

}


//default
- (IBAction)onMC0:(id)sender {
    
    if(m_bMCInit && m_handle== NULL){
        NSString *strMp4= [[NSBundle mainBundle] pathForResource:@"a" ofType:@"mp4"];
        const char *szMp4 = [strMp4 UTF8String];
        NSLog(@"Mp4 = %s",szMp4);
        m_handle = MediaConvertCreate(szMp4);
        if(m_handle){
            
            m_iWidth = MediaConvertGetWidth(m_handle);
            m_iHeight = MediaConvertGetHeight(m_handle);
            m_iFps = MediaConvertGetFps(m_handle);
            int target_size = MediaConvertGetLength(m_handle, m_iWidth, m_iHeight, m_iFps, MODE_HIGH);
            NSLog(@"Parser Video OK [%dx%d %dfps] targetsize=%d",m_iWidth,m_iHeight,m_iFps,target_size);
            
            NSString *documents = [NSHomeDirectory() stringByAppendingPathComponent:@"Documents"];
            //拼接文件绝对路径
            NSString *path = [documents stringByAppendingPathComponent:@"default.mp4"];
            const char *strOutput = [path UTF8String];
            NSLog(@"Output File = %s",strOutput);
            
            //拼接文件绝对路径
            NSString *pathJpeg = [documents stringByAppendingPathComponent:@"default.jpg"];
            const char *strOutputJpeg = [pathJpeg UTF8String];
            NSLog(@"Output Jpeg File = %s",strOutputJpeg);
            int getJpeg = MediaConvertGetThumb(m_handle, strOutputJpeg);
            if(getJpeg){
                NSLog(@"get Jpeg OK");
            }else{
                NSLog(@"get Jpeg Error");
            }
            
            MediaConvertExcute(m_handle, m_iWidth, m_iHeight,m_iFps,target_size, strOutput);
        }
    }
}


//720p
- (IBAction)onMC1:(id)sender {
    if(m_bMCInit && m_handle== NULL){
        NSString *strMp4= [[NSBundle mainBundle] pathForResource:@"a" ofType:@"mp4"];
        const char *szMp4 = [strMp4 UTF8String];
        NSLog(@"Mp4 = %s",szMp4);
        m_handle = MediaConvertCreate(szMp4);
        if(m_handle){
            m_iWidth = MediaConvertGetWidth(m_handle);
            m_iHeight = MediaConvertGetHeight(m_handle);
            m_iFps = MediaConvertGetFps(m_handle);
            int target_size = MediaConvertGetLength(m_handle, 1280, 720, 0, MODE_NORMAL);
            NSLog(@"720P TarSize=%d mb",target_size);
            
            NSString *documents = [NSHomeDirectory() stringByAppendingPathComponent:@"Documents"];
            //拼接文件绝对路径
            NSString *path = [documents stringByAppendingPathComponent:@"default.mp4"];
            const char *strOutput = [path UTF8String];
            NSLog(@"Output File = %s",strOutput);
            MediaConvertExcute(m_handle, 1280, 720,0,target_size, strOutput);
            
        }
    }

}



- (IBAction)onMC2:(id)sender {
    
    if(m_bMCInit && m_handle== NULL){
        NSString *strMp4= [[NSBundle mainBundle] pathForResource:@"a" ofType:@"mp4"];
        const char *szMp4 = [strMp4 UTF8String];
        NSLog(@"Mp4 = %s",szMp4);
        m_handle = MediaConvertCreate(szMp4);
        if(m_handle){
            m_iWidth = MediaConvertGetWidth(m_handle);
            m_iHeight = MediaConvertGetHeight(m_handle);
            m_iFps = MediaConvertGetFps(m_handle);
            int target_size = MediaConvertGetLength(m_handle, m_iWidth, m_iHeight, m_iFps, MODE_NORMAL);
            NSLog(@"Parser Video OK [%dx%d %dfps] targetsize=%d",m_iWidth,m_iHeight,m_iFps,target_size);
            
            NSString *documents = [NSHomeDirectory() stringByAppendingPathComponent:@"Documents"];
            //拼接文件绝对路径
            NSString *path = [documents stringByAppendingPathComponent:@"default.webm"];
            const char *strOutput = [path UTF8String];
            NSLog(@"Output File = %s",strOutput);
            MediaConvertExcute(m_handle, m_iWidth, m_iHeight,m_iFps,target_size, strOutput);
        }
    }
}


@end
