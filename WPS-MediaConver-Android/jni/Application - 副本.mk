APP_PROJECT_PATH := $(call my-dir)/..

ffmpeg-root-dir:=$(APP_PROJECT_PATH)

#APP_STL := c++_shared
APP_STL := c++_static

APP_SHORT_COMMANDS := true

APP_PLATFORM := android-24

APP_ABI=  armeabi-v7a  arm64-v8a  

APP_CPPFLAGS += -std=c++11

APP_CPPFLAGS += -fexceptions  

APP_CPPFLAGS += -frtti  


# 强制使用 GNU 工具链（核心：指定 GNU as/ld）
NDK_TOOLCHAIN_VERSION := 4.9  # NDK r25b 保留的 GNU 工具链版本