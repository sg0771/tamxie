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

	public final int MODE_LOW    = 0; //压缩率较低模式
	public final int MODE_NORMAL = 1; //默认模式
	public final int MODE_HIGH   = 2; //压缩率较高模式
	public final int MODE_CUSTOM = 3; //自定义压缩模式

    static {
        System.loadLibrary("MediaConvert");
    }

    //----------------------NDK API ----------------------------
    
	//功能:       初始化视频压缩SDK
    //参数 [ctx]: 使用Activity类的getApplicationContext() 即可
    //返回值:     成功返回1，失败返回0
    //说明:       最先调用，失败则SDK无法使用
	public native int LibraryInit(Context ctx);

    //功能:   接口调用失败后获取错误信息
    //参数:   无
    //返回值: 返回错误信息
	public native String GetLastError();

	//功能:             创建一个操作句柄
	//参数 [strInput]:  转换的视频源文件名
	//返回值:           文件解析成功返回非0值，解析失败返回0
	public native long Create(String strInput);

	//功能:              获取缩略图,JPEG图像
	//参数  [handle] :   操作句柄，Create方法的返回值
	//参数  [strThumb] : 缩略图文件名
	//返回值 : 成功返回1，失败返回0
	public native int GetThumb(long handle, String jstrThumb);

	//功能:           获取视频源文件分辨率宽度
	//参数 [handle] : 操作句柄，Create方法的返回值
	//返回值 :        返回视频源文件分辨率宽度
	public native int GetWidth(long handle);

	//功能:            获取视频源文件分辨率高度
	//参数  [handle] : 操作句柄，Create方法的返回值
	//返回值 :         返回视频源文件分辨率高度
	public native int GetHeight(long handle);

	//功能:           获取视频源文件帧率
	//参数handle :    操作句柄，Create方法的返回值
	//返回值 :        返回视频源文件帧率
	public native int GetFps(long handle);


	//功能             : 返回指定参数的视频压缩的输出文件预计大小
	//参数 [handle]    : 操作句柄，Create的返回值
	//参数 [width]     : 视频分辨率宽度,0表示原始宽度
	//参数 [height]    : 视频分辨率高度,0表示原始高度
	//参数 [fps]       : 视频分辨率高度,0表示原始帧率
	//参数 [mode]      : 视频压缩模式
	//返回值           : 输出文件预计大小，单位KB
	//说明             : 用于计算指定分辨率对应几种模式的压缩文件的预估大小
	//                 : 其中MODE_LOW    对应最大值
	//                 : 其中MODE_NORMAL 对应默认值
	//                 : 其中MODE_HIGH   对应最小值
	//                 : 返回值可以用于Excute方法来指定视频压缩文件的预估大小
	public native int GetLength(long handle,int width, int height, int fps, int mode);


	//功能             : 压缩视频
	//参数 [handle]    : 操作句柄，Create的返回值
	//参数 [width]     : 视频分辨率宽度,0表示原始宽度
	//参数 [height]    : 视频分辨率高度,0表示原始高度
	//参数 [fps]       : 视频分辨率高度,0表示原始帧率
	//参数 [target_size]: 预设的输出文件大小，单位KB
	//参数 [strOutput] : 表示要转换的输出视频文件名
	//返回值           : 成功返回1，失败返回0
	//说明             : 当预估大小target_size超出aaGetLength最大最小时可能会转换失败
	public native int Excute(long handle, int width, int height, int fps, int target_size, String strOutput);


	//功能                : 返回指定参数的视频压缩的输出文件预计大小
	//参数 [handle]       : 操作句柄，Create的返回值
	//参数 [video_bitrate]: 视频码率,单位kbps，建议不小于300，不大于8000
	//参数 [audio_bitrate]: 视频码率,单位kbps，建议在64-128之间
	//返回值              : 输出文件预计大小，单位KB
	//说明                : 用于计算指定分辨率对应几种模式的压缩文件的预估大小
	//                    : 返回值可以用于显示ExcuteExt转换方法的预估大小
	public native int GetLengthExt(long handle,int video_bitrate, int audio_bitrate);

	//功能             : 返回指定分辨率和压缩模式的视频压缩的输出文件预计大小
	//参数 [handle]    : 操作句柄，Create的返回值
	//参数 [width]     : 视频分辨率宽度,0表示原始宽度
	//参数 [height]    : 视频分辨率高度,0表示原始高度
	//参数 [fps]       : 视频分辨率高度,0表示原始帧率
	//参数 [video_bitrate]: 视频码率,单位kbps，建议不小于300，不大于8000
	//参数 [audio_bitrate]: 视频码率,单位kbps，建议在64-128之间
	//参数 [strOutput] : 表示要转换的输出视频文件名
	//返回值           : 成功返回1，失败返回0
	//说明             : 过大或者过小的音视频码率的设置可能会导致转码失败，请尝试使用MediaConvertExcute重新转码
	public native int ExcuteExt(long handle, int width, int height, int fps, int video_bitrate, int audio_bitrate, String strOutput);


	//功能:               强制中断转换进程
	//参数  [handle] :    操作句柄，Create方法的返回值
	//返回值:             成功返回1，失败返回0
	//说明:               执行后需要不断调用GetState获取执行进度
	public native int Interrupt(long handle);

	//功能:               获取转换进度
	//参数  [handle] :    操作句柄，Create方法的返回值
	//返回值 :
	// -1 表示转换失败
	// -2 表示未开始运行转换
	// 100表示转换成功
	// 0 - 99 表示转换进度百分比
	//说明 : 获得返回值为 - 1 或者 100 才可以调用Destroy销毁句柄
	public native int GetState(long handle);

	//功能:               销毁转换句柄
	//参数  [handle] :    操作句柄，Create方法的返回值
	//返回值:             成功返回1，失败返回0
	//说明:               可以等到GetState 返回 -1 或者100再销毁，或者调用Interrupt再销毁
	public native int Destroy(long handle);
};
