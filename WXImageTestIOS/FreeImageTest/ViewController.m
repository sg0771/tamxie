//
//  ViewController.m
//  FreeImageTest
//
//  Created by momo on 2021/10/25.
//

#import  "ViewController.h"
#import  <CoreFoundation/CoreFoundation.h>
#import  <CoreGraphics/CGImage.h>
#include "../WXImage/libyuv.h"
#include "../WXImage/WXImage.h"


static void WXReadFile(const char* fileName, uint8_t** image_data, int* image_size){
    *image_data = NULL;
    *image_size = 0;
    FILE* fin = fopen(fileName, "rb");
    if(NULL != fin){
        fseek(fin, 0, SEEK_END);
        *image_size = (int)ftell(fin);
        if(image_size > 0){
            NSLog(@"Image Size=%d",*image_size);
            *image_data = (uint8_t*)malloc(*image_size);
            fseek(fin, 0, SEEK_SET);
            fread(*image_data, 1, *image_size, fin);
        }
        fclose(fin);
    }
}


@interface ViewController ()

@end

@implementation ViewController
{
    NSString * m_strPath;
    int m_nQuality;
    int m_nSize;
    int m_nDstW;
    int m_nDstH;
    uint8_t* m_data;
    int m_datasize;
}

@synthesize m_txtDstWidth;
@synthesize m_txtDstHeight;
@synthesize m_txtQuality;
@synthesize m_txtSize;
@synthesize m_labelLogInfo;

- (void)viewDidLoad {
    [super viewDidLoad];
    
    m_strPath = [[NSBundle mainBundle] pathForResource:@"b.raw" ofType:nil];
    NSLog(@"PATH = %@", m_strPath);
    
    // Do any additional setup after loading the view.
    [m_txtDstWidth setText:@"0"];
    [m_txtDstHeight setText:@"0"];
    [m_txtQuality setText:@"80"];
    [m_txtSize setText:@"800"];
    
    [m_labelLogInfo setText:@"INFO"];
}

- (void)GetData{
    m_nDstW = [[m_txtDstWidth text]  intValue];
    m_nDstH = [[m_txtDstHeight text]  intValue];
    m_nQuality = [[m_txtQuality text]  intValue];
    m_nSize = [[m_txtSize text]  intValue];
    NSLog(@"[%dx%d] Quality=%d Size=%d",m_nDstW,m_nDstH,m_nQuality,m_nSize);
    
    WXReadFile([m_strPath UTF8String], &m_data, &m_datasize);
    if(m_datasize){
        NSLog(@"DataSize=%d",m_datasize);
    }
}


- (IBAction)onQualityOrig:(id)sender {
    NSLog(@"%s",__FUNCTION__);
    [self GetData];
    
    dispatch_queue_t queue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
    dispatch_async(queue, ^{
        
        if(m_data){
            
           // NSLog(@"Read Size = %d!!",image_size);
            
            void* dib = WXImage_Load(m_data, m_datasize);//从内存数据获得 Bitmap对象
            
            if(NULL != dib){
                //提取出原始数据用于图像处理
                int width  =  WXImage_GetWidth(dib);//返回分辨率宽度
                int height =  WXImage_GetHeight(dib);//返回分辨率高度
                int channel = WXImage_GetChannel(dib);//RGB Type
                int Pitch  = WXImage_GetPitch(dib);//返回每行数据量
                int ResX = WXImage_GetDotsPerMeterX(dib);
                int ResY = WXImage_GetDotsPerMeterX(dib);
               
                
                void* handle = HandlerCreate();
                int ret = CompressQuality_BufferToBuffer(m_data, m_datasize,
                    handle, WXIMAGE_TYPE_ORIGINAL, m_nQuality, m_nDstW, m_nDstH);
                
                if(ret >= 0){

                    dispatch_async(dispatch_get_main_queue(), ^{
                        int data_size = HandlerGetSize(handle);
                        NSString* strInfo = [[NSString alloc] initWithFormat:@"Orgi [%d,%d,%d,%d] Res=[%dx%d]  TargetSize[%d]=%d", width, height, channel,Pitch, ResX, ResY,  m_nQuality, data_size];
                        NSLog(@"%@",strInfo);
                        　  [m_labelLogInfo setText:strInfo];
                    });
                }
                HandlerDestroy(handle);
            }
            free(m_data);
            m_data = NULL;
            m_datasize = 0;
        }
    });
}

- (IBAction)onQualityJpeg:(id)sender {
    NSLog(@"%s",__FUNCTION__);
    [self GetData];
    
    dispatch_queue_t queue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
    dispatch_async(queue, ^{
        
        if(m_data){
            
           // NSLog(@"Read Size = %d!!",image_size);
            
            void* dib = WXImage_Load(m_data, m_datasize);//从内存数据获得 Bitmap对象
            
            if(NULL != dib){
                //提取出原始数据用于图像处理
                int width  =  WXImage_GetWidth(dib);//返回分辨率宽度
                int height =  WXImage_GetHeight(dib);//返回分辨率高度
                int channel = WXImage_GetChannel(dib);//RGB Type
                int Pitch  = WXImage_GetPitch(dib);//返回每行数据量
                int ResX = WXImage_GetDotsPerMeterX(dib);
                int ResY = WXImage_GetDotsPerMeterX(dib);
               
                
                void* handle = HandlerCreate();
                int ret = CompressQuality_BufferToBuffer(m_data, m_datasize,
                    handle, WXIMAGE_TYPE_JPEG, m_nQuality, m_nDstW, m_nDstH);
                
                if(ret >= 0){

                    dispatch_async(dispatch_get_main_queue(), ^{
                        int data_size = HandlerGetSize(handle);
                        NSString* strInfo = [[NSString alloc] initWithFormat:@"Orgi [%d,%d,%d,%d] Res=[%dx%d]  TargetSize[%d]=%d", width, height, channel,Pitch, ResX, ResY,  m_nQuality, data_size];
                        NSLog(@"%@",strInfo);
                        　  [m_labelLogInfo setText:strInfo];
                    });
                }
                HandlerDestroy(handle);
            }
            free(m_data);
            m_data = NULL;
            m_datasize = 0;
        }
    });
}

- (IBAction)onQualityPng:(id)sender {
    NSLog(@"%s",__FUNCTION__);
    [self GetData];
    
    dispatch_queue_t queue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
    dispatch_async(queue, ^{
        
        if(m_data){
            
           // NSLog(@"Read Size = %d!!",image_size);
            
            void* dib = WXImage_Load(m_data, m_datasize);//从内存数据获得 Bitmap对象
            
            if(NULL != dib){
                //提取出原始数据用于图像处理
                int width  =  WXImage_GetWidth(dib);//返回分辨率宽度
                int height =  WXImage_GetHeight(dib);//返回分辨率高度
                int channel = WXImage_GetChannel(dib);//RGB Type
                int Pitch  = WXImage_GetPitch(dib);//返回每行数据量
                int ResX = WXImage_GetDotsPerMeterX(dib);
                int ResY = WXImage_GetDotsPerMeterX(dib);
               
                
                void* handle = HandlerCreate();
                int ret = CompressQuality_BufferToBuffer(m_data, m_datasize,
                    handle, WXIMAGE_TYPE_PNG, m_nQuality, m_nDstW, m_nDstH);
                
                if(ret >= 0){

                    dispatch_async(dispatch_get_main_queue(), ^{
                        int data_size = HandlerGetSize(handle);
                        NSString* strInfo = [[NSString alloc] initWithFormat:@"Orgi [%d,%d,%d,%d] Res=[%dx%d]  TargetSize[%d]=%d", width, height, channel,Pitch, ResX, ResY,  m_nQuality, data_size];
                        NSLog(@"%@",strInfo);
                        　  [m_labelLogInfo setText:strInfo];
                    });
                }
                HandlerDestroy(handle);
            }
            free(m_data);
            m_data = NULL;
            m_datasize = 0;
        }
    });
}


- (IBAction)onQualityWebp:(id)sender {
    NSLog(@"%s",__FUNCTION__);
    [self GetData];
    
    dispatch_queue_t queue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
    dispatch_async(queue, ^{
        
        if(m_data){
            
           // NSLog(@"Read Size = %d!!",image_size);
            
            void* dib = WXImage_Load(m_data, m_datasize);//从内存数据获得 Bitmap对象
            
            if(NULL != dib){
                //提取出原始数据用于图像处理
                int width  =  WXImage_GetWidth(dib);//返回分辨率宽度
                int height =  WXImage_GetHeight(dib);//返回分辨率高度
                int channel = WXImage_GetChannel(dib);//RGB Type
                int Pitch  = WXImage_GetPitch(dib);//返回每行数据量
                int ResX = WXImage_GetDotsPerMeterX(dib);
                int ResY = WXImage_GetDotsPerMeterX(dib);
               
                
                void* handle = HandlerCreate();
                int ret = CompressQuality_BufferToBuffer(m_data, m_datasize,
                    handle, WXIMAGE_TYPE_WEBP, m_nQuality, m_nDstW, m_nDstH);
                
                if(ret >= 0){

                    dispatch_async(dispatch_get_main_queue(), ^{
                        int data_size = HandlerGetSize(handle);
                        NSString* strInfo = [[NSString alloc] initWithFormat:@"Orgi [%d,%d,%d,%d] Res=[%dx%d]  TargetSize[%d]=%d", width, height, channel,Pitch, ResX, ResY,  m_nQuality, data_size];
                        NSLog(@"%@",strInfo);
                        　  [m_labelLogInfo setText:strInfo];
                    });
                }
                HandlerDestroy(handle);
            }
            free(m_data);
            m_data = NULL;
            m_datasize = 0;
        }
    });
}


- (IBAction)onQualityPng8:(id)sender {
    NSLog(@"%s",__FUNCTION__);
    [self GetData];
    
    dispatch_queue_t queue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
    dispatch_async(queue, ^{
        
        if(m_data){
            
           // NSLog(@"Read Size = %d!!",image_size);
            
            void* dib = WXImage_Load(m_data, m_datasize);//从内存数据获得 Bitmap对象
            
            if(NULL != dib){
                //提取出原始数据用于图像处理
                int width  =  WXImage_GetWidth(dib);//返回分辨率宽度
                int height =  WXImage_GetHeight(dib);//返回分辨率高度
                int channel = WXImage_GetChannel(dib);//RGB Type
                int Pitch  = WXImage_GetPitch(dib);//返回每行数据量
                int ResX = WXImage_GetDotsPerMeterX(dib);
                int ResY = WXImage_GetDotsPerMeterX(dib);
               
                
                void* handle = HandlerCreate();
                int ret = CompressPNG8_BufferToBuffer(m_data, m_datasize,
                    handle, m_nQuality, m_nDstW, m_nDstH);
                
                if(ret >= 0){

                    dispatch_async(dispatch_get_main_queue(), ^{
                        int data_size = HandlerGetSize(handle);
                        NSString* strInfo = [[NSString alloc] initWithFormat:@"Orgi [%d,%d,%d,%d] Res=[%dx%d]  TargetSize[%d]=%d", width, height, channel,Pitch, ResX, ResY,  m_nQuality, data_size];
                        NSLog(@"%@",strInfo);
                        　  [m_labelLogInfo setText:strInfo];
                    });
                }
                HandlerDestroy(handle);
            }
            free(m_data);
            m_data = NULL;
            m_datasize = 0;
        }
    });
}



- (IBAction)onSizeOrig:(id)sender {
    NSLog(@"%s",__FUNCTION__);
    [self GetData];
    
    dispatch_queue_t queue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
    dispatch_async(queue, ^{
        
        if(m_data){
            
           // NSLog(@"Read Size = %d!!",image_size);
            
            void* dib = WXImage_Load(m_data, m_datasize);//从内存数据获得 Bitmap对象
            
            if(NULL != dib){
                //提取出原始数据用于图像处理
                int width  =  WXImage_GetWidth(dib);//返回分辨率宽度
                int height =  WXImage_GetHeight(dib);//返回分辨率高度
                int channel = WXImage_GetChannel(dib);//RGB Type
                int Pitch  = WXImage_GetPitch(dib);//返回每行数据量
                int ResX = WXImage_GetDotsPerMeterX(dib);
                int ResY = WXImage_GetDotsPerMeterX(dib);
               
                
                void* handle = HandlerCreate();
                int ret = CompressSize_BufferToBuffer(m_data, m_datasize,
                    handle, WXIMAGE_TYPE_ORIGINAL, m_nSize, m_nDstW, m_nDstH);
                
                if(ret >= 0){

                    dispatch_async(dispatch_get_main_queue(), ^{
                        int data_size = HandlerGetSize(handle);
                        NSString* strInfo = [[NSString alloc] initWithFormat:@"Orgi [%d,%d,%d,%d] Res=[%dx%d]  TargetSize[%d]=%d", width, height, channel,Pitch, ResX, ResY,  m_nQuality, data_size];
                        NSLog(@"%@",strInfo);
                        　  [m_labelLogInfo setText:strInfo];
                    });
                }
                HandlerDestroy(handle);
            }
            free(m_data);
            m_data = NULL;
            m_datasize = 0;
        }
    });
}

- (IBAction)onSizeJpeg:(id)sender {
    NSLog(@"%s",__FUNCTION__);
    [self GetData];
    
    dispatch_queue_t queue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
    dispatch_async(queue, ^{
        
        if(m_data){
            
           // NSLog(@"Read Size = %d!!",image_size);
            
            void* dib = WXImage_Load(m_data, m_datasize);//从内存数据获得 Bitmap对象
            
            if(NULL != dib){
                //提取出原始数据用于图像处理
                int width  =  WXImage_GetWidth(dib);//返回分辨率宽度
                int height =  WXImage_GetHeight(dib);//返回分辨率高度
                int channel = WXImage_GetChannel(dib);//RGB Type
                int Pitch  = WXImage_GetPitch(dib);//返回每行数据量
                int ResX = WXImage_GetDotsPerMeterX(dib);
                int ResY = WXImage_GetDotsPerMeterX(dib);
               
                
                void* handle = HandlerCreate();
                int ret = CompressSize_BufferToBuffer(m_data, m_datasize,
                    handle, WXIMAGE_TYPE_JPEG, m_nSize, m_nDstW, m_nDstH);
                
                if(ret >= 0){

                    dispatch_async(dispatch_get_main_queue(), ^{
                        int data_size = HandlerGetSize(handle);
                        NSString* strInfo = [[NSString alloc] initWithFormat:@"Orgi [%d,%d,%d,%d] Res=[%dx%d]  TargetSize[%d]=%d", width, height, channel,Pitch, ResX, ResY,  m_nQuality, data_size];
                        NSLog(@"%@",strInfo);
                        　  [m_labelLogInfo setText:strInfo];
                    });
                }
                HandlerDestroy(handle);
            }
            free(m_data);
            m_data = NULL;
            m_datasize = 0;
        }
    });
}

- (IBAction)onSizeWebp:(id)sender {
    NSLog(@"%s",__FUNCTION__);
    [self GetData];
    
    dispatch_queue_t queue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
    dispatch_async(queue, ^{
        
        if(m_data){
            
           // NSLog(@"Read Size = %d!!",image_size);
            
            void* dib = WXImage_Load(m_data, m_datasize);//从内存数据获得 Bitmap对象
            
            if(NULL != dib){
                //提取出原始数据用于图像处理
                int width  =  WXImage_GetWidth(dib);//返回分辨率宽度
                int height =  WXImage_GetHeight(dib);//返回分辨率高度
                int channel = WXImage_GetChannel(dib);//RGB Type
                int Pitch  = WXImage_GetPitch(dib);//返回每行数据量
                int ResX = WXImage_GetDotsPerMeterX(dib);
                int ResY = WXImage_GetDotsPerMeterX(dib);
               
                
                void* handle = HandlerCreate();
                int ret = CompressSize_BufferToBuffer(m_data, m_datasize,
                    handle, WXIMAGE_TYPE_WEBP, m_nSize, m_nDstW, m_nDstH);
                
                if(ret >= 0){

                    dispatch_async(dispatch_get_main_queue(), ^{
                        int data_size = HandlerGetSize(handle);
                        NSString* strInfo = [[NSString alloc] initWithFormat:@"Orgi [%d,%d,%d,%d] Res=[%dx%d]  TargetSize[%d]=%d", width, height, channel,Pitch, ResX, ResY,  m_nQuality, data_size];
                        NSLog(@"%@",strInfo);
                        　  [m_labelLogInfo setText:strInfo];
                    });
                }
                HandlerDestroy(handle);
            }
            free(m_data);
            m_data = NULL;
            m_datasize = 0;
        }
    });
}

@end
