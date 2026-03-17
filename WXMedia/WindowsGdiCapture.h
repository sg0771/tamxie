/*
* 基于GDI的桌面视频采集类
* 首先枚举显示器设备，获取每个显示器在虚拟屏幕上的位置RECT(x,y,w,h)
* 把主屏幕HDC对应区域内容拷贝到内存DC，然后选入到HBITMAP，从HBITMAP获取RGBA数据
* WIN7 专用！
*/

#ifndef __WX_WINDOWS_GDI_CAPTURE_H_
#define __WX_WINDOWS_GDI_CAPTURE_H_

#include <ScreenGrab.hpp>
#include "VideoSource.h"


class WindowsGdiCapture :public VideoSource, public WXTcpThread {
	void* m_pBits = nullptr;

	//数据队列
	int m_nPool = 10;//队列长度
	WXTcpLocker m_lockQueue;
	std::queue<int>m_queueData;
	WXTcpVideoFrame m_arrData[MAX_POOL];//数据池
	WXTcpVideoFrame  m_OutputVideoFrame;//输出的图像
	WXTcpVideoFrame* WindowsGdiCapture::GDIGrabFrame();
public:
	virtual void ThreadProcess();//采集线程
public:
	virtual int      Start(ScreenCapture* capture);//启动
	virtual void     Stop();     //结束
	virtual WXTcpVideoFrame* GrabFrame(); //获取数据
	virtual const wchar_t* Type();  //类型

};

#endif //__WX_WINDOWS_GDI_CAPTURE_H_

