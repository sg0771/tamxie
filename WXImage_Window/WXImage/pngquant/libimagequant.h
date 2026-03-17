/*
 * https://pngquant.org
 */

#ifndef LIBIMAGEQUANT_H
#define LIBIMAGEQUANT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <setjmp.h>

    typedef struct liq_attr liq_attr;
    typedef struct liq_image liq_image;
    typedef struct liq_result liq_result;
    typedef struct liq_histogram liq_histogram;

    typedef struct liq_color {
        unsigned char r, g, b, a;
    } liq_color;

    typedef struct liq_palette {
        unsigned int count;
        liq_color entries[256];
    } liq_palette;

    typedef enum liq_error {
        LIQ_OK = 0,
        LIQ_QUALITY_TOO_LOW = 99,
        LIQ_VALUE_OUT_OF_RANGE = 100,
        LIQ_OUT_OF_MEMORY,
        LIQ_ABORTED,
        LIQ_BITMAP_NOT_AVAILABLE,
        LIQ_BUFFER_TOO_SMALL,
        LIQ_INVALID_POINTER,
        LIQ_UNSUPPORTED,
    } liq_error;

    enum liq_ownership {
        LIQ_OWN_ROWS = 4,
        LIQ_OWN_PIXELS = 8,
        LIQ_COPY_PIXELS = 16,
    };

    typedef struct liq_histogram_entry {
        liq_color color;
        unsigned int count;
    } liq_histogram_entry;

    liq_attr* liq_attr_create(void);
    liq_attr* liq_attr_create_with_allocator(void* (*malloc)(size_t), void (*free)(void*));
    liq_attr* liq_attr_copy(const liq_attr* orig);
    void liq_attr_destroy(liq_attr* attr);

    liq_histogram* liq_histogram_create(const liq_attr* attr);
    liq_error liq_histogram_add_image(liq_histogram* hist, const liq_attr* attr, liq_image* image);
    liq_error liq_histogram_add_colors(liq_histogram* hist, const liq_attr* attr, const liq_histogram_entry entries[], int num_entries, double gamma);
    liq_error liq_histogram_add_fixed_color(liq_histogram* hist, liq_color color, double gamma);
    void liq_histogram_destroy(liq_histogram* hist);

    liq_error liq_set_max_colors(liq_attr* attr, int colors);
    int liq_get_max_colors(const liq_attr* attr);
    liq_error liq_set_speed(liq_attr* attr, int speed);
    int liq_get_speed(const liq_attr* attr);
    liq_error liq_set_min_opacity(liq_attr* attr, int min);
    int liq_get_min_opacity(const liq_attr* attr);
    liq_error liq_set_min_posterization(liq_attr* attr, int bits);
    int liq_get_min_posterization(const liq_attr* attr);
    liq_error liq_set_quality(liq_attr* attr, int minimum, int maximum);
    int liq_get_min_quality(const liq_attr* attr);
    int liq_get_max_quality(const liq_attr* attr);
    void liq_set_last_index_transparent(liq_attr* attr, int is_last);

    typedef void liq_log_callback_function(const liq_attr*, const char* message, void* user_info);
    typedef void liq_log_flush_callback_function(const liq_attr*, void* user_info);
    void liq_set_log_callback(liq_attr*, liq_log_callback_function*, void* user_info);
    void liq_set_log_flush_callback(liq_attr*, liq_log_flush_callback_function*, void* user_info);

    typedef int liq_progress_callback_function(float progress_percent, void* user_info);
    void liq_attr_set_progress_callback(liq_attr*, liq_progress_callback_function*, void* user_info);
    void liq_result_set_progress_callback(liq_result*, liq_progress_callback_function*, void* user_info);

    // The rows and their data are not modified. The type of `rows` is non-const only due to a bug in C's typesystem design.
    liq_image* liq_image_create_rgba_rows(const liq_attr* attr, void* const rows[], int width, int height, double gamma);
    liq_image* liq_image_create_rgba(const liq_attr* attr, const void* bitmap, int width, int height, double gamma);


    typedef void liq_image_get_rgba_row_callback(liq_color row_out[], int row, int width, void* user_info);
    liq_image* liq_image_create_custom(const liq_attr* attr, liq_image_get_rgba_row_callback* row_callback, void* user_info, int width, int height, double gamma);

    liq_error liq_image_set_memory_ownership(liq_image* image, int ownership_flags);
    liq_error liq_image_set_background(liq_image* img, liq_image* background_image);
    liq_error liq_image_add_fixed_color(liq_image* img, liq_color color);
    int liq_image_get_width(const liq_image* img);
    int liq_image_get_height(const liq_image* img);
    void liq_image_destroy(liq_image* img);

    liq_error liq_histogram_quantize(liq_histogram* const input_hist, liq_attr* const options, liq_result** result_output);
    liq_error liq_image_quantize(liq_image* const input_image, liq_attr* const options, liq_result** result_output);

    liq_error liq_set_dithering_level(liq_result* res, float dither_level);
    liq_error liq_set_output_gamma(liq_result* res, double gamma);
    double liq_get_output_gamma(const liq_result* result);

    const liq_palette* liq_get_palette(liq_result* result);

    liq_error liq_write_remapped_image(liq_result* result, liq_image* input_image, void* buffer, size_t buffer_size);
    liq_error liq_write_remapped_image_rows(liq_result* result, liq_image* input_image, unsigned char** row_pointers);

    double liq_get_quantization_error(const liq_result* result);
    int liq_get_quantization_quality(const liq_result* result);
    double liq_get_remapping_error(const liq_result* result);
    int liq_get_remapping_quality(const liq_result* result);
    void liq_result_destroy(liq_result*);
    int liq_version(void);

	typedef enum {
		SUCCESS = 0,
		MISSING_ARGUMENT = 1,
		READ_ERROR = 2,
		INVALID_ARGUMENT = 4,
		NOT_OVERWRITING_ERROR = 15,
		CANT_WRITE_ERROR = 16,
		OUT_OF_MEMORY_ERROR = 17,
		WRONG_ARCHITECTURE = 18, // Missing SSE
		PNG_OUT_OF_MEMORY_ERROR = 24,
		LIBPNG_FATAL_ERROR = 25,
		WRONG_INPUT_COLOR_TYPE = 26,
		LIBPNG_INIT_ERROR = 35,
		TOO_LARGE_FILE = 98,
		TOO_LOW_QUALITY = 99,
	} pngquant_error;

	typedef struct rwpng_rgba {
		unsigned char r, g, b, a;
	} rwpng_rgba;

	struct rwpng_chunk {
		struct rwpng_chunk* next;
		unsigned char* data;
		size_t size;
		unsigned char name[5];
		unsigned char location;
	};

	typedef enum {
		RWPNG_NONE,
		RWPNG_SRGB, // sRGB chunk
		RWPNG_ICCP, // used ICC profile
		RWPNG_ICCP_WARN_GRAY, // ignore and warn about GRAY ICC profile
		RWPNG_GAMA_CHRM, // used gAMA and cHRM
		RWPNG_GAMA_ONLY, // used gAMA only (i.e. not sRGB)
		RWPNG_COCOA, // Colors handled by Cocoa reader
	} rwpng_color_transform;

	typedef struct {
		jmp_buf jmpbuf;
		uint32_t width;
		uint32_t height;
		double gamma;
		unsigned char** row_pointers;
		unsigned char* indexed_data;
		unsigned int num_palette;
		rwpng_rgba m_palette[256];
		rwpng_color_transform output_color;
		int res_x;
		int res_y;
	} png8_image;

	static void rwpng_free_image8(png8_image* image) {
		if (image->indexed_data) {
			free(image->indexed_data);
			image->indexed_data = NULL;
		}
		if (image->row_pointers) {
			free(image->row_pointers);
			image->row_pointers = NULL;
		}

	}

	//设置输出调色板
	static void rwpng_set_palette(liq_result* result, png8_image* output_image) {
		const liq_palette* palette = liq_get_palette(result);
		output_image->num_palette = palette->count;
		for (unsigned int i = 0; i < palette->count; i++) {
			const liq_color px = palette->entries[i];
			output_image->m_palette[i].r = px.r;
			output_image->m_palette[i].g = px.g;
			output_image->m_palette[i].b = px.b;
			output_image->m_palette[i].a = px.a;
		}
	}

	//预备
	static pngquant_error prepare_output_image(liq_result* result, liq_image* input_image, rwpng_color_transform output_color, png8_image* output_image)
	{
		output_image->width = liq_image_get_width(input_image);
		output_image->height = liq_image_get_height(input_image);
		output_image->gamma = liq_get_output_gamma(result);
		output_image->output_color = output_color;
		output_image->indexed_data = (uint8_t*)malloc((size_t)output_image->height * (size_t)output_image->width);
		output_image->row_pointers = (uint8_t**)malloc((size_t)output_image->height * sizeof(output_image->row_pointers[0]));

		if (!output_image->indexed_data || !output_image->row_pointers) {
			return OUT_OF_MEMORY_ERROR;
		}

		for (size_t row = 0; row < output_image->height; row++) {
			output_image->row_pointers[row] = output_image->indexed_data +
				row * output_image->width;
		}

		const liq_palette* palette = liq_get_palette(result);
		output_image->num_palette = palette->count;
		return SUCCESS;
	}


#ifdef __cplusplus
}
#endif

#endif
