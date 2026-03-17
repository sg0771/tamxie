//
//  OpenGLView.m
//  MyTest
//
//  Created by smy on 12/20/11.
//  Copyright (c) 2011 ZY.SYM. All rights reserved.
//

#include "MSVideo.h"
#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#import <UIKit/UIKit.h>

#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#import <OpenGLES/EAGL.h>
#include <sys/time.h>

#import <dispatch/queue.h>

@interface OpenGLView20 : UIView{
    EAGLContext *m_glContext;
    GLuint m_framebuffer;
    GLuint m_renderBuffer;
    GLuint m_program;
    GLuint m_textureYUV[3];
    
    int m_iVideoWidth;
    int m_iVideoHeight;
    int m_iViewScale;
    int m_iWinWidth;
    int m_iWinHeight;
    int m_dx1;
    int m_dy1;
    int m_dx2;
    int m_dy2;
}
#pragma mark - 接口

- (void)setVideoSize:(GLuint)width height:(GLuint)height;
- (void)Draw: (uint8_t *)data;
- (void)Draw2:(uint8_t *)data;
- (void)Clear;
@end

enum AttribEnum{
    ATTRIB_VERTEX,
    ATTRIB_TEXTURE,
    ATTRIB_COLOR,
};

enum TextureType{
    TEXY = 0,
    TEXU,
    TEXV,
    TEXC
};

@interface OpenGLView20()
- (BOOL)doInit;
- (void)setupYUVTexture;
- (BOOL)createFrameAndRenderBuffer;
- (void)destoryFrameAndRenderBuffer;
- (void)loadShader;
- (GLuint)compileShader:(NSString*)shaderCode withType:(GLenum)shaderType;
- (void)render;
- (void)render2;
@end


//留黑模式
static void  xProcess(int src_w, int src_h, int dst_w, int dst_h, int *dw, int *dh) {
    *dw = 0; *dh = 0;
    if ((dst_w * src_h / src_w) > dst_h) {
        *dw = ((dst_w - dst_h * src_w / src_h) + 3) / 4 * 2;
    }else {
        *dh = ((dst_h - dst_w * src_h / src_w) + 3) / 4 * 2;
    }
}

//居中填充模式
static void  xProcess2(int src_w, int src_h, int dst_w, int dst_h, int *dw, int *dh) {
    *dw = 0; *dh = 0;
    if ((dst_w * src_h / src_w) < dst_h) {
        *dw = ((dst_w - dst_h * src_w / src_h) + 3) / 4 * 2;
    }else {
        *dh = ((dst_h - dst_w * src_h / src_w) + 3) / 4 * 2;
    }
}

@implementation OpenGLView20

- (BOOL)doInit{
    m_iVideoWidth = 0;
    m_iVideoHeight = 0;
    m_iWinWidth = 0;
    m_iWinHeight= 0;
    m_dx1 = m_dy1 = m_dx2 = m_dy2 = 0;

    CAEAGLLayer *eaglLayer = (CAEAGLLayer*) self.layer;
    eaglLayer.opaque = YES;
    eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                    [NSNumber numberWithBool:NO], kEAGLDrawablePropertyRetainedBacking,
                                    kEAGLColorFormatRGB565, kEAGLDrawablePropertyColorFormat,
                                    nil];
    self.contentScaleFactor = [UIScreen mainScreen].scale;
    m_iViewScale = [UIScreen mainScreen].scale;
    m_glContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    if(!m_glContext || ![EAGLContext setCurrentContext:m_glContext]){
        return NO;
    }
    [self setupYUVTexture];
    [self loadShader];
    glUseProgram(m_program);
    GLuint textureUniformY = glGetUniformLocation(m_program, "SamplerY");
    GLuint textureUniformU = glGetUniformLocation(m_program, "SamplerU");
    GLuint textureUniformV = glGetUniformLocation(m_program, "SamplerV");
    glUniform1i(textureUniformY, 0);
    glUniform1i(textureUniformU, 1);
    glUniform1i(textureUniformV, 2);
    
    m_iWinWidth  = self.bounds.size.width * m_iViewScale;
    m_iWinHeight = self.bounds.size.height * m_iViewScale;
    
    return YES;
}

- (id)initWithCoder:(NSCoder *)aDecoder{
    self = [super initWithCoder:aDecoder];
    if (self){
        if (![self doInit]){
            self = nil;
        }
    }
    return self;
}

- (id)initWithFrame:(CGRect)frame{
    self = [super initWithFrame:frame];
    if (self){
        if (![self doInit]){
            self = nil;
        }
    }
    return self;
}

- (void)layoutSubviews{
    
    @synchronized(self)
    {
        [EAGLContext setCurrentContext:m_glContext];
        [self destoryFrameAndRenderBuffer];
        [self createFrameAndRenderBuffer];
    }
        
    glViewport(1, 1, self.bounds.size.width*m_iViewScale - 2, self.bounds.size.height*m_iViewScale - 2);
  
}

- (void)setupYUVTexture{
    if (m_textureYUV[TEXY]){
        glDeleteTextures(3, m_textureYUV);
    }
    glGenTextures(3, m_textureYUV);
    if (!m_textureYUV[TEXY] || !m_textureYUV[TEXU] || !m_textureYUV[TEXV]){
        NSLog(@"<<<<<<<<<<<<纹理创建失败!>>>>>>>>>>>>");
        return;
    }
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_textureYUV[TEXY]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_textureYUV[TEXU]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_textureYUV[TEXV]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

- (void)render{
    [EAGLContext setCurrentContext:m_glContext];
    glViewport(m_dx1, m_dy1, m_iWinWidth - 2 * m_dx1, m_iWinHeight - 2 * m_dy1);
    static const GLfloat squareVertices[] = {
        -1.0f, -1.0f, 1.0f, -1.0f,
        -1.0f,  1.0f, 1.0f,  1.0f,
    };
    
    static const GLfloat coordVertices[] = {
        0.0f,  1.0f, 1.0f,  1.0f,
        0.0f,  0.0f, 1.0f,  0.0f,
    };
    // Update attribute values
    glVertexAttribPointer(ATTRIB_VERTEX, 2, GL_FLOAT, 0, 0, squareVertices);
    glEnableVertexAttribArray(ATTRIB_VERTEX);
    glVertexAttribPointer(ATTRIB_TEXTURE, 2, GL_FLOAT, 0, 0, coordVertices);
    glEnableVertexAttribArray(ATTRIB_TEXTURE);
    // Draw
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindRenderbuffer(GL_RENDERBUFFER, m_renderBuffer);
    [m_glContext presentRenderbuffer:GL_RENDERBUFFER];
}

- (void)render2{
    [EAGLContext setCurrentContext:m_glContext];
    glViewport(m_dx2, m_dy2, m_iWinWidth - 2 * m_dx2, m_iWinHeight - 2 * m_dy2);
    static const GLfloat squareVertices[] = {
        -1.0f, -1.0f, 1.0f, -1.0f,
        -1.0f,  1.0f, 1.0f,  1.0f,
    };
    
    static const GLfloat coordVertices[] = {
        0.0f,  1.0f, 1.0f,  1.0f,
        0.0f,  0.0f, 1.0f,  0.0f,
    };
    // Update attribute values
    glVertexAttribPointer(ATTRIB_VERTEX, 2, GL_FLOAT, 0, 0, squareVertices);
    glEnableVertexAttribArray(ATTRIB_VERTEX);
    glVertexAttribPointer(ATTRIB_TEXTURE, 2, GL_FLOAT, 0, 0, coordVertices);
    glEnableVertexAttribArray(ATTRIB_TEXTURE);
    // Draw
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindRenderbuffer(GL_RENDERBUFFER, m_renderBuffer);
    [m_glContext presentRenderbuffer:GL_RENDERBUFFER];
}

#pragma mark - 设置openGL
+ (Class)layerClass{
    return [CAEAGLLayer class];
}

- (BOOL)createFrameAndRenderBuffer{
    glGenFramebuffers(1, &m_framebuffer);
    glGenRenderbuffers(1, &m_renderBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_renderBuffer);
    if (![m_glContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer *)self.layer]){
        NSLog(@"attach渲染缓冲区失败");
    }
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_renderBuffer);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        NSLog(@"创建缓冲区错误 0x%x", glCheckFramebufferStatus(GL_FRAMEBUFFER));
        return NO;
    }
    return YES;
}

- (void)destoryFrameAndRenderBuffer{
    if (m_framebuffer){
        glDeleteFramebuffers(1, &m_framebuffer);
        m_framebuffer = 0;
    }
    
    if (m_renderBuffer){
        glDeleteRenderbuffers(1, &m_renderBuffer);
        m_renderBuffer = 0;
    }
}

#define FSH @"varying lowp vec2 TexCoordOut;\
\
uniform sampler2D SamplerY;\
uniform sampler2D SamplerU;\
uniform sampler2D SamplerV;\
\
void main(void)\
{\
mediump vec3 yuv;\
lowp vec3 rgb;\
\
yuv.x = texture2D(SamplerY, TexCoordOut).r;\
yuv.y = texture2D(SamplerU, TexCoordOut).r - 0.5;\
yuv.z = texture2D(SamplerV, TexCoordOut).r - 0.5;\
\
rgb = mat3( 1,       1,         1,\
0,       -0.39465,  2.03211,\
1.13983, -0.58060,  0) * yuv;\
\
gl_FragColor = vec4(rgb, 1);\
\
}"

#define VSH @"attribute vec4 position;\
attribute vec2 TexCoordIn;\
varying vec2 TexCoordOut;\
\
void main(void)\
{\
gl_Position = position;\
TexCoordOut = TexCoordIn;\
}"

/**
 加载着色器
 */
- (void)loadShader{
    GLuint vertexShader = [self compileShader:VSH withType:GL_VERTEX_SHADER];
    GLuint fragmentShader = [self compileShader:FSH withType:GL_FRAGMENT_SHADER];
    m_program = glCreateProgram();
    glAttachShader(m_program, vertexShader);
    glAttachShader(m_program, fragmentShader);
    glBindAttribLocation(m_program, ATTRIB_VERTEX, "position");
    glBindAttribLocation(m_program, ATTRIB_TEXTURE, "TexCoordIn");
    
    glLinkProgram(m_program);
    GLint linkSuccess;
    glGetProgramiv(m_program, GL_LINK_STATUS, &linkSuccess);
    if (linkSuccess == GL_FALSE) {
        GLchar messages[256];
        glGetProgramInfoLog(m_program, sizeof(messages), 0, &messages[0]);
        NSString *messageString = [NSString stringWithUTF8String:messages];
        NSLog(@"<<<<着色器连接失败 %@>>>", messageString);
        //exit(1);
    }
    
    if (vertexShader)
        glDeleteShader(vertexShader);
    if (fragmentShader)
        glDeleteShader(fragmentShader);
}

- (GLuint)compileShader:(NSString*)shaderString withType:(GLenum)shaderType{
    NSError *error = nil;
    if (!shaderString) {
        NSLog(@"Error loading shader: %@", error.localizedDescription);
        exit(1);
    }
    else{
        //NSLog(@"shader code-->%@", shaderString);
    }
    GLuint shaderHandle = glCreateShader(shaderType);
    const char * shaderStringUTF8 = [shaderString UTF8String];
    int shaderStringLength = (int)[shaderString length];
    glShaderSource(shaderHandle, 1, &shaderStringUTF8, &shaderStringLength);
    glCompileShader(shaderHandle);
    GLint compileSuccess;
    glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &compileSuccess);
    if (compileSuccess == GL_FALSE) {
        GLchar messages[256];
        glGetShaderInfoLog(shaderHandle, sizeof(messages), 0, &messages[0]);
        NSString *messageString = [NSString stringWithUTF8String:messages];
        NSLog(@"%@", messageString);
        exit(1);
    }
    
    return shaderHandle;
}

#pragma mark - 接口
- (void)Draw:(uint8_t *)data{
    @synchronized(self){
        [EAGLContext setCurrentContext:m_glContext];
        glBindTexture(GL_TEXTURE_2D, m_textureYUV[TEXY]);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_iVideoWidth, m_iVideoHeight, GL_RED_EXT, GL_UNSIGNED_BYTE, data);
        glBindTexture(GL_TEXTURE_2D, m_textureYUV[TEXU]);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_iVideoWidth/2, m_iVideoHeight/2, GL_RED_EXT, GL_UNSIGNED_BYTE, data + m_iVideoWidth * m_iVideoHeight);
        glBindTexture(GL_TEXTURE_2D, m_textureYUV[TEXV]);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_iVideoWidth/2, m_iVideoHeight/2, GL_RED_EXT, GL_UNSIGNED_BYTE, data + m_iVideoWidth * m_iVideoHeight * 5 / 4);
        [self render];
    }
}

- (void)Draw2:(uint8_t *)data{
    @synchronized(self){
        [EAGLContext setCurrentContext:m_glContext];
        glBindTexture(GL_TEXTURE_2D, m_textureYUV[TEXY]);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_iVideoWidth, m_iVideoHeight, GL_RED_EXT, GL_UNSIGNED_BYTE, data);
        glBindTexture(GL_TEXTURE_2D, m_textureYUV[TEXU]);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_iVideoWidth/2, m_iVideoHeight/2, GL_RED_EXT, GL_UNSIGNED_BYTE, data + m_iVideoWidth * m_iVideoHeight);
        glBindTexture(GL_TEXTURE_2D, m_textureYUV[TEXV]);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_iVideoWidth/2, m_iVideoHeight/2, GL_RED_EXT, GL_UNSIGNED_BYTE, data + m_iVideoWidth * m_iVideoHeight * 5 / 4);
        [self render2];
    }
}

- (void)setVideoSize:(GLuint)width height:(GLuint)height{
    if(m_iVideoWidth == width && m_iVideoHeight == height)return;//init!!!
    m_iVideoWidth = width;
    m_iVideoHeight = height;
    
    uint8_t *blackData = (uint8_t*)malloc(width * height * 3/2);
    memset(blackData, 0x0, width * height * 3 / 2);
    
    [EAGLContext setCurrentContext:m_glContext];
    glBindTexture(GL_TEXTURE_2D, m_textureYUV[TEXY]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED_EXT, width, height, 0, GL_RED_EXT, GL_UNSIGNED_BYTE, blackData);
    glBindTexture(GL_TEXTURE_2D, m_textureYUV[TEXU]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED_EXT, width/2, height/2, 0, GL_RED_EXT, GL_UNSIGNED_BYTE, blackData + width * height);
    
    glBindTexture(GL_TEXTURE_2D, m_textureYUV[TEXV]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED_EXT, width/2, height/2, 0, GL_RED_EXT, GL_UNSIGNED_BYTE, blackData + width * height * 5 / 4);
    
    free(blackData);
           
    xProcess(m_iVideoWidth,m_iVideoHeight, m_iWinWidth, m_iWinHeight, &m_dx1, &m_dy1);
    xProcess2(m_iVideoWidth,m_iVideoHeight, m_iWinWidth, m_iWinHeight, &m_dx2, &m_dy2);
    NSLog(@"Black Mode Video = %dx%d\n",m_iVideoWidth, m_iVideoHeight);
    NSLog(@"Black Mode Win   = %dx%d\n",m_iWinWidth, m_iWinHeight);
    NSLog(@"Black Mode Left  = %dx%d\n",m_dx1, m_dy1);
    NSLog(@"Fill  Mode Left  = %dx%d\n",m_dx2, m_dy2);
}


- (void)Clear{
    if ([self window]){
        [EAGLContext setCurrentContext:m_glContext];
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
        glBindRenderbuffer(GL_RENDERBUFFER, m_renderBuffer);
        [m_glContext presentRenderbuffer:GL_RENDERBUFFER];
    }
}

@end

class MSDrawImpl: public IMSDraw{
private:
    UIView *m_view = nil;
    OpenGLView20 *m_render = nullptr;
    int m_width = 0;
    int m_height = 0;
public:
    void SetView(void *view){
        
        dispatch_async(dispatch_get_main_queue(), ^{
            NSLog(@"VideoRender Init Video Render!!!");
            m_view = (__bridge  UIView*)view;
            m_render = [[OpenGLView20 alloc]initWithFrame:CGRectMake(10, 10, m_view.frame.size.width - 20, m_view.frame.size.height - 20)];
            [m_view addSubview:m_render];

        });
    }
    
    void SetSize(int w, int h){
        m_width = w;
        m_height = h;
        dispatch_async(dispatch_get_main_queue(), ^{
            if(m_width && m_height){
                [m_render setVideoSize:m_width height:m_height];
            }
        });
    }
    void Draw(uint8_t*buf){
        dispatch_async(dispatch_get_main_queue(), ^{
            [m_render Draw:buf];        });
    }
    void Draw2(uint8_t*buf){
            dispatch_async(dispatch_get_main_queue(), ^{
                [m_render Draw2:buf];        });
    }
    void Clear(){
                dispatch_async(dispatch_get_main_queue(), ^{
                    [m_render Clear];        });
    }
};

///Create Destroy API
IMSDraw*  IMSDraw::Create(){
    MSDrawImpl *p = new MSDrawImpl;
    return (IMSDraw*)p;
}
void IMSDraw::Destroy(IMSDraw *p){
    MSDrawImpl *impl = (MSDrawImpl*)p;
    delete impl;
}
