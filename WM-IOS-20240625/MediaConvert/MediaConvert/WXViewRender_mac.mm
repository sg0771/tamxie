#if !TARGET_OS_IPHONE

#include "WXBase.h"
#include "WXVideoRender.h"
#import  <Foundation/Foundation.h>
#import  <AVFoundation/AVFoundation.h>
#import  <QuartzCore/QuartzCore.h>
#include <sys/time.h>

#import  <dispatch/queue.h>
#import  <Cocoa/Cocoa.h>
#import  <AppKit/AppKit.h>

#import  <OpenGL/OpenGL.h>
#import  <OpenGL/gl.h>
#include <libyuv.h>

//MacRender Using OpenGLView
class WXVideoRender {
public:
    NSView *m_view = nil;
    NSOpenGLView  *m_viewImpl = nil;
    
    WXLocker m_mutex;
    int m_bOpen = 0;
    
    int m_iWidth = 0;
    int m_iHeight = 0;
    uint8_t *m_pBuf= nullptr;
    GLuint m_texture = 0;
    double m_fRed   = 0.0;
    double m_fGreen = 0.0;
    double m_fBlue  = 0.0;
    double m_fAlpha = 0.0;
    int m_bFixed = 1;
    
    WXFfmpegOnVideoData m_cbData = nullptr;
private:
    void InitImpl(HWND hwnd, int width, int height, WXFfmpegOnVideoData cbData){
        dispatch_async(dispatch_get_main_queue(), ^{
            m_view = (__bridge NSView*)hwnd;
            NSLog(@"NSView ----- %dx%d",(int)m_view.frame.size.width,(int)m_view.frame.size.height);
            m_iWidth = width;
            m_iHeight = height;
            m_cbData = cbData;
            
            m_viewImpl = [[NSOpenGLView alloc] init];
            [m_view addSubview:m_viewImpl];
            m_pBuf = new uint8_t[m_iWidth* m_iHeight*2];
            
            m_bOpen = 1;
            printf(" ------ MacOpenGLViewRender %s Open OK  [%dx%d]!!!\n", __FUNCTION__, m_iWidth, m_iHeight);
        });
    }
    
    void OnDraw(){
        dispatch_async(dispatch_get_main_queue(), ^{
            m_viewImpl.frame = NSRect{0,0,m_view.frame.size.width,m_view.frame.size.height};
            [[m_viewImpl openGLContext] makeCurrentContext];
            if(m_texture == 0){
                glGenTextures(1, &m_texture);
                if(m_texture){
                    glEnable(GL_TEXTURE_2D);
                    glBindTexture(GL_TEXTURE_2D, m_texture);
                }
                printf("MacOpenGLViewRender %s glGenTextures\n",__FUNCTION__);
            }
            
            glClearColor(m_fRed, m_fGreen, m_fBlue, m_fAlpha);
            glClear(GL_COLOR_BUFFER_BIT);
            
            glColor3f(0.0f, 1.0f, 0.0f);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_iWidth, m_iHeight, 0,
                         GL_YCBCR_422_APPLE, GL_UNSIGNED_SHORT_8_8_REV_APPLE, m_pBuf);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
            
            glBegin(GL_QUADS);
            
            CGFloat desw = 1.0;
            CGFloat desh = 1.0;
            if(m_bFixed){
                CGFloat sw = (int)m_view.frame.size.width;
                CGFloat sh = (int)m_view.frame.size.height;
                desw = sw;
                desh = sh;
                CGFloat sw1 = sh * m_iWidth / m_iHeight;
                CGFloat sh1 = sw * m_iHeight / m_iWidth;
                if (sw1 <= sw) {
                    desw = sw1;
                }else {
                    desh = sh1;
                }
                desw /= sw1;
                desh /= sh1;
            }
            glTexCoord2f(0.0f, 0.0f);  glVertex3f(-desh,   desw, 0.0f);
            glTexCoord2f(0.0f, 1.0f);  glVertex3f(-desh,  -desw, 0.0f);
            glTexCoord2f(1.0f, 1.0f);  glVertex3f(desh, -desw, 0.0f);
            glTexCoord2f(1.0f, 0.0f);  glVertex3f(desh,  desw, 0.0f);
            glEnd();
            glFlush();
        });
    }
    
    void CloseImpl(){
        if(m_bOpen){
            if(m_texture){
                glDeleteTextures(1, &m_texture);
                m_texture = 0;
                printf("MacOpenGLViewRender %s glDeleteTextures\n",__FUNCTION__);
            }
            if(nil != m_viewImpl){
                [[m_viewImpl openGLContext] makeCurrentContext];
                m_viewImpl = nil;
            }
            m_iWidth = 0;
            m_iHeight = 0;
           printf("-----Tam %s OK!!!\n", __FUNCTION__);
            m_bOpen = 0;
        }
    }
    void OnClose(){
        dispatch_async(dispatch_get_main_queue(), ^{
            CloseImpl();
         });//UI线程
    }
public:
    
    virtual int Init(void* hwnd, int width, int height, WXFfmpegOnVideoData cbData) {
        WXAutoLock al(m_mutex);
        if(hwnd && width && height){
            InitImpl(hwnd, width, height,cbData);
            return 1;
        }
        return 0;
    }
    
    virtual int Display(uint8_t** data, int* linesize) {
        WXAutoLock al(m_mutex);
        if(!m_bOpen)
            return 0;
        libyuv::I420ToYUY2(data[0],  linesize[0],
                           data[1],  linesize[1],
                           data[2],  linesize[2],
                          m_pBuf, m_iWidth*2,
                          m_iWidth, m_iHeight
                           );
        m_bFixed = 1;
        OnDraw();
        return 1;
    }
    
    virtual void Stop() {
        WXAutoLock al(m_mutex);
        if(strcmp(dispatch_queue_get_label(DISPATCH_CURRENT_QUEUE_LABEL), dispatch_queue_get_label(dispatch_get_main_queue())) == 0) {
            // do something in main thread
            printf("MacOpenGLViewRender %s wait in Main thread\n", __FUNCTION__);
            CloseImpl();
        } else {
            // do something in other thread
            printf("MacOpenGLViewRender %s wait in Other thread\n", __FUNCTION__);
            OnClose();
        }
    }
};


///Create Destroy API

extern "C" void* WXVideoRenderCreate(void* hwnd, int width, int height, WXFfmpegOnVideoData cbData){
    WXVideoRender *p = new WXVideoRender;
    p->Init(hwnd, width, height, cbData);
    return (void*)p;
}

//显示解码的YUV420数据
extern "C" void WXVideoRenderDisplay(void* p, uint8_t **data, int* linesize)//YUV420P AVFrame
{
    WXVideoRender *impl = (WXVideoRender*)p;
    impl->Display(data, linesize);
}

extern "C" void WXVideoRenderDestroy(void* p){
    NSLog(@"WXVideoRenderDestroy AAAA");
    WXVideoRender *impl = (WXVideoRender*)p;
    impl->Stop();
    delete impl;
    NSLog(@"WXVideoRenderDestroy BBBB");
}


#endif //OSX
