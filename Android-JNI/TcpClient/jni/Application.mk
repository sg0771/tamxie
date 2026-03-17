APP_PROJECT_PATH := $(call my-dir)/..

ffmpeg-root-dir:=$(APP_PROJECT_PATH)

#APP_STL := c++_static
#APP_STL := stlport_static
APP_STL := gnustl_static

APP_SHORT_COMMANDS := true

APP_PLATFORM := android-21

#APP_ABI =armeabi-v7a  arm64-v8a  

APP_ABI=armeabi-v7a  arm64-v8a  


APP_CPPFLAGS += -std=c++11

APP_CPPFLAGS += -fexceptions  

APP_CPPFLAGS += -frtti  

NDK_TOOLCHAIN_VERSION = 4.9