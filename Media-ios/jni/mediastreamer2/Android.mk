LOCAL_PATH:= $(call my-dir)/src
include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH) \
	$(LOCAL_PATH)/../include \
	$(LOCAL_PATH)/../../oRTP/include \
	$(LOCAL_PATH)/../../speex/include \
	$(LOCAL_PATH)/../../opus/include \
	$(LOCAL_PATH)/../../webrtc
		
LOCAL_CFLAGS += -DV2_PHONE_SUPPORT=1
				
LOCAL_MODULE := libmediastreamer2

LOCAL_SRC_FILES :=audio_mixer.c \
audio_onSend.c \
base_mscommon.c \
base_msfilter.c \
base_msqueue.c \
base_mssndcard.c \
base_msticker.c \
msjava.c \
msvoip.c  msopus.c\
	
LOCAL_SRC_FILES+=androidsound_opensles.cpp opus_impl.c

LOCAL_CFLAGS += \
	-UHAVE_CONFIG_H \
	-include $(LOCAL_PATH)/../libmediastreamer2_AndroidConfig.h \
	-DMS2_INTERNAL \
	-DMS2_FILTERS \
	-DINET6 \
        -DORTP_INET6 \
	-D_POSIX_SOURCE -Wall 
LOCAL_CFLAGS += -DUSE_HARDWARE_RATE=1  -Wno-strict-aliasing
LOCAL_CXXFLAGS += -std=c++11
LOCAL_STATIC_LIBRARIES += cpufeatures
include $(BUILD_STATIC_LIBRARY)
$(call import-module,android/cpufeatures)