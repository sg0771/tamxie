package com.mediastream;

public class AudioStream {
	static{
	     System.loadLibrary("ms2");
	}
	public native int    Create(int bPCM);//创建流 ,返回handle
	public native void   Destroy(int handle);//销毁流
	
	public native int      AddNode(int handle);//增加输入通道
	public native void   RemoveNode(int handle, int channel);//删除输入通道
	public native void   WriteData(int handle, int channel, byte[] src, int size);//往指定输入通道写入数据
	public native int      GetMax(int handle, int channel);//指定输入通道当前最大值
	public native int      GetLocalMax(int handle);//本地采集
			
	public native int    GetData(int handle,  byte[] dst);//从音频流获取数据
}
