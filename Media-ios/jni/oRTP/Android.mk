LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := libortp

LOCAL_SRC_FILES := \
	src/str_utils.c	\
	src/port.c \
	src/logging.c 

LOCAL_CFLAGS += \
	-DORTP_INET6 \
	-UHAVE_CONFIG_H \
	-include ../ortp_AndroidConfig.h

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH) \
	$(LOCAL_PATH)/include \

include $(BUILD_STATIC_LIBRARY)