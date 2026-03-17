/*
 * bitset.c
 * Plain old bitset data type.
 *
 * Copyright (C) 2001-2017 Cosmin Truta.
 *
 * This software is distributed under the zlib license.
 * Please see the accompanying LICENSE file.
 */
#include <ctype.h>
#include <errno.h>
#include <stddef.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>

#include "../WXImage.h" //API

#include "../zlib/zlib.h"
#include "../libpng/libpng.h"

#include <limits.h>
#include <stddef.h>
typedef unsigned int opng_bitset_t;

#define OPNG_BITSET_EMPTY 0U
#define OPNG_BITSET_FULL (~0U)

#define OPNG_BITSIZEOF(object) (sizeof(object) * CHAR_BIT)

enum
{
    OPNG_BITSET_ELT_MIN = 0,
    OPNG_BITSET_ELT_MAX = (int)(OPNG_BITSIZEOF(opng_bitset_t) - 1)
};


inline int
opng_bitset_test(opng_bitset_t set, int elt)
{
    return (set & (1U << elt)) != 0;
}

inline void
opng_bitset_set(opng_bitset_t* set, int elt)
{
    *set |= (1U << elt);
}

inline void
opng_bitset_reset(opng_bitset_t* set, int elt)
{
    *set &= ~(1U << elt);
}

inline void
opng_bitset_flip(opng_bitset_t* set, int elt)
{
    *set ^= (1U << elt);
}

inline opng_bitset_t
opng_bitset__range__(int start_elt, int stop_elt)
{
    return ((1U << (stop_elt - start_elt) << 1) - 1) << start_elt;
}

inline int
opng_bitset_test_all_in_range(opng_bitset_t set, int start_elt, int stop_elt)
{
    return (start_elt <= stop_elt) ?
        ((~set & opng_bitset__range__(start_elt, stop_elt)) == 0) :
        1;
}

inline int
opng_bitset_test_any_in_range(opng_bitset_t set, int start_elt, int stop_elt)
{
    return (start_elt <= stop_elt) ?
        ((set & opng_bitset__range__(start_elt, stop_elt)) != 0) :
        0;
}

inline void
opng_bitset_set_range(opng_bitset_t* set, int start_elt, int stop_elt)
{
    if (start_elt <= stop_elt)
        *set |= opng_bitset__range__(start_elt, stop_elt);
}

inline void
opng_bitset_reset_range(opng_bitset_t* set, int start_elt, int stop_elt)
{
    if (start_elt <= stop_elt)
        *set &= ~opng_bitset__range__(start_elt, stop_elt);
}

inline void
opng_bitset_flip_range(opng_bitset_t* set, int start_elt, int stop_elt)
{
    if (start_elt <= stop_elt)
        *set ^= opng_bitset__range__(start_elt, stop_elt);
}

unsigned int
opng_bitset_count(opng_bitset_t set);

int
opng_bitset_find_first(opng_bitset_t set);

int
opng_bitset_find_next(opng_bitset_t set, int elt);

int
opng_bitset_find_last(opng_bitset_t set);

int
opng_bitset_find_prev(opng_bitset_t set, int elt);

int
opng_strparse_rangeset_to_bitset(opng_bitset_t* out_set,
    const char* rangeset_str,
    opng_bitset_t mask_set);

size_t
opng_strformat_bitset_as_rangeset(char* out_buf,
    size_t out_buf_size,
    opng_bitset_t bitset);



#include <stdio.h>

#include <limits.h>
#if (LONG_MAX > 0x7fffffffL) || (LONG_MAX > INT_MAX)

typedef long opng_foffset_t;
#define OPNG_FOFFSET_MIN LONG_MIN
#define OPNG_FOFFSET_MAX LONG_MAX
#define OPNG_FOFFSET_SCNd "ld"
#define OPNG_FOFFSET_SCNx "lx"
#define OPNG_FOFFSET_PRId "ld"
#define OPNG_FOFFSET_PRIx "lx"
#define OPNG_FOFFSET_PRIX "lX"

typedef unsigned long opng_fsize_t;
#define OPNG_FSIZE_MAX ULONG_MAX
#define OPNG_FSIZE_SCNu "lu"
#define OPNG_FSIZE_SCNx "lx"
#define OPNG_FSIZE_PRIu "lu"
#define OPNG_FSIZE_PRIx "lx"
#define OPNG_FSIZE_PRIX "lX"

#elif defined _I64_MAX && (defined _WIN32 || defined __WIN32__)

typedef __int64 opng_foffset_t;
#define OPNG_FOFFSET_MIN _I64_MIN
#define OPNG_FOFFSET_MAX _I64_MAX
#define OPNG_FOFFSET_SCNd "I64d"
#define OPNG_FOFFSET_SCNx "I64x"
#define OPNG_FOFFSET_PRId "I64d"
#define OPNG_FOFFSET_PRIx "I64x"
#define OPNG_FOFFSET_PRIX "I64X"

typedef unsigned __int64 opng_fsize_t;
#define OPNG_FSIZE_MAX _UI64_MAX
#define OPNG_FSIZE_SCNu "I64u"
#define OPNG_FSIZE_SCNx "I64x"
#define OPNG_FSIZE_PRIu "I64u"
#define OPNG_FSIZE_PRIx "I64x"
#define OPNG_FSIZE_PRIX "I64X"

#else

#include <inttypes.h>
#include <stdint.h>

typedef int_least64_t opng_foffset_t;
#define OPNG_FOFFSET_MIN INT_LEAST64_MIN
#define OPNG_FOFFSET_MAX INT_LEAST64_MAX
#define OPNG_FOFFSET_SCNd SCNdLEAST64
#define OPNG_FOFFSET_SCNx SCNxLEAST64
#define OPNG_FOFFSET_PRId PRIdLEAST64
#define OPNG_FOFFSET_PRIx PRIxLEAST64
#define OPNG_FOFFSET_PRIX PRIXLEAST64

typedef uint_least64_t opng_fsize_t;
#define OPNG_FSIZE_MAX UINT_LEAST64_MAX
#define OPNG_FSIZE_SCNu SCNuLEAST64
#define OPNG_FSIZE_SCNx SCNxLEAST64
#define OPNG_FSIZE_PRIu PRIuLEAST64
#define OPNG_FSIZE_PRIx PRIxLEAST64
#define OPNG_FSIZE_PRIX PRIXLEAST64

#endif

opng_foffset_t
opng_ftello(FILE* stream);

int
opng_fseeko(FILE* stream, opng_foffset_t offset, int whence);

size_t
opng_freado(FILE* stream, opng_foffset_t offset, int whence,
    void* block, size_t blocksize);

size_t
opng_fwriteo(FILE* stream, opng_foffset_t offset, int whence,
    const void* block, size_t blocksize);

int
opng_fgetsize(FILE* stream, opng_fsize_t* size);

char*
opng_path_replace_dir(char* buffer, size_t bufsize,
    const char* old_path, const char* new_dirname);

char*
opng_path_replace_ext(char* buffer, size_t bufsize,
    const char* old_path, const char* new_extname);


int opng_os_rename(const char* src_path, const char* dest_path, int clobber);

int
opng_os_create_dir(const char* dirname);

int
opng_os_test(const char* path, const char* mode);

int
opng_os_test_eq(const char* path1, const char* path2);

int
opng_os_unlink(const char* path);





#ifdef PNG_INFO_IMAGE_SUPPORTED

/*
 * Check if the image information is valid.
 * The image information is said to be valid if all the required
 * critical chunk data is present in the png structures.
 * The function returns 1 if this information is valid, and 0 otherwise.
 */
int PNGAPI opng_validate_image(png_structp png_ptr, png_infop info_ptr);

#endif /* PNG_INFO_IMAGE_SUPPORTED */


#ifndef OPNG_NO_IMAGE_REDUCTIONS
#define OPNG_IMAGE_REDUCTIONS_SUPPORTED
#endif

#ifdef OPNG_IMAGE_REDUCTIONS_SUPPORTED

#ifndef PNG_INFO_IMAGE_SUPPORTED
#error OPNG_IMAGE_REDUCTIONS_SUPPORTED requires PNG_INFO_IMAGE_SUPPORTED
#endif

#ifndef PNG_tRNS_SUPPORTED
#error OPNG_IMAGE_REDUCTIONS_SUPPORTED requires proper transparency support
#endif

/*
 * Reduce the image (bit depth + color type + palette) without
 * losing any information. The palette (if applicable) and the
 * image data must be present, e.g., by calling png_set_rows(),
 * or by loading IDAT.
 * The parameter reductions indicates the intended reductions.
 * The function returns the successful reductions.
 */
png_uint_32 PNGAPI opng_reduce_image(png_structp png_ptr, png_infop info_ptr,
    png_uint_32 reductions);

/*
 * PNG reduction flags.
 */
#define OPNG_REDUCE_NONE             0x0000
#define OPNG_REDUCE_16_TO_8          0x0001  /* discard bits 8-15 */
#define OPNG_REDUCE_8_TO_4_2_1       0x0002  /* discard bits 4-7, 2-7 or 1-7 */
#define OPNG_REDUCE_RGB_TO_GRAY      0x0004  /* ...also RGBA to GA */
#define OPNG_REDUCE_STRIP_ALPHA      0x0008  /* ...and create tRNS if needed */
#define OPNG_REDUCE_RGB_TO_PALETTE   0x0010  /* ...also RGBA to palette/tRNS */
#define OPNG_REDUCE_PALETTE_TO_RGB   0x0020  /* TODO */
#define OPNG_REDUCE_GRAY_TO_PALETTE  0x0040  /* ...also GA to palette/tRNS */
#define OPNG_REDUCE_PALETTE_TO_GRAY  0x0080  /* ...also palette/tRNS to GA */
#define OPNG_REDUCE_PALETTE_SLOW     0x0100  /* TODO: remove all sterile entries
                                                and reorder PLTE */
#define OPNG_REDUCE_PALETTE_FAST     0x0200  /* remove trailing sterile entries
                                                only; do not reorder PLTE */
#define OPNG_REDUCE_METADATA         0x1000  /* TODO */
#define OPNG_REDUCE_REPAIR           0x2000  /* repair broken image data */

#define OPNG_REDUCE_BIT_DEPTH  \
   (OPNG_REDUCE_16_TO_8 | OPNG_REDUCE_8_TO_4_2_1)

#define OPNG_REDUCE_COLOR_TYPE  \
   (OPNG_REDUCE_RGB_TO_GRAY | OPNG_REDUCE_STRIP_ALPHA | \
    OPNG_REDUCE_RGB_TO_PALETTE | OPNG_REDUCE_PALETTE_TO_RGB | \
    OPNG_REDUCE_GRAY_TO_PALETTE | OPNG_REDUCE_PALETTE_TO_GRAY)

#define OPNG_REDUCE_PALETTE  \
   (OPNG_REDUCE_PALETTE_SLOW | OPNG_REDUCE_PALETTE_FAST)

#define OPNG_REDUCE_ALL  \
   (OPNG_REDUCE_BIT_DEPTH | OPNG_REDUCE_COLOR_TYPE | \
    OPNG_REDUCE_PALETTE | OPNG_REDUCE_METADATA)

#endif /* OPNG_IMAGE_REDUCTIONS_SUPPORTED */

#define PROGRAM_NAME \
    "OptiPNG"
#define PROGRAM_SUMMARY \
    "Portable Network Graphics optimizer"
#define PROGRAM_VERSION \
    "0.7.7"
#define PROGRAM_COPYRIGHT \
    "Copyright (C) 2001-2017 Cosmin Truta and the Contributing Authors"
#define PROGRAM_URI \
    "http://optipng.sourceforge.net/"



#include <stddef.h>


#ifndef OPNG_LLONG_T_DEFINED

#include <limits.h>

#if defined LLONG_MAX && defined ULLONG_MAX
#if (LLONG_MAX >= LONG_MAX) && (ULLONG_MAX >= ULONG_MAX)
typedef long long opng_llong_t;
typedef unsigned long long opng_ullong_t;
#define OPNG_LLONG_MIN LLONG_MIN
#define OPNG_LLONG_MAX LLONG_MAX
#define OPNG_ULLONG_MAX ULLONG_MAX
#define OPNG_LLONG_C(value) value##LL
#define OPNG_ULLONG_C(value) value##ULL
#define OPNG_LLONG_T_DEFINED 1
#endif
#elif defined _I64_MAX && defined _UI64_MAX
#if defined _WIN32 || defined __WIN32__
typedef __int64 opng_llong_t;
typedef unsigned __int64 opng_ullong_t;
#define OPNG_LLONG_MIN _I64_MIN
#define OPNG_LLONG_MAX _I64_MAX
#define OPNG_ULLONG_MAX _UI64_MAX
#define OPNG_LLONG_C(value) value##i64
#define OPNG_ULLONG_C(value) value##ui64
#define OPNG_LLONG_T_DEFINED 1
#endif
#endif

#ifdef OPNG_LLONG_T_DEFINED
#if defined _WIN32 || defined __WIN32__
/* The "ll" format modifier may not work on Windows XP and earlier. */
#define OPNG_LLONG_FORMAT_PREFIX "I64"
#else
#define OPNG_LLONG_FORMAT_PREFIX "ll"
#endif
#endif

#endif  /* OPNG_LLONG_T_DEFINED */


struct opng_lratio
{
    long num;
    long denom;
};

struct opng_ulratio
{
    unsigned long num;
    unsigned long denom;
};

#ifdef OPNG_LLONG_T_DEFINED

/*
 * The long long rational type.
 */
struct opng_llratio
{
    opng_llong_t num;
    opng_llong_t denom;
};

/*
 * The unsigned long long rational type.
 */
struct opng_ullratio
{
    opng_ullong_t num;
    opng_ullong_t denom;
};

#endif  /* OPNG_LLONG_T_DEFINED */


/*
 * Converts a rational value to a compact factor string representation.
 * Examples: 34/55 -> "61.82%", 55/34 -> "1.62x".
 *
 * The factor string has the following format:
 *
 *   "DD.DD%"    if ratio < 99.995%
 *   "DD.DDx"    if ratio >= 99.995% and ratio < 99.995
 *   "DDDx"      if ratio >= 99.995
 *   "??%"       if ratio == 0/0
 *   "INFINITY%" if ratio >= 1/0
 *
 * The buffer shall contain the output string, or a part of it if the
 * buffer size is too small, always null-terminated.
 * The function shall return the number of characters stored, not including
 * the null-character terminator, or -1 if the buffer size is too small.
 */
int
opng_ulratio_to_factor_string(char* buffer, size_t buffer_size,
    const struct opng_ulratio* ratio);

/*
 * Converts a rational value to a compact percent string representation.
 * Examples: 34/55 -> "61.82%", 55/34 -> "162%".
 *
 * This is the format "DD.DD%" for ratios below 99.995%, and the format
 * "DDD%" for ratios equal to or above 99.995%.
 *
 * The buffer shall contain the output string, or a part of it if the
 * buffer size is too small, always null-terminated.
 * The function shall return the number of characters stored, not including
 * the null-character terminator, or -1 if the buffer size is too small.
 */
int
opng_ulratio_to_percent_string(char* buffer, size_t buffer_size,
    const struct opng_ulratio* ratio);

#ifdef OPNG_LLONG_T_DEFINED

/*
 * Converts a rational value to a compact factor string representation.
 * See opng_ulratio_to_factor_string.
 */
int
opng_ullratio_to_factor_string(char* buffer, size_t buffer_size,
    const struct opng_ullratio* ratio);

/*
 * Converts a rational value to a compact percent string representation.
 * See opng_ulratio_to_percent_string.
 */
int
opng_ullratio_to_percent_string(char* buffer, size_t buffer_size,
    const struct opng_ullratio* ratio);

#endif  /* OPNG_LLONG_T_DEFINED */





/* Store data into the info structure. */
void PNGAPI pngx_set_compression_type
(png_structp png_ptr, png_infop info_ptr, int compression_type);
void PNGAPI pngx_set_filter_type
(png_structp png_ptr, png_infop info_ptr, int filter_type);
void PNGAPI pngx_set_interlace_type
(png_structp png_ptr, png_infop info_ptr, int interlace_type);


#if PNG_LIBPNG_VER >= 10400
typedef png_alloc_size_t pngx_alloc_size_t;
#else
/* Compatibility backport of png_alloc_size_t */
typedef png_uint_32 pngx_alloc_size_t;
#endif

#ifdef PNG_INFO_IMAGE_SUPPORTED
/* Allocate memory for the row pointers.
 * Use filler to initialize the rows if it is non-negative.
 * On success return the newly-allocated row pointers.
 * On failure issue a png_error() or return NULL,
 * depending on the status of PNG_FLAG_MALLOC_NULL_MEM_OK.
 */
png_bytepp PNGAPI pngx_malloc_rows
(png_structp png_ptr, png_infop info_ptr, int filler);
png_bytepp PNGAPI pngx_malloc_rows_extended
(png_structp png_ptr, png_infop info_ptr,
    pngx_alloc_size_t min_row_size, int filler);
#endif


/*
 * I/O states were introduced in libpng-1.4.0, but they can be reliably used
 * starting with libpng-1.4.5 only.
 */
#if PNG_LIBPNG_VER >= 10405

#ifndef PNG_IO_STATE_SUPPORTED
#error This module requires libpng with PNG_IO_STATE_SUPPORTED
#endif

#define pngx_get_io_state       png_get_io_state
#define pngx_get_io_chunk_name  png_get_io_chunk_name
#define pngx_set_read_fn        png_set_read_fn
#define pngx_set_write_fn       png_set_write_fn
#define pngx_write_sig          png_write_sig

#define PNGX_IO_NONE            PNG_IO_NONE
#define PNGX_IO_READING         PNG_IO_READING
#define PNGX_IO_WRITING         PNG_IO_WRITING
#define PNGX_IO_SIGNATURE       PNG_IO_SIGNATURE
#define PNGX_IO_CHUNK_HDR       PNG_IO_CHUNK_HDR
#define PNGX_IO_CHUNK_DATA      PNG_IO_CHUNK_DATA
#define PNGX_IO_CHUNK_CRC       PNG_IO_CHUNK_CRC
#define PNGX_IO_MASK_OP         PNG_IO_MASK_OP
#define PNGX_IO_MASK_LOC        PNG_IO_MASK_LOC

#else /* PNG_LIBPNG_VER < 10405 */

 /* Compatibility backports of functions added to libpng 1.4 */
png_uint_32 PNGAPI pngx_get_io_state(png_structp png_ptr);
png_bytep PNGAPI pngx_get_io_chunk_name(png_structp png_ptr);
/* Note: although these backports have several limitations in comparison
 * to the actual libpng 1.4 functions, they work properly in OptiPNG,
 * as long as that they are used in conjunction with the wrappers below.
 */

 /* Compatibility wrappers for old libpng functions */
void PNGAPI pngx_set_read_fn
(png_structp png_ptr, png_voidp io_ptr, png_rw_ptr read_data_fn);
void PNGAPI pngx_set_write_fn
(png_structp png_ptr, png_voidp io_ptr, png_rw_ptr write_data_fn,
    png_flush_ptr output_flush_fn);
void PNGAPI pngx_write_sig(png_structp png_ptr);

/* Flags returned by png_get_io_state() */
#define PNGX_IO_NONE        0x0000  /* no I/O at this moment */
#define PNGX_IO_READING     0x0001  /* currently reading */
#define PNGX_IO_WRITING     0x0002  /* currently writing */
#define PNGX_IO_SIGNATURE   0x0010  /* currently at the file signature */
#define PNGX_IO_CHUNK_HDR   0x0020  /* currently at the chunk header */
#define PNGX_IO_CHUNK_DATA  0x0040  /* currently at the chunk data */
#define PNGX_IO_CHUNK_CRC   0x0080  /* currently at the chunk crc */
#define PNGX_IO_MASK_OP     0x000f  /* current operation: reading/writing */
#define PNGX_IO_MASK_LOC    0x00f0  /* current location: sig/hdr/data/crc */

#endif



#define opng__MIN__(a, b) \
    ((a) < (b) ? (a) : (b))

/*
 * Returns the maximum of two given values.
 */
#define opng__MAX__(a, b) \
    ((a) > (b) ? (a) : (b))

 /*
  * Spans the given pointer past the elements that satisfy the given predicate.
  * E.g., opng__SPAN__(str, isspace) moves str past the leading whitespace.
  */
#define opng__SPAN__(ptr, predicate) \
    { \
        while ((predicate)(*(ptr))) \
            ++(ptr); \
    }


  /*
   * Counts the number of elements in a bitset.
   */
unsigned int
opng_bitset_count(opng_bitset_t set)
{
    unsigned int result;

    /* Apply Wegner's method. */
    result = 0;
    while (set != 0)
    {
        set &= (set - 1);
        ++result;
    }
    return result;
}

/*
 * Finds the first element in a bitset.
 */
int
opng_bitset_find_first(opng_bitset_t set)
{
    int i;

    for (i = 0; i <= OPNG_BITSET_ELT_MAX; ++i)
    {
        if (opng_bitset_test(set, i))
            return i;
    }
    return -1;
}

/*
 * Finds the next element in a bitset.
 */
int
opng_bitset_find_next(opng_bitset_t set, int elt)
{
    int i;

    for (i = opng__MAX__(elt, -1) + 1; i <= OPNG_BITSET_ELT_MAX; ++i)
    {
        if (opng_bitset_test(set, i))
            return i;
    }
    return -1;
}

/*
 * Finds the last element in a bitset.
 */
int
opng_bitset_find_last(opng_bitset_t set)
{
    int i;

    for (i = OPNG_BITSET_ELT_MAX; i >= 0; --i)
    {
        if (opng_bitset_test(set, i))
            return i;
    }
    return -1;
}

/*
 * Finds the previous element in a bitset.
 */
int
opng_bitset_find_prev(opng_bitset_t set, int elt)
{
    int i;

    for (i = opng__MIN__(elt, OPNG_BITSET_ELT_MAX + 1) - 1; i >= 0; --i)
    {
        if (opng_bitset_test(set, i))
            return i;
    }
    return -1;
}

/*
 * Parses a rangeset string and converts the result to a bitset.
 */
int
opng_strparse_rangeset_to_bitset(opng_bitset_t* out_set,
    const char* rangeset_str,
    opng_bitset_t mask_set)
{
    opng_bitset_t result;
    const char* ptr;
    int state;
    int num, num1, num2;
    int err_invalid, err_range;

    result = OPNG_BITSET_EMPTY;
    ptr = rangeset_str;
    state = 0;
    err_invalid = err_range = 0;
    num1 = num2 = -1;

    for (; ; )
    {
        opng__SPAN__(ptr, isspace);
        switch (state)
        {
        case 0:  /* "" */
        case 2:  /* "N-" */
            /* Expecting number; go to next state. */
            if (*ptr >= '0' && *ptr <= '9')
            {
                num = 0;
                do
                {
                    num = 10 * num + (*ptr - '0');
                    if (num > OPNG_BITSET_ELT_MAX)
                    {
                        num = OPNG_BITSET_ELT_MAX;
                        err_range = 1;
                    }
                    ++ptr;
                } while (*ptr >= '0' && *ptr <= '9');
                if (!opng_bitset_test(mask_set, num))
                    err_range = 1;
                if (state == 0)
                    num1 = num;
                num2 = num;
                ++state;
                continue;
            }
            break;
        case 1:  /* "N" */
            /* Expecting range operator; go to next state. */
            if (*ptr == '-')
            {
                ++ptr;
                num2 = OPNG_BITSET_ELT_MAX;
                ++state;
                continue;
            }
            break;
        }

        if (state > 0)  /* "N", "N-" or "N-N" */
        {
            /* Store the partial result; go to state 0. */
            if (num1 <= num2)
            {
                opng_bitset_set_range(&result, num1, num2);
                result &= mask_set;
            }
            else
            {
                /* Incorrect range operands. */
                err_range = 1;
            }
            state = 0;
        }

        if (*ptr == ',' || *ptr == ';')
        {
            /* Separator: continue the loop. */
            ++ptr;
            continue;
        }
        else if (*ptr == '-')
        {
            /* Unexpected range operator: invalidate and exit the loop. */
            err_invalid = 1;
            break;
        }
        else
        {
            /* Unexpected character or end of string: exit the loop. */
            break;
        }
    }

    opng__SPAN__(ptr, isspace);
    if (*ptr != '\0')
    {
        /* Unexpected trailing character: invalidate. */
        err_invalid = 1;
    }

    if (err_invalid)
    {
        /* Invalid input error. */
#ifdef EINVAL
        errno = EINVAL;
#endif
        * out_set = OPNG_BITSET_EMPTY;
        return -1;
    }
    else if (err_range)
    {
        /* Range error. */
#ifdef ERANGE
        errno = ERANGE;
#endif
        * out_set = OPNG_BITSET_FULL;
        return -1;
    }
    else
    {
        /* Success. */
        *out_set = result;
        return 0;
    }
}

/*
 * Formats a bitset using the rangeset string representation.
 */
size_t
opng_strformat_bitset_as_rangeset(char* out_buf,
    size_t out_buf_size,
    opng_bitset_t bitset);
/* TODO */



#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#ifdef OPNG_LLONG_T_DEFINED
typedef opng_llong_t opng_xlong_impl_t;
typedef opng_ullong_t opng_uxlong_impl_t;
#define OPNG_XLONG_IMPL_FORMAT_PREFIX OPNG_LLONG_FORMAT_PREFIX
#else
typedef long opng_xlong_impl_t;
typedef unsigned long opng_uxlong_impl_t;
#define OPNG_XLONG_IMPL_FORMAT_PREFIX "l"
#endif


/*
 * Writes formatted output to a memory buffer.
 * This is a wrapper to [v]snprintf which avoids well-known defects
 * occurring in some of the underlying snprintf implementations.
 * The function returns the number of characters written, excluding the
 * null-termination character, if the buffer size is large enough, or -1
 * otherwise. (Unlike the proper snprintf, this function does not return
 * a number larger than zero if the buffer size is too small.)
 */
static int opng_snprintf_impl(char* buffer, size_t buffer_size, const char* format, ...)
{

#if defined _WIN32 || defined __WIN32__ || defined _WIN64 || defined __WIN64__
#define OPNG_VSNPRINTF _vsnprintf
#else
#define OPNG_VSNPRINTF vsnprintf
#endif

    va_list arg_ptr;
    int result;

    va_start(arg_ptr, format);
    result = OPNG_VSNPRINTF(buffer, buffer_size, format, arg_ptr);
    va_end(arg_ptr);

    if (result < 0 || (size_t)result >= buffer_size)
    {
        /* Guard against broken [v]snprintf implementations. */
        if (buffer_size > 0)
            buffer[buffer_size - 1] = '\0';
        return -1;
    }
    return result;

#undef OPNG_VSNPRINTF

}

/*
 * Writes a decomposed rational value to a memory buffer.
 * This is the base implementation used internally by the the other
 * ratio-to-string conversion functions.
 */
static int opng_sprint_uratio_impl(char* buffer, size_t buffer_size,
    opng_uxlong_impl_t num, opng_uxlong_impl_t denom,
    int always_percent)
{
    /* (1) num/denom == 0/0                 ==> print "??%"
     * (2) num/denom == INFINITY            ==> print "INFINITY%"
     * (3) 0 <= num/denom < 99.995%         ==> use the percent format "99.99%"
     *     if always_percent:
     * (4)    0.995 <= num/denom < INFINITY ==> use the percent format "999%"
     *     else:
     * (5)    0.995 <= num/denom < 99.995   ==> use the factor format "9.99x"
     * (6)    99.5 <= num/denom < INFINITY  ==> use the factor format "999x"
     *     end if
     */

    opng_uxlong_impl_t integer_part, remainder;
    unsigned int fractional_part, scale;
    double scaled_ratio;

    /* (1,2): num/denom == 0/0 or num/denom == INFINITY */
    if (denom == 0)
        return opng_snprintf_impl(buffer, buffer_size,
            num == 0 ? "??%%" : "INFINITY%%");

    /* (3): 0 <= num/denom < 99.995% */
    /* num/denom < 99.995% <==> denom/(denom-num) < 20000 */
    if (num < denom && denom / (denom - num) < 20000)
    {
        scale = 10000;
        scaled_ratio = ((double)num * (double)scale) / (double)denom;
        fractional_part = (unsigned int)(scaled_ratio + 0.5);
        /* Adjust the scaled result in the event of a roundoff error. */
        /* Such error may occur only if the numerator is extremely large. */
        if (fractional_part >= scale)
            fractional_part = scale - 1;
        return opng_snprintf_impl(buffer, buffer_size,
            "%u.%02u%%",
            fractional_part / 100,
            fractional_part % 100);
    }

    /* Extract the integer part out of the fraction for the remaining cases. */
    integer_part = num / denom;
    remainder = num % denom;
    scale = 100;
    scaled_ratio = ((double)remainder * (double)scale) / (double)denom;
    fractional_part = (unsigned int)(scaled_ratio + 0.5);
    if (fractional_part >= scale)
    {
        fractional_part = 0;
        ++integer_part;
    }

    /* (4): 0.995 <= num/denom < INFINITY */
    if (always_percent)
        return opng_snprintf_impl(buffer, buffer_size,
            "%" OPNG_XLONG_IMPL_FORMAT_PREFIX "u%02u%%",
            integer_part, fractional_part);

    /* (5): 0.995 <= num/denom < 99.995 */
    if (integer_part < 100)
        return opng_snprintf_impl(buffer, buffer_size,
            "%" OPNG_XLONG_IMPL_FORMAT_PREFIX "u.%02ux",
            integer_part, fractional_part);

    /* (6): 99.5 <= num/denom < INFINITY */
    /* Round to the nearest integer. */
    /* Recalculate the integer part, for corner cases like 123.999. */
    integer_part = num / denom;
    if (remainder > (denom - 1) / 2)
        ++integer_part;
    return opng_snprintf_impl(buffer, buffer_size,
        "%" OPNG_XLONG_IMPL_FORMAT_PREFIX "ux",
        integer_part);
}

/*
 * Converts a rational value to a compact factor string representation.
 */
int
opng_ulratio_to_factor_string(char* buffer, size_t buffer_size,
    const struct opng_ulratio* ratio)
{
    opng_uxlong_impl_t num = ratio->num;
    opng_uxlong_impl_t denom = ratio->denom;
    return opng_sprint_uratio_impl(buffer, buffer_size, num, denom, 0);
}

/*
 * Converts a rational value to a compact percent string representation.
 */
int
opng_ulratio_to_percent_string(char* buffer, size_t buffer_size,
    const struct opng_ulratio* ratio)
{
    opng_uxlong_impl_t num = ratio->num;
    opng_uxlong_impl_t denom = ratio->denom;
    return opng_sprint_uratio_impl(buffer, buffer_size, num, denom, 1);
}

#ifdef OPNG_LLONG_T_DEFINED

/*
 * Converts a rational value to a compact factor string representation.
 */
int
opng_ullratio_to_factor_string(char* buffer, size_t buffer_size,
    const struct opng_ullratio* ratio)
{
    opng_uxlong_impl_t num = ratio->num;
    opng_uxlong_impl_t denom = ratio->denom;
    return opng_sprint_uratio_impl(buffer, buffer_size, num, denom, 0);
}

/*
 * Converts a rational value to a compact percent string representation.
 */
int
opng_ullratio_to_percent_string(char* buffer, size_t buffer_size,
    const struct opng_ullratio* ratio)
{
    opng_uxlong_impl_t num = ratio->num;
    opng_uxlong_impl_t denom = ratio->denom;
    return opng_sprint_uratio_impl(buffer, buffer_size, num, denom, 1);
}

#endif  /* OPNG_LLONG_T_DEFINED */






#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*
 * Auto-configuration.
 */
#if defined UNIX || defined __UNIX__ || defined __unix || defined __unix__ || \
    defined _BSD_SOURCE || defined _GNU_SOURCE || defined _SVID_SOURCE || \
    defined _POSIX_SOURCE || defined _POSIX_C_SOURCE || defined _XOPEN_SOURCE
#  define OPNG_OS_UNIX
#endif

#if defined __APPLE__ && defined __MACH__
#  define OPNG_OS_DARWIN
#  ifndef OPNG_OS_UNIX
 /* The macros __unix and __unix__ are not predefined on Darwin. */
#    define OPNG_OS_UNIX
#  endif
#endif

#if defined WIN32 || defined _WIN32 || defined _WIN32_WCE || \
    defined __WIN32__ || defined __NT__
#  define OPNG_OS_WIN32
#endif

#if defined WIN64 || defined _WIN64 || defined __WIN64__
#  define OPNG_OS_WIN64
#endif

#if defined OPNG_OS_WIN32 || defined OPNG_OS_WIN64
#  define OPNG_OS_WINDOWS
#endif

#if defined DOS || defined _DOS || defined __DOS__ || \
    defined MSDOS || defined _MSDOS || defined __MSDOS__
#  define OPNG_OS_DOS
#endif

#if defined OS2 || defined OS_2 || defined __OS2__
#  define OPNG_OS_OS2
#endif

#if defined OPNG_OS_DOS || defined OPNG_OS_OS2
#  define OPNG_OS_DOSISH
#endif

#if defined __CYGWIN__ || defined __DJGPP__ || defined __EMX__
#  define OPNG_OS_UNIXISH
#  ifndef OPNG_OS_UNIX
     /* Strictly speaking, this is not correct, but "it works". */
#    define OPNG_OS_UNIX
#  endif
#endif

#if defined OPNG_OS_UNIX || \
    (!defined OPNG_OS_WINDOWS && !defined OPNG_OS_DOSISH)
#  include <unistd.h>
#  if defined _POSIX_VERSION || defined _XOPEN_VERSION
#    ifndef OPNG_OS_UNIX
#      define OPNG_OS_UNIX
#    endif
#  endif
#endif

#if defined OPNG_OS_UNIX || defined OPNG_OS_WINDOWS || defined OPNG_OS_DOSISH
#  include <fcntl.h>
#  include <sys/stat.h>
#  include <sys/types.h>
#  if defined _MSC_VER || defined __WATCOMC__
#    include <sys/utime.h>
#  else
#    include <utime.h>
#  endif
#endif

#if defined OPNG_OS_WINDOWS || \
    defined OPNG_OS_DOSISH || defined OPNG_OS_UNIXISH
#  include <io.h>
#endif

#if defined OPNG_OS_WINDOWS
#  include <windows.h>
#endif

#if defined OPNG_OS_DOSISH
#  include <process.h>
#endif


/*
 * More auto-configuration.
 */
#if (defined OPNG_OS_WINDOWS || defined OPNG_OS_DOSISH) && \
    !defined OPNG_OS_UNIXISH
#  define OPNG_PATH_DIRSEP '\\'
#  define OPNG_PATH_DIRSEP_STR "\\"
#  define OPNG_PATH_DIRSEP_ALL_STR "/\\"
#else
#  define OPNG_PATH_DIRSEP '/'
#  define OPNG_PATH_DIRSEP_STR "/"
#  if defined OPNG_OS_UNIXISH
#    define OPNG_PATH_DIRSEP_ALL_STR "/\\"
#  elif defined OPNG_OS_DARWIN
#    define OPNG_PATH_DIRSEP_ALL_STR "/:"
#  else  /* OPNG_OS_UNIX and others */
#    define OPNG_PATH_DIRSEP_ALL_STR "/"
#  endif
#endif
#define OPNG_PATH_EXTSEP '.'
#define OPNG_PATH_EXTSEP_STR "."

#if defined OPNG_OS_WINDOWS || \
    defined OPNG_OS_DOSISH || defined OPNG_OS_UNIXISH
#  define OPNG_PATH_DOS
#endif

#ifdef R_OK
#  define OPNG_TEST_READ R_OK
#else
#  define OPNG_TEST_READ 4
#endif
#ifdef W_OK
#  define OPNG_TEST_WRITE W_OK
#else
#  define OPNG_TEST_WRITE 2
#endif
#ifdef X_OK
#  define OPNG_TEST_EXEC X_OK
#else
#  define OPNG_TEST_EXEC 1
#endif
#ifdef F_OK
#  define OPNG_TEST_FILE F_OK
#else
#  define OPNG_TEST_FILE 0
#endif


 /*
  * Utility macros.
  */
#ifdef OPNG_PATH_DOS
#  define OPNG_PATH_IS_DRIVE_LETTER(ch) \
          (((ch) >= 'A' && (ch) <= 'Z') || ((ch) >= 'a' && (ch) <= 'z'))
#endif

#ifdef OPNG_OS_WINDOWS
#  if defined OPNG_OS_WIN64 || (defined _MSC_VER && _MSC_VER >= 1500)
#    define OPNG_HAVE_STDIO__I64
#    define OPNG_OS_WINDOWS_IS_WIN9X() 0
#  else
#    if (defined _MSC_VER && _MSC_VER >= 1400) || \
        (defined __MSVCRT_VERSION__ && __MSVCRT_VERSION__ >= 0x800)
#      define OPNG_HAVE_STDIO__I64
#    endif
#    define OPNG_OS_WINDOWS_IS_WIN9X() (GetVersion() >= 0x80000000U)
#  endif
#endif


  /*
   * Returns the current value of the file position indicator.
   */
opng_foffset_t
opng_ftello(FILE* stream)
{
#if defined OPNG_HAVE_STDIO__I64

    return (opng_foffset_t)_ftelli64(stream);

#elif defined OPNG_OS_UNIX && (OPNG_FOFFSET_MAX > LONG_MAX)

    /* We don't know if off_t is sufficiently wide, we only know that
     * long isn't. We are trying just a little harder, in the absence
     * of an fopen64/ftell64 solution.
     */
    return (opng_foffset_t)ftello(stream);

#else  /* generic */

    return (opng_foffset_t)ftell(stream);

#endif
}

/*
 * Sets the file position indicator at the specified file offset.
 */
int
opng_fseeko(FILE* stream, opng_foffset_t offset, int whence)
{
#if defined OPNG_HAVE_STDIO__I64

    return _fseeki64(stream, (__int64)offset, whence);

#elif defined OPNG_OS_UNIX

#if OPNG_FOFFSET_MAX > LONG_MAX
    /* We don't know if off_t is sufficiently wide, we only know that
     * long isn't. We are trying just a little harder, in the absence
     * of an fopen64/fseek64 solution.
     */
    return fseeko(stream, (off_t)offset, whence);
#else
    return fseek(stream, (long)offset, whence);
#endif

#else  /* generic */

    return (fseek(stream, (long)offset, whence) == 0) ? 0 : -1;

#endif
}

/*
 * Reads a block of data from the specified file offset.
 */
size_t
opng_freado(FILE* stream, opng_foffset_t offset, int whence,
    void* block, size_t blocksize)
{
    fpos_t pos;
    size_t result;

    if (fgetpos(stream, &pos) != 0)
        return 0;
    if (opng_fseeko(stream, offset, whence) == 0)
        result = fread(block, 1, blocksize, stream);
    else
        result = 0;
    if (fsetpos(stream, &pos) != 0)
        result = 0;
    return result;
}

/*
 * Writes a block of data at the specified file offset.
 */
size_t
opng_fwriteo(FILE* stream, opng_foffset_t offset, int whence,
    const void* block, size_t blocksize)
{
    fpos_t pos;
    size_t result;

    if (fgetpos(stream, &pos) != 0 || fflush(stream) != 0)
        return 0;
    if (opng_fseeko(stream, offset, whence) == 0)
        result = fwrite(block, 1, blocksize, stream);
    else
        result = 0;
    if (fflush(stream) != 0)
        result = 0;
    if (fsetpos(stream, &pos) != 0)
        result = 0;
    return result;
}

/*
 * Gets the size of the specified file stream.
 */
int
opng_fgetsize(FILE* stream, opng_fsize_t* size)
{
#if defined OPNG_OS_WINDOWS

    HANDLE hFile;
    DWORD dwSizeLow, dwSizeHigh;

    hFile = (HANDLE)_get_osfhandle(_fileno(stream));
    dwSizeLow = GetFileSize(hFile, &dwSizeHigh);
    if (GetLastError() != NO_ERROR)
        return -1;
    *size = (opng_fsize_t)dwSizeLow + ((opng_fsize_t)dwSizeHigh << 32);
    return 0;

#elif defined OPNG_OS_UNIX

    struct stat sbuf;

    if (fstat(fileno(stream), &sbuf) != 0)
        return -1;
    if (sbuf.st_size < 0)
        return -1;
    *size = (opng_fsize_t)sbuf.st_size;
    return 0;

#else  /* generic */

    opng_foffset_t offset;

    if (opng_fseeko(stream, 0, SEEK_END) != 0)
        return -1;
    offset = opng_ftello(stream);
    if (offset < 0)
        return -1;
    *size = (opng_fsize_t)offset;
    return 0;

#endif
}

/*
 * Makes a new path name by replacing the directory component of
 * a specified path name.
 */
char*
opng_path_replace_dir(char* buffer, size_t bufsize,
    const char* old_path, const char* new_dirname)
{
    const char* path, * ptr;
    size_t dirlen;

    /* Extract file name from old_path. */
    path = old_path;
#ifdef OPNG_PATH_DOS
    /* Skip the drive name, if present. */
    if (OPNG_PATH_IS_DRIVE_LETTER(path[0]) && path[1] == ':')
        path += 2;
#endif
    for (; ; )
    {
        ptr = strpbrk(path, OPNG_PATH_DIRSEP_ALL_STR);
        if (ptr == NULL)
            break;
        path = ptr + 1;
    }

    /* Make sure the buffer is large enough. */
    dirlen = strlen(new_dirname);
    if (dirlen + strlen(path) + 2 >= bufsize)  /* overflow */
        return NULL;

    /* Copy the new directory name. Also append a slash if necessary. */
    if (dirlen > 0)
    {
        strcpy(buffer, new_dirname);
#ifdef OPNG_PATH_DOS
        if (dirlen == 2 && buffer[1] == ':' &&
            OPNG_PATH_IS_DRIVE_LETTER(buffer[0]))
        {
            /* Special case: do not append slash to "C:". */
        }
        else
#endif
        {
            if (strchr(OPNG_PATH_DIRSEP_ALL_STR, buffer[dirlen - 1]) == NULL)
                buffer[dirlen++] = OPNG_PATH_DIRSEP;
        }
    }

    /* Append the file name. */
    strcpy(buffer + dirlen, path);
    return buffer;
}

/*
 * Makes a new path name by changing the extension component of
 * a specified path name.
 */
char*
opng_path_replace_ext(char* buffer, size_t bufsize,
    const char* old_path, const char* new_extname)
{
    size_t i, pos;

    if (new_extname[0] != OPNG_PATH_EXTSEP)  /* invalid argument */
        return NULL;
    for (i = 0, pos = (size_t)(-1); old_path[i] != '\0'; ++i)
    {
        if (i >= bufsize)  /* overflow */
            return NULL;
        if ((buffer[i] = old_path[i]) == OPNG_PATH_EXTSEP)
            pos = i;
    }
    if (i > pos)
    {
        /* An extension already exists in old_path. Go back. */
        i = pos;
    }
    for (; ; ++i, ++new_extname)
    {
        if (i >= bufsize)  /* overflow */
            return NULL;
        if ((buffer[i] = *new_extname) == '\0')
            return buffer;
    }
}


/*
 * Changes the name of a file system object.
 */
int
opng_os_rename(const char* src_path, const char* dest_path, int clobber)
{
#if defined OPNG_OS_WINDOWS

    DWORD dwFlags;

#if !defined OPNG_OS_WIN64
    if (OPNG_OS_WINDOWS_IS_WIN9X())
    {
        /* MoveFileEx is not available under Win9X; use MoveFile. */
        if (MoveFileA(src_path, dest_path))
            return 0;
        if (!clobber)
            return -1;
        DeleteFileA(dest_path);
        return MoveFileA(src_path, dest_path) ? 0 : -1;
    }
#endif

    dwFlags = clobber ? MOVEFILE_REPLACE_EXISTING : 0;
    return MoveFileExA(src_path, dest_path, dwFlags) ? 0 : -1;

#elif defined OPNG_OS_UNIX

    if (!clobber)
    {
        if (access(dest_path, OPNG_TEST_FILE) >= 0)
            return -1;
    }
    return rename(src_path, dest_path);

#else  /* generic */

    if (opng_test(dest_path, "e") == 0)
    {
        if (!clobber)
            return -1;
        opng_unlink(dest_path);
    }
    return rename(src_path, dest_path);

#endif
}

/*
 * Copies the attributes (access mode, time stamp, etc.) of a file system
 * object.
 */
int opng_os_copy_attr(const char* src_path, const char* dest_path)
{
#if defined OPNG_OS_WINDOWS

    HANDLE hFile;
    FILETIME ftLastWrite;
    BOOL success;

    hFile = CreateFileA(src_path, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, 0);
    if (hFile == INVALID_HANDLE_VALUE)
        return -1;
    success = GetFileTime(hFile, NULL, NULL, &ftLastWrite);
    CloseHandle(hFile);
    if (!success)
        return -1;

    hFile = CreateFileA(dest_path, GENERIC_WRITE, 0, NULL, OPEN_EXISTING,
        (OPNG_OS_WINDOWS_IS_WIN9X() ? 0 : FILE_FLAG_BACKUP_SEMANTICS), 0);
    if (hFile == INVALID_HANDLE_VALUE)
        return -1;
    success = SetFileTime(hFile, NULL, NULL, &ftLastWrite);
    CloseHandle(hFile);
    if (!success)
        return -1;

    /* TODO: Copy the access mode. */

    return 0;

#elif defined OPNG_OS_UNIX || defined OPNG_OS_DOSISH

    struct stat sbuf;
    int result;

    if (stat(src_path, &sbuf) != 0)
        return -1;

    result = 0;

    if (chown(dest_path, sbuf.st_uid, sbuf.st_gid) != 0)
    {
        /* This is not required to succeed. Fall through. */
    }

    if (chmod(dest_path, sbuf.st_mode) != 0)
        result = -1;

#if defined AT_FDCWD && defined UTIME_NOW && defined UTIME_OMIT
    {
        struct timespec times[2];

#if defined OPNG_OS_DARWIN
        times[0] = sbuf.st_atimespec;
        times[1] = sbuf.st_mtimespec;
#else
        times[0] = sbuf.st_atim;
        times[1] = sbuf.st_mtim;
#endif
        if (utimensat(AT_FDCWD, dest_path, times, 0) != 0)
            result = -1;
    }
#else  /* legacy utime */
    {
        struct utimbuf utbuf;

        utbuf.actime = sbuf.st_atime;
        utbuf.modtime = sbuf.st_mtime;
        if (utime(dest_path, &utbuf) != 0)
            result = -1;
    }
#endif

    return result;

#else  /* generic */

    (void)src_path;  /* unused */
    (void)dest_path;  /* unused */

    /* Always fail. */
    return -1;

#endif
}

int opng_os_copy_attr_u(const wchar_t* src_path, const wchar_t* dest_path)
{
#if defined OPNG_OS_WINDOWS

    HANDLE hFile;
    FILETIME ftLastWrite;
    BOOL success;

    hFile = CreateFileW(src_path, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, 0);
    if (hFile == INVALID_HANDLE_VALUE)
        return -1;
    success = GetFileTime(hFile, NULL, NULL, &ftLastWrite);
    CloseHandle(hFile);
    if (!success)
        return -1;

    hFile = CreateFileW(dest_path, GENERIC_WRITE, 0, NULL, OPEN_EXISTING,
        (OPNG_OS_WINDOWS_IS_WIN9X() ? 0 : FILE_FLAG_BACKUP_SEMANTICS), 0);
    if (hFile == INVALID_HANDLE_VALUE)
        return -1;
    success = SetFileTime(hFile, NULL, NULL, &ftLastWrite);
    CloseHandle(hFile);
    if (!success)
        return -1;

    /* TODO: Copy the access mode. */

    return 0;

#elif defined OPNG_OS_UNIX || defined OPNG_OS_DOSISH

    struct stat sbuf;
    int result;

    if (stat(src_path, &sbuf) != 0)
        return -1;

    result = 0;

    if (chown(dest_path, sbuf.st_uid, sbuf.st_gid) != 0)
    {
        /* This is not required to succeed. Fall through. */
    }

    if (chmod(dest_path, sbuf.st_mode) != 0)
        result = -1;

#if defined AT_FDCWD && defined UTIME_NOW && defined UTIME_OMIT
    {
        struct timespec times[2];

#if defined OPNG_OS_DARWIN
        times[0] = sbuf.st_atimespec;
        times[1] = sbuf.st_mtimespec;
#else
        times[0] = sbuf.st_atim;
        times[1] = sbuf.st_mtim;
#endif
        if (utimensat(AT_FDCWD, dest_path, times, 0) != 0)
            result = -1;
    }
#else  /* legacy utime */
    {
        struct utimbuf utbuf;

        utbuf.actime = sbuf.st_atime;
        utbuf.modtime = sbuf.st_mtime;
        if (utime(dest_path, &utbuf) != 0)
            result = -1;
    }
#endif

    return result;

#else  /* generic */

    (void)src_path;  /* unused */
    (void)dest_path;  /* unused */

    /* Always fail. */
    return -1;

#endif
}


/*
 * Creates a new directory.
 */
int
opng_os_create_dir(const char* dirname)
{
    /* Exit early if there is no directory name. */
    if (dirname[0] == '\0')
        return 0;
#ifdef OPNG_PATH_DOS
    if (OPNG_PATH_IS_DRIVE_LETTER(dirname[0]) &&
        dirname[1] == ':' && dirname[2] == '\0')
        return 0;
#endif

#if defined OPNG_OS_WINDOWS

    {
        size_t dirlen;
        char* wildname;
        HANDLE hFind;
        WIN32_FIND_DATAA wfd;

        /* Fail early if dirname is too long. */
        dirlen = strlen(dirname);
        if (dirlen * 2 <= dirlen)
            return -1;

        /* Find files in (dirname + "\\*") and exit early if dirname exists. */
        wildname = (char*)malloc(dirlen + 3);
        if (wildname == NULL)
            return -1;
        strcpy(wildname, dirname);
        if (strchr(OPNG_PATH_DIRSEP_ALL_STR, wildname[dirlen - 1]) == NULL)
            wildname[dirlen++] = OPNG_PATH_DIRSEP;
        wildname[dirlen++] = '*';
        wildname[dirlen] = '\0';
        hFind = FindFirstFileA(wildname, &wfd);
        free(wildname);
        if (hFind != INVALID_HANDLE_VALUE)
        {
            FindClose(hFind);
            return 0;
        }

        /* There is no directory, so create one now. */
        return CreateDirectoryA(dirname, NULL) ? 0 : -1;
    }

#elif defined OPNG_OS_UNIX || defined OPNG_OS_DOSISH

    {
        struct stat sbuf;

        if (stat(dirname, &sbuf) == 0)
            return (sbuf.st_mode & S_IFDIR) ? 0 : -1;

        /* There is no directory, so create one now. */
#if defined OPNG_OS_UNIX
        return mkdir(dirname, 0777);
#else
        return mkdir(dirname);
#endif
    }

#else  /* generic */

    (void)dirname;  /* unused */

    /* Always fail. */
    return -1;

#endif
}

/*
 * Determines if the accessibility of the specified file system object
 * satisfies the specified access mode.
 */
int
opng_os_test(const char* path, const char* mode)
{
    int faccess, freg;

    faccess = freg = 0;
    if (strchr(mode, 'f') != NULL)
        freg = 1;
    if (strchr(mode, 'r') != NULL)
        faccess |= OPNG_TEST_READ;
    if (strchr(mode, 'w') != NULL)
        faccess |= OPNG_TEST_WRITE;
    if (strchr(mode, 'x') != NULL)
        faccess |= OPNG_TEST_EXEC;
    if (faccess == 0 && !freg)
    {
        if (strchr(mode, 'e') == NULL)
            return 0;
    }

#if defined OPNG_OS_WINDOWS

    {
        DWORD attr;

        attr = GetFileAttributesA(path);
        if (attr == 0xffffffffU)
            return -1;
        if (freg && (attr & FILE_ATTRIBUTE_DIRECTORY))
            return -1;
        if ((faccess & OPNG_TEST_WRITE) && (attr & FILE_ATTRIBUTE_READONLY))
            return -1;
        return 0;
    }

#elif defined OPNG_OS_UNIX || defined OPNG_OS_DOSISH

    {
        struct stat sbuf;

        if (stat(path, &sbuf) != 0)
            return -1;
        if (freg && ((sbuf.st_mode & S_IFREG) != S_IFREG))
            return -1;
        if (faccess == 0)
            return 0;
        return access(path, faccess);
    }

#else  /* generic */

    {
        FILE* stream;

        if (faccess & OPNG_TEST_WRITE)
            stream = fopen(path, "r+b");
        else
            stream = fopen(path, "rb");
        if (stream == NULL)
            return -1;
        fclose(stream);
        return 0;
    }

#endif
}

/*
 * Determines if two accessible paths are equivalent, i.e. they
 * refer to the same file system object.
 */
int
opng_os_test_eq(const char* path1, const char* path2)
{
#if defined OPNG_OS_WINDOWS

    HANDLE hFile1, hFile2;
    BY_HANDLE_FILE_INFORMATION fileInfo1, fileInfo2;
    int result;

    hFile1 = CreateFileA(path1, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, 0);
    if (hFile1 == INVALID_HANDLE_VALUE)
        return -1;
    hFile2 = CreateFileA(path2, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, 0);
    if (hFile2 == INVALID_HANDLE_VALUE)
    {
        CloseHandle(hFile1);
        return -1;
    }
    if (!GetFileInformationByHandle(hFile1, &fileInfo1) ||
        !GetFileInformationByHandle(hFile2, &fileInfo2))
    {
        /* Can't retrieve the file info. */
        result = -1;
    }
    else if (fileInfo1.nFileIndexLow == fileInfo2.nFileIndexLow &&
        fileInfo1.nFileIndexHigh == fileInfo2.nFileIndexHigh &&
        fileInfo1.dwVolumeSerialNumber == fileInfo2.dwVolumeSerialNumber)
    {
        /* The two paths have the same ID on the same volume. */
        result = 1;
    }
    else
    {
        /* The two paths have different IDs or sit on different volumes. */
        result = 0;
    }
    CloseHandle(hFile1);
    CloseHandle(hFile2);
    return result;

#elif defined OPNG_OS_UNIX || defined OPNG_OS_DOSISH

    struct stat sbuf1, sbuf2;

    if (stat(path1, &sbuf1) != 0 || stat(path2, &sbuf2) != 0)
    {
        /* Can't stat the paths. */
        return -1;
    }
    if (sbuf1.st_dev == sbuf2.st_dev && sbuf1.st_ino == sbuf2.st_ino)
    {
        /* The two paths have the same device and inode numbers. */
        /* The inode numbers are reliable only if they're not 0. */
        return (sbuf1.st_ino != 0) ? 1 : -1;
    }
    else
    {
        /* The two paths have different device or inode numbers. */
        return 0;
    }

#else  /* generic */

    (void)path1;  /* unused */
    (void)path2;  /* unused */

    /* Always unknown. */
    return -1;

#endif
}

/*
 * Removes a directory entry.
 */
int
opng_os_unlink(const char* path)
{
#if defined OPNG_OS_WINDOWS

    return DeleteFileA(path) ? 0 : -1;

#elif defined OPNG_OS_UNIX || defined OPNG_OS_DOSISH

    return unlink(path);

#else  /* generic */

    return remove(path);

#endif
}





#include <string.h>

#ifndef OPNG_ASSERT
#include <assert.h>
#define OPNG_ASSERT(cond) assert(cond)
#define OPNG_ASSERT_MSG(cond, msg) assert(cond)
#endif

#ifdef png_debug
#define opng_debug(level, msg) png_debug(level, msg)
#else
#define opng_debug(level, msg) ((void)0)
#endif


#ifdef PNG_INFO_IMAGE_SUPPORTED

/*
 * Check if the image information is valid.
 * The image information is said to be valid if all the required
 * critical chunk data is present in the png structures.
 * The function returns 1 if this information is valid, and 0 otherwise.
 */
int PNGAPI
opng_validate_image(png_structp png_ptr, png_infop info_ptr)
{
    opng_debug(1, "in opng_validate_image");

    /* Validate IHDR. */
    if (png_get_bit_depth(png_ptr, info_ptr) == 0)
        return 0;

    /* Validate PLTE. */
    if (png_get_color_type(png_ptr, info_ptr) & PNG_COLOR_MASK_PALETTE)
    {
        if (!png_get_valid(png_ptr, info_ptr, PNG_INFO_PLTE))
            return 0;
    }

    /* Validate IDAT. */
    if (!png_get_valid(png_ptr, info_ptr, PNG_INFO_IDAT))
        return 0;

    return 1;
}

#endif /* PNG_INFO_IMAGE_SUPPORTED */


#ifdef OPNG_IMAGE_REDUCTIONS_SUPPORTED

#define OPNG_CMP_RGB(R1, G1, B1, R2, G2, B2) \
   (((int)(R1) != (int)(R2)) ?      \
      ((int)(R1) - (int)(R2)) :     \
      (((int)(G1) != (int)(G2)) ?   \
         ((int)(G1) - (int)(G2)) :  \
         ((int)(B1) - (int)(B2))))

#define OPNG_CMP_ARGB(A1, R1, G1, B1, A2, R2, G2, B2) \
   (((int)(A1) != (int)(A2)) ?          \
      ((int)(A1) - (int)(A2)) :         \
      (((int)(R1) != (R2)) ?            \
         ((int)(R1) - (int)(R2)) :      \
         (((int)(G1) != (int)(G2)) ?    \
            ((int)(G1) - (int)(G2)) :   \
            ((int)(B1) - (int)(B2)))))

/*
 * Build a color+alpha palette in which the entries are sorted by
 * (alpha, red, green, blue), in this particular order.
 * Use the insertion sort algorithm.
 * The alpha value is ignored if it is not in the range [0 .. 255].
 * The function returns:
 *   1 if the insertion is successful;  *index = position of new entry.
 *   0 if the insertion is unnecessary; *index = position of crt entry.
 *  -1 if overflow;            *num_palette = *num_trans = *index = -1.
 */
static int opng_insert_palette_entry(png_colorp palette, int* num_palette,
    png_bytep trans_alpha, int* num_trans, int max_tuples,
    unsigned int red, unsigned int green, unsigned int blue, unsigned int alpha,
    int* index)
{
    int low, high, mid, cmp;
    int i;

    OPNG_ASSERT(*num_palette >= 0 && *num_palette <= max_tuples);
    OPNG_ASSERT(*num_trans >= 0 && *num_trans <= *num_palette);

    if (alpha < 255)
    {
        /* Do a binary search among transparent tuples. */
        low = 0;
        high = *num_trans - 1;
        while (low <= high)
        {
            mid = (low + high) / 2;
            cmp = OPNG_CMP_ARGB(alpha, red, green, blue,
                trans_alpha[mid],
                palette[mid].red, palette[mid].green, palette[mid].blue);
            if (cmp < 0)
                high = mid - 1;
            else if (cmp > 0)
                low = mid + 1;
            else
            {
                *index = mid;
                return 0;
            }
        }
    }
    else  /* alpha == 255 || alpha not in [0 .. 255] */
    {
        /* Do a (faster) binary search among opaque tuples. */
        low = *num_trans;
        high = *num_palette - 1;
        while (low <= high)
        {
            mid = (low + high) / 2;
            cmp = OPNG_CMP_RGB(red, green, blue,
                palette[mid].red, palette[mid].green, palette[mid].blue);
            if (cmp < 0)
                high = mid - 1;
            else if (cmp > 0)
                low = mid + 1;
            else
            {
                *index = mid;
                return 0;
            }
        }
    }
    if (alpha > 255)
    {
        /* The binary search among opaque tuples has failed. */
        /* Do a linear search among transparent tuples, ignoring alpha. */
        for (i = 0; i < *num_trans; ++i)
        {
            cmp = OPNG_CMP_RGB(red, green, blue,
                palette[i].red, palette[i].green, palette[i].blue);
            if (cmp == 0)
            {
                *index = i;
                return 0;
            }
        }
    }

    /* Check for overflow. */
    if (*num_palette >= max_tuples)
    {
        *num_palette = *num_trans = *index = -1;
        return -1;
    }

    /* Insert new tuple at [low]. */
    OPNG_ASSERT(low >= 0 && low <= *num_palette);
    for (i = *num_palette; i > low; --i)
        palette[i] = palette[i - 1];
    palette[low].red = (png_byte)red;
    palette[low].green = (png_byte)green;
    palette[low].blue = (png_byte)blue;
    ++(*num_palette);
    if (alpha < 255)
    {
        OPNG_ASSERT(low <= *num_trans);
        for (i = *num_trans; i > low; --i)
            trans_alpha[i] = trans_alpha[i - 1];
        trans_alpha[low] = (png_byte)alpha;
        ++(*num_trans);
    }
    *index = low;
    return 1;
}

/*
 * Change the size of the palette buffer.
 * Changing info_ptr->num_palette directly, avoiding reallocation, should
 * have been sufficient, but can't be done using the current libpng API.
 */
static void opng_realloc_PLTE(png_structp png_ptr, png_infop info_ptr, int num_palette)
{
    png_color buffer[PNG_MAX_PALETTE_LENGTH];
    png_colorp palette;
    int src_num_palette;

    opng_debug(1, "in opng_realloc_PLTE");

    OPNG_ASSERT(num_palette > 0);
    src_num_palette = 0;
    png_get_PLTE(png_ptr, info_ptr, &palette, &src_num_palette);
    if (num_palette == src_num_palette)
        return;
    memcpy(buffer, palette, num_palette * sizeof(png_color));
    if (num_palette > src_num_palette)
        memset(buffer + src_num_palette, 0,
            (num_palette - src_num_palette) * sizeof(png_color));
    png_set_PLTE(png_ptr, info_ptr, buffer, num_palette);
}

/*
 * Change the size of the transparency buffer.
 * Changing info_ptr->num_trans directly, avoiding reallocation, should
 * have been sufficient, but can't be done using the current libpng API.
 */
static void /* PRIVATE */
opng_realloc_tRNS(png_structp png_ptr, png_infop info_ptr, int num_trans)
{
    png_byte buffer[PNG_MAX_PALETTE_LENGTH];
    png_bytep trans_alpha;
    int src_num_trans;

    opng_debug(1, "in opng_realloc_tRNS");

    OPNG_ASSERT(num_trans > 0);  /* tRNS should be invalidated in this case */
    src_num_trans = 0;
    png_get_tRNS(png_ptr, info_ptr, &trans_alpha, &src_num_trans, NULL);
    if (num_trans == src_num_trans)
        return;
    memcpy(buffer, trans_alpha, (size_t)num_trans);
    if (num_trans > src_num_trans)
        memset(buffer + src_num_trans, 0, num_trans - src_num_trans);
    png_set_tRNS(png_ptr, info_ptr, buffer, num_trans, NULL);
}

/*
 * Retrieve the alpha samples from the given image row.
 */
static void opng_get_alpha_row(png_row_infop row_info_ptr, png_color_16p trans_color,
    png_bytep row, png_bytep alpha_row)
{
    png_bytep sample_ptr;
    png_uint_32 width;
    int color_type, bit_depth, channels;
    png_byte trans_red, trans_green, trans_blue, trans_gray;
    png_uint_32 i;

    width = row_info_ptr->width;
    color_type = row_info_ptr->color_type;
    bit_depth = row_info_ptr->bit_depth;
    channels = row_info_ptr->channels;

    OPNG_ASSERT(!(color_type & PNG_COLOR_MASK_PALETTE));
    OPNG_ASSERT(bit_depth == 8);

    if (!(color_type & PNG_COLOR_MASK_ALPHA))
    {
        if (trans_color == NULL)
        {
            /* All pixels are fully opaque. */
            memset(alpha_row, 255, (size_t)width);
            return;
        }
        if (color_type == PNG_COLOR_TYPE_RGB)
        {
            OPNG_ASSERT(channels == 3);
            trans_red = (png_byte)trans_color->red;
            trans_green = (png_byte)trans_color->green;
            trans_blue = (png_byte)trans_color->blue;
            sample_ptr = row;
            for (i = 0; i < width; ++i, sample_ptr += 3)
                alpha_row[i] = (png_byte)
                ((sample_ptr[0] == trans_red &&
                    sample_ptr[1] == trans_green &&
                    sample_ptr[2] == trans_blue) ? 0 : 255);
        }
        else
        {
            OPNG_ASSERT(color_type == PNG_COLOR_TYPE_GRAY);
            OPNG_ASSERT(channels == 1);
            trans_gray = (png_byte)trans_color->gray;
            for (i = 0; i < width; ++i)
                alpha_row[i] = (png_byte)((row[i] == trans_gray) ? 0 : 255);
        }
        return;
    }

    /* There is a real alpha channel. The alpha sample is last in RGBA tuple. */
    OPNG_ASSERT(channels > 1);
    sample_ptr = row + (channels - 1);
    for (i = 0; i < width; ++i, sample_ptr += channels, ++alpha_row)
        *alpha_row = *sample_ptr;
}

/*
 * Analyze the redundancy of bits inside the image.
 * The parameter reductions indicates the intended reductions.
 * The function returns the possible reductions.
 */
static png_uint_32  opng_analyze_bits(png_structp png_ptr, png_infop info_ptr,
    png_uint_32 reductions)
{
    png_bytepp row_ptr;
    png_bytep component_ptr;
    png_uint_32 height, width;
    int bit_depth, color_type, byte_depth, channels, sample_size, offset_alpha;
#ifdef PNG_bKGD_SUPPORTED
    png_color_16p background;
#endif
    png_uint_32 i, j;

    opng_debug(1, "in opng_analyze_bits");

    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
        NULL, NULL, NULL);
    if (bit_depth < 8)
        return OPNG_REDUCE_NONE;  /* not applicable */
    if (color_type & PNG_COLOR_MASK_PALETTE)
        return OPNG_REDUCE_NONE;  /* let opng_reduce_palette() handle it */

    byte_depth = bit_depth / 8;
    channels = png_get_channels(png_ptr, info_ptr);
    sample_size = channels * byte_depth;
    offset_alpha = (channels - 1) * byte_depth;

    /* Select the applicable reductions. */
    reductions &= (OPNG_REDUCE_16_TO_8 |
        OPNG_REDUCE_RGB_TO_GRAY | OPNG_REDUCE_STRIP_ALPHA);
    if (bit_depth <= 8)
        reductions &= ~OPNG_REDUCE_16_TO_8;
    if (!(color_type & PNG_COLOR_MASK_COLOR))
        reductions &= ~OPNG_REDUCE_RGB_TO_GRAY;
    if (!(color_type & PNG_COLOR_MASK_ALPHA))
        reductions &= ~OPNG_REDUCE_STRIP_ALPHA;

    /* Check if the ancillary information allows these reductions. */
#ifdef PNG_bKGD_SUPPORTED
    if (png_get_bKGD(png_ptr, info_ptr, &background))
    {
        if (reductions & OPNG_REDUCE_16_TO_8)
        {
            if (background->red % 257 != 0 ||
                background->green % 257 != 0 ||
                background->blue % 257 != 0 ||
                background->gray % 257 != 0)
                reductions &= ~OPNG_REDUCE_16_TO_8;
        }
        if (reductions & OPNG_REDUCE_RGB_TO_GRAY)
        {
            if (background->red != background->green ||
                background->red != background->blue)
                reductions &= ~OPNG_REDUCE_RGB_TO_GRAY;
        }
    }
#endif

    /* Check for each possible reduction, row by row. */
    row_ptr = png_get_rows(png_ptr, info_ptr);
    for (i = 0; i < height; ++i, ++row_ptr)
    {
        if (reductions == OPNG_REDUCE_NONE)
            return OPNG_REDUCE_NONE;  /* no need to go any further */

        /* Check if it is possible to reduce the bit depth to 8. */
        if (reductions & OPNG_REDUCE_16_TO_8)
        {
            component_ptr = *row_ptr;
            for (j = 0; j < channels * width; ++j, component_ptr += 2)
            {
                if (component_ptr[0] != component_ptr[1])
                {
                    reductions &= ~OPNG_REDUCE_16_TO_8;
                    break;
                }
            }
        }

        if (bit_depth == 8)
        {
            /* Check if it is possible to reduce rgb --> gray. */
            if (reductions & OPNG_REDUCE_RGB_TO_GRAY)
            {
                component_ptr = *row_ptr;
                for (j = 0; j < width; ++j, component_ptr += sample_size)
                {
                    if (component_ptr[0] != component_ptr[1] ||
                        component_ptr[0] != component_ptr[2])
                    {
                        reductions &= ~OPNG_REDUCE_RGB_TO_GRAY;
                        break;
                    }
                }
            }

            /* Check if it is possible to strip the alpha channel. */
            if (reductions & OPNG_REDUCE_STRIP_ALPHA)
            {
                component_ptr = *row_ptr + offset_alpha;
                for (j = 0; j < width; ++j, component_ptr += sample_size)
                {
                    if (component_ptr[0] != 255)
                    {
                        reductions &= ~OPNG_REDUCE_STRIP_ALPHA;
                        break;
                    }
                }
            }
        }
        else  /* bit_depth == 16 */
        {
            /* Check if it is possible to reduce rgb --> gray. */
            if (reductions & OPNG_REDUCE_RGB_TO_GRAY)
            {
                component_ptr = *row_ptr;
                for (j = 0; j < width; ++j, component_ptr += sample_size)
                {
                    if (component_ptr[0] != component_ptr[2] ||
                        component_ptr[0] != component_ptr[4] ||
                        component_ptr[1] != component_ptr[3] ||
                        component_ptr[1] != component_ptr[5])
                    {
                        reductions &= ~OPNG_REDUCE_RGB_TO_GRAY;
                        break;
                    }
                }
            }

            /* Check if it is possible to strip the alpha channel. */
            if (reductions & OPNG_REDUCE_STRIP_ALPHA)
            {
                component_ptr = *row_ptr + offset_alpha;
                for (j = 0; j < width; ++j, component_ptr += sample_size)
                {
                    if (component_ptr[0] != 255 || component_ptr[1] != 255)
                    {
                        reductions &= ~OPNG_REDUCE_STRIP_ALPHA;
                        break;
                    }
                }
            }
        }
    }

    return reductions;
}

/*
 * Reduce the image type to a lower bit depth and color type,
 * by removing redundant bits.
 * Possible reductions: 16bpp to 8bpp; RGB to gray; strip alpha.
 * The parameter reductions indicates the intended reductions.
 * The function returns the successful reductions.
 * All reductions are performed in a single step.
 */
static png_uint_32  opng_reduce_bits(png_structp png_ptr, png_infop info_ptr,
    png_uint_32 reductions)
{
    png_bytepp row_ptr;
    png_bytep src_ptr, dest_ptr;
    png_uint_32 width, height;
    int interlace_type, compression_type, filter_type;
    int src_bit_depth, dest_bit_depth;
    int src_byte_depth, dest_byte_depth;
    int src_color_type, dest_color_type;
    int src_channels, dest_channels;
    int src_sample_size, dest_sample_size;
    int tran_tbl[8];
    png_color_16p trans_color;
#ifdef PNG_bKGD_SUPPORTED
    png_color_16p background;
#endif
#ifdef PNG_sBIT_SUPPORTED
    png_color_8p sig_bits;
#endif
    png_uint_32 i, j;
    int k;

    opng_debug(1, "in opng_reduce_bits");

    /* See which reductions may be performed. */
    reductions = opng_analyze_bits(png_ptr, info_ptr, reductions);
    if (reductions == OPNG_REDUCE_NONE)
        return OPNG_REDUCE_NONE;  /* exit early */

    png_get_IHDR(png_ptr, info_ptr, &width, &height,
        &src_bit_depth, &src_color_type,
        &interlace_type, &compression_type, &filter_type);

    /* Compute the new image parameters bit_depth, color_type, etc. */
    OPNG_ASSERT(src_bit_depth >= 8);
    if (reductions & OPNG_REDUCE_16_TO_8)
    {
        OPNG_ASSERT(src_bit_depth == 16);
        dest_bit_depth = 8;
    }
    else
        dest_bit_depth = src_bit_depth;

    src_byte_depth = src_bit_depth / 8;
    dest_byte_depth = dest_bit_depth / 8;

    dest_color_type = src_color_type;
    if (reductions & OPNG_REDUCE_RGB_TO_GRAY)
    {
        OPNG_ASSERT(src_color_type & PNG_COLOR_MASK_COLOR);
        dest_color_type &= ~PNG_COLOR_MASK_COLOR;
    }
    if (reductions & OPNG_REDUCE_STRIP_ALPHA)
    {
        OPNG_ASSERT(src_color_type & PNG_COLOR_MASK_ALPHA);
        dest_color_type &= ~PNG_COLOR_MASK_ALPHA;
    }

    src_channels = png_get_channels(png_ptr, info_ptr);
    dest_channels =
        ((dest_color_type & PNG_COLOR_MASK_COLOR) ? 3 : 1) +
        ((dest_color_type & PNG_COLOR_MASK_ALPHA) ? 1 : 0);

    src_sample_size = src_channels * src_byte_depth;
    dest_sample_size = dest_channels * dest_byte_depth;

    /* Pre-compute the intra-sample translation table. */
    for (k = 0; k < 4 * dest_byte_depth; ++k)
        tran_tbl[k] = k * src_bit_depth / dest_bit_depth;
    /* If rgb --> gray, shift the alpha component two positions to the left. */
    if ((reductions & OPNG_REDUCE_RGB_TO_GRAY) &&
        (dest_color_type & PNG_COLOR_MASK_ALPHA))
    {
        tran_tbl[dest_byte_depth] = tran_tbl[3 * dest_byte_depth];
        if (dest_byte_depth == 2)
            tran_tbl[dest_byte_depth + 1] = tran_tbl[3 * dest_byte_depth + 1];
    }

    /* Translate the samples to the new image type. */
    OPNG_ASSERT(src_sample_size > dest_sample_size);
    row_ptr = png_get_rows(png_ptr, info_ptr);
    for (i = 0; i < height; ++i, ++row_ptr)
    {
        src_ptr = dest_ptr = *row_ptr;
        for (j = 0; j < width; ++j)
        {
            for (k = 0; k < dest_sample_size; ++k)
                dest_ptr[k] = src_ptr[tran_tbl[k]];
            src_ptr += src_sample_size;
            dest_ptr += dest_sample_size;
        }
    }

    /* Update the ancillary information. */
    if (png_get_tRNS(png_ptr, info_ptr, NULL, NULL, &trans_color))
    {
        if (reductions & OPNG_REDUCE_16_TO_8)
        {
            if (trans_color->red % 257 == 0 &&
                trans_color->green % 257 == 0 &&
                trans_color->blue % 257 == 0 &&
                trans_color->gray % 257 == 0)
            {
                trans_color->red &= 255;
                trans_color->green &= 255;
                trans_color->blue &= 255;
                trans_color->gray &= 255;
            }
            else
            {
                /* 16-bit tRNS in 8-bit samples: all pixels are 100% opaque. */
                png_free_data(png_ptr, info_ptr, PNG_FREE_TRNS, -1);
                png_set_invalid(png_ptr, info_ptr, PNG_INFO_tRNS);
            }
        }
        if (reductions & OPNG_REDUCE_RGB_TO_GRAY)
        {
            if (trans_color->red == trans_color->green ||
                trans_color->red == trans_color->blue)
                trans_color->gray = trans_color->red;
            else
            {
                /* Non-gray tRNS in grayscale image: all pixels are 100% opaque. */
                png_free_data(png_ptr, info_ptr, PNG_FREE_TRNS, -1);
                png_set_invalid(png_ptr, info_ptr, PNG_INFO_tRNS);
            }
        }
    }
#ifdef PNG_bKGD_SUPPORTED
    if (png_get_bKGD(png_ptr, info_ptr, &background))
    {
        if (reductions & OPNG_REDUCE_16_TO_8)
        {
            background->red &= 255;
            background->green &= 255;
            background->blue &= 255;
            background->gray &= 255;
        }
        if (reductions & OPNG_REDUCE_RGB_TO_GRAY)
            background->gray = background->red;
    }
#endif
#ifdef PNG_sBIT_SUPPORTED
    if (png_get_sBIT(png_ptr, info_ptr, &sig_bits))
    {
        if (reductions & OPNG_REDUCE_16_TO_8)
        {
            if (sig_bits->red > 8)
                sig_bits->red = 8;
            if (sig_bits->green > 8)
                sig_bits->green = 8;
            if (sig_bits->blue > 8)
                sig_bits->blue = 8;
            if (sig_bits->gray > 8)
                sig_bits->gray = 8;
            if (sig_bits->alpha > 8)
                sig_bits->alpha = 8;
        }
        if (reductions & OPNG_REDUCE_RGB_TO_GRAY)
        {
            png_byte max_sig_bits = sig_bits->red;
            if (max_sig_bits < sig_bits->green)
                max_sig_bits = sig_bits->green;
            if (max_sig_bits < sig_bits->blue)
                max_sig_bits = sig_bits->blue;
            sig_bits->gray = max_sig_bits;
        }
    }
#endif

    /* Update the image information. */
    png_set_IHDR(png_ptr, info_ptr, width, height,
        dest_bit_depth, dest_color_type,
        interlace_type, compression_type, filter_type);

    return reductions;
}

/*
 * Reduce the bit depth of a palette image to the lowest possible value.
 * The parameter reductions should contain OPNG_REDUCE_8_TO_4_2_1.
 * The function returns OPNG_REDUCE_8_TO_4_2_1 if successful.
 */
static png_uint_32  opng_reduce_palette_bits(png_structp png_ptr, png_infop info_ptr,
    png_uint_32 reductions)
{
    png_bytepp row_ptr;
    png_bytep src_sample_ptr, dest_sample_ptr;
    png_uint_32 width, height;
    int color_type, interlace_type, compression_type, filter_type;
    int src_bit_depth, dest_bit_depth;
    unsigned int src_mask_init, src_mask, src_shift, dest_shift;
    unsigned int sample, dest_buf;
    png_colorp palette;
    int num_palette;
    png_uint_32 i, j;

    opng_debug(1, "in opng_reduce_palette_bits");

    /* Check if the reduction applies. */
    if (!(reductions & OPNG_REDUCE_8_TO_4_2_1))
        return OPNG_REDUCE_NONE;
    png_get_IHDR(png_ptr, info_ptr, &width, &height, &src_bit_depth,
        &color_type, &interlace_type, &compression_type, &filter_type);
    if (color_type != PNG_COLOR_TYPE_PALETTE)
        return OPNG_REDUCE_NONE;
    if (!png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette))
        num_palette = 0;

    /* Find the smallest possible bit depth. */
    if (num_palette > 16)
        return OPNG_REDUCE_NONE;
    else if (num_palette > 4)  /* 5 .. 16 entries */
        dest_bit_depth = 4;
    else if (num_palette > 2)  /* 3 or 4 entries */
        dest_bit_depth = 2;
    else  /* 1 or 2 entries */
    {
        OPNG_ASSERT(num_palette > 0);
        dest_bit_depth = 1;
    }

    if (src_bit_depth <= dest_bit_depth)
    {
        OPNG_ASSERT(src_bit_depth == dest_bit_depth);
        return OPNG_REDUCE_NONE;
    }

    /* Iterate through all sample values. */
    row_ptr = png_get_rows(png_ptr, info_ptr);
    if (src_bit_depth == 8)
    {
        for (i = 0; i < height; ++i, ++row_ptr)
        {
            src_sample_ptr = dest_sample_ptr = *row_ptr;
            dest_shift = 8;
            dest_buf = 0;
            for (j = 0; j < width; ++j)
            {
                dest_shift -= dest_bit_depth;
                if (dest_shift > 0)
                    dest_buf |= *src_sample_ptr << dest_shift;
                else
                {
                    *dest_sample_ptr++ = (png_byte)(dest_buf | *src_sample_ptr);
                    dest_shift = 8;
                    dest_buf = 0;
                }
                ++src_sample_ptr;
            }
            if (dest_shift != 0)
                *dest_sample_ptr = (png_byte)dest_buf;
        }
    }
    else  /* src_bit_depth < 8 */
    {
        src_mask_init = (1 << (8 + src_bit_depth)) - (1 << 8);
        for (i = 0; i < height; ++i, ++row_ptr)
        {
            src_sample_ptr = dest_sample_ptr = *row_ptr;
            src_shift = dest_shift = 8;
            src_mask = src_mask_init;
            dest_buf = 0;
            for (j = 0; j < width; ++j)
            {
                src_shift -= src_bit_depth;
                src_mask >>= src_bit_depth;
                sample = (*src_sample_ptr & src_mask) >> src_shift;
                dest_shift -= dest_bit_depth;
                if (dest_shift > 0)
                    dest_buf |= sample << dest_shift;
                else
                {
                    *dest_sample_ptr++ = (png_byte)(dest_buf | sample);
                    dest_shift = 8;
                    dest_buf = 0;
                }
                if (src_shift == 0)
                {
                    src_shift = 8;
                    src_mask = src_mask_init;
                    ++src_sample_ptr;
                }
            }
            if (dest_shift != 0)
                *dest_sample_ptr = (png_byte)dest_buf;
        }
    }

    /* Update the image information. */
    png_set_IHDR(png_ptr, info_ptr, width, height, dest_bit_depth,
        color_type, interlace_type, compression_type, filter_type);
    return OPNG_REDUCE_8_TO_4_2_1;
}

/*
 * Reduce the image type from grayscale(+alpha) or RGB(+alpha) to palette,
 * if possible.
 * The parameter reductions indicates the intended reductions.
 * The function returns the successful reductions.
 */
static png_uint_32  opng_reduce_to_palette(png_structp png_ptr, png_infop info_ptr,
    png_uint_32 reductions)
{
    png_uint_32 result;
    png_row_info row_info;
    png_bytepp row_ptr;
    png_bytep sample_ptr, alpha_row;
    png_uint_32 height, width;
    int color_type, interlace_type, compression_type, filter_type;
    int src_bit_depth, dest_bit_depth, channels;
    png_color palette[256];
    png_byte trans_alpha[256];
    png_color_16p trans_color;
    int num_palette, num_trans, index;
    unsigned int gray, red, green, blue, alpha;
    unsigned int prev_gray, prev_red, prev_green, prev_blue, prev_alpha;
#ifdef PNG_bKGD_SUPPORTED
    png_color_16p background;
#endif
    png_uint_32 i, j;

    opng_debug(1, "in opng_reduce_to_palette");

    png_get_IHDR(png_ptr, info_ptr, &width, &height, &src_bit_depth,
        &color_type, &interlace_type, &compression_type, &filter_type);
    if (src_bit_depth != 8)
        return OPNG_REDUCE_NONE;  /* nothing is done in this case */
    OPNG_ASSERT(!(color_type & PNG_COLOR_MASK_PALETTE));

    row_ptr = png_get_rows(png_ptr, info_ptr);
    channels = png_get_channels(png_ptr, info_ptr);
    alpha_row = (png_bytep)png_malloc(png_ptr, width);

    row_info.width = width;
    row_info.rowbytes = 0;  /* not used */
    row_info.color_type = (png_byte)color_type;
    row_info.bit_depth = (png_byte)src_bit_depth;
    row_info.channels = (png_byte)channels;
    row_info.pixel_depth = 0;  /* not used */

    /* Analyze the possibility of this reduction. */
    num_palette = num_trans = 0;
    trans_color = NULL;
    png_get_tRNS(png_ptr, info_ptr, NULL, NULL, &trans_color);
    prev_gray = prev_red = prev_green = prev_blue = prev_alpha = 256;
    for (i = 0; i < height; ++i, ++row_ptr)
    {
        sample_ptr = *row_ptr;
        opng_get_alpha_row(&row_info, trans_color, *row_ptr, alpha_row);
        if (color_type & PNG_COLOR_MASK_COLOR)
        {
            for (j = 0; j < width; ++j, sample_ptr += channels)
            {
                red = sample_ptr[0];
                green = sample_ptr[1];
                blue = sample_ptr[2];
                alpha = alpha_row[j];
                /* Check the cache first. */
                if (red != prev_red || green != prev_green || blue != prev_blue ||
                    alpha != prev_alpha)
                {
                    prev_red = red;
                    prev_green = green;
                    prev_blue = blue;
                    prev_alpha = alpha;
                    if (opng_insert_palette_entry(palette, &num_palette,
                        trans_alpha, &num_trans, 256,
                        red, green, blue, alpha, &index) < 0)  /* overflow */
                    {
                        OPNG_ASSERT(num_palette < 0);
                        i = height;  /* forced exit from outer loop */
                        break;
                    }
                }
            }
        }
        else  /* grayscale */
        {
            for (j = 0; j < width; ++j, sample_ptr += channels)
            {
                gray = sample_ptr[0];
                alpha = alpha_row[j];
                /* Check the cache first. */
                if (gray != prev_gray || alpha != prev_alpha)
                {
                    prev_gray = gray;
                    prev_alpha = alpha;
                    if (opng_insert_palette_entry(palette, &num_palette,
                        trans_alpha, &num_trans, 256,
                        gray, gray, gray, alpha, &index) < 0)  /* overflow */
                    {
                        OPNG_ASSERT(num_palette < 0);
                        i = height;  /* forced exit from outer loop */
                        break;
                    }
                }
            }
        }
    }
#ifdef PNG_bKGD_SUPPORTED
    if ((num_palette >= 0) && png_get_bKGD(png_ptr, info_ptr, &background))
    {
        /* bKGD has an alpha-agnostic palette entry. */
        if (color_type & PNG_COLOR_MASK_COLOR)
        {
            red = background->red;
            green = background->green;
            blue = background->blue;
        }
        else
            red = green = blue = background->gray;
        opng_insert_palette_entry(palette, &num_palette,
            trans_alpha, &num_trans, 256,
            red, green, blue, 256, &index);
        if (index >= 0)
            background->index = (png_byte)index;
    }
#endif

    /* Continue only if the uncompressed indexed image (pixels + PLTE + tRNS)
     * is smaller than the uncompressed RGB(A) image.
     * Casual overhead (headers, CRCs, etc.) is ignored.
     *
     * Compare:
     * num_pixels * (src_bit_depth * channels - dest_bit_depth) / 8
     * vs.
     * sizeof(PLTE) + sizeof(tRNS)
     */
    if (num_palette >= 0)
    {
        OPNG_ASSERT(num_palette > 0 && num_palette <= 256);
        OPNG_ASSERT(num_trans >= 0 && num_trans <= num_palette);
        if (num_palette <= 2)
            dest_bit_depth = 1;
        else if (num_palette <= 4)
            dest_bit_depth = 2;
        else if (num_palette <= 16)
            dest_bit_depth = 4;
        else
            dest_bit_depth = 8;
        /* Do the comparison in a way that does not cause overflow. */
        if (channels * 8 == dest_bit_depth ||
            (3 * num_palette + num_trans) * 8 / (channels * 8 - dest_bit_depth)
            / width / height >= 1)
            num_palette = -1;
    }

    if (num_palette < 0)  /* can't reduce */
    {
        png_free(png_ptr, alpha_row);
        return OPNG_REDUCE_NONE;
    }

    /* Reduce. */
    row_ptr = png_get_rows(png_ptr, info_ptr);
    index = -1;
    prev_red = prev_green = prev_blue = prev_alpha = (unsigned int)(-1);
    for (i = 0; i < height; ++i, ++row_ptr)
    {
        sample_ptr = *row_ptr;
        opng_get_alpha_row(&row_info, trans_color, *row_ptr, alpha_row);
        if (color_type & PNG_COLOR_MASK_COLOR)
        {
            for (j = 0; j < width; ++j, sample_ptr += channels)
            {
                red = sample_ptr[0];
                green = sample_ptr[1];
                blue = sample_ptr[2];
                alpha = alpha_row[j];
                /* Check the cache first. */
                if (red != prev_red || green != prev_green || blue != prev_blue ||
                    alpha != prev_alpha)
                {
                    prev_red = red;
                    prev_green = green;
                    prev_blue = blue;
                    prev_alpha = alpha;
                    if (opng_insert_palette_entry(palette, &num_palette,
                        trans_alpha, &num_trans, 256,
                        red, green, blue, alpha, &index) != 0)
                        index = -1;  /* this should not happen */
                }
                OPNG_ASSERT(index >= 0);
                (*row_ptr)[j] = (png_byte)index;
            }
        }
        else  /* grayscale */
        {
            for (j = 0; j < width; ++j, sample_ptr += channels)
            {
                gray = sample_ptr[0];
                alpha = alpha_row[j];
                /* Check the cache first. */
                if (gray != prev_gray || alpha != prev_alpha)
                {
                    prev_gray = gray;
                    prev_alpha = alpha;
                    if (opng_insert_palette_entry(palette, &num_palette,
                        trans_alpha, &num_trans, 256,
                        gray, gray, gray, alpha, &index) != 0)
                        index = -1;  /* this should not happen */
                }
                OPNG_ASSERT(index >= 0);
                (*row_ptr)[j] = (png_byte)index;
            }
        }
    }

    /* Update the image information. */
    png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_PALETTE,
        interlace_type, compression_type, filter_type);
    png_set_PLTE(png_ptr, info_ptr, palette, num_palette);
    if (num_trans > 0)
        png_set_tRNS(png_ptr, info_ptr, trans_alpha, num_trans, NULL);
    /* bKGD (if present) is automatically updated. */

    png_free(png_ptr, alpha_row);

    result = OPNG_REDUCE_RGB_TO_PALETTE;
    if (reductions & OPNG_REDUCE_8_TO_4_2_1)
        result |= opng_reduce_palette_bits(png_ptr, info_ptr, reductions);
    return result;
}

/*
 * Analyze the usage of samples.
 * The output value usage_map[n] indicates whether the sample n
 * is used. The usage_map[] array must have 256 entries.
 * The function requires a valid bit depth between 1 and 8.
 */
static void opng_analyze_sample_usage(png_structp png_ptr, png_infop info_ptr,
    png_bytep usage_map)
{
    png_bytepp row_ptr;
    png_bytep sample_ptr;
    png_uint_32 width, height;
    int bit_depth, init_shift, init_mask, shift, mask;
#ifdef PNG_bKGD_SUPPORTED
    png_color_16p background;
#endif
    png_uint_32 i, j;

    opng_debug(1, "in opng_analyze_sample_usage");

    height = png_get_image_height(png_ptr, info_ptr);
    width = png_get_image_width(png_ptr, info_ptr);
    bit_depth = png_get_bit_depth(png_ptr, info_ptr);
    row_ptr = png_get_rows(png_ptr, info_ptr);

    /* Initialize the output entries with 0. */
    memset(usage_map, 0, 256);

    /* Iterate through all sample values. */
    if (bit_depth == 8)
    {
        for (i = 0; i < height; ++i, ++row_ptr)
        {
            for (j = 0, sample_ptr = *row_ptr; j < width; ++j, ++sample_ptr)
                usage_map[*sample_ptr] = 1;
        }
    }
    else
    {
        OPNG_ASSERT(bit_depth < 8);
        init_shift = 8 - bit_depth;
        init_mask = (1 << 8) - (1 << init_shift);
        for (i = 0; i < height; ++i, ++row_ptr)
        {
            for (j = 0, sample_ptr = *row_ptr; j < width; ++sample_ptr)
            {
                mask = init_mask;
                shift = init_shift;
                do
                {
                    usage_map[(*sample_ptr & mask) >> shift] = 1;
                    mask >>= bit_depth;
                    shift -= bit_depth;
                    ++j;
                } while (mask > 0 && j < width);
            }
        }
    }

#ifdef PNG_bKGD_SUPPORTED
    /* bKGD also counts as a used sample. */
    if (png_get_bKGD(png_ptr, info_ptr, &background))
        usage_map[background->index] = 1;
#endif
}

/*
 * Reduce the palette. (Only the fast method is implemented.)
 * The parameter reductions indicates the intended reductions.
 * The function returns the successful reductions.
 */
static png_uint_32  opng_reduce_palette(png_structp png_ptr, png_infop info_ptr,
    png_uint_32 reductions)
{
    png_uint_32 result;
    png_colorp palette;
    png_bytep trans_alpha;
    png_bytepp row_ptr;
    png_uint_32 width, height;
    int bit_depth, color_type, interlace_type, compression_type, filter_type;
    int num_palette, num_trans;
    int last_color_index, last_trans_index;
    png_byte crt_trans_value, last_trans_value;
    png_byte is_used[256];
    png_color_16 gray_trans;
    int is_gray;
#ifdef PNG_bKGD_SUPPORTED
    png_color_16p background;
#endif
#ifdef PNG_hIST_SUPPORTED
    png_uint_16p hist;
#endif
#ifdef PNG_sBIT_SUPPORTED
    png_color_8p sig_bits;
#endif
    png_uint_32 i, j;
    int k;

    opng_debug(1, "in opng_reduce_palette");

    result = OPNG_REDUCE_NONE;

    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth,
        &color_type, &interlace_type, &compression_type, &filter_type);
    row_ptr = png_get_rows(png_ptr, info_ptr);
    if (!png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette))
    {
        palette = NULL;
        num_palette = 0;
    }
    if (!png_get_tRNS(png_ptr, info_ptr, &trans_alpha, &num_trans, NULL))
    {
        trans_alpha = NULL;
        num_trans = 0;
    }
    else
        OPNG_ASSERT(trans_alpha != NULL && num_trans > 0);

    opng_analyze_sample_usage(png_ptr, info_ptr, is_used);
    /* Palette-to-gray does not work (yet) if the bit depth is below 8. */
    is_gray = (reductions & OPNG_REDUCE_PALETTE_TO_GRAY) && (bit_depth == 8);
    last_color_index = last_trans_index = -1;
    for (k = 0; k < 256; ++k)
    {
        if (!is_used[k])
            continue;
        last_color_index = k;
        if (k < num_trans && trans_alpha[k] < 255)
            last_trans_index = k;
        if (is_gray)
            if (palette[k].red != palette[k].green ||
                palette[k].red != palette[k].blue)
                is_gray = 0;
    }
    OPNG_ASSERT(last_color_index >= 0);
    OPNG_ASSERT(last_color_index >= last_trans_index);

    /* Check the integrity of PLTE and tRNS. */
    if (last_color_index >= num_palette)
    {
        png_warning(png_ptr, "Too few colors in PLTE");
        /* Fix the palette by adding blank entries at the end. */
        opng_realloc_PLTE(png_ptr, info_ptr, last_color_index + 1);
        png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette);
        OPNG_ASSERT(num_palette == last_color_index + 1);
        result |= OPNG_REDUCE_REPAIR;
    }
    if (num_trans > num_palette)
    {
        png_warning(png_ptr, "Too many alpha values in tRNS");
        /* Transparency will be fixed further below. */
        result |= OPNG_REDUCE_REPAIR;
    }

    /* Check if tRNS can be reduced to grayscale. */
    if (is_gray && last_trans_index >= 0)
    {
        gray_trans.gray = palette[last_trans_index].red;
        last_trans_value = trans_alpha[last_trans_index];
        for (k = 0; k <= last_color_index; ++k)
        {
            if (!is_used[k])
                continue;
            if (k <= last_trans_index)
            {
                crt_trans_value = trans_alpha[k];
                /* Cannot reduce if different colors have transparency. */
                if (crt_trans_value < 255 && palette[k].red != gray_trans.gray)
                {
                    is_gray = 0;
                    break;
                }
            }
            else
                crt_trans_value = 255;
            /* Cannot reduce if same color has multiple transparency levels. */
            if (palette[k].red == gray_trans.gray &&
                crt_trans_value != last_trans_value)
            {
                is_gray = 0;
                break;
            }
        }
    }

    /* Remove tRNS if it is entirely sterile. */
    if (num_trans > 0 && last_trans_index < 0)
    {
        num_trans = 0;
        png_free_data(png_ptr, info_ptr, PNG_FREE_TRNS, -1);
        png_set_invalid(png_ptr, info_ptr, PNG_INFO_tRNS);
        result |= OPNG_REDUCE_PALETTE_FAST;
    }

    if (reductions & OPNG_REDUCE_PALETTE_FAST)
    {
        if (num_palette != last_color_index + 1)
        {
            /* Reduce PLTE. */
            /* hIST is reduced automatically. */
            opng_realloc_PLTE(png_ptr, info_ptr, last_color_index + 1);
            png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette);
            OPNG_ASSERT(num_palette == last_color_index + 1);
            result |= OPNG_REDUCE_PALETTE_FAST;
        }

        if (num_trans > 0 && num_trans != last_trans_index + 1)
        {
            /* Reduce tRNS. */
            opng_realloc_tRNS(png_ptr, info_ptr, last_trans_index + 1);
            png_get_tRNS(png_ptr, info_ptr, &trans_alpha, &num_trans, NULL);
            OPNG_ASSERT(num_trans == last_trans_index + 1);
            result |= OPNG_REDUCE_PALETTE_FAST;
        }
    }

    if (reductions & OPNG_REDUCE_8_TO_4_2_1)
    {
        result |= opng_reduce_palette_bits(png_ptr, info_ptr, reductions);
        /* Refresh the image information. */
        bit_depth = png_get_bit_depth(png_ptr, info_ptr);
    }
    if ((bit_depth < 8) || !is_gray)
        return result;

    /* Reduce palette --> grayscale. */
    for (i = 0; i < height; ++i)
    {
        for (j = 0; j < width; ++j)
            row_ptr[i][j] = palette[row_ptr[i][j]].red;
    }

    /* Update the ancillary information. */
    if (num_trans > 0)
        png_set_tRNS(png_ptr, info_ptr, NULL, 0, &gray_trans);
#ifdef PNG_bKGD_SUPPORTED
    if (png_get_bKGD(png_ptr, info_ptr, &background))
        background->gray = palette[background->index].red;
#endif
#ifdef PNG_hIST_SUPPORTED
    if (png_get_hIST(png_ptr, info_ptr, &hist))
    {
        png_free_data(png_ptr, info_ptr, PNG_FREE_HIST, -1);
        png_set_invalid(png_ptr, info_ptr, PNG_INFO_hIST);
    }
#endif
#ifdef PNG_sBIT_SUPPORTED
    if (png_get_sBIT(png_ptr, info_ptr, &sig_bits))
    {
        png_byte max_sig_bits = sig_bits->red;
        if (max_sig_bits < sig_bits->green)
            max_sig_bits = sig_bits->green;
        if (max_sig_bits < sig_bits->blue)
            max_sig_bits = sig_bits->blue;
        sig_bits->gray = max_sig_bits;
    }
#endif

    /* Update the image information. */
    png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth,
        PNG_COLOR_TYPE_GRAY, interlace_type, compression_type, filter_type);
    png_free_data(png_ptr, info_ptr, PNG_FREE_PLTE, -1);
    png_set_invalid(png_ptr, info_ptr, PNG_INFO_PLTE);
    return OPNG_REDUCE_PALETTE_TO_GRAY;  /* ignore the former result */
}

/*
 * Reduce the image (bit depth + color type + palette) without
 * losing any information. The palette (if applicable) and the
 * image data must be present, e.g., by calling png_set_rows(),
 * or by loading IDAT.
 * The parameter reductions indicates the intended reductions.
 * The function returns the successful reductions.
 */
png_uint_32 PNGAPI
opng_reduce_image(png_structp png_ptr, png_infop info_ptr,
    png_uint_32 reductions)
{
    png_uint_32 result;
    int color_type;

    opng_debug(1, "in opng_reduce_image_type");

    if (!opng_validate_image(png_ptr, info_ptr))
    {
        png_warning(png_ptr,
            "Image reduction requires the presence of all critical information");
        return OPNG_REDUCE_NONE;
    }

    color_type = png_get_color_type(png_ptr, info_ptr);

    /* The reductions below must be applied in this particular order. */

    /* Try to reduce the high bits and color/alpha channels. */
    result = opng_reduce_bits(png_ptr, info_ptr, reductions);

    /* Try to reduce the palette image. */
    if (color_type == PNG_COLOR_TYPE_PALETTE &&
        (reductions &
            (OPNG_REDUCE_PALETTE_TO_GRAY |
                OPNG_REDUCE_PALETTE_FAST |
                OPNG_REDUCE_8_TO_4_2_1)))
        result |= opng_reduce_palette(png_ptr, info_ptr, reductions);

    /* Try to reduce RGB to palette or grayscale to palette. */
    if (((color_type & ~PNG_COLOR_MASK_ALPHA) == PNG_COLOR_TYPE_GRAY &&
        (reductions & OPNG_REDUCE_GRAY_TO_PALETTE)) ||
        ((color_type & ~PNG_COLOR_MASK_ALPHA) == PNG_COLOR_TYPE_RGB &&
            (reductions & OPNG_REDUCE_RGB_TO_PALETTE)))
    {
        if (!(result & OPNG_REDUCE_PALETTE_TO_GRAY))
            result |= opng_reduce_to_palette(png_ptr, info_ptr, reductions);
    }

    return result;
}

#endif /* OPNG_IMAGE_REDUCTIONS_SUPPORTED */

//-------------------------------------------------------


#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>








int PNGAPI pngx_read_image(png_structp png_ptr, png_infop info_ptr,
    png_const_charpp fmt_name_ptr,
    png_const_charpp fmt_long_name_ptr);

#define OPNG_OPTIM_LEVEL_DEFAULT    2
#define OPNG_OPTIM_LEVEL_MIN        0
#define OPNG_OPTIM_LEVEL_MAX        7

#define OPNG_COMPR_LEVEL_MIN        1
#define OPNG_COMPR_LEVEL_MAX        9
#define OPNG_COMPR_LEVEL_SET_MASK   ((1 << (9+1)) - (1 << 1))  /* 0x03fe */

#define OPNG_MEM_LEVEL_MIN          1
#define OPNG_MEM_LEVEL_MAX          9
#define OPNG_MEM_LEVEL_SET_MASK     ((1 << (9+1)) - (1 << 1))  /* 0x03fe */

#define OPNG_STRATEGY_MIN           0
#define OPNG_STRATEGY_MAX           3
#define OPNG_STRATEGY_SET_MASK      ((1 << (3+1)) - (1 << 0))  /* 0x000f */

#define OPNG_FILTER_MIN             0
#define OPNG_FILTER_MAX             5
#define OPNG_FILTER_SET_MASK        ((1 << (5+1)) - (1 << 0))  /* 0x003f */


static const struct opng_preset
{
    const char* compr_level;
    const char* mem_level;
    const char* strategy;
    const char* filter;
} g_presets[OPNG_OPTIM_LEVEL_MAX + 1] =
{
    /*  { -zc    -zm    -zs   -f    }  */
        { "",    "",    "",   ""    },  /* -o0 */
        { "",    "",    "",   ""    },  /* -o1 */
        { "9",   "8",   "0-", "0,5" },  /* -o2 */
        { "9",   "8-9", "0-", "0,5" },  /* -o3 */
        { "9",   "8",   "0-", "0-"  },  /* -o4 */
        { "9",   "8-9", "0-", "0-"  },  /* -o5 */
        { "1-9", "8",   "0-", "0-"  },  /* -o6 */
        { "1-9", "8-9", "0-", "0-"  }   /* -o7 */
};

/*
 * The filter table.
 */
static const int g_filter_table[OPNG_FILTER_MAX + 1] =
{
    PNG_FILTER_NONE,   /* -f0 */
    PNG_FILTER_SUB,    /* -f1 */
    PNG_FILTER_UP,     /* -f2 */
    PNG_FILTER_AVG,    /* -f3 */
    PNG_FILTER_PAETH,  /* -f4 */
    PNG_ALL_FILTERS    /* -f5 */
};

/*
 * Status flags.
 */
enum
{
    INPUT_IS_PNG_FILE = 0x0001,
    INPUT_HAS_PNG_DATASTREAM = 0x0002,
    INPUT_HAS_PNG_SIGNATURE = 0x0004,
    INPUT_HAS_DIGITAL_SIGNATURE = 0x0008,
    INPUT_HAS_MULTIPLE_IMAGES = 0x0010,
    INPUT_HAS_APNG = 0x0020,
    INPUT_HAS_STRIPPED_DATA = 0x0040,
    INPUT_HAS_JUNK = 0x0080,
    INPUT_HAS_ERRORS = 0x0100,
    OUTPUT_NEEDS_NEW_FILE = 0x1000,
    OUTPUT_NEEDS_NEW_IDAT = 0x2000,
    OUTPUT_HAS_ERRORS = 0x4000
};

/*
 * The chunks handled by OptiPNG.
 */
static const png_byte sig_PLTE[4] = { 0x50, 0x4c, 0x54, 0x45 };
static const png_byte sig_tRNS[4] = { 0x74, 0x52, 0x4e, 0x53 };
static const png_byte sig_IDAT[4] = { 0x49, 0x44, 0x41, 0x54 };
static const png_byte sig_IEND[4] = { 0x49, 0x45, 0x4e, 0x44 };
static const png_byte sig_bKGD[4] = { 0x62, 0x4b, 0x47, 0x44 };
static const png_byte sig_hIST[4] = { 0x68, 0x49, 0x53, 0x54 };
static const png_byte sig_sBIT[4] = { 0x73, 0x42, 0x49, 0x54 };
static const png_byte sig_dSIG[4] = { 0x64, 0x53, 0x49, 0x47 };
static const png_byte sig_acTL[4] = { 0x61, 0x63, 0x54, 0x4c };
static const png_byte sig_fcTL[4] = { 0x66, 0x63, 0x54, 0x4c };
static const png_byte sig_fdAT[4] = { 0x66, 0x64, 0x41, 0x54 };

struct opng_options
{
    /* General options. */
    int clobber;
    int fix;
    int force;
    int preserve;
    int quiet;

    /* Optimization options. */
    int interlace;
    int nb, nc, np, nz;
    int optim_level;
    opng_bitset_t compr_level_set;
    opng_bitset_t mem_level_set;
    opng_bitset_t strategy_set;
    opng_bitset_t filter_set;
    int window_bits;

    /* Editing options. */
    int snip;
    int strip_all;
};

/*
 * The optimization process.
 */
struct opng_process_struct
{
    unsigned int status;
    int num_iterations;
    opng_foffset_t in_datastream_offset;
    opng_fsize_t in_file_size, out_file_size;
    opng_fsize_t in_idat_size, out_idat_size;
    opng_fsize_t best_idat_size, max_idat_size;
    png_uint_32 in_plte_trns_size, out_plte_trns_size;
    png_uint_32 reductions;
    opng_bitset_t compr_level_set, mem_level_set, strategy_set, filter_set;
    int best_compr_level, best_mem_level, best_strategy, best_filter;
};

struct opng_image_struct
{
    png_uint_32 width;             /* IHDR */
    png_uint_32 height;
    int bit_depth;
    int color_type;
    int compression_type;
    int filter_type;
    int interlace_type;
    png_bytepp row_pointers;       /* IDAT */
    png_colorp palette;            /* PLTE */
    int num_palette;
    png_color_16p background_ptr;  /* bKGD */
    png_color_16 background;
    png_uint_16p hist;             /* hIST */
    png_color_8p sig_bit_ptr;      /* sBIT */
    png_color_8 sig_bit;
    png_bytep trans_alpha;         /* tRNS */
    int num_trans;
    png_color_16p trans_color_ptr;
    png_color_16 trans_color;
    png_unknown_chunkp unknowns;   /* everything else */
    int num_unknowns;
};


typedef  struct MyStruct
{
    opng_options option;
    opng_process_struct process;
    opng_image_struct image;

    FILE* in_file;
    FILE* out_file;
    png_infop read_info_ptr;

    int allow_crt_chunk;
    int crt_chunk_is_idat;
    opng_foffset_t crt_idat_offset;
    opng_fsize_t crt_idat_size;
    png_uint_32 crt_idat_crc;
}MyStruct;

/*
 * The optimization process limits.
 */
static const opng_fsize_t idat_size_max = PNG_UINT_31_MAX;
static const char* idat_size_max_string = "2GB";




/*
 * Internal debugging tool.
 */
#define OPNG_ENSURE(cond, msg) 

 /*
  * Size ratio display.
  */
static void
opng_print_fsize_ratio(opng_fsize_t num, opng_fsize_t denom)
{
#if OPNG_FSIZE_MAX <= ULONG_MAX
#define RATIO_TYPE struct opng_ulratio
#define RATIO_CONV_FN opng_ulratio_to_factor_string
#else
#define RATIO_TYPE struct opng_ullratio
#define RATIO_CONV_FN opng_ullratio_to_factor_string
#endif

    char buffer[32];
    RATIO_TYPE ratio;
    int result;

    ratio.num = num;
    ratio.denom = denom;
    result = RATIO_CONV_FN(buffer, sizeof(buffer), &ratio);
    printf("%s%s", buffer, (result > 0) ? "" : "...");

#undef RATIO_TYPE
#undef RATIO_CONV_FN
}

/*
 * Size change display.
 */
static void
opng_print_fsize_difference(opng_fsize_t init_size, opng_fsize_t final_size,
    int show_ratio)
{
    opng_fsize_t difference;
    int sign;

    if (init_size <= final_size)
    {
        sign = 0;
        difference = final_size - init_size;
    }
    else
    {
        sign = 1;
        difference = init_size - final_size;
    }

    if (difference == 0)
    {
        printf("no change");
        return;
    }
    if (difference == 1)
        printf("1 byte");
    else
        printf("%" OPNG_FSIZE_PRIu " bytes", difference);
    if (show_ratio && init_size > 0)
    {
        printf(" = ");
        opng_print_fsize_ratio(difference, init_size);
    }
    printf((sign == 0) ? " increase" : " decrease");
}

/*
 * Image info display.
 */
static void
opng_print_image_info(opng_image_struct* image, int show_dim, int show_depth, int show_type,
    int show_interlaced)
{
    static const int type_channels[8] = { 1, 0, 3, 1, 2, 0, 4, 0 };
    int channels, printed;

    printed = 0;
    if (show_dim)
    {
        printed = 1;
        printf("%lux%lu pixels",
            (unsigned long)image->width, (unsigned long)image->height);
    }
    if (show_depth)
    {
        if (printed)
            printf(", ");
        printed = 1;
        channels = type_channels[image->color_type & 7];
        if (channels != 1)
            printf("%dx%d bits/pixel", channels, image->bit_depth);
        else if (image->bit_depth != 1)
            printf("%d bits/pixel", image->bit_depth);
        else
            printf("1 bit/pixel");
    }
    if (show_type)
    {
        if (printed)
            printf(", ");
        printed = 1;
        if (image->color_type & PNG_COLOR_MASK_PALETTE)
        {
            if (image->num_palette == 1)
                printf("1 color");
            else
                printf("%d colors", image->num_palette);
            if (image->num_trans > 0)
                printf(" (%d transparent)", image->num_trans);
            printf(" in palette");
        }
        else
        {
            printf((image->color_type & PNG_COLOR_MASK_COLOR) ?
                "RGB" : "grayscale");
            if (image->color_type & PNG_COLOR_MASK_ALPHA)
                printf("+alpha");
            else if (image->trans_color_ptr != NULL)
                printf("+transparency");
        }
    }
    if (show_interlaced)
    {
        if (image->interlace_type != PNG_INTERLACE_NONE)
        {
            if (printed)
                printf(", ");
            printf("interlaced");
        }
    }
}

/*
 * Warning display.
 */
static void
opng_print_warning(const char* msg)
{
    printf("Warning: %s\n", msg);
}

/*
 * Error display.
 */
static void
opng_print_error(const char* msg)
{
    printf("Error: %s\n", msg);
}

/*
 * Warning handler.
 */
static void
opng_warning(png_structp png_ptr, png_const_charp msg)
{
    MyStruct* sss = (MyStruct*)png_get_io_ptr(png_ptr);
    opng_print_warning(msg);
}

/*
 * Error handler.
 */
static void
opng_error(png_structp png_ptr, png_const_charp msg)
{
}

/*
 * Memory deallocator.
 */
static void
opng_free(void* ptr)
{
    /* This deallocator must be compatible with libpng's memory allocation
     * routines, png_malloc() and png_free().
     * If those routines change, this one must be changed accordingly.
     */
    free(ptr);
}

/*
 * IDAT size checker.
 */
static void
opng_check_idat_size(opng_fsize_t size)
{
    if (size > idat_size_max) {
    }
}

/*
 * Chunk handler.
 */
static void
opng_set_keep_unknown_chunk(png_structp png_ptr,
    int keep, png_bytep chunk_type)
{
    png_byte chunk_name[5];

    /* Call png_set_keep_unknown_chunks() once per each chunk type only. */
    memcpy(chunk_name, chunk_type, 4);
    chunk_name[4] = 0;
    if (!png_handle_as_unknown(png_ptr, chunk_name))
        png_set_keep_unknown_chunks(png_ptr, keep, chunk_name, 1);
}

/*
 * Chunk categorization.
 */
static int
opng_is_image_chunk(png_bytep chunk_type)
{
    if ((chunk_type[0] & 0x20) == 0)
        return 1;
    /* Although tRNS is listed as ancillary in the PNG specification, it stores
     * alpha samples, which is critical information. For example, tRNS cannot
     * be generally ignored when rendering animations.
     * Operations claimed to be lossless must treat tRNS as a critical chunk.
     */
    if (memcmp(chunk_type, sig_tRNS, 4) == 0)
        return 1;
    return 0;
}

/*
 * Chunk categorization.
 */
static int
opng_is_apng_chunk(png_bytep chunk_type)
{
    if (memcmp(chunk_type, sig_acTL, 4) == 0 ||
        memcmp(chunk_type, sig_fcTL, 4) == 0 ||
        memcmp(chunk_type, sig_fdAT, 4) == 0)
        return 1;
    return 0;
}

/*
 * Chunk filter.
 */
static int opng_allow_chunk(opng_options* option, png_bytep chunk_type)
{
    /* Always allow critical chunks and tRNS. */
    if (opng_is_image_chunk(chunk_type))
        return 1;
    /* Block all the other chunks if requested. */
    if (option->strip_all)
        return 0;
    /* Always block the digital signature chunks. */
    if (memcmp(chunk_type, sig_dSIG, 4) == 0)
        return 0;
    /* Block the APNG chunks when snipping. */
    if (option->snip && opng_is_apng_chunk(chunk_type))
        return 0;
    /* Allow all the other chunks. */
    return 1;
}

/*
 * Chunk handler.
 */
static void
opng_handle_chunk(png_structp png_ptr, png_bytep chunk_type)
{

    MyStruct* sss = (MyStruct*)png_get_io_ptr(png_ptr);

    int keep;

    if (opng_is_image_chunk(chunk_type))
        return;

    if (sss->option.strip_all)
    {
        sss->process.status |= INPUT_HAS_STRIPPED_DATA | INPUT_HAS_JUNK;
        opng_set_keep_unknown_chunk(png_ptr,
            PNG_HANDLE_CHUNK_NEVER, chunk_type);
        return;
    }

    /* Let libpng handle bKGD, hIST and sBIT. */
    if (memcmp(chunk_type, sig_bKGD, 4) == 0 ||
        memcmp(chunk_type, sig_hIST, 4) == 0 ||
        memcmp(chunk_type, sig_sBIT, 4) == 0)
        return;

    /* Everything else is handled as unknown by libpng. */
    keep = PNG_HANDLE_CHUNK_ALWAYS;
    if (memcmp(chunk_type, sig_dSIG, 4) == 0)
    {
        /* Recognize dSIG, but let libpng handle it as unknown. */
        sss->process.status |= INPUT_HAS_DIGITAL_SIGNATURE;
    }
    else if (opng_is_apng_chunk(chunk_type))
    {
        /* Recognize APNG, but let libpng handle it as unknown. */
        sss->process.status |= INPUT_HAS_APNG;
        if (memcmp(chunk_type, sig_fdAT, 4) == 0)
            sss->process.status |= INPUT_HAS_MULTIPLE_IMAGES;
        if (sss->option.snip)
        {
            sss->process.status |= INPUT_HAS_JUNK;
            keep = PNG_HANDLE_CHUNK_NEVER;
        }
    }
    opng_set_keep_unknown_chunk(png_ptr, keep, chunk_type);
}

/*
 * Initialization for input handler.
 */
static void
opng_init_read_data(void)
{
    /* The relevant process data members are set to zero,
     * and nothing else needs to be done at this moment.
     */
}

/*
 * Initialization for output handler.
 */
static void
opng_init_write_data(opng_process_struct* process)
{
    process->out_file_size = 0;
    process->out_plte_trns_size = 0;
    process->out_idat_size = 0;
}



/*
 * Input handler.
 */
static void
opng_read_data(png_structp png_ptr, png_bytep data, size_t length)
{
    MyStruct* sss = (MyStruct*)png_get_io_ptr(png_ptr);
    FILE* stream = (FILE*)sss->in_file;

    int io_state = pngx_get_io_state(png_ptr);
    int io_state_loc = io_state & PNGX_IO_MASK_LOC;
    png_bytep chunk_sig;

    /* Read the data. */
    if (fread(data, 1, length, stream) != length)
        png_error(png_ptr,
            "Can't read the input file or unexpected end of file");

    if (sss->process.in_file_size == 0)  /* first piece of PNG data */
    {
        OPNG_ENSURE(length == 8, "PNG I/O must start with the first 8 bytes");
        sss->process.in_datastream_offset = opng_ftello(stream) - 8;
        sss->process.status |= INPUT_HAS_PNG_DATASTREAM;
        if (io_state_loc == PNGX_IO_SIGNATURE)
            sss->process.status |= INPUT_HAS_PNG_SIGNATURE;
        if (sss->process.in_datastream_offset == 0)
            sss->process.status |= INPUT_IS_PNG_FILE;
        else if (sss->process.in_datastream_offset < 0)
            png_error(png_ptr,
                "Can't get the file-position indicator in input file");
        sss->process.in_file_size = (opng_fsize_t)sss->process.in_datastream_offset;
    }
    sss->process.in_file_size += length;

    /* Handle the OptiPNG-specific events. */
    OPNG_ENSURE((io_state & PNGX_IO_READING) && (io_state_loc != 0),
        "Incorrect info in png_ptr->io_state");
    if (io_state_loc == PNGX_IO_CHUNK_HDR)
    {
        /* In libpng 1.4.x and later, the chunk length and the chunk name
         * are serialized in a single operation. This is also ensured by
         * the opngio add-on for libpng 1.2.x and earlier.
         */
        OPNG_ENSURE(length == 8, "Reading chunk header, expecting 8 bytes");
        chunk_sig = data + 4;

        if (memcmp(chunk_sig, sig_IDAT, 4) == 0)
        {
            OPNG_ENSURE(png_ptr == read_ptr, "Incorrect I/O handler setup");
            if (png_get_rows(png_ptr, sss->read_info_ptr) == NULL)  /* 1st IDAT */
            {
                OPNG_ENSURE(process.in_idat_size == 0,
                    "Found IDAT with no rows");
                /* Allocate the rows here, bypassing libpng.
                 * This allows to initialize the contents and perform recovery
                 * in case of a premature EOF.
                 */
                if (png_get_image_height(png_ptr, sss->read_info_ptr) == 0)
                    return;  /* premature IDAT; an error will occur later */
                OPNG_ENSURE(pngx_malloc_rows(read_ptr, read_info_ptr,
                    0) != NULL,
                    "Failed allocation of image rows; "
                    "unsafe libpng allocator");
                png_data_freer(png_ptr, sss->read_info_ptr,
                    PNG_USER_WILL_FREE_DATA, PNG_FREE_ROWS);
            }
            else
            {
                /* There is split IDAT overhead. Join IDATs. */
                sss->process.status |= INPUT_HAS_JUNK;
            }
            sss->process.in_idat_size += png_get_uint_32(data);
        }
        else if (memcmp(chunk_sig, sig_PLTE, 4) == 0 ||
            memcmp(chunk_sig, sig_tRNS, 4) == 0)
        {
            /* Add the chunk overhead (header + CRC) to the data size. */
            sss->process.in_plte_trns_size += png_get_uint_32(data) + 12;
        }
        else
            opng_handle_chunk(png_ptr, chunk_sig);
    }
    else if (io_state_loc == PNGX_IO_CHUNK_CRC)
    {
        OPNG_ENSURE(length == 4, "Reading chunk CRC, expecting 4 bytes");
    }
}

/*
 * Output handler.
 */
static void opng_write_data(png_structp png_ptr, png_bytep data, size_t length)
{
    MyStruct* sss = (MyStruct*)png_get_io_ptr(png_ptr);
    FILE* stream = (FILE*)sss->out_file;




    int io_state = pngx_get_io_state(png_ptr);
    int io_state_loc = io_state & PNGX_IO_MASK_LOC;
    png_bytep chunk_sig;
    png_byte buf[4];

    /* Handle the OptiPNG-specific events. */
    if (io_state_loc == PNGX_IO_CHUNK_HDR)
    {
        OPNG_ENSURE(length == 8, "Writing chunk header, expecting 8 bytes");
        chunk_sig = data + 4;
        sss->allow_crt_chunk = opng_allow_chunk(&sss->option, chunk_sig);
        if (memcmp(chunk_sig, sig_IDAT, 4) == 0)
        {
            sss->crt_chunk_is_idat = 1;
            sss->process.out_idat_size += png_get_uint_32(data);
            /* Abandon the trial if IDAT is bigger than the maximum allowed. */
            if (stream == NULL)
            {
                if (sss->process.out_idat_size > sss->process.max_idat_size)
                    return;  /* early interruption, not an error */
            }
        }
        else  /* not IDAT */
        {
            sss->crt_chunk_is_idat = 0;
            if (memcmp(chunk_sig, sig_PLTE, 4) == 0 ||
                memcmp(chunk_sig, sig_tRNS, 4) == 0)
            {
                /* Add the chunk overhead (header + CRC) to the data size. */
                sss->process.out_plte_trns_size += png_get_uint_32(data) + 12;
            }
        }
    }
    else if (io_state_loc == PNGX_IO_CHUNK_CRC)
    {
        OPNG_ENSURE(length == 4, "Writing chunk CRC, expecting 4 bytes");
    }

    /* Exit early if this is only a trial. */
    if (stream == NULL)
        return;

    /* Continue only if the current chunk type is allowed. */
    if (io_state_loc != PNGX_IO_SIGNATURE && !sss->allow_crt_chunk)
        return;

    /* Here comes an elaborate way of writing the data, in which all IDATs
     * are joined into a single chunk.
     * Normally, the user-supplied I/O routines are not so complicated.
     */
    switch (io_state_loc)
    {
    case PNGX_IO_CHUNK_HDR:
        if (sss->crt_chunk_is_idat)
        {
            if (sss->crt_idat_offset == 0)
            {
                /* This is the header of the first IDAT. */
                sss->crt_idat_offset = opng_ftello(stream);
                /* Try guessing the size of the final (joined) IDAT. */
                if (sss->process.best_idat_size > 0)
                {
                    /* The guess is expected to be right. */
                    sss->crt_idat_size = sss->process.best_idat_size;
                }
                else
                {
                    /* The guess could be wrong.
                     * The size of the final IDAT will be revised.
                     */
                    sss->crt_idat_size = length;
                }
                png_save_uint_32(data, (png_uint_32)sss->crt_idat_size);
                /* Start computing the CRC of the final IDAT. */
                sss->crt_idat_crc = crc32(0, sig_IDAT, 4);
            }
            else
            {
                /* This is not the first IDAT. Do not write its header. */
                return;
            }
        }
        else
        {
            if (sss->crt_idat_offset != 0)
            {
                /* This is the header of the first chunk after IDAT.
                 * Finalize IDAT before resuming the normal operation.
                 */
                png_save_uint_32(buf, sss->crt_idat_crc);
                if (fwrite(buf, 1, 4, stream) != 4)
                    io_state = 0;  /* error */
                sss->process.out_file_size += 4;
                if (sss->process.out_idat_size != sss->crt_idat_size)
                {
                    /* The IDAT size has not been guessed correctly.
                     * It must be updated in a non-streamable way.
                     */
                    OPNG_ENSURE(g_process.best_idat_size == 0,
                        "Wrong guess of the output IDAT size");
                    opng_check_idat_size(sss->process.out_idat_size);
                    png_save_uint_32(buf, (png_uint_32)sss->process.out_idat_size);
                    if (opng_fwriteo(stream, sss->crt_idat_offset, SEEK_SET,
                        buf, 4) != 4)
                        io_state = 0;  /* error */
                }
                if (io_state == 0)
                    png_error(png_ptr, "Can't finalize IDAT");
                sss->crt_idat_offset = 0;
            }
        }
        break;
    case PNGX_IO_CHUNK_DATA:
        if (sss->crt_chunk_is_idat)
            sss->crt_idat_crc = crc32(sss->crt_idat_crc, data, length);
        break;
    case PNGX_IO_CHUNK_CRC:
        if ( sss->crt_chunk_is_idat)
        {
            /* Defer writing until the first non-IDAT occurs. */
            return;
        }
        break;
    }

    /* Write the data. */
    if (fwrite(data, 1, length, stream) != length)
        png_error(png_ptr, "Can't write the output file");
    sss->process.out_file_size += length;
}

/*
 * Image info initialization.
 */
static void
opng_clear_image_info(MyStruct* ms)
{
    memset(&ms->image, 0, sizeof(opng_image_struct));
}

/*
 * Image info transfer.
 */
static void
opng_load_image_info(png_structp png_ptr, png_infop info_ptr, int load_meta)
{
    MyStruct* sss = (MyStruct*)png_get_io_ptr(png_ptr);
    memset(&sss->image, 0, sizeof(opng_image_struct));

    png_get_IHDR(png_ptr, info_ptr,
        &sss->image.width, &sss->image.height, &sss->image.bit_depth,
        &sss->image.color_type, &sss->image.interlace_type,
        &sss->image.compression_type, &sss->image.filter_type);
    sss->image.row_pointers = png_get_rows(png_ptr, info_ptr);
    png_get_PLTE(png_ptr, info_ptr, &sss->image.palette, &sss->image.num_palette);
    /* Transparency is not considered metadata, although tRNS is ancillary.
     * See the comment in opng_is_image_chunk() above.
     */
    if (png_get_tRNS(png_ptr, info_ptr,
        &sss->image.trans_alpha,
        &sss->image.num_trans, &sss->image.trans_color_ptr))
    {
        /* Double copying (pointer + value) is necessary here
         * due to an inconsistency in the libpng design.
         */
        if (sss->image.trans_color_ptr != NULL)
        {
            sss->image.trans_color = *sss->image.trans_color_ptr;
            sss->image.trans_color_ptr = &sss->image.trans_color;
        }
    }

    if (!load_meta)
        return;

    if (png_get_bKGD(png_ptr, info_ptr, &sss->image.background_ptr))
    {
        /* Same problem as in tRNS. */
        sss->image.background = *sss->image.background_ptr;
        sss->image.background_ptr = &sss->image.background;
    }
    png_get_hIST(png_ptr, info_ptr, &sss->image.hist);
    if (png_get_sBIT(png_ptr, info_ptr, &sss->image.sig_bit_ptr))
    {
        /* Same problem as in tRNS. */
        sss->image.sig_bit = *sss->image.sig_bit_ptr;
        sss->image.sig_bit_ptr = &sss->image.sig_bit;
    }
    sss->image.num_unknowns =
        png_get_unknown_chunks(png_ptr, info_ptr, &sss->image.unknowns);
}

/*
 * Image info transfer.
 */
static void
opng_store_image_info(MyStruct* sss, png_structp png_ptr, png_infop info_ptr, int store_meta)
{
    int i;

    OPNG_ENSURE(image->row_pointers != NULL, "No info in image");

    png_set_IHDR(png_ptr, info_ptr,
        sss->image.width, sss->image.height, sss->image.bit_depth,
        sss->image.color_type, sss->image.interlace_type,
        sss->image.compression_type, sss->image.filter_type);
    png_set_rows(png_ptr, info_ptr, sss->image.row_pointers);
    if (sss->image.palette != NULL)
        png_set_PLTE(png_ptr, info_ptr, sss->image.palette, sss->image.num_palette);
    /* Transparency is not considered metadata, although tRNS is ancillary.
     * See the comment in opng_is_image_chunk() above.
     */
    if (sss->image.trans_alpha != NULL || sss->image.trans_color_ptr != NULL)
        png_set_tRNS(png_ptr, info_ptr,
            sss->image.trans_alpha,
            sss->image.num_trans, sss->image.trans_color_ptr);

    if (!store_meta)
        return;

    if (sss->image.background_ptr != NULL)
        png_set_bKGD(png_ptr, info_ptr, sss->image.background_ptr);
    if (sss->image.hist != NULL)
        png_set_hIST(png_ptr, info_ptr, sss->image.hist);
    if (sss->image.sig_bit_ptr != NULL)
        png_set_sBIT(png_ptr, info_ptr, sss->image.sig_bit_ptr);
    if (sss->image.num_unknowns != 0)
    {
        png_set_unknown_chunks(png_ptr, info_ptr,
            sss->image.unknowns, sss->image.num_unknowns);
        /* This should be handled by libpng. */
        for (i = 0; i < sss->image.num_unknowns; ++i)
            png_set_unknown_chunk_location(png_ptr, info_ptr,
                i, sss->image.unknowns[i].location);
    }
}

/*
 * Image info destruction.
 */
static void
opng_destroy_image_info(MyStruct *sss)
{
    png_uint_32 i;
    int j;

    if (sss->image.row_pointers == NULL)
        return;  /* nothing to clean up */

    for (i = 0; i < sss->image.height; ++i)
        opng_free(sss->image.row_pointers[i]);
    opng_free(sss->image.row_pointers);
    opng_free(sss->image.palette);
    opng_free(sss->image.trans_alpha);
    opng_free(sss->image.hist);
    for (j = 0; j < sss->image.num_unknowns; ++j)
        opng_free(sss->image.unknowns[j].data);
    opng_free(sss->image.unknowns);
    /* DO NOT deallocate background_ptr, sig_bit_ptr, trans_color_ptr.
     * See the comments regarding double copying inside opng_load_image_info().
     */

     /* Clear the space here and do not worry about double-deallocation issues
      * that might arise later on.
      */
    memset(&sss->image, 0, sizeof(opng_image_struct));
}



/*
 * Image file reading.
 */
static void
opng_read_file(MyStruct *sss, FILE* infile)
{
    const char* fmt_name;
    int num_img;
    png_uint_32 reductions;
    const char* volatile err_msg;  /* volatile is required by cexcept */
    png_structp read_ptr = NULL;
    png_infop read_info_ptr = NULL;

    read_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
        NULL, opng_error, opng_warning);
    read_info_ptr = png_create_info_struct(read_ptr);
    sss->read_info_ptr = read_info_ptr;

    if (read_info_ptr == NULL) {
        return;
    }

    /* Override the default libpng settings. */
    png_set_keep_unknown_chunks(read_ptr,
        PNG_HANDLE_CHUNK_ALWAYS, NULL, 0);
    png_set_user_limits(read_ptr, PNG_UINT_31_MAX, PNG_UINT_31_MAX);

    /* Read the input image file. */
    opng_init_read_data();

    sss->in_file = infile;

    pngx_set_read_fn(read_ptr, sss, opng_read_data);
    fmt_name = NULL;
    num_img = pngx_read_image(read_ptr, read_info_ptr, &fmt_name, NULL);
    if (num_img <= 0) {
        return;
    }
    if (num_img > 1)
        sss->process.status |= INPUT_HAS_MULTIPLE_IMAGES;
    if ((sss->process.status & INPUT_IS_PNG_FILE) &&
        (sss->process.status & INPUT_HAS_MULTIPLE_IMAGES))
    {
        /* pngxtern can't distinguish between APNG and proper PNG. */
        fmt_name = (sss->process.status & INPUT_HAS_PNG_SIGNATURE) ?
            "APNG" : "APNG datastream";
    }
    OPNG_ENSURE(fmt_name != NULL, "No format name from pngxtern");

    if (sss->process.in_file_size == 0)
    {
        if (opng_fgetsize(infile, &sss->process.in_file_size) < 0)
        {
            opng_print_warning("Can't get the correct file size");
            sss->process.in_file_size = 0;
        }
    }

    err_msg = NULL;  /* everything is ok */


    /* Display format and image information. */
    if (strcmp(fmt_name, "PNG") != 0)
    {
        printf("Importing %s", fmt_name);
        if (sss->process.status & INPUT_HAS_MULTIPLE_IMAGES)
        {
            if (!(sss->process.status & INPUT_IS_PNG_FILE))
                printf(" (multi-image or animation)");
            if (sss->option.snip)
                printf("; snipping...");
        }
        printf("\n");
    }
    opng_load_image_info(read_ptr, read_info_ptr, 1);
    opng_print_image_info(&sss->image, 1, 1, 1, 1);
    printf("\n");

    /* Choose the applicable image reductions. */
    reductions = OPNG_REDUCE_ALL & ~OPNG_REDUCE_METADATA;
    if (sss->option.nb)
        reductions &= ~OPNG_REDUCE_BIT_DEPTH;
    if (sss->option.nc)
        reductions &= ~OPNG_REDUCE_COLOR_TYPE;
    if (sss->option.np)
        reductions &= ~OPNG_REDUCE_PALETTE;
    if (sss->option.nz && (sss->process.status & INPUT_HAS_PNG_DATASTREAM))
    {
        /* Do not reduce files with PNG datastreams under -nz. */
        reductions = OPNG_REDUCE_NONE;
    }
    if (sss->process.status & INPUT_HAS_DIGITAL_SIGNATURE)
    {
        /* Do not reduce signed files. */
        reductions = OPNG_REDUCE_NONE;
    }
    if ((sss->process.status & INPUT_IS_PNG_FILE) &&
        (sss->process.status & INPUT_HAS_MULTIPLE_IMAGES) &&
        (reductions != OPNG_REDUCE_NONE) && !sss->option.snip)
    {
        printf(
            "Can't reliably reduce APNG file; disabling reductions.\n"
            "(Did you want to -snip and optimize the first frame?)\n");
        reductions = OPNG_REDUCE_NONE;
    }

    /* Try to reduce the image-> */
    sss->process.reductions =
        opng_reduce_image(read_ptr, read_info_ptr, reductions);

    /* If the image is reduced, enforce full compression. */
    if (sss->process.reductions != OPNG_REDUCE_NONE)
    {
        opng_load_image_info(read_ptr, read_info_ptr, 1);
        printf("Reducing image to ");
        opng_print_image_info(&sss->image, 0, 1, 1, 0);
        printf("\n");
    }

    /* Change the interlace type if required. */
    if (sss->option.interlace >= 0 &&
        sss->image.interlace_type != sss->option.interlace)
    {
        sss->image.interlace_type = sss->option.interlace;
        /* A change in interlacing requires IDAT recoding. */
        sss->process.status |= OUTPUT_NEEDS_NEW_IDAT;
    }

    /* Destroy the libpng structures, but leave the enclosed data intact
     * to allow further processing.
     */
    png_data_freer(read_ptr, read_info_ptr,
        PNG_USER_WILL_FREE_DATA, PNG_FREE_ALL);
    png_destroy_read_struct(&read_ptr, &read_info_ptr, NULL);
}

/*
 * PNG file writing.
 *
 * If the output file is NULL, PNG encoding is still done,
 * but no file is written.
 */
static void
opng_write_file(MyStruct* sss, FILE* outfile,
    int compression_level, int memory_level,
    int compression_strategy, int filter)
{
    png_structp write_ptr = NULL;
    png_infop write_info_ptr = NULL;

    write_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
        NULL, opng_error, opng_warning);
    write_info_ptr = png_create_info_struct(write_ptr);
    if (write_info_ptr == NULL) {
        return;
    }

    png_set_compression_level(write_ptr, compression_level);
    png_set_compression_mem_level(write_ptr, memory_level);
    png_set_compression_strategy(write_ptr, compression_strategy);
    png_set_filter(write_ptr, PNG_FILTER_TYPE_BASE, g_filter_table[filter]);//
    if (compression_strategy != Z_HUFFMAN_ONLY &&
        compression_strategy != Z_RLE)
    {
        if (sss->option.window_bits > 0)
            png_set_compression_window_bits(write_ptr, sss->option.window_bits);
    }
    else
    {
        png_set_compression_window_bits(write_ptr, 9);
    }

    /* Override the default libpng settings. */
    png_set_keep_unknown_chunks(write_ptr, PNG_HANDLE_CHUNK_ALWAYS, NULL, 0);
    png_set_user_limits(write_ptr, PNG_UINT_31_MAX, PNG_UINT_31_MAX);

    /* Write the PNG stream. */
    opng_store_image_info(sss, write_ptr, write_info_ptr, (outfile != NULL));
    opng_init_write_data(&sss->process);
    sss->out_file = outfile;
    sss->crt_idat_crc = 0;
    sss->crt_idat_offset = 0;
    sss->crt_idat_size = 0;
    sss->allow_crt_chunk = 0;
    sss->crt_chunk_is_idat = 0;
    png_set_write_fn(write_ptr, sss, opng_write_data, NULL);
    png_write_png(write_ptr, write_info_ptr, 0, NULL);
    png_destroy_write_struct(&write_ptr, &write_info_ptr);
}

/*
 * PNG file copying.
 */
static void
opng_copy_file(MyStruct * sss, FILE* infile, FILE* outfile)
{
    volatile png_bytep buf;  /* volatile is required by cexcept */
    const png_uint_32 buf_size_incr = 0x1000;
    png_uint_32 buf_size, length;
    png_byte chunk_hdr[8];
    const char* volatile err_msg;

    png_structp write_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, opng_error, opng_warning);
    if (write_ptr == NULL) {
        return;
    }
    opng_init_write_data(&sss->process);

    sss->out_file = outfile;
    sss->crt_idat_crc = 0;
    sss->crt_idat_offset = 0;
    sss->crt_idat_size = 0;
    png_set_write_fn(write_ptr, sss, opng_write_data, NULL);

    {
        buf = NULL;
        buf_size = 0;

        /* Write the signature in the output file. */
        pngx_write_sig(write_ptr);

        /* Copy all chunks until IEND. */
        /* Error checking is done only at a very basic level. */
        do
        {
            if (fread(chunk_hdr, 8, 1, infile) != 1) {
                return;
            }
            length = png_get_uint_32(chunk_hdr);
            if (length > PNG_UINT_31_MAX)
            {
                if (buf == NULL && length == 0x89504e47UL)  /* "\x89PNG" */
                {
                    /* Skip the signature. */
                    continue;
                }
                return;
            }
            if (length + 4 > buf_size)
            {
                png_free(write_ptr, buf);
                buf_size =
                    (((length + 4) + (buf_size_incr - 1)) / buf_size_incr) *
                    buf_size_incr;
                buf = (png_bytep)png_malloc(write_ptr, buf_size);
                /* Do not use realloc() here, it's slower. */
            }
            if (fread(buf, length + 4, 1, infile) != 1)  /* data + crc */ {
                return;
            }
            png_write_chunk(write_ptr, chunk_hdr + 4, buf, length);
        } while (memcmp(chunk_hdr + 4, sig_IEND, 4) != 0);

        err_msg = NULL;  /* everything is ok */
    }

    png_free(write_ptr, buf);
    png_destroy_write_struct(&write_ptr, NULL);
}

/*
 * Iteration initialization.
 */
static void
opng_init_iteration(opng_options* option, opng_bitset_t cmdline_set, opng_bitset_t mask_set,
    const char* preset, opng_bitset_t* output_set)
{
    opng_bitset_t preset_set;
    int check;

    *output_set = cmdline_set & mask_set;
    if (*output_set == 0 && cmdline_set != 0) {
        return;
    }
    if (*output_set == 0 || option->optim_level >= 0)
    {
        check =
            opng_strparse_rangeset_to_bitset(&preset_set, preset, mask_set);
        OPNG_ENSURE(check == 0, "[internal] Invalid preset");
        *output_set |= preset_set & mask_set;
    }
}

/*
 * Iteration initialization.
 */
static void
opng_init_iterations(MyStruct* sss)
{
    opng_bitset_t compr_level_set, mem_level_set, strategy_set, filter_set;
    opng_bitset_t strategy_singles_set;
    int preset_index;
    int t1, t2;

    /* Set the IDAT size limit. The trials that pass this limit will be
     * abandoned, as there will be no need to wait until their completion.
     * This limit may further decrease as iterations go on.
     */
    if ((sss->process.status & OUTPUT_NEEDS_NEW_IDAT))
        sss->process.max_idat_size = idat_size_max;
    else
    {
        OPNG_ENSURE(process.in_idat_size > 0, "No IDAT in input");
        /* Add the input PLTE and tRNS sizes to the initial max IDAT size,
         * to account for the changes that may occur during reduction.
         * This incurs a negligible overhead on processing only: the final
         * IDAT size will not be affected, because a precise check will be
         * performed at the end, inside opng_finish_iterations().
         */
        sss->process.max_idat_size =
            sss->process.in_idat_size + sss->process.in_plte_trns_size;
    }

    /* Get preset_index from options.optim_level, but leave the latter intact,
     * because the effect of "optipng -o2 -z... -f..." is slightly different
     * from the effect of "optipng -z... -f..." (without "-o").
     */
    preset_index = sss->option.optim_level;
    if (preset_index < 0)
        preset_index = OPNG_OPTIM_LEVEL_DEFAULT;
    else if (preset_index > OPNG_OPTIM_LEVEL_MAX)
        preset_index = OPNG_OPTIM_LEVEL_MAX;

    /* Initialize the iteration sets.
     * Combine the user-defined values with the optimization presets.
     */
    opng_init_iteration(&sss->option, sss->option.compr_level_set, OPNG_COMPR_LEVEL_SET_MASK,
        g_presets[preset_index].compr_level, &compr_level_set);
    opng_init_iteration(&sss->option, sss->option.mem_level_set, OPNG_MEM_LEVEL_SET_MASK,
        g_presets[preset_index].mem_level, &mem_level_set);
    opng_init_iteration(&sss->option, sss->option.strategy_set, OPNG_STRATEGY_SET_MASK,
        g_presets[preset_index].strategy, &strategy_set);
    opng_init_iteration(&sss->option, sss->option.filter_set, OPNG_FILTER_SET_MASK,
        g_presets[preset_index].filter, &filter_set);

    /* Replace the empty sets with the libpng's "best guess" heuristics. */
    if (compr_level_set == 0)
        opng_bitset_set(&compr_level_set, Z_BEST_COMPRESSION);  /* -zc9 */
    if (mem_level_set == 0)
        opng_bitset_set(&mem_level_set, 8);
    if (sss->image.bit_depth < 8 || sss->image.palette != NULL)
    {
        if (strategy_set == 0)
            opng_bitset_set(&strategy_set, Z_DEFAULT_STRATEGY);  /* -zs0 */
        if (filter_set == 0)
            opng_bitset_set(&filter_set, 0);  /* -f0 */
    }
    else
    {
        if (strategy_set == 0)
            opng_bitset_set(&strategy_set, Z_FILTERED);  /* -zs1 */
        if (filter_set == 0)
            opng_bitset_set(&filter_set, 5);  /* -f0 */
    }

    /* Store the results into process. */
    sss->process.compr_level_set = compr_level_set;
    sss->process.mem_level_set = mem_level_set;
    sss->process.strategy_set = strategy_set;
    sss->process.filter_set = filter_set;
    strategy_singles_set = (1 << Z_HUFFMAN_ONLY) | (1 << Z_RLE);
    t1 = opng_bitset_count(compr_level_set) *
        opng_bitset_count(strategy_set & ~strategy_singles_set);
    t2 = opng_bitset_count(strategy_set & strategy_singles_set);
    sss->process.num_iterations = (t1 + t2) *
        opng_bitset_count(mem_level_set) *
        opng_bitset_count(filter_set);
    OPNG_ENSURE(process.num_iterations > 0, "Invalid iteration parameters");
}


/*
 * Iteration finalization.
 */
static void opng_finish_iterations(MyStruct *sss)
{
    if (sss->process.best_idat_size + sss->process.out_plte_trns_size <
        sss->process.in_idat_size + sss->process.in_plte_trns_size)
        sss->process.status |= OUTPUT_NEEDS_NEW_IDAT;
    if (sss->process.status & OUTPUT_NEEDS_NEW_IDAT)
    {
        if (sss->process.best_idat_size <= idat_size_max)
        {
            //printf("\nSelecting parameters:\n");
            //printf("  zc = %d  zm = %d  zs = %d  f = %d\r\n",
            //    sss->process.best_compr_level, sss->process.best_mem_level,
            //    sss->process.best_strategy, sss->process.best_filter);
            if (sss->process.best_idat_size > 0)
            {
                /* At least one trial has been run. */
               // printf("\t\tIDAT size = %" OPNG_FSIZE_PRIu, sss->process.best_idat_size);
            }
            printf("\n");
        }
        else
        {
            /* The compressed image data is larger than the maximum allowed. */
            printf("  zc = *  zm = *  zs = *  f = *\t\tIDAT size > %s\r\n",idat_size_max_string);
        }
    }
}




#ifdef PNG_INFO_IMAGE_SUPPORTED


png_bytepp PNGAPI
pngx_malloc_rows(png_structp png_ptr, png_infop info_ptr, int filler)
{
    return pngx_malloc_rows_extended(png_ptr, info_ptr, 0, filler);
}

png_bytepp PNGAPI
pngx_malloc_rows_extended(png_structp png_ptr, png_infop info_ptr,
    pngx_alloc_size_t min_row_size, int filler)
{
    pngx_alloc_size_t row_size;
    png_bytep row;
    png_bytepp rows;
    png_uint_32 height, i;

    /* Check the image dimensions and calculate the row size. */
    height = png_get_image_height(png_ptr, info_ptr);
    if (height == 0)
        png_error(png_ptr, "Missing IHDR");
    row_size = png_get_rowbytes(png_ptr, info_ptr);
    /* libpng sets row_size to 0 when the width is too large to process. */
    if (row_size == 0 ||
        (pngx_alloc_size_t)height > (pngx_alloc_size_t)(-1) / sizeof(png_bytep))
        png_error(png_ptr, "Can't handle exceedingly large image dimensions");
    if (row_size < min_row_size)
        row_size = min_row_size;

    /* Deallocate the currently-existing rows. */
    png_free_data(png_ptr, info_ptr, PNG_FREE_ROWS, 0);

    /* Allocate memory for the row index. */
    rows = (png_bytepp)png_malloc(png_ptr,
        (pngx_alloc_size_t)(height * sizeof(png_bytep)));
    if (rows == NULL)
        return NULL;

    /* Allocate memory for each row. */
    for (i = 0; i < height; ++i)
    {
        row = (png_bytep)png_malloc(png_ptr, row_size);
        if (row == NULL)
        {
            /* Release the memory allocated up to the point of failure. */
            while (i > 0)
                png_free(png_ptr, rows[--i]);
            png_free(png_ptr, rows);
            return NULL;
        }
        if (filler >= 0)
            memset(row, filler, row_size);
        rows[i] = row;
    }

    /* Set the row pointers. */
    png_set_rows(png_ptr, info_ptr, rows);
    return rows;
}

#if 0  /* not necessary */
void PNGAPI
pngx_free_rows(png_structp png_ptr, png_infop info_ptr)
{
    png_free_data(png_ptr, info_ptr, PNG_FREE_ROWS, 0);
}
#endif


#endif /* PNG_INFO_IMAGE_SUPPORTED */

void PNGAPI pngx_set_compression_type(png_structp png_ptr, png_infop info_ptr,
    int compression_type)
{
    png_uint_32 width, height;
    int bit_depth, color_type, interlace_type, filter_type;
    int old_compression_type;

    if (!png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth,
        &color_type, &interlace_type, &old_compression_type, &filter_type))
        return;
    if (compression_type == old_compression_type)
        return;
    png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth,
        color_type, interlace_type, compression_type, filter_type);
}

void PNGAPI pngx_set_filter_type(png_structp png_ptr, png_infop info_ptr,
    int filter_type)
{
    png_uint_32 width, height;
    int bit_depth, color_type, interlace_type, compression_type;
    int old_filter_type;

    if (!png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth,
        &color_type, &interlace_type, &compression_type, &old_filter_type))
        return;
    if (filter_type == old_filter_type)
        return;
    png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth,
        color_type, interlace_type, compression_type, filter_type);
}

void PNGAPI pngx_set_interlace_type(png_structp png_ptr, png_infop info_ptr,
    int interlace_type)
{
    png_uint_32 width, height;
    int bit_depth, color_type, compression_type, filter_type;
    int old_interlace_type;

    if (!png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth,
        &color_type, &old_interlace_type, &compression_type, &filter_type))
        return;
    if (interlace_type == old_interlace_type)
        return;
    png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth,
        color_type, interlace_type, compression_type, filter_type);
}




#if defined(PNGX_DEBUG) && (PNGX_DEBUG > 0)
#include <assert.h>
#define PNGX_ASSERT(cond) assert(cond)
#else
#define PNGX_ASSERT(cond) ((void)0)
#endif




/* BMP support */
int pngx_sig_is_bmp(png_bytep sig, size_t sig_size,
    png_const_charpp fmt_name_ptr,
    png_const_charpp fmt_long_name_ptr);
int pngx_read_bmp(png_structp png_ptr, png_infop info_ptr, FILE* stream);

static int pngx_sig_is_png(png_structp png_ptr,
    png_bytep sig, size_t sig_size,
    png_const_charpp fmt_name_ptr,
    png_const_charpp fmt_long_name_ptr)
{
    /* The signature of this function differs from the other pngx_sig_is_X()
     * functions, to allow extra functionality (e.g. customized error messages)
     * without requiring a full pngx_read_png().
     */

    static const char pngx_png_standalone_fmt_name[] =
        "PNG";
    static const char pngx_png_datastream_fmt_name[] =
        "PNG datastream";
    static const char pngx_png_standalone_fmt_long_name[] =
        "Portable Network Graphics";
    static const char pngx_png_datastream_fmt_long_name[] =
        "Portable Network Graphics embedded datastream";

    static const png_byte png_file_sig[8] = { 137, 80, 78, 71, 13, 10, 26, 10 };
    static const png_byte mng_file_sig[8] = { 138, 77, 78, 71, 13, 10, 26, 10 };
    static const png_byte png_ihdr_sig[8] = { 0, 0, 0, 13, 73, 72, 68, 82 };

    int has_png_sig;

    /* Since png_read_png() fails rather abruptly with png_error(),
     * spend a little more effort to ensure that the format is indeed PNG.
     * Among other things, look for the presence of IHDR.
     */
    if (sig_size <= 25 + 18)  /* size of (IHDR + IDAT) > (12+13) + (12+6) */
        return -1;
    has_png_sig = (memcmp(sig, png_file_sig, 8) == 0);
    if (memcmp(sig + (has_png_sig ? 8 : 0), png_ihdr_sig, 8) != 0)
    {
        /* This is not valid PNG: get as much information as possible. */
        if (memcmp(sig, png_file_sig, 4) == 0 && (sig[4] == 10 || sig[4] == 13))
            png_error(png_ptr,
                "PNG file appears to be corrupted by text file conversions");
        else if (memcmp(sig, mng_file_sig, 8) == 0)
            png_error(png_ptr, "MNG decoding is not supported");
        /* JNG is handled by the pngxrjpg module. */
        return 0;  /* not PNG */
    }

    /* Store the format name. */
    if (fmt_name_ptr != NULL)
    {
        *fmt_name_ptr = has_png_sig ?
            pngx_png_standalone_fmt_name :
            pngx_png_datastream_fmt_name;
    }
    if (fmt_long_name_ptr != NULL)
    {
        *fmt_long_name_ptr = has_png_sig ?
            pngx_png_standalone_fmt_long_name :
            pngx_png_datastream_fmt_long_name;
    }
    return 1;  /* PNG, really! */
}

int PNGAPI
pngx_read_image(png_structp png_ptr, png_infop info_ptr,
    png_const_charpp fmt_name_ptr,
    png_const_charpp fmt_long_name_ptr)
{
    png_byte sig[128];
    size_t num;
    int (*read_fn)(png_structp, png_infop, FILE*);
    fpos_t fpos;
    int result;

    /* Precondition. */
#ifdef PNG_FLAG_MALLOC_NULL_MEM_OK
    if (png_ptr->flags & PNG_FLAG_MALLOC_NULL_MEM_OK)
        png_error(png_ptr, "pngxtern requires a safe allocator");
#endif

    /* Read the signature bytes. */
    MyStruct* sss = (MyStruct*)png_get_io_ptr(png_ptr);
    FILE* stream = (FILE*)sss->in_file;

    if (fgetpos(stream, &fpos) != 0)
        png_error(png_ptr, "Can't ftell in input file stream");
    num = fread(sig, 1, sizeof(sig), stream);
    if (fsetpos(stream, &fpos) != 0)
        png_error(png_ptr, "Can't fseek in input file stream");

    /* Try the PNG format first. */
    if (pngx_sig_is_png(png_ptr, sig, num, fmt_name_ptr, fmt_long_name_ptr) > 0)
    {
        png_read_png(png_ptr, info_ptr, 0, NULL);
        if (getc(stream) != EOF)
        {
            png_warning(png_ptr, "Extraneous data found after IEND");
            fseek(stream, 0, SEEK_END);
        }
        return 1;
    }

    /* Check the signature bytes against other known image formats. */
    if (pngx_sig_is_bmp(sig, num, fmt_name_ptr, fmt_long_name_ptr) > 0)
        read_fn = pngx_read_bmp;
    else
        return 0;  /* not a known image format */

    /* Read the image. */
    result = read_fn(png_ptr, info_ptr, stream);
    /* Signature checking may give false positives; reading can still fail. */
    if (result <= 0)  /* this isn't the format we thought it was */
        if (fsetpos(stream, &fpos) != 0)
            png_error(png_ptr, "Can't fseek in input file stream");
    return result;
}

#include <stdio.h>
#include <string.h>

/* BMP file signature */
#define BMP_SIGNATURE       0x4d42  /* "BM" */
#define BMP_SIG_BYTES       2

/* BITMAPFILEHEADER */
#define BFH_WTYPE           0       /* WORD   bfType;           */
#define BFH_DSIZE           2       /* DWORD  bfSize;           */
#define BFH_WRESERVED1      6       /* WORD   bfReserved1;      */
#define BFH_WRESERVED2      8       /* WORD   bfReserved2;      */
#define BFH_DOFFBITS        10      /* DWORD  bfOffBits;        */
#define BFH_DBIHSIZE        14      /* DWORD  biSize;           */
#define FILEHED_SIZE        14      /* sizeof(BITMAPFILEHEADER) */
#define BIHSIZE_SIZE        4       /* sizeof(biSize)           */

/* BITMAPINFOHEADER */
#define BIH_DSIZE           0       /* DWORD  biSize;             */
#define BIH_LWIDTH          4       /* LONG   biWidth;            */
#define BIH_LHEIGHT         8       /* LONG   biHeight;           */
#define BIH_WPLANES         12      /* WORD   biPlanes;           */
#define BIH_WBITCOUNT       14      /* WORD   biBitCount;         */
#define BIH_DCOMPRESSION    16      /* DWORD  biCompression;      */
#define BIH_DSIZEIMAGE      20      /* DWORD  biSizeImage;        */
#define BIH_LXPELSPERMETER  24      /* LONG   biXPelsPerMeter;    */
#define BIH_LYPELSPERMETER  28      /* LONG   biYPelsPerMeter;    */
#define BIH_DCLRUSED        32      /* DWORD  biClrUsed;          */
#define BIH_DCLRIMPORTANT   36      /* DWORD  biClrImportant;     */
#define B4H_DREDMASK        40      /* DWORD  bV4RedMask;         */
#define B4H_DGREENMASK      44      /* DWORD  bV4GreenMask;       */
#define B4H_DBLUEMASK       48      /* DWORD  bV4BlueMask;        */
#define B4H_DALPHAMASK      52      /* DWORD  bV4AlphaMask;       */
#define B4H_DCSTYPE         56      /* DWORD  bV4CSType;          */
#define B4H_XENDPOINTS      60      /* CIEXYZTRIPLE bV4Endpoints; */
#define B4H_DGAMMARED       96      /* DWORD  bV4GammaRed;        */
#define B4H_DGAMMAGREEN     100     /* DWORD  bV4GammaGreen;      */
#define B4H_DGAMMABLUE      104     /* DWORD  bV4GammaBlue;       */
#define B5H_DINTENT         108     /* DWORD  bV5Intent;          */
#define B5H_DPROFILEDATA    112     /* DWORD  bV5ProfileData;     */
#define B5H_DPROFILESIZE    116     /* DWORD  bV5ProfileSize;     */
#define B5H_DRESERVED       120     /* DWORD  bV5Reserved;        */
#define INFOHED_SIZE        40      /* sizeof(BITMAPINFOHEADER)   */
#define BMPV4HED_SIZE       108     /* sizeof(BITMAPV4HEADER)     */
#define BMPV5HED_SIZE       124     /* sizeof(BITMAPV5HEADER)     */

/* BITMAPCOREHEADER */
#define BCH_DSIZE           0       /* DWORD  bcSize;           */
#define BCH_WWIDTH          4       /* WORD   bcWidth;          */
#define BCH_WHEIGHT         6       /* WORD   bcHeight;         */
#define BCH_WPLANES         8       /* WORD   bcPlanes;         */
#define BCH_WBITCOUNT       10      /* WORD   bcBitCount;       */
#define COREHED_SIZE        12      /* sizeof(BITMAPCOREHEADER) */

/* RGB */
#define RGB_BLUE            0       /* BYTE   rgbBlue;     */
#define RGB_GREEN           1       /* BYTE   rgbGreen;    */
#define RGB_RED             2       /* BYTE   rgbRed;      */
#define RGB_RESERVED        3       /* BYTE   rgbReserved; */
#define RGBTRIPLE_SIZE      3       /* sizeof(RGBTRIPLE)   */
#define RGBQUAD_SIZE        4       /* sizeof(RGBQUAD)     */

/* Constants for the biCompression field */
#ifndef BI_RGB
#define BI_RGB              0       /* Uncompressed format       */
#define BI_RLE8             1       /* RLE format (8 bits/pixel) */
#define BI_RLE4             2       /* RLE format (4 bits/pixel) */
#define BI_BITFIELDS        3       /* Bitfield format           */
#define BI_JPEG             4       /* JPEG format               */
#define BI_PNG              5       /* PNG format                */
#endif


/*****************************************************************************/
/* BMP memory utilities                                                      */
/*****************************************************************************/

static unsigned int
bmp_get_word(png_bytep ptr)
{
    return (unsigned int)ptr[0] + ((unsigned int)ptr[1] << 8);
}

static png_uint_32
bmp_get_dword(png_bytep ptr)
{
    return ((png_uint_32)ptr[0]) + ((png_uint_32)ptr[1] << 8) +
        ((png_uint_32)ptr[2] << 16) + ((png_uint_32)ptr[3] << 24);
}


static void
bmp_memset_bytes(png_bytep ptr, size_t offset, int ch, size_t len)
{
    memset(ptr + offset, ch, len);
}

static void
bmp_memset_halfbytes(png_bytep ptr, size_t offset, int ch, size_t len)
{
    if (len == 0)
        return;
    ptr += offset / 2;
    if (offset & 1)  /* use half-byte operations at odd offset */
    {
        *ptr = (png_byte)((*ptr & 0xf0) | (ch & 0x0f));
        ch = ((ch & 0xf0) >> 4) | ((ch & 0x0f) << 4);
        ++ptr;
        --len;
    }
    memset(ptr, ch, len / 2);
    if (len & 1)
        ptr[len / 2] = (png_byte)(ch & 0xf0);
}

static size_t
bmp_fread_bytes(png_bytep ptr, size_t offset, size_t len, FILE* stream)
{
    size_t result;

    result = fread(ptr + offset, 1, len, stream);
    if (len & 1)
        getc(stream);  /* skip padding */
    return result;
}

static size_t
bmp_fread_halfbytes(png_bytep ptr, size_t offset, size_t len, FILE* stream)
{
    size_t result;
    int ch;

    if (len == 0)
        return 0;
    ptr += offset / 2;
    if (offset & 1)  /* use half-byte operations at odd offset */
    {
        for (result = 0; result < len - 1; result += 2)
        {
            ch = getc(stream);
            if (ch == EOF)
                break;
            *ptr = (png_byte)((*ptr & 0xf0) | ((ch & 0xf0) >> 4));
            ++ptr;
            *ptr = (png_byte)((ch & 0x0f) << 4);
        }
    }
    else
    {
        result = fread(ptr, 1, (len + 1) / 2, stream) * 2;
    }
    if (len & 2)
        getc(stream);  /* skip padding */
    return (result <= len) ? result : len;
}


/*****************************************************************************/
/* BMP packbit (BITFIELDS) helpers                                           */
/*****************************************************************************/

static void
bmp_process_mask(png_uint_32 bmp_mask, png_bytep sig_bit, png_bytep shift_bit)
{
    *sig_bit = *shift_bit = (png_byte)0;
    if (bmp_mask == 0)
        return;
    while ((bmp_mask & 1) == 0)
    {
        bmp_mask >>= 1;
        ++* shift_bit;
    }
    while (bmp_mask != 0)
    {
        if ((bmp_mask & 1) == 0 || *sig_bit >= 8)
        {
            *sig_bit = (png_byte)0;
            return;
        }
        bmp_mask >>= 1;
        ++* sig_bit;
    }
}


/*****************************************************************************/
/* BMP I/O utilities                                                         */
/*****************************************************************************/

static size_t
bmp_read_rows(png_bytepp begin_row, png_bytepp end_row, size_t row_size,
    unsigned int compression, FILE* stream)
{
    size_t result;
    png_bytepp crt_row;
    int inc;
    size_t crtn, dcrtn, endn;
    unsigned int len, b1, b2;
    int ch;
    void (*bmp_memset_fn)(png_bytep, size_t, int, size_t);
    size_t(*bmp_fread_fn)(png_bytep, size_t, size_t, FILE*);

    if (row_size == 0)
        return 0;  /* this should not happen */

    inc = (begin_row <= end_row) ? 1 : -1;
    crtn = 0;
    result = 0;
    if (compression == BI_RLE4)
    {
        endn = row_size * 2;
        if (endn <= row_size)
            return 0;  /* overflow */
        bmp_memset_fn = bmp_memset_halfbytes;
        bmp_fread_fn = bmp_fread_halfbytes;
    }
    else
    {
        endn = row_size;
        bmp_memset_fn = bmp_memset_bytes;
        bmp_fread_fn = bmp_fread_bytes;
    }

    if (compression == BI_RGB || compression == BI_BITFIELDS)
    {
        /* Read uncompressed bitmap. */
        for (crt_row = begin_row; crt_row != end_row; crt_row += inc)
        {
            crtn = bmp_fread_fn(*crt_row, 0, endn, stream);
            if (crtn != endn)
                break;
            ++result;
        }
    }
    else if (compression == BI_RLE8 || compression == BI_RLE4)
    {
        /* Read RLE-compressed bitmap. */
        if (compression == BI_RLE8)
        {
            endn = row_size;
        }
        else  /* BI_RLE4 */
        {
            endn = row_size * 2;
            if (endn <= row_size)
                return 0;  /* overflow */
        }
        for (crt_row = begin_row; crt_row != end_row; )
        {
            ch = getc(stream); b1 = (unsigned int)ch;
            ch = getc(stream); b2 = (unsigned int)ch;
            if (ch == EOF)
                break;
            if (b1 == 0)  /* escape */
            {
                if (b2 == 0)  /* end of line */
                {
                    bmp_memset_fn(*crt_row, crtn, 0, endn - crtn);
                    crt_row += inc;
                    crtn = 0;
                    ++result;
                    if (crt_row == end_row)  /* all rows are read */
                    {
                        ch = getc(stream);  /* check for the end of bitmap */
                        if (ch != EOF && ch != 0)
                        {
                            ungetc(ch, stream);  /* forget about the end of bitmap */
                            break;
                        }
                        getc(stream);  /* expect 1, but break the loop anyway */
                        break;
                    }
                }
                else if (b2 == 1)  /* end of bitmap */
                {
                    bmp_memset_fn(*crt_row, crtn, 0, endn - crtn);
                    crt_row += inc;
                    crtn = 0;
                    result = (begin_row <= end_row) ?
                        (end_row - begin_row) : (begin_row - end_row);
                    break;  /* the rest is wiped out at the end */
                }
                else if (b2 == 2)  /* delta */
                {
                    ch = getc(stream); b1 = (unsigned int)ch;  /* horiz. offset */
                    ch = getc(stream); b2 = (unsigned int)ch;  /* vert. offset */
                    if (ch == EOF)
                        break;
                    dcrtn = (b1 < endn - crtn) ? (crtn + b1) : endn;
                    for (; b2 > 0; --b2)
                    {
                        bmp_memset_fn(*crt_row, crtn, 0, endn - crtn);
                        crt_row += inc;
                        crtn = 0;
                        ++result;
                        if (crt_row == end_row)
                            break;
                    }
                    if (crt_row != end_row)
                        bmp_memset_fn(*crt_row, crtn, 0, dcrtn - crtn);
                }
                else  /* b2 >= 3 bytes in absolute mode */
                {
                    len = (b2 <= endn - crtn) ? b2 : (unsigned int)(endn - crtn);
                    if (bmp_fread_fn(*crt_row, crtn, len, stream) != len)
                        break;
                    crtn += len;
                }
            }
            else  /* b1 > 0 bytes in run-length encoded mode */
            {
                len = (b1 <= endn - crtn) ? b1 : (unsigned int)(endn - crtn);
                bmp_memset_fn(*crt_row, crtn, (int)b2, len);
                crtn += len;
            }
        }
    }
    else
        return 0;  /* error: compression method not applicable. */

    /* Wipe out the portion left unread. */
    for (; crt_row != end_row; crt_row += inc)
    {
        bmp_memset_fn(*crt_row, crtn, 0, endn - crtn);
        crtn = 0;
    }

    return result;
}


/*****************************************************************************/
/* BMP-to-PNG sample conversion                                              */
/*****************************************************************************/

static void
bmp_to_png_rows(png_bytepp row_pointers,
    png_uint_32 width, png_uint_32 height, unsigned int pixdepth,
    png_bytep rgba_sig, png_bytep rgba_shift)
{
    png_bytep src_ptr, dest_ptr;
    unsigned int rgba_mask[4];
    unsigned int num_samples, sample, mask;
    unsigned int wpix;
    png_uint_32 dwpix;
    png_uint_32 x, y;
    unsigned int i;

    if (pixdepth == 24)  /* BGR -> RGB */
    {
        for (y = 0; y < height; ++y)
        {
            src_ptr = row_pointers[y];
            for (x = 0; x < width; ++x, src_ptr += 3)
            {
                png_byte tmp = src_ptr[0];
                src_ptr[0] = src_ptr[2];
                src_ptr[2] = tmp;
            }
        }
        return;
    }

    num_samples = (rgba_sig[3] != 0) ? 4 : 3;
    for (i = 0; i < num_samples; ++i)
        rgba_mask[i] = (1U << rgba_sig[i]) - 1;

    if (pixdepth == 16)
    {
        for (y = 0; y < height; ++y)
        {
            src_ptr = row_pointers[y] + (width - 1) * 2;
            dest_ptr = row_pointers[y] + (width - 1) * num_samples;
            for (x = 0; x < width; ++x, src_ptr -= 2, dest_ptr -= num_samples)
            {
                /* Inline bmp_get_word() for performance reasons. */
                wpix = (unsigned int)src_ptr[0] + ((unsigned int)src_ptr[1] << 8);
                for (i = 0; i < num_samples; ++i)
                {
                    mask = rgba_mask[i];
                    sample = (wpix >> rgba_shift[i]) & mask;
                    dest_ptr[i] = (png_byte)((sample * 255 + mask / 2) / mask);
                }
            }
        }
    }
    else if (pixdepth == 32)
    {
        for (y = 0; y < height; ++y)
        {
            src_ptr = dest_ptr = row_pointers[y];
            for (x = 0; x < width; ++x, src_ptr += 4, dest_ptr += num_samples)
            {
                /* Inline bmp_get_dword() for performance reasons. */
                dwpix = (png_uint_32)src_ptr[0] + ((png_uint_32)src_ptr[1] << 8) +
                    ((png_uint_32)src_ptr[2] << 16) + ((png_uint_32)src_ptr[3] << 24);
                for (i = 0; i < num_samples; ++i)
                {
                    mask = rgba_mask[i];
                    sample = (dwpix >> rgba_shift[i]) & mask;
                    dest_ptr[i] = (png_byte)((sample * 255 + mask / 2) / mask);
                }
            }
        }
    }
    /* else do nothing */
}


/*****************************************************************************/
/* BMP read support for pngxtern                                             */
/*****************************************************************************/

int /* PRIVATE */
pngx_sig_is_bmp(png_bytep sig, size_t sig_size,
    png_const_charpp fmt_name_ptr,
    png_const_charpp fmt_long_name_ptr)
{
    static const char bmp_fmt_name[] = "BMP";
    static const char os2bmp_fmt_long_name[] = "OS/2 Bitmap";
    static const char winbmp_fmt_long_name[] = "Windows Bitmap";
    png_uint_32 bihsize;

    /* Require at least the bitmap file header and the subsequent 4 bytes. */
    if (sig_size < FILEHED_SIZE + 4)
        return -1;  /* insufficient data */
    if (bmp_get_word(sig) != BMP_SIGNATURE)
        return 0;  /* not BMP */
    /* Avoid using bfhsize because it is not reliable. */
    bihsize = bmp_get_dword(sig + FILEHED_SIZE);
    if ((bihsize > PNG_UINT_31_MAX) ||
        (bihsize != COREHED_SIZE && bihsize < INFOHED_SIZE))
        return 0;  /* garbage in bihsize, this cannot be BMP */

    /* Store the format name. */
    if (fmt_name_ptr != NULL)
        *fmt_name_ptr = bmp_fmt_name;
    if (fmt_long_name_ptr != NULL)
    {
        if (bihsize == COREHED_SIZE)
            *fmt_long_name_ptr = os2bmp_fmt_long_name;
        else
            *fmt_long_name_ptr = winbmp_fmt_long_name;
    }
    return 1;  /* BMP */
}

int /* PRIVATE */
pngx_read_bmp(png_structp png_ptr, png_infop info_ptr, FILE* stream)
{
    png_byte bfh[FILEHED_SIZE + BMPV5HED_SIZE];
    png_bytep const bih = bfh + FILEHED_SIZE;
    png_byte rgbq[RGBQUAD_SIZE];
    png_uint_32 offbits, bihsize, skip;
    png_uint_32 width, height, rowsize;
    int topdown;
    unsigned int pixdepth;
    png_uint_32 compression;
    unsigned int palsize, palnum;
    png_uint_32 rgba_mask[4];
    png_byte rgba_sig[4], rgba_shift[4];
    int bit_depth, color_type;
    png_color palette[256];
    png_color_8 sig_bit;
    png_bytepp row_pointers, begin_row, end_row;
    unsigned int i;
    size_t y;

    /* Find the BMP header. */
    for (i = 0; ; ++i)  /* skip macbinary header */
    {
        if (fread(bfh, FILEHED_SIZE + BIHSIZE_SIZE, 1, stream) != 1)
            ++i;
        else if (bmp_get_word(bfh + BFH_WTYPE) == BMP_SIGNATURE)
            break;
        if (fread(bfh, 128 - FILEHED_SIZE - BIHSIZE_SIZE, 1, stream) != 1)
            ++i;
        if (i > 0)
            return 0;  /* not a BMP file */
    }

    /* Read the BMP header. */
    offbits = bmp_get_dword(bfh + BFH_DOFFBITS);
    bihsize = bmp_get_dword(bfh + BFH_DBIHSIZE);
    if ((offbits > PNG_UINT_31_MAX) || (bihsize > PNG_UINT_31_MAX) ||
        (offbits < bihsize + FILEHED_SIZE) ||
        (bihsize != COREHED_SIZE && bihsize < INFOHED_SIZE))
        return 0;  /* not a BMP file, just a file with a matching signature */
    if (bihsize > BMPV5HED_SIZE)
    {
        skip = bihsize - BMPV5HED_SIZE;
        bihsize = BMPV5HED_SIZE;
    }
    else
        skip = 0;
    if (fread(bih + BIHSIZE_SIZE, bihsize - BIHSIZE_SIZE, 1, stream) != 1)
        return 0;
    if (skip > 0)
        if (fseek(stream, (long)skip, SEEK_CUR) != 0)
            return 0;
    skip = offbits - bihsize - FILEHED_SIZE;  /* new skip */
    topdown = 0;
    if (bihsize < INFOHED_SIZE)  /* OS/2 BMP */
    {
        width = bmp_get_word(bih + BCH_WWIDTH);
        height = bmp_get_word(bih + BCH_WHEIGHT);
        pixdepth = bmp_get_word(bih + BCH_WBITCOUNT);
        compression = BI_RGB;
        palsize = RGBTRIPLE_SIZE;
    }
    else  /* Windows BMP */
    {
        width = bmp_get_dword(bih + BIH_LWIDTH);
        height = bmp_get_dword(bih + BIH_LHEIGHT);
        pixdepth = bmp_get_word(bih + BIH_WBITCOUNT);
        compression = bmp_get_dword(bih + BIH_DCOMPRESSION);
        palsize = RGBQUAD_SIZE;
        if (height > PNG_UINT_31_MAX)  /* top-down BMP */
        {
            height = PNG_UINT_32_MAX - height + 1;
            topdown = 1;
        }
        if (bihsize == INFOHED_SIZE && compression == BI_BITFIELDS)
        {
            /* Read the RGB[A] mask. */
            i = (skip <= 16) ? (unsigned int)skip : 16;
            if (fread(bih + B4H_DREDMASK, i, 1, stream) != 1)
                return 0;
            bihsize += i;
            skip -= i;
        }
    }

    memset(rgba_mask, 0, sizeof(rgba_mask));
    if (pixdepth > 8)
    {
        if (compression == BI_RGB)
        {
            if (pixdepth == 16)
            {
                compression = BI_BITFIELDS;
                rgba_mask[0] = 0x7c00;
                rgba_mask[1] = 0x03e0;
                rgba_mask[2] = 0x001f;
            }
            else  /* pixdepth == 24 || pixdepth == 32 */
            {
                rgba_mask[0] = (png_uint_32)0x00ff0000L;
                rgba_mask[1] = (png_uint_32)0x0000ff00L;
                rgba_mask[2] = (png_uint_32)0x000000ffL;
            }
        }
        else if (compression == BI_BITFIELDS)
        {
            if (bihsize >= INFOHED_SIZE + 12)
            {
                rgba_mask[0] = bmp_get_dword(bih + B4H_DREDMASK);
                rgba_mask[1] = bmp_get_dword(bih + B4H_DGREENMASK);
                rgba_mask[2] = bmp_get_dword(bih + B4H_DBLUEMASK);
            }
            else
                png_error(png_ptr, "Missing color mask in BMP file");
        }
        if (bihsize >= INFOHED_SIZE + 16)
            rgba_mask[3] = bmp_get_dword(bih + B4H_DALPHAMASK);
    }

    switch (compression)
    {
    case BI_RGB:
        /* Allow pixel depth values 1, 2, 4, 8, 16, 24, 32.
         * (Although the BMP spec does not mention pixel depth = 2,
         * it is supported for robustness reasons, at no extra cost.)
         */
        if (pixdepth > 0 && 32 % pixdepth != 0 && pixdepth != 24)
            pixdepth = 0;
        break;
    case BI_RLE8:
        if (pixdepth != 8)
            pixdepth = 0;
        break;
    case BI_RLE4:
        if (pixdepth != 4)
            pixdepth = 0;
        break;
    case BI_BITFIELDS:
        if (pixdepth != 16 && pixdepth != 32)
            pixdepth = 0;
        break;
    case BI_JPEG:
        png_error(png_ptr, "JPEG-compressed BMP files are not supported");
        /* NOTREACHED */
        break;
    case BI_PNG:
        if (ungetc(getc(stream), stream) == 0)  /* IHDR is likely to follow */
            png_set_sig_bytes(png_ptr, 8);
        png_set_read_fn(png_ptr, stream, NULL);
        png_read_png(png_ptr, info_ptr, 0, NULL);
        /* TODO: Check for mismatches between the BMP and PNG info. */
        return 1;
    default:
        png_error(png_ptr, "Unsupported compression method in BMP file");
    }

    /* Check the BMP image parameters. */
    if (width == 0 || width > PNG_UINT_31_MAX || height == 0)
        png_error(png_ptr, "Invalid image dimensions in BMP file");
    if (pixdepth == 0)
        png_error(png_ptr, "Invalid pixel depth in BMP file");

    /* Compute the PNG image parameters. */
    if (pixdepth <= 8)
    {
        palnum = skip / palsize;
        if (palnum > 256)
            palnum = 256;
        skip -= palsize * palnum;
        rowsize = (width + (32 / pixdepth) - 1) / (32 / pixdepth) * 4;
        /* rowsize becomes 0 on overflow. */
        bit_depth = pixdepth;
        color_type = (palnum > 0) ? PNG_COLOR_TYPE_PALETTE : PNG_COLOR_TYPE_GRAY;
    }
    else
    {
        palnum = 0;
        bit_depth = 8;
        switch (pixdepth)
        {
        case 16:
            rowsize = (width * 2 + 3) & (~3);
            break;
        case 24:
            rowsize = (width * 3 + 3) & (~3);
            break;
        case 32:
            rowsize = width * 4;
            break;
        default:  /* never get here */
            bit_depth = 0;
            rowsize = 0;
        }
        if (rowsize / width < pixdepth / 8)
            rowsize = 0;  /* overflow */
        color_type = (rgba_mask[3] != 0) ?
            PNG_COLOR_TYPE_RGBA : PNG_COLOR_TYPE_RGB;
    }
    if (rowsize == 0)
        png_error(png_ptr, "Can't handle exceedingly large BMP dimensions");

    /* Set the PNG image type. */
    png_set_IHDR(png_ptr, info_ptr,
        width, height, bit_depth, color_type,
        PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
    if (pixdepth > 8)
    {
        for (i = 0; i < 4; ++i)
            bmp_process_mask(rgba_mask[i], &rgba_sig[i], &rgba_shift[i]);
        if (rgba_sig[0] == 0 || rgba_sig[1] == 0 || rgba_sig[2] == 0)
            png_error(png_ptr, "Invalid color mask in BMP file");
        if (rgba_sig[0] != 8 || rgba_sig[1] != 8 ||
            rgba_sig[2] != 8 || (rgba_sig[3] != 0 && rgba_sig[3] != 8))
        {
            sig_bit.red = rgba_sig[0];
            sig_bit.green = rgba_sig[1];
            sig_bit.blue = rgba_sig[2];
            sig_bit.alpha = rgba_sig[3];
            png_set_sBIT(png_ptr, info_ptr, &sig_bit);
        }
    }

    /* Read the color palette (if applicable). */
    if (palnum > 0)
    {
        for (i = 0; i < palnum; ++i)
        {
            if (fread(rgbq, palsize, 1, stream) != 1)
                break;
            palette[i].red = rgbq[RGB_RED];
            palette[i].green = rgbq[RGB_GREEN];
            palette[i].blue = rgbq[RGB_BLUE];
        }
        png_set_PLTE(png_ptr, info_ptr, palette, i);
        if (i != palnum)
            png_error(png_ptr, "Error reading color palette in BMP file");
    }

    /* Allocate memory and read the image data. */
    row_pointers = pngx_malloc_rows_extended(png_ptr, info_ptr, rowsize, -1);
    if (topdown)
    {
        begin_row = row_pointers;
        end_row = row_pointers + height;
    }
    else
    {
        begin_row = row_pointers + height - 1;
        end_row = row_pointers - 1;
    }
    if (skip > 0)
        fseek(stream, (long)skip, SEEK_CUR);
    y = bmp_read_rows(begin_row, end_row, rowsize, compression, stream);

    /* Postprocess the image data, even if it has not been read entirely. */
    if (pixdepth > 8)
        bmp_to_png_rows(row_pointers, width, height, pixdepth,
            rgba_sig, rgba_shift);

    /* Check the result. */
    if (y != (size_t)height)
        png_error(png_ptr, "Error reading BMP file");

    return 1;  /* one image has been successfully read */
}

static void opng_iterate_1(MyStruct* sss)
{
    opng_bitset_t compr_level_set = sss->process.compr_level_set;
    opng_bitset_t mem_level_set = sss->process.mem_level_set;
    opng_bitset_t strategy_set = sss->process.strategy_set;
    opng_bitset_t filter_set = sss->process.filter_set;

    if ((sss->process.num_iterations == 1) && (sss->process.status & OUTPUT_NEEDS_NEW_IDAT))
    {
        /* There is only one combination. Select it and return. */
        sss->process.best_idat_size = 0;  /* unknown */
        sss->process.best_compr_level = opng_bitset_find_first(compr_level_set);
        sss->process.best_mem_level = opng_bitset_find_first(mem_level_set);
        sss->process.best_strategy = opng_bitset_find_first(strategy_set);
        sss->process.best_filter = opng_bitset_find_first(filter_set);
        return;
    }

    /* Prepare for the big iteration. */
    sss->process.best_idat_size = idat_size_max + 1;
    sss->process.best_compr_level = -1;
    sss->process.best_mem_level = -1;
    sss->process.best_strategy = -1;
    sss->process.best_filter = -1;

    /* Iterate through the "hyper-rectangle" (zc, zm, zs, f). */
   // printf("\nTrying[%s]:\n", __FUNCTION__);

    //获得最佳参数组
    for (int g_filter = OPNG_FILTER_MIN; g_filter <= OPNG_FILTER_MAX; ++g_filter)
    {
        if (!opng_bitset_test(filter_set, g_filter))
            continue;
        for (int strategy = OPNG_STRATEGY_MIN; strategy <= OPNG_STRATEGY_MAX; ++strategy) {
            if (!opng_bitset_test(strategy_set, strategy))
                continue;
            if (strategy == Z_HUFFMAN_ONLY)
            {
                compr_level_set = 0;
                opng_bitset_set(&compr_level_set, 1);
            }
            else if (strategy == Z_RLE) {
                compr_level_set = 0;
                opng_bitset_set(&compr_level_set, 9);
            }
            else {
                compr_level_set = sss->process.compr_level_set;
            }

            for (int compr_level = OPNG_COMPR_LEVEL_MAX;
                compr_level >= OPNG_COMPR_LEVEL_MIN;
                --compr_level)
            {
                if (!opng_bitset_test(compr_level_set, compr_level))
                    continue;
                for (int mem_level = OPNG_MEM_LEVEL_MAX;
                    mem_level >= OPNG_MEM_LEVEL_MIN;
                    --mem_level)
                {
                    if (!opng_bitset_test(mem_level_set, mem_level))
                        continue;
                   // printf("  zc = %d  zm = %d  zs = %d  f = %d\r\n",compr_level, mem_level, strategy, g_filter);
                    //计算对应参数组下面的输出编码大小
                    opng_write_file(sss, NULL, compr_level, mem_level, strategy, g_filter);
                    if (sss->process.out_idat_size > idat_size_max)
                    {
                        continue;
                    }
                   // printf("\t\tIDAT size = %" OPNG_FSIZE_PRIu "\n", sss->process.out_idat_size);
                    if (sss->process.best_idat_size < sss->process.out_idat_size)
                    {
                        /* The current best size is smaller than the last size.
                         * Discard the last iteration.
                         */
                        continue;
                    }
                    if (sss->process.best_idat_size == sss->process.out_idat_size &&
                        (sss->process.best_strategy == Z_HUFFMAN_ONLY ||
                            sss->process.best_strategy == Z_RLE))
                    {
                        /* The current best size is equal to the last size;
                         * the current best strategy is already the fastest.
                         * Discard the last iteration.
                         */
                        continue;
                    }
                    sss->process.best_compr_level = compr_level;
                    sss->process.best_mem_level = mem_level;
                    sss->process.best_strategy = strategy;
                    sss->process.best_filter = g_filter;
                    sss->process.best_idat_size = sss->process.out_idat_size;
                }
            }
        }
    }
}

// Thread!!
static void opng_iterate_2_impl(MyStruct* sss, int g_filter)
{
    opng_bitset_t compr_level_set = sss->process.compr_level_set;
    opng_bitset_t mem_level_set = sss->process.mem_level_set;
    opng_bitset_t strategy_set = sss->process.strategy_set;
    opng_bitset_t filter_set = sss->process.filter_set;

    if ((sss->process.num_iterations == 1) && (sss->process.status & OUTPUT_NEEDS_NEW_IDAT))
    {
        /* There is only one combination. Select it and return. */
        sss->process.best_idat_size = 0;  /* unknown */
        sss->process.best_compr_level = opng_bitset_find_first(compr_level_set);
        sss->process.best_mem_level = opng_bitset_find_first(mem_level_set);
        sss->process.best_strategy = opng_bitset_find_first(strategy_set);
        sss->process.best_filter = opng_bitset_find_first(filter_set);
        return;
    }

    /* Prepare for the big iteration. */
    sss->process.best_idat_size = idat_size_max + 1;
    sss->process.best_compr_level = -1;
    sss->process.best_mem_level = -1;
    sss->process.best_strategy = -1;
    sss->process.best_filter = -1;

    /* Iterate through the "hyper-rectangle" (zc, zm, zs, f). */
   // printf("\nTrying[%s] [%d]:\n", __FUNCTION__, g_filter);

    if (!opng_bitset_test(filter_set, g_filter))
        return;
    for (int strategy = OPNG_STRATEGY_MIN; strategy <= OPNG_STRATEGY_MAX; ++strategy) {
        if (!opng_bitset_test(strategy_set, strategy))
            continue;
        if (strategy == Z_HUFFMAN_ONLY)
        {
            compr_level_set = 0;
            opng_bitset_set(&compr_level_set, 1);
        }
        else if (strategy == Z_RLE) {
            compr_level_set = 0;
            opng_bitset_set(&compr_level_set, 9);
        }
        else {
            compr_level_set = sss->process.compr_level_set;
        }

        for (int compr_level = OPNG_COMPR_LEVEL_MAX;
            compr_level >= OPNG_COMPR_LEVEL_MIN;
            --compr_level)
        {
            if (!opng_bitset_test(compr_level_set, compr_level))
                continue;
            for (int mem_level = OPNG_MEM_LEVEL_MAX;
                mem_level >= OPNG_MEM_LEVEL_MIN;
                --mem_level)
            {
                if (!opng_bitset_test(mem_level_set, mem_level))
                    continue;
               // printf("  zc = %d  zm = %d  zs = %d  f = %d\r\n",compr_level, mem_level, strategy, g_filter);
                //计算对应参数组下面的输出编码大小
                opng_write_file(sss, NULL, compr_level, mem_level, strategy, g_filter);
                if (sss->process.out_idat_size > idat_size_max)
                {
                    continue;
                }
               // printf("\t\tIDAT size = %" OPNG_FSIZE_PRIu "\n", sss->process.out_idat_size);
                if (sss->process.best_idat_size < sss->process.out_idat_size)
                {
                    /* The current best size is smaller than the last size.
                     * Discard the last iteration.
                     */
                    continue;
                }
                if (sss->process.best_idat_size == sss->process.out_idat_size &&
                    (sss->process.best_strategy == Z_HUFFMAN_ONLY ||
                        sss->process.best_strategy == Z_RLE))
                {
                    /* The current best size is equal to the last size;
                     * the current best strategy is already the fastest.
                     * Discard the last iteration.
                     */
                    continue;
                }
                sss->process.best_compr_level = compr_level;
                sss->process.best_mem_level = mem_level;
                sss->process.best_strategy = strategy;
                sss->process.best_filter = g_filter;
                sss->process.best_idat_size = sss->process.out_idat_size;
            }
        }
    }
}


#include <thread>
static void opng_iterate_2(MyStruct* sss)
{
    /* Prepare for the big iteration. */
    sss->process.best_idat_size = idat_size_max + 1;
    sss->process.best_compr_level = -1;
    sss->process.best_mem_level = -1;
    sss->process.best_strategy = -1;
    sss->process.best_filter = -1;

    //获得最佳参数组
    MyStruct obj_arr[OPNG_FILTER_MAX + 1];
    for (int g_filter = OPNG_FILTER_MIN; g_filter < OPNG_FILTER_MAX + 1; g_filter++) {
        memcpy(&obj_arr[g_filter].option, &sss->option, sizeof(opng_options));
        memcpy(&obj_arr[g_filter].process, &sss->process, sizeof(opng_process_struct));
        memcpy(&obj_arr[g_filter].image, &sss->image, sizeof(opng_image_struct));
        obj_arr[g_filter].in_file = sss->in_file;
        obj_arr[g_filter].read_info_ptr = sss->read_info_ptr;
        obj_arr[g_filter].out_file = sss->out_file;
    }

    int bRun[OPNG_FILTER_MAX + 1] = {0};

    //计算
    for (int g_filter = OPNG_FILTER_MIN; g_filter < OPNG_FILTER_MAX + 1; g_filter++){
        MyStruct* obj = &obj_arr[g_filter];
        int* bFlag = &bRun[g_filter];
        std::thread th([obj, g_filter, bFlag] {
            opng_iterate_2_impl(obj, g_filter);
            *bFlag = 1;
        });
        th.detach();
    }

    while (true){
        int sum = bRun[0] + bRun[1] + bRun[2] + bRun[3] + bRun[4] + bRun[5];
        //if(sum > 0)
            //printf("Sum = %d\r\n", sum);
        if (sum == 6) {
            break;
        }
        Sleep(50);
    }

    //printf("OKK 111\r\n");
    //找出最好的参数组
    for (int g_filter = OPNG_FILTER_MIN; g_filter < OPNG_FILTER_MAX + 1; g_filter++) {
        if (sss->process.best_idat_size > obj_arr[g_filter].process.best_idat_size &&
            obj_arr[g_filter].process.best_idat_size > 0) {
            memcpy(sss, &obj_arr[g_filter], sizeof(MyStruct));
        }
    }

   // printf("OKK  222\r\n");
}

WXIMAGE_API  int WXOptipngCompress(const char* strInput, const char* strOutput, int level, int bMulThreads)
{
    MyStruct sss;
    memset(&sss, 0, sizeof(struct MyStruct));
    memset(&sss.process, 0, sizeof(struct opng_process_struct));
    memset(&sss.image, 0, sizeof(struct opng_image_struct));
    memset(&sss.option, 0, sizeof(struct opng_options));
    sss.option.interlace = -1;
    sss.option.optim_level = level;//压缩强度  1-7

    if (sss.option.optim_level == 0)
    {
        sss.option.nb = 1;
        sss.option.nc = 1;
        sss.option.np = 1;
        sss.option.nz = 1;
    }
    int my_result = 0;

    opng_clear_image_info(&sss);

    {
        FILE* infile = fopen(strInput, "rb");

        if (infile == NULL) {
            return 0;
        }

        opng_read_file(&sss, infile);//读取png文件

        fclose(infile);  /* finally */

        /* Check the error flag. This must be the first check. */
        if (sss.process.status & INPUT_HAS_ERRORS)
        {
            printf("Recoverable errors found in input.");
            if (sss.option.fix)
            {
                printf(" Fixing...\n");
                sss.process.status |= OUTPUT_NEEDS_NEW_FILE;
            }
            else
            {
                printf(" Rerun " PROGRAM_NAME " with -fix enabled.\n");
                return 0;
            }
        }

        /* Check the junk flag. */
        if (sss.process.status & INPUT_HAS_JUNK)
            sss.process.status |= OUTPUT_NEEDS_NEW_FILE;

        /* Check the PNG signature and datastream flags. */
        if (!(sss.process.status & INPUT_HAS_PNG_SIGNATURE))
            sss.process.status |= OUTPUT_NEEDS_NEW_FILE;
        if (sss.process.status & INPUT_HAS_PNG_DATASTREAM)
        {
            if (sss.option.nz && (sss.process.status & OUTPUT_NEEDS_NEW_IDAT))
            {
                printf(
                    "IDAT recoding is necessary, but is disabled by the user.\n");
                return 0;
            }
        }
        else
            sss.process.status |= OUTPUT_NEEDS_NEW_IDAT;

        /* Check the digital signature flag. */
        if (sss.process.status & INPUT_HAS_DIGITAL_SIGNATURE)
        {
            printf("Digital signature found in input.");
            if (sss.option.force)
            {
                printf(" Erasing...\n");
                sss.process.status |= OUTPUT_NEEDS_NEW_FILE;
            }
            else
            {
                printf(" Rerun " PROGRAM_NAME " with -force enabled.\n");
                return 0;
            }
        }

        /* Check the multi-image flag. */
        if (sss.process.status & INPUT_HAS_MULTIPLE_IMAGES)
        {
            if (!sss.option.snip && !(sss.process.status & INPUT_IS_PNG_FILE))
            {
                printf("Conversion to PNG requires snipping. "
                    "Rerun " PROGRAM_NAME " with -snip enabled.\n");
                return 0;
            }
        }
        if ((sss.process.status & INPUT_HAS_APNG) && sss.option.snip)
            sss.process.status |= OUTPUT_NEEDS_NEW_FILE;

        /* Check the stripped-data flag. */
        if (sss.process.status & INPUT_HAS_STRIPPED_DATA)
            printf("Stripping metadata...\n");

        /* Find the best parameters and see if it's worth recompressing. */
        if (!sss.option.nz || (sss.process.status & OUTPUT_NEEDS_NEW_IDAT))
        {
            opng_init_iterations(&sss);
            if (bMulThreads)
                opng_iterate_2(&sss);
            else
                opng_iterate_1(&sss);
            opng_finish_iterations(&sss);
        }
        if (sss.process.status & OUTPUT_NEEDS_NEW_IDAT)
        {
            sss.process.status |= OUTPUT_NEEDS_NEW_FILE;
            opng_check_idat_size(sss.process.best_idat_size);
        }

        if (sss.process.status & OUTPUT_NEEDS_NEW_IDAT)
        {
            FILE* outfile = fopen(strOutput, "wb");
            if (outfile == NULL) {
                return 0;
            }
            opng_write_file(&sss, outfile,
                sss.process.best_compr_level, sss.process.best_mem_level,
                sss.process.best_strategy, sss.process.best_filter);
            fclose(outfile);
        }
        else
        { //拷贝数据
            FILE* outfile = fopen(strOutput, "wb");
            if (outfile == NULL) {
                return -1;
            }
            /* Copy the input PNG datastream to the output. */
            infile = fopen(strInput, "rb");
            if (infile == NULL) {
                return 0;
            }
            if (sss.process.in_datastream_offset > 0 && opng_fseeko(infile, sss.process.in_datastream_offset, SEEK_SET) != 0)
                return 0;
            sss.process.best_idat_size = sss.process.in_idat_size;
            opng_copy_file(&sss, infile, outfile);
            fclose(infile);  /* finally */
        }


        if (sss.option.preserve)
            opng_os_copy_attr(strInput, strOutput);

        /* Display the output IDAT/file sizes. */
       // printf("\nOutput IDAT size = %" OPNG_FSIZE_PRIu " bytes", sss.process.out_idat_size);
        if (sss.process.status & INPUT_HAS_PNG_DATASTREAM)
        {
            printf(" (");
            opng_print_fsize_difference(sss.process.in_idat_size,
                sss.process.out_idat_size, 0);
            printf(")");
        }
        opng_print_fsize_difference(sss.process.in_file_size, sss.process.out_file_size, 1);
        my_result = 1;
    }
    opng_destroy_image_info(&sss);
    return my_result;
}


WXIMAGE_API  int WXOptipngCompressU(const wchar_t* strInput, const wchar_t* strOutput, int level, int bMulThreads)
{
    MyStruct sss;
    memset(&sss, 0, sizeof(struct MyStruct));
    memset(&sss.process, 0, sizeof(struct opng_process_struct));
    memset(&sss.image, 0, sizeof(struct opng_image_struct));
    memset(&sss.option, 0, sizeof(struct opng_options));
    sss.option.interlace = -1;
    sss.option.optim_level = level;//压缩强度  1-7

    if (sss.option.optim_level == 0)
    {
        sss.option.nb = 1;
        sss.option.nc = 1;
        sss.option.np = 1;
        sss.option.nz = 1;
    }
    int my_result = 0;

    opng_clear_image_info(&sss);

    {
        FILE* infile = _wfopen(strInput, L"rb");

        if (infile == NULL) {
            return 0;
        }

        opng_read_file(&sss, infile);//读取png文件

        fclose(infile);  /* finally */

        /* Check the error flag. This must be the first check. */
        if (sss.process.status & INPUT_HAS_ERRORS)
        {
            printf("Recoverable errors found in input.");
            if (sss.option.fix)
            {
                printf(" Fixing...\n");
                sss.process.status |= OUTPUT_NEEDS_NEW_FILE;
            }
            else
            {
                printf(" Rerun " PROGRAM_NAME " with -fix enabled.\n");
                return 0;
            }
        }

        /* Check the junk flag. */
        if (sss.process.status & INPUT_HAS_JUNK)
            sss.process.status |= OUTPUT_NEEDS_NEW_FILE;

        /* Check the PNG signature and datastream flags. */
        if (!(sss.process.status & INPUT_HAS_PNG_SIGNATURE))
            sss.process.status |= OUTPUT_NEEDS_NEW_FILE;
        if (sss.process.status & INPUT_HAS_PNG_DATASTREAM)
        {
            if (sss.option.nz && (sss.process.status & OUTPUT_NEEDS_NEW_IDAT))
            {
                printf(
                    "IDAT recoding is necessary, but is disabled by the user.\n");
                return 0;
            }
        }
        else
            sss.process.status |= OUTPUT_NEEDS_NEW_IDAT;

        /* Check the digital signature flag. */
        if (sss.process.status & INPUT_HAS_DIGITAL_SIGNATURE)
        {
            printf("Digital signature found in input.");
            if (sss.option.force)
            {
                printf(" Erasing...\n");
                sss.process.status |= OUTPUT_NEEDS_NEW_FILE;
            }
            else
            {
                printf(" Rerun " PROGRAM_NAME " with -force enabled.\n");
                return 0;
            }
        }

        /* Check the multi-image flag. */
        if (sss.process.status & INPUT_HAS_MULTIPLE_IMAGES)
        {
            if (!sss.option.snip && !(sss.process.status & INPUT_IS_PNG_FILE))
            {
                printf("Conversion to PNG requires snipping. "
                    "Rerun " PROGRAM_NAME " with -snip enabled.\n");
                return 0;
            }
        }
        if ((sss.process.status & INPUT_HAS_APNG) && sss.option.snip)
            sss.process.status |= OUTPUT_NEEDS_NEW_FILE;

        /* Check the stripped-data flag. */
        if (sss.process.status & INPUT_HAS_STRIPPED_DATA)
            printf("Stripping metadata...\n");

        /* Find the best parameters and see if it's worth recompressing. */
        if (!sss.option.nz || (sss.process.status & OUTPUT_NEEDS_NEW_IDAT))
        {
            opng_init_iterations(&sss);
            if (bMulThreads)
                opng_iterate_2(&sss);
            else
                opng_iterate_1(&sss);
            opng_finish_iterations(&sss);
        }
        if (sss.process.status & OUTPUT_NEEDS_NEW_IDAT)
        {
            sss.process.status |= OUTPUT_NEEDS_NEW_FILE;
            opng_check_idat_size(sss.process.best_idat_size);
        }

        if (sss.process.status & OUTPUT_NEEDS_NEW_IDAT)
        {
            FILE* outfile = _wfopen(strOutput, L"wb");
            if (outfile == NULL) {
                return 0;
            }
            opng_write_file(&sss, outfile,
                sss.process.best_compr_level, sss.process.best_mem_level,
                sss.process.best_strategy, sss.process.best_filter);
            fclose(outfile);
        }
        else
        { //拷贝数据
            FILE* outfile = _wfopen(strOutput, L"wb");
            if (outfile == NULL) {
                return -1;
            }
            /* Copy the input PNG datastream to the output. */
            infile = _wfopen(strInput, L"rb");
            if (infile == NULL) {
                return 0;
            }
            if (sss.process.in_datastream_offset > 0 && opng_fseeko(infile, sss.process.in_datastream_offset, SEEK_SET) != 0)
                return 0;
            sss.process.best_idat_size = sss.process.in_idat_size;
            opng_copy_file(&sss, infile, outfile);
            fclose(infile);  /* finally */
        }


        if (sss.option.preserve)
            opng_os_copy_attr_u(strInput, strOutput);

        /* Display the output IDAT/file sizes. */
       // printf("\nOutput IDAT size = %" OPNG_FSIZE_PRIu " bytes", sss.process.out_idat_size);
        if (sss.process.status & INPUT_HAS_PNG_DATASTREAM)
        {
            printf(" (");
            opng_print_fsize_difference(sss.process.in_idat_size,
                sss.process.out_idat_size, 0);
            printf(")");
        }
        opng_print_fsize_difference(sss.process.in_file_size, sss.process.out_file_size, 1);
        my_result = 1;
    }
    opng_destroy_image_info(&sss);
    return my_result;
}
