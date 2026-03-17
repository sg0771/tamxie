//
//  VideoDraw.m
//
//
//  Created by ftanx on 16/20/11.
//  Copyright (c) 2011 ZY.SYM. All rights reserved.
//

#import "MSVideoDraw.h"
#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#import <OpenGLES/EAGL.h>
#include <sys/time.h>


//留黑模式
static void  DrawProcess(int src_w, int src_h, int dst_w, int dst_h, int *dw, int *dh) {
    *dw = 0; *dh = 0;
    if ((dst_w * src_h / src_w) > dst_h) {
        *dw = ((dst_w - dst_h * src_w / src_h) + 3) / 4 * 2;
    }else {
        *dh = ((dst_h - dst_w * src_h / src_w) + 3) / 4 * 2;
    }
}

//居中填充模式
static void  DrawProcess2(int src_w, int src_h, int dst_w, int dst_h, int *dw, int *dh) {
    *dw = 0; *dh = 0;
    if ((dst_w * src_h / src_w) < dst_h) {
        *dw = ((dst_w - dst_h * src_w / src_h) + 3) / 4 * 2;
    }else {
        *dh = ((dst_h - dst_w * src_h / src_w) + 3) / 4 * 2;
    }
}

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

@interface MSVideoDraw(){
    EAGLContext             *m_glCtx;
    GLuint                  _framebuffer;
    GLuint                  _renderBuffer;
    GLuint                  _program;
    GLuint                  _textureYUV[3];
    GLsizei                 _viewScale;
    
    CAShapeLayer *m_shapeLayer;
    
    BOOL m_bOpen;
    int m_iWinWidth;
    int m_iWinHeight;
    int m_dx1;
    int m_dy1;
    int m_dx2;
    int m_dy2;
}
- (void)setupYUVTexture;
- (BOOL)createFrameAndRenderBuffer;
- (void)destoryFrameAndRenderBuffer;
- (void)loadShader;
- (GLuint)compileShader:(NSString*)shaderCode withType:(GLenum)shaderType;
@end

@implementation MSVideoDraw

@synthesize m_iVideoWidth;
@synthesize m_iVideoHeight;
@synthesize m_bShowFull;


// 判断View是否显示在屏幕上
- (BOOL)isDisplayedInScreen
{
    if (self == nil) {
        return FALSE;
    }
    
    CGRect screenRect = [UIScreen mainScreen].bounds;
    
    // 转换view对应window的Rect
    CGRect rect = [self convertRect:self.frame fromView:nil];
    if (CGRectIsEmpty(rect) || CGRectIsNull(rect)) {
        return FALSE;
    }
    
    // 若view 隐藏
    if (self.hidden) {
        return FALSE;
    }
    
    // 若没有superview
    if (self.superview == nil) {
        return FALSE;
    }
    
    // 若size为CGrectZero
    if (CGSizeEqualToSize(rect.size, CGSizeZero)) {
        return  FALSE;
    }
    
    // 获取 该view与window 交叉的 Rect
    CGRect intersectionRect = CGRectIntersection(rect, screenRect);
    if (CGRectIsEmpty(intersectionRect) || CGRectIsNull(intersectionRect)) {
        return FALSE;
    }
    
    return TRUE;
}


//初始化函数
- (BOOL)doInit{
    
    m_bOpen = NO;
    m_bShowFull = NO;
    m_iVideoWidth  = 0;
    m_iVideoHeight = 0;
    
    m_iWinWidth  = self.frame.size.width;
    m_iWinHeight = self.frame.size.height;
    m_dx1 = m_dy1 = m_dx2 = m_dy2 = 0;
    
    CAEAGLLayer *eaglLayer = (CAEAGLLayer*) self.layer;
    eaglLayer.opaque = YES;
    eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                    [NSNumber numberWithBool:NO], kEAGLDrawablePropertyRetainedBacking,
                                    kEAGLColorFormatRGB565, kEAGLDrawablePropertyColorFormat,
                                    //[NSNumber numberWithBool:YES], kEAGLDrawablePropertyRetainedBacking,
                                    nil];
    self.contentScaleFactor = [UIScreen mainScreen].scale;
    _viewScale = [UIScreen mainScreen].scale;
    m_glCtx = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    if(!m_glCtx || ![EAGLContext setCurrentContext:m_glCtx]){
        return NO;
    }
    
    [self setupYUVTexture];
    [self loadShader];
    glUseProgram(_program);
    
    GLuint textureUniformY = glGetUniformLocation(_program, "SamplerY");
    GLuint textureUniformU = glGetUniformLocation(_program, "SamplerU");
    GLuint textureUniformV = glGetUniformLocation(_program, "SamplerV");
    glUniform1i(textureUniformY, 0);
    glUniform1i(textureUniformU, 1);
    glUniform1i(textureUniformV, 2);
    
    return YES;
}

//从xib初始化
- (id)initWithCoder:(NSCoder *)aDecoder{
    self = [super initWithCoder:aDecoder];
    if (self){
        if (![self doInit]){
            self = nil;
        }
    }
    return self;
}

//自定义
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
    dispatch_async(dispatch_get_global_queue(0, 0), ^{
        @synchronized(self){
            [EAGLContext setCurrentContext:m_glCtx];
            [self destoryFrameAndRenderBuffer];
            [self createFrameAndRenderBuffer];
        }
        glViewport(1, 1, self.bounds.size.width*_viewScale - 2, self.bounds.size.height*_viewScale - 2);
    });
}

- (void)setupYUVTexture{
    if (_textureYUV[TEXY]){
        glDeleteTextures(3, _textureYUV);
    }
    glGenTextures(3, _textureYUV);
    if (!_textureYUV[TEXY] || !_textureYUV[TEXU] || !_textureYUV[TEXV]){
        NSLog(@"<<<<<<<<<<<<纹理创建失败!>>>>>>>>>>>>");
        return;
    }
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _textureYUV[TEXY]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, _textureYUV[TEXU]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, _textureYUV[TEXV]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

//计算绘制窗口
-(void)ProcessSize{
    m_iWinWidth  = self.frame.size.width * [UIScreen mainScreen].scale;
    m_iWinHeight = self.frame.size.height * [UIScreen mainScreen].scale;
    DrawProcess(m_iVideoWidth,m_iVideoHeight, m_iWinWidth, m_iWinHeight, &m_dx1, &m_dy1);
    DrawProcess2(m_iVideoWidth,m_iVideoHeight, m_iWinWidth, m_iWinHeight, &m_dx2, &m_dy2);
}

#pragma mark - 设置openGL
+ (Class)layerClass{
    return [CAEAGLLayer class];
}

- (BOOL)createFrameAndRenderBuffer{
    glGenFramebuffers(1, &_framebuffer);
    glGenRenderbuffers(1, &_renderBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, _renderBuffer);
    if (![m_glCtx renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer *)self.layer]){
        NSLog(@"attach渲染缓冲区失败");
    }
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _renderBuffer);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        NSLog(@"创建缓冲区错误 0x%x", glCheckFramebufferStatus(GL_FRAMEBUFFER));
        return NO;
    }
    return YES;
}

- (void)destoryFrameAndRenderBuffer{
    if (_framebuffer){
        glDeleteFramebuffers(1, &_framebuffer);
    }
    
    if (_renderBuffer){
        glDeleteRenderbuffers(1, &_renderBuffer);
    }
    _framebuffer = 0;
    _renderBuffer = 0;
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

- (void)loadShader{
    GLuint vertexShader = [self compileShader:VSH withType:GL_VERTEX_SHADER];
    GLuint fragmentShader = [self compileShader:FSH withType:GL_FRAGMENT_SHADER];
    _program = glCreateProgram();
    glAttachShader(_program, vertexShader);
    glAttachShader(_program, fragmentShader);
    glBindAttribLocation(_program, ATTRIB_VERTEX, "position");
    glBindAttribLocation(_program, ATTRIB_TEXTURE, "TexCoordIn");
    glLinkProgram(_program);
    GLint linkSuccess;
    glGetProgramiv(_program, GL_LINK_STATUS, &linkSuccess);
    if (linkSuccess == GL_FALSE) {
        GLchar messages[256];
        glGetProgramInfoLog(_program, sizeof(messages), 0, &messages[0]);
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
    if (!shaderString) {
        exit(1);
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


#pragma mark - 设置父窗口
- (MSVideoDraw*)initWithParent:(UIView*)view  doAction:(SEL)action{
    if(view == nil)return nil;
    CGRect frame = CGRectMake(0, 0, view.frame.size.width, view.frame.size.height);
    self = [self initWithFrame:frame];
    if(self){
        [view addSubview:self];
        if(action != nil){
            //长按函数
            UILongPressGestureRecognizer* longPressReger = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:action];
            longPressReger.minimumPressDuration = 0.2;
            [self addGestureRecognizer:longPressReger];
        }
    }
    return self;
}
#pragma mark - 视频绘制接口
- (void)DrawVideoData:(uint8_t *)data{
    @synchronized(self) {
        if(!m_bOpen)return;
        if ([UIApplication sharedApplication].applicationState != UIApplicationStateActive)
            return; //后台模式不绘制
        if(![self isDisplayedInScreen]){
            NSLog(@"Self VideoDraw is Hide %@", self);
            return;
        }
        if(self.superview != nil){
            if(self.superview.frame.size.width != self.frame.size.width || self.superview.frame.size.height != self.frame.size.height){
                //随着父窗口改变大小
                dispatch_async(dispatch_get_main_queue(), ^{
                    self.frame = CGRectMake(0, 0, self.superview.frame.size.width, self.superview.frame.size.height);
                });
            }
            
        }
        if(self.frame.size.width != m_iWinWidth || self.frame.size.height != m_iWinHeight){
            [self ProcessSize]; //改变绘制窗口的大小
        }
        [EAGLContext setCurrentContext:m_glCtx];
        glBindTexture(GL_TEXTURE_2D, _textureYUV[TEXY]);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_iVideoWidth, m_iVideoHeight, GL_RED_EXT, GL_UNSIGNED_BYTE, data);
        glBindTexture(GL_TEXTURE_2D, _textureYUV[TEXU]);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_iVideoWidth/2, m_iVideoHeight/2, GL_RED_EXT, GL_UNSIGNED_BYTE, data + m_iVideoWidth * m_iVideoHeight);
        glBindTexture(GL_TEXTURE_2D, _textureYUV[TEXV]);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_iVideoWidth/2, m_iVideoHeight/2, GL_RED_EXT, GL_UNSIGNED_BYTE, data + m_iVideoWidth * m_iVideoHeight * 5 / 4);
        
        m_bShowFull ?
        glViewport(m_dx2, m_dy2, m_iWinWidth - 2 * m_dx2, m_iWinHeight - 2 * m_dy2) :
        glViewport(m_dx1, m_dy1, m_iWinWidth - 2 * m_dx1, m_iWinHeight - 2 * m_dy1);
        
        static const GLfloat squareVertices[] = {
            -1.0f, -1.0f, 1.0f, -1.0f,
            -1.0f,  1.0f, 1.0f,  1.0f,
        };
        
        static const GLfloat coordVertices[] = {
            0.0f, 1.0f,  1.0f, 1.0f,
            0.0f,  0.0f, 1.0f,  0.0f,
        };
        glVertexAttribPointer(ATTRIB_VERTEX, 2, GL_FLOAT, 0, 0, squareVertices);
        glEnableVertexAttribArray(ATTRIB_VERTEX);
        glVertexAttribPointer(ATTRIB_TEXTURE, 2, GL_FLOAT, 0, 0, coordVertices);
        glEnableVertexAttribArray(ATTRIB_TEXTURE);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindRenderbuffer(GL_RENDERBUFFER, _renderBuffer);
        [m_glCtx presentRenderbuffer:GL_RENDERBUFFER];
    }
}


- (void)Close{
    @synchronized (self) {
        m_iVideoWidth = 0;
        m_iVideoHeight = 0;
        m_bOpen = NO;
    }
}

- (BOOL)Open{
    @synchronized(self) {
        if(m_iVideoWidth == 0 || m_iVideoHeight == 0)return NO;
        [self ProcessSize];
        m_bOpen = YES;
        void *blackData = alloca(m_iVideoWidth * m_iVideoHeight * 3 / 2);
        memset(blackData, 0, m_iVideoWidth * m_iVideoHeight * 3 / 2);
        [EAGLContext setCurrentContext:m_glCtx];
        glBindTexture(GL_TEXTURE_2D, _textureYUV[TEXY]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED_EXT, m_iVideoWidth, m_iVideoHeight, 0, GL_RED_EXT, GL_UNSIGNED_BYTE, blackData);
        glBindTexture(GL_TEXTURE_2D, _textureYUV[TEXU]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED_EXT, m_iVideoWidth/2, m_iVideoHeight/2, 0, GL_RED_EXT, GL_UNSIGNED_BYTE, blackData + m_iVideoWidth * m_iVideoHeight);
        glBindTexture(GL_TEXTURE_2D, _textureYUV[TEXV]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED_EXT, m_iVideoWidth/2, m_iVideoHeight/2, 0, GL_RED_EXT, GL_UNSIGNED_BYTE, blackData + m_iVideoWidth * m_iVideoHeight * 5 / 4);
        return m_bOpen;
    }
}


- (void)ClearVideo{
    @synchronized(self) {
        if ([self window]){
            [EAGLContext setCurrentContext:m_glCtx];
            glClearColor(0.0, 0.0, 0.0, 1.0);
            glClear(GL_COLOR_BUFFER_BIT);
            glBindRenderbuffer(GL_RENDERBUFFER, _renderBuffer);
            [m_glCtx presentRenderbuffer:GL_RENDERBUFFER];
        }
    }
    
}



#pragma mark - 绘制音波条方法
-(void)DrawAudioLevel:(int)level{
    @synchronized (self) {
        if(level > 32767)level = 32767;
        if(level < 0)level = 0;
        int width = self.frame.size.width;
        int height = 3.0;
        int dst_width = level * width / 32767;
        if(m_shapeLayer!=nil)[m_shapeLayer removeFromSuperlayer];
        m_shapeLayer = [CAShapeLayer layer];
        [m_shapeLayer setBounds:CGRectMake(0, 0, width, height)];
        [m_shapeLayer setPosition:CGPointMake(width / 2, height)];
        [m_shapeLayer setLineWidth:height];
        [m_shapeLayer setLineJoin:kCALineJoinRound];
        [m_shapeLayer setLineDashPattern:[NSArray arrayWithObjects:[NSNumber numberWithInt:4], [NSNumber numberWithInt:1],  nil]];
        [m_shapeLayer setFillColor:[UIColor clearColor].CGColor];
        [m_shapeLayer setStrokeColor:[UIColor greenColor].CGColor];
        CGMutablePathRef path = CGPathCreateMutable();
        CGPathMoveToPoint(path, NULL, 0, 0);
        CGPathAddLineToPoint(path, NULL, dst_width, 0);
        [m_shapeLayer setPath:path];
        CGPathRelease(path);
        [self.layer addSublayer:m_shapeLayer];
    }
}

@end
