#按指定quality参数压缩图像为JPEG、PBG、WEBP图像
import platform
import ctypes
import os.path
import time
import os
    
strPath  = os.path.dirname(os.path.abspath(__file__)) + os.path.sep 
print(strPath)

strSep = "\\"
dllName = "WX_Image.dll"   #Windows 动态库
if platform.system() == 'Linux':   #区分系统
    dllName = "WX_Image.so"
    strSep = "/"

dllPath  = strPath + dllName
print(dllPath)

WXIMAGE_TYPE_ORGI = 0
WXIMAGE_TYPE_JPEG = 1
WXIMAGE_TYPE_PNG = 2
WXIMAGE_TYPE_WEBP = 3

WXImage_dll = ctypes.CDLL(dllPath)  #加载DLL

#dll 返回值是指针，需要强制声明
WXImage_dll.HandlerCreate.restype=ctypes.c_void_p


def ReadFile(FileName): #以二进制方式读取图像数据
    fin = open(FileName,"rb")
    DataBuffer = fin.read()      #  数据
    DataSize = len(DataBuffer); #  长度
    fin.close()
    #print(FileName, "Read Size=" ,DataSize)
    return DataBuffer


def WriteFile(FileName, srcBuf): #以二进制方式写入图像数据
    buf_size = len(srcBuf); #  长度
    #print(FileName, "Write Size=" ,buf_size)
    fout = open(FileName,"wb")
    fout.write(srcBuf)
    fout.close()

def mkdir(path):
    path = path.strip()
    # 去除尾部 \ 符号
    path = path.rstrip("\\")
    isExists = os.path.exists(path)
    if not isExists:
        # 如果不存在则创建目录
         # 创建目录操作函数
        os.makedirs(path)
        return True
    else:
        return False

#################################################################################################################################################
#[_srcName]:输入文件名
#[_dstName]:输出文件名
#[_image_type]:压缩图像类型
#[_target_size]:指定输出文件大小，单位KByte
#[_dst_w]: 指定输出分辨率宽度
#[_dst_w]: 指定输出分辨率高度
#返回图像压缩结果，小于0表示失败
    

#[_srcBuf]: 输入图像数据
#[_image_type]:压缩图像类型
#[_target_size]: 指定输出文件大小，单位KByte
#[_dst_w]: 指定输出分辨率宽度
#[_dst_w]: 指定输出分辨率高度
#返回确定长度的buffer
def Compress_BufferToBuffer(_srcBuf, _image_type,_target_quality, _dst_w ,_dst_h):
    data_handle =     WXImage_dll.HandlerCreate()  
    print("CompressQuality_BufferToBuffer BEGIN")
    ret = WXImage_dll.CompressQuality_BufferToBuffer(_srcBuf, len(_srcBuf), ctypes.c_void_p(data_handle), _image_type, _target_quality, _dst_w ,_dst_h)
    print("CompressQuality_BufferToBuffer =", ret)
    if ret >= 0:
        data_size = WXImage_dll.HandlerGetSize(ctypes.c_void_p(data_handle))
        dstBuf = bytes(data_size)
        WXImage_dll.HandlerGetData(ctypes.c_void_p(data_handle), dstBuf) 
        WXImage_dll.HandlerDestroy(ctypes.c_void_p(data_handle))
        return dstBuf
    WXImage_dll.HandlerDestroy(ctypes.c_void_p(data_handle))  
    return None




#################################################################################################################################################
def TestQuality(_srcName, _strDir,  _str_ext, image_type, _target_size, _dst_w ,_dst_h):
    dirName = strPath +  _strDir + strSep;
    mkdir(dirName)
    WXImage_dll.WXImage_SetLog(1)
    srcName  = strPath + _srcName
    dstName4 = dirName + _srcName + ".BufferToBuffer." + _str_ext
    srcBuf = ReadFile(srcName) #读取文件到Buffer

    print("TestQuality!!!!")
    dstBuf4 = Compress_BufferToBuffer(srcBuf, image_type, _target_size, _dst_w ,_dst_h) #OK， 内存数据压缩到内存数据
    if dstBuf4 != None:
        WriteFile(dstName4, dstBuf4);



#################################################################################################################################################
def TestQualityAll(_srcName):
    nImageQuality = 85  # 设置qualiy参数 85
    TestQuality(_srcName, "test_quality_orig_0x0" ,    "jpg", WXIMAGE_TYPE_ORGI,  nImageQuality, 0,   0)    #保持分辨率,优先源文件编码格式



TestQualityAll("1.avif")
