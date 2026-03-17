LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm
LOCAL_MODULE := webrtc

LOCAL_CFLAGS := -DWEBRTC_ANDROID \
    -DWEBRTC_LINUX \
    -DWEBRTC_CLOCK_TYPE_REALTIME \
    -DWEBRTC_ARCH_ARM

LOCAL_MODULE_TAGS := optional
LOCAL_CPP_EXTENSION := .cc

ifeq ($(TARGET_ARCH_ABI), armeabi-v7a)
LOCAL_CFLAGS += -DWEBRTC_DETECT_ARM_NEON
LOCAL_CFLAGS +=-mfpu=neon  -mfloat-abi=softfp  -flax-vector-conversions
endif

#Audio signal processing
LOCAL_C_INCLUDES := $(LOCAL_PATH)
#LOCAL_C_INCLUDES += $(LOCAL_PATH)/common_audio/signal_processing/include 


LOCAL_SRC_FILES := common_audio/signal_processing/complex_fft.c \
    common_audio/signal_processing/cross_correlation.c \
    common_audio/signal_processing/division_operations.c \
    common_audio/signal_processing/downsample_fast.c \
    common_audio/signal_processing/min_max_operations.c \
    common_audio/signal_processing/randomization_functions.c \
    common_audio/signal_processing/real_fft.c \
    common_audio/signal_processing/spl_init.c \
    common_audio/signal_processing/vector_scaling_operations.c

ifeq ($(TARGET_ARCH),arm)
LOCAL_SRC_FILES += common_audio/signal_processing/complex_bit_reverse_arm.s 
LOCAL_SRC_FILES += common_audio/signal_processing/spl_sqrt_floor_arm.s
else
LOCAL_SRC_FILES += common_audio/signal_processing/complex_bit_reverse.c 
LOCAL_SRC_FILES += common_audio/signal_processing/spl_sqrt_floor.c
endif


#AECM
LOCAL_C_INCLUDES += $(LOCAL_PATH)/modules/audio_processing/aecm/include
LOCAL_SRC_FILES += modules/audio_processing/aecm/echo_control_mobile.c   
LOCAL_SRC_FILES += modules/audio_processing/aecm/aecm_core.c

#APM
LOCAL_C_INCLUDES += $(LOCAL_PATH)/modules/audio_processing/utility
LOCAL_SRC_FILES += modules/audio_processing/utility/ring_buffer.c 
LOCAL_SRC_FILES += modules/audio_processing/utility/delay_estimator.c 
LOCAL_SRC_FILES += modules/audio_processing/utility/delay_estimator_wrapper.c

#system
LOCAL_C_INCLUDES += $(LOCAL_PATH)/system_wrappers/interface
LOCAL_SRC_FILES +=  system_wrappers/source/android/cpu-features.c \
    system_wrappers/source/cpu_features_android.c \
    system_wrappers/source/map.cc \
    system_wrappers/source/sort.cc \
    system_wrappers/source/aligned_malloc.cc \
    system_wrappers/source/atomic32_posix.cc \
    system_wrappers/source/condition_variable.cc \
    system_wrappers/source/cpu_no_op.cc \
    system_wrappers/source/cpu_features.cc \
    system_wrappers/source/cpu_info.cc \
    system_wrappers/source/critical_section.cc \
    system_wrappers/source/event.cc \
    system_wrappers/source/file_impl.cc \
    system_wrappers/source/list_no_stl.cc \
    system_wrappers/source/rw_lock.cc \
    system_wrappers/source/thread.cc \
    system_wrappers/source/trace_impl.cc \
    system_wrappers/source/condition_variable_posix.cc \
    system_wrappers/source/cpu_linux.cc \
    system_wrappers/source/critical_section_posix.cc \
    system_wrappers/source/event_posix.cc \
    system_wrappers/source/sleep.cc \
    system_wrappers/source/thread_posix.cc \
    system_wrappers/source/trace_posix.cc \
    system_wrappers/source/rw_lock_posix.cc 


ifeq ($(TARGET_ARCH_ABI), armeabi-v7a)
LOCAL_SRC_FILES += common_audio/signal_processing/cross_correlation_neon.s 
LOCAL_SRC_FILES += common_audio/signal_processing/downsample_fast_neon.s 
LOCAL_SRC_FILES += common_audio/signal_processing/min_max_operations_neon.s 
LOCAL_SRC_FILES += common_audio/signal_processing/vector_scaling_operations_neon.s  
LOCAL_SRC_FILES += modules/audio_processing/aecm/aecm_core_neon.S
endif

ifndef NDK_ROOT
include external/stlport/libstlport.mk
endif

include $(BUILD_STATIC_LIBRARY)