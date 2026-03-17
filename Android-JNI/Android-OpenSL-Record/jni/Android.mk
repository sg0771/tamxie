LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := SLRecord

LOCAL_C_INCLUDES := $(LOCAL_PATH)/include \
$(LOCAL_PATH)/aec \
$(LOCAL_PATH)/aecm \
$(LOCAL_PATH)/agc \
$(LOCAL_PATH)/ns \
$(LOCAL_PATH)/vad \
$(LOCAL_PATH)/libfaac \
$(LOCAL_PATH)/Depends

LOCAL_SRC_FILES := aec/echo_cancellation.c \
aec/aec_resampler.c \
aec/aec_rdft.c \
aec/aec_core.c \
aecm/aecm_core.c  \
aecm/echo_control_mobile.c  \
agc/analog_agc.c \
agc/digital_agc.c \
ns/noise_suppression.c \
ns/noise_suppression_x.c \
ns/ns_core.c \
ns/nsx_core.c \
vad/vad_core.c \
vad/vad_filterbank.c \
vad/vad_gmm.c \
vad/vad_sp.c \
vad/webrtc_vad.c \
Depends/complex_bit_reverse.c \
Depends/complex_fft.c \
Depends/copy_set_operations.c \
Depends/delay_estimator.c \
Depends/delay_estimator_wrapper.c \
Depends/division_operations.c \
Depends/dot_product_with_scale.c \
Depends/energy.c \
Depends/fft4g.c \
Depends/get_scaling_square.c \
Depends/min_max_operations.c \
Depends/randomization_functions.c \
Depends/resample_by_2.c \
Depends/ring_buffer.c \
Depends/spl_sqrt.c \
Depends/spl_sqrt_floor.c \
Depends/cpu_features.cc \
gips.cpp \
SLRecord.cpp


LOCAL_SRC_FILES+=       \
		libfaac/aacquant.c      \
		 libfaac/backpred.c      \
		 libfaac/bitstream.c     \
		 libfaac/channels.c      \
		 libfaac/fft.c           \
		 libfaac/filtbank.c      \
		 libfaac/frame.c         \
		 libfaac/huffman.c       \
		 libfaac/ltp.c           \
		 libfaac/midside.c       \
		 libfaac/psychkni.c      \
		 libfaac/tns.c           \
		 libfaac/util.c
		
LOCAL_LDLIBS    += -llog   
LOCAL_LDLIBS    += -lOpenSLES

include $(BUILD_SHARED_LIBRARY)


