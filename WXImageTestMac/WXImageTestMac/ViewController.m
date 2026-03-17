//
//  ViewController.m
//  WXImageTestMac
//
//  Created by momo on 2021/10/25.
//

#import "ViewController.h"

#include "../WXImage/WXImage/WXImage.h"

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

@implementation ViewController

@synthesize m_txtInput;
@synthesize m_txtOutput;

@synthesize m_txtDstWidth;
@synthesize m_txtDstHeight;

@synthesize m_log;
@synthesize m_txtQuality;


@synthesize m_txtSize;

- (void)viewDidLoad {
    [super viewDidLoad];
    
    
    [m_txtQuality setStringValue:@"80"];
    [m_txtDstWidth setStringValue:@"0"];
    [m_txtDstHeight setStringValue:@"0"];
    
    [m_txtSize setStringValue:@"800"];
    
    //NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentationDirectory, NSUserDomainMask, YES);
    //NSString *strDoc = ([paths count] > 0) ? [paths objectAtIndex:0] : nil;
    
    
    //[m_txtInput setStringValue:@"/Users/momo/Desktop/a.raw"];
    //[m_txtOutput setStringValue:@"/Users/momo/Desktop/a_out.png"];
    
}


- (void)setRepresentedObject:(id)representedObject {
    [super setRepresentedObject:representedObject];

    // Update the view, if already loaded.
}

- (IBAction)onInput:(id)sender {
    NSOpenPanel *openPanel = [NSOpenPanel openPanel];
    [openPanel setCanChooseDirectories:NO];//是否可以选择文件夹
    [openPanel setCanChooseFiles:YES];//是否可以选择文件
    BOOL okButtonPressed = ([openPanel runModal] == NSModalResponseOK);
    //[openPanel runModal]此时线程会停在这里等待选择
    //NO表示用户取消 YES表示用户做出选择
    if(okButtonPressed) {
       NSString *path = [[openPanel URL] path];
       [m_txtInput setStringValue:path];
        
        uint8_t *image_data = NULL;
        int image_size = 0;
        WXReadFile([path UTF8String], &image_data, &image_size);
        if(image_data){
            
            void* dib = WXImage_Load(image_data, image_size);//从内存数据获得 Bitmap对象
            if(NULL != dib){
                NSLog(@"laod image using WXImage OK!!");
                //提取出原始数据用于图像处理
                int width  =  WXImage_GetWidth(dib);//返回分辨率宽度
                int height =  WXImage_GetHeight(dib);//返回分辨率高度
                int channel = WXImage_GetChannel(dib);//RGB Type
                int Pitch  = WXImage_GetPitch(dib);//返回每行数据量
                int ResX = WXImage_GetDotsPerMeterX(dib);
                int ResY = WXImage_GetDotsPerMeterX(dib);
               // uint8_t* image_data =  WXImage_GetBits(dib);//返回图像数据
                NSString* strInfo = [[NSString alloc] initWithFormat:@"Image [%d,%d,%d,%d]\n Res=[%dx%d]",
                      width, height, channel,Pitch, ResX, ResY];
                [m_log setStringValue:strInfo];
            }
            free(image_data);
        }
        
    }
    
}


- (IBAction)onOutput:(id)sender {
    NSSavePanel*    panel = [NSSavePanel savePanel];
    [panel setNameFieldStringValue:@"Create File"];
    [panel setMessage:@"Choose the path to save the document"];
    [panel setAllowsOtherFileTypes:YES];
    //[panel setAllowedFileTypes:@[@".txt"]];//设置新建文件默认的后缀,默认是无后缀需自己添加
    [panel setExtensionHidden:YES];
    [panel setCanCreateDirectories:YES];
    [panel beginSheetModalForWindow:self.view.window completionHandler:^(NSInteger result){
        if (result == NSFileHandlingPanelOKButton) {//NSFileHandlingPanelOKButton
            NSString *path = [[panel URL] path];
            [m_txtOutput setStringValue:path];
        }
    }];
}

- (IBAction)onQualityPng8:(id)sender {
    NSString* strInput = [m_txtInput stringValue];
    NSString* strOutPut = [m_txtOutput stringValue];
    NSString* strQuality = [m_txtQuality stringValue]; //UI 线程
    int nQuality = [strQuality intValue];
    
    int nDstW = [[m_txtDstWidth stringValue] intValue];
    int nDstH = [[m_txtDstHeight stringValue] intValue];
    
    dispatch_queue_t queue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
    dispatch_async(queue, ^{
        
        if([strInput length] > 0){
            
            NSLog(@"Quality = %d", nQuality);
            
            uint8_t *image_data = NULL;
            int image_size = 0;
            WXReadFile([strInput UTF8String], &image_data, &image_size);
            if(image_data){
                
                NSLog(@"Read Size = %d!!",image_size);
                
                void* dib = WXImage_Load(image_data, image_size);//从内存数据获得 Bitmap对象
                
                if(NULL != dib){
                    NSLog(@"laod image using WXImage OK!!");
                    //提取出原始数据用于图像处理
                    int width  =  WXImage_GetWidth(dib);//返回分辨率宽度
                    int height =  WXImage_GetHeight(dib);//返回分辨率高度
                    int channel = WXImage_GetChannel(dib);//RGB Type
                    int Pitch  = WXImage_GetPitch(dib);//返回每行数据量
                    int ResX = WXImage_GetDotsPerMeterX(dib);
                    int ResY = WXImage_GetDotsPerMeterX(dib);
                   
                    
                    void* handle = HandlerCreate();
                    int ret = CompressPNG8_BufferToBuffer(image_data, image_size,
                        handle, nQuality, nDstW, nDstH);
                    
                    NSLog(@"laod image using WXImage OK!!  Ret=%d",ret);
                    
                    if(ret >= 0){
                        int data_size = HandlerGetSize(handle);
                        uint8_t * tmp = (uint8_t*)malloc(data_size);
                        HandlerGetData(handle, tmp);
                        
                        FILE* fout = fopen([strOutPut UTF8String], "wb");
                        if(fout){
                            
                            fwrite(tmp, data_size, 1, fout);
                            fclose(fout);
                            
                            NSString* strInfo = [[NSString alloc] initWithFormat:@"Orgi %@ [%d,%d,%d,%d]\n Res=[%dx%d]  TargetSize[%d]=%d",
                                                 strOutPut, width, height, channel,Pitch, ResX, ResY,  nQuality, data_size];
                            dispatch_async(dispatch_get_main_queue(), ^{
                                　  [self.m_log setStringValue:strInfo];
                            });
                        }else{
                            NSLog(@"Fopen %@ Error",strOutPut);
                        }
                        free(tmp);
                    }
                    HandlerDestroy(handle);
                }
                free(image_data);
            }else{
                NSLog(@"Read Size ERROR");
            }
        }
    });
}

- (IBAction)onQualityWebp:(id)sender {
    
    NSString* strInput = [m_txtInput stringValue];
    NSString* strOutPut = [m_txtOutput stringValue];
    NSString* strQuality = [m_txtQuality stringValue]; //UI 线程
    int nQuality = [strQuality intValue];
    
    int nDstW = [[m_txtDstWidth stringValue] intValue];
    int nDstH = [[m_txtDstHeight stringValue] intValue];
    
    dispatch_queue_t queue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
    dispatch_async(queue, ^{
        
        if([strInput length] > 0){
            
            NSLog(@"Quality = %d", nQuality);
            
            uint8_t *image_data = NULL;
            int image_size = 0;
            WXReadFile([strInput UTF8String], &image_data, &image_size);
            if(image_data){
                
                NSLog(@"Read Size = %d!!",image_size);
                
                void* dib = WXImage_Load(image_data, image_size);//从内存数据获得 Bitmap对象
                
                if(NULL != dib){
                    NSLog(@"laod image using WXImage OK!!");
                    //提取出原始数据用于图像处理
                    int width  =  WXImage_GetWidth(dib);//返回分辨率宽度
                    int height =  WXImage_GetHeight(dib);//返回分辨率高度
                    int channel = WXImage_GetChannel(dib);//RGB Type
                    int Pitch  = WXImage_GetPitch(dib);//返回每行数据量
                    int ResX = WXImage_GetDotsPerMeterX(dib);
                    int ResY = WXImage_GetDotsPerMeterX(dib);
                   
                    
                    void* handle = HandlerCreate();
                    int ret = CompressQuality_BufferToBuffer(image_data, image_size,
                        handle, WXIMAGE_TYPE_WEBP, nQuality, nDstW, nDstH);
                    
                    NSLog(@"laod image using WXImage OK!!  Ret=%d",ret);
                    
                    if(ret >= 0){
                        int data_size = HandlerGetSize(handle);
                        uint8_t * tmp = (uint8_t*)malloc(data_size);
                        HandlerGetData(handle, tmp);
                        
                        FILE* fout = fopen([strOutPut UTF8String], "wb");
                        if(fout){
                            
                            fwrite(tmp, data_size, 1, fout);
                            fclose(fout);
                            
                            NSString* strInfo = [[NSString alloc] initWithFormat:@"Orgi %@ [%d,%d,%d,%d]\n Res=[%dx%d]  TargetSize[%d]=%d",
                                                 strOutPut, width, height, channel,Pitch, ResX, ResY,  nQuality, data_size];
                            dispatch_async(dispatch_get_main_queue(), ^{
                                　  [self.m_log setStringValue:strInfo];
                            });
                        }else{
                            NSLog(@"Fopen %@ Error",strOutPut);
                        }
                        free(tmp);
                    }
                    HandlerDestroy(handle);
                }
                free(image_data);
            }else{
                NSLog(@"Read Size ERROR");
            }
        }
    });
}

- (IBAction)onQualityPng:(id)sender {
    NSString* strInput = [m_txtInput stringValue];
    NSString* strOutPut = [m_txtOutput stringValue];
    NSString* strQuality = [m_txtQuality stringValue]; //UI 线程
    int nQuality = [strQuality intValue];
    
    int nDstW = [[m_txtDstWidth stringValue] intValue];
    int nDstH = [[m_txtDstHeight stringValue] intValue];
    
    dispatch_queue_t queue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
    dispatch_async(queue, ^{
        
        if([strInput length] > 0){
            
            NSLog(@"Quality = %d", nQuality);
            
            uint8_t *image_data = NULL;
            int image_size = 0;
            WXReadFile([strInput UTF8String], &image_data, &image_size);
            if(image_data){
                
                NSLog(@"Read Size = %d!!",image_size);
                
                void* dib = WXImage_Load(image_data, image_size);//从内存数据获得 Bitmap对象
                
                if(NULL != dib){
                    NSLog(@"laod image using WXImage OK!!");
                    //提取出原始数据用于图像处理
                    int width  =  WXImage_GetWidth(dib);//返回分辨率宽度
                    int height =  WXImage_GetHeight(dib);//返回分辨率高度
                    int channel = WXImage_GetChannel(dib);//RGB Type
                    int Pitch  = WXImage_GetPitch(dib);//返回每行数据量
                    int ResX = WXImage_GetDotsPerMeterX(dib);
                    int ResY = WXImage_GetDotsPerMeterX(dib);
                   
                    
                    void* handle = HandlerCreate();
                    int ret = CompressQuality_BufferToBuffer(image_data, image_size,
                        handle, WXIMAGE_TYPE_PNG, nQuality, nDstW, nDstH);
                    
                    NSLog(@"laod image using WXImage OK!!  Ret=%d",ret);
                    
                    if(ret >= 0){
                        int data_size = HandlerGetSize(handle);
                        uint8_t * tmp = (uint8_t*)malloc(data_size);
                        HandlerGetData(handle, tmp);
                        
                        FILE* fout = fopen([strOutPut UTF8String], "wb");
                        if(fout){
                            
                            fwrite(tmp, data_size, 1, fout);
                            fclose(fout);
                            
                            NSString* strInfo = [[NSString alloc] initWithFormat:@"Orgi %@ [%d,%d,%d,%d]\n Res=[%dx%d]  TargetSize[%d]=%d",
                                                 strOutPut, width, height, channel,Pitch, ResX, ResY,  nQuality, data_size];
                            dispatch_async(dispatch_get_main_queue(), ^{
                                　  [self.m_log setStringValue:strInfo];
                            });
                        }else{
                            NSLog(@"Fopen %@ Error",strOutPut);
                        }
                        free(tmp);
                    }
                    HandlerDestroy(handle);
                }
                free(image_data);
            }else{
                NSLog(@"Read Size ERROR");
            }
        }
    });
}

- (IBAction)onQualityJpeg:(id)sender {
    
    NSString* strInput = [m_txtInput stringValue];
    NSString* strOutPut = [m_txtOutput stringValue];
    NSString* strQuality = [m_txtQuality stringValue]; //UI 线程
    int nQuality = [strQuality intValue];
    
    int nDstW = [[m_txtDstWidth stringValue] intValue];
    int nDstH = [[m_txtDstHeight stringValue] intValue];
    
    dispatch_queue_t queue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
    dispatch_async(queue, ^{
        
        if([strInput length] > 0){
            
            NSLog(@"Quality = %d", nQuality);
            
            uint8_t *image_data = NULL;
            int image_size = 0;
            WXReadFile([strInput UTF8String], &image_data, &image_size);
            if(image_data){
                
                NSLog(@"Read Size = %d!!",image_size);
                
                void* dib = WXImage_Load(image_data, image_size);//从内存数据获得 Bitmap对象
                
                if(NULL != dib){
                    NSLog(@"laod image using WXImage OK!!");
                    //提取出原始数据用于图像处理
                    int width  =  WXImage_GetWidth(dib);//返回分辨率宽度
                    int height =  WXImage_GetHeight(dib);//返回分辨率高度
                    int channel = WXImage_GetChannel(dib);//RGB Type
                    int Pitch  = WXImage_GetPitch(dib);//返回每行数据量
                    int ResX = WXImage_GetDotsPerMeterX(dib);
                    int ResY = WXImage_GetDotsPerMeterX(dib);
                   
                    
                    void* handle = HandlerCreate();
                    int ret = CompressQuality_BufferToBuffer(image_data, image_size,
                        handle, WXIMAGE_TYPE_JPEG, nQuality, nDstW, nDstH);
                    
                    NSLog(@"laod image using WXImage OK!!  Ret=%d",ret);
                    
                    if(ret >= 0){
                        int data_size = HandlerGetSize(handle);
                        uint8_t * tmp = (uint8_t*)malloc(data_size);
                        HandlerGetData(handle, tmp);
                        
                        FILE* fout = fopen([strOutPut UTF8String], "wb");
                        if(fout){
                            
                            fwrite(tmp, data_size, 1, fout);
                            fclose(fout);
                            
                            NSString* strInfo = [[NSString alloc] initWithFormat:@"Orgi %@ [%d,%d,%d,%d]\n Res=[%dx%d]  TargetSize[%d]=%d",
                                                 strOutPut, width, height, channel,Pitch, ResX, ResY,  nQuality, data_size];
                            dispatch_async(dispatch_get_main_queue(), ^{
                                　  [self.m_log setStringValue:strInfo];
                            });
                        }else{
                            NSLog(@"Fopen %@ Error",strOutPut);
                        }
                        free(tmp);
                    }
                    HandlerDestroy(handle);
                }
                free(image_data);
            }else{
                NSLog(@"Read Size ERROR");
            }
        }
    });
}

- (IBAction)onQualityOrig:(id)sender {

    NSString* strInput = [m_txtInput stringValue];
    NSString* strOutPut = [m_txtOutput stringValue];
    NSString* strQuality = [m_txtQuality stringValue]; //UI 线程
    int nQuality = [strQuality intValue];
    
    int nDstW = [[m_txtDstWidth stringValue] intValue];
    int nDstH = [[m_txtDstHeight stringValue] intValue];
    
    dispatch_queue_t queue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
    dispatch_async(queue, ^{
        
        if([strInput length] > 0){
            
            NSLog(@"Quality = %d", nQuality);
            
            uint8_t *image_data = NULL;
            int image_size = 0;
            WXReadFile([strInput UTF8String], &image_data, &image_size);
            if(image_data){
                
                NSLog(@"Read Size = %d!!",image_size);
                
                void* dib = WXImage_Load(image_data, image_size);//从内存数据获得 Bitmap对象
                
                if(NULL != dib){
                    NSLog(@"laod image using WXImage OK!!");
                    //提取出原始数据用于图像处理
                    int width  =  WXImage_GetWidth(dib);//返回分辨率宽度
                    int height =  WXImage_GetHeight(dib);//返回分辨率高度
                    int channel = WXImage_GetChannel(dib);//RGB Type
                    int Pitch  = WXImage_GetPitch(dib);//返回每行数据量
                    int ResX = WXImage_GetDotsPerMeterX(dib);
                    int ResY = WXImage_GetDotsPerMeterX(dib);
                   
                    
                    void* handle = HandlerCreate();
                    int ret = CompressQuality_BufferToBuffer(image_data, image_size,
                        handle, WXIMAGE_TYPE_ORIGINAL, nQuality, nDstW, nDstH);
                    
                    NSLog(@"laod image using WXImage OK!!  Ret=%d",ret);
                    
                    if(ret >= 0){
                        int data_size = HandlerGetSize(handle);
                        uint8_t * tmp = (uint8_t*)malloc(data_size);
                        HandlerGetData(handle, tmp);
                        
                        FILE* fout = fopen([strOutPut UTF8String], "wb");
                        if(fout){
                            
                            fwrite(tmp, data_size, 1, fout);
                            fclose(fout);
                            
                            NSString* strInfo = [[NSString alloc] initWithFormat:@"Orgi %@ [%d,%d,%d,%d]\n Res=[%dx%d]  TargetSize[%d]=%d",
                                                 strOutPut, width, height, channel,Pitch, ResX, ResY,  nQuality, data_size];
                            dispatch_async(dispatch_get_main_queue(), ^{
                                　  [self.m_log setStringValue:strInfo];
                            });
                        }else{
                            NSLog(@"Fopen %@ Error",strOutPut);
                        }
                        free(tmp);
                    }
                    HandlerDestroy(handle);
                }
                free(image_data);
            }else{
                NSLog(@"Read Size ERROR");
            }
        }
    });
    
}

- (IBAction)onSizeOrig:(id)sender {
    NSLog(@"%s", __FUNCTION__);
    
    NSString* strInput = [m_txtInput stringValue];
    NSString* strOutPut = [m_txtOutput stringValue];
    NSString* strSize = [m_txtSize stringValue]; //UI 线程
    int nSize = [strSize intValue];
    
    int nDstW = [[m_txtDstWidth stringValue] intValue];
    int nDstH = [[m_txtDstHeight stringValue] intValue];
    
    dispatch_queue_t queue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
    dispatch_async(queue, ^{
        
        if([strInput length] > 0){
            
            NSLog(@"TargetSize = %d", nSize);
            
            uint8_t *image_data = NULL;
            int image_size = 0;
            WXReadFile([strInput UTF8String], &image_data, &image_size);
            if(image_data){
                
                NSLog(@"Read Size = %d!!",image_size);
                
                void* dib = WXImage_Load(image_data, image_size);//从内存数据获得 Bitmap对象
                
                if(NULL != dib){
                    NSLog(@"laod image using WXImage OK!!");
                    //提取出原始数据用于图像处理
                    int width  =  WXImage_GetWidth(dib);//返回分辨率宽度
                    int height =  WXImage_GetHeight(dib);//返回分辨率高度
                    int channel = WXImage_GetChannel(dib);//RGB Type
                    int Pitch  = WXImage_GetPitch(dib);//返回每行数据量
                    int ResX = WXImage_GetDotsPerMeterX(dib);
                    int ResY = WXImage_GetDotsPerMeterX(dib);
                   
                    
                    void* handle = HandlerCreate();
                    int ret = CompressSize_BufferToBuffer(image_data, image_size,
                        handle, WXIMAGE_TYPE_ORIGINAL, nSize, nDstW, nDstH);
                    
                    NSLog(@"laod image using WXImage OK!!  CompressSize_BufferToBuffer Ret=%d",ret);
                    
                    if(ret >= 0){
                        int data_size = HandlerGetSize(handle);
                        uint8_t * tmp = (uint8_t*)malloc(data_size);
                        HandlerGetData(handle, tmp);
                        
                        FILE* fout = fopen([strOutPut UTF8String], "wb");
                        if(fout){
                            
                            fwrite(tmp, data_size, 1, fout);
                            fclose(fout);
                            
                            NSString* strInfo = [[NSString alloc] initWithFormat:@"Orgi %@ [%d,%d,%d,%d]\n Res=[%dx%d]  TargetSize[%d]=%d",
                                                 strOutPut, width, height, channel,Pitch, ResX, ResY,  nSize, data_size];
                            dispatch_async(dispatch_get_main_queue(), ^{
                                　  [self.m_log setStringValue:strInfo];
                            });
                        }else{
                            NSLog(@"Fopen %@ Error",strOutPut);
                        }
                        free(tmp);
                    }
                    HandlerDestroy(handle);
                }
                free(image_data);
            }else{
                NSLog(@"Read Size ERROR");
            }
        }
    });
    
    
}

- (IBAction)onSizeWebp:(id)sender {
    NSLog(@"%s", __FUNCTION__);
    
    NSLog(@"%s", __FUNCTION__);
    
    NSString* strInput = [m_txtInput stringValue];
    NSString* strOutPut = [m_txtOutput stringValue];
    NSString* strSize = [m_txtSize stringValue]; //UI 线程
    int nSize = [strSize intValue];
    
    int nDstW = [[m_txtDstWidth stringValue] intValue];
    int nDstH = [[m_txtDstHeight stringValue] intValue];
    
    dispatch_queue_t queue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
    dispatch_async(queue, ^{
        
        if([strInput length] > 0){
            
            NSLog(@"TargetSize = %d", nSize);
            
            uint8_t *image_data = NULL;
            int image_size = 0;
            WXReadFile([strInput UTF8String], &image_data, &image_size);
            if(image_data){
                
                NSLog(@"Read Size = %d!!",image_size);
                
                void* dib = WXImage_Load(image_data, image_size);//从内存数据获得 Bitmap对象
                
                if(NULL != dib){
                    NSLog(@"laod image using WXImage OK!!");
                    //提取出原始数据用于图像处理
                    int width  =  WXImage_GetWidth(dib);//返回分辨率宽度
                    int height =  WXImage_GetHeight(dib);//返回分辨率高度
                    int channel = WXImage_GetChannel(dib);//RGB Type
                    int Pitch  = WXImage_GetPitch(dib);//返回每行数据量
                    int ResX = WXImage_GetDotsPerMeterX(dib);
                    int ResY = WXImage_GetDotsPerMeterX(dib);
                   
                    
                    void* handle = HandlerCreate();
                    int ret = CompressSize_BufferToBuffer(image_data, image_size,
                        handle, WXIMAGE_TYPE_WEBP, nSize, nDstW, nDstH);
                    
                    NSLog(@"laod image using WXImage OK!!  CompressSize_BufferToBuffer Ret=%d",ret);
                    
                    if(ret >= 0){
                        int data_size = HandlerGetSize(handle);
                        uint8_t * tmp = (uint8_t*)malloc(data_size);
                        HandlerGetData(handle, tmp);
                        
                        FILE* fout = fopen([strOutPut UTF8String], "wb");
                        if(fout){
                            
                            fwrite(tmp, data_size, 1, fout);
                            fclose(fout);
                            
                            NSString* strInfo = [[NSString alloc] initWithFormat:@"Orgi %@ [%d,%d,%d,%d]\n Res=[%dx%d]  TargetSize[%d]=%d",
                                                 strOutPut, width, height, channel,Pitch, ResX, ResY,  nSize, data_size];
                            dispatch_async(dispatch_get_main_queue(), ^{
                                　  [self.m_log setStringValue:strInfo];
                            });
                        }else{
                            NSLog(@"Fopen %@ Error",strOutPut);
                        }
                        free(tmp);
                    }
                    HandlerDestroy(handle);
                }
                free(image_data);
            }else{
                NSLog(@"Read Size ERROR");
            }
        }
    });
    
}


- (IBAction)onSizeJpg:(id)sender {
    NSLog(@"%s", __FUNCTION__);
    
    
    NSLog(@"%s", __FUNCTION__);
    
    NSString* strInput = [m_txtInput stringValue];
    NSString* strOutPut = [m_txtOutput stringValue];
    NSString* strSize = [m_txtSize stringValue]; //UI 线程
    int nSize = [strSize intValue];
    
    int nDstW = [[m_txtDstWidth stringValue] intValue];
    int nDstH = [[m_txtDstHeight stringValue] intValue];
    
    dispatch_queue_t queue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
    dispatch_async(queue, ^{
        
        if([strInput length] > 0){
            
            NSLog(@"TargetSize = %d", nSize);
            
            uint8_t *image_data = NULL;
            int image_size = 0;
            WXReadFile([strInput UTF8String], &image_data, &image_size);
            if(image_data){
                
                NSLog(@"Read Size = %d!!",image_size);
                
                void* dib = WXImage_Load(image_data, image_size);//从内存数据获得 Bitmap对象
                
                if(NULL != dib){
                    NSLog(@"laod image using WXImage OK!!");
                    //提取出原始数据用于图像处理
                    int width  =  WXImage_GetWidth(dib);//返回分辨率宽度
                    int height =  WXImage_GetHeight(dib);//返回分辨率高度
                    int channel = WXImage_GetChannel(dib);//RGB Type
                    int Pitch  = WXImage_GetPitch(dib);//返回每行数据量
                    int ResX = WXImage_GetDotsPerMeterX(dib);
                    int ResY = WXImage_GetDotsPerMeterX(dib);
                   
                    
                    void* handle = HandlerCreate();
                    int ret = CompressSize_BufferToBuffer(image_data, image_size,
                        handle, WXIMAGE_TYPE_JPEG, nSize, nDstW, nDstH);
                    
                    NSLog(@"laod image using WXImage OK!!  CompressSize_BufferToBuffer Ret=%d",ret);
                    
                    if(ret >= 0){
                        int data_size = HandlerGetSize(handle);
                        uint8_t * tmp = (uint8_t*)malloc(data_size);
                        HandlerGetData(handle, tmp);
                        
                        FILE* fout = fopen([strOutPut UTF8String], "wb");
                        if(fout){
                            
                            fwrite(tmp, data_size, 1, fout);
                            fclose(fout);
                            
                            NSString* strInfo = [[NSString alloc] initWithFormat:@"Orgi %@ [%d,%d,%d,%d]\n Res=[%dx%d]  TargetSize[%d]=%d",
                                                 strOutPut, width, height, channel,Pitch, ResX, ResY,  nSize, data_size];
                            dispatch_async(dispatch_get_main_queue(), ^{
                                　  [self.m_log setStringValue:strInfo];
                            });
                        }else{
                            NSLog(@"Fopen %@ Error",strOutPut);
                        }
                        free(tmp);
                    }
                    HandlerDestroy(handle);
                }
                free(image_data);
            }else{
                NSLog(@"Read Size ERROR");
            }
        }
    });

}





@end
