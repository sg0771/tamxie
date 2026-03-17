APP_PROJECT_PATH := $(call my-dir)/..
ffmpeg-root-dir:=$(APP_PROJECT_PATH)
APP_STL := c++_static
APP_PLATFORM := android-17
APP_ABI =armeabi armeabi-v7a  arm64-v8a x86 x86_64
APP_CPPFLAGS += -std=c++11
APP_CPPFLAGS += -fexceptions  
APP_CPPFLAGS += -frtti  