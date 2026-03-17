#ifndef AVUTIL_AVUTIL_H
#define AVUTIL_AVUTIL_H


unsigned avutil_version(void);
const char *av_version_info(void);
const char *avutil_configuration(void);
const char *avutil_license(void);


enum AVMediaType {
    AVMEDIA_TYPE_UNKNOWN = -1,  ///< Usually treated as AVMEDIA_TYPE_DATA
    AVMEDIA_TYPE_VIDEO,
    AVMEDIA_TYPE_AUDIO,
    AVMEDIA_TYPE_DATA,          ///< Opaque data information usually continuous
    AVMEDIA_TYPE_SUBTITLE,
    AVMEDIA_TYPE_ATTACHMENT,    ///< Opaque data information usually sparse
    AVMEDIA_TYPE_NB
};

const char *av_get_media_type_string(enum AVMediaType media_type);

#define FF_LAMBDA_SHIFT 7
#define FF_LAMBDA_SCALE (1<<FF_LAMBDA_SHIFT)
#define FF_QP2LAMBDA 118 ///< factor to convert from H.263 QP to lambda
#define FF_LAMBDA_MAX (256*128-1)

#define FF_QUALITY_SCALE FF_LAMBDA_SCALE //FIXME maybe remove
#define AV_NOPTS_VALUE          ((int64_t)UINT64_C(0x8000000000000000))
#define AV_TIME_BASE            1000000
#define AV_TIME_BASE_Q          (AVRational){1, AV_TIME_BASE}
enum AVPictureType {
    AV_PICTURE_TYPE_NONE = 0, ///< Undefined
    AV_PICTURE_TYPE_I,     ///< Intra
    AV_PICTURE_TYPE_P,     ///< Predicted
    AV_PICTURE_TYPE_B,     ///< Bi-dir predicted
    AV_PICTURE_TYPE_S,     ///< S(GMC)-VOP MPEG4
    AV_PICTURE_TYPE_SI,    ///< Switching Intra
    AV_PICTURE_TYPE_SP,    ///< Switching Predicted
    AV_PICTURE_TYPE_BI,    ///< BI type
};

char av_get_picture_type_char(enum AVPictureType pict_type);


#include "common.h"
#include "error.h"
#include "rational.h"
#include "version.h"
#include "macros.h"
#include "mathematics.h"
#include "log.h"
#include "pixfmt.h"

#include "internal.h"
#include "intmath.h"

/**
 * Return x default pointer in case p is NULL.
 */
static inline void *av_x_if_null(const void *p, const void *x)
{
    return (void *)(intptr_t)(p ? p : x);
}

/**
 * Compute the length of an integer list.
 *
 * @param elsize  size in bytes of each list element (only 1, 2, 4 or 8)
 * @param term    list terminator (usually 0 or -1)
 * @param list    pointer to the list
 * @return  length of the list, in elements, not counting the terminator
 */
unsigned av_int_list_length_for_size(unsigned elsize,
                                     const void *list, uint64_t term) av_pure;

/**
 * Compute the length of an integer list.
 *
 * @param term  list terminator (usually 0 or -1)
 * @param list  pointer to the list
 * @return  length of the list, in elements, not counting the terminator
 */
#define av_int_list_length(list, term) \
    av_int_list_length_for_size(sizeof(*(list)), list, term)

/**
 * Open a file using a UTF-8 filename.
 * The API of this function matches POSIX fopen(), errors are returned through
 * errno.
 */
FILE *av_fopen_utf8(const char *path, const char *mode);

/**
 * Return the fractional representation of the internal time base.
 */
AVRational av_get_time_base_q(void);

/**
 * @}
 * @}
 */

#endif /* AVUTIL_AVUTIL_H */
