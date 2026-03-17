APP_PROJECT_PATH := $(call my-dir)/..
root-dir:=$(APP_PROJECT_PATH)
#APP_STL := stlport_static
APP_STL := c++_static
#APP_SHORT_COMMANDS := true
APP_PLATFORM := android-16
APP_ABI =arm64-v8a armeabi-v7a   
APP_CPPFLAGS += -std=c++11
APP_CPPFLAGS += -fexceptions  
APP_CPPFLAGS += -frtti  
NDK_TOOLCHAIN_VERSION = 4.9