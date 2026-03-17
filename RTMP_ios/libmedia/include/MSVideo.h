#ifndef _MS_VIDEO_H_
#define _MS_VIDEO_H_


#include <stdint.h>


#define SIZE_CIF 0
#define SIZE_VGA 1
#define SIZE_HD  2


class IAudioCaptureSink{
public:
    virtual void OnAudioData(uint8_t *buf, int buf_size) = 0;
};
//摄像头初始化
void* IAudioCaptureCreate(IAudioCaptureSink* pSink, int sample_rate, int channel);
//销毁摄像头
void  IAudioCaptureDestroy(void* ptr);

class IVideoCaptureSink{
public:
    virtual void OnVideoData(uint8_t *buf, int width, int height) = 0;
};

//摄像头初始化
void* IVideoCaptureCreate(IVideoCaptureSink* pSink);
//销毁摄像头
void  IVideoCaptureDestroy(void* ptr);

class IMSDraw{
public:
    static IMSDraw* Create();
    static void Destroy(IMSDraw *p);
public:
    virtual void SetView(void *view) = 0;//UIView*
    virtual void SetSize(int w, int h) = 0;
    virtual void Draw(uint8_t *buf) = 0;
    virtual void Draw2(uint8_t *buf) = 0;
    virtual void Clear() = 0;
};

#endif
