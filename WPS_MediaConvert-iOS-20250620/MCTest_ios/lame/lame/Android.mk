LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE :=lame
LOCAL_ARM_MODE := arm
LOCAL_C_INCLUDES +=$(LOCAL_PATH) 

LOCAL_CFLAGS :=-std=gnu99  -DSTDC_HEADERS=1

LOCAL_SRC_FILES:=VbrTag.c \
bitstream.c \
encoder.c \
fft.c \
gain_analysis.c \
id3tag.c \
lame.c \
mpglib_interface.c \
newmdct.c \
presets.c \
psymodel.c \
quantize.c \
quantize_pvt.c \
reservoir.c \
set_get.c \
tables.c \
takehiro.c \
util.c \
vbrquantize.c \
version.c \


include $(BUILD_STATIC_LIBRARY)