/*
** © 2009-2018 by Kornel Lesiński.
** © 1989, 1991 by Jef Poskanzer.
** © 1997, 2000, 2002 by Greg Roelofs; based on an idea by Stefan Schneider.
**
** See COPYRIGHT file for license.
*/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <limits.h>
#include "libimagequant.h"

#define LIQ_PRIVATE
#define LIQ_NONNULL
#define LIQ_USERESULT

#ifndef LIQ_EXPORT
#define LIQ_EXPORT extern
#endif

#define LIQ_VERSION 21501
#define LIQ_VERSION_STRING "2.15.1"

// accidental debug assertions make color search much slower,
// so force assertions off if there's no explicit setting
#if !defined(NDEBUG) && !defined(DEBUG)
#define NDEBUG
#endif

#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

#ifndef MAX
#  define MAX(a,b)  ((a) > (b)? (a) : (b))
#  define MIN(a,b)  ((a) < (b)? (a) : (b))
#endif

#define MAX_DIFF 1e20

#define __SSE__
#ifndef USE_SSE
#  if defined(__SSE__) && (defined(__amd64__) || defined(__X86_64__) || defined(_WIN64) || defined(_WIN32) || defined(__WIN32__))
#    define USE_SSE 1
#  else
#    define USE_SSE 0
#  endif
#endif

#if USE_SSE
#  include <xmmintrin.h>
#  ifdef _MSC_VER
#    include <intrin.h>
#    define SSE_ALIGN
#  else
#    define SSE_ALIGN __attribute__ ((aligned (16)))
#    if defined(__i386__) && defined(__PIC__)
#       define cpuid(func,ax,bx,cx,dx)\
        __asm__ __volatile__ ( \
        "push %%ebx\n" \
        "cpuid\n" \
        "mov %%ebx, %1\n" \
        "pop %%ebx\n" \
        : "=a" (ax), "=r" (bx), "=c" (cx), "=d" (dx) \
        : "a" (func));
#    else
#       define cpuid(func,ax,bx,cx,dx)\
        __asm__ __volatile__ ("cpuid":\
        "=a" (ax), "=b" (bx), "=c" (cx), "=d" (dx) : "a" (func));
#    endif
#endif
#else
#  define SSE_ALIGN
#endif

#ifndef _MSC_VER
#define LIQ_ARRAY(type, var, count) type var[count]
#else
#define LIQ_ARRAY(type, var, count) type* var = (type*)_alloca(sizeof(type)*(count))
#endif

#if defined(__GNUC__) || defined (__llvm__)
#define ALWAYS_INLINE __attribute__((always_inline)) inline
#define NEVER_INLINE __attribute__ ((noinline))
#elif defined(_MSC_VER)
#define inline __inline
#define restrict __restrict
#define ALWAYS_INLINE __forceinline
#define NEVER_INLINE __declspec(noinline)
#else
#define ALWAYS_INLINE inline
#define NEVER_INLINE
#endif


// Spread memory touched by different threads at least 64B apart which I assume is the cache line size. This should avoid memory write contention.
#define KMEANS_CACHE_LINE_GAP ((64+sizeof(kmeans_state)-1)/sizeof(kmeans_state))

#define LIQ_TEMP_ROW_WIDTH(img_width) (img_width)
#define omp_get_max_threads() 1
#define omp_get_thread_num() 0

typedef struct {
    double a, r, g, b, total;
} kmeans_state;
/* from pam.h */

typedef struct {
    unsigned char r, g, b, a;
} rgba_pixel;

typedef struct {
    float a, r, g, b;
} SSE_ALIGN f_pixel;

/* from pamcmap.h */
union rgba_as_int {
    rgba_pixel rgba;
    unsigned int l;
};

typedef struct {
    f_pixel acolor;
    float adjusted_weight,   // perceptual weight changed to tweak how mediancut selects colors
        perceptual_weight; // number of pixels weighted by importance of different areas of the picture

    float color_weight;      // these two change every time histogram subset is sorted
    union {
        unsigned int sort_value;
        unsigned char likely_colormap_index;
    } tmp;
} hist_item;

struct nearest_map;
struct mempool;
typedef struct mempool* mempoolptr;

typedef void (*kmeans_callback)(hist_item* item, float diff);
typedef struct {
    hist_item* achv;
    void (*free)(void*);
    double total_perceptual_weight;
    unsigned int size;
    unsigned int ignorebits;
} histogram;

typedef struct {
    f_pixel acolor;
    float popularity;
    bool fixed; // if true it's user-supplied and must not be changed (e.g in K-Means iteration)
} colormap_item;

typedef struct colormap {
    unsigned int colors;
    void* (*malloc)(size_t);
    void (*free)(void*);
    colormap_item palette[];
} colormap;

struct acolorhist_arr_item {
    union rgba_as_int color;
    unsigned int perceptual_weight;
};

struct acolorhist_arr_head {
    struct acolorhist_arr_item inline1, inline2;
    unsigned int used, capacity;
    struct acolorhist_arr_item* other_items;
};

struct acolorhash_table {
    struct mempool* mempool;
    unsigned int ignorebits, maxcolors, colors, cols, rows;
    unsigned int hash_size;
    unsigned int freestackp;
    struct acolorhist_arr_item* freestack[512];
    struct acolorhist_arr_head buckets[];
};

struct liq_attr {
    const char* magic_header;
    void* (*malloc)(size_t);
    void (*free)(void*);

    double target_mse, max_mse, kmeans_iteration_limit;
    unsigned int max_colors, max_histogram_entries;
    unsigned int min_posterization_output /* user setting */, min_posterization_input /* speed setting */;
    unsigned int kmeans_iterations, feedback_loop_trials;
    bool last_index_transparent, use_contrast_maps;
    unsigned char use_dither_map;
    unsigned char speed;

    unsigned char progress_stage1, progress_stage2, progress_stage3;
    liq_progress_callback_function* progress_callback;
    void* progress_callback_user_info;

    liq_log_callback_function* log_callback;
    void* log_callback_user_info;
    liq_log_flush_callback_function* log_flush_callback;
    void* log_flush_callback_user_info;
};

struct liq_image {
    const char* magic_header;
    void* (*malloc)(size_t);
    void (*free)(void*);

    f_pixel* f_pixels;
    rgba_pixel** rows;
    double gamma;
    unsigned int width, height;
    unsigned char* importance_map, * edges, * dither_map;
    rgba_pixel* pixels, * temp_row;
    f_pixel* temp_f_row;
    liq_image_get_rgba_row_callback* row_callback;
    void* row_callback_user_info;
    liq_image* background;
    f_pixel fixed_colors[256];
    unsigned short fixed_colors_count;
    bool free_pixels, free_rows, free_rows_internal;
};

typedef struct liq_remapping_result {
    const char* magic_header;
    void* (*malloc)(size_t);
    void (*free)(void*);

    unsigned char* pixels;
    colormap* palette;
    liq_progress_callback_function* progress_callback;
    void* progress_callback_user_info;

    liq_palette int_palette;
    double gamma, palette_error;
    float dither_level;
    unsigned char use_dither_map;
    unsigned char progress_stage1;
} liq_remapping_result;

struct liq_result {
    const char* magic_header;
    void* (*malloc)(size_t);
    void (*free)(void*);

    liq_remapping_result* remapping;
    colormap* palette;
    liq_progress_callback_function* progress_callback;
    void* progress_callback_user_info;

    liq_palette int_palette;
    float dither_level;
    double gamma, palette_error;
    int min_posterization_output;
    unsigned char use_dither_map;
};

struct liq_histogram {
    const char* magic_header;
    void* (*malloc)(size_t);
    void (*free)(void*);

    struct acolorhash_table* acht;
    double gamma;
    f_pixel fixed_colors[256];
    unsigned short fixed_colors_count;
    unsigned short ignorebits;
    bool had_image_added;
};

typedef struct vp_sort_tmp {
    float distance_squared;
    unsigned int idx;
} vp_sort_tmp;

typedef struct vp_search_tmp {
    float distance;
    float distance_squared;
    unsigned int idx;
    int exclude;
} vp_search_tmp;

struct leaf {
    f_pixel color;
    unsigned int idx;
};

typedef struct vp_node {
    struct vp_node* near, * far;
    f_pixel vantage_point;
    float radius, radius_squared;
    struct leaf* rest;
    unsigned short idx;
    unsigned short restcount;
} vp_node;

struct nearest_map {
    vp_node* root;
    const colormap_item* palette;
    float nearest_other_color_dist[256];
    mempoolptr mempool;
};

typedef struct {
    unsigned int chan; float variance;
} channelvariance;

typedef void free_func(void*);

static const float internal_gamma = 0.5499f;

LIQ_PRIVATE void liq_blur(unsigned char* src, unsigned char* tmp, unsigned char* dst, unsigned int width, unsigned int height, unsigned int size);
LIQ_PRIVATE void liq_max3(unsigned char* src, unsigned char* dst, unsigned int width, unsigned int height);
LIQ_PRIVATE void liq_min3(unsigned char* src, unsigned char* dst, unsigned int width, unsigned int height);

LIQ_PRIVATE void* mempool_create(mempoolptr* mptr, const unsigned int size, unsigned int capacity, void* (*malloc)(size_t), void (*free)(void*));
LIQ_PRIVATE void* mempool_alloc(mempoolptr* mptr, const unsigned int size, const unsigned int capacity);
LIQ_PRIVATE void mempool_destroy(mempoolptr m);

LIQ_PRIVATE struct nearest_map* nearest_init(const colormap* palette);
LIQ_PRIVATE unsigned int nearest_search(const struct nearest_map* map, const f_pixel* px, const int palette_index_guess, float* diff);
LIQ_PRIVATE void nearest_free(struct nearest_map* map);

LIQ_PRIVATE void to_f_set_gamma(float gamma_lut[], const double gamma);

/**
 Converts 8-bit color to internal gamma and premultiplied alpha.
 (premultiplied color space is much better for blending of semitransparent colors)
 */
ALWAYS_INLINE static f_pixel rgba_to_f(const float gamma_lut[], const rgba_pixel px);
inline static f_pixel rgba_to_f(const float gamma_lut[], const rgba_pixel px)
{
    float a = px.a / 255.f;

    return (f_pixel) {
        .a = a,
            .r = gamma_lut[px.r] * a,
            .g = gamma_lut[px.g] * a,
            .b = gamma_lut[px.b] * a,
    };
}

inline static rgba_pixel f_to_rgb(const float gamma, const f_pixel px)
{
    if (px.a < 1.f / 256.f) {
        return (rgba_pixel) { 0, 0, 0, 0 };
    }

    float r = px.r / px.a,
        g = px.g / px.a,
        b = px.b / px.a,
        a = px.a;

    r = powf(r, gamma / internal_gamma);
    g = powf(g, gamma / internal_gamma);
    b = powf(b, gamma / internal_gamma);

    // 256, because numbers are in range 1..255.9999… rounded down
    r *= 256.f;
    g *= 256.f;
    b *= 256.f;
    a *= 256.f;

    return (rgba_pixel) {
        .r = r >= 255.f ? 255 : r,
            .g = g >= 255.f ? 255 : g,
            .b = b >= 255.f ? 255 : b,
            .a = a >= 255.f ? 255 : a,
    };
}

ALWAYS_INLINE static double colordifference_ch(const double x, const double y, const double alphas);
inline static double colordifference_ch(const double x, const double y, const double alphas)
{
    // maximum of channel blended on white, and blended on black
    // premultiplied alpha and backgrounds 0/1 shorten the formula
    const double black = x - y, white = black + alphas;
    return MAX(black * black, white * white);
}

ALWAYS_INLINE static float colordifference_stdc(const f_pixel px, const f_pixel py);
inline static float colordifference_stdc(const f_pixel px, const f_pixel py)
{
    // px_b.rgb = px.rgb + 0*(1-px.a) // blend px on black
    // px_b.a   = px.a   + 1*(1-px.a)
    // px_w.rgb = px.rgb + 1*(1-px.a) // blend px on white
    // px_w.a   = px.a   + 1*(1-px.a)

    // px_b.rgb = px.rgb              // difference same as in opaque RGB
    // px_b.a   = 1
    // px_w.rgb = px.rgb - px.a       // difference simplifies to formula below
    // px_w.a   = 1

    // (px.rgb - px.a) - (py.rgb - py.a)
    // (px.rgb - py.rgb) + (py.a - px.a)

    const double alphas = py.a - px.a;
    return colordifference_ch(px.r, py.r, alphas) +
        colordifference_ch(px.g, py.g, alphas) +
        colordifference_ch(px.b, py.b, alphas);
}

ALWAYS_INLINE static float colordifference(f_pixel px, f_pixel py);
inline static float colordifference(f_pixel px, f_pixel py)
{
#if USE_SSE
#ifdef _MSC_VER
    /* In MSVC we cannot use the align attribute in parameters.
     * This is used a lot, so we just use an unaligned load.
     * Also the compiler incorrectly inlines vpx and vpy without
     * the volatile when optimization is applied for x86_64. */
    const volatile __m128 vpx = _mm_loadu_ps((const float*)&px);
    const volatile __m128 vpy = _mm_loadu_ps((const float*)&py);
#else
    const __m128 vpx = _mm_load_ps((const float*)&px);
    const __m128 vpy = _mm_load_ps((const float*)&py);
#endif

    // y.a - x.a
    __m128 alphas = _mm_sub_ss(vpy, vpx);
    alphas = _mm_shuffle_ps(alphas, alphas, 0); // copy first to all four

    __m128 onblack = _mm_sub_ps(vpx, vpy); // x - y
    __m128 onwhite = _mm_add_ps(onblack, alphas); // x - y + (y.a - x.a)

    onblack = _mm_mul_ps(onblack, onblack);
    onwhite = _mm_mul_ps(onwhite, onwhite);
    const __m128 max = _mm_max_ps(onwhite, onblack);

    // add rgb, not a
    const __m128 maxhl = _mm_movehl_ps(max, max);
    const __m128 tmp = _mm_add_ps(max, maxhl);
    const __m128 sum = _mm_add_ss(maxhl, _mm_shuffle_ps(tmp, tmp, 1));

    const float res = _mm_cvtss_f32(sum);
    assert(fabs(res - colordifference_stdc(px, py)) < 0.001);
    return res;
#else
    return colordifference_stdc(px, py);
#endif
}


LIQ_PRIVATE void kmeans_init(const colormap* map, const unsigned int max_threads, kmeans_state state[]);
LIQ_PRIVATE void kmeans_update_color(const f_pixel acolor, const float value, const colormap* map, unsigned int match, const unsigned int thread, kmeans_state average_color[]);
LIQ_PRIVATE void kmeans_finalize(colormap* map, const unsigned int max_threads, const kmeans_state state[]);
LIQ_PRIVATE double kmeans_do_iteration(histogram* hist, colormap* const map, kmeans_callback callback);

LIQ_PRIVATE colormap* mediancut(histogram* hist, unsigned int newcolors, const double target_mse, const double max_mse, void* (*malloc)(size_t), void (*free)(void*));

LIQ_PRIVATE void pam_freeacolorhash(struct acolorhash_table* acht);
LIQ_PRIVATE struct acolorhash_table* pam_allocacolorhash(unsigned int maxcolors, unsigned int surface, unsigned int ignorebits, void* (*malloc)(size_t), void (*free)(void*));
LIQ_PRIVATE histogram* pam_acolorhashtoacolorhist(const struct acolorhash_table* acht, const double gamma, void* (*malloc)(size_t), void (*free)(void*));
LIQ_PRIVATE bool pam_computeacolorhash(struct acolorhash_table* acht, const rgba_pixel* const pixels[], unsigned int cols, unsigned int rows, const unsigned char* importance_map);
LIQ_PRIVATE bool pam_add_to_hash(struct acolorhash_table* acht, unsigned int hash, unsigned int boost, union rgba_as_int px, unsigned int row, unsigned int rows);

LIQ_PRIVATE void pam_freeacolorhist(histogram* h);

LIQ_PRIVATE colormap* pam_colormap(unsigned int colors, void* (*malloc)(size_t), void (*free)(void*));
LIQ_PRIVATE colormap* pam_duplicate_colormap(colormap* map);
LIQ_PRIVATE void pam_freecolormap(colormap* c);

#define LIQ_HIGH_MEMORY_LIMIT (1<<26)  /* avoid allocating buffers larger than 64MB */

// each structure has a pointer as a unique identifier that allows type checking at run time
static const char liq_attr_magic[] = "liq_attr";
static const char liq_image_magic[] = "liq_image";
static const char liq_result_magic[] = "liq_result";
static const char liq_histogram_magic[] = "liq_histogram";
static const char liq_remapping_result_magic[] = "liq_remapping_result";
static const char liq_freed_magic[] = "free";
#define CHECK_STRUCT_TYPE(attr, kind) liq_crash_if_invalid_handle_pointer_given((const liq_attr*)attr, kind ## _magic)
#define CHECK_USER_POINTER(ptr) liq_crash_if_invalid_pointer_given(ptr)



static void contrast_maps(liq_image *image) LIQ_NONNULL;
static liq_error finalize_histogram(liq_histogram *input_hist, liq_attr *options, histogram **hist_output) LIQ_NONNULL;
static const rgba_pixel *liq_image_get_row_rgba(liq_image *input_image, unsigned int row) LIQ_NONNULL;
static bool liq_image_get_row_f_init(liq_image *img) LIQ_NONNULL;
static const f_pixel *liq_image_get_row_f(liq_image *input_image, unsigned int row) LIQ_NONNULL;
static void liq_remapping_result_destroy(liq_remapping_result *result) LIQ_NONNULL;
static liq_error pngquant_quantize(histogram *hist, const liq_attr *options, const int fixed_colors_count, const f_pixel fixed_colors[], const double gamma, bool fixed_result_colors, liq_result **) LIQ_NONNULL;
static liq_error liq_histogram_quantize_internal(liq_histogram *input_hist, liq_attr *attr, bool fixed_result_colors, liq_result **result_output) LIQ_NONNULL;

LIQ_NONNULL static void liq_verbose_printf(const liq_attr *context, const char *fmt, ...)
{
    if (context->log_callback) {
        va_list va;
        va_start(va, fmt);
        int required_space = vsnprintf(NULL, 0, fmt, va)+1; // +\0
        va_end(va);

        LIQ_ARRAY(char, buf, required_space);
        va_start(va, fmt);
        vsnprintf(buf, required_space, fmt, va);
        va_end(va);

        context->log_callback(context, buf, context->log_callback_user_info);
    }
}

LIQ_NONNULL inline static void verbose_print(const liq_attr *attr, const char *msg)
{
    if (attr->log_callback) {
        attr->log_callback(attr, msg, attr->log_callback_user_info);
    }
}

LIQ_NONNULL static void liq_verbose_printf_flush(liq_attr *attr)
{
    if (attr->log_flush_callback) {
        attr->log_flush_callback(attr, attr->log_flush_callback_user_info);
    }
}

LIQ_NONNULL static bool liq_progress(const liq_attr *attr, const float percent)
{
    return attr->progress_callback && !attr->progress_callback(percent, attr->progress_callback_user_info);
}

LIQ_NONNULL static bool liq_remap_progress(const liq_remapping_result *quant, const float percent)
{
    return quant->progress_callback && !quant->progress_callback(percent, quant->progress_callback_user_info);
}

#if USE_SSE
inline static bool is_sse_available()
{
#if (defined(__x86_64__) || defined(__amd64) || defined(_WIN64))
    return true;
#elif _MSC_VER
    int info[4];
    __cpuid(info, 1);
    /* bool is implemented as a built-in type of size 1 in MSVC */
    return info[3] & (1<<26) ? true : false;
#else
    int a,b,c,d;
        cpuid(1, a, b, c, d);
    return d & (1<<25); // edx bit 25 is set when SSE is present
#endif
}
#endif

/* make it clear in backtrace when user-supplied handle points to invalid memory */
NEVER_INLINE LIQ_EXPORT bool liq_crash_if_invalid_handle_pointer_given(const liq_attr *user_supplied_pointer, const char *const expected_magic_header);
LIQ_EXPORT bool liq_crash_if_invalid_handle_pointer_given(const liq_attr *user_supplied_pointer, const char *const expected_magic_header)
{
    if (!user_supplied_pointer) {
        return false;
    }

    if (user_supplied_pointer->magic_header == liq_freed_magic) {
        fprintf(stderr, "%s used after being freed", expected_magic_header);
        // this is not normal error handling, this is programmer error that should crash the program.
        // program cannot safely continue if memory has been used after it's been freed.
        // abort() is nasty, but security vulnerability may be worse.
        abort();
    }

    return user_supplied_pointer->magic_header == expected_magic_header;
}

NEVER_INLINE LIQ_EXPORT bool liq_crash_if_invalid_pointer_given(const void *pointer);
LIQ_EXPORT bool liq_crash_if_invalid_pointer_given(const void *pointer)
{
    if (!pointer) {
        return false;
    }
    // Force a read from the given (potentially invalid) memory location in order to check early whether this crashes the program or not.
    // It doesn't matter what value is read, the code here is just to shut the compiler up about unused read.
    char test_access = *((volatile char *)pointer);
    return test_access || true;
}

LIQ_NONNULL static void liq_log_error(const liq_attr *attr, const char *msg)
{
    if (!CHECK_STRUCT_TYPE(attr, liq_attr)) return;
    liq_verbose_printf(attr, "  error: %s", msg);
}

static double quality_to_mse(long quality)
{
    if (quality == 0) {
        return MAX_DIFF;
    }
    if (quality == 100) {
        return 0;
    }

    // curve fudged to be roughly similar to quality of libjpeg
    // except lowest 10 for really low number of colors
    const double extra_low_quality_fudge = MAX(0,0.016/(0.001+quality) - 0.001);
    return extra_low_quality_fudge + 2.5/pow(210.0 + quality, 1.2) * (100.1-quality)/100.0;
}

static unsigned int mse_to_quality(double mse)
{
    for(int i=100; i > 0; i--) {
        if (mse <= quality_to_mse(i) + 0.000001) { // + epsilon for floating point errors
            return i;
        }
    }
    return 0;
}

/** internally MSE is a sum of all channels with pixels 0..1 range,
 but other software gives per-RGB-channel MSE for 0..255 range */
static double mse_to_standard_mse(double mse) {
    return mse * 65536.0/6.0;
}

LIQ_EXPORT LIQ_NONNULL liq_error liq_set_quality(liq_attr* attr, int minimum, int target)
{
    if (!CHECK_STRUCT_TYPE(attr, liq_attr)) return LIQ_INVALID_POINTER;
    if (target < 0 || target > 100 || target < minimum || minimum < 0) return LIQ_VALUE_OUT_OF_RANGE;

    attr->target_mse = quality_to_mse(target);
    attr->max_mse = quality_to_mse(minimum);
    return LIQ_OK;
}

LIQ_EXPORT LIQ_NONNULL int liq_get_min_quality(const liq_attr *attr)
{
    if (!CHECK_STRUCT_TYPE(attr, liq_attr)) return -1;
    return mse_to_quality(attr->max_mse);
}

LIQ_EXPORT LIQ_NONNULL int liq_get_max_quality(const liq_attr *attr)
{
    if (!CHECK_STRUCT_TYPE(attr, liq_attr)) return -1;
    return mse_to_quality(attr->target_mse);
}


LIQ_EXPORT LIQ_NONNULL liq_error liq_set_max_colors(liq_attr* attr, int colors)
{
    if (!CHECK_STRUCT_TYPE(attr, liq_attr)) return LIQ_INVALID_POINTER;
    if (colors < 2 || colors > 256) return LIQ_VALUE_OUT_OF_RANGE;

    attr->max_colors = colors;
    return LIQ_OK;
}

LIQ_EXPORT LIQ_NONNULL int liq_get_max_colors(const liq_attr *attr)
{
    if (!CHECK_STRUCT_TYPE(attr, liq_attr)) return -1;

    return attr->max_colors;
}

LIQ_EXPORT LIQ_NONNULL liq_error liq_set_min_posterization(liq_attr *attr, int bits)
{
    if (!CHECK_STRUCT_TYPE(attr, liq_attr)) return LIQ_INVALID_POINTER;
    if (bits < 0 || bits > 4) return LIQ_VALUE_OUT_OF_RANGE;

    attr->min_posterization_output = bits;
    return LIQ_OK;
}

LIQ_EXPORT LIQ_NONNULL int liq_get_min_posterization(const liq_attr *attr)
{
    if (!CHECK_STRUCT_TYPE(attr, liq_attr)) return -1;

    return attr->min_posterization_output;
}

LIQ_EXPORT LIQ_NONNULL liq_error liq_set_speed(liq_attr* attr, int speed)
{
    if (!CHECK_STRUCT_TYPE(attr, liq_attr)) return LIQ_INVALID_POINTER;
    if (speed < 1 || speed > 10) return LIQ_VALUE_OUT_OF_RANGE;

    unsigned int iterations = MAX(8-speed, 0);
    iterations += iterations * iterations/2;
    attr->kmeans_iterations = iterations;
    attr->kmeans_iteration_limit = 1.0/(double)(1<<(23-speed));
    attr->feedback_loop_trials = MAX(56-9*speed, 0);

    attr->max_histogram_entries = (1<<17) + (1<<18)*(10-speed);
    attr->min_posterization_input = (speed >= 8) ? 1 : 0;
    attr->use_dither_map = (speed <= (omp_get_max_threads() > 1 ? 7 : 5)); // parallelized dither map might speed up floyd remapping
    if (attr->use_dither_map && speed < 3) {
        attr->use_dither_map = 2; // always
    }
    attr->use_contrast_maps = (speed <= 7) || attr->use_dither_map;
    attr->speed = speed;

    attr->progress_stage1 = attr->use_contrast_maps ? 20 : 8;
    if (attr->feedback_loop_trials < 2) {
        attr->progress_stage1 += 30;
    }
    attr->progress_stage3 = 50 / (1+speed);
    attr->progress_stage2 = 100 - attr->progress_stage1 - attr->progress_stage3;
    return LIQ_OK;
}

LIQ_EXPORT LIQ_NONNULL int liq_get_speed(const liq_attr *attr)
{
    if (!CHECK_STRUCT_TYPE(attr, liq_attr)) return -1;

    return attr->speed;
}

LIQ_EXPORT LIQ_NONNULL liq_error liq_set_output_gamma(liq_result* res, double gamma)
{
    if (!CHECK_STRUCT_TYPE(res, liq_result)) return LIQ_INVALID_POINTER;
    if (gamma <= 0 || gamma >= 1.0) return LIQ_VALUE_OUT_OF_RANGE;

    if (res->remapping) {
        liq_remapping_result_destroy(res->remapping);
        res->remapping = NULL;
    }

    res->gamma = gamma;
    return LIQ_OK;
}

LIQ_EXPORT LIQ_NONNULL liq_error liq_set_min_opacity(liq_attr* attr, int min)
{
    return LIQ_OK;
}

LIQ_EXPORT LIQ_NONNULL int liq_get_min_opacity(const liq_attr *attr)
{
    return 0;
}

LIQ_EXPORT LIQ_NONNULL void liq_set_last_index_transparent(liq_attr* attr, int is_last)
{
    if (!CHECK_STRUCT_TYPE(attr, liq_attr)) return;

    attr->last_index_transparent = !!is_last;
}

LIQ_EXPORT void liq_attr_set_progress_callback(liq_attr *attr, liq_progress_callback_function *callback, void *user_info)
{
    if (!CHECK_STRUCT_TYPE(attr, liq_attr)) return;

    attr->progress_callback = callback;
    attr->progress_callback_user_info = user_info;
}

LIQ_EXPORT void liq_result_set_progress_callback(liq_result *result, liq_progress_callback_function *callback, void *user_info)
{
    if (!CHECK_STRUCT_TYPE(result, liq_result)) return;

    result->progress_callback = callback;
    result->progress_callback_user_info = user_info;
}

LIQ_EXPORT void liq_set_log_callback(liq_attr *attr, liq_log_callback_function *callback, void* user_info)
{
    if (!CHECK_STRUCT_TYPE(attr, liq_attr)) return;

    liq_verbose_printf_flush(attr);
    attr->log_callback = callback;
    attr->log_callback_user_info = user_info;
}

LIQ_EXPORT void liq_set_log_flush_callback(liq_attr *attr, liq_log_flush_callback_function *callback, void* user_info)
{
    if (!CHECK_STRUCT_TYPE(attr, liq_attr)) return;

    attr->log_flush_callback = callback;
    attr->log_flush_callback_user_info = user_info;
}

LIQ_EXPORT liq_attr* liq_attr_create()
{
    return liq_attr_create_with_allocator(NULL, NULL);
}

LIQ_EXPORT LIQ_NONNULL void liq_attr_destroy(liq_attr *attr)
{
    if (!CHECK_STRUCT_TYPE(attr, liq_attr)) {
        return;
    }

    liq_verbose_printf_flush(attr);

    attr->magic_header = liq_freed_magic;
    attr->free(attr);
}

LIQ_EXPORT LIQ_NONNULL liq_attr* liq_attr_copy(const liq_attr *orig)
{
    if (!CHECK_STRUCT_TYPE(orig, liq_attr)) {
        return NULL;
    }

    liq_attr *attr = orig->malloc(sizeof(liq_attr));
    if (!attr) return NULL;
    *attr = *orig;
    return attr;
}

static void *liq_aligned_malloc(size_t size)
{
    unsigned char *ptr = malloc(size + 16);
    if (!ptr) {
        return NULL;
    }

    uintptr_t offset = 16 - ((uintptr_t)ptr & 15); // also reserves 1 byte for ptr[-1]
    ptr += offset;
    assert(0 == (((uintptr_t)ptr) & 15));
    ptr[-1] = offset ^ 0x59; // store how much pointer was shifted to get the original for free()
    return ptr;
}

LIQ_NONNULL static void liq_aligned_free(void *inptr)
{
    unsigned char *ptr = inptr;
    size_t offset = ptr[-1] ^ 0x59;
    assert(offset > 0 && offset <= 16);
    free(ptr - offset);
}

LIQ_EXPORT liq_attr* liq_attr_create_with_allocator(void* (*custom_malloc)(size_t), void (*custom_free)(void*))
{
#if USE_SSE
    if (!is_sse_available()) {
        return NULL;
    }
#endif
    if (!custom_malloc && !custom_free) {
        custom_malloc = liq_aligned_malloc;
        custom_free = liq_aligned_free;
    } else if (!custom_malloc != !custom_free) {
        return NULL; // either specify both or none
    }

    liq_attr *attr = custom_malloc(sizeof(liq_attr));
    if (!attr) return NULL;
    *attr = (liq_attr) {
        .magic_header = liq_attr_magic,
        .malloc = custom_malloc,
        .free = custom_free,
        .max_colors = 256,
        .last_index_transparent = false, // puts transparent color at last index. This is workaround for blu-ray subtitles.
        .target_mse = 0,
        .max_mse = MAX_DIFF,
    };
    liq_set_speed(attr, 4);
    return attr;
}

LIQ_EXPORT LIQ_NONNULL liq_error liq_image_add_fixed_color(liq_image *img, liq_color color)
{
    if (!CHECK_STRUCT_TYPE(img, liq_image)) return LIQ_INVALID_POINTER;
    if (img->fixed_colors_count > 255) return LIQ_UNSUPPORTED;

    float gamma_lut[256];
    to_f_set_gamma(gamma_lut, img->gamma);
    img->fixed_colors[img->fixed_colors_count++] = rgba_to_f(gamma_lut, (rgba_pixel){
        .r = color.r,
        .g = color.g,
        .b = color.b,
        .a = color.a,
    });
    return LIQ_OK;
}

LIQ_NONNULL static liq_error liq_histogram_add_fixed_color_f(liq_histogram *hist, f_pixel color)
{
    if (hist->fixed_colors_count > 255) return LIQ_UNSUPPORTED;

    hist->fixed_colors[hist->fixed_colors_count++] = color;
    return LIQ_OK;
}

LIQ_EXPORT LIQ_NONNULL liq_error liq_histogram_add_fixed_color(liq_histogram *hist, liq_color color, double gamma)
{
    if (!CHECK_STRUCT_TYPE(hist, liq_histogram)) return LIQ_INVALID_POINTER;

    float gamma_lut[256];
    to_f_set_gamma(gamma_lut, gamma ? gamma : 0.45455);
    const f_pixel px = rgba_to_f(gamma_lut, (rgba_pixel){
        .r = color.r,
        .g = color.g,
        .b = color.b,
        .a = color.a,
    });
    return liq_histogram_add_fixed_color_f(hist, px);
}

LIQ_NONNULL static bool liq_image_use_low_memory(liq_image *img)
{
    img->temp_f_row = img->malloc(sizeof(img->f_pixels[0]) * LIQ_TEMP_ROW_WIDTH(img->width) * omp_get_max_threads());
    return img->temp_f_row != NULL;
}

LIQ_NONNULL static bool liq_image_should_use_low_memory(liq_image *img, const bool low_memory_hint)
{
    return (size_t)img->width * (size_t)img->height > (low_memory_hint ? LIQ_HIGH_MEMORY_LIMIT/8 : LIQ_HIGH_MEMORY_LIMIT) / sizeof(f_pixel); // Watch out for integer overflow
}

static liq_image *liq_image_create_internal(const liq_attr *attr, rgba_pixel* rows[], liq_image_get_rgba_row_callback *row_callback, void *row_callback_user_info, int width, int height, double gamma)
{
    if (gamma < 0 || gamma > 1.0) {
        liq_log_error(attr, "gamma must be >= 0 and <= 1 (try 1/gamma instead)");
        return NULL;
    }

    if (!rows && !row_callback) {
        liq_log_error(attr, "missing row data");
        return NULL;
    }

    liq_image *img = attr->malloc(sizeof(liq_image));
    if (!img) return NULL;
    *img = (liq_image){
        .magic_header = liq_image_magic,
        .malloc = attr->malloc,
        .free = attr->free,
        .width = width, .height = height,
        .gamma = gamma ? gamma : 0.45455,
        .rows = rows,
        .row_callback = row_callback,
        .row_callback_user_info = row_callback_user_info,
    };

    if (!rows) {
        img->temp_row = attr->malloc(sizeof(img->temp_row[0]) * LIQ_TEMP_ROW_WIDTH(width) * omp_get_max_threads());
        if (!img->temp_row) return NULL;
    }

    // if image is huge or converted pixels are not likely to be reused then don't cache converted pixels
    if (liq_image_should_use_low_memory(img, !img->temp_row && !attr->use_contrast_maps && !attr->use_dither_map)) {
        verbose_print(attr, "  conserving memory");
        if (!liq_image_use_low_memory(img)) return NULL;
    }

    return img;
}

LIQ_EXPORT LIQ_NONNULL liq_error liq_image_set_memory_ownership(liq_image *img, int ownership_flags)
{
    if (!CHECK_STRUCT_TYPE(img, liq_image)) return LIQ_INVALID_POINTER;
    if (!img->rows || !ownership_flags || (ownership_flags & ~(LIQ_OWN_ROWS|LIQ_OWN_PIXELS))) {
        return LIQ_VALUE_OUT_OF_RANGE;
    }

    if (ownership_flags & LIQ_OWN_ROWS) {
        if (img->free_rows_internal) return LIQ_VALUE_OUT_OF_RANGE;
        img->free_rows = true;
    }

    if (ownership_flags & LIQ_OWN_PIXELS) {
        img->free_pixels = true;
        if (!img->pixels) {
            // for simplicity of this API there's no explicit bitmap argument,
            // so the row with the lowest address is assumed to be at the start of the bitmap
            img->pixels = img->rows[0];
            for(unsigned int i=1; i < img->height; i++) {
                img->pixels = MIN(img->pixels, img->rows[i]);
            }
        }
    }

    return LIQ_OK;
}

LIQ_NONNULL static void liq_image_free_maps(liq_image *input_image);
LIQ_NONNULL static void liq_image_free_dither_map(liq_image *input_image);
LIQ_NONNULL static void liq_image_free_importance_map(liq_image *input_image);

LIQ_EXPORT LIQ_NONNULL liq_error liq_image_set_importance_map(liq_image *img, unsigned char importance_map[], size_t buffer_size, enum liq_ownership ownership) {
    if (!CHECK_STRUCT_TYPE(img, liq_image)) return LIQ_INVALID_POINTER;
    if (!CHECK_USER_POINTER(importance_map)) return LIQ_INVALID_POINTER;

    const size_t required_size = (size_t)img->width * (size_t)img->height;
    if (buffer_size < required_size) {
        return LIQ_BUFFER_TOO_SMALL;
    }

    if (ownership == LIQ_COPY_PIXELS) {
        unsigned char *tmp = img->malloc(required_size);
        if (!tmp) {
            return LIQ_OUT_OF_MEMORY;
        }
        memcpy(tmp, importance_map, required_size);
        importance_map = tmp;
    } else if (ownership != LIQ_OWN_PIXELS) {
        return LIQ_UNSUPPORTED;
    }

    liq_image_free_importance_map(img);
    img->importance_map = importance_map;

    return LIQ_OK;
}

LIQ_EXPORT LIQ_NONNULL liq_error liq_image_set_background(liq_image *img, liq_image *background)
{
    if (!CHECK_STRUCT_TYPE(img, liq_image)) return LIQ_INVALID_POINTER;
    if (!CHECK_STRUCT_TYPE(background, liq_image)) return LIQ_INVALID_POINTER;

    if (background->background) {
        return LIQ_UNSUPPORTED;
    }
    if (img->width != background->width || img->height != background->height) {
        return LIQ_BUFFER_TOO_SMALL;
    }

    if (img->background) {
        liq_image_destroy(img->background);
    }

    img->background = background;
    liq_image_free_dither_map(img); // Force it to be re-analyzed with the background

    return LIQ_OK;
}

LIQ_NONNULL static bool check_image_size(const liq_attr *attr, const int width, const int height)
{
    if (!CHECK_STRUCT_TYPE(attr, liq_attr)) {
        return false;
    }

    if (width <= 0 || height <= 0) {
        liq_log_error(attr, "width and height must be > 0");
        return false;
    }

    if (width > INT_MAX/sizeof(rgba_pixel)/height || width > INT_MAX/16/sizeof(f_pixel) || height > INT_MAX/sizeof(size_t)) {
        liq_log_error(attr, "image too large");
        return false;
    }
    return true;
}

LIQ_EXPORT liq_image *liq_image_create_custom(const liq_attr *attr, liq_image_get_rgba_row_callback *row_callback, void* user_info, int width, int height, double gamma)
{
    if (!check_image_size(attr, width, height)) {
        return NULL;
    }
    return liq_image_create_internal(attr, NULL, row_callback, user_info, width, height, gamma);
}

LIQ_EXPORT liq_image *liq_image_create_rgba_rows(const liq_attr *attr, void *const rows[], int width, int height, double gamma)
{
    if (!check_image_size(attr, width, height)) {
        return NULL;
    }
    for(int i=0; i < height; i++) {
        if (!CHECK_USER_POINTER(rows+i) || !CHECK_USER_POINTER(rows[i])) {
            liq_log_error(attr, "invalid row pointers");
            return NULL;
        }
    }
    return liq_image_create_internal(attr, (rgba_pixel**)rows, NULL, NULL, width, height, gamma);
}

LIQ_EXPORT LIQ_NONNULL liq_image *liq_image_create_rgba(const liq_attr *attr, const void* bitmap, int width, int height, double gamma)
{
    if (!check_image_size(attr, width, height)) {
        return NULL;
    }
    if (!CHECK_USER_POINTER(bitmap)) {
        liq_log_error(attr, "invalid bitmap pointer");
        return NULL;
    }

    rgba_pixel *const pixels = (rgba_pixel *const)bitmap;
    rgba_pixel **rows = attr->malloc(sizeof(rows[0])*height);
    if (!rows) return NULL;

    for(int i=0; i < height; i++) {
        rows[i] = pixels + width * i;
    }

    liq_image *image = liq_image_create_internal(attr, rows, NULL, NULL, width, height, gamma);
    if (!image) {
        attr->free(rows);
        return NULL;
    }
    image->free_rows = true;
    image->free_rows_internal = true;
    return image;
}


NEVER_INLINE LIQ_EXPORT void liq_executing_user_callback(liq_image_get_rgba_row_callback *callback, liq_color *temp_row, int row, int width, void *user_info);
LIQ_EXPORT void liq_executing_user_callback(liq_image_get_rgba_row_callback *callback, liq_color *temp_row, int row, int width, void *user_info)
{
    assert(callback);
    assert(temp_row);
    callback(temp_row, row, width, user_info);
}

LIQ_NONNULL inline static bool liq_image_has_rgba_pixels(const liq_image *img)
{
    if (!CHECK_STRUCT_TYPE(img, liq_image)) {
        return false;
    }
    return img->rows || (img->temp_row && img->row_callback);
}

LIQ_NONNULL inline static bool liq_image_can_use_rgba_rows(const liq_image *img)
{
    assert(liq_image_has_rgba_pixels(img));
    return img->rows;
}

LIQ_NONNULL static const rgba_pixel *liq_image_get_row_rgba(liq_image *img, unsigned int row)
{
    if (liq_image_can_use_rgba_rows(img)) {
        return img->rows[row];
    }

    assert(img->temp_row);
    rgba_pixel *temp_row = img->temp_row + LIQ_TEMP_ROW_WIDTH(img->width) * omp_get_thread_num();
    if (img->rows) {
        memcpy(temp_row, img->rows[row], img->width * sizeof(temp_row[0]));
    } else {
        liq_executing_user_callback(img->row_callback, (liq_color*)temp_row, row, img->width, img->row_callback_user_info);
    }

    return temp_row;
}

LIQ_NONNULL static void convert_row_to_f(liq_image *img, f_pixel *row_f_pixels, const unsigned int row, const float gamma_lut[])
{
    assert(row_f_pixels);
    assert(!USE_SSE || 0 == ((uintptr_t)row_f_pixels & 15));

    const rgba_pixel *const row_pixels = liq_image_get_row_rgba(img, row);

    for(unsigned int col=0; col < img->width; col++) {
        row_f_pixels[col] = rgba_to_f(gamma_lut, row_pixels[col]);
    }
}

LIQ_NONNULL static bool liq_image_get_row_f_init(liq_image *img)
{
    assert(omp_get_thread_num() == 0);
    if (img->f_pixels) {
        return true;
    }
    if (!liq_image_should_use_low_memory(img, false)) {
        img->f_pixels = img->malloc(sizeof(img->f_pixels[0]) * img->width * img->height);
    }
    if (!img->f_pixels) {
        return liq_image_use_low_memory(img);
    }

    if (!liq_image_has_rgba_pixels(img)) {
        return false;
    }

    float gamma_lut[256];
    to_f_set_gamma(gamma_lut, img->gamma);
    for(unsigned int i=0; i < img->height; i++) {
        convert_row_to_f(img, &img->f_pixels[i*img->width], i, gamma_lut);
    }
    return true;
}

LIQ_NONNULL static const f_pixel *liq_image_get_row_f(liq_image *img, unsigned int row)
{
    if (!img->f_pixels) {
        assert(img->temp_f_row); // init should have done that
        float gamma_lut[256];
        to_f_set_gamma(gamma_lut, img->gamma);
        f_pixel *row_for_thread = img->temp_f_row + LIQ_TEMP_ROW_WIDTH(img->width) * omp_get_thread_num();
        convert_row_to_f(img, row_for_thread, row, gamma_lut);
        return row_for_thread;
    }
    return img->f_pixels + img->width * row;
}

LIQ_EXPORT LIQ_NONNULL int liq_image_get_width(const liq_image *input_image)
{
    if (!CHECK_STRUCT_TYPE(input_image, liq_image)) return -1;
    return input_image->width;
}

LIQ_EXPORT LIQ_NONNULL int liq_image_get_height(const liq_image *input_image)
{
    if (!CHECK_STRUCT_TYPE(input_image, liq_image)) return -1;
    return input_image->height;
}



LIQ_NONNULL static free_func *get_default_image_free_func(liq_image *img)
{
    // When default allocator is used then user-supplied pointers must be freed with free()
    if (img->free != liq_aligned_free) {
        return img->free;
    }
    return free;
}

LIQ_NONNULL static free_func *get_default_rows_free_func(liq_image *img)
{
    // When default allocator is used then user-supplied pointers must be freed with free()
    if (img->free_rows_internal || img->free != liq_aligned_free) {
        return img->free;
    }
    return free;
}

LIQ_NONNULL static void liq_image_free_rgba_source(liq_image *input_image)
{
    if (input_image->free_pixels && input_image->pixels) {
        get_default_image_free_func(input_image)(input_image->pixels);
        input_image->pixels = NULL;
    }

    if (input_image->free_rows && input_image->rows) {
        get_default_rows_free_func(input_image)(input_image->rows);
        input_image->rows = NULL;
    }
}

LIQ_NONNULL static void liq_image_free_importance_map(liq_image *input_image) {
    if (input_image->importance_map) {
        input_image->free(input_image->importance_map);
        input_image->importance_map = NULL;
    }
}

LIQ_NONNULL static void liq_image_free_maps(liq_image *input_image) {
    liq_image_free_importance_map(input_image);

    if (input_image->edges) {
        input_image->free(input_image->edges);
        input_image->edges = NULL;
    }
    liq_image_free_dither_map(input_image);
}

LIQ_NONNULL static void liq_image_free_dither_map(liq_image *input_image) {
    if (input_image->dither_map) {
        input_image->free(input_image->dither_map);
        input_image->dither_map = NULL;
    }
}

LIQ_EXPORT LIQ_NONNULL void liq_image_destroy(liq_image *input_image)
{
    if (!CHECK_STRUCT_TYPE(input_image, liq_image)) return;

    liq_image_free_rgba_source(input_image);

    liq_image_free_maps(input_image);

    if (input_image->f_pixels) {
        input_image->free(input_image->f_pixels);
    }

    if (input_image->temp_row) {
        input_image->free(input_image->temp_row);
    }

    if (input_image->temp_f_row) {
        input_image->free(input_image->temp_f_row);
    }

    if (input_image->background) {
        liq_image_destroy(input_image->background);
    }

    input_image->magic_header = liq_freed_magic;
    input_image->free(input_image);
}

LIQ_EXPORT liq_histogram* liq_histogram_create(const liq_attr* attr)
{
    if (!CHECK_STRUCT_TYPE(attr, liq_attr)) {
        return NULL;
    }

    liq_histogram *hist = attr->malloc(sizeof(liq_histogram));
    if (!hist) return NULL;
    *hist = (liq_histogram) {
        .magic_header = liq_histogram_magic,
        .malloc = attr->malloc,
        .free = attr->free,

        .ignorebits = MAX(attr->min_posterization_output, attr->min_posterization_input),
    };
    return hist;
}

LIQ_EXPORT LIQ_NONNULL void liq_histogram_destroy(liq_histogram *hist)
{
    if (!CHECK_STRUCT_TYPE(hist, liq_histogram)) return;
    hist->magic_header = liq_freed_magic;

    pam_freeacolorhash(hist->acht);
    hist->free(hist);
}


LIQ_EXPORT LIQ_NONNULL liq_error liq_image_quantize(liq_image *const img, liq_attr *const attr, liq_result **result_output)
{
    if (!CHECK_STRUCT_TYPE(attr, liq_attr)) return LIQ_INVALID_POINTER;
    if (!liq_image_has_rgba_pixels(img)) {
        return LIQ_UNSUPPORTED;
    }

    liq_histogram *hist = liq_histogram_create(attr);
    if (!hist) {
        return LIQ_OUT_OF_MEMORY;
    }
    liq_error err = liq_histogram_add_image(hist, attr, img);
    if (LIQ_OK != err) {
        liq_histogram_destroy(hist);
        return err;
    }

    err = liq_histogram_quantize_internal(hist, attr, false, result_output);
    liq_histogram_destroy(hist);

    return err;
}

LIQ_EXPORT LIQ_NONNULL liq_error liq_histogram_quantize(liq_histogram *input_hist, liq_attr *attr, liq_result **result_output) {
    return liq_histogram_quantize_internal(input_hist, attr, true, result_output);
}

LIQ_NONNULL static liq_error liq_histogram_quantize_internal(liq_histogram *input_hist, liq_attr *attr, bool fixed_result_colors, liq_result **result_output)
{
    if (!CHECK_USER_POINTER(result_output)) return LIQ_INVALID_POINTER;
    *result_output = NULL;

    if (!CHECK_STRUCT_TYPE(attr, liq_attr)) return LIQ_INVALID_POINTER;
    if (!CHECK_STRUCT_TYPE(input_hist, liq_histogram)) return LIQ_INVALID_POINTER;

    if (liq_progress(attr, 0)) return LIQ_ABORTED;

    histogram *hist;
    liq_error err = finalize_histogram(input_hist, attr, &hist);
    if (err != LIQ_OK) {
        return err;
    }

    err = pngquant_quantize(hist, attr, input_hist->fixed_colors_count, input_hist->fixed_colors, input_hist->gamma, fixed_result_colors, result_output);
    pam_freeacolorhist(hist);

    return err;
}

LIQ_EXPORT LIQ_NONNULL liq_error liq_set_dithering_level(liq_result *res, float dither_level)
{
    if (!CHECK_STRUCT_TYPE(res, liq_result)) return LIQ_INVALID_POINTER;

    if (res->remapping) {
        liq_remapping_result_destroy(res->remapping);
        res->remapping = NULL;
    }

    if (dither_level < 0 || dither_level > 1.0f) return LIQ_VALUE_OUT_OF_RANGE;
    res->dither_level = dither_level;
    return LIQ_OK;
}

LIQ_NONNULL static liq_remapping_result *liq_remapping_result_create(liq_result *result)
{
    if (!CHECK_STRUCT_TYPE(result, liq_result)) {
        return NULL;
    }

    liq_remapping_result *res = result->malloc(sizeof(liq_remapping_result));
    if (!res) return NULL;
    *res = (liq_remapping_result) {
        .magic_header = liq_remapping_result_magic,
        .malloc = result->malloc,
        .free = result->free,
        .dither_level = result->dither_level,
        .use_dither_map = result->use_dither_map,
        .palette_error = result->palette_error,
        .gamma = result->gamma,
        .palette = pam_duplicate_colormap(result->palette),
        .progress_callback = result->progress_callback,
        .progress_callback_user_info = result->progress_callback_user_info,
        .progress_stage1 = result->use_dither_map ? 20 : 0,
    };
    return res;
}

LIQ_EXPORT LIQ_NONNULL double liq_get_output_gamma(const liq_result *result)
{
    if (!CHECK_STRUCT_TYPE(result, liq_result)) return -1;

    return result->gamma;
}

LIQ_NONNULL static void liq_remapping_result_destroy(liq_remapping_result *result)
{
    if (!CHECK_STRUCT_TYPE(result, liq_remapping_result)) return;

    if (result->palette) pam_freecolormap(result->palette);
    if (result->pixels) result->free(result->pixels);

    result->magic_header = liq_freed_magic;
    result->free(result);
}

LIQ_EXPORT LIQ_NONNULL void liq_result_destroy(liq_result *res)
{
    if (!CHECK_STRUCT_TYPE(res, liq_result)) return;

    memset(&res->int_palette, 0, sizeof(liq_palette));

    if (res->remapping) {
        memset(&res->remapping->int_palette, 0, sizeof(liq_palette));
        liq_remapping_result_destroy(res->remapping);
    }

    pam_freecolormap(res->palette);

    res->magic_header = liq_freed_magic;
    res->free(res);
}


LIQ_EXPORT LIQ_NONNULL double liq_get_quantization_error(const liq_result *result) {
    if (!CHECK_STRUCT_TYPE(result, liq_result)) return -1;

    if (result->palette_error >= 0) {
        return mse_to_standard_mse(result->palette_error);
    }

    return -1;
}

LIQ_EXPORT LIQ_NONNULL double liq_get_remapping_error(const liq_result *result) {
    if (!CHECK_STRUCT_TYPE(result, liq_result)) return -1;

    if (result->remapping && result->remapping->palette_error >= 0) {
        return mse_to_standard_mse(result->remapping->palette_error);
    }

    return -1;
}

LIQ_EXPORT LIQ_NONNULL int liq_get_quantization_quality(const liq_result *result) {
    if (!CHECK_STRUCT_TYPE(result, liq_result)) return -1;

    if (result->palette_error >= 0) {
        return mse_to_quality(result->palette_error);
    }

    return -1;
}

LIQ_EXPORT LIQ_NONNULL int liq_get_remapping_quality(const liq_result *result) {
    if (!CHECK_STRUCT_TYPE(result, liq_result)) return -1;

    if (result->remapping && result->remapping->palette_error >= 0) {
        return mse_to_quality(result->remapping->palette_error);
    }

    return -1;
}

LIQ_NONNULL static int compare_popularity(const void *ch1, const void *ch2)
{
    const float v1 = ((const colormap_item*)ch1)->popularity;
    const float v2 = ((const colormap_item*)ch2)->popularity;
    return v1 > v2 ? -1 : 1;
}

LIQ_NONNULL static void sort_palette_qsort(colormap *map, int start, int nelem)
{
    if (!nelem) return;
    qsort(map->palette + start, nelem, sizeof(map->palette[0]), compare_popularity);
}

#define SWAP_PALETTE(map, a,b) { \
    const colormap_item tmp = (map)->palette[(a)]; \
    (map)->palette[(a)] = (map)->palette[(b)]; \
    (map)->palette[(b)] = tmp; }

LIQ_NONNULL static void sort_palette(colormap *map, const liq_attr *options)
{
    /*
    ** Step 3.5 [GRR]: remap the palette colors so that all entries with
    ** the maximal alpha value (i.e., fully opaque) are at the end and can
    ** therefore be omitted from the tRNS chunk.
    */
    if (options->last_index_transparent) {
        for(unsigned int i=0; i < map->colors; i++) {
            if (map->palette[i].acolor.a < 1.f/256.f) {
                const unsigned int old = i, transparent_dest = map->colors-1;

                SWAP_PALETTE(map, transparent_dest, old);

                /* colors sorted by popularity make pngs slightly more compressible */
                sort_palette_qsort(map, 0, map->colors-1);
                return;
            }
        }
    }

    unsigned int non_fixed_colors = 0;
    for(unsigned int i = 0; i < map->colors; i++) {
        if (map->palette[i].fixed) {
            break;
        }
        non_fixed_colors++;
    }

    /* move transparent colors to the beginning to shrink trns chunk */
    unsigned int num_transparent = 0;
    for(unsigned int i = 0; i < non_fixed_colors; i++) {
        if (map->palette[i].acolor.a < 255.f/256.f) {
            // current transparent color is swapped with earlier opaque one
            if (i != num_transparent) {
                SWAP_PALETTE(map, num_transparent, i);
                i--;
            }
            num_transparent++;
        }
    }

    liq_verbose_printf(options, "  eliminated opaque tRNS-chunk entries...%d entr%s transparent", num_transparent, (num_transparent == 1)? "y" : "ies");

    /* colors sorted by popularity make pngs slightly more compressible
     * opaque and transparent are sorted separately
     */
    sort_palette_qsort(map, 0, num_transparent);
    sort_palette_qsort(map, num_transparent, non_fixed_colors - num_transparent);

    if (non_fixed_colors > 9 && map->colors > 16) {
        SWAP_PALETTE(map, 7, 1); // slightly improves compression
        SWAP_PALETTE(map, 8, 2);
        SWAP_PALETTE(map, 9, 3);
    }
}

inline static unsigned int posterize_channel(unsigned int color, unsigned int bits)
{
    return (color & ~((1<<bits)-1)) | (color >> (8-bits));
}

LIQ_NONNULL static void set_rounded_palette(liq_palette *const dest, colormap *const map, const double gamma, unsigned int posterize)
{
    float gamma_lut[256];
    to_f_set_gamma(gamma_lut, gamma);

    dest->count = map->colors;
    for(unsigned int x = 0; x < map->colors; ++x) {
        rgba_pixel px = f_to_rgb(gamma, map->palette[x].acolor);

        px.r = posterize_channel(px.r, posterize);
        px.g = posterize_channel(px.g, posterize);
        px.b = posterize_channel(px.b, posterize);
        px.a = posterize_channel(px.a, posterize);

        map->palette[x].acolor = rgba_to_f(gamma_lut, px); /* saves rounding error introduced by to_rgb, which makes remapping & dithering more accurate */

        if (!px.a && !map->palette[x].fixed) {
            px.r = 71; px.g = 112; px.b = 76;
        }

        dest->entries[x] = (liq_color){.r=px.r,.g=px.g,.b=px.b,.a=px.a};
    }
}

LIQ_EXPORT LIQ_NONNULL const liq_palette *liq_get_palette(liq_result *result)
{
    if (!CHECK_STRUCT_TYPE(result, liq_result)) return NULL;

    if (result->remapping && result->remapping->int_palette.count) {
        return &result->remapping->int_palette;
    }

    if (!result->int_palette.count) {
        set_rounded_palette(&result->int_palette, result->palette, result->gamma, result->min_posterization_output);
    }
    return &result->int_palette;
}

LIQ_NONNULL static float remap_to_palette(liq_image *const input_image, unsigned char *const *const output_pixels, colormap *const map)
{
    const int rows = input_image->height;
    const unsigned int cols = input_image->width;
    double remapping_error=0;

    if (!liq_image_get_row_f_init(input_image)) {
        return -1;
    }
    if (input_image->background && !liq_image_get_row_f_init(input_image->background)) {
        return -1;
    }

    const colormap_item *acolormap = map->palette;

    struct nearest_map *const n = nearest_init(map);
    liq_image *background = input_image->background;
    const int transparent_index = background ? nearest_search(n, &(f_pixel){0,0,0,0}, 0, NULL) : -1;
    if (background && acolormap[transparent_index].acolor.a > 1.f/256.f) {
        // palette unsuitable for using the bg
        background = NULL;
    }


    const unsigned int max_threads = omp_get_max_threads();
    LIQ_ARRAY(kmeans_state, average_color, (KMEANS_CACHE_LINE_GAP+map->colors) * max_threads);
    kmeans_init(map, max_threads, average_color);


    for(int row = 0; row < rows; ++row) {
        const f_pixel *const row_pixels = liq_image_get_row_f(input_image, row);
        const f_pixel *const bg_pixels = background && acolormap[transparent_index].acolor.a < 1.f/256.f ? liq_image_get_row_f(background, row) : NULL;

        unsigned int last_match=0;
        for(unsigned int col = 0; col < cols; ++col) {
            float diff;
            last_match = nearest_search(n, &row_pixels[col], last_match, &diff);
            if (bg_pixels) {
                float bg_diff = colordifference(bg_pixels[col], acolormap[last_match].acolor);
                if (bg_diff <= diff) {
                    diff = bg_diff;
                    last_match = transparent_index;
                }
            }
            output_pixels[row][col] = last_match;

            remapping_error += diff;
            if (last_match != transparent_index) {
                kmeans_update_color(row_pixels[col], 1.0, map, last_match, omp_get_thread_num(), average_color);
            }
        }
    }

    kmeans_finalize(map, max_threads, average_color);

    nearest_free(n);

    return remapping_error / (input_image->width * input_image->height);
}

inline static f_pixel get_dithered_pixel(const float dither_level, const float max_dither_error, const f_pixel thiserr, const f_pixel px)
{
    /* Use Floyd-Steinberg errors to adjust actual color. */
    const float sr = thiserr.r * dither_level,
                sg = thiserr.g * dither_level,
                sb = thiserr.b * dither_level,
                sa = thiserr.a * dither_level;

    float ratio = 1.0;
    const float max_overflow = 1.1f;
    const float max_underflow = -0.1f;

    // allowing some overflow prevents undithered bands caused by clamping of all channels
           if (px.r + sr > max_overflow)  ratio = MIN(ratio, (max_overflow -px.r)/sr);
    else { if (px.r + sr < max_underflow) ratio = MIN(ratio, (max_underflow-px.r)/sr); }
           if (px.g + sg > max_overflow)  ratio = MIN(ratio, (max_overflow -px.g)/sg);
    else { if (px.g + sg < max_underflow) ratio = MIN(ratio, (max_underflow-px.g)/sg); }
           if (px.b + sb > max_overflow)  ratio = MIN(ratio, (max_overflow -px.b)/sb);
    else { if (px.b + sb < max_underflow) ratio = MIN(ratio, (max_underflow-px.b)/sb); }

    float a = px.a + sa;
         if (a > 1.f) { a = 1.f; }
    else if (a < 0)   { a = 0; }

     // If dithering error is crazy high, don't propagate it that much
     // This prevents crazy geen pixels popping out of the blue (or red or black! ;)
     const float dither_error = sr*sr + sg*sg + sb*sb + sa*sa;
     if (dither_error > max_dither_error) {
         ratio *= 0.8f;
     } else if (dither_error < 2.f/256.f/256.f) {
        // don't dither areas that don't have noticeable error — makes file smaller
        return px;
    }

    return (f_pixel) {
        .r=px.r + sr * ratio,
        .g=px.g + sg * ratio,
        .b=px.b + sb * ratio,
        .a=a,
    };
}

/**
  Uses edge/noise map to apply dithering only to flat areas. Dithering on edges creates jagged lines, and noisy areas are "naturally" dithered.

  If output_image_is_remapped is true, only pixels noticeably changed by error diffusion will be written to output image.
 */
LIQ_NONNULL static bool remap_to_palette_floyd(liq_image *input_image, unsigned char *const output_pixels[], liq_remapping_result *quant, const float max_dither_error, const bool output_image_is_remapped)
{
    const int rows = input_image->height, cols = input_image->width;
    const unsigned char *dither_map = quant->use_dither_map ? (input_image->dither_map ? input_image->dither_map : input_image->edges) : NULL;

    const colormap *map = quant->palette;
    const colormap_item *acolormap = map->palette;

    if (!liq_image_get_row_f_init(input_image)) {
        return false;
    }
    if (input_image->background && !liq_image_get_row_f_init(input_image->background)) {
        return false;
    }

    /* Initialize Floyd-Steinberg error vectors. */
    const size_t errwidth = cols+2;
    f_pixel *restrict thiserr = input_image->malloc(errwidth * sizeof(thiserr[0]) * 2); // +2 saves from checking out of bounds access
    if (!thiserr) return false;
    f_pixel *restrict nexterr = thiserr + errwidth;
    memset(thiserr, 0, errwidth * sizeof(thiserr[0]));

    bool ok = true;
    struct nearest_map *const n = nearest_init(map);
    liq_image *background = input_image->background;
    const int transparent_index = background ? nearest_search(n, &(f_pixel){0,0,0,0}, 0, NULL) : -1;
    if (background && acolormap[transparent_index].acolor.a > 1.f/256.f) {
        // palette unsuitable for using the bg
        background = NULL;
    }

    // response to this value is non-linear and without it any value < 0.8 would give almost no dithering
    float base_dithering_level = quant->dither_level;
    base_dithering_level = 1.f - (1.f-base_dithering_level)*(1.f-base_dithering_level);

    if (dither_map) {
        base_dithering_level *= 1.f/255.f; // convert byte to float
    }
    base_dithering_level *= 15.f/16.f; // prevent small errors from accumulating

    int fs_direction = 1;
    unsigned int last_match=0;
    for (int row = 0; row < rows; ++row) {
        if (liq_remap_progress(quant, quant->progress_stage1 + row * (100.f - quant->progress_stage1) / rows)) {
            ok = false;
            break;
        }

        memset(nexterr, 0, errwidth * sizeof(nexterr[0]));

        int col = (fs_direction > 0) ? 0 : (cols - 1);
        const f_pixel *const row_pixels = liq_image_get_row_f(input_image, row);
        const f_pixel *const bg_pixels = background && acolormap[transparent_index].acolor.a < 1.f/256.f ? liq_image_get_row_f(background, row) : NULL;
        int undithered_bg_used = 0;

        do {
            float dither_level = base_dithering_level;
            if (dither_map) {
                dither_level *= dither_map[row*cols + col];
            }

            const f_pixel spx = get_dithered_pixel(dither_level, max_dither_error, thiserr[col + 1], row_pixels[col]);

            const unsigned int guessed_match = output_image_is_remapped ? output_pixels[row][col] : last_match;
            float dither_diff;
            last_match = nearest_search(n, &spx, guessed_match, &dither_diff);
            f_pixel output_px = acolormap[last_match].acolor;
            // this is for animgifs
            if (bg_pixels) {
                // if the background makes better match *with* dithering, it's a definitive win
                float bg_for_dither_diff = colordifference(spx, bg_pixels[col]);
                if (bg_for_dither_diff <= dither_diff) {
                    output_px = bg_pixels[col];
                    last_match = transparent_index;
                } else if (undithered_bg_used > 1) {
                    // the undithered fallback can cause artifacts when too many undithered pixels accumulate a big dithering error
                    // so periodically ignore undithered fallback to prevent that
                    undithered_bg_used = 0;
                } else {
                    // if dithering is not applied, there's a high risk of creating artifacts (flat areas, error accumulating badly),
                    // OTOH poor dithering disturbs static backgrounds and creates oscilalting frames that break backgrounds
                    // back and forth in two differently bad ways
                    float max_diff = colordifference(row_pixels[col], bg_pixels[col]);
                    float dithered_diff = colordifference(row_pixels[col], output_px);
                    // if dithering is worse than natural difference between frames
                    // (this rule dithers moving areas, but does not dither static areas)
                    if (dithered_diff > max_diff) {
                        // then see if an undithered color is closer to the ideal
                        float undithered_diff = colordifference(row_pixels[col], acolormap[guessed_match].acolor);
                        if (undithered_diff < max_diff) {
                            undithered_bg_used++;
                            output_px = acolormap[guessed_match].acolor;
                            last_match = guessed_match;
                        }
                    }
                }
            }

            output_pixels[row][col] = last_match;

            f_pixel err = {
                .r = (spx.r - output_px.r),
                .g = (spx.g - output_px.g),
                .b = (spx.b - output_px.b),
                .a = (spx.a - output_px.a),
            };

            // If dithering error is crazy high, don't propagate it that much
            // This prevents crazy geen pixels popping out of the blue (or red or black! ;)
            if (err.r*err.r + err.g*err.g + err.b*err.b + err.a*err.a > max_dither_error) {
                err.r *= 0.75f;
                err.g *= 0.75f;
                err.b *= 0.75f;
                err.a *= 0.75f;
            }

            /* Propagate Floyd-Steinberg error terms. */
            if (fs_direction > 0) {
                thiserr[col + 2].a += err.a * (7.f/16.f);
                thiserr[col + 2].r += err.r * (7.f/16.f);
                thiserr[col + 2].g += err.g * (7.f/16.f);
                thiserr[col + 2].b += err.b * (7.f/16.f);

                nexterr[col + 2].a  = err.a * (1.f/16.f);
                nexterr[col + 2].r  = err.r * (1.f/16.f);
                nexterr[col + 2].g  = err.g * (1.f/16.f);
                nexterr[col + 2].b  = err.b * (1.f/16.f);

                nexterr[col + 1].a += err.a * (5.f/16.f);
                nexterr[col + 1].r += err.r * (5.f/16.f);
                nexterr[col + 1].g += err.g * (5.f/16.f);
                nexterr[col + 1].b += err.b * (5.f/16.f);

                nexterr[col    ].a += err.a * (3.f/16.f);
                nexterr[col    ].r += err.r * (3.f/16.f);
                nexterr[col    ].g += err.g * (3.f/16.f);
                nexterr[col    ].b += err.b * (3.f/16.f);

            } else {
                thiserr[col    ].a += err.a * (7.f/16.f);
                thiserr[col    ].r += err.r * (7.f/16.f);
                thiserr[col    ].g += err.g * (7.f/16.f);
                thiserr[col    ].b += err.b * (7.f/16.f);

                nexterr[col    ].a  = err.a * (1.f/16.f);
                nexterr[col    ].r  = err.r * (1.f/16.f);
                nexterr[col    ].g  = err.g * (1.f/16.f);
                nexterr[col    ].b  = err.b * (1.f/16.f);

                nexterr[col + 1].a += err.a * (5.f/16.f);
                nexterr[col + 1].r += err.r * (5.f/16.f);
                nexterr[col + 1].g += err.g * (5.f/16.f);
                nexterr[col + 1].b += err.b * (5.f/16.f);

                nexterr[col + 2].a += err.a * (3.f/16.f);
                nexterr[col + 2].r += err.r * (3.f/16.f);
                nexterr[col + 2].g += err.g * (3.f/16.f);
                nexterr[col + 2].b += err.b * (3.f/16.f);
            }

            // remapping is done in zig-zag
            col += fs_direction;
            if (fs_direction > 0) {
                if (col >= cols) break;
            } else {
                if (col < 0) break;
            }
        } while(1);

        f_pixel *const temperr = thiserr;
        thiserr = nexterr;
        nexterr = temperr;
        fs_direction = -fs_direction;
    }

    input_image->free(MIN(thiserr, nexterr)); // MIN because pointers were swapped
    nearest_free(n);

    return ok;
}

/* fixed colors are always included in the palette, so it would be wasteful to duplicate them in palette from histogram */
LIQ_NONNULL static void remove_fixed_colors_from_histogram(histogram *hist, const int fixed_colors_count, const f_pixel fixed_colors[], const float target_mse)
{
    const float max_difference = MAX(target_mse/2.f, 2.f/256.f/256.f);
    if (fixed_colors_count) {
        for(int j=0; j < hist->size; j++) {
            for(unsigned int i=0; i < fixed_colors_count; i++) {
                if (colordifference(hist->achv[j].acolor, fixed_colors[i]) < max_difference) {
                    hist->achv[j] = hist->achv[--hist->size]; // remove color from histogram by overwriting with the last entry
                    j--; break; // continue searching histogram
                }
            }
        }
    }
}

LIQ_EXPORT LIQ_NONNULL liq_error liq_histogram_add_colors(liq_histogram *input_hist, const liq_attr *options, const liq_histogram_entry entries[], int num_entries, double gamma)
{
    if (!CHECK_STRUCT_TYPE(options, liq_attr)) return LIQ_INVALID_POINTER;
    if (!CHECK_STRUCT_TYPE(input_hist, liq_histogram)) return LIQ_INVALID_POINTER;
    if (!CHECK_USER_POINTER(entries)) return LIQ_INVALID_POINTER;
    if (gamma < 0 || gamma >= 1.0) return LIQ_VALUE_OUT_OF_RANGE;
    if (num_entries <= 0 || num_entries > 1<<30) return LIQ_VALUE_OUT_OF_RANGE;

    if (input_hist->ignorebits > 0 && input_hist->had_image_added) {
        return LIQ_UNSUPPORTED;
    }
    input_hist->ignorebits = 0;

    input_hist->had_image_added = true;
    input_hist->gamma = gamma ? gamma : 0.45455;

    if (!input_hist->acht) {
        input_hist->acht = pam_allocacolorhash(~0, num_entries*num_entries, 0, options->malloc, options->free);
        if (!input_hist->acht) {
            return LIQ_OUT_OF_MEMORY;
        }
    }
    // Fake image size. It's only for hash size estimates.
    if (!input_hist->acht->cols) {
        input_hist->acht->cols = num_entries;
    }
    input_hist->acht->rows += num_entries;

    const unsigned int hash_size = input_hist->acht->hash_size;
    for(int i=0; i < num_entries; i++) {
        const rgba_pixel rgba = {
            .r = entries[i].color.r,
            .g = entries[i].color.g,
            .b = entries[i].color.b,
            .a = entries[i].color.a,
        };
        union rgba_as_int px = {rgba};
        unsigned int hash;
        if (px.rgba.a) {
            hash = px.l % hash_size;
        } else {
            hash=0; px.l=0;
        }
        if (!pam_add_to_hash(input_hist->acht, hash, entries[i].count, px, i, num_entries)) {
            return LIQ_OUT_OF_MEMORY;
        }
    }

    return LIQ_OK;
}

LIQ_EXPORT LIQ_NONNULL liq_error liq_histogram_add_image(liq_histogram *input_hist, const liq_attr *options, liq_image *input_image)
{
    if (!CHECK_STRUCT_TYPE(options, liq_attr)) return LIQ_INVALID_POINTER;
    if (!CHECK_STRUCT_TYPE(input_hist, liq_histogram)) return LIQ_INVALID_POINTER;
    if (!CHECK_STRUCT_TYPE(input_image, liq_image)) return LIQ_INVALID_POINTER;

    const unsigned int cols = input_image->width, rows = input_image->height;

    if (!input_image->importance_map && options->use_contrast_maps) {
        contrast_maps(input_image);
    }

    input_hist->gamma = input_image->gamma;

    for(int i = 0; i < input_image->fixed_colors_count; i++) {
        liq_error res = liq_histogram_add_fixed_color_f(input_hist, input_image->fixed_colors[i]);
        if (res != LIQ_OK) {
            return res;
        }
    }

    /*
     ** Step 2: attempt to make a histogram of the colors, unclustered.
     ** If at first we don't succeed, increase ignorebits to increase color
     ** coherence and try again.
     */

    if (liq_progress(options, options->progress_stage1 * 0.4f)) {
        return LIQ_ABORTED;
    }

    const bool all_rows_at_once = liq_image_can_use_rgba_rows(input_image);

    // Usual solution is to start from scratch when limit is exceeded, but that's not possible if it's not
    // the first image added
    const unsigned int max_histogram_entries = input_hist->had_image_added ? ~0 : options->max_histogram_entries;
    do {
        if (!input_hist->acht) {
            input_hist->acht = pam_allocacolorhash(max_histogram_entries, rows*cols, input_hist->ignorebits, options->malloc, options->free);
        }
        if (!input_hist->acht) return LIQ_OUT_OF_MEMORY;

        // histogram uses noise contrast map for importance. Color accuracy in noisy areas is not very important.
        // noise map does not include edges to avoid ruining anti-aliasing
        for(unsigned int row=0; row < rows; row++) {
            bool added_ok;
            if (all_rows_at_once) {
                added_ok = pam_computeacolorhash(input_hist->acht, (const rgba_pixel *const *)input_image->rows, cols, rows, input_image->importance_map);
                if (added_ok) break;
            } else {
                const rgba_pixel* rows_p[1] = { liq_image_get_row_rgba(input_image, row) };
                added_ok = pam_computeacolorhash(input_hist->acht, rows_p, cols, 1, input_image->importance_map ? &input_image->importance_map[row * cols] : NULL);
            }
            if (!added_ok) {
                input_hist->ignorebits++;
                liq_verbose_printf(options, "  too many colors! Scaling colors to improve clustering... %d", input_hist->ignorebits);
                pam_freeacolorhash(input_hist->acht);
                input_hist->acht = NULL;
                if (liq_progress(options, options->progress_stage1 * 0.6f)) return LIQ_ABORTED;
                break;
            }
        }
    } while(!input_hist->acht);

    input_hist->had_image_added = true;

    liq_image_free_importance_map(input_image);

    if (input_image->free_pixels && input_image->f_pixels) {
        liq_image_free_rgba_source(input_image); // bow can free the RGBA source if copy has been made in f_pixels
    }

    return LIQ_OK;
}

LIQ_NONNULL static liq_error finalize_histogram(liq_histogram *input_hist, liq_attr *options, histogram **hist_output)
{
    if (liq_progress(options, options->progress_stage1 * 0.9f)) {
        return LIQ_ABORTED;
    }

    if (!input_hist->acht) {
        return LIQ_BITMAP_NOT_AVAILABLE;
    }

    histogram *hist = pam_acolorhashtoacolorhist(input_hist->acht, input_hist->gamma, options->malloc, options->free);
    pam_freeacolorhash(input_hist->acht);
    input_hist->acht = NULL;

    if (!hist) {
        return LIQ_OUT_OF_MEMORY;
    }
    liq_verbose_printf(options, "  made histogram...%d colors found", hist->size);
    remove_fixed_colors_from_histogram(hist, input_hist->fixed_colors_count, input_hist->fixed_colors, options->target_mse);

    *hist_output = hist;
    return LIQ_OK;
}

/**
 Builds two maps:
    importance_map - approximation of areas with high-frequency noise, except straight edges. 1=flat, 0=noisy.
    edges - noise map including all edges
 */
LIQ_NONNULL static void contrast_maps(liq_image *image)
{
    const unsigned int cols = image->width, rows = image->height;
    if (cols < 4 || rows < 4 || (3*cols*rows) > LIQ_HIGH_MEMORY_LIMIT) {
        return;
    }

    unsigned char *restrict noise = image->importance_map ? image->importance_map : image->malloc(cols*rows);
    image->importance_map = NULL;
    unsigned char *restrict edges = image->edges ? image->edges : image->malloc(cols*rows);
    image->edges = NULL;

    unsigned char *restrict tmp = image->malloc(cols*rows);

    if (!noise || !edges || !tmp || !liq_image_get_row_f_init(image)) {
        image->free(noise);
        image->free(edges);
        image->free(tmp);
        return;
    }

    const f_pixel *curr_row, *prev_row, *next_row;
    curr_row = prev_row = next_row = liq_image_get_row_f(image, 0);

    for (unsigned int j=0; j < rows; j++) {
        prev_row = curr_row;
        curr_row = next_row;
        next_row = liq_image_get_row_f(image, MIN(rows-1,j+1));

        f_pixel prev, curr = curr_row[0], next=curr;
        for (unsigned int i=0; i < cols; i++) {
            prev=curr;
            curr=next;
            next = curr_row[MIN(cols-1,i+1)];

            // contrast is difference between pixels neighbouring horizontally and vertically
            const float a = fabsf(prev.a+next.a - curr.a*2.f),
                        r = fabsf(prev.r+next.r - curr.r*2.f),
                        g = fabsf(prev.g+next.g - curr.g*2.f),
                        b = fabsf(prev.b+next.b - curr.b*2.f);

            const f_pixel prevl = prev_row[i];
            const f_pixel nextl = next_row[i];

            const float a1 = fabsf(prevl.a+nextl.a - curr.a*2.f),
                        r1 = fabsf(prevl.r+nextl.r - curr.r*2.f),
                        g1 = fabsf(prevl.g+nextl.g - curr.g*2.f),
                        b1 = fabsf(prevl.b+nextl.b - curr.b*2.f);

            const float horiz = MAX(MAX(a,r),MAX(g,b));
            const float vert = MAX(MAX(a1,r1),MAX(g1,b1));
            const float edge = MAX(horiz,vert);
            float z = edge - fabsf(horiz-vert)*.5f;
            z = 1.f - MAX(z,MIN(horiz,vert));
            z *= z; // noise is amplified
            z *= z;
            // 85 is about 1/3rd of weight (not 0, because noisy pixels still need to be included, just not as precisely).
            const unsigned int z_int = 85 + (unsigned int)(z * 171.f);
            noise[j*cols+i] = MIN(z_int, 255);
            const int e_int = 255 - (int)(edge * 256.f);
            edges[j*cols+i] = e_int > 0 ? MIN(e_int, 255) : 0;
        }
    }

    // noise areas are shrunk and then expanded to remove thin edges from the map
    liq_max3(noise, tmp, cols, rows);
    liq_max3(tmp, noise, cols, rows);

    liq_blur(noise, tmp, noise, cols, rows, 3);

    liq_max3(noise, tmp, cols, rows);

    liq_min3(tmp, noise, cols, rows);
    liq_min3(noise, tmp, cols, rows);
    liq_min3(tmp, noise, cols, rows);

    liq_min3(edges, tmp, cols, rows);
    liq_max3(tmp, edges, cols, rows);
    for(unsigned int i=0; i < cols*rows; i++) edges[i] = MIN(noise[i], edges[i]);

    image->free(tmp);

    image->importance_map = noise;
    image->edges = edges;
}

/**
 * Builds map of neighbor pixels mapped to the same palette entry
 *
 * For efficiency/simplicity it mainly looks for same consecutive pixels horizontally
 * and peeks 1 pixel above/below. Full 2d algorithm doesn't improve it significantly.
 * Correct flood fill doesn't have visually good properties.
 */
LIQ_NONNULL static void update_dither_map(liq_image *input_image, unsigned char *const *const row_pointers, colormap *map)
{
    const unsigned int width = input_image->width;
    const unsigned int height = input_image->height;
    unsigned char *const edges = input_image->edges;

    for(unsigned int row=0; row < height; row++) {
        unsigned char lastpixel = row_pointers[row][0];
        unsigned int lastcol=0;

        for(unsigned int col=1; col < width; col++) {
            const unsigned char px = row_pointers[row][col];
            if (input_image->background && map->palette[px].acolor.a < 1.f/256.f) {
                // Transparency may or may not create an edge. When there's an explicit background set, assume no edge.
                continue;
            }

            if (px != lastpixel || col == width-1) {
                int neighbor_count = 10 * (col-lastcol);

                unsigned int i=lastcol;
                while(i < col) {
                    if (row > 0) {
                        unsigned char pixelabove = row_pointers[row-1][i];
                        if (pixelabove == lastpixel) neighbor_count += 15;
                    }
                    if (row < height-1) {
                        unsigned char pixelbelow = row_pointers[row+1][i];
                        if (pixelbelow == lastpixel) neighbor_count += 15;
                    }
                    i++;
                }

                while(lastcol <= col) {
                    int e = edges[row*width + lastcol];
                    edges[row*width + lastcol++] = (e+128) * (255.f/(255+128)) * (1.f - 20.f / (20 + neighbor_count));
                }
                lastpixel = px;
            }
        }
    }
    input_image->dither_map = input_image->edges;
    input_image->edges = NULL;
}

/**
 * Palette can be NULL, in which case it creates a new palette from scratch.
 */
static colormap *add_fixed_colors_to_palette(colormap *palette, const int max_colors, const f_pixel fixed_colors[], const int fixed_colors_count, void* (*malloc)(size_t), void (*free)(void*))
{
    if (!fixed_colors_count) return palette;

    colormap *newpal = pam_colormap(MIN(max_colors, (palette ? palette->colors : 0) + fixed_colors_count), malloc, free);
    unsigned int i=0;
    if (palette && fixed_colors_count < max_colors) {
        unsigned int palette_max = MIN(palette->colors, max_colors - fixed_colors_count);
        for(; i < palette_max; i++) {
            newpal->palette[i] = palette->palette[i];
        }
    }
    for(int j=0; j < MIN(max_colors, fixed_colors_count); j++) {
        newpal->palette[i++] = (colormap_item){
            .acolor = fixed_colors[j],
            .fixed = true,
        };
    }
    if (palette) pam_freecolormap(palette);
    return newpal;
}

LIQ_NONNULL static void adjust_histogram_callback(hist_item *item, float diff)
{
    item->adjusted_weight = (item->perceptual_weight+item->adjusted_weight) * (sqrtf(1.f+diff));
}

/**
 Repeats mediancut with different histogram weights to find palette with minimum error.

 feedback_loop_trials controls how long the search will take. < 0 skips the iteration.
 */
static colormap *find_best_palette(histogram *hist, const liq_attr *options, const double max_mse, const f_pixel fixed_colors[], const unsigned int fixed_colors_count, double *palette_error_p)
{
    unsigned int max_colors = options->max_colors;

    // if output is posterized it doesn't make sense to aim for perfrect colors, so increase target_mse
    // at this point actual gamma is not set, so very conservative posterization estimate is used
    const double target_mse = MIN(max_mse, MAX(options->target_mse, pow((1<<options->min_posterization_output)/1024.0, 2)));
    int feedback_loop_trials = options->feedback_loop_trials;
    if (hist->size > 5000) {feedback_loop_trials = (feedback_loop_trials*3 + 3)/4;}
    if (hist->size > 25000) {feedback_loop_trials = (feedback_loop_trials*3 + 3)/4;}
    if (hist->size > 50000) {feedback_loop_trials = (feedback_loop_trials*3 + 3)/4;}
    if (hist->size > 100000) {feedback_loop_trials = (feedback_loop_trials*3 + 3)/4;}
    colormap *acolormap = NULL;
    double least_error = MAX_DIFF;
    double target_mse_overshoot = feedback_loop_trials>0 ? 1.05 : 1.0;
    const float total_trials = (float)(feedback_loop_trials>0?feedback_loop_trials:1);
    int fails_in_a_row=0;

    do {
        colormap *newmap;
        if (hist->size && fixed_colors_count < max_colors) {
            newmap = mediancut(hist, max_colors-fixed_colors_count, target_mse * target_mse_overshoot, MAX(MAX(45.0/65536.0, target_mse), least_error)*1.2,
                               options->malloc, options->free);
        } else {
            feedback_loop_trials = 0;
            newmap = NULL;
        }
        newmap = add_fixed_colors_to_palette(newmap, max_colors, fixed_colors, fixed_colors_count, options->malloc, options->free);
        if (!newmap) {
            return NULL;
        }

        if (feedback_loop_trials <= 0) {
            return newmap;
        }

        // after palette has been created, total error (MSE) is calculated to keep the best palette
        // at the same time K-Means iteration is done to improve the palette
        // and histogram weights are adjusted based on remapping error to give more weight to poorly matched colors

        const bool first_run_of_target_mse = !acolormap && target_mse > 0;
        double total_error = kmeans_do_iteration(hist, newmap, first_run_of_target_mse ? NULL : adjust_histogram_callback);

        // goal is to increase quality or to reduce number of colors used if quality is good enough
        if (!acolormap || total_error < least_error || (total_error <= target_mse && newmap->colors < max_colors)) {
            if (acolormap) pam_freecolormap(acolormap);
            acolormap = newmap;

            if (total_error < target_mse && total_error > 0) {
                // K-Means iteration improves quality above what mediancut aims for
                // this compensates for it, making mediancut aim for worse
                target_mse_overshoot = MIN(target_mse_overshoot*1.25, target_mse/total_error);
            }

            least_error = total_error;

            // if number of colors could be reduced, try to keep it that way
            // but allow extra color as a bit of wiggle room in case quality can be improved too
            max_colors = MIN(newmap->colors+1, max_colors);

            feedback_loop_trials -= 1; // asymptotic improvement could make it go on forever
            fails_in_a_row = 0;
        } else {
            fails_in_a_row++;
            target_mse_overshoot = 1.0;

            // if error is really bad, it's unlikely to improve, so end sooner
            feedback_loop_trials -= 5 + fails_in_a_row;
            pam_freecolormap(newmap);
        }

        float fraction_done = 1.f-MAX(0.f, feedback_loop_trials/total_trials);
        if (liq_progress(options, options->progress_stage1 + fraction_done * options->progress_stage2)) break;
        liq_verbose_printf(options, "  selecting colors...%d%%", (int)(100.f * fraction_done));
    }
    while(feedback_loop_trials > 0);

    *palette_error_p = least_error;
    return acolormap;
}

static colormap *histogram_to_palette(const histogram *hist, const liq_attr *options) {
    if (!hist->size) {
        return NULL;
    }
    colormap *acolormap = pam_colormap(hist->size, options->malloc, options->free);
    for(unsigned int i=0; i < hist->size; i++) {
        acolormap->palette[i].acolor = hist->achv[i].acolor;
        acolormap->palette[i].popularity = hist->achv[i].perceptual_weight;
    }
    return acolormap;
}

LIQ_NONNULL static liq_error pngquant_quantize(histogram *hist, const liq_attr *options, const int fixed_colors_count, const f_pixel fixed_colors[], const double gamma, bool fixed_result_colors, liq_result **result_output)
{
    colormap *acolormap;
    double palette_error = -1;

    assert((verbose_print(options, "SLOW debug checks enabled. Recompile with NDEBUG for normal operation."),1));

    const bool few_input_colors = hist->size+fixed_colors_count <= options->max_colors;

    if (liq_progress(options, options->progress_stage1)) return LIQ_ABORTED;

    // If image has few colors to begin with (and no quality degradation is required)
    // then it's possible to skip quantization entirely
    if (few_input_colors && options->target_mse == 0) {
        acolormap = add_fixed_colors_to_palette(histogram_to_palette(hist, options), options->max_colors, fixed_colors, fixed_colors_count, options->malloc, options->free);
        palette_error = 0;
    } else {
        const double max_mse = options->max_mse * (few_input_colors ? 0.33 : 1.0); // when degrading image that's already paletted, require much higher improvement, since pal2pal often looks bad and there's little gain
        acolormap = find_best_palette(hist, options, max_mse, fixed_colors, fixed_colors_count, &palette_error);
        if (!acolormap) {
            return LIQ_VALUE_OUT_OF_RANGE;
        }

        // K-Means iteration approaches local minimum for the palette
        double iteration_limit = options->kmeans_iteration_limit;
        unsigned int iterations = options->kmeans_iterations;

        if (!iterations && palette_error < 0 && max_mse < MAX_DIFF) iterations = 1; // otherwise total error is never calculated and MSE limit won't work

        if (iterations) {
            // likely_colormap_index (used and set in kmeans_do_iteration) can't point to index outside colormap
            if (acolormap->colors < 256) for(unsigned int j=0; j < hist->size; j++) {
                if (hist->achv[j].tmp.likely_colormap_index >= acolormap->colors) {
                    hist->achv[j].tmp.likely_colormap_index = 0; // actual value doesn't matter, as the guess is out of date anyway
                }
            }

            if (hist->size > 5000) {iterations = (iterations*3 + 3)/4;}
            if (hist->size > 25000) {iterations = (iterations*3 + 3)/4;}
            if (hist->size > 50000) {iterations = (iterations*3 + 3)/4;}
            if (hist->size > 100000) {iterations = (iterations*3 + 3)/4; iteration_limit *= 2;}

            verbose_print(options, "  moving colormap towards local minimum");

            double previous_palette_error = MAX_DIFF;

            for(unsigned int i=0; i < iterations; i++) {
                palette_error = kmeans_do_iteration(hist, acolormap, NULL);

                if (liq_progress(options, options->progress_stage1 + options->progress_stage2 + (i * options->progress_stage3 * 0.9f) / iterations)) {
                    break;
                }

                if (fabs(previous_palette_error-palette_error) < iteration_limit) {
                    break;
                }

                if (palette_error > max_mse*1.5) { // probably hopeless
                    if (palette_error > max_mse*3.0) break; // definitely hopeless
                    i++;
                }

                previous_palette_error = palette_error;
            }
        }

        //if (palette_error > max_mse) {
        //    pam_freecolormap(acolormap);
        //    return LIQ_QUALITY_TOO_LOW;
        //}
    }

    if (liq_progress(options, options->progress_stage1 + options->progress_stage2 + options->progress_stage3 * 0.95f)) {
        pam_freecolormap(acolormap);
        return LIQ_ABORTED;
    }

    sort_palette(acolormap, options);

    // If palette was created from a multi-image histogram,
    // then it shouldn't be optimized for one image during remapping
    if (fixed_result_colors) {
        for(unsigned int i=0; i < acolormap->colors; i++) {
            acolormap->palette[i].fixed = true;
        }
    }

    liq_result *result = options->malloc(sizeof(liq_result));
    if (!result) return LIQ_OUT_OF_MEMORY;
    *result = (liq_result){
        .magic_header = liq_result_magic,
        .malloc = options->malloc,
        .free = options->free,
        .palette = acolormap,
        .palette_error = palette_error,
        .use_dither_map = options->use_dither_map,
        .gamma = gamma,
        .min_posterization_output = options->min_posterization_output,
    };
    *result_output = result;
    return LIQ_OK;
}

LIQ_EXPORT LIQ_NONNULL liq_error liq_write_remapped_image(liq_result *result, liq_image *input_image, void *buffer, size_t buffer_size)
{
    if (!CHECK_STRUCT_TYPE(result, liq_result)) {
        return LIQ_INVALID_POINTER;
    }
    if (!CHECK_STRUCT_TYPE(input_image, liq_image)) {
        return LIQ_INVALID_POINTER;
    }
    if (!CHECK_USER_POINTER(buffer)) {
        return LIQ_INVALID_POINTER;
    }

    const size_t required_size = (size_t)input_image->width * (size_t)input_image->height;
    if (buffer_size < required_size) {
        return LIQ_BUFFER_TOO_SMALL;
    }

    LIQ_ARRAY(unsigned char *, rows, input_image->height);
    unsigned char *buffer_bytes = buffer;
    for(unsigned int i=0; i < input_image->height; i++) {
        rows[i] = &buffer_bytes[input_image->width * i];
    }
    return liq_write_remapped_image_rows(result, input_image, rows);
}

LIQ_EXPORT LIQ_NONNULL liq_error liq_write_remapped_image_rows(liq_result *quant, liq_image *input_image, unsigned char **row_pointers)
{
    if (!CHECK_STRUCT_TYPE(quant, liq_result)) return LIQ_INVALID_POINTER;
    if (!CHECK_STRUCT_TYPE(input_image, liq_image)) return LIQ_INVALID_POINTER;
    for(unsigned int i=0; i < input_image->height; i++) {
        if (!CHECK_USER_POINTER(row_pointers+i) || !CHECK_USER_POINTER(row_pointers[i])) return LIQ_INVALID_POINTER;
    }

    if (quant->remapping) {
        liq_remapping_result_destroy(quant->remapping);
    }
    liq_remapping_result *const result = quant->remapping = liq_remapping_result_create(quant);
    if (!result) return LIQ_OUT_OF_MEMORY;

    if (!input_image->edges && !input_image->dither_map && quant->use_dither_map) {
        contrast_maps(input_image);
    }

    if (liq_remap_progress(result, result->progress_stage1 * 0.25f)) {
        return LIQ_ABORTED;
    }

    /*
     ** Step 4: map the colors in the image to their closest match in the
     ** new colormap, and write 'em out.
     */

    float remapping_error = result->palette_error;
    if (result->dither_level == 0) {
        set_rounded_palette(&result->int_palette, result->palette, result->gamma, quant->min_posterization_output);
        remapping_error = remap_to_palette(input_image, row_pointers, result->palette);
    } else {
        const bool is_image_huge = (input_image->width * input_image->height) > 2000 * 2000;
        const bool allow_dither_map = result->use_dither_map == 2 || (!is_image_huge && result->use_dither_map);
        const bool generate_dither_map = allow_dither_map && (input_image->edges && !input_image->dither_map);
        if (generate_dither_map) {
            // If dithering (with dither map) is required, this image is used to find areas that require dithering
            remapping_error = remap_to_palette(input_image, row_pointers, result->palette);
            update_dither_map(input_image, row_pointers, result->palette);
        }

        if (liq_remap_progress(result, result->progress_stage1 * 0.5f)) {
            return LIQ_ABORTED;
        }

        // remapping above was the last chance to do K-Means iteration, hence the final palette is set after remapping
        set_rounded_palette(&result->int_palette, result->palette, result->gamma, quant->min_posterization_output);

        if (!remap_to_palette_floyd(input_image, row_pointers, result, MAX(remapping_error*2.4, 16.f/256.f), generate_dither_map)) {
            return LIQ_ABORTED;
        }
    }

    // remapping error from dithered image is absurd, so always non-dithered value is used
    // palette_error includes some perceptual weighting from histogram which is closer correlated with dssim
    // so that should be used when possible.
    if (result->palette_error < 0) {
        result->palette_error = remapping_error;
    }

    return LIQ_OK;
}

LIQ_EXPORT int liq_version() {
    return LIQ_VERSION;
}

LIQ_PRIVATE bool pam_computeacolorhash(struct acolorhash_table* acht, const rgba_pixel* const pixels[], unsigned int cols, unsigned int rows, const unsigned char* importance_map)
{
    const unsigned int ignorebits = acht->ignorebits;
    const unsigned int channel_mask = 255U >> ignorebits << ignorebits;
    const unsigned int channel_hmask = (255U >> ignorebits) ^ 0xFFU;
    const unsigned int posterize_mask = channel_mask << 24 | channel_mask << 16 | channel_mask << 8 | channel_mask;
    const unsigned int posterize_high_mask = channel_hmask << 24 | channel_hmask << 16 | channel_hmask << 8 | channel_hmask;

    const unsigned int hash_size = acht->hash_size;

    /* Go through the entire image, building a hash table of colors. */
    for (unsigned int row = 0; row < rows; ++row) {

        for (unsigned int col = 0; col < cols; ++col) {
            unsigned int boost;

            // RGBA color is casted to long for easier hasing/comparisons
            union rgba_as_int px = { pixels[row][col] };
            unsigned int hash;
            if (!px.rgba.a) {
                // "dirty alpha" has different RGBA values that end up being the same fully transparent color
                px.l = 0; hash = 0;

                boost = 2000;
                if (importance_map) {
                    importance_map++;
                }
            }
            else {
                // mask posterizes all 4 channels in one go
                px.l = (px.l & posterize_mask) | ((px.l & posterize_high_mask) >> (8 - ignorebits));
                // fancier hashing algorithms didn't improve much
                hash = px.l % hash_size;

                if (importance_map) {
                    boost = *importance_map++;
                }
                else {
                    boost = 255;
                }
            }

            if (!pam_add_to_hash(acht, hash, boost, px, row, rows)) {
                return false;
            }
        }

    }
    acht->cols = cols;
    acht->rows += rows;
    return true;
}

LIQ_PRIVATE bool pam_add_to_hash(struct acolorhash_table* acht, unsigned int hash, unsigned int boost, union rgba_as_int px, unsigned int row, unsigned int rows)
{
    /* head of the hash function stores first 2 colors inline (achl->used = 1..2),
       to reduce number of allocations of achl->other_items.
     */
    struct acolorhist_arr_head* achl = &acht->buckets[hash];
    if (achl->inline1.color.l == px.l && achl->used) {
        achl->inline1.perceptual_weight += boost;
        return true;
    }
    if (achl->used) {
        if (achl->used > 1) {
            if (achl->inline2.color.l == px.l) {
                achl->inline2.perceptual_weight += boost;
                return true;
            }
            // other items are stored as an array (which gets reallocated if needed)
            struct acolorhist_arr_item* other_items = achl->other_items;
            unsigned int i = 0;
            for (; i < achl->used - 2; i++) {
                if (other_items[i].color.l == px.l) {
                    other_items[i].perceptual_weight += boost;
                    return true;
                }
            }

            // the array was allocated with spare items
            if (i < achl->capacity) {
                other_items[i] = (struct acolorhist_arr_item){
                    .color = px,
                    .perceptual_weight = boost,
                };
                achl->used++;
                ++acht->colors;
                return true;
            }

            if (++acht->colors > acht->maxcolors) {
                return false;
            }

            struct acolorhist_arr_item* new_items;
            unsigned int capacity;
            if (!other_items) { // there was no array previously, alloc "small" array
                capacity = 8;
                if (acht->freestackp <= 0) {
                    // estimate how many colors are going to be + headroom
                    const size_t mempool_size = ((acht->rows + rows - row) * 2 * acht->colors / (acht->rows + row + 1) + 1024) * sizeof(struct acolorhist_arr_item);
                    new_items = mempool_alloc(&acht->mempool, sizeof(struct acolorhist_arr_item) * capacity, mempool_size);
                }
                else {
                    // freestack stores previously freed (reallocated) arrays that can be reused
                    // (all pesimistically assumed to be capacity = 8)
                    new_items = acht->freestack[--acht->freestackp];
                }
            }
            else {
                const unsigned int stacksize = sizeof(acht->freestack) / sizeof(acht->freestack[0]);

                // simply reallocs and copies array to larger capacity
                capacity = achl->capacity * 2 + 16;
                if (acht->freestackp < stacksize - 1) {
                    acht->freestack[acht->freestackp++] = other_items;
                }
                const size_t mempool_size = ((acht->rows + rows - row) * 2 * acht->colors / (acht->rows + row + 1) + 32 * capacity) * sizeof(struct acolorhist_arr_item);
                new_items = mempool_alloc(&acht->mempool, sizeof(struct acolorhist_arr_item) * capacity, mempool_size);
                if (!new_items) return false;
                memcpy(new_items, other_items, sizeof(other_items[0]) * achl->capacity);
            }

            achl->other_items = new_items;
            achl->capacity = capacity;
            new_items[i] = (struct acolorhist_arr_item){
                .color = px,
                .perceptual_weight = boost,
            };
            achl->used++;
        }
        else {
            // these are elses for first checks whether first and second inline-stored colors are used
            achl->inline2.color.l = px.l;
            achl->inline2.perceptual_weight = boost;
            achl->used = 2;
            ++acht->colors;
        }
    }
    else {
        achl->inline1.color.l = px.l;
        achl->inline1.perceptual_weight = boost;
        achl->used = 1;
        ++acht->colors;
    }
    return true;
}

LIQ_PRIVATE struct acolorhash_table* pam_allocacolorhash(unsigned int maxcolors, unsigned int surface, unsigned int ignorebits, void* (*malloc)(size_t), void (*free)(void*))
{
    const size_t estimated_colors = MIN(maxcolors, surface / (ignorebits + (surface > 512 * 512 ? 6 : 5)));
    const size_t hash_size = estimated_colors < 66000 ? 6673 : (estimated_colors < 200000 ? 12011 : 24019);

    mempoolptr m = NULL;
    const size_t buckets_size = hash_size * sizeof(struct acolorhist_arr_head);
    const size_t mempool_size = sizeof(struct acolorhash_table) + buckets_size + estimated_colors * sizeof(struct acolorhist_arr_item);
    struct acolorhash_table* t = mempool_create(&m, sizeof(*t) + buckets_size, mempool_size, malloc, free);
    if (!t) return NULL;
    *t = (struct acolorhash_table){
        .mempool = m,
        .hash_size = hash_size,
        .maxcolors = maxcolors,
        .ignorebits = ignorebits,
    };
    memset(t->buckets, 0, buckets_size);
    return t;
}

ALWAYS_INLINE static float pam_add_to_hist(const float* gamma_lut, hist_item* achv, unsigned int* j, const struct acolorhist_arr_item* entry, const float max_perceptual_weight)
{
    if (entry->perceptual_weight == 0 && *j > 0) {
        return 0;
    }
    const float w = MIN(entry->perceptual_weight / 128.f, max_perceptual_weight);
    achv[*j].adjusted_weight = achv[*j].perceptual_weight = w;
    achv[*j].acolor = rgba_to_f(gamma_lut, entry->color.rgba);
    *j += 1;
    return w;
}

LIQ_PRIVATE histogram* pam_acolorhashtoacolorhist(const struct acolorhash_table* acht, const double gamma, void* (*malloc)(size_t), void (*free)(void*))
{
    histogram* hist = malloc(sizeof(hist[0]));
    if (!hist || !acht) return NULL;
    *hist = (histogram){
        .achv = malloc(MAX(1,acht->colors) * sizeof(hist->achv[0])),
        .size = acht->colors,
        .free = free,
        .ignorebits = acht->ignorebits,
    };
    if (!hist->achv) return NULL;

    float gamma_lut[256];
    to_f_set_gamma(gamma_lut, gamma);

    /* Limit perceptual weight to 1/10th of the image surface area to prevent
       a single color from dominating all others. */
    float max_perceptual_weight = 0.1f * acht->cols * acht->rows;
    double total_weight = 0;

    unsigned int j = 0;
    for (unsigned int i = 0; i < acht->hash_size; ++i) {
        const struct acolorhist_arr_head* const achl = &acht->buckets[i];
        if (achl->used) {
            total_weight += pam_add_to_hist(gamma_lut, hist->achv, &j, &achl->inline1, max_perceptual_weight);

            if (achl->used > 1) {
                total_weight += pam_add_to_hist(gamma_lut, hist->achv, &j, &achl->inline2, max_perceptual_weight);

                for (unsigned int k = 0; k < achl->used - 2; k++) {
                    total_weight += pam_add_to_hist(gamma_lut, hist->achv, &j, &achl->other_items[k], max_perceptual_weight);
                }
            }
        }
    }
    hist->size = j;
    hist->total_perceptual_weight = total_weight;
    for (unsigned int k = 0; k < hist->size; k++) {
        hist->achv[k].tmp.likely_colormap_index = 0;
    }
    if (!j) {
        pam_freeacolorhist(hist);
        return NULL;
    }
    return hist;
}

LIQ_PRIVATE void pam_freeacolorhash(struct acolorhash_table* acht)
{
    if (acht) {
        mempool_destroy(acht->mempool);
    }
}

LIQ_PRIVATE void pam_freeacolorhist(histogram* hist)
{
    hist->free(hist->achv);
    hist->free(hist);
}

LIQ_PRIVATE colormap* pam_colormap(unsigned int colors, void* (*malloc)(size_t), void (*free)(void*))
{
    assert(colors > 0 && colors < 65536);

    colormap* map;
    const size_t colors_size = colors * sizeof(map->palette[0]);
    map = malloc(sizeof(colormap) + colors_size);
    if (!map) return NULL;
    *map = (colormap){
        .malloc = malloc,
        .free = free,
        .colors = colors,
    };
    memset(map->palette, 0, colors_size);
    return map;
}

LIQ_PRIVATE colormap* pam_duplicate_colormap(colormap* map)
{
    colormap* dupe = pam_colormap(map->colors, map->malloc, map->free);
    for (unsigned int i = 0; i < map->colors; i++) {
        dupe->palette[i] = map->palette[i];
    }
    return dupe;
}

LIQ_PRIVATE void pam_freecolormap(colormap* c)
{
    c->free(c);
}

LIQ_PRIVATE void to_f_set_gamma(float gamma_lut[], const double gamma)
{
    for (int i = 0; i < 256; i++) {
        gamma_lut[i] = pow((double)i / 255.0, internal_gamma / gamma);
    }
}



static void vp_search_node(const vp_node* node, const f_pixel* const needle, vp_search_tmp* const best_candidate);

static int vp_compare_distance(const void* ap, const void* bp) {
    float a = ((const vp_sort_tmp*)ap)->distance_squared;
    float b = ((const vp_sort_tmp*)bp)->distance_squared;
    return a > b ? 1 : -1;
}

static void vp_sort_indexes_by_distance(const f_pixel vantage_point, vp_sort_tmp indexes[], int num_indexes, const colormap_item items[]) {
    for (int i = 0; i < num_indexes; i++) {
        indexes[i].distance_squared = colordifference(vantage_point, items[indexes[i].idx].acolor);
    }
    qsort(indexes, num_indexes, sizeof(indexes[0]), vp_compare_distance);
}

/*
 * Usually it should pick farthest point, but picking most popular point seems to make search quicker anyway
 */
static int vp_find_best_vantage_point_index(vp_sort_tmp indexes[], int num_indexes, const colormap_item items[]) {
    int best = 0;
    float best_popularity = items[indexes[0].idx].popularity;
    for (int i = 1; i < num_indexes; i++) {
        if (items[indexes[i].idx].popularity > best_popularity) {
            best_popularity = items[indexes[i].idx].popularity;
            best = i;
        }
    }
    return best;
}

static vp_node* vp_create_node(mempoolptr* m, vp_sort_tmp indexes[], int num_indexes, const colormap_item items[]) {
    if (num_indexes <= 0) {
        return NULL;
    }

    vp_node* node = mempool_alloc(m, sizeof(node[0]), 0);

    if (num_indexes == 1) {
        *node = (vp_node){
            .vantage_point = items[indexes[0].idx].acolor,
            .idx = indexes[0].idx,
            .radius = MAX_DIFF,
            .radius_squared = MAX_DIFF,
        };
        return node;
    }

    const int ref = vp_find_best_vantage_point_index(indexes, num_indexes, items);
    const int ref_idx = indexes[ref].idx;

    // Removes the `ref_idx` item from remaining items, because it's included in the current node
    num_indexes -= 1;
    indexes[ref] = indexes[num_indexes];

    vp_sort_indexes_by_distance(items[ref_idx].acolor, indexes, num_indexes, items);

    // Remaining items are split by the median distance
    const int half_idx = num_indexes / 2;

    *node = (vp_node){
        .vantage_point = items[ref_idx].acolor,
        .idx = ref_idx,
        .radius = sqrtf(indexes[half_idx].distance_squared),
        .radius_squared = indexes[half_idx].distance_squared,
    };
    if (num_indexes < 7) {
        node->rest = mempool_alloc(m, sizeof(node->rest[0]) * num_indexes, 0);
        node->restcount = num_indexes;
        for (int i = 0; i < num_indexes; i++) {
            node->rest[i].idx = indexes[i].idx;
            node->rest[i].color = items[indexes[i].idx].acolor;
        }
    }
    else {
        node->near = vp_create_node(m, indexes, half_idx, items);
        node->far = vp_create_node(m, &indexes[half_idx], num_indexes - half_idx, items);
    }

    return node;
}

LIQ_PRIVATE struct nearest_map* nearest_init(const colormap* map) {
    mempoolptr m = NULL;
    struct nearest_map* handle = mempool_create(&m, sizeof(handle[0]), sizeof(handle[0]) + sizeof(vp_node) * map->colors + 16, map->malloc, map->free);

    LIQ_ARRAY(vp_sort_tmp, indexes, map->colors);

    for (unsigned int i = 0; i < map->colors; i++) {
        indexes[i].idx = i;
    }

    vp_node* root = vp_create_node(&m, indexes, map->colors, map->palette);
    *handle = (struct nearest_map){
        .root = root,
        .palette = map->palette,
        .mempool = m,
    };

    for (unsigned int i = 0; i < map->colors; i++) {
        vp_search_tmp best = {
            .distance = MAX_DIFF,
            .distance_squared = MAX_DIFF,
            .exclude = i,
        };
        vp_search_node(root, &map->palette[i].acolor, &best);
        handle->nearest_other_color_dist[i] = best.distance * best.distance / 4.0; // half of squared distance
    }

    return handle;
}

static void vp_search_node(const vp_node* node, const f_pixel* const needle, vp_search_tmp* const best_candidate) {
    do {
        const float distance_squared = colordifference(node->vantage_point, *needle);
        const float distance = sqrtf(distance_squared);

        if (distance_squared < best_candidate->distance_squared && best_candidate->exclude != node->idx) {
            best_candidate->distance = distance;
            best_candidate->distance_squared = distance_squared;
            best_candidate->idx = node->idx;
        }

        if (node->restcount) {
            for (int i = 0; i < node->restcount; i++) {
                const float distance_squared = colordifference(node->rest[i].color, *needle);
                if (distance_squared < best_candidate->distance_squared && best_candidate->exclude != node->rest[i].idx) {
                    best_candidate->distance = sqrtf(distance_squared);
                    best_candidate->distance_squared = distance_squared;
                    best_candidate->idx = node->rest[i].idx;
                }
            }
            return;
        }

        // Recurse towards most likely candidate first to narrow best candidate's distance as soon as possible
        if (distance_squared < node->radius_squared) {
            if (node->near) {
                vp_search_node(node->near, needle, best_candidate);
            }
            // The best node (final answer) may be just ouside the radius, but not farther than
            // the best distance we know so far. The vp_search_node above should have narrowed
            // best_candidate->distance, so this path is rarely taken.
            if (node->far && distance >= node->radius - best_candidate->distance) {
                node = node->far; // Fast tail recursion
            }
            else {
                return;
            }
        }
        else {
            if (node->far) {
                vp_search_node(node->far, needle, best_candidate);
            }
            if (node->near && distance <= node->radius + best_candidate->distance) {
                node = node->near; // Fast tail recursion
            }
            else {
                return;
            }
        }
    } while (true);
}

LIQ_PRIVATE unsigned int nearest_search(const struct nearest_map* handle, const f_pixel* px, const int likely_colormap_index, float* diff) {
    const float guess_diff = colordifference(handle->palette[likely_colormap_index].acolor, *px);
    if (guess_diff < handle->nearest_other_color_dist[likely_colormap_index]) {
        if (diff) *diff = guess_diff;
        return likely_colormap_index;
    }

    vp_search_tmp best_candidate = {
        .distance = sqrtf(guess_diff),
        .distance_squared = guess_diff,
        .idx = likely_colormap_index,
        .exclude = -1,
    };
    vp_search_node(handle->root, px, &best_candidate);
    if (diff) {
        *diff = best_candidate.distance * best_candidate.distance;
    }
    return best_candidate.idx;
}

LIQ_PRIVATE void nearest_free(struct nearest_map* centroids)
{
    mempool_destroy(centroids->mempool);
}

static void transposing_1d_blur(unsigned char* restrict src, unsigned char* restrict dst, unsigned int width, unsigned int height, const unsigned int size)
{
    assert(size > 0);

    for (unsigned int j = 0; j < height; j++) {
        unsigned char* restrict row = src + j * width;

        // accumulate sum for pixels outside line
        unsigned int sum;
        sum = row[0] * size;
        for (unsigned int i = 0; i < size; i++) {
            sum += row[i];
        }

        // blur with left side outside line
        for (unsigned int i = 0; i < size; i++) {
            sum -= row[0];
            sum += row[i + size];

            dst[i * height + j] = sum / (size * 2);
        }

        for (unsigned int i = size; i < width - size; i++) {
            sum -= row[i - size];
            sum += row[i + size];

            dst[i * height + j] = sum / (size * 2);
        }

        // blur with right side outside line
        for (unsigned int i = width - size; i < width; i++) {
            sum -= row[i - size];
            sum += row[width - 1];

            dst[i * height + j] = sum / (size * 2);
        }
    }
}

LIQ_PRIVATE void liq_max3(unsigned char* src, unsigned char* dst, unsigned int width, unsigned int height)
{
    for (unsigned int j = 0; j < height; j++) {
        const unsigned char* row = src + j * width,
            * prevrow = src + (j > 1 ? j - 1 : 0) * width,
            * nextrow = src + MIN(height - 1, j + 1) * width;

        unsigned char prev, curr = row[0], next = row[0];

        for (unsigned int i = 0; i < width - 1; i++) {
            prev = curr;
            curr = next;
            next = row[i + 1];

            unsigned char t1 = MAX(prev, next);
            unsigned char t2 = MAX(nextrow[i], prevrow[i]);
            *dst++ = MAX(curr, MAX(t1, t2));
        }
        unsigned char t1 = MAX(curr, next);
        unsigned char t2 = MAX(nextrow[width - 1], prevrow[width - 1]);
        *dst++ = MAX(t1, t2);
    }
}

LIQ_PRIVATE void liq_min3(unsigned char* src, unsigned char* dst, unsigned int width, unsigned int height)
{
    for (unsigned int j = 0; j < height; j++) {
        const unsigned char* row = src + j * width,
            * prevrow = src + (j > 1 ? j - 1 : 0) * width,
            * nextrow = src + MIN(height - 1, j + 1) * width;

        unsigned char prev, curr = row[0], next = row[0];

        for (unsigned int i = 0; i < width - 1; i++) {
            prev = curr;
            curr = next;
            next = row[i + 1];

            unsigned char t1 = MIN(prev, next);
            unsigned char t2 = MIN(nextrow[i], prevrow[i]);
            *dst++ = MIN(curr, MIN(t1, t2));
        }
        unsigned char t1 = MIN(curr, next);
        unsigned char t2 = MIN(nextrow[width - 1], prevrow[width - 1]);
        *dst++ = MIN(t1, t2);
    }
}

LIQ_PRIVATE void liq_blur(unsigned char* src, unsigned char* tmp, unsigned char* dst, unsigned int width, unsigned int height, unsigned int size)
{
    assert(size > 0);
    if (width < 2 * size + 1 || height < 2 * size + 1) {
        return;
    }
    transposing_1d_blur(src, tmp, width, height, size);
    transposing_1d_blur(tmp, dst, height, width, size);
}


#define omp_get_max_threads() 1
#define omp_get_thread_num() 0

/*
 * K-Means iteration: new palette color is computed from weighted average of colors that map to that palette entry.
 */
LIQ_PRIVATE void kmeans_init(const colormap* map, const unsigned int max_threads, kmeans_state average_color[])
{
    memset(average_color, 0, sizeof(average_color[0]) * (KMEANS_CACHE_LINE_GAP + map->colors) * max_threads);
}

LIQ_PRIVATE void kmeans_update_color(const f_pixel acolor, const float value, const colormap* map, unsigned int match, const unsigned int thread, kmeans_state average_color[])
{
    match += thread * (KMEANS_CACHE_LINE_GAP + map->colors);
    average_color[match].a += acolor.a * value;
    average_color[match].r += acolor.r * value;
    average_color[match].g += acolor.g * value;
    average_color[match].b += acolor.b * value;
    average_color[match].total += value;
}

LIQ_PRIVATE void kmeans_finalize(colormap* map, const unsigned int max_threads, const kmeans_state average_color[])
{
    for (unsigned int i = 0; i < map->colors; i++) {
        double a = 0, r = 0, g = 0, b = 0, total = 0;

        // Aggregate results from all threads
        for (unsigned int t = 0; t < max_threads; t++) {
            const unsigned int offset = (KMEANS_CACHE_LINE_GAP + map->colors) * t + i;

            a += average_color[offset].a;
            r += average_color[offset].r;
            g += average_color[offset].g;
            b += average_color[offset].b;
            total += average_color[offset].total;
        }

        if (!map->palette[i].fixed) {
            map->palette[i].popularity = total;
            if (total) {
                map->palette[i].acolor = (f_pixel){
                    .a = a / total,
                    .r = r / total,
                    .g = g / total,
                    .b = b / total,
                };
            }
            else {
                // if a color is useless, make a new one
                // (it was supposed to be random, but Android NDK has problematic stdlib headers)
                map->palette[i].acolor.a = map->palette[(i + 1) % map->colors].acolor.a;
                map->palette[i].acolor.r = map->palette[(i + 2) % map->colors].acolor.r;
                map->palette[i].acolor.g = map->palette[(i + 3) % map->colors].acolor.g;
                map->palette[i].acolor.b = map->palette[(i + 4) % map->colors].acolor.b;
            }
        }
    }
}

LIQ_PRIVATE double kmeans_do_iteration(histogram* hist, colormap* const map, kmeans_callback callback)
{
    const unsigned int max_threads = omp_get_max_threads();
    LIQ_ARRAY(kmeans_state, average_color, (KMEANS_CACHE_LINE_GAP + map->colors) * max_threads);
    kmeans_init(map, max_threads, average_color);
    struct nearest_map* const n = nearest_init(map);
    hist_item* const achv = hist->achv;
    const int hist_size = hist->size;

    double total_diff = 0;

    for (int j = 0; j < hist_size; j++) {
        float diff;
        const f_pixel px = achv[j].acolor;
        const unsigned int match = nearest_search(n, &px, achv[j].tmp.likely_colormap_index, &diff);
        achv[j].tmp.likely_colormap_index = match;

        if (callback) {
            // Check how average diff would look like if there was dithering
            const f_pixel remapped = map->palette[match].acolor;
            nearest_search(n, &(f_pixel){
                .a = px.a + px.a - remapped.a,
                    .r = px.r + px.r - remapped.r,
                    .g = px.g + px.g - remapped.g,
                    .b = px.b + px.b - remapped.b,
            }, match, & diff);

            callback(&achv[j], diff);
        }

        total_diff += diff * achv[j].perceptual_weight;

        kmeans_update_color(px, achv[j].adjusted_weight, map, match, omp_get_thread_num(), average_color);
    }

    nearest_free(n);
    kmeans_finalize(map, max_threads, average_color);

    return total_diff / hist->total_perceptual_weight;
}


#define index_of_channel(ch) (offsetof(f_pixel,ch)/sizeof(float))

static f_pixel averagepixels(unsigned int clrs, const hist_item achv[]);

struct box {
    f_pixel color;
    f_pixel variance;
    double sum, total_error, max_error;
    unsigned int ind;
    unsigned int colors;
};

ALWAYS_INLINE static double variance_diff(double val, const double good_enough);
inline static double variance_diff(double val, const double good_enough)
{
    val *= val;
    if (val < good_enough * good_enough) return val * 0.25;
    return val;
}

/** Weighted per-channel variance of the box. It's used to decide which channel to split by */
static f_pixel box_variance(const hist_item achv[], const struct box* box)
{
    f_pixel mean = box->color;
    double variancea = 0, variancer = 0, varianceg = 0, varianceb = 0;

    for (unsigned int i = 0; i < box->colors; ++i) {
        const f_pixel px = achv[box->ind + i].acolor;
        double weight = achv[box->ind + i].adjusted_weight;
        variancea += variance_diff(mean.a - px.a, 2.0 / 256.0) * weight;
        variancer += variance_diff(mean.r - px.r, 1.0 / 256.0) * weight;
        varianceg += variance_diff(mean.g - px.g, 1.0 / 256.0) * weight;
        varianceb += variance_diff(mean.b - px.b, 1.0 / 256.0) * weight;
    }

    return (f_pixel) {
        .a = variancea * (4.0 / 16.0),
            .r = variancer * (7.0 / 16.0),
            .g = varianceg * (9.0 / 16.0),
            .b = varianceb * (5.0 / 16.0),
    };
}

static double box_max_error(const hist_item achv[], const struct box* box)
{
    f_pixel mean = box->color;
    double max_error = 0;

    for (unsigned int i = 0; i < box->colors; ++i) {
        const double diff = colordifference(mean, achv[box->ind + i].acolor);
        if (diff > max_error) {
            max_error = diff;
        }
    }
    return max_error;
}

ALWAYS_INLINE static double color_weight(f_pixel median, hist_item h);

static inline void hist_item_swap(hist_item* l, hist_item* r)
{
    if (l != r) {
        hist_item t = *l;
        *l = *r;
        *r = t;
    }
}

ALWAYS_INLINE static unsigned int qsort_pivot(const hist_item* const base, const unsigned int len);
inline static unsigned int qsort_pivot(const hist_item* const base, const unsigned int len)
{
    if (len < 32) {
        return len / 2;
    }

    const unsigned int aidx = 8, bidx = len / 2, cidx = len - 1;
    const unsigned int a = base[aidx].tmp.sort_value, b = base[bidx].tmp.sort_value, c = base[cidx].tmp.sort_value;
    return (a < b) ? ((b < c) ? bidx : ((a < c) ? cidx : aidx))
        : ((b > c) ? bidx : ((a < c) ? aidx : cidx));
}

ALWAYS_INLINE static unsigned int qsort_partition(hist_item* const base, const unsigned int len);
inline static unsigned int qsort_partition(hist_item* const base, const unsigned int len)
{
    unsigned int l = 1, r = len;
    if (len >= 8) {
        hist_item_swap(&base[0], &base[qsort_pivot(base, len)]);
    }

    const unsigned int pivot_value = base[0].tmp.sort_value;
    while (l < r) {
        if (base[l].tmp.sort_value >= pivot_value) {
            l++;
        }
        else {
            while (l < --r && base[r].tmp.sort_value <= pivot_value) {}
            hist_item_swap(&base[l], &base[r]);
        }
    }
    l--;
    hist_item_swap(&base[0], &base[l]);

    return l;
}

/** quick select algorithm */
static void hist_item_sort_range(hist_item base[], unsigned int len, unsigned int sort_start)
{
    for (;;) {
        const unsigned int l = qsort_partition(base, len), r = l + 1;

        if (l > 0 && sort_start < l) {
            len = l;
        }
        else if (r < len && sort_start > r) {
            base += r; len -= r; sort_start -= r;
        }
        else break;
    }
}

/** sorts array to make sum of weights lower than halfvar one side, returns edge between <halfvar and >halfvar parts of the set */
static hist_item* hist_item_sort_halfvar(hist_item base[], unsigned int len, double* const lowervar, const double halfvar)
{
    do {
        const unsigned int l = qsort_partition(base, len), r = l + 1;

        // check if sum of left side is smaller than half,
        // if it is, then it doesn't need to be sorted
        unsigned int t = 0; double tmpsum = *lowervar;
        while (t <= l && tmpsum < halfvar) tmpsum += base[t++].color_weight;

        if (tmpsum < halfvar) {
            *lowervar = tmpsum;
        }
        else {
            if (l > 0) {
                hist_item* res = hist_item_sort_halfvar(base, l, lowervar, halfvar);
                if (res) return res;
            }
            else {
                // End of left recursion. This will be executed in order from the first element.
                *lowervar += base[0].color_weight;
                if (*lowervar > halfvar) return &base[0];
            }
        }

        if (len > r) {
            base += r; len -= r; // tail-recursive "call"
        }
        else {
            *lowervar += base[r].color_weight;
            return (*lowervar > halfvar) ? &base[r] : NULL;
        }
    } while (1);
}

static f_pixel get_median(const struct box* b, hist_item achv[]);


static int comparevariance(const void* ch1, const void* ch2)
{
    return ((const channelvariance*)ch1)->variance > ((const channelvariance*)ch2)->variance ? -1 :
        (((const channelvariance*)ch1)->variance < ((const channelvariance*)ch2)->variance ? 1 : 0);
}

/** Finds which channels need to be sorted first and preproceses achv for fast sort */
static double prepare_sort(struct box* b, hist_item achv[])
{
    /*
     ** Sort dimensions by their variance, and then sort colors first by dimension with highest variance
     */
    channelvariance channels[4] = {
        {index_of_channel(a), b->variance.a},
        {index_of_channel(r), b->variance.r},
        {index_of_channel(g), b->variance.g},
        {index_of_channel(b), b->variance.b},
    };

    qsort(channels, 4, sizeof(channels[0]), comparevariance);

    const unsigned int ind1 = b->ind;
    const unsigned int colors = b->colors;

    for (unsigned int i = 0; i < colors; i++) {
        const float* chans = (const float*)&achv[ind1 + i].acolor;
        // Only the first channel really matters. When trying median cut many times
        // with different histogram weights, I don't want sort randomness to influence outcome.
        achv[ind1 + i].tmp.sort_value = ((unsigned int)(chans[channels[0].chan] * 65535.0) << 16) |
            (unsigned int)((chans[channels[2].chan] + chans[channels[1].chan] / 2.0 + chans[channels[3].chan] / 4.0) * 65535.0);
    }

    const f_pixel median = get_median(b, achv);

    // box will be split to make color_weight of each side even
    const unsigned int ind = b->ind, end = ind + b->colors;
    double totalvar = 0;
    for (unsigned int j = ind; j < end; j++) totalvar += (achv[j].color_weight = color_weight(median, achv[j]));
    return totalvar / 2.0;
}

/** finds median in unsorted set by sorting only minimum required */
static f_pixel get_median(const struct box* b, hist_item achv[])
{
    const unsigned int median_start = (b->colors - 1) / 2;

    hist_item_sort_range(&(achv[b->ind]), b->colors,
        median_start);

    if (b->colors & 1) return achv[b->ind + median_start].acolor;

    // technically the second color is not guaranteed to be sorted correctly
    // but most of the time it is good enough to be useful
    return averagepixels(2, &achv[b->ind + median_start]);
}

/*
 ** Find the best splittable box. -1 if no boxes are splittable.
 */
static int best_splittable_box(struct box bv[], unsigned int boxes, const double max_mse)
{
    int bi = -1; double maxsum = 0;
    for (unsigned int i = 0; i < boxes; i++) {
        if (bv[i].colors < 2) {
            continue;
        }

        // looks only at max variance, because it's only going to split by it
        const double cv = MAX(bv[i].variance.r, MAX(bv[i].variance.g, bv[i].variance.b));
        double thissum = bv[i].sum * MAX(bv[i].variance.a, cv);

        if (bv[i].max_error > max_mse) {
            thissum = thissum * bv[i].max_error / max_mse;
        }

        if (thissum > maxsum) {
            maxsum = thissum;
            bi = i;
        }
    }
    return bi;
}

inline static double color_weight(f_pixel median, hist_item h)
{
    float diff = colordifference(median, h.acolor);
    return sqrt(diff) * (sqrt(1.0 + h.adjusted_weight) - 1.0);
}

static void set_colormap_from_boxes(colormap* map, struct box bv[], unsigned int boxes, hist_item* achv);
static void adjust_histogram(hist_item* achv, const struct box bv[], unsigned int boxes);

static double box_error(const struct box* box, const hist_item achv[])
{
    f_pixel avg = box->color;

    double total_error = 0;
    for (unsigned int i = 0; i < box->colors; ++i) {
        total_error += colordifference(avg, achv[box->ind + i].acolor) * achv[box->ind + i].perceptual_weight;
    }

    return total_error;
}


static bool total_box_error_below_target(double target_mse, struct box bv[], unsigned int boxes, const histogram* hist)
{
    target_mse *= hist->total_perceptual_weight;
    double total_error = 0;

    for (unsigned int i = 0; i < boxes; i++) {
        // error is (re)calculated lazily
        if (bv[i].total_error >= 0) {
            total_error += bv[i].total_error;
        }
        if (total_error > target_mse) return false;
    }

    for (unsigned int i = 0; i < boxes; i++) {
        if (bv[i].total_error < 0) {
            bv[i].total_error = box_error(&bv[i], hist->achv);
            total_error += bv[i].total_error;
        }
        if (total_error > target_mse) return false;
    }

    return true;
}

static void box_init(struct box* box, const hist_item* achv, const unsigned int ind, const unsigned int colors, const double sum) {
    box->ind = ind;
    box->colors = colors;
    box->sum = sum;
    box->total_error = -1;

    box->color = averagepixels(colors, &achv[ind]);
    box->variance = box_variance(achv, box);
    box->max_error = box_max_error(achv, box);
}

/*
 ** Here is the fun part, the median-cut colormap generator.  This is based
 ** on Paul Heckbert's paper, "Color Image Quantization for Frame Buffer
 ** Display," SIGGRAPH 1982 Proceedings, page 297.
 */
LIQ_PRIVATE colormap* mediancut(histogram* hist, unsigned int newcolors, const double target_mse, const double max_mse, void* (*malloc)(size_t), void (*free)(void*))
{
    hist_item* achv = hist->achv;
    LIQ_ARRAY(struct box, bv, newcolors);
    unsigned int boxes = 1;

    /*
     ** Set up the initial box.
     */
    {
        double sum = 0;
        for (unsigned int i = 0; i < hist->size; i++) {
            sum += achv[i].adjusted_weight;
        }
        box_init(&bv[0], achv, 0, hist->size, sum);


        /*
         ** Main loop: split boxes until we have enough.
         */
        while (boxes < newcolors) {

            // first splits boxes that exceed quality limit (to have colors for things like odd green pixel),
            // later raises the limit to allow large smooth areas/gradients get colors.
            const double current_max_mse = max_mse + (boxes / (double)newcolors) * 16.0 * max_mse;
            const int bi = best_splittable_box(bv, boxes, current_max_mse);
            if (bi < 0) {
                break;    /* ran out of colors! */
            }

            unsigned int indx = bv[bi].ind;
            unsigned int clrs = bv[bi].colors;

            /*
             Classic implementation tries to get even number of colors or pixels in each subdivision.

             Here, instead of popularity I use (sqrt(popularity)*variance) metric.
             Each subdivision balances number of pixels (popular colors) and low variance -
             boxes can be large if they have similar colors. Later boxes with high variance
             will be more likely to be split.

             Median used as expected value gives much better results than mean.
             */

            const double halfvar = prepare_sort(&bv[bi], achv);
            double lowervar = 0;

            // hist_item_sort_halfvar sorts and sums lowervar at the same time
            // returns item to break at …minus one, which does smell like an off-by-one error.
            hist_item* break_p = hist_item_sort_halfvar(&achv[indx], clrs, &lowervar, halfvar);
            unsigned int break_at = MIN(clrs - 1, break_p - &achv[indx] + 1);

            /*
             ** Split the box.
             */
            double sm = bv[bi].sum;
            double lowersum = 0;
            for (unsigned int i = 0; i < break_at; i++) lowersum += achv[indx + i].adjusted_weight;

            box_init(&bv[bi], achv, indx, break_at, lowersum);
            box_init(&bv[boxes], achv, indx + break_at, clrs - break_at, sm - lowersum);

            ++boxes;

            if (total_box_error_below_target(target_mse, bv, boxes, hist)) {
                break;
            }
        }
    }

    colormap* map = pam_colormap(boxes, malloc, free);
    set_colormap_from_boxes(map, bv, boxes, achv);

    adjust_histogram(achv, bv, boxes);

    return map;
}

static void set_colormap_from_boxes(colormap* map, struct box* bv, unsigned int boxes, hist_item* achv)
{
    /*
     ** Ok, we've got enough boxes.  Now choose a representative color for
     ** each box.  There are a number of possible ways to make this choice.
     ** One would be to choose the center of the box; this ignores any structure
     ** within the boxes.  Another method would be to average all the colors in
     ** the box - this is the method specified in Heckbert's paper.
     */

    for (unsigned int bi = 0; bi < boxes; ++bi) {
        map->palette[bi].acolor = bv[bi].color;

        /* store total color popularity (perceptual_weight is approximation of it) */
        map->palette[bi].popularity = 0;
        for (unsigned int i = bv[bi].ind; i < bv[bi].ind + bv[bi].colors; i++) {
            map->palette[bi].popularity += achv[i].perceptual_weight;
        }
    }
}

/* increase histogram popularity by difference from the final color (this is used as part of feedback loop) */
static void adjust_histogram(hist_item* achv, const struct box* bv, unsigned int boxes)
{
    for (unsigned int bi = 0; bi < boxes; ++bi) {
        for (unsigned int i = bv[bi].ind; i < bv[bi].ind + bv[bi].colors; i++) {
            achv[i].tmp.likely_colormap_index = bi;
        }
    }
}

static f_pixel averagepixels(unsigned int clrs, const hist_item achv[])
{
    double r = 0, g = 0, b = 0, a = 0, sum = 0;

    for (unsigned int i = 0; i < clrs; i++) {
        const f_pixel px = achv[i].acolor;
        const double weight = achv[i].adjusted_weight;

        sum += weight;
        a += px.a * weight;
        r += px.r * weight;
        g += px.g * weight;
        b += px.b * weight;
    }

    if (sum) {
        a /= sum;
        r /= sum;
        g /= sum;
        b /= sum;
    }

    assert(!isnan(r) && !isnan(g) && !isnan(b) && !isnan(a));

    return (f_pixel) { .r = r, .g = g, .b = b, .a = a };
}



#define ALIGN_MASK 15UL
#define MEMPOOL_RESERVED ((sizeof(struct mempool)+ALIGN_MASK) & ~ALIGN_MASK)

struct mempool {
    unsigned int used, size;
    void* (*malloc)(size_t);
    void (*free)(void*);
    struct mempool* next;
};
LIQ_PRIVATE void* mempool_create(mempoolptr* mptr, const unsigned int size, unsigned int max_size, void* (*malloc)(size_t), void (*free)(void*))
{
    if (*mptr && ((*mptr)->used + size) <= (*mptr)->size) {
        unsigned int prevused = (*mptr)->used;
        (*mptr)->used += (size + 15UL) & ~0xFUL;
        return ((char*)(*mptr)) + prevused;
    }

    mempoolptr old = *mptr;
    if (!max_size) max_size = (1 << 17);
    max_size = size + ALIGN_MASK > max_size ? size + ALIGN_MASK : max_size;

    *mptr = malloc(MEMPOOL_RESERVED + max_size);
    if (!*mptr) return NULL;
    **mptr = (struct mempool){
        .malloc = malloc,
        .free = free,
        .size = MEMPOOL_RESERVED + max_size,
        .used = sizeof(struct mempool),
        .next = old,
    };
    uintptr_t mptr_used_start = (uintptr_t)(*mptr) + (*mptr)->used;
    (*mptr)->used += (ALIGN_MASK + 1 - (mptr_used_start & ALIGN_MASK)) & ALIGN_MASK; // reserve bytes required to make subsequent allocations aligned
    assert(!(((uintptr_t)(*mptr) + (*mptr)->used) & ALIGN_MASK));

    return mempool_alloc(mptr, size, size);
}

LIQ_PRIVATE void* mempool_alloc(mempoolptr* mptr, const unsigned int size, const unsigned int max_size)
{
    if (((*mptr)->used + size) <= (*mptr)->size) {
        unsigned int prevused = (*mptr)->used;
        (*mptr)->used += (size + ALIGN_MASK) & ~ALIGN_MASK;
        return ((char*)(*mptr)) + prevused;
    }

    return mempool_create(mptr, size, max_size, (*mptr)->malloc, (*mptr)->free);
}

LIQ_PRIVATE void mempool_destroy(mempoolptr m)
{
    while (m) {
        mempoolptr next = m->next;
        m->free(m);
        m = next;
    }
}
