#
# Copyright (c) 2013 Bilibili
# Copyright (c) 2013 Zhang Rui <bbcallen@gmail.com>
#
# This file is part of ijkPlayer.
#
# ijkPlayer is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# ijkPlayer is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with ijkPlayer; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
# -mfloat-abi=soft is a workaround for FP register corruption on Exynos 4210
# http://www.spinics.net/lists/arm-kernel/msg368417.html
ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
LOCAL_CFLAGS += -mfloat-abi=soft
endif
LOCAL_CFLAGS += -std=c99
LOCAL_LDLIBS += -llog -landroid

ifeq ($(TARGET_ARCH_ABI), arm64-v8a)
	LOCAL_ASMFLAGS+= -DFFMPEG_ARM64
	LOCAL_CFLAGS +=  -DFFMPEG_ARM64
else  ifeq ($(TARGET_ARCH_ABI), armeabi-v7a)
	LOCAL_ASMFLAGS   += -DFFMPEG_ARMV7
	LOCAL_CFLAGS     += -DFFMPEG_ARMV7
else  ifeq ($(TARGET_ARCH_ABI), armeabi)
	LOCAL_ASMFLAGS   += -DFFMPEG_ARMV5
	LOCAL_CFLAGS     +=  -DFFMPEG_ARMV5
else  ifeq ($(TARGET_ARCH_ABI), x86_64)
	LOCAL_ASMFLAGS   +=  -DFFMPEG_X64  
	LOCAL_CFLAGS     +=  -DFFMPEG_X64 
else  ifeq ($(TARGET_ARCH_ABI), x86)
	LOCAL_ASMFLAGS+= -DFFMPEG_X86
	LOCAL_CFLAGS +=  -DFFMPEG_X86
endif

LOCAL_ASMFLAGS += -DPIC=1  -fPIC  
LOCAL_CFLAGS += -DPIC=1  -fomit-frame-pointer -fPIC -D_ISOC99_SOURCE -D_FILE_OFFSET_BITS=64 
LOCAL_CFLAGS += -D_LARGEFILE_SOURCE -O3 -fno-math-errno -fno-signed-zeros -fno-tree-vectorize  -D__ANDROID=1 -Wno-deprecated-declarations  -fpermissive 
LOCAL_C_INCLUDES += $(ffmpeg-root-dir)/ffmpeg
LOCAL_C_INCLUDES += $(ffmpeg-root-dir)/ijkyuv/include

ifeq ($(TARGET_ARCH_ABI), armeabi-v7a)
LOCAL_LDLIBS += -Wl,--no-warn-shared-textrel
endif



LOCAL_C_INCLUDES += $(LOCAL_PATH)
LOCAL_C_INCLUDES += $(realpath $(LOCAL_PATH)/..)
LOCAL_C_INCLUDES += $(realpath $(LOCAL_PATH)/../ffmpeg)
LOCAL_C_INCLUDES += $(realpath $(LOCAL_PATH)/../ijkj4a)

LOCAL_SRC_FILES += ff_cmdutils.c
LOCAL_SRC_FILES += ff_ffplay.c
LOCAL_SRC_FILES += ff_ffpipeline.c
LOCAL_SRC_FILES += ff_ffpipenode.c
LOCAL_SRC_FILES += ijkmeta.c
LOCAL_SRC_FILES += ijkplayer.c

LOCAL_SRC_FILES += pipeline/ffpipeline_ffplay.c
LOCAL_SRC_FILES += pipeline/ffpipenode_ffplay_vdec.c

LOCAL_SRC_FILES += android/ffmpeg_api_jni.c
LOCAL_SRC_FILES += android/ijkplayer_android.c
LOCAL_SRC_FILES += android/ijkplayer_jni.c

LOCAL_SRC_FILES += android/pipeline/ffpipeline_android.c
LOCAL_SRC_FILES += android/pipeline/ffpipenode_android_mediacodec_vdec.c

LOCAL_SRC_FILES += ijkavformat/allformats.c
LOCAL_SRC_FILES += ijkavformat/cJSON.c
LOCAL_SRC_FILES += ijkavformat/ijklas.c
LOCAL_SRC_FILES += ijkavformat/ijklivehook.c
LOCAL_SRC_FILES += ijkavformat/ijkmediadatasource.c
LOCAL_SRC_FILES += ijkavformat/ijkio.c
LOCAL_SRC_FILES += ijkavformat/ijkiomanager.c
LOCAL_SRC_FILES += ijkavformat/ijkiocache.c
LOCAL_SRC_FILES += ijkavformat/ijkioffio.c
LOCAL_SRC_FILES += ijkavformat/ijkioandroidio.c
LOCAL_SRC_FILES += ijkavformat/ijkioprotocol.c
LOCAL_SRC_FILES += ijkavformat/ijkioapplication.c
LOCAL_SRC_FILES += ijkavformat/ijkiourlhook.c

LOCAL_SRC_FILES  += ijkavformat/ijkasync.c
LOCAL_SRC_FILES  += ijkavformat/ijkurlhook.c
LOCAL_SRC_FILES  += ijkavformat/ijklongurl.c
LOCAL_SRC_FILES  += ijkavformat/ijksegment.c

LOCAL_SRC_FILES += ijkavutil/ijkdict.c
LOCAL_SRC_FILES += ijkavutil/ijkutils.c
LOCAL_SRC_FILES += ijkavutil/ijkthreadpool.c
LOCAL_SRC_FILES += ijkavutil/ijktree.c
LOCAL_SRC_FILES += ijkavutil/ijkfifo.c
LOCAL_SRC_FILES += ijkavutil/ijkstl.cpp

#LOCAL_SHARED_LIBRARIES := wxijkffmpeg ijksdl

LOCAL_SRC_FILES  += LibDelogo.cpp    

LOCAL_SHARED_LIBRARIES := wxffmpeg 
LOCAL_STATIC_LIBRARIES := ijksdl ijkj4a cpufeatures yuv_static  ijksoundtouch android-ndk-profiler

#LOCAL_MODULE := ijkplayer
LOCAL_MODULE := wxmedia

LOCAL_LDLIBS += -llog -landroid -lOpenSLES -lEGL -lGLESv2

#VERSION_SH  = $(LOCAL_PATH)/version.sh
#VERSION_H   = ijkversion.h
#$(info $(shell ($(VERSION_SH) $(LOCAL_PATH) $(VERSION_H))))
include $(BUILD_SHARED_LIBRARY)
$(call import-module,android/cpufeatures)