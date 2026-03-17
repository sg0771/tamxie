#APP_STL := stlport_static
APP_STL := c++_static
#APP_SHORT_COMMANDS := true
APP_PLATFORM := android-16
APP_ABI =arm64-v8a armeabi-v7a  x86_64 x86 armeabi
APP_CPPFLAGS += -std=c++11
APP_CPPFLAGS += -fexceptions  
APP_CPPFLAGS += -frtti  
NDK_TOOLCHAIN_VERSION = 4.9