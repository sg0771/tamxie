/*
功能: 视频压缩SDK
作者: Tam.Xie
版本: v1.0
时间: 2022.03.18
*/

#ifndef _WXMEDIA_CONVET_H_
#define _WXMEDIA_CONVET_H_

#ifdef __cplusplus
#define MC_API extern "C"
#else
#define MC_API 
#endif

//视频压缩模式
#define MODE_LOW     0 //压缩率较低模式
#define MODE_NORMAL  1 //默认模式
#define MODE_HIGH    2 //压缩率较高模式
#define MODE_CUSTOM  3 //自定义压缩模式

//功能             : 视频转换压缩SDK 库初始化
//参数             : 无
//返回值           : 成功为1 失败为0
MC_API int MediaConvertLibraryInit();

//功能             : 获取执行错误信息
//参数             : 无
//返回值           : 返回执行错误信息
//说明             : 库全局操作，在其它方法调用失败时可以通过该方法获取错误信息
MC_API const char* MediaConvertGetLastError();

//功能             : 创建一个操作句柄
//参数 [strInput]  : 表示要转换的视频源文件名
//返回值           : 文件解析成功返回非0指针，解析失败返回NULL
MC_API void* MediaConvertCreate(const char* strInput);

//功能             : 设置输出文件名
//参数 [handle]    : 操作句柄，MediaConvertCreate的返回值

//功能             : 获取缩略图，一般是JPG图像
//参数 [ptr]       : 操作句柄，MediaConvertCreate的返回值
//参数 [strThumb]  : 缩略图文件名
//返回值           : 成功返回1，失败返回0
MC_API int MediaConvertGetThumb(void* handle, const char* strThumb);

//功能             : 获取视频源文件分辨率宽度
//参数 [handle]    : 操作句柄，MediaConvertCreate的返回值
//返回值           : 返回视频源文件分辨率宽度
MC_API int MediaConvertGetWidth(void* handle);

//功能             : 获取视频源文件分辨率高度
//参数 [handle]       : 操作句柄，MediaConvertCreate的返回值
//返回值           : 返回视频源文件分辨率高度
MC_API int MediaConvertGetHeight(void* handle);

//功能             : 获取视频源文件帧率
//参数 [handle]    : 操作句柄，MediaConvertCreate的返回值
//返回值           : 返回视频源文件帧率
MC_API int MediaConvertGetFps(void* handle);

//功能             : 返回指定参数的视频压缩的输出文件预计大小
//参数 [handle]    : 操作句柄，MediaConvertCreate的返回值
//参数 [width]     : 视频分辨率宽度,0表示原始宽度
//参数 [height]    : 视频分辨率高度,0表示原始高度
//参数 [fps]       : 视频分辨率高度,0表示原始帧率
//参数 [mode]      : 视频压缩模式
//返回值           : 输出文件预计大小，单位KB
//说明             : 用于计算指定分辨率对应几种模式的压缩文件的预估大小
//                 : 其中MODE_LOW    对应最大值
//                 : 其中MODE_NORMAL 对应默认值
//                 : 其中MODE_HIGH   对应最小值
//                 : 返回值可以用于MediaConvertExcute方法来指定视频压缩文件的预估大小
MC_API int MediaConvertGetLength(void* handle, int width, int height, int fps, int mode);


//功能             : 压缩视频
//参数 [handle]       : 操作句柄，MediaConvertCreate的返回值
//参数 [width]     : 视频分辨率宽度,0表示原始宽度
//参数 [height]    : 视频分辨率高度,0表示原始高度
//参数 [fps]       : 视频分辨率高度,0表示原始帧率
//参数 [target_size]: 预设的输出文件大小，单位KB
//参数 [strOutput] : 表示要转换的输出视频文件名
//返回值           : 成功返回1，失败返回0
//说明             : 当预估大小target_size超出MediaConvertGetLength最大最小时可能会转换失败
MC_API int MediaConvertExcute(void* handle, int width, int height, int fps, int target_size, const char* strOutput);

//功能                : 返回指定参数的视频压缩的输出文件预计大小
//参数 [handle]          : 操作句柄，MediaConvertCreate的返回值
//参数 [video_bitrate]: 视频码率,单位kbps，建议不小于300，不大于8000
//参数 [audio_bitrate]: 视频码率,单位kbps，建议在64-128之间
//返回值              : 输出文件预计大小，单位KB
//说明                : 用于计算指定分辨率对应几种模式的压缩文件的预估大小
//                    : 返回值可以用于显示MediaConvertExcuteExt转换方法的预估大小
MC_API int MediaConvertGetLengthExt(void* ptr, int video_bitrate, int audio_bitrate);


//功能             : 返回指定分辨率和压缩模式的视频压缩的输出文件预计大小
//参数 [ptr]       : 操作句柄，MediaConvertCreate的返回值
//参数 [width]     : 视频分辨率宽度,0表示原始宽度
//参数 [height]    : 视频分辨率高度,0表示原始高度
//参数 [fps]       : 视频分辨率高度,0表示原始帧率
//参数 [video_bitrate]: 视频码率,单位kbps，建议不小于300，不大于8000
//参数 [audio_bitrate]: 视频码率,单位kbps，建议在64-128之间
//参数 [strOutput] : 表示要转换的输出视频文件名
//返回值           : 成功返回1，失败返回0
//说明             : 过大或者过小的音视频码率的设置可能会导致转码失败，请尝试使用MediaConvertExcute重新转码
MC_API int MediaConvertExcuteExt(void* handle, int width, int height, int fps, int video_bitrate, int audio_bitrate, const char* strOutput);

//功能             : 强制中断转换进程
//参数 [handle]    : 操作句柄，MediaConvertCreate的返回值
//返回值           : 成功返回1，失败返回0
//说明             : 堵塞操作，执行完毕后可以调用MediaConvertDestroy销毁句柄
MC_API int MediaConvertInterrupt(void* handle);

//功能             : 获取转换进度
//参数 [handle]    : 操作句柄，MediaConvertCreate的返回值
//返回值           : -1 表示转换失败
//-２ 表示没有开始执行转换
//100表示转换成功
//0 - 99 表示转换进度百分比
MC_API int MediaConvertGetState(void* handle);

//功能             : 获取转换进度
//参数 [handle]    : 操作句柄，MediaConvertCreate的返回值
//返回值           : 成功返回1，失败返回0
MC_API int MediaConvertDestroy(void* handle);


#endif
