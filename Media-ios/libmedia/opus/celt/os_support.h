#ifndef OS_SUPPORT_H
#define OS_SUPPORT_H

#ifdef CUSTOM_SUPPORT
#  include "custom_support.h"
#endif

#include "opus_types.h"
#include "opus_defines.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/** Opus wrapper for malloc(). To do your own dynamic allocation, all you need to do is replace this function and opus_free */
#ifndef OVERRIDE_OPUS_ALLOC
static OPUS_INLINE void *opus_alloc (size_t size)
{
   return malloc(size);
}
#endif

/** Same as celt_alloc(), except that the area is only needed inside a CELT call (might cause problem with wideband though) */
#ifndef OVERRIDE_OPUS_ALLOC_SCRATCH
static OPUS_INLINE void *opus_alloc_scratch (size_t size)
{
   /* Scratch space doesn't need to be cleared */
   return opus_alloc(size);
}
#endif

/** Opus wrapper for free(). To do your own dynamic allocation, all you need to do is replace this function and opus_alloc */
#ifndef OVERRIDE_OPUS_FREE
static OPUS_INLINE void opus_free (void *ptr)
{
   free(ptr);
}
#endif

/** Copy n bytes of memory from src to dst. The 0* term provides compile-time type checking  */
#ifndef OVERRIDE_OPUS_COPY
#define OPUS_COPY(dst, src, n) (memcpy((dst), (src), (n)*sizeof(*(dst)) + 0*((dst)-(src)) ))
#endif

/** Copy n bytes of memory from src to dst, allowing overlapping regions. The 0* term
    provides compile-time type checking */
#ifndef OVERRIDE_OPUS_MOVE
#define OPUS_MOVE(dst, src, n) (memmove((dst), (src), (n)*sizeof(*(dst)) + 0*((dst)-(src)) ))
#endif

/** Set n elements of dst to zero, starting at address s */
#ifndef OVERRIDE_OPUS_CLEAR
#define OPUS_CLEAR(dst, n) (memset((dst), 0, (n)*sizeof(*(dst))))
#endif

/*#ifdef __GNUC__
#pragma GCC poison printf sprintf
#pragma GCC poison malloc free realloc calloc
#endif*/

#endif /* OS_SUPPORT_H */

