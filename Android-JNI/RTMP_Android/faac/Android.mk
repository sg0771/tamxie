LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE :=libfaac
LOCAL_ARM_MODE := arm
LOCAL_C_INCLUDES +=	$(LOCAL_PATH)

LOCAL_CFLAGS :=-DPIC=1 -fPIC   -D_FILE_OFFSET_BITS=64  
LOCAL_CPP_FEATURES += exceptions

LOCAL_SRC_FILES:=aacquant.c  \
backpred.c  \
bitstream.c  \
channels.c  \
fft.c  \
filtbank.c  \
frame.c  \
huffman.c  \
ltp.c  \
midside.c  \
psychkni.c  \
tns.c  \
util.c  \


#LOCAL_LDLIBS +=  -lz 

include $(BUILD_STATIC_LIBRARY)
