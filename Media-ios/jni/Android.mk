#Video
include $(APP_PROJECT_PATH)/ffmpeg/Android.mk
include $(APP_PROJECT_PATH)/x264/Android.mk
include $(APP_PROJECT_PATH)/libyuv/Android.mk

#Audio
include $(APP_PROJECT_PATH)/mediastreamer2/Android.mk
include $(APP_PROJECT_PATH)/opus/Android.mk
include $(APP_PROJECT_PATH)/ortp/Android.mk
include $(APP_PROJECT_PATH)/ms2/Android.mk  
include $(APP_PROJECT_PATH)/webrtc/Android.mk  