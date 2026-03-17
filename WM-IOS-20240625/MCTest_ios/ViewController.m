//
//  ViewController.m
//  MCTest
//
//  Created by momo on 2022/3/23.
//

#import "ViewController.h"

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

static void onRenderData(uint8_t** data, int* linesize, int width, int height){
    WXDelogo(data, linesize,
        data, linesize,
        width, height, WX_PIX_FMT_YUV420,
             70, 70, 300, 300,0, 0);
}

@interface ViewController (){
    int m_bMCInit;
    void  *m_handle;
    int m_iWidth;
    int m_iHeight;
    int m_iFps;
    void*m_player;//播放器句柄
}

@end


@implementation ViewController
@synthesize m_label;
@synthesize m_view;

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    
    m_handle = NULL;
    m_bMCInit = MediaConvertLibraryInit();
    m_player = NULL;
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

- (IBAction)onMC1:(id)sender {
    NSString *strMp4= [[NSBundle mainBundle] pathForResource:@"1" ofType:@"mp4"];
    const char *szMp4 = [strMp4 UTF8String];
    NSLog(@"Input Mp4 = %s",szMp4);
    NSString *documents = [NSHomeDirectory() stringByAppendingPathComponent:@"Documents"];
    //拼接文件绝对路径
    NSString *path = [documents stringByAppendingPathComponent:@"1_delogo_999.mp4"];
    const char *strOutput = [path UTF8String];
    NSLog(@"Output Mp4 = %s",strOutput);
    test_delogo_convert(szMp4,strOutput,70, 70,250,250);
    NSLog(@"OK!!!");
}


- (IBAction)onMC2:(id)sender {
    NSLog(@"ffplay Start++----- ");
    if(NULL == m_player){
        NSString *strMp4= [[NSBundle mainBundle] pathForResource:@"1" ofType:@"mp4"];
        const char *szMp4 = [strMp4 UTF8String];
        m_player = WXFfplayCreate(NULL, szMp4, 100, 0);
        if(m_player){
            WXFfplaySetView(m_player, (__bridge void*)m_view);
            WXFfplaySetVideoCB(m_player, onRenderData);
            WXFfplayStart(m_player);
            NSLog(@"ffplay Start++++");
        }
    }
}



//default
- (IBAction)onMC0:(id)sender {

    NSLog(@"ffplay Stop!");
    if(m_player != NULL){
        
        NSLog(@"ffplay Stop!----");
        WXFfplayStop(m_player);
        WXFfplayDestroy(m_player);
        m_player = NULL;
        
        NSLog(@"ffplay Destroy!!!");
    }
}

@end
