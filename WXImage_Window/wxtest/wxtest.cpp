#include "WXImage.h"
#include <iostream>
//#include <stdint.h>

#ifdef _WIN32
#pragma warning(disable:4996)
#include <Windows.h>
#define dlopen(name, flags) LoadLibraryA(name)
#define dlclose  FreeLibrary
#define dlsym    GetProcAddress
#define LIB_NAME "WX_Image.dll"
#else
#include <dlfcn.h>
#include <strings.h>
typedef void* HMODULE;
#define LIB_NAME "WX_Image.so"
#define stricmp strcasecmp
#endif

static HMODULE s_handle = nullptr;
EXTERN_C typedef  void (*func_WXImage_InitLibrary)();
func_WXImage_InitLibrary s_init = nullptr;

EXTERN_C typedef void (*func_WXImage_SetLog)(int);
func_WXImage_SetLog s_set_log = nullptr;

EXTERN_C typedef int (*func_CompressQuality_FileToFile)(const char* strInput, const char* strOutput,
	int target_type, int target_level, int dst_width, int dst_height);
func_CompressQuality_FileToFile s_CompressQuality = nullptr;

EXTERN_C typedef int (*func_CompressSize_FileToFile)(const char* strInput, const char* strOutput,
	int target_type, int target_size, int dst_width, int dst_height);;
func_CompressSize_FileToFile s_CompressSize = nullptr;


int main(int argc, char* argv[]) {

	//动态加载库
	s_handle = dlopen(LIB_NAME, RTLD_LAZY);
	if (s_handle) {
		s_init = (func_WXImage_InitLibrary)dlsym(s_handle, "WXImage_InitLibrary");
		s_set_log = (func_WXImage_SetLog)dlsym(s_handle, "WXImage_SetLog");
		s_CompressQuality = (func_CompressQuality_FileToFile)dlsym(s_handle, "CompressQuality_FileToFile");
		s_CompressSize = (func_CompressSize_FileToFile)dlsym(s_handle, "CompressSize_FileToFile");
		if (s_init == nullptr || s_set_log == nullptr || s_CompressQuality == nullptr || s_CompressSize == nullptr) {
			dlclose(s_handle);
			s_handle = nullptr;
		}
	}
	else {
		std::cout << "Laod " << LIB_NAME << " Error" << std::endl;
		return 0;
	}

	if (s_handle == nullptr) {
		std::cout << "Laod " << LIB_NAME << " Error" << std::endl;
		return 0;
	}

	if (s_init)
		s_init();//库初始化

	//可选参数
	int bLog = 0;//是否打印日志  // -log //默认0
	int type = WXIMAGE_TYPE_JPEG;//-type //默认jpeg输出，可用[jpeg/webp/png]
	int dw = 0;//-dw 指定输出分辨率宽度，默认0表示不变
	int dh = 0;//-dh 指定输出分辨率高度，默认0表示不变[需要同时设置 dw dh]

	//必选参数
	std::string  strInput = ""; // -i
	std::string  strOutput = ""; // -o

	//二选一参数
	int dsize = 0;//指定输出大小编码[单位KB][只能使用jpeg/webp格式]
	int level = 0;//指定编码系数[0-100]，可用[jpeg/webp/png]格式，不能同时使用 dsize

	for (int i = 1; i < argc; i +=2) {
		const char* strParam = argv[i];
		const char* strValue = argv[i+1];
		if (stricmp(strParam, "-i") == 0) {
			strInput = strValue;
		}
		else if (stricmp(strParam, "-o") == 0) {
			strOutput = strValue;
		}
		else if (stricmp(strParam, "-log") == 0) {
			bLog = atoi(strValue);
		}
		else if (stricmp(strParam, "-type") == 0) {
			if (stricmp(strValue, "jpeg") == 0) {
				type = WXIMAGE_TYPE_JPEG;
				std::cout << "type=JPEG" << std::endl;
			}
			else if (stricmp(strValue, "jpg") == 0) {
				type = WXIMAGE_TYPE_JPEG;
				std::cout << "type=JPEG" << std::endl;
			}
			else if (stricmp(strValue, "png") == 0) {
				type = WXIMAGE_TYPE_PNG;
				std::cout << "type=PNG" << std::endl;
			}
			else if (stricmp(strValue, "webp") == 0) {
				type = WXIMAGE_TYPE_WEBP;
				std::cout << "type=webp" << std::endl;
			}
		}
		else if (stricmp(strParam, "-dw") == 0) {
			dw = atoi(strValue);
		}
		else if (stricmp(strParam, "-dh") == 0) {
			dh = atoi(strValue);
		}
		else if (stricmp(strParam, "-dsize") == 0) {
			dsize = atoi(strValue);
		}
		else if (stricmp(strParam, "-level") == 0) {
			level = atoi(strValue);
		}
		else {
			break;
		}
	}
	if (strInput.length() == 0) {
		std::cout << "No Input File" << std::endl;
		return -1;
	}

	if (strOutput.length() == 0) {
		std::cout << "No Output File" << std::endl;
		return -1;
	}

	if (dsize != 0 && level != 0) {
		std::cout << "Cannot set two parameters simultaneously [dsize level]" << std::endl;
		return -1;
	}

	if (dsize == 0 && level == 0) {
		std::cout << "Must set dsize or level" << std::endl;
		return -1;
	}

	if ((dw == 0 && dh != 0) || (dw != 0 && dh == 0)) {
		std::cout << "Must Simultaneously setting two parameters [dw dh]" << std::endl;
		return -1;
	}

	if (dsize != 0 && type == WXIMAGE_TYPE_PNG) {
		std::cout << "PNG does not support specifying compression size" << std::endl;
		return -1;
	}

	if (bLog) {
		s_set_log(1);//打印日志
		std::cout << "strInput=" << strInput << std::endl;
		std::cout << "strOutput=" << strOutput << std::endl;
		std::cout << "dsize=" << dsize << std::endl;
		std::cout << "level=" << level << std::endl;
		std::cout << "dh=" << dh << std::endl;
		std::cout << "dw=" << dw << std::endl;

	}


	if (dsize != 0) { //指定压缩大小
		int ret = s_CompressSize(strInput.c_str(), strOutput.c_str(), type, dsize, dw, dh);
		std::cout << "CompressSize " << strInput << " " << strOutput << " " << "Dsize=" << dsize << "KB" << std::endl;
		if (dw && dh) {
			std::cout << dw << "x" << dh << std::endl;
		}
		std::cout << "Ret= " << ret << std::endl;
	}
	else { //按系数压缩
		int ret = s_CompressQuality(strInput.c_str(), strOutput.c_str(), type, level, dw, dh);
		std::cout << "CompressQuality " << strInput << " " << strOutput << " " << "level=" << level << std::endl;
		if (dw && dh) {
			std::cout << dw << "x" << dh << std::endl;
		}
		std::cout << "Ret= " << ret << std::endl;
	}
	return 0;
}