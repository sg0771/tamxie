/*
视频编码器
输出长度是个int，高位第一个字节表示是否关键帧
其余三个字节表示长度
*/
package com.mediastream;

public class H264Encoder{
	static{
	     System.loadLibrary("ms2");
	}
	//fram_interval,  关键帧间隔
	//qp质量参数手机一般在28-34
	//fps 左右不大，一般用来标记fram_interval
	public native int    Open(int width, int height, int fram_interval, int qp, int fps);
	public native int    Close(int handle);
	
	public native int    GetWidth(int handle);
	public native int    GetHeight(int handle);
	//返回长度第一个字节标记是否为关键帧
	//out由java申请控件，一般用w*h 足够
	public native int    Encode(int handle, byte []in, byte []out, int force_key);
	

	int m_handle = 0;
	public int  X264Open(int width, int height, int fram_interval, int qp, int fps){
		if(m_handle != 0)X264Close();
		m_handle = Open(width,height, fram_interval, qp, fps);
		return m_handle;
	}
	public void X264Close(){
		if(m_handle == 0)return;
		Close(m_handle);
		m_handle = 0;
	}
	//返回长度第一个字节标记是否为关键帧
	//out由java申请控件，一般用w*h 足够
	public int X264Encode(byte []in, byte []out, int force_key){
		if(m_handle == 0)return 0;
		return Encode(m_handle,in,out,force_key);
	}	
}