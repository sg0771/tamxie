/*
 视频转换去水印API
 来自于linux版本的水印管家
 */
#ifndef _WX_PKGFF_H_
#define _WX_PKGFF_H_


#include "WXFFmpegConvertAPI.h"

//创建转换对象
//返回:返回NULL表示失败
WXDELOGO_CAPI void* CreateConvert();

//销毁转换对象
//参数
//ptr:转换对象
WXDELOGO_CAPI void  DestroyConvert(void* ptr);


//设置转换开始-结束时间
//参数
//ptr:转换对象
//_start_time:开始时间，单位毫秒
//_start_time:结束时间，单位毫秒
WXDELOGO_CAPI void  SetVideoConvertTime(void* ptr, int64_t _start_time, int64_t duration);

//设置转换参数
//参数
//ptr:转换对象
//in_file:输入文件
//out_file:输出文件
//logo_rectangles:水印数组
//nb_rects:水印个数
//返回值:返回0表示失败，返回1表示成功
WXDELOGO_CAPI int   InitSource(void* ptr, WXCTSTR in_file, WXCTSTR out_file, int(*logo_rectangles)[4], int nb_rects);


//启动转换
//参数
//ptr:转换对象
WXDELOGO_CAPI void  StartConvert(void* ptr);

//中断或者结束转换
//参数
//ptr:转换对象
WXDELOGO_CAPI void  StopConvert(void* ptr);

//获取转换状态
//参数
//ptr:转换对象
WXDELOGO_CAPI int   GetConvertProgress(void* ptr);

#endif
