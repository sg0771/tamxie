/*
MAC OpenGL
将YV12 转换为YUY2格式
使用OpenGL绘制
*/

extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include "libavutil/frame.h"
}

#include "IWXVideo.h"
#include "WXBase.h"
#include <libyuv.h>
#import  <Cocoa/Cocoa.h>
#import  <OpenGL/OpenGL.h>
#import  <OpenGL/gl.h>

class WXMACRender:public IWXVideoRender, public WXLocker{
    NSOpenGLView  *m_view = nil;
    uint8_t *m_pBuf = nullptr;
    int m_bOpen = 0;
    int m_iWidth = 0;
    int m_iHeight = 0;    
    GLuint m_texture = 0;    
    double m_fRed   = 0.0;
    double m_fGreen = 0.0;
    double m_fBlue  = 0.0;
    double m_fAlpha = 0.0;
public:

	virtual WXCTSTR GetType() {
		return _T("OPENGL");
	}

    void SetBgColor(double red, double green, double blue, double alpha){
		WXAutoLock al(this);
        m_fRed   = red;
        m_fGreen = green;
        m_fBlue  = blue;
        m_fAlpha = alpha;
    }
    
    void SetView(void *view){
		WXAutoLock al(this);
        m_view = (__bridge NSOpenGLView*)view;
    }
    void SetSize(int width, int height){
		WXAutoLock al(this);
		m_iWidth = width;
		m_iHeight = height;
		if (m_bOpen){
			Close();
			Open(); //重新打开
		}
    }

    int  isOpen(){
        return m_bOpen;
    }
    int  GetWidth(){
        return m_iWidth;
    }
    int  GetHeight(){
        return m_iHeight;
    }
    
    int  Open() {
        WXAutoLock al(this);
        if(!m_bOpen){
            m_pBuf = new uint8_t[m_iWidth * m_iHeight * 2];//YUY2 Data
            m_bOpen = 1;
        }
        return m_bOpen;
    }
	        
    
    void Display(AVFrame *frame) {
        WXAutoLock al(this);
        libyuv::I420ToYUY2(frame->data[0], frame->linesize[0],
                           frame->data[1], frame->linesize[1],
                           frame->data[2], frame->linesize[2],
                           m_pBuf + (m_iHeight - 1) * m_iWidth * 2, -m_iWidth * 2,
                           m_iWidth, m_iHeight);
        
        dispatch_async(dispatch_get_main_queue(), ^{
            [[m_view openGLContext] makeCurrentContext];
            
            if(m_texture == 0){
                glGenTextures(1, &m_texture);
                if(m_texture){
                    glEnable(GL_TEXTURE_2D);
                    glBindTexture(GL_TEXTURE_2D, m_texture);
                }
            }            
            
            glClearColor(m_fRed, m_fGreen, m_fBlue, m_fAlpha);
            glClear(GL_COLOR_BUFFER_BIT);
            
            glColor3f(0.0f, 1.0f, 0.0f);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_iWidth, m_iHeight, 0, GL_YCBCR_422_APPLE, GL_UNSIGNED_SHORT_8_8_REV_APPLE, m_pBuf);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
            
            glBegin(GL_QUADS);
            
            {                
                CGFloat sw = (int)m_view.frame.size.width;
                CGFloat sh = (int)m_view.frame.size.height;
                CGFloat desw = sw;
                CGFloat desh = sh;
                CGFloat sw1 = sh * m_iWidth / m_iHeight;
                CGFloat sh1 = sw * m_iHeight / m_iWidth;
                if (sw1 <= sw) {
                    desw = sw1;
                }else {
                    desh = sh1;
                }
                desw /= sw1;
                desh /= sh1;
                glTexCoord2f(1.0f, 1.0f);  glVertex3f(desh,   desw, 0.0f);
                glTexCoord2f(1.0f, 0.0f);  glVertex3f(desh,  -desw, 0.0f);
                glTexCoord2f(0.0f, 0.0f);  glVertex3f(-desh, -desw, 0.0f);
                glTexCoord2f(0.0f, 1.0f);  glVertex3f(-desh,  desw, 0.0f);
            }
            glEnd();
            glFlush();
        });
    }
    
    void Close(){
        WXAutoLock al(this);
        if(m_bOpen){
            m_bOpen = 0;
            if(m_pBuf){
                delete []m_pBuf;
                m_pBuf = nullptr;
            }
            m_iWidth = 0;
            m_iHeight = 0;
            m_view = nullptr;            
            dispatch_async(dispatch_get_main_queue(), ^{
                 [[m_view openGLContext] makeCurrentContext];
                if(m_texture){
                    glDeleteTextures(1, &m_texture);
                    m_texture = 0;
                }
            });
        }
    }
};

WXFFMPEG_CAPI IWXVideoRender *IWXVideoRenderCreate() {
	WXMACRender *render = new WXMACRender;
	return (WXMACRender*)render;
}

WXFFMPEG_CAPI void IWXVideoRenderDestroy(IWXVideoRender*p) {
	WXMACRender *render = (WXMACRender*)p;
	delete render;
}


WXFFMPEG_CAPI IWXVideoRender *IWXVideoRenderCreateByName(WXCTSTR strName, int async) {
	return IWXVideoRenderCreate();
}
