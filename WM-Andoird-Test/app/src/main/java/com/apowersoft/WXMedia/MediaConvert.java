/******************************************************************************************************
    功能: 视频压缩SDK 
    版权: Copyright ©  2022 Apowersoft Ltd. All rights reserved.
    作者: Tam.Xie
    日期: 2022.03.21
*******************************************************************************************************/

package com.apowersoft.WXMedia;

import android.app.Application;
import android.content.Context;

import cn.wps.moffice_eng.AppApplication;

public class MediaConvert {
    static {
        System.loadLibrary("MediaConvert");
    }

    // ------------------- 水印管家API ----------------------
	//创建转换对象
	//返回:返回0表示失败
	public native long CreateConvert();

	//销毁转换对象
	//参数
	//handle:转换对象
	public native  void  DestroyConvert(long handle);

	//启动转换
	//handle:转换对象
	//ptr:转换对象
	public native  void  StartConvert(long handle);

	//中断或者结束转换
	//参数
	//handle:转换对象
	public native  void   StopConvert(long handle);


	//获取转换状态
	//参数
	//handle:转换对象
	//返回值，小于0表示失败，100表示成功
	public native int   GetConvertProgress(long handle);

	//设置转换开始-结束时间
	//参数
	//handle:转换对象
	//_start_time:开始时间，单位毫秒
	//duration:持续时间，单位毫秒
	public native  void  SetVideoConvertTime(long handle, long _start_time, long duration);


	//设置转换参数
	//参数
	//handle:转换对象
	//in_file:输入文件
	//out_file:输出文件
	//logo_rects:水印数组，4个一组，x,y,w,h
	//nb_rect:水印数量
	//返回值:返回0表示失败，返回1表示成功
	public native  int   InitSource(long handle, String in_file, String out_file,int[] logo_rects, int nb_rect);

	//设置转换参数
	//参数
	//handle:转换对象
	//in_file:输入文件
	//out_file:输出文件
	//x,y:水印起始位置
	//width,height:水印大小
	//返回值:返回0表示失败，返回1表示成功
	public native  int   InitSourceEx(long handle, String in_file, String out_file,
									int PosX, int PosY, int width, int height);
};
