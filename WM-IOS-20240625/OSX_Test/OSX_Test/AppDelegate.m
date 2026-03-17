//
//  AppDelegate.m
//  OSX_Test
//
//  Created by momo on 2023/12/30.
//

#import "AppDelegate.h"
#include <MediaConvert.h> //Base API

#include <WXFfmpegConvertAPI.h> //播放器API
#include <WaterMarkRemoveAPI.h> //去水印API



@interface AppDelegate ()

@property (strong) IBOutlet NSWindow *window;
@end

@implementation AppDelegate{
    void *m_play;

    NSString *m_strPath;
    NSString *m_strLog;
    
    
    NSString *m_strInput;
    NSString *m_strOutput;
}

@synthesize m_txtInput;
@synthesize m_txtOutput;
@synthesize m_image;

//INIT
- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // Insert code here to initialize your application
    NSLog(@"%s",__FUNCTION__);
    
    m_play = NULL;
    
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    m_strPath = [paths objectAtIndex:0];
    m_strLog = [m_strPath stringByAppendingString:@"/WXMedia.log"];
    NSLog(@"m_strLog=%@",m_strLog);
    WXFfmpegInit();
    
    m_strInput = nil;
    m_strOutput = nil;
}


- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
    NSLog(@"%s",__FUNCTION__);
}


- (BOOL)applicationSupportsSecureRestorableState:(NSApplication *)app {
    NSLog(@"%s",__FUNCTION__);
    return YES;
}


//720p
//去水印视频转换
static int test_delogo_convert(char* media_in_file, char* media_out_file ,int x, int y,int w, int h ) {
    WXFfmpegInit();
    
    int(*logo_rects)[4] = (int(*)[4])malloc(20 * sizeof(int));
    int nb_logos =1;
    logo_rects[0][0] = x ;
    logo_rects[0][1] = y ;
    logo_rects[0][2] = w ;
    logo_rects[0][3] = h ;

    void* convert = CreateConvert();
    int ret = InitSource(convert, media_in_file, media_out_file, logo_rects, nb_logos);
    if (ret > 0) {
        //SetVideoConvertTime(convert,0, 60000);
        StartConvert(convert);

        int pos = 0;
        int last_pos = 0;
        fprintf(stdout, "Progress: %d\n", pos);
        fflush(stdout);
        while (1) {
            pos = GetConvertProgress(convert);
            if (pos >= 0 && pos < 100) {
                if (pos != last_pos) {
                    fprintf(stdout, "Progress: %d\n", pos);
                    fflush(stdout);
                    last_pos = pos;
                }
                sleep(1);
            }
            else {
                if (pos < 0) {
                    fprintf(stderr, "Error:converting error!!!\n");
                    fflush(stderr);
                }
                else {
                    fprintf(stdout, "Progress: 100\n");
                    fflush(stdout);
                }
                DestroyConvert(convert);
                convert = NULL;
                break;
            }
        }
    }
    else {
        {
            fprintf(stderr, "Error:convert init error!!!\n");
            fflush(stderr);
        }
        return 0;
    }

    if (logo_rects) {
        //free(logo_rects);
        //logo_rects = NULL;
    }


    return 1;
}


- (IBAction)onConvert:(id)sender {
    NSLog(@"%s",__FUNCTION__);
    const char *szInput  = [m_strInput UTF8String];
    const char *szOutput = [m_strOutput UTF8String];
    test_delogo_convert(szInput,szOutput,70, 70,250,250);
    
    
}

- (IBAction)onStop:(id)sender {
    NSLog(@"%s",__FUNCTION__);
    if(m_play != NULL){
        WXFfplayStop(m_play);
        WXFfplayDestroy(m_play);
        m_play = NULL;
    }
}


//单个水印去除，基于avframe数据，但是并不依赖相关库,dst与src地址相同，不用拷贝，不同需要拷贝，必须同分辨率
//dst：目标帧data[]
//dst_linesize：目标帧linesize[]
//src：源帧data[]
//src_linesize：源帧linesize[]
//w:图像宽
//h:图像高
//pix_fmt:图像颜色空间，比较有限
//logo_x:水印x坐标
//logo_y:水印y坐标
//logo_w:水印宽
//logo_h:水印高
//band:水印外扩宽
//logo_h:是否显示去水印框
//WXDELOGO_CAPI int WXDelogo(uint8_t** dstData, int* dst_linesize,
 //   uint8_t** srcData, int* src_linesize,
 //   int w, int h, enum WXPixelFMT pix_fmt,
 //   int logo_x, int logo_y, int logo_w, int logo_h,
 //  int band, int show);
//播放时去回调
static void onRenderData(uint8_t** data, int* linesize, int width, int height){
    WXDelogo(data, linesize,
        data, linesize,
        width, height, WX_PIX_FMT_YUV420,
             70, 70, 300, 300,0, 0);
}

- (IBAction)onPlay:(id)sender {
    NSLog(@"%s",__FUNCTION__);
    if(m_play == NULL){
        if([m_strInput length] > 0){
            NSLog(@"Start FFPLAY  %@ ! !!!",m_strInput);
            m_play = WXFfplayCreate("FFPLAY", [m_strInput UTF8String], 100, 0);
            if(m_play){
                NSLog(@"------------- %dx%d",(int)m_image.frame.size.width,(int)m_image.frame.size.height);
                WXFfplaySetView(m_play, (__bridge void*)m_image);
                //WXFfplaySetVolume(m_play,100);
                WXFfplaySetVideoCB(m_play, onRenderData);//播放时去水印
                WXFfplayStart(m_play);
                NSLog(@"Start FFPLAY !!!!");
            }
        }
    }
}



//Save
- (IBAction)onOutputFile:(id)sender {
    NSLog(@"%s",__FUNCTION__);

    NSSavePanel*    panel = [NSSavePanel savePanel];
    [panel setNameFieldStringValue:@"Save.mp4"];
    [panel setMessage:@"Choose the path to save the document"];
    [panel setAllowsOtherFileTypes:YES];
    [panel setAllowedFileTypes:nil];
    [panel setExtensionHidden:YES];
    [panel setCanCreateDirectories:YES];
    [panel beginSheetModalForWindow:self.window completionHandler:^(NSInteger result){
        if (result == NSFileHandlingPanelOKButton) {
                m_strOutput = [[panel URL] path];
                [@"mp4" writeToFile:m_strOutput atomically:YES encoding:NSUTF8StringEncoding error:nil];
                m_txtOutput.stringValue = m_strOutput ;
        }
    }];
}


//opne
- (IBAction)onInputFile:(id)sender {
    NSLog(@"%s",__FUNCTION__);
    
    NSOpenPanel *openPanel = [NSOpenPanel openPanel];
    if([openPanel runModal] == NSFileHandlingPanelOKButton){
        m_strInput = [[openPanel URL] path];
        
        m_txtInput.stringValue = m_strInput ;
    }
}
@end
