

#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <assert.h>

#include <stdint.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "WXResampleApi.h"

#define AVERROR(e) (-(e))   ///< Returns a negative error code from a POSIX error code, to return from library functions.

#define AV_CH_FRONT_LEFT             0x00000001
#define AV_CH_FRONT_RIGHT            0x00000002
#define AV_CH_FRONT_CENTER           0x00000004
#define AV_CH_LOW_FREQUENCY          0x00000008
#define AV_CH_BACK_LEFT              0x00000010
#define AV_CH_BACK_RIGHT             0x00000020
#define AV_CH_FRONT_LEFT_OF_CENTER   0x00000040
#define AV_CH_FRONT_RIGHT_OF_CENTER  0x00000080
#define AV_CH_BACK_CENTER            0x00000100
#define AV_CH_SIDE_LEFT              0x00000200
#define AV_CH_SIDE_RIGHT             0x00000400
#define AV_CH_TOP_CENTER             0x00000800
#define AV_CH_TOP_FRONT_LEFT         0x00001000
#define AV_CH_TOP_FRONT_CENTER       0x00002000
#define AV_CH_TOP_FRONT_RIGHT        0x00004000
#define AV_CH_TOP_BACK_LEFT          0x00008000
#define AV_CH_TOP_BACK_CENTER        0x00010000
#define AV_CH_TOP_BACK_RIGHT         0x00020000
#define AV_CH_STEREO_LEFT            0x20000000  ///< Stereo downmix.
#define AV_CH_STEREO_RIGHT           0x40000000  ///< See AV_CH_STEREO_LEFT.
#define AV_CH_WIDE_LEFT              0x0000000080000000ULL
#define AV_CH_WIDE_RIGHT             0x0000000100000000ULL
#define AV_CH_SURROUND_DIRECT_LEFT   0x0000000200000000ULL
#define AV_CH_SURROUND_DIRECT_RIGHT  0x0000000400000000ULL
#define AV_CH_LOW_FREQUENCY_2        0x0000000800000000ULL

#define AV_CH_LAYOUT_NATIVE          0x8000000000000000ULL

#define AV_CH_LAYOUT_MONO              (AV_CH_FRONT_CENTER)
#define AV_CH_LAYOUT_STEREO            (AV_CH_FRONT_LEFT|AV_CH_FRONT_RIGHT)
#define AV_CH_LAYOUT_2POINT1           (AV_CH_LAYOUT_STEREO|AV_CH_LOW_FREQUENCY)
#define AV_CH_LAYOUT_2_1               (AV_CH_LAYOUT_STEREO|AV_CH_BACK_CENTER)
#define AV_CH_LAYOUT_SURROUND          (AV_CH_LAYOUT_STEREO|AV_CH_FRONT_CENTER)
#define AV_CH_LAYOUT_3POINT1           (AV_CH_LAYOUT_SURROUND|AV_CH_LOW_FREQUENCY)
#define AV_CH_LAYOUT_4POINT0           (AV_CH_LAYOUT_SURROUND|AV_CH_BACK_CENTER)
#define AV_CH_LAYOUT_4POINT1           (AV_CH_LAYOUT_4POINT0|AV_CH_LOW_FREQUENCY)
#define AV_CH_LAYOUT_2_2               (AV_CH_LAYOUT_STEREO|AV_CH_SIDE_LEFT|AV_CH_SIDE_RIGHT)
#define AV_CH_LAYOUT_QUAD              (AV_CH_LAYOUT_STEREO|AV_CH_BACK_LEFT|AV_CH_BACK_RIGHT)
#define AV_CH_LAYOUT_5POINT0           (AV_CH_LAYOUT_SURROUND|AV_CH_SIDE_LEFT|AV_CH_SIDE_RIGHT)
#define AV_CH_LAYOUT_5POINT1           (AV_CH_LAYOUT_5POINT0|AV_CH_LOW_FREQUENCY)
#define AV_CH_LAYOUT_5POINT0_BACK      (AV_CH_LAYOUT_SURROUND|AV_CH_BACK_LEFT|AV_CH_BACK_RIGHT)
#define AV_CH_LAYOUT_5POINT1_BACK      (AV_CH_LAYOUT_5POINT0_BACK|AV_CH_LOW_FREQUENCY)
#define AV_CH_LAYOUT_6POINT0           (AV_CH_LAYOUT_5POINT0|AV_CH_BACK_CENTER)
#define AV_CH_LAYOUT_6POINT0_FRONT     (AV_CH_LAYOUT_2_2|AV_CH_FRONT_LEFT_OF_CENTER|AV_CH_FRONT_RIGHT_OF_CENTER)
#define AV_CH_LAYOUT_HEXAGONAL         (AV_CH_LAYOUT_5POINT0_BACK|AV_CH_BACK_CENTER)
#define AV_CH_LAYOUT_6POINT1           (AV_CH_LAYOUT_5POINT1|AV_CH_BACK_CENTER)
#define AV_CH_LAYOUT_6POINT1_BACK      (AV_CH_LAYOUT_5POINT1_BACK|AV_CH_BACK_CENTER)
#define AV_CH_LAYOUT_6POINT1_FRONT     (AV_CH_LAYOUT_6POINT0_FRONT|AV_CH_LOW_FREQUENCY)
#define AV_CH_LAYOUT_7POINT0           (AV_CH_LAYOUT_5POINT0|AV_CH_BACK_LEFT|AV_CH_BACK_RIGHT)
#define AV_CH_LAYOUT_7POINT0_FRONT     (AV_CH_LAYOUT_5POINT0|AV_CH_FRONT_LEFT_OF_CENTER|AV_CH_FRONT_RIGHT_OF_CENTER)
#define AV_CH_LAYOUT_7POINT1           (AV_CH_LAYOUT_5POINT1|AV_CH_BACK_LEFT|AV_CH_BACK_RIGHT)
#define AV_CH_LAYOUT_7POINT1_WIDE      (AV_CH_LAYOUT_5POINT1|AV_CH_FRONT_LEFT_OF_CENTER|AV_CH_FRONT_RIGHT_OF_CENTER)
#define AV_CH_LAYOUT_7POINT1_WIDE_BACK (AV_CH_LAYOUT_5POINT1_BACK|AV_CH_FRONT_LEFT_OF_CENTER|AV_CH_FRONT_RIGHT_OF_CENTER)
#define AV_CH_LAYOUT_OCTAGONAL         (AV_CH_LAYOUT_5POINT0|AV_CH_BACK_LEFT|AV_CH_BACK_CENTER|AV_CH_BACK_RIGHT)
#define AV_CH_LAYOUT_STEREO_DOWNMIX    (AV_CH_STEREO_LEFT|AV_CH_STEREO_RIGHT)

enum WXTcpFormat {
    AV_SAMPLE_FMT_NONE = -1,
    AV_SAMPLE_FMT_S16,         ///< signed 16 bits
    AV_SAMPLE_FMT_FLT,         ///< float
    AV_SAMPLE_FMT_S16P,        ///< signed 16 bits, planar
    AV_SAMPLE_FMT_FLTP,        ///< float, planar
    AV_SAMPLE_FMT_NB           ///< Number of sample formats. DO NOT USE if linking dynamically
};

enum WXTcpFormat wx_get_packed_sample_fmt(enum WXTcpFormat sample_fmt);
enum WXTcpFormat wx_get_planar_sample_fmt(enum WXTcpFormat sample_fmt);
int wx_get_bytes_per_sample(enum WXTcpFormat sample_fmt);
int wx_sample_fmt_is_planar(enum WXTcpFormat sample_fmt);
int64_t wx_gcd(int64_t a, int64_t b);
int64_t wx_rescale(int64_t a, int64_t b, int64_t c);
int wx_get_channel_layout_nb_channels(uint64_t channel_layout);
int64_t wx_get_default_channel_layout(int nb_channels);

void* wx_malloc(size_t size);
void wx_free(void* ptr);
void* wx_mallocz(size_t size);
void* wx_calloc(size_t nmemb, size_t size);
void wx_freep(void* ptr);

int wx_reduce(int* dst_num, int* dst_den, int64_t num, int64_t den, int64_t max);
void* wx_malloc_array(size_t nmemb, size_t size);
int wx_clip(int a, int amin, int amax);
uint8_t wx_clip_uint8(int a);
int16_t wx_clip_int16(int a);
int32_t wx_clipl_int32(int64_t a);
float wx_clipf(float a, float amin, float amax);

#define SWR_CH_MAX 32   ///< Maximum number of channels
#define SWR_FLAG_RESAMPLE 1 ///< Force resampling even if equal sample rate

#define SQRT3_2      1.22474487139158904909  /* sqrt(3/2) */

#define NS_TAPS 20

#define AV_NOPTS_VALUE          ((int64_t)UINT64_C(0x8000000000000000))

#define FFMAX(a,b) ((a) > (b) ? (a) : (b))
#define FFMIN(a,b) ((a) > (b) ? (b) : (a))
#define FF_ARRAY_ELEMS(a) (sizeof(a) / sizeof((a)[0]))
#define FFALIGN(x, a) (((x)+(a)-1)&~((a)-1))

#ifndef M_LN10
#define M_LN10         2.30258509299404568402  /* log_e 10 */
#endif

#ifndef M_PI
#define M_PI           3.14159265358979323846  /* pi */
#endif

#ifndef M_SQRT1_2
#define M_SQRT1_2      0.70710678118654752440  /* 1/sqrt(2) */
#endif

typedef int integer;

typedef void (mix_1_1_func_type)(void* out, const void* in, void* coeffp, integer index, integer len);
typedef void (mix_2_1_func_type)(void* out, const void* in1, const void* in2, void* coeffp, integer index1, integer index2, integer len);

typedef void (mix_any_func_type)(uint8_t** out, const uint8_t** in1, void* coeffp, integer len);

typedef struct AudioData {
    uint8_t* ch[SWR_CH_MAX];    ///< samples buffer per channel
    uint8_t* data;              ///< samples buffer
    int ch_count;               ///< number of channels
    int bps;                    ///< bytes per sample
    int count;                  ///< number of samples
    int planar;                 ///< 1 if planar audio, 0 otherwise
    enum WXTcpFormat fmt;    ///< sample format
} AudioData;


enum SwrDitherType {
    SWR_DITHER_NONE = 0,
    SWR_DITHER_RECTANGULAR,
    SWR_DITHER_TRIANGULAR,
    SWR_DITHER_TRIANGULAR_HIGHPASS,

    SWR_DITHER_NS = 64,         ///< not part of API/ABI
    SWR_DITHER_NS_LIPSHITZ,
    SWR_DITHER_NS_F_WEIGHTED,
    SWR_DITHER_NS_MODIFIED_E_WEIGHTED,
    SWR_DITHER_NS_IMPROVED_E_WEIGHTED,
    SWR_DITHER_NS_SHIBATA,
    SWR_DITHER_NS_LOW_SHIBATA,
    SWR_DITHER_NS_HIGH_SHIBATA,
    SWR_DITHER_NB,              ///< not part of API/ABI
};
struct DitherContext {
    enum SwrDitherType method;
    int noise_pos;
    float scale;
    float noise_scale;                              ///< Noise scale
    int ns_taps;                                    ///< Noise shaping dither taps
    float ns_scale;                                 ///< Noise shaping dither scale
    float ns_scale_1;                               ///< Noise shaping dither scale^-1
    int ns_pos;                                     ///< Noise shaping dither position
    float ns_coeffs[NS_TAPS];                       ///< Noise shaping filter coefficients
    float ns_errors[SWR_CH_MAX][2 * NS_TAPS];
    AudioData noise;                                ///< noise used for dithering
    AudioData temp;                                 ///< temporary storage when writing into the input buffer isn't possible
    int output_sample_bits;                         ///< the number of used output bits, needed to scale dither correctly
};

enum AVMatrixEncoding {
    AV_MATRIX_ENCODING_NONE,
    AV_MATRIX_ENCODING_DOLBY,
    AV_MATRIX_ENCODING_DPLII,
    AV_MATRIX_ENCODING_DPLIIX,
    AV_MATRIX_ENCODING_DPLIIZ,
    AV_MATRIX_ENCODING_DOLBYEX,
    AV_MATRIX_ENCODING_DOLBYHEADPHONE,
    AV_MATRIX_ENCODING_NB
};

/** Resampling Filter Types */
enum SwrFilterType {
    SWR_FILTER_TYPE_CUBIC,              /**< Cubic */
    SWR_FILTER_TYPE_BLACKMAN_NUTTALL,   /**< Blackman Nuttall Windowed Sinc */
    SWR_FILTER_TYPE_KAISER,             /**< Kaiser Windowed Sinc */
};

struct WXTcpSwrContext {
    enum WXTcpFormat  in_sample_fmt;             ///< input sample format
    enum WXTcpFormat int_sample_fmt;             ///< internal sample format (AV_SAMPLE_FMT_FLTP or AV_SAMPLE_FMT_S16P)
    enum WXTcpFormat out_sample_fmt;             ///< output sample format
    int64_t  in_ch_layout;                          ///< input channel layout
    int64_t out_ch_layout;                          ///< output channel layout
    int      in_sample_rate;                        ///< input sample rate
    int     out_sample_rate;                        ///< output sample rate
    int flags;                                      ///< miscellaneous flags such as SWR_FLAG_RESAMPLE
    float slev;                                     ///< surround mixing level
    float clev;                                     ///< center mixing level
    float lfe_mix_level;                            ///< LFE mixing level
    float rematrix_volume;                          ///< rematrixing volume coefficient
    float rematrix_maxval;                          ///< maximum value for rematrixing output
    enum AVMatrixEncoding matrix_encoding;          /**< matrixed stereo encoding */
    const int* channel_map;                         ///< channel index (or -1 if muted channel) map
    int used_ch_count;                              ///< number of used input channels (mapped channel count if channel_map, otherwise in.ch_count)

    struct DitherContext dither;

    int filter_size;                                /**< length of each FIR filter in the resampling filterbank relative to the cutoff frequency */
    int phase_shift;                                /**< log2 of the number of entries in the resampling polyphase filterbank */
    int linear_interp;                              /**< if 1 then the resampling FIR filter will be linearly interpolated */
    double cutoff;                                  /**< resampling cutoff frequency (swr: 6dB point; soxr: 0dB point). 1.0 corresponds to half the output sample rate */
    enum SwrFilterType filter_type;                 /**< swr resampling filter type */
    int kaiser_beta;                                /**< swr beta value for Kaiser window (only applicable if filter_type == AV_FILTER_TYPE_KAISER) */
    double precision;                               /**< soxr resampling precision (in bits) */
    int cheby;                                      /**< soxr: if 1 then passband rolloff will be none (Chebyshev) & irrational ratio approximation precision will be higher */

    float min_compensation;                         ///< swr minimum below which no compensation will happen
    float min_hard_compensation;                    ///< swr minimum below which no silence inject / sample drop will happen
    float soft_compensation_duration;               ///< swr duration over which soft compensation is applied
    float max_soft_compensation;                    ///< swr maximum soft compensation in seconds over soft_compensation_duration
    float async;                                    ///< swr simple 1 parameter async, similar to ffmpegs -async
    int64_t firstpts_in_samples;                    ///< swr first pts in samples

    int resample_first;                             ///< 1 if resampling must come first, 0 if rematrixing
    int rematrix;                                   ///< flag to indicate if rematrixing is needed (basically if input and output layouts mismatch)
    int rematrix_custom;                            ///< flag to indicate that a custom matrix has been defined

    AudioData in;                                   ///< input audio data
    AudioData postin;                               ///< post-input audio data: used for rematrix/resample
    AudioData midbuf;                               ///< intermediate audio data (postin/preout)
    AudioData preout;                               ///< pre-output audio data: used for rematrix/resample
    AudioData out;                                  ///< converted output audio data
    AudioData in_buffer;                            ///< cached audio data (convert and resample purpose)
    AudioData silence;                              ///< temporary with silence
    AudioData drop_temp;                            ///< temporary used to discard output
    int in_buffer_index;                            ///< cached buffer position
    int in_buffer_count;                            ///< cached buffer length
    int resample_in_constraint;                     ///< 1 if the input end was reach before the output end, 0 otherwise
    int flushed;                                    ///< 1 if data is to be flushed and no further input is expected
    int64_t outpts;                                 ///< output PTS
    int64_t firstpts;                               ///< first PTS
    int drop_output;                                ///< number of output samples to drop

    struct AudioConvert* in_convert;                ///< input conversion context
    struct AudioConvert* out_convert;               ///< output conversion context
    struct AudioConvert* full_convert;              ///< full conversion context (single conversion for input and output)
    struct ResampleContext* resample;               ///< resampling context
    struct Resampler const* resampler;              ///< resampler virtual function table

    float matrix[SWR_CH_MAX][SWR_CH_MAX];           ///< floating point rematrixing coefficients
    uint8_t* native_matrix;
    uint8_t* native_one;
    uint8_t* native_simd_one;
    uint8_t* native_simd_matrix;
    int32_t matrix32[SWR_CH_MAX][SWR_CH_MAX];       ///< 17.15 fixed point rematrixing coefficients
    uint8_t matrix_ch[SWR_CH_MAX][SWR_CH_MAX + 1];    ///< Lists of input channels per output channel that have non zero rematrixing coefficients
    mix_1_1_func_type* mix_1_1_f;
    mix_1_1_func_type* mix_1_1_simd;

    mix_2_1_func_type* mix_2_1_f;
    mix_2_1_func_type* mix_2_1_simd;

    mix_any_func_type* mix_any_f;

    /* TODO: callbacks for ASM optimizations */
};

typedef struct ResampleContext* (*resample_init_func)(struct ResampleContext* c, int out_rate, int in_rate, int filter_size, int phase_shift, int linear,
    double cutoff, enum WXTcpFormat format, enum SwrFilterType filter_type, int kaiser_beta, double precision, int cheby);
typedef void    (*resample_free_func)(struct ResampleContext** c);
typedef int     (*multiple_resample_func)(struct ResampleContext* c, AudioData* dst, int dst_size, AudioData* src, int src_size, int* consumed);
typedef int     (*resample_flush_func)(struct WXTcpSwrContext* c);
typedef int     (*set_compensation_func)(struct ResampleContext* c, int sample_delta, int compensation_distance);
typedef int64_t(*get_delay_func)(struct WXTcpSwrContext* s, int64_t base);
typedef int     (*invert_initial_buffer_func)(struct ResampleContext* c, AudioData* dst, const AudioData* src, int src_size, int* dst_idx, int* dst_count);

struct Resampler {
    resample_init_func            init;
    resample_free_func            free;
    multiple_resample_func        multiple_resample;
    resample_flush_func           flush;
    set_compensation_func         set_compensation;
    get_delay_func                get_delay;
    invert_initial_buffer_func    invert_initial_buffer;
};

extern struct Resampler const swri_resampler;

int swri_realloc_audio(AudioData* a, int count);

typedef struct WXTcpSwrContext WXTcpSwrContext;
void swri_noise_shaping_int16(WXTcpSwrContext* s, AudioData* dsts, const AudioData* srcs, const AudioData* noises, int count);
void swri_noise_shaping_float(WXTcpSwrContext* s, AudioData* dsts, const AudioData* srcs, const AudioData* noises, int count);

int swri_rematrix_init(WXTcpSwrContext* s);
void swri_rematrix_free(WXTcpSwrContext* s);
int swri_rematrix(WXTcpSwrContext* s, AudioData* out, AudioData* in, int len, int mustcopy);

void swri_get_dither(WXTcpSwrContext* s, void* dst, int len, unsigned seed, enum WXTcpFormat noise_fmt);
int swri_dither_init(WXTcpSwrContext* s, enum WXTcpFormat out_fmt, enum WXTcpFormat in_fmt);

typedef struct ResampleContext {

    uint8_t* filter_bank;
    int filter_length;
    int filter_alloc;
    int ideal_dst_incr;
    int dst_incr;
    int dst_incr_div;
    int dst_incr_mod;
    int index;
    int frac;
    int src_incr;
    int compensation_distance;
    int phase_shift;
    int phase_mask;
    int linear;
    enum SwrFilterType filter_type;
    int kaiser_beta;
    double factor;
    enum WXTcpFormat format;
    int felem_size;
    int filter_shift;

    struct {
        void(*resample_one)(void* dst, const void* src,
            int n, int64_t index, int64_t incr);
        int(*resample)(struct ResampleContext* c, void* dst,
            const void* src, int n, int update_ctx);
    } dsp;
} ResampleContext;

void swri_resample_dsp_init(ResampleContext* c);


typedef void (conv_func_type)(uint8_t* po, const uint8_t* pi, int is, int os, uint8_t* end);
typedef void (simd_func_type)(uint8_t** dst, const uint8_t** src, int len);

typedef struct AudioConvert {
    int channels;
    int  in_simd_align_mask;
    int out_simd_align_mask;
    conv_func_type* conv_f;
    simd_func_type* simd_f;
    const int* ch_map;
    uint8_t silence[8]; ///< silence input sample
}AudioConvert;

AudioConvert* swri_audio_convert_alloc(enum WXTcpFormat out_fmt,
    enum WXTcpFormat in_fmt,
    int channels, const int* ch_map,
    int flags);


void swri_audio_convert_free(AudioConvert** ctx);
int swri_audio_convert(AudioConvert* ctx, AudioData* out, AudioData* in, int len);

/**
 * 0th order modified bessel function of the first kind.
 */
static double bessel(double x) {
    double v = 1;
    double lastv = 0;
    double t = 1;
    int i;
    static const double inv[100] = {
 1.0 / (1 * 1), 1.0 / (2 * 2), 1.0 / (3 * 3), 1.0 / (4 * 4), 1.0 / (5 * 5), 1.0 / (6 * 6), 1.0 / (7 * 7), 1.0 / (8 * 8), 1.0 / (9 * 9), 1.0 / (10 * 10),
 1.0 / (11 * 11), 1.0 / (12 * 12), 1.0 / (13 * 13), 1.0 / (14 * 14), 1.0 / (15 * 15), 1.0 / (16 * 16), 1.0 / (17 * 17), 1.0 / (18 * 18), 1.0 / (19 * 19), 1.0 / (20 * 20),
 1.0 / (21 * 21), 1.0 / (22 * 22), 1.0 / (23 * 23), 1.0 / (24 * 24), 1.0 / (25 * 25), 1.0 / (26 * 26), 1.0 / (27 * 27), 1.0 / (28 * 28), 1.0 / (29 * 29), 1.0 / (30 * 30),
 1.0 / (31 * 31), 1.0 / (32 * 32), 1.0 / (33 * 33), 1.0 / (34 * 34), 1.0 / (35 * 35), 1.0 / (36 * 36), 1.0 / (37 * 37), 1.0 / (38 * 38), 1.0 / (39 * 39), 1.0 / (40 * 40),
 1.0 / (41 * 41), 1.0 / (42 * 42), 1.0 / (43 * 43), 1.0 / (44 * 44), 1.0 / (45 * 45), 1.0 / (46 * 46), 1.0 / (47 * 47), 1.0 / (48 * 48), 1.0 / (49 * 49), 1.0 / (50 * 50),
 1.0 / (51 * 51), 1.0 / (52 * 52), 1.0 / (53 * 53), 1.0 / (54 * 54), 1.0 / (55 * 55), 1.0 / (56 * 56), 1.0 / (57 * 57), 1.0 / (58 * 58), 1.0 / (59 * 59), 1.0 / (60 * 60),
 1.0 / (61 * 61), 1.0 / (62 * 62), 1.0 / (63 * 63), 1.0 / (64 * 64), 1.0 / (65 * 65), 1.0 / (66 * 66), 1.0 / (67 * 67), 1.0 / (68 * 68), 1.0 / (69 * 69), 1.0 / (70 * 70),
 1.0 / (71 * 71), 1.0 / (72 * 72), 1.0 / (73 * 73), 1.0 / (74 * 74), 1.0 / (75 * 75), 1.0 / (76 * 76), 1.0 / (77 * 77), 1.0 / (78 * 78), 1.0 / (79 * 79), 1.0 / (80 * 80),
 1.0 / (81 * 81), 1.0 / (82 * 82), 1.0 / (83 * 83), 1.0 / (84 * 84), 1.0 / (85 * 85), 1.0 / (86 * 86), 1.0 / (87 * 87), 1.0 / (88 * 88), 1.0 / (89 * 89), 1.0 / (90 * 90),
 1.0 / (91 * 91), 1.0 / (92 * 92), 1.0 / (93 * 93), 1.0 / (94 * 94), 1.0 / (95 * 95), 1.0 / (96 * 96), 1.0 / (97 * 97), 1.0 / (98 * 98), 1.0 / (99 * 99), 1.0 / (10000)
    };

    x = x * x / 4;
    for (i = 0; v != lastv; i++) {
        lastv = v;
        t *= x * inv[i];
        v += t;
        assert(i < 99);
    }
    return v;
}

/**
 * builds a polyphase filterbank.
 * @param factor resampling factor
 * @param scale wanted sum of coefficients for each filter
 * @param filter_type  filter type
 * @param kaiser_beta  kaiser window beta
 * @return 0 on success, negative on error
 */
static int build_filter(ResampleContext* c, void* filter, double factor, int tap_count, int alloc, int phase_count, int scale,
    int filter_type, int kaiser_beta) {
    int ph, i;
    double x, y, w;
    double* tab = wx_malloc_array(tap_count, sizeof(*tab));
    const int center = (tap_count - 1) / 2;

    if (!tab)
        return AVERROR(ENOMEM);

    /* if upsampling, only need to interpolate, no filter */
    if (factor > 1.0)
        factor = 1.0;

    for (ph = 0; ph < phase_count; ph++) {
        double norm = 0;
        for (i = 0; i < tap_count; i++) {
            x = M_PI * ((double)(i - center) - (double)ph / phase_count) * factor;
            if (x == 0) y = 1.0;
            else        y = sin(x) / x;
            switch (filter_type) {
            case SWR_FILTER_TYPE_CUBIC: {
                const float d = -0.5; //first order derivative = -0.5
                x = fabs(((double)(i - center) - (double)ph / phase_count) * factor);
                if (x < 1.0) y = 1 - 3 * x * x + 2 * x * x * x + d * (-x * x + x * x * x);
                else      y = d * (-4 + 8 * x - 5 * x * x + x * x * x);
                break;
            }
            case SWR_FILTER_TYPE_BLACKMAN_NUTTALL:
                w = 2.0 * x / (factor * tap_count) + M_PI;
                y *= 0.3635819 - 0.4891775 * cos(w) + 0.1365995 * cos(2 * w) - 0.0106411 * cos(3 * w);
                break;
            case SWR_FILTER_TYPE_KAISER:
                w = 2.0 * x / (factor * tap_count * M_PI);
                y *= bessel(kaiser_beta * sqrt(FFMAX(1 - w * w, 0)));
                break;
            default:
                assert(0);
            }

            tab[i] = y;
            norm += y;
        }

        /* normalize so that an uniform color remains the same */
        switch (c->format) {
        case AV_SAMPLE_FMT_S16P:
            for (i = 0; i < tap_count; i++)
                ((int16_t*)filter)[ph * alloc + i] = wx_clip(lrintf(tab[i] * scale / norm), INT16_MIN, INT16_MAX);
            break;
        case AV_SAMPLE_FMT_FLTP:
            for (i = 0; i < tap_count; i++)
                ((float*)filter)[ph * alloc + i] = tab[i] * scale / norm;
            break;
        }
    }
    wx_free(tab);
    return 0;
}

static ResampleContext* resample_init(ResampleContext* c, int out_rate, int in_rate, int filter_size, int phase_shift, int linear,
    double cutoff0, enum WXTcpFormat format, enum SwrFilterType filter_type, int kaiser_beta,
    double precision, int cheby)
{
    double cutoff = cutoff0 ? cutoff0 : 0.97;
    double factor = FFMIN(out_rate * cutoff / in_rate, 1.0);
    int phase_count = 1 << phase_shift;

    if (!c || c->phase_shift != phase_shift || c->linear != linear || c->factor != factor
        || c->filter_length != FFMAX((int)ceil(filter_size / factor), 1) || c->format != format
        || c->filter_type != filter_type || c->kaiser_beta != kaiser_beta) {
        c = wx_mallocz(sizeof(*c));
        if (!c)
            return NULL;

        c->format = format;

        c->felem_size = wx_get_bytes_per_sample(c->format);

        switch (c->format) {
        case AV_SAMPLE_FMT_S16P:
            c->filter_shift = 15;
            break;
        case AV_SAMPLE_FMT_FLTP:
            c->filter_shift = 0;
            break;
        default:
            // wx_log(NULL, AV_LOG_ERROR, "Unsupported sample format\n");
            assert(0);
        }

        if (filter_size / factor > INT32_MAX / 256) {
            // wx_log(NULL, AV_LOG_ERROR, "Filter length too large\n");
            goto error;
        }

        c->phase_shift = phase_shift;
        c->phase_mask = phase_count - 1;
        c->linear = linear;
        c->factor = factor;
        c->filter_length = FFMAX((int)ceil(filter_size / factor), 1);
        c->filter_alloc = FFALIGN(c->filter_length, 8);
        c->filter_bank = wx_calloc(c->filter_alloc, (phase_count + 1) * c->felem_size);
        c->filter_type = filter_type;
        c->kaiser_beta = kaiser_beta;
        if (!c->filter_bank)
            goto error;
        if (build_filter(c, (void*)c->filter_bank, factor, c->filter_length, c->filter_alloc, phase_count, 1 << c->filter_shift, filter_type, kaiser_beta))
            goto error;
        memcpy(c->filter_bank + (c->filter_alloc * phase_count + 1) * c->felem_size, c->filter_bank, (c->filter_alloc - 1) * c->felem_size);
        memcpy(c->filter_bank + (c->filter_alloc * phase_count) * c->felem_size, c->filter_bank + (c->filter_alloc - 1) * c->felem_size, c->felem_size);
    }

    c->compensation_distance = 0;
    if (!wx_reduce(&c->src_incr, &c->dst_incr, out_rate, in_rate * (int64_t)phase_count, INT32_MAX / 2))
        goto error;
    c->ideal_dst_incr = c->dst_incr;
    c->dst_incr_div = c->dst_incr / c->src_incr;
    c->dst_incr_mod = c->dst_incr % c->src_incr;

    c->index = -phase_count * ((c->filter_length - 1) / 2);
    c->frac = 0;

    swri_resample_dsp_init(c);

    return c;
error:
    wx_freep(&c->filter_bank);
    wx_free(c);
    return NULL;
}

static void resample_free(ResampleContext** c) {
    if (!*c)
        return;
    wx_freep(&(*c)->filter_bank);
    wx_freep(c);
}

static int set_compensation(ResampleContext* c, int sample_delta, int compensation_distance) {
    c->compensation_distance = compensation_distance;
    if (compensation_distance)
        c->dst_incr = c->ideal_dst_incr - c->ideal_dst_incr * (int64_t)sample_delta / compensation_distance;
    else
        c->dst_incr = c->ideal_dst_incr;

    c->dst_incr_div = c->dst_incr / c->src_incr;
    c->dst_incr_mod = c->dst_incr % c->src_incr;

    return 0;
}

static int swri_resample(ResampleContext* c,
    uint8_t* dst, const uint8_t* src, int* consumed,
    int src_size, int dst_size, int update_ctx)
{
    if (c->filter_length == 1 && c->phase_shift == 0) {
        int index = c->index;
        int frac = c->frac;
        int64_t index2 = (1LL << 32) * c->frac / c->src_incr + (1LL << 32) * index;
        int64_t incr = (1LL << 32) * c->dst_incr / c->src_incr;
        int new_size = (src_size * (int64_t)c->src_incr - frac + c->dst_incr - 1) / c->dst_incr;

        dst_size = FFMIN(dst_size, new_size);
        c->dsp.resample_one(dst, src, dst_size, index2, incr);

        index += dst_size * c->dst_incr_div;
        index += (frac + dst_size * (int64_t)c->dst_incr_mod) / c->src_incr;
        assert(index >= 0);
        *consumed = index;
        if (update_ctx) {
            c->frac = (frac + dst_size * (int64_t)c->dst_incr_mod) % c->src_incr;
            c->index = 0;
        }
    }
    else {
        int64_t end_index = (1LL + src_size - c->filter_length) << c->phase_shift;
        int64_t delta_frac = (end_index - c->index) * c->src_incr - c->frac;
        int delta_n = (delta_frac + c->dst_incr - 1) / c->dst_incr;

        dst_size = FFMIN(dst_size, delta_n);
        if (dst_size > 0) {
            *consumed = c->dsp.resample(c, dst, src, dst_size, update_ctx);
        }
        else {
            *consumed = 0;
        }
    }

    return dst_size;
}

static int multiple_resample(ResampleContext* c, AudioData* dst, int dst_size, AudioData* src, int src_size, int* consumed) {
    int i, ret = -1;
    //int wx_unused mm_flags = wx_get_cpu_flags();

    int64_t max_src_size = (INT64_MAX >> (c->phase_shift + 1)) / c->src_incr;

    if (c->compensation_distance)
        dst_size = FFMIN(dst_size, c->compensation_distance);
    src_size = FFMIN(src_size, max_src_size);

    for (i = 0; i < dst->ch_count; i++) {
        ret = swri_resample(c, dst->ch[i], src->ch[i],
            consumed, src_size, dst_size, i + 1 == dst->ch_count);
    }


    if (c->compensation_distance) {
        c->compensation_distance -= ret;
        if (!c->compensation_distance) {
            c->dst_incr = c->ideal_dst_incr;
            c->dst_incr_div = c->dst_incr / c->src_incr;
            c->dst_incr_mod = c->dst_incr % c->src_incr;
        }
    }

    return ret;
}

static int64_t get_delay(struct WXTcpSwrContext* s, int64_t base) {
    ResampleContext* c = s->resample;
    int64_t num = s->in_buffer_count - (c->filter_length - 1) / 2;
    num <<= c->phase_shift;
    num -= c->index;
    num *= c->src_incr;
    num -= c->frac;
    return wx_rescale(num, base, s->in_sample_rate * (int64_t)c->src_incr << c->phase_shift);
}

static int resample_flush(struct WXTcpSwrContext* s) {
    AudioData* a = &s->in_buffer;
    int i, j, ret;
    if ((ret = swri_realloc_audio(a, s->in_buffer_index + 2 * s->in_buffer_count)) < 0)
        return ret;
    assert(a->planar);
    for (i = 0; i < a->ch_count; i++) {
        for (j = 0; j < s->in_buffer_count; j++) {
            memcpy(a->ch[i] + (s->in_buffer_index + s->in_buffer_count + j) * a->bps,
                a->ch[i] + (s->in_buffer_index + s->in_buffer_count - j - 1) * a->bps, a->bps);
        }
    }
    s->in_buffer_count += (s->in_buffer_count + 1) / 2;
    return 0;
}

// in fact the whole handle multiple ridiculously small buffers might need more thinking...
static int invert_initial_buffer(ResampleContext* c, AudioData* dst, const AudioData* src,
    int in_count, int* out_idx, int* out_sz)
{
    int n, ch, num = FFMIN(in_count + *out_sz, c->filter_length + 1), res;

    if (c->index >= 0)
        return 0;

    if ((res = swri_realloc_audio(dst, c->filter_length * 2 + 1)) < 0)
        return res;

    // copy
    for (n = *out_sz; n < num; n++) {
        for (ch = 0; ch < src->ch_count; ch++) {
            memcpy(dst->ch[ch] + ((c->filter_length + n) * c->felem_size),
                src->ch[ch] + ((n - *out_sz) * c->felem_size), c->felem_size);
        }
    }

    // if not enough data is in, return and wait for more
    if (num < c->filter_length + 1) {
        *out_sz = num;
        *out_idx = c->filter_length;
        return INT_MAX;
    }

    // else invert
    for (n = 1; n <= c->filter_length; n++) {
        for (ch = 0; ch < src->ch_count; ch++) {
            memcpy(dst->ch[ch] + ((c->filter_length - n) * c->felem_size),
                dst->ch[ch] + ((c->filter_length + n) * c->felem_size),
                c->felem_size);
        }
    }

    res = num - *out_sz;
    *out_idx = c->filter_length + (c->index >> c->phase_shift);
    *out_sz = 1 + c->filter_length * 2 - *out_idx;
    c->index &= c->phase_mask;
    assert(res > 0);

    return res;
}

struct Resampler const swri_resampler = {
  resample_init,
  resample_free,
  multiple_resample,
  resample_flush,
  set_compensation,
  get_delay,
  invert_initial_buffer,
};




#define TEMPLATE_RESAMPLE_S16
#include "WXResample_resample_template.h"
#undef TEMPLATE_RESAMPLE_S16

#define TEMPLATE_RESAMPLE_FLT
#include "WXResample_resample_template.h"
#undef TEMPLATE_RESAMPLE_FLT

void swri_resample_dsp_init(ResampleContext* c)
{
    switch (c->format) {
    case AV_SAMPLE_FMT_S16P:
        c->dsp.resample_one = resample_one_int16;
        c->dsp.resample = c->linear ? resample_linear_int16 : resample_common_int16;
        break;
    case AV_SAMPLE_FMT_FLTP:
        c->dsp.resample_one = resample_one_float;
        c->dsp.resample = c->linear ? resample_linear_float : resample_common_float;
        break;
    }
}




void* wx_malloc_array(size_t nmemb, size_t size)
{
    if (!size || nmemb >= INT_MAX / size)
        return NULL;
    return wx_malloc(nmemb * size);
}

int wx_clip(int a, int amin, int amax) {
    if (a < amin) return amin;
    else if (a > amax) return amax;
    else               return a;
}

uint8_t wx_clip_uint8(int a) {
    if (a & (~0xFF)) return (-a) >> 31;
    else           return a;
}


int16_t wx_clip_int16(int a)
{
    if ((a + 0x8000) & ~0xFFFF) return (a >> 31) ^ 0x7FFF;
    else                      return a;
}

int32_t wx_clipl_int32(int64_t a)
{
    if ((a + 0x80000000u) & ~UINT64_C(0xFFFFFFFF)) return (int32_t)((a >> 63) ^ 0x7FFFFFFF);
    else                                         return (int32_t)a;
}


float wx_clipf(float a, float amin, float amax) {
    if (a < amin) return amin;
    else if (a > amax) return amax;
    else               return a;
}

static const struct {
    const char* name;
    int         nb_channels;
    uint64_t     layout;
} channel_layout_map[] = {
    { "mono",        1,  AV_CH_LAYOUT_MONO },
    { "stereo",      2,  AV_CH_LAYOUT_STEREO },
    { "2.1",         3,  AV_CH_LAYOUT_2POINT1 },
    { "3.0",         3,  AV_CH_LAYOUT_SURROUND },
    { "3.0(back)",   3,  AV_CH_LAYOUT_2_1 },
    { "4.0",         4,  AV_CH_LAYOUT_4POINT0 },
    { "quad",        4,  AV_CH_LAYOUT_QUAD },
    { "quad(side)",  4,  AV_CH_LAYOUT_2_2 },
    { "3.1",         4,  AV_CH_LAYOUT_3POINT1 },
    { "5.0",         5,  AV_CH_LAYOUT_5POINT0_BACK },
    { "5.0(side)",   5,  AV_CH_LAYOUT_5POINT0 },
    { "4.1",         5,  AV_CH_LAYOUT_4POINT1 },
    { "5.1",         6,  AV_CH_LAYOUT_5POINT1_BACK },
    { "5.1(side)",   6,  AV_CH_LAYOUT_5POINT1 },
    { "6.0",         6,  AV_CH_LAYOUT_6POINT0 },
    { "6.0(front)",  6,  AV_CH_LAYOUT_6POINT0_FRONT },
    { "hexagonal",   6,  AV_CH_LAYOUT_HEXAGONAL },
    { "6.1",         7,  AV_CH_LAYOUT_6POINT1 },
    { "6.1",         7,  AV_CH_LAYOUT_6POINT1_BACK },
    { "6.1(front)",  7,  AV_CH_LAYOUT_6POINT1_FRONT },
    { "7.0",         7,  AV_CH_LAYOUT_7POINT0 },
    { "7.0(front)",  7,  AV_CH_LAYOUT_7POINT0_FRONT },
    { "7.1",         8,  AV_CH_LAYOUT_7POINT1 },
    { "7.1(wide)",   8,  AV_CH_LAYOUT_7POINT1_WIDE_BACK },
    { "7.1(wide-side)",   8,  AV_CH_LAYOUT_7POINT1_WIDE },
    { "octagonal",   8,  AV_CH_LAYOUT_OCTAGONAL },
    { "downmix",     2,  AV_CH_LAYOUT_STEREO_DOWNMIX, },
};

static  int wx_popcount_c(uint32_t x)
{
    x -= (x >> 1) & 0x55555555;
    x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
    x = (x + (x >> 4)) & 0x0F0F0F0F;
    x += x >> 8;
    return (x + (x >> 16)) & 0x3F;
}

static  int wx_popcount64(uint64_t x)
{
    return wx_popcount_c((uint32_t)x) + wx_popcount_c((uint32_t)(x >> 32));
}

int wx_get_channel_layout_nb_channels(uint64_t channel_layout)
{
    return wx_popcount64(channel_layout);
}

int64_t wx_get_default_channel_layout(int nb_channels) {
    int i;
    for (i = 0; i < FF_ARRAY_ELEMS(channel_layout_map); i++)
        if (nb_channels == channel_layout_map[i].nb_channels)
            return channel_layout_map[i].layout;
    return 0;
}


typedef struct SampleFmtInfo {
    int bits;
    int planar;
    enum WXTcpFormat altform; ///< planar<->packed alternative form
} SampleFmtInfo;

/** this table gives more information about formats */
static const SampleFmtInfo sample_fmt_info[AV_SAMPLE_FMT_NB] = {
    [AV_SAMPLE_FMT_S16] = {.bits = 16,.planar = 0,.altform = AV_SAMPLE_FMT_S16P },
    [AV_SAMPLE_FMT_FLT] = {.bits = 32,.planar = 0,.altform = AV_SAMPLE_FMT_FLTP },
    [AV_SAMPLE_FMT_S16P] = {.bits = 16,.planar = 1,.altform = AV_SAMPLE_FMT_S16 },
    [AV_SAMPLE_FMT_FLTP] = {.bits = 32,.planar = 1,.altform = AV_SAMPLE_FMT_FLT },
};

enum WXTcpFormat wx_get_packed_sample_fmt(enum WXTcpFormat sample_fmt)
{
    if (sample_fmt < 0 || sample_fmt >= AV_SAMPLE_FMT_NB)
        return AV_SAMPLE_FMT_NONE;
    if (sample_fmt_info[sample_fmt].planar)
        return sample_fmt_info[sample_fmt].altform;
    return sample_fmt;
}

enum WXTcpFormat wx_get_planar_sample_fmt(enum WXTcpFormat sample_fmt)
{
    if (sample_fmt < 0 || sample_fmt >= AV_SAMPLE_FMT_NB)
        return AV_SAMPLE_FMT_NONE;
    if (sample_fmt_info[sample_fmt].planar)
        return sample_fmt;
    return sample_fmt_info[sample_fmt].altform;
}

int wx_get_bytes_per_sample(enum WXTcpFormat sample_fmt)
{
    return sample_fmt < 0 || sample_fmt >= AV_SAMPLE_FMT_NB ?
        0 : sample_fmt_info[sample_fmt].bits >> 3;
}

int wx_sample_fmt_is_planar(enum WXTcpFormat sample_fmt)
{
    if (sample_fmt < 0 || sample_fmt >= AV_SAMPLE_FMT_NB)
        return 0;
    return sample_fmt_info[sample_fmt].planar;
}

typedef struct AVRational {
    int num; ///< numerator
    int den; ///< denominator
} AVRational;

#define FFABS(a) ((a) >= 0 ? (a) : (-(a)))
int wx_reduce(int* dst_num, int* dst_den,
    int64_t num, int64_t den, int64_t max)
{
    AVRational a0 = { 0, 1 }, a1 = { 1, 0 };
    int sign = (num < 0) ^ (den < 0);
    int64_t gcd = wx_gcd(FFABS(num), FFABS(den));

    if (gcd) {
        num = FFABS(num) / gcd;
        den = FFABS(den) / gcd;
    }
    if (num <= max && den <= max) {
        a1 = (AVRational){ num, den };
        den = 0;
    }

    while (den) {
        uint64_t x = num / den;
        int64_t next_den = num - den * x;
        int64_t a2n = x * a1.num + a0.num;
        int64_t a2d = x * a1.den + a0.den;

        if (a2n > max || a2d > max) {
            if (a1.num) x = (max - a0.num) / a1.num;
            if (a1.den) x = FFMIN(x, (max - a0.den) / a1.den);

            if (den * (2 * x * a1.den + a0.den) > num * a1.den)
                a1 = (AVRational){ x * a1.num + a0.num, x * a1.den + a0.den };
            break;
        }

        a0 = a1;
        a1 = (AVRational){ a2n, a2d };
        num = den;
        den = next_den;
    }
    assert(wx_gcd(a1.num, a1.den) <= 1U);

    *dst_num = sign ? -a1.num : a1.num;
    *dst_den = a1.den;

    return den == 0;
}

int64_t wx_gcd(int64_t a, int64_t b)
{
    if (b)
        return wx_gcd(b, a % b);
    else
        return a;
}

enum AVRounding {
    AV_ROUND_ZERO = 0, ///< Round toward zero.
    AV_ROUND_INF = 1, ///< Round away from zero.
    AV_ROUND_DOWN = 2, ///< Round toward -infinity.
    AV_ROUND_UP = 3, ///< Round toward +infinity.
    AV_ROUND_NEAR_INF = 5, ///< Round to nearest and halfway cases away from zero.
    AV_ROUND_PASS_MINMAX = 8192,
};
int64_t wx_rescale_rnd(int64_t a, int64_t b, int64_t c, enum AVRounding rnd)
{
    int64_t r = 0;
    assert(c > 0);
    assert(b >= 0);
    assert((unsigned)(rnd & ~AV_ROUND_PASS_MINMAX) <= 5 && (rnd & ~AV_ROUND_PASS_MINMAX) != 4);

    if (c <= 0 || b < 0 || !((unsigned)(rnd & ~AV_ROUND_PASS_MINMAX) <= 5 && (rnd & ~AV_ROUND_PASS_MINMAX) != 4))
        return INT64_MIN;

    if (rnd & AV_ROUND_PASS_MINMAX) {
        if (a == INT64_MIN || a == INT64_MAX)
            return a;
        rnd -= AV_ROUND_PASS_MINMAX;
    }

    if (a < 0 && a != INT64_MIN)
        return -wx_rescale_rnd(-a, b, c, rnd ^ ((rnd >> 1) & 1));

    if (rnd == AV_ROUND_NEAR_INF)
        r = c / 2;
    else if (rnd & 1)
        r = c - 1;

    if (b <= INT_MAX && c <= INT_MAX) {
        if (a <= INT_MAX)
            return (a * b + r) / c;
        else
            return a / c * b + (a % c * b + r) / c;
    }
    else {
        uint64_t a0 = a & 0xFFFFFFFF;
        uint64_t a1 = a >> 32;
        uint64_t b0 = b & 0xFFFFFFFF;
        uint64_t b1 = b >> 32;
        uint64_t t1 = a0 * b1 + a1 * b0;
        uint64_t t1a = t1 << 32;
        int i;

        a0 = a0 * b0 + t1a;
        a1 = a1 * b1 + (t1 >> 32) + (a0 < t1a);
        a0 += r;
        a1 += a0 < r;

        for (i = 63; i >= 0; i--) {
            a1 += a1 + ((a0 >> i) & 1);
            t1 += t1;
            if (c <= a1) {
                a1 -= c;
                t1++;
            }
        }
        return t1;
    }
}

int64_t wx_rescale(int64_t a, int64_t b, int64_t c)
{
    return wx_rescale_rnd(a, b, c, AV_ROUND_NEAR_INF);
}


#define ALIGN (HAVE_AVX ? 32 : 16)

static size_t max_alloc_size = INT_MAX;


void* wx_malloc(size_t size)
{
    void* ptr = NULL;
#if CONFIG_MEMALIGN_HACK
    long diff;
#endif

    /* let's disallow possibly ambiguous cases */
    if (size > (max_alloc_size - 32))
        return NULL;

#if CONFIG_MEMALIGN_HACK
    ptr = malloc(size + ALIGN);
    if (!ptr)
        return ptr;
    diff = ((~(long)ptr) & (ALIGN - 1)) + 1;
    ptr = (char*)ptr + diff;
    ((char*)ptr)[-1] = diff;
#elif HAVE_POSIX_MEMALIGN
    if (size) //OS X on SDK 10.6 has a broken posix_memalign implementation
        if (posix_memalign(&ptr, ALIGN, size))
            ptr = NULL;
#elif HAVE_ALIGNED_MALLOC
    ptr = _aligned_malloc(size, ALIGN);
#elif HAVE_MEMALIGN
#ifndef __DJGPP__
    ptr = memalign(ALIGN, size);
#else
    ptr = memalign(size, ALIGN);
#endif
    /* Why 64?
    * Indeed, we should align it:
    *   on  4 for 386
    *   on 16 for 486
    *   on 32 for 586, PPro - K6-III
    *   on 64 for K7 (maybe for P3 too).
    * Because L1 and L2 caches are aligned on those values.
    * But I don't want to code such logic here!
    */
    /* Why 32?
    * For AVX ASM. SSE / NEON needs only 16.
    * Why not larger? Because I did not see a difference in benchmarks ...
    */
    /* benchmarks with P3
    * memalign(64) + 1          3071, 3051, 3032
    * memalign(64) + 2          3051, 3032, 3041
    * memalign(64) + 4          2911, 2896, 2915
    * memalign(64) + 8          2545, 2554, 2550
    * memalign(64) + 16         2543, 2572, 2563
    * memalign(64) + 32         2546, 2545, 2571
    * memalign(64) + 64         2570, 2533, 2558
    *
    * BTW, malloc seems to do 8-byte alignment by default here.
    */
#else
    ptr = malloc(size);
#endif
    if (!ptr && !size) {
        size = 1;
        ptr = wx_malloc(1);
    }
    return ptr;
}

void wx_free(void* ptr) {
#if CONFIG_MEMALIGN_HACK
    if (ptr) {
        int v = ((char*)ptr)[-1];
        assert(v > 0 && v <= ALIGN);
        free((char*)ptr - v);
    }
#elif HAVE_ALIGNED_MALLOC
    _aligned_free(ptr);
#else
    free(ptr);
#endif
}

void wx_freep(void* arg) {
    void** ptr = (void**)arg;
    wx_free(*ptr);
    *ptr = NULL;
}

void* wx_mallocz(size_t size) {
    void* ptr = wx_malloc(size);
    if (ptr)
        memset(ptr, 0, size);
    return ptr;
}

void* wx_calloc(size_t nmemb, size_t size) {
    if (size <= 0 || nmemb >= INT_MAX / size)
        return NULL;
    return wx_mallocz(nmemb * size);
}

#include <float.h>
#define ALIGN 32

static void set_audiodata_fmt(AudioData *a, enum WXTcpFormat fmt);
static void clear_context(WXTcpSwrContext *s);
static int WXResampleInit(struct WXTcpSwrContext *s) {
	int ret;

	clear_context(s);

	s->resampler = &swri_resampler;

	if (s->in_sample_fmt >= AV_SAMPLE_FMT_NB) {
		// wx_log(s, AV_LOG_ERROR, "Requested input sample format %d is invalid\n", s->in_sample_fmt);
		return AVERROR(EINVAL);
	}
	if (s->out_sample_fmt >= AV_SAMPLE_FMT_NB) {
		// wx_log(s, AV_LOG_ERROR, "Requested output sample format %d is invalid\n", s->out_sample_fmt);
		return AVERROR(EINVAL);
	}

	if (wx_get_channel_layout_nb_channels(s->in_ch_layout) > SWR_CH_MAX) {
		s->in_ch_layout = 0;
	}

	if (wx_get_channel_layout_nb_channels(s->out_ch_layout) > SWR_CH_MAX) {
		s->out_ch_layout = 0;
	}



	if (!s->used_ch_count)
		s->used_ch_count = s->in.ch_count;

	if (s->used_ch_count && s->in_ch_layout && s->used_ch_count != wx_get_channel_layout_nb_channels(s->in_ch_layout)) {
		// wx_log(s, AV_LOG_WARNING, "Input channel layout has a different number of channels than the number of used channels, ignoring layout\n");
		s->in_ch_layout = 0;
	}

	if (!s->in_ch_layout)
		s->in_ch_layout = wx_get_default_channel_layout(s->used_ch_count);
	if (!s->out_ch_layout)
		s->out_ch_layout = wx_get_default_channel_layout(s->out.ch_count);

	s->rematrix = s->out_ch_layout != s->in_ch_layout || s->rematrix_volume != 1.0 ||
		s->rematrix_custom;

	if (s->int_sample_fmt == AV_SAMPLE_FMT_NONE) {
		if (wx_get_planar_sample_fmt(s->in_sample_fmt) <= AV_SAMPLE_FMT_S16P) {
			s->int_sample_fmt = AV_SAMPLE_FMT_S16P;
		}
		else if (wx_get_planar_sample_fmt(s->in_sample_fmt) <= AV_SAMPLE_FMT_FLTP) {
			s->int_sample_fmt = AV_SAMPLE_FMT_FLTP;
		}
	}

	if (s->int_sample_fmt != AV_SAMPLE_FMT_S16P
		&&s->int_sample_fmt != AV_SAMPLE_FMT_FLTP) {
		return AVERROR(EINVAL);
	}

	set_audiodata_fmt(&s->in, s->in_sample_fmt);
	set_audiodata_fmt(&s->out, s->out_sample_fmt);

	if (s->firstpts_in_samples != AV_NOPTS_VALUE) {
		if (!s->async && s->min_compensation >= FLT_MAX / 2)
			s->async = 1;
		s->firstpts =
			s->outpts = s->firstpts_in_samples * s->out_sample_rate;
	}
	else
		s->firstpts = AV_NOPTS_VALUE;

	if (s->async) {
		if (s->min_compensation >= FLT_MAX / 2)
			s->min_compensation = 0.001;
		if (s->async > 1.0001) {
			s->max_soft_compensation = s->async / (double)s->in_sample_rate;
		}
	}

	if (s->out_sample_rate != s->in_sample_rate || (s->flags & SWR_FLAG_RESAMPLE)) {
		s->resample = s->resampler->init(s->resample, s->out_sample_rate, s->in_sample_rate, s->filter_size, s->phase_shift, s->linear_interp, s->cutoff, s->int_sample_fmt, s->filter_type, s->kaiser_beta, s->precision, s->cheby);
	}
	else
		s->resampler->free(&s->resample);
	if (s->int_sample_fmt != AV_SAMPLE_FMT_S16P
		&& s->int_sample_fmt != AV_SAMPLE_FMT_FLTP
		&& s->resample) {
		// wx_log(s, AV_LOG_ERROR, "Resampling only supported with internal s16/s32/flt/dbl\n");
		return -1;
	}

#define RSC 1 //FIXME finetune
	if (!s->in.ch_count)
		s->in.ch_count = wx_get_channel_layout_nb_channels(s->in_ch_layout);
	if (!s->used_ch_count)
		s->used_ch_count = s->in.ch_count;
	if (!s->out.ch_count)
		s->out.ch_count = wx_get_channel_layout_nb_channels(s->out_ch_layout);

	if (!s->in.ch_count) {
		assert(!s->in_ch_layout);
		// wx_log(s, AV_LOG_ERROR, "Input channel count and layout are unset\n");
		return -1;
	}

	if ((!s->out_ch_layout || !s->in_ch_layout) && s->used_ch_count != s->out.ch_count && !s->rematrix_custom) {
		return -1;
	}

	assert(s->used_ch_count);
	assert(s->out.ch_count);
	s->resample_first = RSC*s->out.ch_count / s->in.ch_count - RSC < s->out_sample_rate / (float)s->in_sample_rate - 1.0;

	s->in_buffer = s->in;
	s->silence = s->in;
	s->drop_temp = s->out;

	if (!s->resample && !s->rematrix && !s->channel_map && !s->dither.method) {
		s->full_convert = swri_audio_convert_alloc(s->out_sample_fmt,
			s->in_sample_fmt, s->in.ch_count, NULL, 0);
		return 0;
	}

	s->in_convert = swri_audio_convert_alloc(s->int_sample_fmt,
		s->in_sample_fmt, s->used_ch_count, s->channel_map, 0);
	s->out_convert = swri_audio_convert_alloc(s->out_sample_fmt,
		s->int_sample_fmt, s->out.ch_count, NULL, 0);

	if (!s->in_convert || !s->out_convert)
		return AVERROR(ENOMEM);

	s->postin = s->in;
	s->preout = s->out;
	s->midbuf = s->in;

	if (s->channel_map) {
		s->postin.ch_count =
			s->midbuf.ch_count = s->used_ch_count;
		if (s->resample)
			s->in_buffer.ch_count = s->used_ch_count;
	}
	if (!s->resample_first) {
		s->midbuf.ch_count = s->out.ch_count;
		if (s->resample)
			s->in_buffer.ch_count = s->out.ch_count;
	}

	set_audiodata_fmt(&s->postin, s->int_sample_fmt);
	set_audiodata_fmt(&s->midbuf, s->int_sample_fmt);
	set_audiodata_fmt(&s->preout, s->int_sample_fmt);

	if (s->resample) {
		set_audiodata_fmt(&s->in_buffer, s->int_sample_fmt);
	}

	if ((ret = swri_dither_init(s, s->out_sample_fmt, s->int_sample_fmt)) < 0)
		return ret;

	if (s->rematrix || s->dither.method)
		return swri_rematrix_init(s);

	return 0;
}

void *WXResampleCreate(int out_bFloat, int out_channels, int out_sample_rate,
	int in_bFloat, int  in_channels, int  in_sample_rate) {

	WXTcpSwrContext *s = wx_mallocz(sizeof(WXTcpSwrContext));


	int64_t out_ch_layout = wx_get_default_channel_layout(out_channels);
	int64_t in_ch_layout = wx_get_default_channel_layout(in_channels);


	s->out_ch_layout = out_ch_layout;

	s->out_sample_fmt = out_bFloat ? AV_SAMPLE_FMT_FLT : AV_SAMPLE_FMT_S16;
	s->out_sample_rate = out_sample_rate;

	s->in_ch_layout = in_ch_layout;
	s->in_sample_fmt = in_bFloat ? AV_SAMPLE_FMT_FLT : AV_SAMPLE_FMT_S16;

	s->in_sample_rate = in_sample_rate;

	s->int_sample_fmt = AV_SAMPLE_FMT_NONE;

	s->in.ch_count = in_channels;
	s->out.ch_count = out_channels;
	s->used_ch_count = 0;
	WXResampleInit(s);
    return (void*)s;
}

static void set_audiodata_fmt(AudioData *a, enum WXTcpFormat fmt){
    a->fmt   = fmt;
    a->bps   = wx_get_bytes_per_sample(fmt);
    a->planar= wx_sample_fmt_is_planar(fmt);
}

static void free_temp(AudioData *a){
    wx_free(a->data);
    memset(a, 0, sizeof(*a));
}

static void clear_context(WXTcpSwrContext *s){
    s->in_buffer_index= 0;
    s->in_buffer_count= 0;
    s->resample_in_constraint= 0;
    memset(s->in.ch, 0, sizeof(s->in.ch));
    memset(s->out.ch, 0, sizeof(s->out.ch));
    free_temp(&s->postin);
    free_temp(&s->midbuf);
    free_temp(&s->preout);
    free_temp(&s->in_buffer);
    free_temp(&s->silence);
    free_temp(&s->drop_temp);
    free_temp(&s->dither.noise);
    free_temp(&s->dither.temp);
    swri_audio_convert_free(&s-> in_convert);
    swri_audio_convert_free(&s->out_convert);
    swri_audio_convert_free(&s->full_convert);
    swri_rematrix_free(s);

    s->flushed = 0;
}

 void WXResampleDestroy(void **ss){
    WXTcpSwrContext *s= *ss;
    if(s){
        clear_context(s);
        if (s->resampler)
            s->resampler->free(&s->resample);
    }

    wx_freep(ss);
}



int swri_realloc_audio(AudioData *a, int count){
    int i, countb;
    AudioData old;

    if(count < 0 || count > INT_MAX/2/a->bps/a->ch_count)
        return AVERROR(EINVAL);

    if(a->count >= count)
        return 0;

    count*=2;

    countb= FFALIGN(count*a->bps, ALIGN);
    old= *a;

    assert(a->bps);
    assert(a->ch_count);

    a->data= wx_mallocz(countb*a->ch_count);
    if(!a->data)
        return AVERROR(ENOMEM);
    for(i=0; i<a->ch_count; i++){
        a->ch[i]= a->data + i*(a->planar ? countb : a->bps);
        if(a->planar) memcpy(a->ch[i], old.ch[i], a->count*a->bps);
    }
    if(!a->planar) memcpy(a->ch[0], old.ch[0], a->count*a->ch_count*a->bps);
    wx_freep(&old.data);
    a->count= count;

    return 1;
}

static void copy(AudioData *out, AudioData *in,
                 int count){
    assert(out->planar == in->planar);
    assert(out->bps == in->bps);
    assert(out->ch_count == in->ch_count);
    if(out->planar){
        int ch;
        for(ch=0; ch<out->ch_count; ch++)
            memcpy(out->ch[ch], in->ch[ch], count*out->bps);
    }else
        memcpy(out->ch[0], in->ch[0], count*out->ch_count*out->bps);
}

static void fill_audiodata(AudioData *out, uint8_t *in_arg [SWR_CH_MAX]){
    int i;
    if(!in_arg){
        memset(out->ch, 0, sizeof(out->ch));
    }else if(out->planar){
        for(i=0; i<out->ch_count; i++)
            out->ch[i]= in_arg[i];
    }else{
        for(i=0; i<out->ch_count; i++)
            out->ch[i]= in_arg[0] + i*out->bps;
    }
}

static void reversefill_audiodata(AudioData *out, uint8_t *in_arg [SWR_CH_MAX]){
    int i;
    if(out->planar){
        for(i=0; i<out->ch_count; i++)
            in_arg[i]= out->ch[i];
    }else{
        in_arg[0]= out->ch[0];
    }
}

/**
 *
 * out may be equal in.
 */
static void buf_set(AudioData *out, AudioData *in, int count){
    int ch;
    if(in->planar){
        for(ch=0; ch<out->ch_count; ch++)
            out->ch[ch]= in->ch[ch] + count*out->bps;
    }else{
        for(ch=out->ch_count-1; ch>=0; ch--)
            out->ch[ch]= in->ch[0] + (ch + count*out->ch_count) * out->bps;
    }
}

/**
 *
 * @return number of samples output per channel
 */
static int resample(WXTcpSwrContext *s, AudioData *out_param, int out_count,
                             const AudioData * in_param, int in_count){
    AudioData in, out, tmp;
    int ret_sum=0;
    int border=0;
    int padless = 0;

    assert(s->in_buffer.ch_count == in_param->ch_count);
    assert(s->in_buffer.planar   == in_param->planar);
    assert(s->in_buffer.fmt      == in_param->fmt);

    tmp=out=*out_param;
    in =  *in_param;

    border = s->resampler->invert_initial_buffer(s->resample, &s->in_buffer,
                 &in, in_count, &s->in_buffer_index, &s->in_buffer_count);
    if (border == INT_MAX) return 0;
    else if (border < 0) return border;
    else if (border) { buf_set(&in, &in, border); in_count -= border; s->resample_in_constraint = 0; }

    do{
        int ret, size, consumed;
        if(!s->resample_in_constraint && s->in_buffer_count){
            buf_set(&tmp, &s->in_buffer, s->in_buffer_index);
            ret= s->resampler->multiple_resample(s->resample, &out, out_count, &tmp, s->in_buffer_count, &consumed);
            out_count -= ret;
            ret_sum += ret;
            buf_set(&out, &out, ret);
            s->in_buffer_count -= consumed;
            s->in_buffer_index += consumed;

            if(!in_count)
                break;
            if(s->in_buffer_count <= border){
                buf_set(&in, &in, -s->in_buffer_count);
                in_count += s->in_buffer_count;
                s->in_buffer_count=0;
                s->in_buffer_index=0;
                border = 0;
            }
        }

        if((s->flushed || in_count > padless) && !s->in_buffer_count){
            s->in_buffer_index=0;
            ret= s->resampler->multiple_resample(s->resample, &out, out_count, &in, FFMAX(in_count-padless, 0), &consumed);
            out_count -= ret;
            ret_sum += ret;
            buf_set(&out, &out, ret);
            in_count -= consumed;
            buf_set(&in, &in, consumed);
        }

        //TODO is this check sane considering the advanced copy avoidance below
        size= s->in_buffer_index + s->in_buffer_count + in_count;
        if(   size > s->in_buffer.count
           && s->in_buffer_count + in_count <= s->in_buffer_index){
            buf_set(&tmp, &s->in_buffer, s->in_buffer_index);
            copy(&s->in_buffer, &tmp, s->in_buffer_count);
            s->in_buffer_index=0;
        }else
            if((ret=swri_realloc_audio(&s->in_buffer, size)) < 0)
                return ret;

        if(in_count){
            int count= in_count;
            if(s->in_buffer_count && s->in_buffer_count+2 < count && out_count) count= s->in_buffer_count+2;

            buf_set(&tmp, &s->in_buffer, s->in_buffer_index + s->in_buffer_count);
            copy(&tmp, &in, /*in_*/count);
            s->in_buffer_count += count;
            in_count -= count;
            border += count;
            buf_set(&in, &in, count);
            s->resample_in_constraint= 0;
            if(s->in_buffer_count != count || in_count)
                continue;
            if (padless) {
                padless = 0;
                continue;
            }
        }
        break;
    }while(1);

    s->resample_in_constraint= !!out_count;

    return ret_sum;
}

static int WXResampleConvert_internal(struct WXTcpSwrContext *s, AudioData *out, int out_count,
                                                      AudioData *in , int  in_count){
    AudioData *postin, *midbuf, *preout;
    int ret/*, in_max*/;
    AudioData preout_tmp, midbuf_tmp;

    if(s->full_convert){
        assert(!s->resample);
        swri_audio_convert(s->full_convert, out, in, in_count);
        return out_count;
    }

//     in_max= out_count*(int64_t)s->in_sample_rate / s->out_sample_rate + resample_filter_taps;
//     in_count= FFMIN(in_count, in_in + 2 - s->hist_buffer_count);

    if((ret=swri_realloc_audio(&s->postin, in_count))<0)
        return ret;
    if(s->resample_first){
        assert(s->midbuf.ch_count == s->used_ch_count);
        if((ret=swri_realloc_audio(&s->midbuf, out_count))<0)
            return ret;
    }else{
        assert(s->midbuf.ch_count ==  s->out.ch_count);
        if((ret=swri_realloc_audio(&s->midbuf,  in_count))<0)
            return ret;
    }
    if((ret=swri_realloc_audio(&s->preout, out_count))<0)
        return ret;

    postin= &s->postin;

    midbuf_tmp= s->midbuf;
    midbuf= &midbuf_tmp;
    preout_tmp= s->preout;
    preout= &preout_tmp;

    if(s->int_sample_fmt == s-> in_sample_fmt && s->in.planar && !s->channel_map)
        postin= in;

    if(s->resample_first ? !s->resample : !s->rematrix)
        midbuf= postin;

    if(s->resample_first ? !s->rematrix : !s->resample)
        preout= midbuf;

    if(s->int_sample_fmt == s->out_sample_fmt && s->out.planar
       && !(0 && (s->dither.output_sample_bits&31))){
        if(preout==in){
            out_count= FFMIN(out_count, in_count); //TODO check at the end if this is needed or redundant
            assert(s->in.planar); //we only support planar internally so it has to be, we support copying non planar though
            copy(out, in, out_count);
            return out_count;
        }
        else if(preout==postin) preout= midbuf= postin= out;
        else if(preout==midbuf) preout= midbuf= out;
        else                    preout= out;
    }

    if(in != postin){
        swri_audio_convert(s->in_convert, postin, in, in_count);
    }

    if(s->resample_first){
        if(postin != midbuf)
            out_count= resample(s, midbuf, out_count, postin, in_count);
        if(midbuf != preout)
            swri_rematrix(s, preout, midbuf, out_count, preout==out);
    }else{
        if(postin != midbuf)
            swri_rematrix(s, midbuf, postin, in_count, midbuf==out);
        if(midbuf != preout)
            out_count= resample(s, preout, out_count, midbuf, in_count);
    }

    if(preout != out && out_count){
        AudioData *conv_src = preout;
        if(s->dither.method){
            int ch;
            int dither_count= FFMAX(out_count, 1<<16);

            if (preout == in) {
                conv_src = &s->dither.temp;
                if((ret=swri_realloc_audio(&s->dither.temp, dither_count))<0)
                    return ret;
            }

            if((ret=swri_realloc_audio(&s->dither.noise, dither_count))<0)
                return ret;
            if(ret)
                for(ch=0; ch<s->dither.noise.ch_count; ch++)
                    swri_get_dither(s, s->dither.noise.ch[ch], s->dither.noise.count, 12345678913579<<ch, s->dither.noise.fmt);
            assert(s->dither.noise.ch_count == preout->ch_count);

            if(s->dither.noise_pos + out_count > s->dither.noise.count)
                s->dither.noise_pos = 0;

            if (s->dither.method < SWR_DITHER_NS){
                if (s->mix_2_1_simd) {
                    int len1= out_count&~15;
                    int off = len1 * preout->bps;

                    if(len1)
                        for(ch=0; ch<preout->ch_count; ch++)
                            s->mix_2_1_simd(conv_src->ch[ch], preout->ch[ch], s->dither.noise.ch[ch] + s->dither.noise.bps * s->dither.noise_pos, s->native_simd_one, 0, 0, len1);
                    if(out_count != len1)
                        for(ch=0; ch<preout->ch_count; ch++)
                            s->mix_2_1_f(conv_src->ch[ch] + off, preout->ch[ch] + off, s->dither.noise.ch[ch] + s->dither.noise.bps * s->dither.noise_pos + off + len1, s->native_one, 0, 0, out_count - len1);
                } else {
                    for(ch=0; ch<preout->ch_count; ch++)
                        s->mix_2_1_f(conv_src->ch[ch], preout->ch[ch], s->dither.noise.ch[ch] + s->dither.noise.bps * s->dither.noise_pos, s->native_one, 0, 0, out_count);
                }
            } else {
                switch(s->int_sample_fmt) {
                case AV_SAMPLE_FMT_S16P :
					swri_noise_shaping_int16(s, conv_src, preout, &s->dither.noise, out_count); break;
                case AV_SAMPLE_FMT_FLTP :
					swri_noise_shaping_float(s, conv_src, preout, &s->dither.noise, out_count); break;
                }
            }
            s->dither.noise_pos += out_count;
        }
//FIXME packed doesn't need more than 1 chan here!
        swri_audio_convert(s->out_convert, out, conv_src, out_count);
    }
    return out_count;
}

int WXResampleis_initialized(struct WXTcpSwrContext *s) {
    return !!s->in_buffer.ch_count;
}

int WXResampleConvert(struct WXTcpSwrContext *s, uint8_t *out_arg[SWR_CH_MAX], int out_count,
                                const uint8_t *in_arg [SWR_CH_MAX], int  in_count){
    AudioData * in= &s->in;
    AudioData *out= &s->out;

    if (!WXResampleis_initialized(s)) {
        // wx_log(s, AV_LOG_ERROR, "Context has not been initialized\n");
        return AVERROR(EINVAL);
    }

    while(s->drop_output > 0){
        int ret;
        uint8_t *tmp_arg[SWR_CH_MAX];
#define MAX_DROP_STEP 16384
        if((ret=swri_realloc_audio(&s->drop_temp, FFMIN(s->drop_output, MAX_DROP_STEP)))<0)
            return ret;

        reversefill_audiodata(&s->drop_temp, tmp_arg);
        s->drop_output *= -1; //FIXME find a less hackish solution
        ret = WXResampleConvert(s, tmp_arg, FFMIN(-s->drop_output, MAX_DROP_STEP), in_arg, in_count); //FIXME optimize but this is as good as never called so maybe it doesn't matter
        s->drop_output *= -1;
        in_count = 0;
        if(ret>0) {
            s->drop_output -= ret;
            continue;
        }

        if(s->drop_output || !out_arg)
            return 0;
    }

    if(!in_arg){
        if(s->resample){
            if (!s->flushed)
                s->resampler->flush(s);
            s->resample_in_constraint = 0;
            s->flushed = 1;
        }else if(!s->in_buffer_count){
            return 0;
        }
    }else
        fill_audiodata(in ,  (void*)in_arg);

    fill_audiodata(out, out_arg);

    if(s->resample){
        int ret = WXResampleConvert_internal(s, out, out_count, in, in_count);
        if(ret>0 && !s->drop_output)
            s->outpts += ret * (int64_t)s->in_sample_rate;
        return ret;
    }else{
        AudioData tmp= *in;
        int ret2=0;
        int ret, size;
        size = FFMIN(out_count, s->in_buffer_count);
        if(size){
            buf_set(&tmp, &s->in_buffer, s->in_buffer_index);
            ret= WXResampleConvert_internal(s, out, size, &tmp, size);
            if(ret<0)
                return ret;
            ret2= ret;
            s->in_buffer_count -= ret;
            s->in_buffer_index += ret;
            buf_set(out, out, ret);
            out_count -= ret;
            if(!s->in_buffer_count)
                s->in_buffer_index = 0;
        }

        if(in_count){
            size= s->in_buffer_index + s->in_buffer_count + in_count - out_count;

            if(in_count > out_count) { //FIXME move after WXResampleConvert_internal
                if(   size > s->in_buffer.count
                && s->in_buffer_count + in_count - out_count <= s->in_buffer_index){
                    buf_set(&tmp, &s->in_buffer, s->in_buffer_index);
                    copy(&s->in_buffer, &tmp, s->in_buffer_count);
                    s->in_buffer_index=0;
                }else
                    if((ret=swri_realloc_audio(&s->in_buffer, size)) < 0)
                        return ret;
            }

            if(out_count){
                size = FFMIN(in_count, out_count);
                ret= WXResampleConvert_internal(s, out, size, in, size);
                if(ret<0)
                    return ret;
                buf_set(in, in, ret);
                in_count -= ret;
                ret2 += ret;
            }
            if(in_count){
                buf_set(&tmp, &s->in_buffer, s->in_buffer_index + s->in_buffer_count);
                copy(&tmp, in, in_count);
                s->in_buffer_count += in_count;
            }
        }
        if(ret2>0 && !s->drop_output)
            s->outpts += ret2 * (int64_t)s->in_sample_rate;
        return ret2;
    }
}


#define TEMPLATE_REMATRIX_FLT
#include "WXResample_rematrix_template.h"
#undef TEMPLATE_REMATRIX_FLT

#define TEMPLATE_REMATRIX_S16
#include "WXResample_rematrix_template.h"
#undef TEMPLATE_REMATRIX_S16

#define FRONT_LEFT             0
#define FRONT_RIGHT            1
#define FRONT_CENTER           2
#define LOW_FREQUENCY          3
#define BACK_LEFT              4
#define BACK_RIGHT             5
#define FRONT_LEFT_OF_CENTER   6
#define FRONT_RIGHT_OF_CENTER  7
#define BACK_CENTER            8
#define SIDE_LEFT              9
#define SIDE_RIGHT             10
#define TOP_CENTER             11
#define TOP_FRONT_LEFT         12
#define TOP_FRONT_CENTER       13
#define TOP_FRONT_RIGHT        14
#define TOP_BACK_LEFT          15
#define TOP_BACK_CENTER        16
#define TOP_BACK_RIGHT         17



static int even(int64_t layout) {
    if (!layout) return 1;
    if (layout & (layout - 1)) return 1;
    return 0;
}

static int clean_layout(WXTcpSwrContext* s, int64_t layout) {
    if (layout && layout != AV_CH_FRONT_CENTER && !(layout & (layout - 1))) {
        return AV_CH_FRONT_CENTER;
    }

    return layout;
}

static int sane_layout(int64_t layout) {
    if (!(layout & AV_CH_LAYOUT_SURROUND)) // at least 1 front speaker
        return 0;
    if (!even(layout & (AV_CH_FRONT_LEFT | AV_CH_FRONT_RIGHT))) // no asymetric front
        return 0;
    if (!even(layout & (AV_CH_SIDE_LEFT | AV_CH_SIDE_RIGHT)))   // no asymetric side
        return 0;
    if (!even(layout & (AV_CH_BACK_LEFT | AV_CH_BACK_RIGHT)))
        return 0;
    if (!even(layout & (AV_CH_FRONT_LEFT_OF_CENTER | AV_CH_FRONT_RIGHT_OF_CENTER)))
        return 0;
    if (wx_get_channel_layout_nb_channels(layout) >= SWR_CH_MAX)
        return 0;

    return 1;
}

static int auto_matrix(WXTcpSwrContext* s)
{
    int i, j, out_i;
    double matrix[64][64] = { {0} };
    int64_t unaccounted, in_ch_layout, out_ch_layout;
    double maxcoef = 0;
    char buf[128];
    const int matrix_encoding = s->matrix_encoding;
    float maxval;

    in_ch_layout = clean_layout(s, s->in_ch_layout);
    out_ch_layout = clean_layout(s, s->out_ch_layout);

    if (out_ch_layout == AV_CH_LAYOUT_STEREO_DOWNMIX
        && (in_ch_layout & AV_CH_LAYOUT_STEREO_DOWNMIX) == 0
        )
        out_ch_layout = AV_CH_LAYOUT_STEREO;

    if (in_ch_layout == AV_CH_LAYOUT_STEREO_DOWNMIX
        && (out_ch_layout & AV_CH_LAYOUT_STEREO_DOWNMIX) == 0
        )
        in_ch_layout = AV_CH_LAYOUT_STEREO;

    if (!sane_layout(in_ch_layout)) {
        return AVERROR(EINVAL);
    }

    if (!sane_layout(out_ch_layout)) {
        return AVERROR(EINVAL);
    }

    memset(s->matrix, 0, sizeof(s->matrix));
    for (i = 0; i < 64; i++) {
        if (in_ch_layout & out_ch_layout & (1ULL << i))
            matrix[i][i] = 1.0;
    }

    unaccounted = in_ch_layout & ~out_ch_layout;

    //FIXME implement dolby surround
    //FIXME implement full ac3


    if (unaccounted & AV_CH_FRONT_CENTER) {
        if ((out_ch_layout & AV_CH_LAYOUT_STEREO) == AV_CH_LAYOUT_STEREO) {
            if (in_ch_layout & AV_CH_LAYOUT_STEREO) {
                matrix[FRONT_LEFT][FRONT_CENTER] += s->clev;
                matrix[FRONT_RIGHT][FRONT_CENTER] += s->clev;
            }
            else {
                matrix[FRONT_LEFT][FRONT_CENTER] += M_SQRT1_2;
                matrix[FRONT_RIGHT][FRONT_CENTER] += M_SQRT1_2;
            }
        }
        else
            assert(0);
    }
    if (unaccounted & AV_CH_LAYOUT_STEREO) {
        if (out_ch_layout & AV_CH_FRONT_CENTER) {
            matrix[FRONT_CENTER][FRONT_LEFT] += M_SQRT1_2;
            matrix[FRONT_CENTER][FRONT_RIGHT] += M_SQRT1_2;
            if (in_ch_layout & AV_CH_FRONT_CENTER)
                matrix[FRONT_CENTER][FRONT_CENTER] = s->clev * sqrt(2);
        }
        else
            assert(0);
    }

    if (unaccounted & AV_CH_BACK_CENTER) {
        if (out_ch_layout & AV_CH_BACK_LEFT) {
            matrix[BACK_LEFT][BACK_CENTER] += M_SQRT1_2;
            matrix[BACK_RIGHT][BACK_CENTER] += M_SQRT1_2;
        }
        else if (out_ch_layout & AV_CH_SIDE_LEFT) {
            matrix[SIDE_LEFT][BACK_CENTER] += M_SQRT1_2;
            matrix[SIDE_RIGHT][BACK_CENTER] += M_SQRT1_2;
        }
        else if (out_ch_layout & AV_CH_FRONT_LEFT) {
            if (matrix_encoding == AV_MATRIX_ENCODING_DOLBY ||
                matrix_encoding == AV_MATRIX_ENCODING_DPLII) {
                if (unaccounted & (AV_CH_BACK_LEFT | AV_CH_SIDE_LEFT)) {
                    matrix[FRONT_LEFT][BACK_CENTER] -= s->slev * M_SQRT1_2;
                    matrix[FRONT_RIGHT][BACK_CENTER] += s->slev * M_SQRT1_2;
                }
                else {
                    matrix[FRONT_LEFT][BACK_CENTER] -= s->slev;
                    matrix[FRONT_RIGHT][BACK_CENTER] += s->slev;
                }
            }
            else {
                matrix[FRONT_LEFT][BACK_CENTER] += s->slev * M_SQRT1_2;
                matrix[FRONT_RIGHT][BACK_CENTER] += s->slev * M_SQRT1_2;
            }
        }
        else if (out_ch_layout & AV_CH_FRONT_CENTER) {
            matrix[FRONT_CENTER][BACK_CENTER] += s->slev * M_SQRT1_2;
        }
        else
            assert(0);
    }
    if (unaccounted & AV_CH_BACK_LEFT) {
        if (out_ch_layout & AV_CH_BACK_CENTER) {
            matrix[BACK_CENTER][BACK_LEFT] += M_SQRT1_2;
            matrix[BACK_CENTER][BACK_RIGHT] += M_SQRT1_2;
        }
        else if (out_ch_layout & AV_CH_SIDE_LEFT) {
            if (in_ch_layout & AV_CH_SIDE_LEFT) {
                matrix[SIDE_LEFT][BACK_LEFT] += M_SQRT1_2;
                matrix[SIDE_RIGHT][BACK_RIGHT] += M_SQRT1_2;
            }
            else {
                matrix[SIDE_LEFT][BACK_LEFT] += 1.0;
                matrix[SIDE_RIGHT][BACK_RIGHT] += 1.0;
            }
        }
        else if (out_ch_layout & AV_CH_FRONT_LEFT) {
            if (matrix_encoding == AV_MATRIX_ENCODING_DOLBY) {
                matrix[FRONT_LEFT][BACK_LEFT] -= s->slev * M_SQRT1_2;
                matrix[FRONT_LEFT][BACK_RIGHT] -= s->slev * M_SQRT1_2;
                matrix[FRONT_RIGHT][BACK_LEFT] += s->slev * M_SQRT1_2;
                matrix[FRONT_RIGHT][BACK_RIGHT] += s->slev * M_SQRT1_2;
            }
            else if (matrix_encoding == AV_MATRIX_ENCODING_DPLII) {
                matrix[FRONT_LEFT][BACK_LEFT] -= s->slev * SQRT3_2;
                matrix[FRONT_LEFT][BACK_RIGHT] -= s->slev * M_SQRT1_2;
                matrix[FRONT_RIGHT][BACK_LEFT] += s->slev * M_SQRT1_2;
                matrix[FRONT_RIGHT][BACK_RIGHT] += s->slev * SQRT3_2;
            }
            else {
                matrix[FRONT_LEFT][BACK_LEFT] += s->slev;
                matrix[FRONT_RIGHT][BACK_RIGHT] += s->slev;
            }
        }
        else if (out_ch_layout & AV_CH_FRONT_CENTER) {
            matrix[FRONT_CENTER][BACK_LEFT] += s->slev * M_SQRT1_2;
            matrix[FRONT_CENTER][BACK_RIGHT] += s->slev * M_SQRT1_2;
        }
        else
            assert(0);
    }

    if (unaccounted & AV_CH_SIDE_LEFT) {
        if (out_ch_layout & AV_CH_BACK_LEFT) {
            /* if back channels do not exist in the input, just copy side
               channels to back channels, otherwise mix side into back */
            if (in_ch_layout & AV_CH_BACK_LEFT) {
                matrix[BACK_LEFT][SIDE_LEFT] += M_SQRT1_2;
                matrix[BACK_RIGHT][SIDE_RIGHT] += M_SQRT1_2;
            }
            else {
                matrix[BACK_LEFT][SIDE_LEFT] += 1.0;
                matrix[BACK_RIGHT][SIDE_RIGHT] += 1.0;
            }
        }
        else if (out_ch_layout & AV_CH_BACK_CENTER) {
            matrix[BACK_CENTER][SIDE_LEFT] += M_SQRT1_2;
            matrix[BACK_CENTER][SIDE_RIGHT] += M_SQRT1_2;
        }
        else if (out_ch_layout & AV_CH_FRONT_LEFT) {
            if (matrix_encoding == AV_MATRIX_ENCODING_DOLBY) {
                matrix[FRONT_LEFT][SIDE_LEFT] -= s->slev * M_SQRT1_2;
                matrix[FRONT_LEFT][SIDE_RIGHT] -= s->slev * M_SQRT1_2;
                matrix[FRONT_RIGHT][SIDE_LEFT] += s->slev * M_SQRT1_2;
                matrix[FRONT_RIGHT][SIDE_RIGHT] += s->slev * M_SQRT1_2;
            }
            else if (matrix_encoding == AV_MATRIX_ENCODING_DPLII) {
                matrix[FRONT_LEFT][SIDE_LEFT] -= s->slev * SQRT3_2;
                matrix[FRONT_LEFT][SIDE_RIGHT] -= s->slev * M_SQRT1_2;
                matrix[FRONT_RIGHT][SIDE_LEFT] += s->slev * M_SQRT1_2;
                matrix[FRONT_RIGHT][SIDE_RIGHT] += s->slev * SQRT3_2;
            }
            else {
                matrix[FRONT_LEFT][SIDE_LEFT] += s->slev;
                matrix[FRONT_RIGHT][SIDE_RIGHT] += s->slev;
            }
        }
        else if (out_ch_layout & AV_CH_FRONT_CENTER) {
            matrix[FRONT_CENTER][SIDE_LEFT] += s->slev * M_SQRT1_2;
            matrix[FRONT_CENTER][SIDE_RIGHT] += s->slev * M_SQRT1_2;
        }
        else
            assert(0);
    }

    if (unaccounted & AV_CH_FRONT_LEFT_OF_CENTER) {
        if (out_ch_layout & AV_CH_FRONT_LEFT) {
            matrix[FRONT_LEFT][FRONT_LEFT_OF_CENTER] += 1.0;
            matrix[FRONT_RIGHT][FRONT_RIGHT_OF_CENTER] += 1.0;
        }
        else if (out_ch_layout & AV_CH_FRONT_CENTER) {
            matrix[FRONT_CENTER][FRONT_LEFT_OF_CENTER] += M_SQRT1_2;
            matrix[FRONT_CENTER][FRONT_RIGHT_OF_CENTER] += M_SQRT1_2;
        }
        else
            assert(0);
    }
    /* mix LFE into front left/right or center */
    if (unaccounted & AV_CH_LOW_FREQUENCY) {
        if (out_ch_layout & AV_CH_FRONT_CENTER) {
            matrix[FRONT_CENTER][LOW_FREQUENCY] += s->lfe_mix_level;
        }
        else if (out_ch_layout & AV_CH_FRONT_LEFT) {
            matrix[FRONT_LEFT][LOW_FREQUENCY] += s->lfe_mix_level * M_SQRT1_2;
            matrix[FRONT_RIGHT][LOW_FREQUENCY] += s->lfe_mix_level * M_SQRT1_2;
        }
        else
            assert(0);
    }

    for (out_i = i = 0; i < 64; i++) {
        double sum = 0;
        int in_i = 0;
        for (j = 0; j < 64; j++) {
            s->matrix[out_i][in_i] = matrix[i][j];
            if (matrix[i][j]) {
                sum += fabs(matrix[i][j]);
            }
            if (in_ch_layout & (1ULL << j))
                in_i++;
        }
        maxcoef = FFMAX(maxcoef, sum);
        if (out_ch_layout & (1ULL << i))
            out_i++;
    }
    if (s->rematrix_volume < 0)
        maxcoef = -s->rematrix_volume;

    if (s->rematrix_maxval > 0) {
        maxval = s->rematrix_maxval;
    }
    else if (wx_get_packed_sample_fmt(s->out_sample_fmt) < AV_SAMPLE_FMT_FLT
        || wx_get_packed_sample_fmt(s->int_sample_fmt) < AV_SAMPLE_FMT_FLT) {
        maxval = 1.0;
    }
    else
        maxval = INT_MAX;

    if (maxcoef > maxval || s->rematrix_volume < 0) {
        maxcoef /= maxval;
        for (i = 0; i < SWR_CH_MAX; i++)
            for (j = 0; j < SWR_CH_MAX; j++) {
                s->matrix[i][j] /= maxcoef;
            }
    }

    if (s->rematrix_volume > 0) {
        for (i = 0; i < SWR_CH_MAX; i++)
            for (j = 0; j < SWR_CH_MAX; j++) {
                s->matrix[i][j] *= s->rematrix_volume;
            }
    }

    for (i = 0; i < wx_get_channel_layout_nb_channels(out_ch_layout); i++) {
        for (j = 0; j < wx_get_channel_layout_nb_channels(in_ch_layout); j++) {
            // wx_log(NULL, AV_LOG_DEBUG, "%f ", s->matrix[i][j]);
        }
        // wx_log(NULL, AV_LOG_DEBUG, "\n");
    }
    return 0;
}

int swri_rematrix_init(WXTcpSwrContext* s) {
    int i, j;
    int nb_in = wx_get_channel_layout_nb_channels(s->in_ch_layout);
    int nb_out = wx_get_channel_layout_nb_channels(s->out_ch_layout);

    s->mix_any_f = NULL;

    if (!s->rematrix_custom) {
        int r = auto_matrix(s);
        if (r)
            return r;
    }
    if (s->midbuf.fmt == AV_SAMPLE_FMT_S16P) {
        s->native_matrix = wx_calloc(nb_in * nb_out, sizeof(int));
        s->native_one = wx_mallocz(sizeof(int));
        for (i = 0; i < nb_out; i++)
            for (j = 0; j < nb_in; j++)
                ((int*)s->native_matrix)[i * nb_in + j] = lrintf(s->matrix[i][j] * 32768);
        *((int*)s->native_one) = 32768;
        s->mix_1_1_f = (mix_1_1_func_type*)copy_s16;
        s->mix_2_1_f = (mix_2_1_func_type*)sum2_s16;
        s->mix_any_f = (mix_any_func_type*)get_mix_any_func_s16(s);
    }
    else if (s->midbuf.fmt == AV_SAMPLE_FMT_FLTP) {
        s->native_matrix = wx_calloc(nb_in * nb_out, sizeof(float));
        s->native_one = wx_mallocz(sizeof(float));
        for (i = 0; i < nb_out; i++)
            for (j = 0; j < nb_in; j++)
                ((float*)s->native_matrix)[i * nb_in + j] = s->matrix[i][j];
        *((float*)s->native_one) = 1.0;
        s->mix_1_1_f = (mix_1_1_func_type*)copy_float;
        s->mix_2_1_f = (mix_2_1_func_type*)sum2_float;
        s->mix_any_f = (mix_any_func_type*)get_mix_any_func_float(s);
    }
    else
        assert(0);
    //FIXME quantize for integeres
    for (i = 0; i < SWR_CH_MAX; i++) {
        int ch_in = 0;
        for (j = 0; j < SWR_CH_MAX; j++) {
            s->matrix32[i][j] = lrintf(s->matrix[i][j] * 32768);
            if (s->matrix[i][j])
                s->matrix_ch[i][++ch_in] = j;
        }
        s->matrix_ch[i][0] = ch_in;
    }
    return 0;
}

void swri_rematrix_free(WXTcpSwrContext* s) {
    wx_freep(&s->native_matrix);
    wx_freep(&s->native_one);
    wx_freep(&s->native_simd_matrix);
    wx_freep(&s->native_simd_one);
}

int swri_rematrix(WXTcpSwrContext* s, AudioData* out, AudioData* in, int len, int mustcopy) {
    int out_i, in_i, i, j;
    int len1 = 0;
    int off = 0;

    if (s->mix_any_f) {
        s->mix_any_f(out->ch, (const uint8_t**)in->ch, s->native_matrix, len);
        return 0;
    }

    if (s->mix_2_1_simd || s->mix_1_1_simd) {
        len1 = len & ~15;
        off = len1 * out->bps;
    }

    assert(!s->out_ch_layout || out->ch_count == wx_get_channel_layout_nb_channels(s->out_ch_layout));
    assert(!s->in_ch_layout || in->ch_count == wx_get_channel_layout_nb_channels(s->in_ch_layout));

    for (out_i = 0; out_i < out->ch_count; out_i++) {
        switch (s->matrix_ch[out_i][0]) {
        case 0:
            if (mustcopy)
                memset(out->ch[out_i], 0, len * wx_get_bytes_per_sample(s->int_sample_fmt));
            break;
        case 1:
            in_i = s->matrix_ch[out_i][1];
            if (s->matrix[out_i][in_i] != 1.0) {
                if (s->mix_1_1_simd && len1)
                    s->mix_1_1_simd(out->ch[out_i], in->ch[in_i], s->native_simd_matrix, in->ch_count * out_i + in_i, len1);
                if (len != len1)
                    s->mix_1_1_f(out->ch[out_i] + off, in->ch[in_i] + off, s->native_matrix, in->ch_count * out_i + in_i, len - len1);
            }
            else if (mustcopy) {
                memcpy(out->ch[out_i], in->ch[in_i], len * out->bps);
            }
            else {
                out->ch[out_i] = in->ch[in_i];
            }
            break;
        case 2: {
            int in_i1 = s->matrix_ch[out_i][1];
            int in_i2 = s->matrix_ch[out_i][2];
            if (s->mix_2_1_simd && len1)
                s->mix_2_1_simd(out->ch[out_i], in->ch[in_i1], in->ch[in_i2], s->native_simd_matrix, in->ch_count * out_i + in_i1, in->ch_count * out_i + in_i2, len1);
            else
                s->mix_2_1_f(out->ch[out_i], in->ch[in_i1], in->ch[in_i2], s->native_matrix, in->ch_count * out_i + in_i1, in->ch_count * out_i + in_i2, len1);
            if (len != len1)
                s->mix_2_1_f(out->ch[out_i] + off, in->ch[in_i1] + off, in->ch[in_i2] + off, s->native_matrix, in->ch_count * out_i + in_i1, in->ch_count * out_i + in_i2, len - len1);
            break;
        }
        default:
            if (s->int_sample_fmt == AV_SAMPLE_FMT_FLTP) {
                for (i = 0; i < len; i++) {
                    float v = 0;
                    for (j = 0; j < s->matrix_ch[out_i][0]; j++) {
                        in_i = s->matrix_ch[out_i][1 + j];
                        v += ((float*)in->ch[in_i])[i] * s->matrix[out_i][in_i];
                    }
                    ((float*)out->ch[out_i])[i] = v;
                }
            }
            else {
                for (i = 0; i < len; i++) {
                    int v = 0;
                    for (j = 0; j < s->matrix_ch[out_i][0]; j++) {
                        in_i = s->matrix_ch[out_i][1 + j];
                        v += ((int16_t*)in->ch[in_i])[i] * s->matrix32[out_i][in_i];
                    }
                    ((int16_t*)out->ch[out_i])[i] = (v + 16384) >> 15;
                }
            }
        }
    }
    return 0;
}


typedef struct {
    int  rate;
    enum { fir, iir } type;
    size_t len;
    int gain_cB; /* Chosen so clips are few if any, but not guaranteed none. */
    double const* coefs;
    enum SwrDitherType name;
} filter_t;

static double const lip44[] = { 2.033, -2.165, 1.959, -1.590, .6149 };
static double const fwe44[] = {
    2.412, -3.370, 3.937, -4.174, 3.353, -2.205, 1.281, -.569, .0847 };
static double const mew44[] = {
    1.662, -1.263, .4827, -.2913, .1268, -.1124, .03252, -.01265, -.03524 };
static double const iew44[] = {
    2.847, -4.685, 6.214, -7.184, 6.639, -5.032, 3.263, -1.632, .4191 };
static double const ges44[] = {
    2.2061, -.4706, -.2534, -.6214, 1.0587, .0676, -.6054, -.2738 };
static double const ges48[] = {
    2.2374, -.7339, -.1251, -.6033, .903, .0116, -.5853, -.2571 };

static double const shi48[] = {
    2.8720729351043701172,  -5.0413231849670410156,   6.2442994117736816406,
    -5.8483986854553222656, 3.7067542076110839844,  -1.0495119094848632812,
    -1.1830236911773681641,   2.1126792430877685547, -1.9094531536102294922,
    0.99913084506988525391, -0.17090806365013122559, -0.32615602016448974609,
    0.39127644896507263184, -0.26876461505889892578,  0.097676105797290802002,
    -0.023473845794796943665,
};
static double const shi44[] = {
    2.6773197650909423828,  -4.8308925628662109375,   6.570110321044921875,
    -7.4572014808654785156, 6.7263274192810058594,  -4.8481650352478027344,
    2.0412089824676513672,   0.7006359100341796875, -2.9537565708160400391,
    4.0800385475158691406,  -4.1845216751098632812,   3.3311812877655029297,
    -2.1179926395416259766,   0.879302978515625,      -0.031759146600961685181,
    -0.42382788658142089844, 0.47882103919982910156, -0.35490813851356506348,
    0.17496839165687561035, -0.060908168554306030273,
};
static double const shi38[] = {
    1.6335992813110351562,  -2.2615492343902587891,   2.4077029228210449219,
    -2.6341717243194580078, 2.1440362930297851562,  -1.8153258562088012695,
    1.0816224813461303711,  -0.70302653312683105469, 0.15991993248462677002,
    0.041549518704414367676, -0.29416576027870178223,  0.2518316805362701416,
    -0.27766478061676025391,  0.15785403549671173096, -0.10165894031524658203,
    0.016833892092108726501,
};
static double const shi32[] =
{ /* dmaker 32000: bestmax=4.99659 (inverted) */
    0.82118552923202515,
    -1.0063692331314087,
    0.62341964244842529,
    -1.0447187423706055,
    0.64532512426376343,
    -0.87615132331848145,
    0.52219754457473755,
    -0.67434263229370117,
    0.44954317808151245,
    -0.52557498216629028,
    0.34567299485206604,
    -0.39618203043937683,
    0.26791760325431824,
    -0.28936097025871277,
    0.1883765310049057,
    -0.19097308814525604,
    0.10431359708309174,
    -0.10633844882249832,
    0.046832218766212463,
    -0.039653312414884567,
};
static double const shi22[] =
{ /* dmaker 22050: bestmax=5.77762 (inverted) */
    0.056581053882837296,
    -0.56956905126571655,
    -0.40727734565734863,
    -0.33870288729667664,
    -0.29810553789138794,
    -0.19039161503314972,
    -0.16510021686553955,
    -0.13468159735202789,
    -0.096633769571781158,
    -0.081049129366874695,
    -0.064953058958053589,
    -0.054459091275930405,
    -0.043378707021474838,
    -0.03660014271736145,
    -0.026256965473294258,
    -0.018786206841468811,
    -0.013387725688517094,
    -0.0090983230620622635,
    -0.0026585909072309732,
    -0.00042083300650119781,
};
static double const shi16[] =
{ /* dmaker 16000: bestmax=5.97128 (inverted) */
    -0.37251132726669312,
    -0.81423574686050415,
    -0.55010956525802612,
    -0.47405767440795898,
    -0.32624706625938416,
    -0.3161766529083252,
    -0.2286367267370224,
    -0.22916607558727264,
    -0.19565616548061371,
    -0.18160104751586914,
    -0.15423151850700378,
    -0.14104481041431427,
    -0.11844276636838913,
    -0.097583092749118805,
    -0.076493598520755768,
    -0.068106919527053833,
    -0.041881654411554337,
    -0.036922425031661987,
    -0.019364040344953537,
    -0.014994367957115173,
};
static double const shi11[] =
{ /* dmaker 11025: bestmax=5.9406 (inverted) */
    -0.9264228343963623,
    -0.98695987462997437,
    -0.631156325340271,
    -0.51966935396194458,
    -0.39738872647285461,
    -0.35679301619529724,
    -0.29720726609230042,
    -0.26310476660728455,
    -0.21719355881214142,
    -0.18561814725399017,
    -0.15404847264289856,
    -0.12687471508979797,
    -0.10339745879173279,
    -0.083688631653785706,
    -0.05875682458281517,
    -0.046893671154975891,
    -0.027950936928391457,
    -0.020740609616041183,
    -0.009366452693939209,
    -0.0060260160826146603,
};
static double const shi08[] =
{ /* dmaker 8000: bestmax=5.56234 (inverted) */
    -1.202863335609436,
    -0.94103097915649414,
    -0.67878556251525879,
    -0.57650017738342285,
    -0.50004476308822632,
    -0.44349345564842224,
    -0.37833768129348755,
    -0.34028723835945129,
    -0.29413089156150818,
    -0.24994957447052002,
    -0.21715600788593292,
    -0.18792112171649933,
    -0.15268312394618988,
    -0.12135542929172516,
    -0.099610626697540283,
    -0.075273610651493073,
    -0.048787496984004974,
    -0.042586319148540497,
    -0.028991291299462318,
    -0.011869125068187714,
};
static double const shl48[] = {
    2.3925774097442626953,  -3.4350297451019287109,   3.1853709220886230469,
    -1.8117271661758422852, -0.20124770700931549072,  1.4759907722473144531,
    -1.7210904359817504883,   0.97746700048446655273, -0.13790138065814971924,
    -0.38185903429985046387,  0.27421241998672485352,  0.066584214568138122559,
    -0.35223302245140075684,  0.37672343850135803223, -0.23964276909828186035,
    0.068674825131893157959,
};
static double const shl44[] = {
    2.0833916664123535156,  -3.0418450832366943359,   3.2047898769378662109,
    -2.7571926116943359375, 1.4978630542755126953,  -0.3427594602108001709,
    -0.71733748912811279297,  1.0737057924270629883, -1.0225815773010253906,
    0.56649994850158691406, -0.20968692004680633545, -0.065378531813621520996,
    0.10322438180446624756, -0.067442022264003753662, -0.00495197344571352005,
    0,
};
static double const shh44[] = {
    3.0259189605712890625, -6.0268716812133789062,   9.195003509521484375,
    -11.824929237365722656, 12.767142295837402344, -11.917946815490722656,
    9.1739168167114257812,  -5.3712320327758789062, 1.1393624544143676758,
    2.4484779834747314453,  -4.9719839096069335938,   6.0392003059387207031,
    -5.9359521865844726562,  4.903278350830078125,   -3.5527443885803222656,
    2.1909697055816650391, -1.1672389507293701172,  0.4903914332389831543,
    -0.16519790887832641602,  0.023217858746647834778,
};

static const filter_t filters[] = {
    { 44100, fir,  5, 210, lip44,          SWR_DITHER_NS_LIPSHITZ },
    { 46000, fir,  9, 276, fwe44,          SWR_DITHER_NS_F_WEIGHTED },
    { 46000, fir,  9, 160, mew44,          SWR_DITHER_NS_MODIFIED_E_WEIGHTED },
    { 46000, fir,  9, 321, iew44,          SWR_DITHER_NS_IMPROVED_E_WEIGHTED },
    //   {48000, iir,  4, 220, ges48, SWR_DITHER_NS_GESEMANN},
    //   {44100, iir,  4, 230, ges44, SWR_DITHER_NS_GESEMANN},
    { 48000, fir, 16, 301, shi48,          SWR_DITHER_NS_SHIBATA },
    { 44100, fir, 20, 333, shi44,          SWR_DITHER_NS_SHIBATA },
    { 37800, fir, 16, 240, shi38,          SWR_DITHER_NS_SHIBATA },
    { 32000, fir, 20, 240/*TBD*/, shi32,   SWR_DITHER_NS_SHIBATA },
    { 22050, fir, 20, 240/*TBD*/, shi22,   SWR_DITHER_NS_SHIBATA },
    { 16000, fir, 20, 240/*TBD*/, shi16,   SWR_DITHER_NS_SHIBATA },
    { 11025, fir, 20, 240/*TBD*/, shi11,   SWR_DITHER_NS_SHIBATA },
    { 8000, fir, 20, 240/*TBD*/, shi08,   SWR_DITHER_NS_SHIBATA },
    { 48000, fir, 16, 250, shl48,          SWR_DITHER_NS_LOW_SHIBATA },
    { 44100, fir, 15, 250, shl44,          SWR_DITHER_NS_LOW_SHIBATA },
    { 44100, fir, 20, 383, shh44,          SWR_DITHER_NS_HIGH_SHIBATA },
    { 0, fir,  0,   0,  NULL,          SWR_DITHER_NONE },
};


void swri_get_dither(WXTcpSwrContext* s, void* dst, int len, unsigned seed, enum WXTcpFormat noise_fmt) {
    double scale = s->dither.noise_scale;
#define TMP_EXTRA 2
    double* tmp = wx_malloc_array(len + TMP_EXTRA, sizeof(double));
    int i;

    for (i = 0; i < len + TMP_EXTRA; i++) {
        double v;
        seed = seed * 1664525 + 1013904223;

        switch (s->dither.method) {
        case SWR_DITHER_RECTANGULAR: v = ((double)seed) / UINT_MAX - 0.5; break;
        default:
            assert(s->dither.method < SWR_DITHER_NB);
            v = ((double)seed) / UINT_MAX;
            seed = seed * 1664525 + 1013904223;
            v -= ((double)seed) / UINT_MAX;
            break;
        }
        tmp[i] = v;
    }

    for (i = 0; i < len; i++) {
        double v;

        switch (s->dither.method) {
        default:
            assert(s->dither.method < SWR_DITHER_NB);
            v = tmp[i];
            break;
        case SWR_DITHER_TRIANGULAR_HIGHPASS:
            v = (-tmp[i] + 2 * tmp[i + 1] - tmp[i + 2]) / sqrt(6);
            break;
        }

        v *= scale;

        switch (noise_fmt) {
        case AV_SAMPLE_FMT_S16P: ((int16_t*)dst)[i] = v; break;
        case AV_SAMPLE_FMT_FLTP: ((float*)dst)[i] = v; break;
        default: assert(0);
        }
    }

    wx_free(tmp);
}

int swri_dither_init(WXTcpSwrContext* s, enum WXTcpFormat out_fmt, enum WXTcpFormat in_fmt)
{
    int i;
    double scale = 0;

    if (s->dither.method > SWR_DITHER_TRIANGULAR_HIGHPASS && s->dither.method <= SWR_DITHER_NS)
        return AVERROR(EINVAL);

    out_fmt = wx_get_packed_sample_fmt(out_fmt);
    in_fmt = wx_get_packed_sample_fmt(in_fmt);

    if (in_fmt == AV_SAMPLE_FMT_FLT) {
        if (out_fmt == AV_SAMPLE_FMT_S16) scale = 1.0 / (1L << 15);
    }

    scale *= s->dither.scale;

    s->dither.ns_pos = 0;
    s->dither.noise_scale = scale;
    s->dither.ns_scale = scale;
    s->dither.ns_scale_1 = scale ? 1 / scale : 0;
    memset(s->dither.ns_errors, 0, sizeof(s->dither.ns_errors));
    for (i = 0; filters[i].coefs; i++) {
        const filter_t* f = &filters[i];
        if (fabs(s->out_sample_rate - f->rate) / f->rate <= .05 && f->name == s->dither.method) {
            int j;
            s->dither.ns_taps = f->len;
            for (j = 0; j < f->len; j++)
                s->dither.ns_coeffs[j] = f->coefs[j];
            s->dither.ns_scale_1 *= 1 - exp(f->gain_cB * M_LN10 * 0.005) * 2 / (1 << (8 * wx_get_bytes_per_sample(out_fmt)));
            break;
        }
    }
    if (!filters[i].coefs && s->dither.method > SWR_DITHER_NS) {
        // wx_log(s, AV_LOG_WARNING, "Requested noise shaping dither not available at this sampling rate, using triangular hp dither\n");
        s->dither.method = SWR_DITHER_TRIANGULAR_HIGHPASS;
    }

    assert(!s->preout.count);
    s->dither.noise = s->preout;
    s->dither.temp = s->preout;
    if (s->dither.method > SWR_DITHER_NS) {
        s->dither.noise.bps = 4;
        s->dither.noise.fmt = AV_SAMPLE_FMT_FLTP;
        s->dither.noise_scale = 1;
    }

    return 0;
}

#define TEMPLATE_DITHER_S16
#include "WXResample_dither_template.h"
#undef TEMPLATE_DITHER_S16

#define TEMPLATE_DITHER_FLT
#include "WXResample_dither_template.h"
#undef TEMPLATE_DITHER_FLT




#define CONV_FUNC_NAME(dst_fmt, src_fmt) conv_ ## src_fmt ## _to_ ## dst_fmt

//FIXME rounding ?
#define CONV_FUNC(ofmt, otype, ifmt, expr)\
static void CONV_FUNC_NAME(ofmt, ifmt)(uint8_t *po, const uint8_t *pi, int is, int os, uint8_t *end)\
{\
    uint8_t *end2 = end - 3*os;\
    while(po < end2){\
        *(otype*)po = expr; pi += is; po += os;\
        *(otype*)po = expr; pi += is; po += os;\
        *(otype*)po = expr; pi += is; po += os;\
        *(otype*)po = expr; pi += is; po += os;\
    }\
    while(po < end){\
        *(otype*)po = expr; pi += is; po += os;\
    }\
}

//FIXME put things below under ifdefs so we do not waste space for cases no codec will need


CONV_FUNC(AV_SAMPLE_FMT_S16, int16_t, AV_SAMPLE_FMT_S16, *(const int16_t*)pi)
CONV_FUNC(AV_SAMPLE_FMT_FLT, float, AV_SAMPLE_FMT_S16, *(const int16_t*)pi* (1.0f / (1 << 15)))

CONV_FUNC(AV_SAMPLE_FMT_S16, int16_t, AV_SAMPLE_FMT_FLT, wx_clip_int16(lrintf(*(const float*)pi* (1 << 15))))
CONV_FUNC(AV_SAMPLE_FMT_FLT, float, AV_SAMPLE_FMT_FLT, *(const float*)pi)

#define FMT_PAIR_FUNC(out, in) [(out) + AV_SAMPLE_FMT_NB*(in)] = CONV_FUNC_NAME(out, in)

static conv_func_type* const fmt_pair_to_conv_functions[AV_SAMPLE_FMT_NB * AV_SAMPLE_FMT_NB] = {
    FMT_PAIR_FUNC(AV_SAMPLE_FMT_S16, AV_SAMPLE_FMT_S16),
    FMT_PAIR_FUNC(AV_SAMPLE_FMT_FLT, AV_SAMPLE_FMT_S16),
    FMT_PAIR_FUNC(AV_SAMPLE_FMT_S16, AV_SAMPLE_FMT_FLT),
    FMT_PAIR_FUNC(AV_SAMPLE_FMT_FLT, AV_SAMPLE_FMT_FLT),
};

static void cpy1(uint8_t** dst, const uint8_t** src, int len) {
    memcpy(*dst, *src, len);
}
static void cpy2(uint8_t** dst, const uint8_t** src, int len) {
    memcpy(*dst, *src, 2 * len);
}
static void cpy4(uint8_t** dst, const uint8_t** src, int len) {
    memcpy(*dst, *src, 4 * len);
}
static void cpy8(uint8_t** dst, const uint8_t** src, int len) {
    memcpy(*dst, *src, 8 * len);
}

AudioConvert* swri_audio_convert_alloc(enum WXTcpFormat out_fmt,
    enum WXTcpFormat in_fmt,
    int channels, const int* ch_map,
    int flags)
{
    AudioConvert* ctx;
    conv_func_type* f = fmt_pair_to_conv_functions[wx_get_packed_sample_fmt(out_fmt) + AV_SAMPLE_FMT_NB * wx_get_packed_sample_fmt(in_fmt)];

    if (!f)
        return NULL;
    ctx = wx_mallocz(sizeof(*ctx));
    if (!ctx)
        return NULL;

    if (channels == 1) {
        in_fmt = wx_get_planar_sample_fmt(in_fmt);
        out_fmt = wx_get_planar_sample_fmt(out_fmt);
    }

    ctx->channels = channels;
    ctx->conv_f = f;
    ctx->ch_map = ch_map;

    if (out_fmt == in_fmt && !ch_map) {
        switch (wx_get_bytes_per_sample(in_fmt)) {
        case 1:ctx->simd_f = cpy1; break;
        case 2:ctx->simd_f = cpy2; break;
        case 4:ctx->simd_f = cpy4; break;
        case 8:ctx->simd_f = cpy8; break;
        }
    }
    return ctx;
}

void swri_audio_convert_free(AudioConvert** ctx)
{
    wx_freep(ctx);
}

int swri_audio_convert(AudioConvert* ctx, AudioData* out, AudioData* in, int len)
{
    int ch;
    int off = 0;
    const int os = (out->planar ? 1 : out->ch_count) * out->bps;
    unsigned misaligned = 0;

    assert(ctx->channels == out->ch_count);

    if (ctx->in_simd_align_mask) {
        int planes = in->planar ? in->ch_count : 1;
        unsigned m = 0;
        for (ch = 0; ch < planes; ch++)
            m |= (intptr_t)in->ch[ch];
        misaligned |= m & ctx->in_simd_align_mask;
    }
    if (ctx->out_simd_align_mask) {
        int planes = out->planar ? out->ch_count : 1;
        unsigned m = 0;
        for (ch = 0; ch < planes; ch++)
            m |= (intptr_t)out->ch[ch];
        misaligned |= m & ctx->out_simd_align_mask;
    }

    //FIXME optimize common cases

    if (ctx->simd_f && !ctx->ch_map && !misaligned) {
        off = len & ~15;
        assert(off >= 0);
        assert(off <= len);
        assert(ctx->channels == SWR_CH_MAX || !in->ch[ctx->channels]);
        if (off > 0) {
            if (out->planar == in->planar) {
                int planes = out->planar ? out->ch_count : 1;
                for (ch = 0; ch < planes; ch++) {
                    ctx->simd_f(out->ch + ch, (const uint8_t**)in->ch + ch, off * (out->planar ? 1 : out->ch_count));
                }
            }
            else {
                ctx->simd_f(out->ch, (const uint8_t**)in->ch, off);
            }
        }
        if (off == len)
            return 0;
    }

    for (ch = 0; ch < ctx->channels; ch++) {
        const int ich = ctx->ch_map ? ctx->ch_map[ch] : ch;
        const int is = ich < 0 ? 0 : (in->planar ? 1 : in->ch_count) * in->bps;
        const uint8_t* pi = ich < 0 ? ctx->silence : in->ch[ich];
        uint8_t* po = out->ch[ch];
        uint8_t* end = po + os * len;
        if (!po)
            continue;
        ctx->conv_f(po + off * os, pi + off * is, is, os, end);
    }
    return 0;
}
