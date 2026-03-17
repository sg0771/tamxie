#include "WXImage.h"
#include <iostream>
#ifdef _WIN32
#include <Windows.h>
#define dlopen(name, flags) LoadLibraryA(name)
#define dlclose  FreeLibrary
#define dlsym    GetProcAddress
#define LIB_NAME "WX_Image.dll"
#else
#include <dlfcn.h>
typedef void* HMODULE;
#define LIB_NAME "WX_Image.so"
#endif

static HMODULE s_handle = nullptr;
EXTERN_C typedef  void ( *func_WXImage_InitLibrary)();
func_WXImage_InitLibrary s_init = nullptr;

EXTERN_C typedef void (*func_WXImage_SetLog)(int);
func_WXImage_SetLog s_set_log = nullptr;

EXTERN_C typedef int (*func_CompressQuality_FileToFile)(const char* strInput, const char* strOutput,
	int target_type, int target_level, int dst_width, int dst_height);
func_CompressQuality_FileToFile s_CompressQuality = nullptr;

EXTERN_C typedef int (*func_CompressSize_FileToFile)(const char* strInput, const char* strOutput,
	int target_type, int target_size, int dst_width, int dst_height);;
func_CompressSize_FileToFile s_CompressSize = nullptr;


void InitLib() {
	s_handle = dlopen(LIB_NAME, 0);
	if (s_handle) {
		s_init            = (func_WXImage_InitLibrary)dlsym(s_handle, "WXImage_InitLibrary");
		s_set_log         = (func_WXImage_SetLog)dlsym(s_handle, "WXImage_SetLog");
		s_CompressQuality = (func_CompressQuality_FileToFile)dlsym(s_handle, "CompressQuality_FileToFile");
		s_CompressSize    = (func_CompressSize_FileToFile)dlsym(s_handle, "CompressSize_FileToFile");
	}
}

int main() {
	InitLib();
	if(s_init)
		s_init();//库初始化

	if(s_set_log)
		s_set_log(1);//打印日志

	//按质量系数压缩
	//按压缩质量系数80, 编码为JPEG格式， 保持分辨率不变
	int ret = s_CompressQuality("1.jpg", "1_CompressQuality_jpeg.jpg", WXIMAGE_TYPE_JPEG, 80, 0, 0);
	std::cout << "CompressQuality_FileToFile Quality80  = " << ret << std::endl;

	//按压缩质量系数80, 编码为JPEG格式， 指定分辨率800x500
	ret = s_CompressQuality("1.jpg", "1_CompressQuality_jpeg_800x500.jpg", WXIMAGE_TYPE_JPEG, 80, 800, 500);

	std::cout << "CompressQuality_FileToFile Quality80 800x500 = " << ret << std::endl;

	//按压缩质量系数80, 编码为JPEG格式， 保持分辨率不变
	ret = s_CompressQuality("1.jpg", "1_CompressQuality_webp.jpg", WXIMAGE_TYPE_WEBP, 80, 0, 0);

	std::cout << "CompressQuality_FileToFile Quality80 = " << ret << std::endl;

	//按压缩质量系数80, 编码为JPEG格式，  指定分辨率800x500
	ret = s_CompressQuality("1.jpg", "1_CompressQuality_webp_800x500.jpg", WXIMAGE_TYPE_WEBP, 80, 800, 500);

	std::cout << "CompressQuality_FileToFile Quality80 800x500 = " << ret << std::endl;
	//按压缩系数80, 编码为PNG格式， 保持分辨率不变
	//PNG格式其实只是指定了内部zlib压缩系数，对实际压缩大小可能没什么影响
	ret = s_CompressQuality("1.jpg", "1_CompressQuality_png.jpg", WXIMAGE_TYPE_PNG, 80, 0, 0);

	std::cout << "CompressQuality_FileToFile Quality80  = " << ret << std::endl;

	//按压缩系数80, 编码为PNG格式，  指定分辨率800x500
	//PNG格式其实只是指定了内部zlib压缩系数，对实际压缩大小可能没什么影响
	ret = s_CompressQuality("1.jpg", "1_CompressQuality_png_800x500.jpg", WXIMAGE_TYPE_PNG, 80, 800, 500);

	std::cout << "CompressQuality_FileToFile Quality80 800x500 = " << ret << std::endl;


	//按指定大小压缩
	//指定大小800K, 编码为JPEG格式， 保持分辨率不变
	ret = s_CompressSize("1.jpg", "1_CompressSize_jpeg.jpg", WXIMAGE_TYPE_JPEG, 500, 0, 0);

	std::cout << "CompressSize_FileToFile Size 500K = " << ret << std::endl;


	//指定大小500K, 编码为JPEG格式， 指定分辨率800x500
	ret = s_CompressSize("1.jpg", "1_CompressSize_jpeg_800x500.jpg", WXIMAGE_TYPE_JPEG, 200, 800, 500);
	std::cout << "CompressSize_FileToFile Size=200K 800x500 = " << ret << std::endl;

	//指定大小800K, 编码为JPEG格式， 保持分辨率不变
	ret = s_CompressSize("1.jpg", "1_CompressSize_jpeg.webp", WXIMAGE_TYPE_WEBP, 500, 0, 0);
	std::cout << "CompressSize_FileToFile Size 500K = " << ret << std::endl;

	//指定大小500K, 编码为JPEG格式， 指定分辨率800x500
	ret = s_CompressSize("1.jpg", "1_CompressSize_jpeg_800x500.webp", WXIMAGE_TYPE_WEBP, 200, 800, 500);
	std::cout << "CompressSize_FileToFile  Size 200K 800x500 = " << ret << std::endl;

	return 0;
}