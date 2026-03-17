LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_ARM_MODE := arm

LOCAL_C_INCLUDES +=$(ffmpeg-root-dir)
LOCAL_C_INCLUDES +=$(ffmpeg-root-dir)/libffmpeg

LOCAL_ASMFLAGS :=-DPIC=1  -fPIC  
LOCAL_CFLAGS :=-DPIC=1  -fomit-frame-pointer -fPIC -D_ISOC99_SOURCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -O3 -fno-math-errno -fno-signed-zeros -fno-tree-vectorize  -D__ANDROID=1 -Wno-deprecated-declarations  -fpermissive 

LOCAL_LDLIBS += -llog -ldl -lz -lm -pthread -ljnigraphics    -landroid



ifeq ($(TARGET_ARCH_ABI), arm64-v8a)
    # 强制开启函数和数据段隔离，这是链接器插入跳板（Veneer）的前提
    LOCAL_CFLAGS += -ffunction-sections -fdata-sections
    LOCAL_LDFLAGS += -Wl,--gc-sections

    # 修正：通过 -Wl 传递 LLD 识别的参数
    # --fix-cortex-a53-843419 会强制 LLD 扫描所有分支并修复跳转范围
    LOCAL_LDFLAGS += -Wl,--fix-cortex-a53-843419
    
    # 手动指定存根组大小，注意格式：-Wl,前缀
    #LOCAL_LDFLAGS += -Wl,--stub-group-size=1048576
endif


LOCAL_MODULE := MediaConvert
LOCAL_SRC_FILES=  MediaConvert.cpp

LOCAL_STATIC_LIBRARIES +=  wxffmpeg opus  vpx webp   lame  openh264_decoder openh264_encoder openh264_processing  openh264_common 

LOCAL_LDLIBS += -llog -ldl -lz -lm -pthread -ljnigraphics    -landroid


LOCAL_LDFLAGS += -Wl,-z,max-page-size=16384 -Wl,-z,common-page-size=16384

include $(BUILD_SHARED_LIBRARY)