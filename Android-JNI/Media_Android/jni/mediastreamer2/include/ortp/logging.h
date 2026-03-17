#ifndef ORTP_LOGGING_H
#define ORTP_LOGGING_H

#include <android/log.h>
#define TAG1 "ortp-"
#define ortp_debug(...) __android_log_print(ANDROID_LOG_DEBUG,TAG 1,__VA_ARGS__)
#define ortp_message(...) __android_log_print(ANDROID_LOG_INFO,TAG 1,__VA_ARGS__)
#define ortp_warning(...) __android_log_print(ANDROID_LOG_WARN,TAG1 ,__VA_ARGS__)
#define ortp_error(...) __android_log_print(ANDROID_LOG_ERROR,TAG1 ,__VA_ARGS__)
#define ortp_fatal(...) __android_log_print(ANDROID_LOG_FATAL,TAG1 ,__VA_ARGS__)

#endif
