/*
 *  Copyright 2011 The LibYuv Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS. All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef INCLUDE_LIBYUV_H_
#define INCLUDE_LIBYUV_H_

#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>  // For size_t and NULL

#if !defined(INT_TYPES_DEFINED) && !defined(GG_LONGLONG)
#define INT_TYPES_DEFINED

#if defined(_MSC_VER) && (_MSC_VER < 1600)
#include <sys/types.h>  // for uintptr_t on x86
typedef unsigned __int64 uint64_t;
typedef __int64 int64_t;
typedef unsigned int uint32_t;
typedef int int32_t;
typedef unsigned short uint16_t;
typedef short int16_t;
typedef unsigned char uint8_t;
typedef signed char int8_t;
#else
#include <stdint.h>  // for uintptr_t and C99 types
#endif               // defined(_MSC_VER) && (_MSC_VER < 1600)
 // Types are deprecated.  Enable this macro for legacy types.
#ifdef LIBYUV_LEGACY_TYPES
typedef uint64_t uint64;
typedef int64_t int64;
typedef uint32_t uint32;
typedef int32_t int32;
typedef uint16_t uint16;
typedef int16_t int16;
typedef uint8_t uint8;
typedef int8_t int8;
#endif  // LIBYUV_LEGACY_TYPES
#endif  // INT_TYPES_DEFINED

#if !defined(LIBYUV_API)
#if defined(_WIN32) || defined(__CYGWIN__)
#if defined(LIBYUV_BUILDING_SHARED_LIBRARY)
#define LIBYUV_API __declspec(dllexport)
#elif defined(LIBYUV_USING_SHARED_LIBRARY)
#define LIBYUV_API __declspec(dllimport)
#else
#define LIBYUV_API
#endif  // LIBYUV_BUILDING_SHARED_LIBRARY
#elif defined(__GNUC__) && (__GNUC__ >= 4) && !defined(__APPLE__) && \
    (defined(LIBYUV_BUILDING_SHARED_LIBRARY) ||                      \
     defined(LIBYUV_USING_SHARED_LIBRARY))
#define LIBYUV_API __attribute__((visibility("default")))
#else
#define LIBYUV_API
#endif  // __GNUC__
#endif  // LIBYUV_API

// TODO(fbarchard): Remove bool macros.
#define LIBYUV_BOOL int
#define LIBYUV_FALSE 0
#define LIBYUV_TRUE 1




#ifdef __cplusplus
namespace libyuv {
    extern "C" {
#endif

        // Compute a hash for specified memory. Seed of 5381 recommended.
        LIBYUV_API
            uint32_t HashDjb2(const uint8_t* src, uint64_t count, uint32_t seed);

        // Hamming Distance
        LIBYUV_API
            uint64_t ComputeHammingDistance(const uint8_t* src_a,
                const uint8_t* src_b,
                int count);

        // Scan an opaque argb image and return fourcc based on alpha offset.
        // Returns FOURCC_ARGB, FOURCC_BGRA, or 0 if unknown.
        LIBYUV_API
            uint32_t ARGBDetect(const uint8_t* argb,
                int stride_argb,
                int width,
                int height);

        // Sum Square Error - used to compute Mean Square Error or PSNR.
        LIBYUV_API
            uint64_t ComputeSumSquareError(const uint8_t* src_a,
                const uint8_t* src_b,
                int count);

        LIBYUV_API
            uint64_t ComputeSumSquareErrorPlane(const uint8_t* src_a,
                int stride_a,
                const uint8_t* src_b,
                int stride_b,
                int width,
                int height);

        static const int kMaxPsnr = 128;

        LIBYUV_API
            double SumSquareErrorToPsnr(uint64_t sse, uint64_t count);

        LIBYUV_API
            double CalcFramePsnr(const uint8_t* src_a,
                int stride_a,
                const uint8_t* src_b,
                int stride_b,
                int width,
                int height);

        LIBYUV_API
            double I420Psnr(const uint8_t* src_y_a,
                int stride_y_a,
                const uint8_t* src_u_a,
                int stride_u_a,
                const uint8_t* src_v_a,
                int stride_v_a,
                const uint8_t* src_y_b,
                int stride_y_b,
                const uint8_t* src_u_b,
                int stride_u_b,
                const uint8_t* src_v_b,
                int stride_v_b,
                int width,
                int height);

        LIBYUV_API
            double CalcFrameSsim(const uint8_t* src_a,
                int stride_a,
                const uint8_t* src_b,
                int stride_b,
                int width,
                int height);

        LIBYUV_API
            double I420Ssim(const uint8_t* src_y_a,
                int stride_y_a,
                const uint8_t* src_u_a,
                int stride_u_a,
                const uint8_t* src_v_a,
                int stride_v_a,
                const uint8_t* src_y_b,
                int stride_y_b,
                const uint8_t* src_u_b,
                int stride_u_b,
                const uint8_t* src_v_b,
                int stride_v_b,
                int width,
                int height);

#ifdef __cplusplus
    }  // extern "C"
}  // namespace libyuv
#endif




#ifdef __cplusplus
namespace libyuv {
    extern "C" {
#endif

        // Supported rotation.
        typedef enum RotationMode {
            kRotate0 = 0,      // No rotation.
            kRotate90 = 90,    // Rotate 90 degrees clockwise.
            kRotate180 = 180,  // Rotate 180 degrees.
            kRotate270 = 270,  // Rotate 270 degrees clockwise.

            // Deprecated.
            kRotateNone = 0,
            kRotateClockwise = 90,
            kRotateCounterClockwise = 270,
        } RotationModeEnum;

        // Rotate I420 frame.
        LIBYUV_API
            int I420Rotate(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_u,
                int dst_stride_u,
                uint8_t* dst_v,
                int dst_stride_v,
                int width,
                int height,
                enum RotationMode mode);

        // Rotate I422 frame.
        LIBYUV_API
            int I422Rotate(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_u,
                int dst_stride_u,
                uint8_t* dst_v,
                int dst_stride_v,
                int width,
                int height,
                enum RotationMode mode);

        // Rotate I444 frame.
        LIBYUV_API
            int I444Rotate(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_u,
                int dst_stride_u,
                uint8_t* dst_v,
                int dst_stride_v,
                int width,
                int height,
                enum RotationMode mode);

        // Rotate I010 frame.
        LIBYUV_API
            int I010Rotate(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                uint16_t* dst_y,
                int dst_stride_y,
                uint16_t* dst_u,
                int dst_stride_u,
                uint16_t* dst_v,
                int dst_stride_v,
                int width,
                int height,
                enum RotationMode mode);

        // Rotate I210 frame.
        LIBYUV_API
            int I210Rotate(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                uint16_t* dst_y,
                int dst_stride_y,
                uint16_t* dst_u,
                int dst_stride_u,
                uint16_t* dst_v,
                int dst_stride_v,
                int width,
                int height,
                enum RotationMode mode);

        // Rotate I410 frame.
        LIBYUV_API
            int I410Rotate(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                uint16_t* dst_y,
                int dst_stride_y,
                uint16_t* dst_u,
                int dst_stride_u,
                uint16_t* dst_v,
                int dst_stride_v,
                int width,
                int height,
                enum RotationMode mode);

        // Rotate NV12 input and store in I420.
        LIBYUV_API
            int NV12ToI420Rotate(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_uv,
                int src_stride_uv,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_u,
                int dst_stride_u,
                uint8_t* dst_v,
                int dst_stride_v,
                int width,
                int height,
                enum RotationMode mode);

        // Convert Android420 to I420 with rotation.
        // "rotation" can be 0, 90, 180 or 270.
        LIBYUV_API
            int Android420ToI420Rotate(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                int src_pixel_stride_uv,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_u,
                int dst_stride_u,
                uint8_t* dst_v,
                int dst_stride_v,
                int width,
                int height,
                enum RotationMode rotation);

        // Rotate a plane by 0, 90, 180, or 270.
        LIBYUV_API
            int RotatePlane(const uint8_t* src,
                int src_stride,
                uint8_t* dst,
                int dst_stride,
                int width,
                int height,
                enum RotationMode mode);

        // Rotate planes by 90, 180, 270. Deprecated.
        LIBYUV_API
            void RotatePlane90(const uint8_t* src,
                int src_stride,
                uint8_t* dst,
                int dst_stride,
                int width,
                int height);

        LIBYUV_API
            void RotatePlane180(const uint8_t* src,
                int src_stride,
                uint8_t* dst,
                int dst_stride,
                int width,
                int height);

        LIBYUV_API
            void RotatePlane270(const uint8_t* src,
                int src_stride,
                uint8_t* dst,
                int dst_stride,
                int width,
                int height);

        // Rotate a plane by 0, 90, 180, or 270.
        LIBYUV_API
            int RotatePlane_16(const uint16_t* src,
                int src_stride,
                uint16_t* dst,
                int dst_stride,
                int width,
                int height,
                enum RotationMode mode);

        // Rotations for when U and V are interleaved.
        // These functions take one UV input pointer and
        // split the data into two buffers while
        // rotating them.
        // width and height expected to be half size for NV12.
        LIBYUV_API
            int SplitRotateUV(const uint8_t* src_uv,
                int src_stride_uv,
                uint8_t* dst_u,
                int dst_stride_u,
                uint8_t* dst_v,
                int dst_stride_v,
                int width,
                int height,
                enum RotationMode mode);

        LIBYUV_API
            void SplitRotateUV90(const uint8_t* src,
                int src_stride,
                uint8_t* dst_a,
                int dst_stride_a,
                uint8_t* dst_b,
                int dst_stride_b,
                int width,
                int height);

        LIBYUV_API
            void SplitRotateUV180(const uint8_t* src,
                int src_stride,
                uint8_t* dst_a,
                int dst_stride_a,
                uint8_t* dst_b,
                int dst_stride_b,
                int width,
                int height);

        LIBYUV_API
            void SplitRotateUV270(const uint8_t* src,
                int src_stride,
                uint8_t* dst_a,
                int dst_stride_a,
                uint8_t* dst_b,
                int dst_stride_b,
                int width,
                int height);

        // The 90 and 270 functions are based on transposes.
        // Doing a transpose with reversing the read/write
        // order will result in a rotation by +- 90 degrees.
        // Deprecated.
        LIBYUV_API
            void TransposePlane(const uint8_t* src,
                int src_stride,
                uint8_t* dst,
                int dst_stride,
                int width,
                int height);

        LIBYUV_API
            void SplitTransposeUV(const uint8_t* src,
                int src_stride,
                uint8_t* dst_a,
                int dst_stride_a,
                uint8_t* dst_b,
                int dst_stride_b,
                int width,
                int height);

#ifdef __cplusplus
    }  // extern "C"
}  // namespace libyuv
#endif



#ifdef __cplusplus
namespace libyuv {
    extern "C" {
#endif

        // Supported filtering.
        typedef enum FilterMode {
            kFilterNone = 0,      // Point sample; Fastest.
            kFilterLinear = 1,    // Filter horizontally only.
            kFilterBilinear = 2,  // Faster than box, but lower quality scaling down.
            kFilterBox = 3        // Highest quality.
        } FilterModeEnum;

        // Scale a YUV plane.
        // Returns 0 if successful.
        LIBYUV_API
            int ScalePlane(const uint8_t* src,
                int src_stride,
                int src_width,
                int src_height,
                uint8_t* dst,
                int dst_stride,
                int dst_width,
                int dst_height,
                enum FilterMode filtering);

        LIBYUV_API
            int ScalePlane_16(const uint16_t* src,
                int src_stride,
                int src_width,
                int src_height,
                uint16_t* dst,
                int dst_stride,
                int dst_width,
                int dst_height,
                enum FilterMode filtering);

        // Sample is expected to be in the low 12 bits.
        LIBYUV_API
            int ScalePlane_12(const uint16_t* src,
                int src_stride,
                int src_width,
                int src_height,
                uint16_t* dst,
                int dst_stride,
                int dst_width,
                int dst_height,
                enum FilterMode filtering);

        // Scales a YUV 4:2:0 image from the src width and height to the
        // dst width and height.
        // If filtering is kFilterNone, a simple nearest-neighbor algorithm is
        // used. This produces basic (blocky) quality at the fastest speed.
        // If filtering is kFilterBilinear, interpolation is used to produce a better
        // quality image, at the expense of speed.
        // If filtering is kFilterBox, averaging is used to produce ever better
        // quality image, at further expense of speed.
        // Returns 0 if successful.

        LIBYUV_API
            int I420Scale(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                int src_width,
                int src_height,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_u,
                int dst_stride_u,
                uint8_t* dst_v,
                int dst_stride_v,
                int dst_width,
                int dst_height,
                enum FilterMode filtering);

        LIBYUV_API
            int I420Scale_16(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                int src_width,
                int src_height,
                uint16_t* dst_y,
                int dst_stride_y,
                uint16_t* dst_u,
                int dst_stride_u,
                uint16_t* dst_v,
                int dst_stride_v,
                int dst_width,
                int dst_height,
                enum FilterMode filtering);

        LIBYUV_API
            int I420Scale_12(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                int src_width,
                int src_height,
                uint16_t* dst_y,
                int dst_stride_y,
                uint16_t* dst_u,
                int dst_stride_u,
                uint16_t* dst_v,
                int dst_stride_v,
                int dst_width,
                int dst_height,
                enum FilterMode filtering);

        // Scales a YUV 4:4:4 image from the src width and height to the
        // dst width and height.
        // If filtering is kFilterNone, a simple nearest-neighbor algorithm is
        // used. This produces basic (blocky) quality at the fastest speed.
        // If filtering is kFilterBilinear, interpolation is used to produce a better
        // quality image, at the expense of speed.
        // If filtering is kFilterBox, averaging is used to produce ever better
        // quality image, at further expense of speed.
        // Returns 0 if successful.

        LIBYUV_API
            int I444Scale(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                int src_width,
                int src_height,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_u,
                int dst_stride_u,
                uint8_t* dst_v,
                int dst_stride_v,
                int dst_width,
                int dst_height,
                enum FilterMode filtering);

        LIBYUV_API
            int I444Scale_16(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                int src_width,
                int src_height,
                uint16_t* dst_y,
                int dst_stride_y,
                uint16_t* dst_u,
                int dst_stride_u,
                uint16_t* dst_v,
                int dst_stride_v,
                int dst_width,
                int dst_height,
                enum FilterMode filtering);

        LIBYUV_API
            int I444Scale_12(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                int src_width,
                int src_height,
                uint16_t* dst_y,
                int dst_stride_y,
                uint16_t* dst_u,
                int dst_stride_u,
                uint16_t* dst_v,
                int dst_stride_v,
                int dst_width,
                int dst_height,
                enum FilterMode filtering);

        // Scales a YUV 4:2:2 image from the src width and height to the
        // dst width and height.
        // If filtering is kFilterNone, a simple nearest-neighbor algorithm is
        // used. This produces basic (blocky) quality at the fastest speed.
        // If filtering is kFilterBilinear, interpolation is used to produce a better
        // quality image, at the expense of speed.
        // If filtering is kFilterBox, averaging is used to produce ever better
        // quality image, at further expense of speed.
        // Returns 0 if successful.
        LIBYUV_API
            int I422Scale(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                int src_width,
                int src_height,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_u,
                int dst_stride_u,
                uint8_t* dst_v,
                int dst_stride_v,
                int dst_width,
                int dst_height,
                enum FilterMode filtering);

        LIBYUV_API
            int I422Scale_16(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                int src_width,
                int src_height,
                uint16_t* dst_y,
                int dst_stride_y,
                uint16_t* dst_u,
                int dst_stride_u,
                uint16_t* dst_v,
                int dst_stride_v,
                int dst_width,
                int dst_height,
                enum FilterMode filtering);

        LIBYUV_API
            int I422Scale_12(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                int src_width,
                int src_height,
                uint16_t* dst_y,
                int dst_stride_y,
                uint16_t* dst_u,
                int dst_stride_u,
                uint16_t* dst_v,
                int dst_stride_v,
                int dst_width,
                int dst_height,
                enum FilterMode filtering);

        // Scales an NV12 image from the src width and height to the
        // dst width and height.
        // If filtering is kFilterNone, a simple nearest-neighbor algorithm is
        // used. This produces basic (blocky) quality at the fastest speed.
        // If filtering is kFilterBilinear, interpolation is used to produce a better
        // quality image, at the expense of speed.
        // kFilterBox is not supported for the UV channel and will be treated as
        // bilinear.
        // Returns 0 if successful.

        LIBYUV_API
            int NV12Scale(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_uv,
                int src_stride_uv,
                int src_width,
                int src_height,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_uv,
                int dst_stride_uv,
                int dst_width,
                int dst_height,
                enum FilterMode filtering);

        LIBYUV_API
            int NV24Scale(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_uv,
                int src_stride_uv,
                int src_width,
                int src_height,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_uv,
                int dst_stride_uv,
                int dst_width,
                int dst_height,
                enum FilterMode filtering);

#ifdef __cplusplus
        // Legacy API.  Deprecated.
        LIBYUV_API
            int Scale(const uint8_t* src_y,
                const uint8_t* src_u,
                const uint8_t* src_v,
                int src_stride_y,
                int src_stride_u,
                int src_stride_v,
                int src_width,
                int src_height,
                uint8_t* dst_y,
                uint8_t* dst_u,
                uint8_t* dst_v,
                int dst_stride_y,
                int dst_stride_u,
                int dst_stride_v,
                int dst_width,
                int dst_height,
                LIBYUV_BOOL interpolate);

        // For testing, allow disabling of specialized scalers.
        LIBYUV_API
            void SetUseReferenceImpl(LIBYUV_BOOL use);
#endif  // __cplusplus

#ifdef __cplusplus
    }  // extern "C"
}  // namespace libyuv
#endif


#ifdef __cplusplus
namespace libyuv {
    extern "C" {
#endif

        // Conversion matrix for YUV to RGB
        LIBYUV_API extern const struct YuvConstants kYuvI601Constants;   // BT.601
        LIBYUV_API extern const struct YuvConstants kYuvJPEGConstants;   // BT.601 full
        LIBYUV_API extern const struct YuvConstants kYuvH709Constants;   // BT.709
        LIBYUV_API extern const struct YuvConstants kYuvF709Constants;   // BT.709 full
        LIBYUV_API extern const struct YuvConstants kYuv2020Constants;   // BT.2020
        LIBYUV_API extern const struct YuvConstants kYuvV2020Constants;  // BT.2020 full

        // Conversion matrix for YVU to BGR
        LIBYUV_API extern const struct YuvConstants kYvuI601Constants;   // BT.601
        LIBYUV_API extern const struct YuvConstants kYvuJPEGConstants;   // BT.601 full
        LIBYUV_API extern const struct YuvConstants kYvuH709Constants;   // BT.709
        LIBYUV_API extern const struct YuvConstants kYvuF709Constants;   // BT.709 full
        LIBYUV_API extern const struct YuvConstants kYvu2020Constants;   // BT.2020
        LIBYUV_API extern const struct YuvConstants kYvuV2020Constants;  // BT.2020 full

        // Macros for end swapped destination Matrix conversions.
        // Swap UV and pass mirrored kYvuJPEGConstants matrix.
        // TODO(fbarchard): Add macro for each Matrix function.
#define kYuvI601ConstantsVU kYvuI601Constants
#define kYuvJPEGConstantsVU kYvuJPEGConstants
#define kYuvH709ConstantsVU kYvuH709Constants
#define kYuvF709ConstantsVU kYvuF709Constants
#define kYuv2020ConstantsVU kYvu2020Constants
#define kYuvV2020ConstantsVU kYvuV2020Constants

#define NV12ToABGRMatrix(a, b, c, d, e, f, g, h, i) \
  NV21ToARGBMatrix(a, b, c, d, e, f, g##VU, h, i)
#define NV21ToABGRMatrix(a, b, c, d, e, f, g, h, i) \
  NV12ToARGBMatrix(a, b, c, d, e, f, g##VU, h, i)
#define NV12ToRAWMatrix(a, b, c, d, e, f, g, h, i) \
  NV21ToRGB24Matrix(a, b, c, d, e, f, g##VU, h, i)
#define NV21ToRAWMatrix(a, b, c, d, e, f, g, h, i) \
  NV12ToRGB24Matrix(a, b, c, d, e, f, g##VU, h, i)
#define I010ToABGRMatrix(a, b, c, d, e, f, g, h, i, j, k) \
  I010ToARGBMatrix(a, b, e, f, c, d, g, h, i##VU, j, k)
#define I210ToABGRMatrix(a, b, c, d, e, f, g, h, i, j, k) \
  I210ToARGBMatrix(a, b, e, f, c, d, g, h, i##VU, j, k)
#define I410ToABGRMatrix(a, b, c, d, e, f, g, h, i, j, k) \
  I410ToARGBMatrix(a, b, e, f, c, d, g, h, i##VU, j, k)
#define I010ToAB30Matrix(a, b, c, d, e, f, g, h, i, j, k) \
  I010ToAR30Matrix(a, b, e, f, c, d, g, h, i##VU, j, k)
#define I210ToAB30Matrix(a, b, c, d, e, f, g, h, i, j, k) \
  I210ToAR30Matrix(a, b, e, f, c, d, g, h, i##VU, j, k)
#define I410ToAB30Matrix(a, b, c, d, e, f, g, h, i, j, k) \
  I410ToAR30Matrix(a, b, e, f, c, d, g, h, i##VU, j, k)
#define I012ToAB30Matrix(a, b, c, d, e, f, g, h, i, j, k) \
  I012ToAR30Matrix(a, b, e, f, c, d, g, h, i##VU, j, k)
#define I420AlphaToABGRMatrix(a, b, c, d, e, f, g, h, i, j, k, l, m, n) \
  I420AlphaToARGBMatrix(a, b, e, f, c, d, g, h, i, j, k##VU, l, m, n)
#define I422AlphaToABGRMatrix(a, b, c, d, e, f, g, h, i, j, k, l, m, n) \
  I422AlphaToARGBMatrix(a, b, e, f, c, d, g, h, i, j, k##VU, l, m, n)
#define I444AlphaToABGRMatrix(a, b, c, d, e, f, g, h, i, j, k, l, m, n) \
  I444AlphaToARGBMatrix(a, b, e, f, c, d, g, h, i, j, k##VU, l, m, n)
#define I010AlphaToABGRMatrix(a, b, c, d, e, f, g, h, i, j, k, l, m, n) \
  I010AlphaToARGBMatrix(a, b, e, f, c, d, g, h, i, j, k##VU, l, m, n)
#define I210AlphaToABGRMatrix(a, b, c, d, e, f, g, h, i, j, k, l, m, n) \
  I210AlphaToARGBMatrix(a, b, e, f, c, d, g, h, i, j, k##VU, l, m, n)
#define I410AlphaToABGRMatrix(a, b, c, d, e, f, g, h, i, j, k, l, m, n) \
  I410AlphaToARGBMatrix(a, b, e, f, c, d, g, h, i, j, k##VU, l, m, n)

// Alias.
#define ARGBToARGB ARGBCopy

// Copy ARGB to ARGB.
        LIBYUV_API
            int ARGBCopy(const uint8_t* src_argb,
                int src_stride_argb,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int width,
                int height);

        // Convert I420 to ARGB.
        LIBYUV_API
            int I420ToARGB(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int width,
                int height);

        // Convert I420 to ABGR.
        LIBYUV_API
            int I420ToABGR(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_abgr,
                int dst_stride_abgr,
                int width,
                int height);

        // Convert J420 to ARGB.
        LIBYUV_API
            int J420ToARGB(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int width,
                int height);

        // Convert J420 to ABGR.
        LIBYUV_API
            int J420ToABGR(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_abgr,
                int dst_stride_abgr,
                int width,
                int height);

        // Convert H420 to ARGB.
        LIBYUV_API
            int H420ToARGB(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int width,
                int height);

        // Convert H420 to ABGR.
        LIBYUV_API
            int H420ToABGR(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_abgr,
                int dst_stride_abgr,
                int width,
                int height);

        // Convert U420 to ARGB.
        LIBYUV_API
            int U420ToARGB(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int width,
                int height);

        // Convert U420 to ABGR.
        LIBYUV_API
            int U420ToABGR(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_abgr,
                int dst_stride_abgr,
                int width,
                int height);

        // Convert I422 to ARGB.
        LIBYUV_API
            int I422ToARGB(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int width,
                int height);

        // Convert I422 to ABGR.
        LIBYUV_API
            int I422ToABGR(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_abgr,
                int dst_stride_abgr,
                int width,
                int height);

        // Convert J422 to ARGB.
        LIBYUV_API
            int J422ToARGB(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int width,
                int height);

        // Convert J422 to ABGR.
        LIBYUV_API
            int J422ToABGR(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_abgr,
                int dst_stride_abgr,
                int width,
                int height);

        // Convert H422 to ARGB.
        LIBYUV_API
            int H422ToARGB(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int width,
                int height);

        // Convert H422 to ABGR.
        LIBYUV_API
            int H422ToABGR(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_abgr,
                int dst_stride_abgr,
                int width,
                int height);

        // Convert U422 to ARGB.
        LIBYUV_API
            int U422ToARGB(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int width,
                int height);

        // Convert U422 to ABGR.
        LIBYUV_API
            int U422ToABGR(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_abgr,
                int dst_stride_abgr,
                int width,
                int height);

        // Convert I444 to ARGB.
        LIBYUV_API
            int I444ToARGB(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int width,
                int height);

        // Convert I444 to ABGR.
        LIBYUV_API
            int I444ToABGR(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_abgr,
                int dst_stride_abgr,
                int width,
                int height);

        // Convert J444 to ARGB.
        LIBYUV_API
            int J444ToARGB(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int width,
                int height);

        // Convert J444 to ABGR.
        LIBYUV_API
            int J444ToABGR(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_abgr,
                int dst_stride_abgr,
                int width,
                int height);

        // Convert H444 to ARGB.
        LIBYUV_API
            int H444ToARGB(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int width,
                int height);

        // Convert H444 to ABGR.
        LIBYUV_API
            int H444ToABGR(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_abgr,
                int dst_stride_abgr,
                int width,
                int height);

        // Convert U444 to ARGB.
        LIBYUV_API
            int U444ToARGB(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int width,
                int height);

        // Convert U444 to ABGR.
        LIBYUV_API
            int U444ToABGR(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_abgr,
                int dst_stride_abgr,
                int width,
                int height);

        // Convert I444 to RGB24.
        LIBYUV_API
            int I444ToRGB24(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_rgb24,
                int dst_stride_rgb24,
                int width,
                int height);

        // Convert I444 to RAW.
        LIBYUV_API
            int I444ToRAW(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_raw,
                int dst_stride_raw,
                int width,
                int height);

        // Convert I010 to ARGB.
        LIBYUV_API
            int I010ToARGB(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int width,
                int height);

        // Convert I010 to ABGR.
        LIBYUV_API
            int I010ToABGR(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                uint8_t* dst_abgr,
                int dst_stride_abgr,
                int width,
                int height);

        // Convert H010 to ARGB.
        LIBYUV_API
            int H010ToARGB(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int width,
                int height);

        // Convert H010 to ABGR.
        LIBYUV_API
            int H010ToABGR(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                uint8_t* dst_abgr,
                int dst_stride_abgr,
                int width,
                int height);

        // Convert U010 to ARGB.
        LIBYUV_API
            int U010ToARGB(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int width,
                int height);

        // Convert U010 to ABGR.
        LIBYUV_API
            int U010ToABGR(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                uint8_t* dst_abgr,
                int dst_stride_abgr,
                int width,
                int height);

        // Convert I210 to ARGB.
        LIBYUV_API
            int I210ToARGB(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int width,
                int height);

        // Convert I210 to ABGR.
        LIBYUV_API
            int I210ToABGR(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                uint8_t* dst_abgr,
                int dst_stride_abgr,
                int width,
                int height);

        // Convert H210 to ARGB.
        LIBYUV_API
            int H210ToARGB(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int width,
                int height);

        // Convert H210 to ABGR.
        LIBYUV_API
            int H210ToABGR(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                uint8_t* dst_abgr,
                int dst_stride_abgr,
                int width,
                int height);

        // Convert U210 to ARGB.
        LIBYUV_API
            int U210ToARGB(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int width,
                int height);

        // Convert U210 to ABGR.
        LIBYUV_API
            int U210ToABGR(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                uint8_t* dst_abgr,
                int dst_stride_abgr,
                int width,
                int height);

        // Convert I420 with Alpha to preattenuated ARGB.
        LIBYUV_API
            int I420AlphaToARGB(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                const uint8_t* src_a,
                int src_stride_a,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int width,
                int height,
                int attenuate);

        // Convert I420 with Alpha to preattenuated ABGR.
        LIBYUV_API
            int I420AlphaToABGR(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                const uint8_t* src_a,
                int src_stride_a,
                uint8_t* dst_abgr,
                int dst_stride_abgr,
                int width,
                int height,
                int attenuate);

        // Convert I422 with Alpha to preattenuated ARGB.
        LIBYUV_API
            int I422AlphaToARGB(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                const uint8_t* src_a,
                int src_stride_a,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int width,
                int height,
                int attenuate);

        // Convert I422 with Alpha to preattenuated ABGR.
        LIBYUV_API
            int I422AlphaToABGR(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                const uint8_t* src_a,
                int src_stride_a,
                uint8_t* dst_abgr,
                int dst_stride_abgr,
                int width,
                int height,
                int attenuate);

        // Convert I444 with Alpha to preattenuated ARGB.
        LIBYUV_API
            int I444AlphaToARGB(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                const uint8_t* src_a,
                int src_stride_a,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int width,
                int height,
                int attenuate);

        // Convert I444 with Alpha to preattenuated ABGR.
        LIBYUV_API
            int I444AlphaToABGR(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                const uint8_t* src_a,
                int src_stride_a,
                uint8_t* dst_abgr,
                int dst_stride_abgr,
                int width,
                int height,
                int attenuate);

        // Convert I400 (grey) to ARGB.  Reverse of ARGBToI400.
        LIBYUV_API
            int I400ToARGB(const uint8_t* src_y,
                int src_stride_y,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int width,
                int height);

        // Convert J400 (jpeg grey) to ARGB.
        LIBYUV_API
            int J400ToARGB(const uint8_t* src_y,
                int src_stride_y,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int width,
                int height);

        // Alias.
#define YToARGB I400ToARGB

// Convert NV12 to ARGB.
        LIBYUV_API
            int NV12ToARGB(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_uv,
                int src_stride_uv,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int width,
                int height);

        // Convert NV21 to ARGB.
        LIBYUV_API
            int NV21ToARGB(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_vu,
                int src_stride_vu,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int width,
                int height);

        // Convert NV12 to ABGR.
        LIBYUV_API
            int NV12ToABGR(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_uv,
                int src_stride_uv,
                uint8_t* dst_abgr,
                int dst_stride_abgr,
                int width,
                int height);

        // Convert NV21 to ABGR.
        LIBYUV_API
            int NV21ToABGR(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_vu,
                int src_stride_vu,
                uint8_t* dst_abgr,
                int dst_stride_abgr,
                int width,
                int height);

        // Convert NV12 to RGB24.
        LIBYUV_API
            int NV12ToRGB24(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_uv,
                int src_stride_uv,
                uint8_t* dst_rgb24,
                int dst_stride_rgb24,
                int width,
                int height);

        // Convert NV21 to RGB24.
        LIBYUV_API
            int NV21ToRGB24(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_vu,
                int src_stride_vu,
                uint8_t* dst_rgb24,
                int dst_stride_rgb24,
                int width,
                int height);

        // Convert NV21 to YUV24.
        LIBYUV_API
            int NV21ToYUV24(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_vu,
                int src_stride_vu,
                uint8_t* dst_yuv24,
                int dst_stride_yuv24,
                int width,
                int height);

        // Convert NV12 to RAW.
        LIBYUV_API
            int NV12ToRAW(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_uv,
                int src_stride_uv,
                uint8_t* dst_raw,
                int dst_stride_raw,
                int width,
                int height);

        // Convert NV21 to RAW.
        LIBYUV_API
            int NV21ToRAW(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_vu,
                int src_stride_vu,
                uint8_t* dst_raw,
                int dst_stride_raw,
                int width,
                int height);

        // Convert YUY2 to ARGB.
        LIBYUV_API
            int YUY2ToARGB(const uint8_t* src_yuy2,
                int src_stride_yuy2,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int width,
                int height);

        // Convert UYVY to ARGB.
        LIBYUV_API
            int UYVYToARGB(const uint8_t* src_uyvy,
                int src_stride_uyvy,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int width,
                int height);

        // Convert I010 to AR30.
        LIBYUV_API
            int I010ToAR30(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                uint8_t* dst_ar30,
                int dst_stride_ar30,
                int width,
                int height);

        // Convert H010 to AR30.
        LIBYUV_API
            int H010ToAR30(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                uint8_t* dst_ar30,
                int dst_stride_ar30,
                int width,
                int height);

        // Convert I010 to AB30.
        LIBYUV_API
            int I010ToAB30(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                uint8_t* dst_ab30,
                int dst_stride_ab30,
                int width,
                int height);

        // Convert H010 to AB30.
        LIBYUV_API
            int H010ToAB30(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                uint8_t* dst_ab30,
                int dst_stride_ab30,
                int width,
                int height);

        // Convert U010 to AR30.
        LIBYUV_API
            int U010ToAR30(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                uint8_t* dst_ar30,
                int dst_stride_ar30,
                int width,
                int height);

        // Convert U010 to AB30.
        LIBYUV_API
            int U010ToAB30(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                uint8_t* dst_ab30,
                int dst_stride_ab30,
                int width,
                int height);

        // Convert I210 to AR30.
        LIBYUV_API
            int I210ToAR30(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                uint8_t* dst_ar30,
                int dst_stride_ar30,
                int width,
                int height);

        // Convert I210 to AB30.
        LIBYUV_API
            int I210ToAB30(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                uint8_t* dst_ab30,
                int dst_stride_ab30,
                int width,
                int height);

        // Convert H210 to AR30.
        LIBYUV_API
            int H210ToAR30(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                uint8_t* dst_ar30,
                int dst_stride_ar30,
                int width,
                int height);

        // Convert H210 to AB30.
        LIBYUV_API
            int H210ToAB30(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                uint8_t* dst_ab30,
                int dst_stride_ab30,
                int width,
                int height);

        // Convert U210 to AR30.
        LIBYUV_API
            int U210ToAR30(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                uint8_t* dst_ar30,
                int dst_stride_ar30,
                int width,
                int height);

        // Convert U210 to AB30.
        LIBYUV_API
            int U210ToAB30(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                uint8_t* dst_ab30,
                int dst_stride_ab30,
                int width,
                int height);

        // BGRA little endian (argb in memory) to ARGB.
        LIBYUV_API
            int BGRAToARGB(const uint8_t* src_bgra,
                int src_stride_bgra,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int width,
                int height);

        // ABGR little endian (rgba in memory) to ARGB.
        LIBYUV_API
            int ABGRToARGB(const uint8_t* src_abgr,
                int src_stride_abgr,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int width,
                int height);

        // RGBA little endian (abgr in memory) to ARGB.
        LIBYUV_API
            int RGBAToARGB(const uint8_t* src_rgba,
                int src_stride_rgba,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int width,
                int height);

        // Deprecated function name.
#define BG24ToARGB RGB24ToARGB

// RGB little endian (bgr in memory) to ARGB.
        LIBYUV_API
            int RGB24ToARGB(const uint8_t* src_rgb24,
                int src_stride_rgb24,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int width,
                int height);

        // RGB big endian (rgb in memory) to ARGB.
        LIBYUV_API
            int RAWToARGB(const uint8_t* src_raw,
                int src_stride_raw,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int width,
                int height);

        // RGB big endian (rgb in memory) to RGBA.
        LIBYUV_API
            int RAWToRGBA(const uint8_t* src_raw,
                int src_stride_raw,
                uint8_t* dst_rgba,
                int dst_stride_rgba,
                int width,
                int height);

        // RGB16 (RGBP fourcc) little endian to ARGB.
        LIBYUV_API
            int RGB565ToARGB(const uint8_t* src_rgb565,
                int src_stride_rgb565,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int width,
                int height);

        // RGB15 (RGBO fourcc) little endian to ARGB.
        LIBYUV_API
            int ARGB1555ToARGB(const uint8_t* src_argb1555,
                int src_stride_argb1555,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int width,
                int height);

        // RGB12 (R444 fourcc) little endian to ARGB.
        LIBYUV_API
            int ARGB4444ToARGB(const uint8_t* src_argb4444,
                int src_stride_argb4444,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int width,
                int height);

        // Aliases
#define AB30ToARGB AR30ToABGR
#define AB30ToABGR AR30ToARGB
#define AB30ToAR30 AR30ToAB30

// Convert AR30 To ARGB.
        LIBYUV_API
            int AR30ToARGB(const uint8_t* src_ar30,
                int src_stride_ar30,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int width,
                int height);

        // Convert AR30 To ABGR.
        LIBYUV_API
            int AR30ToABGR(const uint8_t* src_ar30,
                int src_stride_ar30,
                uint8_t* dst_abgr,
                int dst_stride_abgr,
                int width,
                int height);

        // Convert AR30 To AB30.
        LIBYUV_API
            int AR30ToAB30(const uint8_t* src_ar30,
                int src_stride_ar30,
                uint8_t* dst_ab30,
                int dst_stride_ab30,
                int width,
                int height);

        // Convert AR64 to ARGB.
        LIBYUV_API
            int AR64ToARGB(const uint16_t* src_ar64,
                int src_stride_ar64,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int width,
                int height);

        // Convert AB64 to ABGR.
#define AB64ToABGR AR64ToARGB

// Convert AB64 to ARGB.
        LIBYUV_API
            int AB64ToARGB(const uint16_t* src_ab64,
                int src_stride_ab64,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int width,
                int height);

        // Convert AR64 to ABGR.
#define AR64ToABGR AB64ToARGB

// Convert AR64 To AB64.
        LIBYUV_API
            int AR64ToAB64(const uint16_t* src_ar64,
                int src_stride_ar64,
                uint16_t* dst_ab64,
                int dst_stride_ab64,
                int width,
                int height);

        // Convert AB64 To AR64.
#define AB64ToAR64 AR64ToAB64

// src_width/height provided by capture
// dst_width/height for clipping determine final size.
        LIBYUV_API
            int MJPGToARGB(const uint8_t* sample,
                size_t sample_size,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int src_width,
                int src_height,
                int dst_width,
                int dst_height);

        // Convert Android420 to ARGB.
        LIBYUV_API
            int Android420ToARGB(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                int src_pixel_stride_uv,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int width,
                int height);

        // Convert Android420 to ABGR.
        LIBYUV_API
            int Android420ToABGR(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                int src_pixel_stride_uv,
                uint8_t* dst_abgr,
                int dst_stride_abgr,
                int width,
                int height);

        // Convert NV12 to RGB565.
        LIBYUV_API
            int NV12ToRGB565(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_uv,
                int src_stride_uv,
                uint8_t* dst_rgb565,
                int dst_stride_rgb565,
                int width,
                int height);

        // Convert I422 to BGRA.
        LIBYUV_API
            int I422ToBGRA(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_bgra,
                int dst_stride_bgra,
                int width,
                int height);

        // Convert I422 to ABGR.
        LIBYUV_API
            int I422ToABGR(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_abgr,
                int dst_stride_abgr,
                int width,
                int height);

        // Convert I422 to RGBA.
        LIBYUV_API
            int I422ToRGBA(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_rgba,
                int dst_stride_rgba,
                int width,
                int height);

        LIBYUV_API
            int I420ToARGB(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int width,
                int height);

        LIBYUV_API
            int I420ToBGRA(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_bgra,
                int dst_stride_bgra,
                int width,
                int height);

        LIBYUV_API
            int I420ToABGR(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_abgr,
                int dst_stride_abgr,
                int width,
                int height);

        LIBYUV_API
            int I420ToRGBA(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_rgba,
                int dst_stride_rgba,
                int width,
                int height);

        LIBYUV_API
            int I420ToRGB24(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_rgb24,
                int dst_stride_rgb24,
                int width,
                int height);

        LIBYUV_API
            int I420ToRAW(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_raw,
                int dst_stride_raw,
                int width,
                int height);

        LIBYUV_API
            int H420ToRGB24(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_rgb24,
                int dst_stride_rgb24,
                int width,
                int height);

        LIBYUV_API
            int H420ToRAW(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_raw,
                int dst_stride_raw,
                int width,
                int height);

        LIBYUV_API
            int J420ToRGB24(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_rgb24,
                int dst_stride_rgb24,
                int width,
                int height);

        LIBYUV_API
            int J420ToRAW(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_raw,
                int dst_stride_raw,
                int width,
                int height);

        // Convert I422 to RGB24.
        LIBYUV_API
            int I422ToRGB24(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_rgb24,
                int dst_stride_rgb24,
                int width,
                int height);

        // Convert I422 to RAW.
        LIBYUV_API
            int I422ToRAW(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_raw,
                int dst_stride_raw,
                int width,
                int height);

        LIBYUV_API
            int I420ToRGB565(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_rgb565,
                int dst_stride_rgb565,
                int width,
                int height);

        LIBYUV_API
            int J420ToRGB565(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_rgb565,
                int dst_stride_rgb565,
                int width,
                int height);

        LIBYUV_API
            int H420ToRGB565(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_rgb565,
                int dst_stride_rgb565,
                int width,
                int height);

        LIBYUV_API
            int I422ToRGB565(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_rgb565,
                int dst_stride_rgb565,
                int width,
                int height);

        // Convert I420 To RGB565 with 4x4 dither matrix (16 bytes).
        // Values in dither matrix from 0 to 7 recommended.
        // The order of the dither matrix is first byte is upper left.

        LIBYUV_API
            int I420ToRGB565Dither(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_rgb565,
                int dst_stride_rgb565,
                const uint8_t* dither4x4,
                int width,
                int height);

        LIBYUV_API
            int I420ToARGB1555(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_argb1555,
                int dst_stride_argb1555,
                int width,
                int height);

        LIBYUV_API
            int I420ToARGB4444(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_argb4444,
                int dst_stride_argb4444,
                int width,
                int height);

        // Convert I420 to AR30.
        LIBYUV_API
            int I420ToAR30(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_ar30,
                int dst_stride_ar30,
                int width,
                int height);

        // Convert I420 to AB30.
        LIBYUV_API
            int I420ToAB30(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_ab30,
                int dst_stride_ab30,
                int width,
                int height);

        // Convert H420 to AR30.
        LIBYUV_API
            int H420ToAR30(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_ar30,
                int dst_stride_ar30,
                int width,
                int height);

        // Convert H420 to AB30.
        LIBYUV_API
            int H420ToAB30(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_ab30,
                int dst_stride_ab30,
                int width,
                int height);

        // Convert I420 to ARGB with matrix.
        LIBYUV_API
            int I420ToARGBMatrix(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_argb,
                int dst_stride_argb,
                const struct YuvConstants* yuvconstants,
                int width,
                int height);

        // Convert I422 to ARGB with matrix.
        LIBYUV_API
            int I422ToARGBMatrix(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_argb,
                int dst_stride_argb,
                const struct YuvConstants* yuvconstants,
                int width,
                int height);

        // Convert I444 to ARGB with matrix.
        LIBYUV_API
            int I444ToARGBMatrix(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_argb,
                int dst_stride_argb,
                const struct YuvConstants* yuvconstants,
                int width,
                int height);

        // Convert I444 to RGB24 with matrix.
        LIBYUV_API
            int I444ToRGB24Matrix(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_rgb24,
                int dst_stride_rgb24,
                const struct YuvConstants* yuvconstants,
                int width,
                int height);

        // Convert 10 bit 420 YUV to ARGB with matrix.
        LIBYUV_API
            int I010ToAR30Matrix(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                uint8_t* dst_ar30,
                int dst_stride_ar30,
                const struct YuvConstants* yuvconstants,
                int width,
                int height);

        // Convert 10 bit 420 YUV to ARGB with matrix.
        LIBYUV_API
            int I210ToAR30Matrix(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                uint8_t* dst_ar30,
                int dst_stride_ar30,
                const struct YuvConstants* yuvconstants,
                int width,
                int height);

        // Convert 10 bit 444 YUV to ARGB with matrix.
        LIBYUV_API
            int I410ToAR30Matrix(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                uint8_t* dst_ar30,
                int dst_stride_ar30,
                const struct YuvConstants* yuvconstants,
                int width,
                int height);

        // Convert 10 bit YUV to ARGB with matrix.
        LIBYUV_API
            int I010ToARGBMatrix(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                uint8_t* dst_argb,
                int dst_stride_argb,
                const struct YuvConstants* yuvconstants,
                int width,
                int height);

        // multiply 12 bit yuv into high bits to allow any number of bits.
        LIBYUV_API
            int I012ToAR30Matrix(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                uint8_t* dst_ar30,
                int dst_stride_ar30,
                const struct YuvConstants* yuvconstants,
                int width,
                int height);

        // Convert 12 bit YUV to ARGB with matrix.
        LIBYUV_API
            int I012ToARGBMatrix(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                uint8_t* dst_argb,
                int dst_stride_argb,
                const struct YuvConstants* yuvconstants,
                int width,
                int height);

        // Convert 10 bit 422 YUV to ARGB with matrix.
        LIBYUV_API
            int I210ToARGBMatrix(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                uint8_t* dst_argb,
                int dst_stride_argb,
                const struct YuvConstants* yuvconstants,
                int width,
                int height);

        // Convert 10 bit 444 YUV to ARGB with matrix.
        LIBYUV_API
            int I410ToARGBMatrix(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                uint8_t* dst_argb,
                int dst_stride_argb,
                const struct YuvConstants* yuvconstants,
                int width,
                int height);

        // Convert P010 to ARGB with matrix.
        LIBYUV_API
            int P010ToARGBMatrix(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_uv,
                int src_stride_uv,
                uint8_t* dst_argb,
                int dst_stride_argb,
                const struct YuvConstants* yuvconstants,
                int width,
                int height);

        // Convert P210 to ARGB with matrix.
        LIBYUV_API
            int P210ToARGBMatrix(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_uv,
                int src_stride_uv,
                uint8_t* dst_argb,
                int dst_stride_argb,
                const struct YuvConstants* yuvconstants,
                int width,
                int height);

        // Convert P010 to AR30 with matrix.
        LIBYUV_API
            int P010ToAR30Matrix(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_uv,
                int src_stride_uv,
                uint8_t* dst_ar30,
                int dst_stride_ar30,
                const struct YuvConstants* yuvconstants,
                int width,
                int height);

        // Convert P210 to AR30 with matrix.
        LIBYUV_API
            int P210ToAR30Matrix(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_uv,
                int src_stride_uv,
                uint8_t* dst_ar30,
                int dst_stride_ar30,
                const struct YuvConstants* yuvconstants,
                int width,
                int height);

        // P012 and P010 use most significant bits so the conversion is the same.
        // Convert P012 to ARGB with matrix.
#define P012ToARGBMatrix P010ToARGBMatrix
// Convert P012 to AR30 with matrix.
#define P012ToAR30Matrix P010ToAR30Matrix
// Convert P212 to ARGB with matrix.
#define P212ToARGBMatrix P210ToARGBMatrix
// Convert P212 to AR30 with matrix.
#define P212ToAR30Matrix P210ToAR30Matrix

// Convert P016 to ARGB with matrix.
#define P016ToARGBMatrix P010ToARGBMatrix
// Convert P016 to AR30 with matrix.
#define P016ToAR30Matrix P010ToAR30Matrix
// Convert P216 to ARGB with matrix.
#define P216ToARGBMatrix P210ToARGBMatrix
// Convert P216 to AR30 with matrix.
#define P216ToAR30Matrix P210ToAR30Matrix

// Convert I420 with Alpha to preattenuated ARGB with matrix.
        LIBYUV_API
            int I420AlphaToARGBMatrix(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                const uint8_t* src_a,
                int src_stride_a,
                uint8_t* dst_argb,
                int dst_stride_argb,
                const struct YuvConstants* yuvconstants,
                int width,
                int height,
                int attenuate);

        // Convert I422 with Alpha to preattenuated ARGB with matrix.
        LIBYUV_API
            int I422AlphaToARGBMatrix(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                const uint8_t* src_a,
                int src_stride_a,
                uint8_t* dst_argb,
                int dst_stride_argb,
                const struct YuvConstants* yuvconstants,
                int width,
                int height,
                int attenuate);

        // Convert I444 with Alpha to preattenuated ARGB with matrix.
        LIBYUV_API
            int I444AlphaToARGBMatrix(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                const uint8_t* src_a,
                int src_stride_a,
                uint8_t* dst_argb,
                int dst_stride_argb,
                const struct YuvConstants* yuvconstants,
                int width,
                int height,
                int attenuate);

        // Convert I010 with Alpha to preattenuated ARGB with matrix.
        LIBYUV_API
            int I010AlphaToARGBMatrix(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                const uint16_t* src_a,
                int src_stride_a,
                uint8_t* dst_argb,
                int dst_stride_argb,
                const struct YuvConstants* yuvconstants,
                int width,
                int height,
                int attenuate);

        // Convert I210 with Alpha to preattenuated ARGB with matrix.
        LIBYUV_API
            int I210AlphaToARGBMatrix(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                const uint16_t* src_a,
                int src_stride_a,
                uint8_t* dst_argb,
                int dst_stride_argb,
                const struct YuvConstants* yuvconstants,
                int width,
                int height,
                int attenuate);

        // Convert I410 with Alpha to preattenuated ARGB with matrix.
        LIBYUV_API
            int I410AlphaToARGBMatrix(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                const uint16_t* src_a,
                int src_stride_a,
                uint8_t* dst_argb,
                int dst_stride_argb,
                const struct YuvConstants* yuvconstants,
                int width,
                int height,
                int attenuate);

        // Convert NV12 to ARGB with matrix.
        LIBYUV_API
            int NV12ToARGBMatrix(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_uv,
                int src_stride_uv,
                uint8_t* dst_argb,
                int dst_stride_argb,
                const struct YuvConstants* yuvconstants,
                int width,
                int height);

        // Convert NV21 to ARGB with matrix.
        LIBYUV_API
            int NV21ToARGBMatrix(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_vu,
                int src_stride_vu,
                uint8_t* dst_argb,
                int dst_stride_argb,
                const struct YuvConstants* yuvconstants,
                int width,
                int height);

        // Convert NV12 to RGB565 with matrix.
        LIBYUV_API
            int NV12ToRGB565Matrix(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_uv,
                int src_stride_uv,
                uint8_t* dst_rgb565,
                int dst_stride_rgb565,
                const struct YuvConstants* yuvconstants,
                int width,
                int height);

        // Convert NV12 to RGB24 with matrix.
        LIBYUV_API
            int NV12ToRGB24Matrix(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_uv,
                int src_stride_uv,
                uint8_t* dst_rgb24,
                int dst_stride_rgb24,
                const struct YuvConstants* yuvconstants,
                int width,
                int height);

        // Convert NV21 to RGB24 with matrix.
        LIBYUV_API
            int NV21ToRGB24Matrix(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_vu,
                int src_stride_vu,
                uint8_t* dst_rgb24,
                int dst_stride_rgb24,
                const struct YuvConstants* yuvconstants,
                int width,
                int height);

        // Convert YUY2 to ARGB with matrix.
        LIBYUV_API
            int YUY2ToARGBMatrix(const uint8_t* src_yuy2,
                int src_stride_yuy2,
                uint8_t* dst_argb,
                int dst_stride_argb,
                const struct YuvConstants* yuvconstants,
                int width,
                int height);

        // Convert UYVY to ARGB with matrix.
        LIBYUV_API
            int UYVYToARGBMatrix(const uint8_t* src_uyvy,
                int src_stride_uyvy,
                uint8_t* dst_argb,
                int dst_stride_argb,
                const struct YuvConstants* yuvconstants,
                int width,
                int height);

        // Convert Android420 to ARGB with matrix.
        LIBYUV_API
            int Android420ToARGBMatrix(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                int src_pixel_stride_uv,
                uint8_t* dst_argb,
                int dst_stride_argb,
                const struct YuvConstants* yuvconstants,
                int width,
                int height);

        // Convert I422 to RGBA with matrix.
        LIBYUV_API
            int I422ToRGBAMatrix(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_rgba,
                int dst_stride_rgba,
                const struct YuvConstants* yuvconstants,
                int width,
                int height);

        // Convert I420 to RGBA with matrix.
        LIBYUV_API
            int I420ToRGBAMatrix(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_rgba,
                int dst_stride_rgba,
                const struct YuvConstants* yuvconstants,
                int width,
                int height);

        // Convert I420 to RGB24 with matrix.
        LIBYUV_API
            int I420ToRGB24Matrix(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_rgb24,
                int dst_stride_rgb24,
                const struct YuvConstants* yuvconstants,
                int width,
                int height);

        // Convert I422 to RGB24 with matrix.
        LIBYUV_API
            int I422ToRGB24Matrix(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_rgb24,
                int dst_stride_rgb24,
                const struct YuvConstants* yuvconstants,
                int width,
                int height);

        // Convert I420 to RGB565 with specified color matrix.
        LIBYUV_API
            int I420ToRGB565Matrix(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_rgb565,
                int dst_stride_rgb565,
                const struct YuvConstants* yuvconstants,
                int width,
                int height);

        // Convert I422 to RGB565 with specified color matrix.
        LIBYUV_API
            int I422ToRGB565Matrix(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_rgb565,
                int dst_stride_rgb565,
                const struct YuvConstants* yuvconstants,
                int width,
                int height);

        // Convert I420 to AR30 with matrix.
        LIBYUV_API
            int I420ToAR30Matrix(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_ar30,
                int dst_stride_ar30,
                const struct YuvConstants* yuvconstants,
                int width,
                int height);

        // Convert I400 (grey) to ARGB.  Reverse of ARGBToI400.
        LIBYUV_API
            int I400ToARGBMatrix(const uint8_t* src_y,
                int src_stride_y,
                uint8_t* dst_argb,
                int dst_stride_argb,
                const struct YuvConstants* yuvconstants,
                int width,
                int height);

        // Convert I420 to ARGB with matrix and UV filter mode.
        LIBYUV_API
            int I420ToARGBMatrixFilter(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_argb,
                int dst_stride_argb,
                const struct YuvConstants* yuvconstants,
                int width,
                int height,
                enum FilterMode filter);

        // Convert I422 to ARGB with matrix and UV filter mode.
        LIBYUV_API
            int I422ToARGBMatrixFilter(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_argb,
                int dst_stride_argb,
                const struct YuvConstants* yuvconstants,
                int width,
                int height,
                enum FilterMode filter);

        // Convert I422 to RGB24 with matrix and UV filter mode.
        LIBYUV_API
            int I422ToRGB24MatrixFilter(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_rgb24,
                int dst_stride_rgb24,
                const struct YuvConstants* yuvconstants,
                int width,
                int height,
                enum FilterMode filter);

        // Convert I420 to RGB24 with matrix and UV filter mode.
        LIBYUV_API
            int I420ToRGB24MatrixFilter(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_rgb24,
                int dst_stride_rgb24,
                const struct YuvConstants* yuvconstants,
                int width,
                int height,
                enum FilterMode filter);

        // Convert I010 to AR30 with matrix and UV filter mode.
        LIBYUV_API
            int I010ToAR30MatrixFilter(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                uint8_t* dst_ar30,
                int dst_stride_ar30,
                const struct YuvConstants* yuvconstants,
                int width,
                int height,
                enum FilterMode filter);

        // Convert I210 to AR30 with matrix and UV filter mode.
        LIBYUV_API
            int I210ToAR30MatrixFilter(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                uint8_t* dst_ar30,
                int dst_stride_ar30,
                const struct YuvConstants* yuvconstants,
                int width,
                int height,
                enum FilterMode filter);

        // Convert I010 to ARGB with matrix and UV filter mode.
        LIBYUV_API
            int I010ToARGBMatrixFilter(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                uint8_t* dst_argb,
                int dst_stride_argb,
                const struct YuvConstants* yuvconstants,
                int width,
                int height,
                enum FilterMode filter);

        // Convert I210 to ARGB with matrix and UV filter mode.
        LIBYUV_API
            int I210ToARGBMatrixFilter(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                uint8_t* dst_argb,
                int dst_stride_argb,
                const struct YuvConstants* yuvconstants,
                int width,
                int height,
                enum FilterMode filter);

        // Convert I420 with Alpha to attenuated ARGB with matrix and UV filter mode.
        LIBYUV_API
            int I420AlphaToARGBMatrixFilter(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                const uint8_t* src_a,
                int src_stride_a,
                uint8_t* dst_argb,
                int dst_stride_argb,
                const struct YuvConstants* yuvconstants,
                int width,
                int height,
                int attenuate,
                enum FilterMode filter);

        // Convert I422 with Alpha to attenuated ARGB with matrix and UV filter mode.
        LIBYUV_API
            int I422AlphaToARGBMatrixFilter(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                const uint8_t* src_a,
                int src_stride_a,
                uint8_t* dst_argb,
                int dst_stride_argb,
                const struct YuvConstants* yuvconstants,
                int width,
                int height,
                int attenuate,
                enum FilterMode filter);

        // Convert I010 with Alpha to attenuated ARGB with matrix and UV filter mode.
        LIBYUV_API
            int I010AlphaToARGBMatrixFilter(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                const uint16_t* src_a,
                int src_stride_a,
                uint8_t* dst_argb,
                int dst_stride_argb,
                const struct YuvConstants* yuvconstants,
                int width,
                int height,
                int attenuate,
                enum FilterMode filter);

        // Convert I210 with Alpha to attenuated ARGB with matrix and UV filter mode.
        LIBYUV_API
            int I210AlphaToARGBMatrixFilter(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                const uint16_t* src_a,
                int src_stride_a,
                uint8_t* dst_argb,
                int dst_stride_argb,
                const struct YuvConstants* yuvconstants,
                int width,
                int height,
                int attenuate,
                enum FilterMode filter);

        // Convert P010 to ARGB with matrix and UV filter mode.
        LIBYUV_API
            int P010ToARGBMatrixFilter(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_uv,
                int src_stride_uv,
                uint8_t* dst_argb,
                int dst_stride_argb,
                const struct YuvConstants* yuvconstants,
                int width,
                int height,
                enum FilterMode filter);

        // Convert P210 to ARGB with matrix and UV filter mode.
        LIBYUV_API
            int P210ToARGBMatrixFilter(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_uv,
                int src_stride_uv,
                uint8_t* dst_argb,
                int dst_stride_argb,
                const struct YuvConstants* yuvconstants,
                int width,
                int height,
                enum FilterMode filter);

        // Convert P010 to AR30 with matrix and UV filter mode.
        LIBYUV_API
            int P010ToAR30MatrixFilter(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_uv,
                int src_stride_uv,
                uint8_t* dst_ar30,
                int dst_stride_ar30,
                const struct YuvConstants* yuvconstants,
                int width,
                int height,
                enum FilterMode filter);

        // Convert P210 to AR30 with matrix and UV filter mode.
        LIBYUV_API
            int P210ToAR30MatrixFilter(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_uv,
                int src_stride_uv,
                uint8_t* dst_ar30,
                int dst_stride_ar30,
                const struct YuvConstants* yuvconstants,
                int width,
                int height,
                enum FilterMode filter);

        // Convert camera sample to ARGB with cropping, rotation and vertical flip.
        // "sample_size" is needed to parse MJPG.
        // "dst_stride_argb" number of bytes in a row of the dst_argb plane.
        //   Normally this would be the same as dst_width, with recommended alignment
        //   to 16 bytes for better efficiency.
        //   If rotation of 90 or 270 is used, stride is affected. The caller should
        //   allocate the I420 buffer according to rotation.
        // "dst_stride_u" number of bytes in a row of the dst_u plane.
        //   Normally this would be the same as (dst_width + 1) / 2, with
        //   recommended alignment to 16 bytes for better efficiency.
        //   If rotation of 90 or 270 is used, stride is affected.
        // "crop_x" and "crop_y" are starting position for cropping.
        //   To center, crop_x = (src_width - dst_width) / 2
        //              crop_y = (src_height - dst_height) / 2
        // "src_width" / "src_height" is size of src_frame in pixels.
        //   "src_height" can be negative indicating a vertically flipped image source.
        // "crop_width" / "crop_height" is the size to crop the src to.
        //    Must be less than or equal to src_width/src_height
        //    Cropping parameters are pre-rotation.
        // "rotation" can be 0, 90, 180 or 270.
        // "fourcc" is a fourcc. ie 'I420', 'YUY2'
        // Returns 0 for successful; -1 for invalid parameter. Non-zero for failure.
        LIBYUV_API
            int ConvertToARGB(const uint8_t* sample,
                size_t sample_size,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int crop_x,
                int crop_y,
                int src_width,
                int src_height,
                int crop_width,
                int crop_height,
                enum RotationMode rotation,
                uint32_t fourcc);

#ifdef __cplusplus
    }  // extern "C"
}  // namespace libyuv
#endif





#ifdef __cplusplus
namespace libyuv {
    extern "C" {
#endif

        // See Also convert.h for conversions from formats to I420.

        // Convert 8 bit YUV to 10 bit.
#define H420ToH010 I420ToI010
        LIBYUV_API
            int I420ToI010(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint16_t* dst_y,
                int dst_stride_y,
                uint16_t* dst_u,
                int dst_stride_u,
                uint16_t* dst_v,
                int dst_stride_v,
                int width,
                int height);

        // Convert 8 bit YUV to 12 bit.
#define H420ToH012 I420ToI012
        LIBYUV_API
            int I420ToI012(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint16_t* dst_y,
                int dst_stride_y,
                uint16_t* dst_u,
                int dst_stride_u,
                uint16_t* dst_v,
                int dst_stride_v,
                int width,
                int height);

        LIBYUV_API
            int I420ToI422(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_u,
                int dst_stride_u,
                uint8_t* dst_v,
                int dst_stride_v,
                int width,
                int height);

        LIBYUV_API
            int I420ToI444(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_u,
                int dst_stride_u,
                uint8_t* dst_v,
                int dst_stride_v,
                int width,
                int height);

        // Copy to I400. Source can be I420, I422, I444, I400, NV12 or NV21.
        LIBYUV_API
            int I400Copy(const uint8_t* src_y,
                int src_stride_y,
                uint8_t* dst_y,
                int dst_stride_y,
                int width,
                int height);

        LIBYUV_API
            int I420ToNV12(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_uv,
                int dst_stride_uv,
                int width,
                int height);

        LIBYUV_API
            int I420ToNV21(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_vu,
                int dst_stride_vu,
                int width,
                int height);

        LIBYUV_API
            int I420ToYUY2(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_yuy2,
                int dst_stride_yuy2,
                int width,
                int height);

        LIBYUV_API
            int I420ToUYVY(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_uyvy,
                int dst_stride_uyvy,
                int width,
                int height);

        // The following are from convert_argb.h
        // DEPRECATED: The prototypes will be removed in future.  Use convert_argb.h

        // Convert I420 to ARGB.
        LIBYUV_API
            int I420ToARGB(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int width,
                int height);

        // Convert I420 to ABGR.
        LIBYUV_API
            int I420ToABGR(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_abgr,
                int dst_stride_abgr,
                int width,
                int height);

        // Convert I420 to specified format.
        // "dst_sample_stride" is bytes in a row for the destination. Pass 0 if the
        //    buffer has contiguous rows. Can be negative. A multiple of 16 is optimal.
        LIBYUV_API
            int ConvertFromI420(const uint8_t* y,
                int y_stride,
                const uint8_t* u,
                int u_stride,
                const uint8_t* v,
                int v_stride,
                uint8_t* dst_sample,
                int dst_sample_stride,
                int width,
                int height,
                uint32_t fourcc);

#ifdef __cplusplus
    }  // extern "C"
}  // namespace libyuv
#endif




#ifdef __cplusplus
namespace libyuv {
    extern "C" {
#endif

        // Copy a plane of data.
        LIBYUV_API
            void CopyPlane(const uint8_t* src_y,
                int src_stride_y,
                uint8_t* dst_y,
                int dst_stride_y,
                int width,
                int height);

        LIBYUV_API
            void CopyPlane_16(const uint16_t* src_y,
                int src_stride_y,
                uint16_t* dst_y,
                int dst_stride_y,
                int width,
                int height);

        LIBYUV_API
            void Convert16To8Plane(const uint16_t* src_y,
                int src_stride_y,
                uint8_t* dst_y,
                int dst_stride_y,
                int scale,  // 16384 for 10 bits
                int width,
                int height);

        LIBYUV_API
            void Convert8To16Plane(const uint8_t* src_y,
                int src_stride_y,
                uint16_t* dst_y,
                int dst_stride_y,
                int scale,  // 1024 for 10 bits
                int width,
                int height);

        LIBYUV_API
            void Convert8To8Plane(const uint8_t* src_y,
                int src_stride_y,
                uint8_t* dst_y,
                int dst_stride_y,
                int scale,  // 220 for Y, 225 for U,V
                int bias,   // 16
                int width,
                int height);

        // Set a plane of data to a 32 bit value.
        LIBYUV_API
            void SetPlane(uint8_t* dst_y,
                int dst_stride_y,
                int width,
                int height,
                uint32_t value);

        // Convert a plane of tiles of 16 x H to linear.
        LIBYUV_API
            int DetilePlane(const uint8_t* src_y,
                int src_stride_y,
                uint8_t* dst_y,
                int dst_stride_y,
                int width,
                int height,
                int tile_height);

        // Convert a plane of 16 bit tiles of 16 x H to linear.
        LIBYUV_API
            int DetilePlane_16(const uint16_t* src_y,
                int src_stride_y,
                uint16_t* dst_y,
                int dst_stride_y,
                int width,
                int height,
                int tile_height);

        // Convert a UV plane of tiles of 16 x H into linear U and V planes.
        LIBYUV_API
            void DetileSplitUVPlane(const uint8_t* src_uv,
                int src_stride_uv,
                uint8_t* dst_u,
                int dst_stride_u,
                uint8_t* dst_v,
                int dst_stride_v,
                int width,
                int height,
                int tile_height);

        // Convert a Y and UV plane of tiles into interlaced YUY2.
        LIBYUV_API
            void DetileToYUY2(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_uv,
                int src_stride_uv,
                uint8_t* dst_yuy2,
                int dst_stride_yuy2,
                int width,
                int height,
                int tile_height);

        // Split interleaved UV plane into separate U and V planes.
        LIBYUV_API
            void SplitUVPlane(const uint8_t* src_uv,
                int src_stride_uv,
                uint8_t* dst_u,
                int dst_stride_u,
                uint8_t* dst_v,
                int dst_stride_v,
                int width,
                int height);

        // Merge separate U and V planes into one interleaved UV plane.
        LIBYUV_API
            void MergeUVPlane(const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_uv,
                int dst_stride_uv,
                int width,
                int height);

        // Split interleaved msb UV plane into separate lsb U and V planes.
        LIBYUV_API
            void SplitUVPlane_16(const uint16_t* src_uv,
                int src_stride_uv,
                uint16_t* dst_u,
                int dst_stride_u,
                uint16_t* dst_v,
                int dst_stride_v,
                int width,
                int height,
                int depth);

        // Merge separate lsb U and V planes into one interleaved msb UV plane.
        LIBYUV_API
            void MergeUVPlane_16(const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                uint16_t* dst_uv,
                int dst_stride_uv,
                int width,
                int height,
                int depth);

        // Convert lsb plane to msb plane
        LIBYUV_API
            void ConvertToMSBPlane_16(const uint16_t* src_y,
                int src_stride_y,
                uint16_t* dst_y,
                int dst_stride_y,
                int width,
                int height,
                int depth);

        // Convert msb plane to lsb plane
        LIBYUV_API
            void ConvertToLSBPlane_16(const uint16_t* src_y,
                int src_stride_y,
                uint16_t* dst_y,
                int dst_stride_y,
                int width,
                int height,
                int depth);

        // Scale U and V to half width and height and merge into interleaved UV plane.
        // width and height are source size, allowing odd sizes.
        // Use for converting I444 or I422 to NV12.
        LIBYUV_API
            void HalfMergeUVPlane(const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_uv,
                int dst_stride_uv,
                int width,
                int height);

        // Swap U and V channels in interleaved UV plane.
        LIBYUV_API
            void SwapUVPlane(const uint8_t* src_uv,
                int src_stride_uv,
                uint8_t* dst_vu,
                int dst_stride_vu,
                int width,
                int height);

        // Split interleaved RGB plane into separate R, G and B planes.
        LIBYUV_API
            void SplitRGBPlane(const uint8_t* src_rgb,
                int src_stride_rgb,
                uint8_t* dst_r,
                int dst_stride_r,
                uint8_t* dst_g,
                int dst_stride_g,
                uint8_t* dst_b,
                int dst_stride_b,
                int width,
                int height);

        // Merge separate R, G and B planes into one interleaved RGB plane.
        LIBYUV_API
            void MergeRGBPlane(const uint8_t* src_r,
                int src_stride_r,
                const uint8_t* src_g,
                int src_stride_g,
                const uint8_t* src_b,
                int src_stride_b,
                uint8_t* dst_rgb,
                int dst_stride_rgb,
                int width,
                int height);

        // Split interleaved ARGB plane into separate R, G, B and A planes.
        // dst_a can be NULL to discard alpha plane.
        LIBYUV_API
            void SplitARGBPlane(const uint8_t* src_argb,
                int src_stride_argb,
                uint8_t* dst_r,
                int dst_stride_r,
                uint8_t* dst_g,
                int dst_stride_g,
                uint8_t* dst_b,
                int dst_stride_b,
                uint8_t* dst_a,
                int dst_stride_a,
                int width,
                int height);

        // Merge separate R, G, B and A planes into one interleaved ARGB plane.
        // src_a can be NULL to fill opaque value to alpha.
        LIBYUV_API
            void MergeARGBPlane(const uint8_t* src_r,
                int src_stride_r,
                const uint8_t* src_g,
                int src_stride_g,
                const uint8_t* src_b,
                int src_stride_b,
                const uint8_t* src_a,
                int src_stride_a,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int width,
                int height);

        // Merge separate 'depth' bit R, G and B planes stored in lsb
        // into one interleaved XR30 plane.
        // depth should in range [10, 16]
        LIBYUV_API
            void MergeXR30Plane(const uint16_t* src_r,
                int src_stride_r,
                const uint16_t* src_g,
                int src_stride_g,
                const uint16_t* src_b,
                int src_stride_b,
                uint8_t* dst_ar30,
                int dst_stride_ar30,
                int width,
                int height,
                int depth);

        // Merge separate 'depth' bit R, G, B and A planes stored in lsb
        // into one interleaved AR64 plane.
        // src_a can be NULL to fill opaque value to alpha.
        // depth should in range [1, 16]
        LIBYUV_API
            void MergeAR64Plane(const uint16_t* src_r,
                int src_stride_r,
                const uint16_t* src_g,
                int src_stride_g,
                const uint16_t* src_b,
                int src_stride_b,
                const uint16_t* src_a,
                int src_stride_a,
                uint16_t* dst_ar64,
                int dst_stride_ar64,
                int width,
                int height,
                int depth);

        // Merge separate 'depth' bit R, G, B and A planes stored in lsb
        // into one interleaved ARGB plane.
        // src_a can be NULL to fill opaque value to alpha.
        // depth should in range [8, 16]
        LIBYUV_API
            void MergeARGB16To8Plane(const uint16_t* src_r,
                int src_stride_r,
                const uint16_t* src_g,
                int src_stride_g,
                const uint16_t* src_b,
                int src_stride_b,
                const uint16_t* src_a,
                int src_stride_a,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int width,
                int height,
                int depth);

        // Copy I400.  Supports inverting.
        LIBYUV_API
            int I400ToI400(const uint8_t* src_y,
                int src_stride_y,
                uint8_t* dst_y,
                int dst_stride_y,
                int width,
                int height);

#define J400ToJ400 I400ToI400

        // Copy I422 to I422.
#define I422ToI422 I422Copy
        LIBYUV_API
            int I422Copy(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_u,
                int dst_stride_u,
                uint8_t* dst_v,
                int dst_stride_v,
                int width,
                int height);

        // Copy I444 to I444.
#define I444ToI444 I444Copy
        LIBYUV_API
            int I444Copy(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_u,
                int dst_stride_u,
                uint8_t* dst_v,
                int dst_stride_v,
                int width,
                int height);

        // Copy I210 to I210.
#define I210ToI210 I210Copy
        LIBYUV_API
            int I210Copy(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                uint16_t* dst_y,
                int dst_stride_y,
                uint16_t* dst_u,
                int dst_stride_u,
                uint16_t* dst_v,
                int dst_stride_v,
                int width,
                int height);

        // Copy I410 to I410.
#define I410ToI410 I410Copy
        LIBYUV_API
            int I410Copy(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                uint16_t* dst_y,
                int dst_stride_y,
                uint16_t* dst_u,
                int dst_stride_u,
                uint16_t* dst_v,
                int dst_stride_v,
                int width,
                int height);

        // Copy NV12. Supports inverting.
        LIBYUV_API
            int NV12Copy(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_uv,
                int src_stride_uv,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_uv,
                int dst_stride_uv,
                int width,
                int height);

        // Copy NV21. Supports inverting.
        LIBYUV_API
            int NV21Copy(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_vu,
                int src_stride_vu,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_vu,
                int dst_stride_vu,
                int width,
                int height);

        // Convert YUY2 to I422.
        LIBYUV_API
            int YUY2ToI422(const uint8_t* src_yuy2,
                int src_stride_yuy2,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_u,
                int dst_stride_u,
                uint8_t* dst_v,
                int dst_stride_v,
                int width,
                int height);

        // Convert UYVY to I422.
        LIBYUV_API
            int UYVYToI422(const uint8_t* src_uyvy,
                int src_stride_uyvy,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_u,
                int dst_stride_u,
                uint8_t* dst_v,
                int dst_stride_v,
                int width,
                int height);

        LIBYUV_API
            int YUY2ToNV12(const uint8_t* src_yuy2,
                int src_stride_yuy2,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_uv,
                int dst_stride_uv,
                int width,
                int height);

        LIBYUV_API
            int UYVYToNV12(const uint8_t* src_uyvy,
                int src_stride_uyvy,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_uv,
                int dst_stride_uv,
                int width,
                int height);

        // Convert NV21 to NV12.
        LIBYUV_API
            int NV21ToNV12(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_vu,
                int src_stride_vu,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_uv,
                int dst_stride_uv,
                int width,
                int height);

        LIBYUV_API
            int YUY2ToY(const uint8_t* src_yuy2,
                int src_stride_yuy2,
                uint8_t* dst_y,
                int dst_stride_y,
                int width,
                int height);

        LIBYUV_API
            int UYVYToY(const uint8_t* src_uyvy,
                int src_stride_uyvy,
                uint8_t* dst_y,
                int dst_stride_y,
                int width,
                int height);

        // Convert I420 to I400. (calls CopyPlane ignoring u/v).
        LIBYUV_API
            int I420ToI400(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_y,
                int dst_stride_y,
                int width,
                int height);

        // Alias
#define J420ToJ400 I420ToI400
#define I420ToI420Mirror I420Mirror

// I420 mirror.
        LIBYUV_API
            int I420Mirror(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_u,
                int dst_stride_u,
                uint8_t* dst_v,
                int dst_stride_v,
                int width,
                int height);

        // Alias
#define I400ToI400Mirror I400Mirror

// I400 mirror.  A single plane is mirrored horizontally.
// Pass negative height to achieve 180 degree rotation.
        LIBYUV_API
            int I400Mirror(const uint8_t* src_y,
                int src_stride_y,
                uint8_t* dst_y,
                int dst_stride_y,
                int width,
                int height);

        // Alias
#define NV12ToNV12Mirror NV12Mirror

// NV12 mirror.
        LIBYUV_API
            int NV12Mirror(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_uv,
                int src_stride_uv,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_uv,
                int dst_stride_uv,
                int width,
                int height);

        // Alias
#define ARGBToARGBMirror ARGBMirror

// ARGB mirror.
        LIBYUV_API
            int ARGBMirror(const uint8_t* src_argb,
                int src_stride_argb,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int width,
                int height);

        // Alias
#define RGB24ToRGB24Mirror RGB24Mirror

// RGB24 mirror.
        LIBYUV_API
            int RGB24Mirror(const uint8_t* src_rgb24,
                int src_stride_rgb24,
                uint8_t* dst_rgb24,
                int dst_stride_rgb24,
                int width,
                int height);

        // Mirror a plane of data.
        LIBYUV_API
            void MirrorPlane(const uint8_t* src_y,
                int src_stride_y,
                uint8_t* dst_y,
                int dst_stride_y,
                int width,
                int height);

        // Mirror a plane of UV data.
        LIBYUV_API
            void MirrorUVPlane(const uint8_t* src_uv,
                int src_stride_uv,
                uint8_t* dst_uv,
                int dst_stride_uv,
                int width,
                int height);

        // Alias
#define RGB24ToRAW RAWToRGB24

        LIBYUV_API
            int RAWToRGB24(const uint8_t* src_raw,
                int src_stride_raw,
                uint8_t* dst_rgb24,
                int dst_stride_rgb24,
                int width,
                int height);

        // Draw a rectangle into I420.
        LIBYUV_API
            int I420Rect(uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_u,
                int dst_stride_u,
                uint8_t* dst_v,
                int dst_stride_v,
                int x,
                int y,
                int width,
                int height,
                int value_y,
                int value_u,
                int value_v);

        // Draw a rectangle into ARGB.
        LIBYUV_API
            int ARGBRect(uint8_t* dst_argb,
                int dst_stride_argb,
                int dst_x,
                int dst_y,
                int width,
                int height,
                uint32_t value);

        // Convert ARGB to gray scale ARGB.
        LIBYUV_API
            int ARGBGrayTo(const uint8_t* src_argb,
                int src_stride_argb,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int width,
                int height);

        // Make a rectangle of ARGB gray scale.
        LIBYUV_API
            int ARGBGray(uint8_t* dst_argb,
                int dst_stride_argb,
                int dst_x,
                int dst_y,
                int width,
                int height);

        // Make a rectangle of ARGB Sepia tone.
        LIBYUV_API
            int ARGBSepia(uint8_t* dst_argb,
                int dst_stride_argb,
                int dst_x,
                int dst_y,
                int width,
                int height);

        // Apply a matrix rotation to each ARGB pixel.
        // matrix_argb is 4 signed ARGB values. -128 to 127 representing -2 to 2.
        // The first 4 coefficients apply to B, G, R, A and produce B of the output.
        // The next 4 coefficients apply to B, G, R, A and produce G of the output.
        // The next 4 coefficients apply to B, G, R, A and produce R of the output.
        // The last 4 coefficients apply to B, G, R, A and produce A of the output.
        LIBYUV_API
            int ARGBColorMatrix(const uint8_t* src_argb,
                int src_stride_argb,
                uint8_t* dst_argb,
                int dst_stride_argb,
                const int8_t* matrix_argb,
                int width,
                int height);

        // Deprecated. Use ARGBColorMatrix instead.
        // Apply a matrix rotation to each ARGB pixel.
        // matrix_argb is 3 signed ARGB values. -128 to 127 representing -1 to 1.
        // The first 4 coefficients apply to B, G, R, A and produce B of the output.
        // The next 4 coefficients apply to B, G, R, A and produce G of the output.
        // The last 4 coefficients apply to B, G, R, A and produce R of the output.
        LIBYUV_API
            int RGBColorMatrix(uint8_t* dst_argb,
                int dst_stride_argb,
                const int8_t* matrix_rgb,
                int dst_x,
                int dst_y,
                int width,
                int height);

        // Apply a color table each ARGB pixel.
        // Table contains 256 ARGB values.
        LIBYUV_API
            int ARGBColorTable(uint8_t* dst_argb,
                int dst_stride_argb,
                const uint8_t* table_argb,
                int dst_x,
                int dst_y,
                int width,
                int height);

        // Apply a color table each ARGB pixel but preserve destination alpha.
        // Table contains 256 ARGB values.
        LIBYUV_API
            int RGBColorTable(uint8_t* dst_argb,
                int dst_stride_argb,
                const uint8_t* table_argb,
                int dst_x,
                int dst_y,
                int width,
                int height);

        // Apply a luma/color table each ARGB pixel but preserve destination alpha.
        // Table contains 32768 values indexed by [Y][C] where 7 it 7 bit luma from
        // RGB (YJ style) and C is an 8 bit color component (R, G or B).
        LIBYUV_API
            int ARGBLumaColorTable(const uint8_t* src_argb,
                int src_stride_argb,
                uint8_t* dst_argb,
                int dst_stride_argb,
                const uint8_t* luma,
                int width,
                int height);

        // Apply a 3 term polynomial to ARGB values.
        // poly points to a 4x4 matrix.  The first row is constants.  The 2nd row is
        // coefficients for b, g, r and a.  The 3rd row is coefficients for b squared,
        // g squared, r squared and a squared.  The 4rd row is coefficients for b to
        // the 3, g to the 3, r to the 3 and a to the 3.  The values are summed and
        // result clamped to 0 to 255.
        // A polynomial approximation can be dirived using software such as 'R'.

        LIBYUV_API
            int ARGBPolynomial(const uint8_t* src_argb,
                int src_stride_argb,
                uint8_t* dst_argb,
                int dst_stride_argb,
                const float* poly,
                int width,
                int height);

        // Convert plane of 16 bit shorts to half floats.
        // Source values are multiplied by scale before storing as half float.
        //
        // Note: Unlike other libyuv functions that operate on uint16_t buffers, the
        // src_stride_y and dst_stride_y parameters of HalfFloatPlane() are in bytes,
        // not in units of uint16_t.
        LIBYUV_API
            int HalfFloatPlane(const uint16_t* src_y,
                int src_stride_y,
                uint16_t* dst_y,
                int dst_stride_y,
                float scale,
                int width,
                int height);

        // Convert a buffer of bytes to floats, scale the values and store as floats.
        LIBYUV_API
            int ByteToFloat(const uint8_t* src_y, float* dst_y, float scale, int width);

        // Quantize a rectangle of ARGB. Alpha unaffected.
        // scale is a 16 bit fractional fixed point scaler between 0 and 65535.
        // interval_size should be a value between 1 and 255.
        // interval_offset should be a value between 0 and 255.
        LIBYUV_API
            int ARGBQuantize(uint8_t* dst_argb,
                int dst_stride_argb,
                int scale,
                int interval_size,
                int interval_offset,
                int dst_x,
                int dst_y,
                int width,
                int height);

        // Copy ARGB to ARGB.
        LIBYUV_API
            int ARGBCopy(const uint8_t* src_argb,
                int src_stride_argb,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int width,
                int height);

        // Copy Alpha channel of ARGB to alpha of ARGB.
        LIBYUV_API
            int ARGBCopyAlpha(const uint8_t* src_argb,
                int src_stride_argb,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int width,
                int height);

        // Extract the alpha channel from ARGB.
        LIBYUV_API
            int ARGBExtractAlpha(const uint8_t* src_argb,
                int src_stride_argb,
                uint8_t* dst_a,
                int dst_stride_a,
                int width,
                int height);

        // Copy Y channel to Alpha of ARGB.
        LIBYUV_API
            int ARGBCopyYToAlpha(const uint8_t* src_y,
                int src_stride_y,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int width,
                int height);

        // Alpha Blend ARGB images and store to destination.
        // Source is pre-multiplied by alpha using ARGBAttenuate.
        // Alpha of destination is set to 255.
        LIBYUV_API
            int ARGBBlend(const uint8_t* src_argb0,
                int src_stride_argb0,
                const uint8_t* src_argb1,
                int src_stride_argb1,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int width,
                int height);

        // Alpha Blend plane and store to destination.
        // Source is not pre-multiplied by alpha.
        LIBYUV_API
            int BlendPlane(const uint8_t* src_y0,
                int src_stride_y0,
                const uint8_t* src_y1,
                int src_stride_y1,
                const uint8_t* alpha,
                int alpha_stride,
                uint8_t* dst_y,
                int dst_stride_y,
                int width,
                int height);

        // Alpha Blend YUV images and store to destination.
        // Source is not pre-multiplied by alpha.
        // Alpha is full width x height and subsampled to half size to apply to UV.
        LIBYUV_API
            int I420Blend(const uint8_t* src_y0,
                int src_stride_y0,
                const uint8_t* src_u0,
                int src_stride_u0,
                const uint8_t* src_v0,
                int src_stride_v0,
                const uint8_t* src_y1,
                int src_stride_y1,
                const uint8_t* src_u1,
                int src_stride_u1,
                const uint8_t* src_v1,
                int src_stride_v1,
                const uint8_t* alpha,
                int alpha_stride,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_u,
                int dst_stride_u,
                uint8_t* dst_v,
                int dst_stride_v,
                int width,
                int height);

        // Multiply ARGB image by ARGB image. Shifted down by 8. Saturates to 255.
        LIBYUV_API
            int ARGBMultiply(const uint8_t* src_argb0,
                int src_stride_argb0,
                const uint8_t* src_argb1,
                int src_stride_argb1,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int width,
                int height);

        // Add ARGB image with ARGB image. Saturates to 255.
        LIBYUV_API
            int ARGBAdd(const uint8_t* src_argb0,
                int src_stride_argb0,
                const uint8_t* src_argb1,
                int src_stride_argb1,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int width,
                int height);

        // Subtract ARGB image (argb1) from ARGB image (argb0). Saturates to 0.
        LIBYUV_API
            int ARGBSubtract(const uint8_t* src_argb0,
                int src_stride_argb0,
                const uint8_t* src_argb1,
                int src_stride_argb1,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int width,
                int height);

        // Convert I422 to YUY2.
        LIBYUV_API
            int I422ToYUY2(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_yuy2,
                int dst_stride_yuy2,
                int width,
                int height);

        // Convert I422 to UYVY.
        LIBYUV_API
            int I422ToUYVY(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_uyvy,
                int dst_stride_uyvy,
                int width,
                int height);

        // Convert unattentuated ARGB to preattenuated ARGB.
        LIBYUV_API
            int ARGBAttenuate(const uint8_t* src_argb,
                int src_stride_argb,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int width,
                int height);

        // Convert preattentuated ARGB to unattenuated ARGB.
        LIBYUV_API
            int ARGBUnattenuate(const uint8_t* src_argb,
                int src_stride_argb,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int width,
                int height);

        // Internal function - do not call directly.
        // Computes table of cumulative sum for image where the value is the sum
        // of all values above and to the left of the entry. Used by ARGBBlur.
        LIBYUV_API
            int ARGBComputeCumulativeSum(const uint8_t* src_argb,
                int src_stride_argb,
                int32_t* dst_cumsum,
                int dst_stride32_cumsum,
                int width,
                int height);

        // Blur ARGB image.
        // dst_cumsum table of width * (height + 1) * 16 bytes aligned to
        //   16 byte boundary.
        // dst_stride32_cumsum is number of ints in a row (width * 4).
        // radius is number of pixels around the center.  e.g. 1 = 3x3. 2=5x5.
        // Blur is optimized for radius of 5 (11x11) or less.
        LIBYUV_API
            int ARGBBlur(const uint8_t* src_argb,
                int src_stride_argb,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int32_t* dst_cumsum,
                int dst_stride32_cumsum,
                int width,
                int height,
                int radius);

        // Gaussian 5x5 blur a float plane.
        // Coefficients of 1, 4, 6, 4, 1.
        // Each destination pixel is a blur of the 5x5
        // pixels from the source.
        // Source edges are clamped.
        LIBYUV_API
            int GaussPlane_F32(const float* src,
                int src_stride,
                float* dst,
                int dst_stride,
                int width,
                int height);

        // Multiply ARGB image by ARGB value.
        LIBYUV_API
            int ARGBShade(const uint8_t* src_argb,
                int src_stride_argb,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int width,
                int height,
                uint32_t value);

        // Interpolate between two images using specified amount of interpolation
        // (0 to 255) and store to destination.
        // 'interpolation' is specified as 8 bit fraction where 0 means 100% src0
        // and 255 means 1% src0 and 99% src1.
        LIBYUV_API
            int InterpolatePlane(const uint8_t* src0,
                int src_stride0,
                const uint8_t* src1,
                int src_stride1,
                uint8_t* dst,
                int dst_stride,
                int width,
                int height,
                int interpolation);

        // Interpolate between two images using specified amount of interpolation
        // (0 to 255) and store to destination.
        // 'interpolation' is specified as 8 bit fraction where 0 means 100% src0
        // and 255 means 1% src0 and 99% src1.
        LIBYUV_API
            int InterpolatePlane_16(const uint16_t* src0,
                int src_stride0,  // measured in 16 bit pixels
                const uint16_t* src1,
                int src_stride1,
                uint16_t* dst,
                int dst_stride,
                int width,
                int height,
                int interpolation);

        // Interpolate between two ARGB images using specified amount of interpolation
        // Internally calls InterpolatePlane with width * 4 (bpp).
        LIBYUV_API
            int ARGBInterpolate(const uint8_t* src_argb0,
                int src_stride_argb0,
                const uint8_t* src_argb1,
                int src_stride_argb1,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int width,
                int height,
                int interpolation);

        // Interpolate between two YUV images using specified amount of interpolation
        // Internally calls InterpolatePlane on each plane where the U and V planes
        // are half width and half height.
        LIBYUV_API
            int I420Interpolate(const uint8_t* src0_y,
                int src0_stride_y,
                const uint8_t* src0_u,
                int src0_stride_u,
                const uint8_t* src0_v,
                int src0_stride_v,
                const uint8_t* src1_y,
                int src1_stride_y,
                const uint8_t* src1_u,
                int src1_stride_u,
                const uint8_t* src1_v,
                int src1_stride_v,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_u,
                int dst_stride_u,
                uint8_t* dst_v,
                int dst_stride_v,
                int width,
                int height,
                int interpolation);

        // Shuffle ARGB channel order.  e.g. BGRA to ARGB.
        // shuffler is 16 bytes.
        LIBYUV_API
            int ARGBShuffle(const uint8_t* src_bgra,
                int src_stride_bgra,
                uint8_t* dst_argb,
                int dst_stride_argb,
                const uint8_t* shuffler,
                int width,
                int height);

        // Shuffle AR64 channel order.  e.g. AR64 to AB64.
        // shuffler is 16 bytes.
        LIBYUV_API
            int AR64Shuffle(const uint16_t* src_ar64,
                int src_stride_ar64,
                uint16_t* dst_ar64,
                int dst_stride_ar64,
                const uint8_t* shuffler,
                int width,
                int height);

        // Sobel ARGB effect with planar output.
        LIBYUV_API
            int ARGBSobelToPlane(const uint8_t* src_argb,
                int src_stride_argb,
                uint8_t* dst_y,
                int dst_stride_y,
                int width,
                int height);

        // Sobel ARGB effect.
        LIBYUV_API
            int ARGBSobel(const uint8_t* src_argb,
                int src_stride_argb,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int width,
                int height);

        // Sobel ARGB effect w/ Sobel X, Sobel, Sobel Y in ARGB.
        LIBYUV_API
            int ARGBSobelXY(const uint8_t* src_argb,
                int src_stride_argb,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int width,
                int height);

#ifdef __cplusplus
    }  // extern "C"
}  // namespace libyuv
#endif


#ifdef __cplusplus
namespace libyuv {
    extern "C" {
#endif

        // Convert I444 to I420.
        LIBYUV_API
            int I444ToI420(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_u,
                int dst_stride_u,
                uint8_t* dst_v,
                int dst_stride_v,
                int width,
                int height);

        // Convert I444 to NV12.
        LIBYUV_API
            int I444ToNV12(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_uv,
                int dst_stride_uv,
                int width,
                int height);

        // Convert I444 to NV21.
        LIBYUV_API
            int I444ToNV21(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_vu,
                int dst_stride_vu,
                int width,
                int height);

        // Convert I422 to I420.
        LIBYUV_API
            int I422ToI420(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_u,
                int dst_stride_u,
                uint8_t* dst_v,
                int dst_stride_v,
                int width,
                int height);

        // Convert I422 to I444.
        LIBYUV_API
            int I422ToI444(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_u,
                int dst_stride_u,
                uint8_t* dst_v,
                int dst_stride_v,
                int width,
                int height);

        // Convert I422 to I210.
        LIBYUV_API
            int I422ToI210(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint16_t* dst_y,
                int dst_stride_y,
                uint16_t* dst_u,
                int dst_stride_u,
                uint16_t* dst_v,
                int dst_stride_v,
                int width,
                int height);

        // Convert MM21 to NV12.
        LIBYUV_API
            int MM21ToNV12(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_uv,
                int src_stride_uv,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_uv,
                int dst_stride_uv,
                int width,
                int height);

        // Convert MM21 to I420.
        LIBYUV_API
            int MM21ToI420(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_uv,
                int src_stride_uv,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_u,
                int dst_stride_u,
                uint8_t* dst_v,
                int dst_stride_v,
                int width,
                int height);

        // Convert MM21 to YUY2
        LIBYUV_API
            int MM21ToYUY2(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_uv,
                int src_stride_uv,
                uint8_t* dst_yuy2,
                int dst_stride_yuy2,
                int width,
                int height);

        // Convert MT2T to P010
        // Note that src_y and src_uv point to packed 10-bit values, so the Y plane will
        // be 10 / 8 times the dimensions of the image. Also for this reason,
        // src_stride_y and src_stride_uv are given in bytes.
        LIBYUV_API
            int MT2TToP010(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_uv,
                int src_stride_uv,
                uint16_t* dst_y,
                int dst_stride_y,
                uint16_t* dst_uv,
                int dst_stride_uv,
                int width,
                int height);

        // Convert I422 to NV21.
        LIBYUV_API
            int I422ToNV21(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_vu,
                int dst_stride_vu,
                int width,
                int height);

        // Copy I420 to I420.
#define I420ToI420 I420Copy
        LIBYUV_API
            int I420Copy(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_u,
                int dst_stride_u,
                uint8_t* dst_v,
                int dst_stride_v,
                int width,
                int height);

        // Convert I420 to I444.
        LIBYUV_API
            int I420ToI444(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_u,
                int dst_stride_u,
                uint8_t* dst_v,
                int dst_stride_v,
                int width,
                int height);

        // Copy I010 to I010
#define I010ToI010 I010Copy
#define H010ToH010 I010Copy
        LIBYUV_API
            int I010Copy(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                uint16_t* dst_y,
                int dst_stride_y,
                uint16_t* dst_u,
                int dst_stride_u,
                uint16_t* dst_v,
                int dst_stride_v,
                int width,
                int height);

        // Convert 10 bit YUV to 8 bit
#define H010ToH420 I010ToI420
        LIBYUV_API
            int I010ToI420(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_u,
                int dst_stride_u,
                uint8_t* dst_v,
                int dst_stride_v,
                int width,
                int height);

#define H210ToH420 I210ToI420
        LIBYUV_API
            int I210ToI420(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_u,
                int dst_stride_u,
                uint8_t* dst_v,
                int dst_stride_v,
                int width,
                int height);

#define H210ToH422 I210ToI422
        LIBYUV_API
            int I210ToI422(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_u,
                int dst_stride_u,
                uint8_t* dst_v,
                int dst_stride_v,
                int width,
                int height);

#define H410ToH420 I410ToI420
        LIBYUV_API
            int I410ToI420(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_u,
                int dst_stride_u,
                uint8_t* dst_v,
                int dst_stride_v,
                int width,
                int height);

#define H410ToH444 I410ToI444
        LIBYUV_API
            int I410ToI444(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_u,
                int dst_stride_u,
                uint8_t* dst_v,
                int dst_stride_v,
                int width,
                int height);

#define H012ToH420 I012ToI420
        LIBYUV_API
            int I012ToI420(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_u,
                int dst_stride_u,
                uint8_t* dst_v,
                int dst_stride_v,
                int width,
                int height);

#define H212ToH422 I212ToI422
        LIBYUV_API
            int I212ToI422(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_u,
                int dst_stride_u,
                uint8_t* dst_v,
                int dst_stride_v,
                int width,
                int height);

#define H212ToH420 I212ToI420
        LIBYUV_API
            int I212ToI420(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_u,
                int dst_stride_u,
                uint8_t* dst_v,
                int dst_stride_v,
                int width,
                int height);

#define H412ToH444 I412ToI444
        LIBYUV_API
            int I412ToI444(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_u,
                int dst_stride_u,
                uint8_t* dst_v,
                int dst_stride_v,
                int width,
                int height);

#define H412ToH420 I412ToI420
        LIBYUV_API
            int I412ToI420(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_u,
                int dst_stride_u,
                uint8_t* dst_v,
                int dst_stride_v,
                int width,
                int height);

        // Convert 10 bit P010 to 8 bit NV12.
        // dst_y can be NULL
        LIBYUV_API
            int P010ToNV12(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_uv,
                int src_stride_uv,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_uv,
                int dst_stride_uv,
                int width,
                int height);

#define I412ToI012 I410ToI010
#define H410ToH010 I410ToI010
#define H412ToH012 I410ToI010
        LIBYUV_API
            int I410ToI010(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                uint16_t* dst_y,
                int dst_stride_y,
                uint16_t* dst_u,
                int dst_stride_u,
                uint16_t* dst_v,
                int dst_stride_v,
                int width,
                int height);

#define I212ToI012 I210ToI010
#define H210ToH010 I210ToI010
#define H212ToH012 I210ToI010
        LIBYUV_API
            int I210ToI010(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                uint16_t* dst_y,
                int dst_stride_y,
                uint16_t* dst_u,
                int dst_stride_u,
                uint16_t* dst_v,
                int dst_stride_v,
                int width,
                int height);

        // Convert I010 to I410
        LIBYUV_API
            int I010ToI410(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                uint16_t* dst_y,
                int dst_stride_y,
                uint16_t* dst_u,
                int dst_stride_u,
                uint16_t* dst_v,
                int dst_stride_v,
                int width,
                int height);

        // Convert I012 to I412
#define I012ToI412 I010ToI410

// Convert I210 to I410
        LIBYUV_API
            int I210ToI410(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                uint16_t* dst_y,
                int dst_stride_y,
                uint16_t* dst_u,
                int dst_stride_u,
                uint16_t* dst_v,
                int dst_stride_v,
                int width,
                int height);

        // Convert I212 to I412
#define I212ToI412 I210ToI410

// Convert I010 to P010
        LIBYUV_API
            int I010ToP010(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                uint16_t* dst_y,
                int dst_stride_y,
                uint16_t* dst_uv,
                int dst_stride_uv,
                int width,
                int height);

        // Convert 10 bit YUV I010 to NV12
        LIBYUV_API
            int I010ToNV12(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_uv,
                int dst_stride_uv,
                int width,
                int height);

        // Convert I210 to P210
        LIBYUV_API
            int I210ToP210(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                uint16_t* dst_y,
                int dst_stride_y,
                uint16_t* dst_uv,
                int dst_stride_uv,
                int width,
                int height);

        // Convert I012 to P012
        LIBYUV_API
            int I012ToP012(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                uint16_t* dst_y,
                int dst_stride_y,
                uint16_t* dst_uv,
                int dst_stride_uv,
                int width,
                int height);

        // Convert I212 to P212
        LIBYUV_API
            int I212ToP212(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_u,
                int src_stride_u,
                const uint16_t* src_v,
                int src_stride_v,
                uint16_t* dst_y,
                int dst_stride_y,
                uint16_t* dst_uv,
                int dst_stride_uv,
                int width,
                int height);

        // Convert I400 (grey) to I420.
        LIBYUV_API
            int I400ToI420(const uint8_t* src_y,
                int src_stride_y,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_u,
                int dst_stride_u,
                uint8_t* dst_v,
                int dst_stride_v,
                int width,
                int height);

        // Convert J420 to I420.
        LIBYUV_API
            int J420ToI420(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_u,
                int dst_stride_u,
                uint8_t* dst_v,
                int dst_stride_v,
                int width,
                int height);

        // Convert I400 (grey) to NV21.
        LIBYUV_API
            int I400ToNV21(const uint8_t* src_y,
                int src_stride_y,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_vu,
                int dst_stride_vu,
                int width,
                int height);

#define J400ToJ420 I400ToI420

        // Convert NV12 to I420.
        LIBYUV_API
            int NV12ToI420(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_uv,
                int src_stride_uv,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_u,
                int dst_stride_u,
                uint8_t* dst_v,
                int dst_stride_v,
                int width,
                int height);

        // Convert NV21 to I420.
        LIBYUV_API
            int NV21ToI420(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_vu,
                int src_stride_vu,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_u,
                int dst_stride_u,
                uint8_t* dst_v,
                int dst_stride_v,
                int width,
                int height);

        // Convert NV12 to NV24.
        LIBYUV_API
            int NV12ToNV24(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_uv,
                int src_stride_uv,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_uv,
                int dst_stride_uv,
                int width,
                int height);

        // Convert NV16 to NV24.
        LIBYUV_API
            int NV16ToNV24(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_uv,
                int src_stride_uv,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_uv,
                int dst_stride_uv,
                int width,
                int height);

        // Convert P010 to I010.
        LIBYUV_API
            int P010ToI010(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_uv,
                int src_stride_uv,
                uint16_t* dst_y,
                int dst_stride_y,
                uint16_t* dst_u,
                int dst_stride_u,
                uint16_t* dst_v,
                int dst_stride_v,
                int width,
                int height);

        // Convert P012 to I012.
        LIBYUV_API
            int P012ToI012(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_uv,
                int src_stride_uv,
                uint16_t* dst_y,
                int dst_stride_y,
                uint16_t* dst_u,
                int dst_stride_u,
                uint16_t* dst_v,
                int dst_stride_v,
                int width,
                int height);

        // Convert P010 to P410.
        LIBYUV_API
            int P010ToP410(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_uv,
                int src_stride_uv,
                uint16_t* dst_y,
                int dst_stride_y,
                uint16_t* dst_uv,
                int dst_stride_uv,
                int width,
                int height);

        // Convert P012 to P412.
#define P012ToP412 P010ToP410

// Convert P016 to P416.
#define P016ToP416 P010ToP410

// Convert P210 to P410.
        LIBYUV_API
            int P210ToP410(const uint16_t* src_y,
                int src_stride_y,
                const uint16_t* src_uv,
                int src_stride_uv,
                uint16_t* dst_y,
                int dst_stride_y,
                uint16_t* dst_uv,
                int dst_stride_uv,
                int width,
                int height);

        // Convert P212 to P412.
#define P212ToP412 P210ToP410

// Convert P216 to P416.
#define P216ToP416 P210ToP410

// Convert YUY2 to I420.
        LIBYUV_API
            int YUY2ToI420(const uint8_t* src_yuy2,
                int src_stride_yuy2,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_u,
                int dst_stride_u,
                uint8_t* dst_v,
                int dst_stride_v,
                int width,
                int height);

        // Convert UYVY to I420.
        LIBYUV_API
            int UYVYToI420(const uint8_t* src_uyvy,
                int src_stride_uyvy,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_u,
                int dst_stride_u,
                uint8_t* dst_v,
                int dst_stride_v,
                int width,
                int height);

        // Convert AYUV to NV12.
        LIBYUV_API
            int AYUVToNV12(const uint8_t* src_ayuv,
                int src_stride_ayuv,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_uv,
                int dst_stride_uv,
                int width,
                int height);

        // Convert AYUV to NV21.
        LIBYUV_API
            int AYUVToNV21(const uint8_t* src_ayuv,
                int src_stride_ayuv,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_vu,
                int dst_stride_vu,
                int width,
                int height);

        // Convert Android420 to I420.
        LIBYUV_API
            int Android420ToI420(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                int src_pixel_stride_uv,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_u,
                int dst_stride_u,
                uint8_t* dst_v,
                int dst_stride_v,
                int width,
                int height);

        // ARGB little endian (bgra in memory) to I420.
        LIBYUV_API
            int ARGBToI420(const uint8_t* src_argb,
                int src_stride_argb,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_u,
                int dst_stride_u,
                uint8_t* dst_v,
                int dst_stride_v,
                int width,
                int height);

        // Convert ARGB to I420 with Alpha
        LIBYUV_API
            int ARGBToI420Alpha(const uint8_t* src_argb,
                int src_stride_argb,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_u,
                int dst_stride_u,
                uint8_t* dst_v,
                int dst_stride_v,
                uint8_t* dst_a,
                int dst_stride_a,
                int width,
                int height);

        // BGRA little endian (argb in memory) to I420.
        LIBYUV_API
            int BGRAToI420(const uint8_t* src_bgra,
                int src_stride_bgra,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_u,
                int dst_stride_u,
                uint8_t* dst_v,
                int dst_stride_v,
                int width,
                int height);

        // ABGR little endian (rgba in memory) to I420.
        LIBYUV_API
            int ABGRToI420(const uint8_t* src_abgr,
                int src_stride_abgr,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_u,
                int dst_stride_u,
                uint8_t* dst_v,
                int dst_stride_v,
                int width,
                int height);

        // RGBA little endian (abgr in memory) to I420.
        LIBYUV_API
            int RGBAToI420(const uint8_t* src_rgba,
                int src_stride_rgba,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_u,
                int dst_stride_u,
                uint8_t* dst_v,
                int dst_stride_v,
                int width,
                int height);

        // RGB little endian (bgr in memory) to I420.
        LIBYUV_API
            int RGB24ToI420(const uint8_t* src_rgb24,
                int src_stride_rgb24,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_u,
                int dst_stride_u,
                uint8_t* dst_v,
                int dst_stride_v,
                int width,
                int height);

        // RGB little endian (bgr in memory) to J420.
        LIBYUV_API
            int RGB24ToJ420(const uint8_t* src_rgb24,
                int src_stride_rgb24,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_u,
                int dst_stride_u,
                uint8_t* dst_v,
                int dst_stride_v,
                int width,
                int height);

        // RGB big endian (rgb in memory) to I420.
        LIBYUV_API
            int RAWToI420(const uint8_t* src_raw,
                int src_stride_raw,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_u,
                int dst_stride_u,
                uint8_t* dst_v,
                int dst_stride_v,
                int width,
                int height);

        // RGB big endian (rgb in memory) to I444.
        LIBYUV_API
            int RAWToI444(const uint8_t* src_raw,
                int src_stride_raw,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_u,
                int dst_stride_u,
                uint8_t* dst_v,
                int dst_stride_v,
                int width,
                int height);

        // RGB big endian (rgb in memory) to J420.
        LIBYUV_API
            int RAWToJ420(const uint8_t* src_raw,
                int src_stride_raw,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_u,
                int dst_stride_u,
                uint8_t* dst_v,
                int dst_stride_v,
                int width,
                int height);

        // RGB big endian (rgb in memory) to J444.
        LIBYUV_API
            int RAWToJ444(const uint8_t* src_raw,
                int src_stride_raw,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_u,
                int dst_stride_u,
                uint8_t* dst_v,
                int dst_stride_v,
                int width,
                int height);

        // RGB16 (RGBP fourcc) little endian to I420.
        LIBYUV_API
            int RGB565ToI420(const uint8_t* src_rgb565,
                int src_stride_rgb565,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_u,
                int dst_stride_u,
                uint8_t* dst_v,
                int dst_stride_v,
                int width,
                int height);

        // RGB15 (RGBO fourcc) little endian to I420.
        LIBYUV_API
            int ARGB1555ToI420(const uint8_t* src_argb1555,
                int src_stride_argb1555,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_u,
                int dst_stride_u,
                uint8_t* dst_v,
                int dst_stride_v,
                int width,
                int height);

        // RGB12 (R444 fourcc) little endian to I420.
        LIBYUV_API
            int ARGB4444ToI420(const uint8_t* src_argb4444,
                int src_stride_argb4444,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_u,
                int dst_stride_u,
                uint8_t* dst_v,
                int dst_stride_v,
                int width,
                int height);

        // RGB little endian (bgr in memory) to J400.
        LIBYUV_API
            int RGB24ToJ400(const uint8_t* src_rgb24,
                int src_stride_rgb24,
                uint8_t* dst_yj,
                int dst_stride_yj,
                int width,
                int height);

        // RGB big endian (rgb in memory) to J400.
        LIBYUV_API
            int RAWToJ400(const uint8_t* src_raw,
                int src_stride_raw,
                uint8_t* dst_yj,
                int dst_stride_yj,
                int width,
                int height);

        // src_width/height provided by capture.
        // dst_width/height for clipping determine final size.
        LIBYUV_API
            int MJPGToI420(const uint8_t* sample,
                size_t sample_size,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_u,
                int dst_stride_u,
                uint8_t* dst_v,
                int dst_stride_v,
                int src_width,
                int src_height,
                int dst_width,
                int dst_height);

        // JPEG to NV21
        LIBYUV_API
            int MJPGToNV21(const uint8_t* sample,
                size_t sample_size,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_vu,
                int dst_stride_vu,
                int src_width,
                int src_height,
                int dst_width,
                int dst_height);

        // JPEG to NV12
        LIBYUV_API
            int MJPGToNV12(const uint8_t* sample,
                size_t sample_size,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_uv,
                int dst_stride_uv,
                int src_width,
                int src_height,
                int dst_width,
                int dst_height);

        // Query size of MJPG in pixels.
        LIBYUV_API
            int MJPGSize(const uint8_t* sample,
                size_t sample_size,
                int* width,
                int* height);

        // Convert camera sample to I420 with cropping, rotation and vertical flip.
        // "src_size" is needed to parse MJPG.
        // "dst_stride_y" number of bytes in a row of the dst_y plane.
        //   Normally this would be the same as dst_width, with recommended alignment
        //   to 16 bytes for better efficiency.
        //   If rotation of 90 or 270 is used, stride is affected. The caller should
        //   allocate the I420 buffer according to rotation.
        // "dst_stride_u" number of bytes in a row of the dst_u plane.
        //   Normally this would be the same as (dst_width + 1) / 2, with
        //   recommended alignment to 16 bytes for better efficiency.
        //   If rotation of 90 or 270 is used, stride is affected.
        // "crop_x" and "crop_y" are starting position for cropping.
        //   To center, crop_x = (src_width - dst_width) / 2
        //              crop_y = (src_height - dst_height) / 2
        // "src_width" / "src_height" is size of src_frame in pixels.
        //   "src_height" can be negative indicating a vertically flipped image source.
        // "crop_width" / "crop_height" is the size to crop the src to.
        //    Must be less than or equal to src_width/src_height
        //    Cropping parameters are pre-rotation.
        // "rotation" can be 0, 90, 180 or 270.
        // "fourcc" is a fourcc. ie 'I420', 'YUY2'
        // Returns 0 for successful; -1 for invalid parameter. Non-zero for failure.
        LIBYUV_API
            int ConvertToI420(const uint8_t* sample,
                size_t sample_size,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_u,
                int dst_stride_u,
                uint8_t* dst_v,
                int dst_stride_v,
                int crop_x,
                int crop_y,
                int src_width,
                int src_height,
                int crop_width,
                int crop_height,
                enum RotationMode rotation,
                uint32_t fourcc);

#ifdef __cplusplus
    }  // extern "C"
}  // namespace libyuv
#endif



#ifdef __cplusplus
namespace libyuv {
    extern "C" {
#endif

        // Copy ARGB to ARGB.
#define ARGBToARGB ARGBCopy
        LIBYUV_API
            int ARGBCopy(const uint8_t* src_argb,
                int src_stride_argb,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int width,
                int height);

        // Convert ARGB To BGRA.
        LIBYUV_API
            int ARGBToBGRA(const uint8_t* src_argb,
                int src_stride_argb,
                uint8_t* dst_bgra,
                int dst_stride_bgra,
                int width,
                int height);

        // Convert ARGB To ABGR.
        LIBYUV_API
            int ARGBToABGR(const uint8_t* src_argb,
                int src_stride_argb,
                uint8_t* dst_abgr,
                int dst_stride_abgr,
                int width,
                int height);

        // Convert ARGB To RGBA.
        LIBYUV_API
            int ARGBToRGBA(const uint8_t* src_argb,
                int src_stride_argb,
                uint8_t* dst_rgba,
                int dst_stride_rgba,
                int width,
                int height);

        // Aliases
#define ARGBToAB30 ABGRToAR30
#define ABGRToAB30 ARGBToAR30

// Convert ABGR To AR30.
        LIBYUV_API
            int ABGRToAR30(const uint8_t* src_abgr,
                int src_stride_abgr,
                uint8_t* dst_ar30,
                int dst_stride_ar30,
                int width,
                int height);

        // Convert ARGB To AR30.
        LIBYUV_API
            int ARGBToAR30(const uint8_t* src_argb,
                int src_stride_argb,
                uint8_t* dst_ar30,
                int dst_stride_ar30,
                int width,
                int height);

        // Aliases
#define ABGRToRGB24 ARGBToRAW
#define ABGRToRAW ARGBToRGB24

// Convert ARGB To RGB24.
        LIBYUV_API
            int ARGBToRGB24(const uint8_t* src_argb,
                int src_stride_argb,
                uint8_t* dst_rgb24,
                int dst_stride_rgb24,
                int width,
                int height);

        // Convert ARGB To RAW.
        LIBYUV_API
            int ARGBToRAW(const uint8_t* src_argb,
                int src_stride_argb,
                uint8_t* dst_raw,
                int dst_stride_raw,
                int width,
                int height);

        // Convert ARGB To RGB565.
        LIBYUV_API
            int ARGBToRGB565(const uint8_t* src_argb,
                int src_stride_argb,
                uint8_t* dst_rgb565,
                int dst_stride_rgb565,
                int width,
                int height);

        // Convert ARGB To RGB565 with 4x4 dither matrix (16 bytes).
        // Values in dither matrix from 0 to 7 recommended.
        // The order of the dither matrix is first byte is upper left.
        // TODO(fbarchard): Consider pointer to 2d array for dither4x4.
        // const uint8_t(*dither)[4][4];
        LIBYUV_API
            int ARGBToRGB565Dither(const uint8_t* src_argb,
                int src_stride_argb,
                uint8_t* dst_rgb565,
                int dst_stride_rgb565,
                const uint8_t* dither4x4,
                int width,
                int height);

        // Convert ARGB To ARGB1555.
        LIBYUV_API
            int ARGBToARGB1555(const uint8_t* src_argb,
                int src_stride_argb,
                uint8_t* dst_argb1555,
                int dst_stride_argb1555,
                int width,
                int height);

        // Convert ARGB To ARGB4444.
        LIBYUV_API
            int ARGBToARGB4444(const uint8_t* src_argb,
                int src_stride_argb,
                uint8_t* dst_argb4444,
                int dst_stride_argb4444,
                int width,
                int height);

        // Convert ARGB To I444.
        LIBYUV_API
            int ARGBToI444(const uint8_t* src_argb,
                int src_stride_argb,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_u,
                int dst_stride_u,
                uint8_t* dst_v,
                int dst_stride_v,
                int width,
                int height);

        // Convert ARGB to AR64.
        LIBYUV_API
            int ARGBToAR64(const uint8_t* src_argb,
                int src_stride_argb,
                uint16_t* dst_ar64,
                int dst_stride_ar64,
                int width,
                int height);

        // Convert ABGR to AB64.
#define ABGRToAB64 ARGBToAR64

// Convert ARGB to AB64.
        LIBYUV_API
            int ARGBToAB64(const uint8_t* src_argb,
                int src_stride_argb,
                uint16_t* dst_ab64,
                int dst_stride_ab64,
                int width,
                int height);

        // Convert ABGR to AR64.
#define ABGRToAR64 ARGBToAB64

// Convert ARGB To I422.
        LIBYUV_API
            int ARGBToI422(const uint8_t* src_argb,
                int src_stride_argb,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_u,
                int dst_stride_u,
                uint8_t* dst_v,
                int dst_stride_v,
                int width,
                int height);

        // Convert ARGB To I420. (also in convert.h)
        LIBYUV_API
            int ARGBToI420(const uint8_t* src_argb,
                int src_stride_argb,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_u,
                int dst_stride_u,
                uint8_t* dst_v,
                int dst_stride_v,
                int width,
                int height);

        // Convert ARGB to J420. (JPeg full range I420).
        LIBYUV_API
            int ARGBToJ420(const uint8_t* src_argb,
                int src_stride_argb,
                uint8_t* dst_yj,
                int dst_stride_yj,
                uint8_t* dst_uj,
                int dst_stride_uj,
                uint8_t* dst_vj,
                int dst_stride_vj,
                int width,
                int height);

        // Convert ARGB to J422.
        LIBYUV_API
            int ARGBToJ422(const uint8_t* src_argb,
                int src_stride_argb,
                uint8_t* dst_yj,
                int dst_stride_yj,
                uint8_t* dst_uj,
                int dst_stride_uj,
                uint8_t* dst_vj,
                int dst_stride_vj,
                int width,
                int height);

        // Convert ARGB to J444.
        LIBYUV_API
            int ARGBToJ444(const uint8_t* src_argb,
                int src_stride_argb,
                uint8_t* dst_yj,
                int dst_stride_yj,
                uint8_t* dst_uj,
                int dst_stride_uj,
                uint8_t* dst_vj,
                int dst_stride_vj,
                int width,
                int height);

        // Convert ARGB to J400. (JPeg full range).
        LIBYUV_API
            int ARGBToJ400(const uint8_t* src_argb,
                int src_stride_argb,
                uint8_t* dst_yj,
                int dst_stride_yj,
                int width,
                int height);

        // Convert ABGR to J420. (JPeg full range I420).
        LIBYUV_API
            int ABGRToJ420(const uint8_t* src_abgr,
                int src_stride_abgr,
                uint8_t* dst_yj,
                int dst_stride_yj,
                uint8_t* dst_uj,
                int dst_stride_uj,
                uint8_t* dst_vj,
                int dst_stride_vj,
                int width,
                int height);

        // Convert ABGR to J422.
        LIBYUV_API
            int ABGRToJ422(const uint8_t* src_abgr,
                int src_stride_abgr,
                uint8_t* dst_yj,
                int dst_stride_yj,
                uint8_t* dst_uj,
                int dst_stride_uj,
                uint8_t* dst_vj,
                int dst_stride_vj,
                int width,
                int height);

        // Convert ABGR to J400. (JPeg full range).
        LIBYUV_API
            int ABGRToJ400(const uint8_t* src_abgr,
                int src_stride_abgr,
                uint8_t* dst_yj,
                int dst_stride_yj,
                int width,
                int height);

        // Convert RGBA to J400. (JPeg full range).
        LIBYUV_API
            int RGBAToJ400(const uint8_t* src_rgba,
                int src_stride_rgba,
                uint8_t* dst_yj,
                int dst_stride_yj,
                int width,
                int height);

        // Convert ARGB to I400.
        LIBYUV_API
            int ARGBToI400(const uint8_t* src_argb,
                int src_stride_argb,
                uint8_t* dst_y,
                int dst_stride_y,
                int width,
                int height);

        // Convert ARGB to G. (Reverse of J400toARGB, which replicates G back to ARGB)
        LIBYUV_API
            int ARGBToG(const uint8_t* src_argb,
                int src_stride_argb,
                uint8_t* dst_g,
                int dst_stride_g,
                int width,
                int height);

        // Convert ARGB To NV12.
        LIBYUV_API
            int ARGBToNV12(const uint8_t* src_argb,
                int src_stride_argb,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_uv,
                int dst_stride_uv,
                int width,
                int height);

        // Convert ARGB To NV21.
        LIBYUV_API
            int ARGBToNV21(const uint8_t* src_argb,
                int src_stride_argb,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_vu,
                int dst_stride_vu,
                int width,
                int height);

        // Convert ABGR To NV12.
        LIBYUV_API
            int ABGRToNV12(const uint8_t* src_abgr,
                int src_stride_abgr,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_uv,
                int dst_stride_uv,
                int width,
                int height);

        // Convert ABGR To NV21.
        LIBYUV_API
            int ABGRToNV21(const uint8_t* src_abgr,
                int src_stride_abgr,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_vu,
                int dst_stride_vu,
                int width,
                int height);

        // Convert ARGB To YUY2.
        LIBYUV_API
            int ARGBToYUY2(const uint8_t* src_argb,
                int src_stride_argb,
                uint8_t* dst_yuy2,
                int dst_stride_yuy2,
                int width,
                int height);

        // Convert ARGB To UYVY.
        LIBYUV_API
            int ARGBToUYVY(const uint8_t* src_argb,
                int src_stride_argb,
                uint8_t* dst_uyvy,
                int dst_stride_uyvy,
                int width,
                int height);

        // RAW to JNV21 full range NV21
        LIBYUV_API
            int RAWToJNV21(const uint8_t* src_raw,
                int src_stride_raw,
                uint8_t* dst_y,
                int dst_stride_y,
                uint8_t* dst_vu,
                int dst_stride_vu,
                int width,
                int height);

#ifdef __cplusplus
    }  // extern "C"
}  // namespace libyuv
#endif




#ifdef __cplusplus
namespace libyuv {
    extern "C" {
#endif

        // Internal flag to indicate cpuid requires initialization.
        static const int kCpuInitialized = 0x1;

        // These flags are only valid on Arm processors.
        static const int kCpuHasARM = 0x2;
        static const int kCpuHasNEON = 0x100;
        static const int kCpuHasNeonDotProd = 0x200;
        static const int kCpuHasNeonI8MM = 0x400;
        static const int kCpuHasSVE = 0x800;
        static const int kCpuHasSVE2 = 0x1000;
        static const int kCpuHasSME = 0x2000;
        static const int kCpuHasSME2 = 0x4000;
        static const int kCpuHasSVEF32MM = 0x8000;

        // These flags are only valid on RISCV processors.
        static const int kCpuHasRISCV = 0x4;
        static const int kCpuHasRVV = 0x100;
        static const int kCpuHasRVVZVFH = 0x200;

        // These flags are only valid on x86 processors.
        static const int kCpuHasX86 = 0x8;
        static const int kCpuHasSSE2 = 0x100;
        static const int kCpuHasSSSE3 = 0x200;
        static const int kCpuHasSSE41 = 0x400;
        static const int kCpuHasSSE42 = 0x800;
        static const int kCpuHasAVX = 0x1000;
        static const int kCpuHasAVX2 = 0x2000;
        static const int kCpuHasERMS = 0x4000;
        static const int kCpuHasFSMR = 0x8000;
        static const int kCpuHasFMA3 = 0x10000;
        static const int kCpuHasF16C = 0x20000;
        static const int kCpuHasAVX512BW = 0x40000;
        static const int kCpuHasAVX512VL = 0x80000;
        static const int kCpuHasAVX512VNNI = 0x100000;
        static const int kCpuHasAVX512VBMI = 0x200000;
        static const int kCpuHasAVX512VBMI2 = 0x400000;
        static const int kCpuHasAVX512VBITALG = 0x800000;
        static const int kCpuHasAVX10 = 0x1000000;
        static const int kCpuHasAVX10_2 = 0x2000000;
        static const int kCpuHasAVXVNNI = 0x4000000;
        static const int kCpuHasAVXVNNIINT8 = 0x8000000;
        static const int kCpuHasAMXINT8 = 0x10000000;

        // These flags are only valid on LOONGARCH processors.
        static const int kCpuHasLOONGARCH = 0x20;
        static const int kCpuHasLSX = 0x100;
        static const int kCpuHasLASX = 0x200;

        // Optional init function. TestCpuFlag does an auto-init.
        // Returns cpu_info flags.
        LIBYUV_API
            int InitCpuFlags(void);

        // Detect CPU has SSE2 etc.
        // Test_flag parameter should be one of kCpuHas constants above.
        // Returns non-zero if instruction set is detected
        static __inline int TestCpuFlag(int test_flag) {
            LIBYUV_API extern int cpu_info_;
#ifdef __ATOMIC_RELAXED
            int cpu_info = __atomic_load_n(&cpu_info_, __ATOMIC_RELAXED);
#else
            int cpu_info = cpu_info_;
#endif
            return (!cpu_info ? InitCpuFlags() : cpu_info) & test_flag;
        }

        // Internal function for parsing /proc/cpuinfo.
        LIBYUV_API
            int ArmCpuCaps(const char* cpuinfo_name);
        LIBYUV_API
            int RiscvCpuCaps(const char* cpuinfo_name);

#ifdef __linux__
        // On Linux, parse AArch64 features from getauxval(AT_HWCAP{,2}).
        LIBYUV_API
            int AArch64CpuCaps(unsigned long hwcap, unsigned long hwcap2);
#else
        LIBYUV_API
            int AArch64CpuCaps();
#endif

        // For testing, allow CPU flags to be disabled.
        // ie MaskCpuFlags(~kCpuHasSSSE3) to disable SSSE3.
        // MaskCpuFlags(-1) to enable all cpu specific optimizations.
        // MaskCpuFlags(1) to disable all cpu specific optimizations.
        // MaskCpuFlags(0) to reset state so next call will auto init.
        // Returns cpu_info flags.
        LIBYUV_API
            int MaskCpuFlags(int enable_flags);

        // Sets the CPU flags to |cpu_flags|, bypassing the detection code. |cpu_flags|
        // should be a valid combination of the kCpuHas constants above and include
        // kCpuInitialized. Use this method when running in a sandboxed process where
        // the detection code might fail (as it might access /proc/cpuinfo). In such
        // cases the cpu_info can be obtained from a non sandboxed process by calling
        // InitCpuFlags() and passed to the sandboxed process (via command line
        // parameters, IPC...) which can then call this method to initialize the CPU
        // flags.
        // Notes:
        // - when specifying 0 for |cpu_flags|, the auto initialization is enabled
        //   again.
        // - enabling CPU features that are not supported by the CPU will result in
        //   undefined behavior.
        // TODO(fbarchard): consider writing a helper function that translates from
        // other library CPU info to libyuv CPU info and add a .md doc that explains
        // CPU detection.
        static __inline void SetCpuFlags(int cpu_flags) {
            LIBYUV_API extern int cpu_info_;
#ifdef __ATOMIC_RELAXED
            __atomic_store_n(&cpu_info_, cpu_flags, __ATOMIC_RELAXED);
#else
            cpu_info_ = cpu_flags;
#endif
        }

        // Low level cpuid for X86. Returns zeros on other CPUs.
        // eax is the info type that you want.
        // ecx is typically the cpu number, and should normally be zero.
        LIBYUV_API
            void CpuId(int info_eax, int info_ecx, int* cpu_info);

#ifdef __cplusplus
    }  // extern "C"
}  // namespace libyuv
#endif



#ifdef __cplusplus
// NOTE: For a simplified public API use convert.h MJPGToI420().

struct jpeg_common_struct;
struct jpeg_decompress_struct;
struct jpeg_source_mgr;

namespace libyuv {

#ifdef __cplusplus
    extern "C" {
#endif

        LIBYUV_BOOL ValidateJpeg(const uint8_t* sample, size_t sample_size);

#ifdef __cplusplus
    }  // extern "C"
#endif

    static const uint32_t kUnknownDataSize = 0xFFFFFFFF;

    enum JpegSubsamplingType {
        kJpegYuv420,
        kJpegYuv422,
        kJpegYuv444,
        kJpegYuv400,
        kJpegUnknown
    };

    struct Buffer {
        const uint8_t* data;
        int len;
    };

    struct BufferVector {
        Buffer* buffers;
        int len;
        int pos;
    };

    struct SetJmpErrorMgr;

    // MJPEG ("Motion JPEG") is a pseudo-standard video codec where the frames are
    // simply independent JPEG images with a fixed huffman table (which is omitted).
    // It is rarely used in video transmission, but is common as a camera capture
    // format, especially in Logitech devices. This class implements a decoder for
    // MJPEG frames.
    //
    // See http://tools.ietf.org/html/rfc2435
    class LIBYUV_API MJpegDecoder {
    public:
        typedef void (*CallbackFunction)(void* opaque,
            const uint8_t* const* data,
            const int* strides,
            int rows);

        static const int kColorSpaceUnknown;
        static const int kColorSpaceGrayscale;
        static const int kColorSpaceRgb;
        static const int kColorSpaceYCbCr;
        static const int kColorSpaceCMYK;
        static const int kColorSpaceYCCK;

        MJpegDecoder();
        ~MJpegDecoder();

        // Loads a new frame, reads its headers, and determines the uncompressed
        // image format.
        // Returns LIBYUV_TRUE if image looks valid and format is supported.
        // If return value is LIBYUV_TRUE, then the values for all the following
        // getters are populated.
        // src_len is the size of the compressed mjpeg frame in bytes.
        LIBYUV_BOOL LoadFrame(const uint8_t* src, size_t src_len);

        // Returns width of the last loaded frame in pixels.
        int GetWidth();

        // Returns height of the last loaded frame in pixels.
        int GetHeight();

        // Returns format of the last loaded frame. The return value is one of the
        // kColorSpace* constants.
        int GetColorSpace();

        // Number of color components in the color space.
        int GetNumComponents();

        // Sample factors of the n-th component.
        int GetHorizSampFactor(int component);

        int GetVertSampFactor(int component);

        int GetHorizSubSampFactor(int component);

        int GetVertSubSampFactor(int component);

        // Public for testability.
        int GetImageScanlinesPerImcuRow();

        // Public for testability.
        int GetComponentScanlinesPerImcuRow(int component);

        // Width of a component in bytes.
        int GetComponentWidth(int component);

        // Height of a component.
        int GetComponentHeight(int component);

        // Width of a component in bytes with padding for DCTSIZE. Public for testing.
        int GetComponentStride(int component);

        // Size of a component in bytes.
        int GetComponentSize(int component);

        // Call this after LoadFrame() if you decide you don't want to decode it
        // after all.
        LIBYUV_BOOL UnloadFrame();

        // Decodes the entire image into a one-buffer-per-color-component format.
        // dst_width must match exactly. dst_height must be <= to image height; if
        // less, the image is cropped. "planes" must have size equal to at least
        // GetNumComponents() and they must point to non-overlapping buffers of size
        // at least GetComponentSize(i). The pointers in planes are incremented
        // to point to after the end of the written data.
        // TODO(fbarchard): Add dst_x, dst_y to allow specific rect to be decoded.
        LIBYUV_BOOL DecodeToBuffers(uint8_t** planes, int dst_width, int dst_height);

        // Decodes the entire image and passes the data via repeated calls to a
        // callback function. Each call will get the data for a whole number of
        // image scanlines.
        // TODO(fbarchard): Add dst_x, dst_y to allow specific rect to be decoded.
        LIBYUV_BOOL DecodeToCallback(CallbackFunction fn,
            void* opaque,
            int dst_width,
            int dst_height);

        // The helper function which recognizes the jpeg sub-sampling type.
        static JpegSubsamplingType JpegSubsamplingTypeHelper(
            int* subsample_x,
            int* subsample_y,
            int number_of_components);

    private:
        void AllocOutputBuffers(int num_outbufs);
        void DestroyOutputBuffers();

        LIBYUV_BOOL StartDecode();
        LIBYUV_BOOL FinishDecode();

        void SetScanlinePointers(uint8_t** data);
        LIBYUV_BOOL DecodeImcuRow();

        int GetComponentScanlinePadding(int component);

        // A buffer holding the input data for a frame.
        Buffer buf_;
        BufferVector buf_vec_;

        jpeg_decompress_struct* decompress_struct_;
        jpeg_source_mgr* source_mgr_;
        SetJmpErrorMgr* error_mgr_;

        // LIBYUV_TRUE iff at least one component has scanline padding. (i.e.,
        // GetComponentScanlinePadding() != 0.)
        LIBYUV_BOOL has_scanline_padding_;

        // Temporaries used to point to scanline outputs.
        int num_outbufs_;  // Outermost size of all arrays below.
        uint8_t*** scanlines_;
        int* scanlines_sizes_;
        // Temporary buffer used for decoding when we can't decode directly to the
        // output buffers. Large enough for just one iMCU row.
        uint8_t** databuf_;
        int* databuf_strides_;
    };

}  // namespace libyuv

#endif  //  __cplusplus




#ifdef __cplusplus
namespace libyuv {
    extern "C" {
#endif

        // Rotate ARGB frame
        LIBYUV_API
            int ARGBRotate(const uint8_t* src_argb,
                int src_stride_argb,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int src_width,
                int src_height,
                enum RotationMode mode);

#ifdef __cplusplus
    }  // extern "C"
}  // namespace libyuv
#endif


#include <stddef.h>  // For NULL
#include <stdlib.h>  // For malloc



#ifdef __cplusplus
namespace libyuv {
    extern "C" {
#endif

#if defined(__pnacl__) || defined(__CLR_VER) ||            \
    (defined(__native_client__) && defined(__x86_64__)) || \
    (defined(__i386__) && !defined(__SSE__) && !defined(__clang__))
#define LIBYUV_DISABLE_X86
#endif

#if defined(__native_client__)
#define LIBYUV_DISABLE_NEON
#endif

        // MemorySanitizer does not support assembly code yet. http://crbug.com/344505
#if defined(__has_feature)
#if __has_feature(memory_sanitizer)
#if !defined(LIBYUV_DISABLE_NEON)
#define LIBYUV_DISABLE_NEON
#endif
#if !defined(LIBYUV_DISABLE_SME)
#define LIBYUV_DISABLE_SME
#endif
#if !defined(LIBYUV_DISABLE_SVE)
#define LIBYUV_DISABLE_SVE
#endif
#if !defined(LIBYUV_DISABLE_X86)
#define LIBYUV_DISABLE_X86
#endif
#endif  // __has_feature(memory_sanitizer)
#endif  // defined(__has_feature)

// clang >= 3.5.0 required for Arm64.
#if defined(__clang__) && defined(__aarch64__) && !defined(LIBYUV_DISABLE_NEON)
#if (__clang_major__ < 3) || (__clang_major__ == 3 && (__clang_minor__ < 5))
#define LIBYUV_DISABLE_NEON
#endif  // clang >= 3.5
#endif  // __clang__

// GCC >= 4.7.0 required for AVX2.
#if defined(__GNUC__) && !defined(LIBYUV_ENABLE_ROWWIN) && \
    (defined(__x86_64__) || defined(__i386__))
#if (__GNUC__ > 4) || (__GNUC__ == 4 && (__GNUC_MINOR__ >= 7))
#define GCC_HAS_AVX2 1
#endif  // GNUC >= 4.7
#endif  // __GNUC__

// clang >= 3.4.0 required for AVX2.
#if defined(__clang__) && !defined(LIBYUV_ENABLE_ROWWIN) && \
    (defined(__x86_64__) || defined(__i386__))
#if (__clang_major__ > 3) || (__clang_major__ == 3 && (__clang_minor__ >= 4))
#define CLANG_HAS_AVX2 1
#endif  // clang >= 3.4
#endif  // __clang__

// clang >= 6.0.0 required for AVX512.
#if defined(__clang__) && !defined(LIBYUV_ENABLE_ROWWIN) && \
    (defined(__x86_64__) || defined(__i386__))
// clang in xcode follows a different versioning scheme.
// TODO(fbarchard): fix xcode 9 ios b/789.
#if (__clang_major__ >= 7) && !defined(__APPLE__)
#define CLANG_HAS_AVX512 1
#endif  // clang >= 7
#endif  // __clang__

// Visual C 2012 required for AVX2.
#if defined(_M_IX86) &&                                       \
    (!defined(__clang__) || defined(LIBYUV_ENABLE_ROWWIN)) && \
    defined(_MSC_VER) && _MSC_VER >= 1700
#define VISUALC_HAS_AVX2 1
#endif  // VisualStudio >= 2012

// Clang 19 required for SME due to needing __arm_tpidr2_save from compiler-rt,
// only enabled on Linux and Android (both define __linux__) for now.
#if !defined(LIBYUV_DISABLE_SME) && defined(__aarch64__) && \
    defined(__linux__) && defined(__clang__) && (__clang_major__ >= 19)
#define CLANG_HAS_SME 1
#endif

#ifdef __cplusplus
    }  // extern "C"
}  // namespace libyuv
#endif




#ifdef __cplusplus
namespace libyuv {
    extern "C" {
#endif

        // The following are available on all x86 platforms:
#if !defined(LIBYUV_DISABLE_X86) && defined(_MSC_VER) &&  !defined(__clang__) &&                            \
    (defined(_M_IX86) || (defined(__x86_64__)  &&  !defined(LIBYUV_ENABLE_ROWWIN) ) || defined(__i386__)) \
// Conversions:
#define HAS_ARGB1555TOARGBROW_SSE2
#define HAS_ARGB4444TOARGBROW_SSE2
#define HAS_ARGBEXTRACTALPHAROW_SSE2
#define HAS_ARGBSETROW_X86
#define HAS_ARGBSHUFFLEROW_SSSE3
#define HAS_ARGBTOARGB1555ROW_SSE2
#define HAS_ARGBTOARGB4444ROW_SSE2
#define HAS_ARGBTORAWROW_SSSE3
#define HAS_ARGBTORGB24ROW_SSSE3
#define HAS_ARGBTORGB565DITHERROW_SSE2
#define HAS_ARGBTORGB565ROW_SSE2
#define HAS_COPYROW_ERMS
#define HAS_COPYROW_SSE2
#define HAS_H422TOARGBROW_SSSE3
#define HAS_HALFFLOATROW_SSE2
#define HAS_I422TOARGB1555ROW_SSSE3
#define HAS_I422TOARGB4444ROW_SSSE3
#define HAS_I422TOARGBROW_SSSE3
#define HAS_I422TORGB24ROW_SSSE3
#define HAS_I422TORGB565ROW_SSSE3
#define HAS_I422TORGBAROW_SSSE3
#define HAS_I422TOUYVYROW_SSE2
#define HAS_I422TOYUY2ROW_SSE2
#define HAS_I444TOARGBROW_SSSE3
#define HAS_I444TORGB24ROW_SSSE3
#define HAS_INTERPOLATEROW_SSSE3
#define HAS_J400TOARGBROW_SSE2
#define HAS_J422TOARGBROW_SSSE3
#define HAS_MERGEUVROW_SSE2
#define HAS_MIRRORROW_SSSE3
#define HAS_MIRRORSPLITUVROW_SSSE3
#define HAS_NV12TOARGBROW_SSSE3
#define HAS_NV12TORGB24ROW_SSSE3
#define HAS_NV12TORGB565ROW_SSSE3
#define HAS_NV21TOARGBROW_SSSE3
#define HAS_NV21TORGB24ROW_SSSE3
#define HAS_RAWTOARGBROW_SSSE3
#define HAS_RAWTORGB24ROW_SSSE3
#define HAS_RGB24TOARGBROW_SSSE3
#define HAS_RGB565TOARGBROW_SSE2
#define HAS_SETROW_ERMS
#define HAS_SETROW_X86
#define HAS_SPLITUVROW_SSE2
#define HAS_UYVYTOARGBROW_SSSE3
#define HAS_UYVYTOUV422ROW_SSE2
#define HAS_UYVYTOUVROW_SSE2
#define HAS_UYVYTOYROW_SSE2
#define HAS_YUY2TOARGBROW_SSSE3
#define HAS_YUY2TOUV422ROW_SSE2
#define HAS_YUY2TOUVROW_SSE2
#define HAS_YUY2TOYROW_SSE2

// Effects:
#define HAS_ARGBADDROW_SSE2
#define HAS_ARGBAFFINEROW_SSE2
#define HAS_ARGBBLENDROW_SSSE3
#define HAS_ARGBCOLORMATRIXROW_SSSE3
#define HAS_ARGBCOLORTABLEROW_X86
#define HAS_ARGBCOPYALPHAROW_SSE2
#define HAS_ARGBCOPYYTOALPHAROW_SSE2
#define HAS_ARGBGRAYROW_SSSE3
#define HAS_ARGBLUMACOLORTABLEROW_SSSE3
#define HAS_ARGBMIRRORROW_SSE2
// TODO: Re-enable once rounding behaviour is fixed.
// #define HAS_ARGBMULTIPLYROW_SSE2
#define HAS_ARGBPOLYNOMIALROW_SSE2
#define HAS_ARGBQUANTIZEROW_SSE2
#define HAS_ARGBSEPIAROW_SSSE3
#define HAS_ARGBSHADEROW_SSE2
#define HAS_ARGBSUBTRACTROW_SSE2
#define HAS_BLENDPLANEROW_SSSE3
#define HAS_COMPUTECUMULATIVESUMROW_SSE2
#define HAS_CUMULATIVESUMTOAVERAGEROW_SSE2
#define HAS_RGBCOLORTABLEROW_X86
#define HAS_SOBELROW_SSE2
#define HAS_SOBELTOPLANEROW_SSE2
#define HAS_SOBELXROW_SSE2
#define HAS_SOBELXYROW_SSE2
#define HAS_SOBELYROW_SSE2

// The following functions fail on gcc/clang 32 bit with fpic and framepointer.
// caveat: clangcl uses row_win.cc which works.
#if (defined(__x86_64__) || !defined(__pic__) || defined(__clang__) || \
     defined(_MSC_VER)) &&                                             \
    !defined(LIBYUV_ENABLE_ROWWIN)
// TODO(fbarchard): fix build error on android_full_debug=1
// https://code.google.com/p/libyuv/issues/detail?id=517
#define HAS_I422ALPHATOARGBROW_SSSE3
#define HAS_I444ALPHATOARGBROW_SSSE3
#endif
#if (defined(__x86_64__) || !defined(__pic__) || defined(__clang__) || \
     defined(_MSC_VER)) &&                                             \
    !defined(LIBYUV_ENABLE_ROWWIN)
// TODO(fbarchard): fix build error on android_full_debug=1
// https://code.google.com/p/libyuv/issues/detail?id=517
// TODO(fbarchard): fix LIBYUV_ENABLE_ROWWIN with clang
#define HAS_I422ALPHATOARGBROW_AVX2
#define HAS_I444ALPHATOARGBROW_AVX2
#endif

#endif

// The following are available on all x86 platforms, but
// require VS2012, clang 3.4 or gcc 4.7.
#if !defined(LIBYUV_DISABLE_X86) && defined(_MSC_VER) &&  !defined(__clang__)  &&                     \
    (defined(VISUALC_HAS_AVX2) || defined(CLANG_HAS_AVX2) || \
     defined(GCC_HAS_AVX2))
#define HAS_ARGBCOPYALPHAROW_AVX2
#define HAS_ARGBCOPYYTOALPHAROW_AVX2
#define HAS_ARGBEXTRACTALPHAROW_AVX2
#define HAS_ARGBMIRRORROW_AVX2
#define HAS_ARGBPOLYNOMIALROW_AVX2
#define HAS_ARGBSHUFFLEROW_AVX2
#define HAS_ARGBTORGB565DITHERROW_AVX2
#define HAS_COPYROW_AVX
#define HAS_H422TOARGBROW_AVX2
#define HAS_HALFFLOATROW_AVX2
#define HAS_I422TOARGB1555ROW_AVX2
#define HAS_I422TOARGB4444ROW_AVX2
#define HAS_I422TOARGBROW_AVX2
#define HAS_I422TORGB24ROW_AVX2
#define HAS_I422TORGB565ROW_AVX2
#define HAS_I422TORGBAROW_AVX2
#define HAS_I444TOARGBROW_AVX2
#define HAS_I444TORGB24ROW_AVX2
#define HAS_INTERPOLATEROW_AVX2
#define HAS_J422TOARGBROW_AVX2
#define HAS_MERGEUVROW_AVX2
#define HAS_MIRRORROW_AVX2
#define HAS_NV12TOARGBROW_AVX2
#define HAS_NV12TORGB24ROW_AVX2
#define HAS_NV12TORGB565ROW_AVX2
#define HAS_NV21TOARGBROW_AVX2
#define HAS_NV21TORGB24ROW_AVX2
#define HAS_SPLITUVROW_AVX2
#define HAS_UYVYTOARGBROW_AVX2
#define HAS_UYVYTOUV422ROW_AVX2
#define HAS_UYVYTOUVROW_AVX2
#define HAS_UYVYTOYROW_AVX2
#define HAS_YUY2TOARGBROW_AVX2
#define HAS_YUY2TOUV422ROW_AVX2
#define HAS_YUY2TOUVROW_AVX2
#define HAS_YUY2TOYROW_AVX2
//  #define HAS_HALFFLOATROW_F16C  // Enable to test half float cast

// Effects:
#define HAS_ARGBADDROW_AVX2
// TODO: Re-enable once rounding behaviour is fixed.
// #define HAS_ARGBMULTIPLYROW_AVX2
#define HAS_ARGBSUBTRACTROW_AVX2
#define HAS_BLENDPLANEROW_AVX2
#endif

// The following are available for AVX2 Visual C 32 bit:
// TODO(fbarchard): Port to gcc.
#if !defined(LIBYUV_DISABLE_X86) && defined(_M_IX86) && defined(_MSC_VER) && \
    !defined(__clang__) && defined(VISUALC_HAS_AVX2)
#define HAS_ARGB1555TOARGBROW_AVX2
#define HAS_ARGB4444TOARGBROW_AVX2
#define HAS_ARGBTOARGB1555ROW_AVX2
#define HAS_ARGBTOARGB4444ROW_AVX2
#define HAS_ARGBTORGB565ROW_AVX2
#define HAS_J400TOARGBROW_AVX2
#define HAS_RGB565TOARGBROW_AVX2
#endif

// The following are also available on x64 Visual C.
#if !defined(LIBYUV_DISABLE_X86) && defined(_MSC_VER) && defined(_M_X64) && \
    (!defined(__clang__) || defined(__SSSE3__))
#define HAS_I444ALPHATOARGBROW_SSSE3
#define HAS_I444TOARGBROW_SSSE3
#define HAS_I422ALPHATOARGBROW_SSSE3
#define HAS_I422TOARGBROW_SSSE3
#endif

// The following are available for gcc/clang x86 platforms:
// TODO(fbarchard): Port to Visual C
#if !defined(LIBYUV_DISABLE_X86) &&  !defined(__clang__) &&             \
    (defined(__x86_64__) || defined(__i386__)) && \
    !defined(LIBYUV_ENABLE_ROWWIN)
#define HAS_RAWTOYJROW_SSSE3
#define HAS_AB64TOARGBROW_SSSE3
#define HAS_ABGRTOAR30ROW_SSSE3
#define HAS_ABGRTOYJROW_SSSE3
#define HAS_AR64TOARGBROW_SSSE3
#define HAS_ARGBATTENUATEROW_SSSE3
#define HAS_ARGBTOAB64ROW_SSSE3
#define HAS_ARGBTOAR30ROW_SSSE3
#define HAS_ARGBTOAR64ROW_SSSE3
#define HAS_ARGBTOUV444ROW_SSSE3
#define HAS_ARGBTOUVJ444ROW_SSSE3
#define HAS_ARGBUNATTENUATEROW_SSE2
#define HAS_CONVERT16TO8ROW_SSSE3
#define HAS_CONVERT8TO16ROW_SSE2
#define HAS_DETILEROW_16_SSE2
#define HAS_DETILEROW_SSE2
#define HAS_DETILESPLITUVROW_SSSE3
#define HAS_DETILETOYUY2_SSE2
#define HAS_HALFMERGEUVROW_SSSE3
#define HAS_I210TOAR30ROW_SSSE3
#define HAS_I210TOARGBROW_SSSE3
#define HAS_I212TOAR30ROW_SSSE3
#define HAS_I212TOARGBROW_SSSE3
#define HAS_I400TOARGBROW_SSE2
#define HAS_I410TOAR30ROW_SSSE3
#define HAS_I410TOARGBROW_SSSE3
#define HAS_I422TOAR30ROW_SSSE3
#define HAS_MERGEARGBROW_SSE2
#define HAS_MERGERGBROW_SSSE3
#define HAS_MERGEXRGBROW_SSE2
#define HAS_MIRRORUVROW_SSSE3
#define HAS_NV21TOYUV24ROW_SSSE3
#define HAS_P210TOAR30ROW_SSSE3
#define HAS_P210TOARGBROW_SSSE3
#define HAS_P410TOAR30ROW_SSSE3
#define HAS_P410TOARGBROW_SSSE3
#define HAS_RAWTOARGBROW_AVX2
#define HAS_RAWTORGBAROW_SSSE3
#define HAS_RGB24MIRRORROW_SSSE3
#define HAS_RGBATOYJROW_SSSE3
#define HAS_SPLITARGBROW_SSE2
#define HAS_SPLITARGBROW_SSSE3
#define HAS_SPLITRGBROW_SSE41
#define HAS_SPLITRGBROW_SSSE3
#define HAS_SPLITXRGBROW_SSE2
#define HAS_SPLITXRGBROW_SSSE3
#define HAS_SWAPUVROW_SSSE3
#define HAS_YUY2TONVUVROW_SSE2
// TODO: port row_win to use 8 bit coefficients.
#define HAS_ARGBTOYJROW_SSSE3
#define HAS_ARGBTOYROW_SSSE3
#define HAS_BGRATOYROW_SSSE3
#define HAS_RAWTOYROW_SSSE3
#define HAS_ABGRTOYROW_SSSE3
#define HAS_RGB24TOYJROW_SSSE3
#define HAS_RGB24TOYROW_SSSE3
#define HAS_RGBATOYROW_SSSE3

// TODO: adjust row_win to use 8 bit negative coefficients.
#define HAS_ABGRTOUVJROW_SSSE3
#define HAS_ARGBTOUVJROW_SSSE3
#define HAS_ABGRTOUVROW_SSSE3
#define HAS_ARGBTOUVROW_SSSE3
#define HAS_BGRATOUVROW_SSSE3
#define HAS_RGBATOUVROW_SSSE3

#if defined(__x86_64__) || !defined(__pic__)
// TODO(fbarchard): fix build error on android_full_debug=1
// https://code.google.com/p/libyuv/issues/detail?id=517
#define HAS_I210ALPHATOARGBROW_SSSE3
#define HAS_I410ALPHATOARGBROW_SSSE3
#endif
#endif

// The following are available for AVX2 gcc/clang x86 platforms:
// TODO(fbarchard): Port to Visual C
#if !defined(LIBYUV_DISABLE_X86)&&  !defined(__clang__) &&                       \
    (defined(__x86_64__) || defined(__i386__)) &&         \
    (defined(CLANG_HAS_AVX2) || defined(GCC_HAS_AVX2)) && \
    !defined(LIBYUV_ENABLE_ROWWIN)
#define HAS_AB64TOARGBROW_AVX2
#define HAS_ABGRTOAR30ROW_AVX2
#define HAS_ABGRTOUVJROW_AVX2
#define HAS_ABGRTOUVROW_AVX2
#define HAS_ABGRTOYJROW_AVX2
#define HAS_ABGRTOYROW_AVX2
#define HAS_AR64TOARGBROW_AVX2
#define HAS_ARGBATTENUATEROW_AVX2
#define HAS_ARGBTOAB64ROW_AVX2
#define HAS_ARGBTOAR30ROW_AVX2
#define HAS_ARGBTOAR64ROW_AVX2
#define HAS_ARGBTORAWROW_AVX2
#define HAS_ARGBTORGB24ROW_AVX2
#define HAS_ARGBTOUV444ROW_AVX2
#define HAS_ARGBTOUVJ444ROW_AVX2
#define HAS_ARGBTOUVJROW_AVX2
#define HAS_ARGBTOUVROW_AVX2
#define HAS_ARGBTOYJROW_AVX2
#define HAS_ARGBTOYROW_AVX2
#define HAS_ARGBUNATTENUATEROW_AVX2
#define HAS_CONVERT16TO8ROW_AVX2
#define HAS_CONVERT8TO16ROW_AVX2
#define HAS_CONVERT8TO8ROW_AVX2
#define HAS_DETILEROW_16_AVX
#define HAS_DIVIDEROW_16_AVX2
#define HAS_HALFMERGEUVROW_AVX2
#define HAS_I210TOAR30ROW_AVX2
#define HAS_I210TOARGBROW_AVX2
#define HAS_I212TOAR30ROW_AVX2
#define HAS_I212TOARGBROW_AVX2
#define HAS_I400TOARGBROW_AVX2
#define HAS_I410TOAR30ROW_AVX2
#define HAS_I410TOARGBROW_AVX2
#define HAS_I422TOAR30ROW_AVX2
#define HAS_I422TOUYVYROW_AVX2
#define HAS_I422TOYUY2ROW_AVX2
#define HAS_INTERPOLATEROW_16TO8_AVX2
#define HAS_MERGEAR64ROW_AVX2
#define HAS_MERGEARGB16TO8ROW_AVX2
#define HAS_MERGEARGBROW_AVX2
#define HAS_MERGEUVROW_16_AVX2
#define HAS_MERGEXR30ROW_AVX2
#define HAS_MERGEXR64ROW_AVX2
#define HAS_MERGEXRGB16TO8ROW_AVX2
#define HAS_MERGEXRGBROW_AVX2
#define HAS_MIRRORUVROW_AVX2
#define HAS_MULTIPLYROW_16_AVX2
#define HAS_NV21TOYUV24ROW_AVX2
#define HAS_P210TOAR30ROW_AVX2
#define HAS_P210TOARGBROW_AVX2
#define HAS_P410TOAR30ROW_AVX2
#define HAS_P410TOARGBROW_AVX2
#define HAS_RAWTOYJROW_AVX2
#define HAS_RGB24TOYJROW_AVX2
#define HAS_RGBATOYJROW_AVX2
#define HAS_SPLITARGBROW_AVX2
#define HAS_SPLITRGBROW_AVX2
#define HAS_SPLITUVROW_16_AVX2
#define HAS_SPLITXRGBROW_AVX2
#define HAS_SWAPUVROW_AVX2
#define HAS_YUY2TONVUVROW_AVX2

#if defined(__x86_64__) || !defined(__pic__)
// TODO(fbarchard): fix build error on android_full_debug=1
// https://code.google.com/p/libyuv/issues/detail?id=517
#define HAS_I210ALPHATOARGBROW_AVX2
#define HAS_I410ALPHATOARGBROW_AVX2
#endif
#endif

// The following are available for AVX512 clang x86 platforms:
// TODO(fbarchard): Port to GCC and Visual C
// TODO(b/42280744): re-enable HAS_ARGBTORGB24ROW_AVX512VBMI.
#if !defined(LIBYUV_DISABLE_X86)&& defined(_MSC_VER) &&  !defined(__clang__)  && \
   (defined(__x86_64__) || defined(__i386__)) && defined(CLANG_HAS_AVX512) && \
    !defined(LIBYUV_ENABLE_ROWWIN)
#define HAS_COPYROW_AVX512BW
#define HAS_ARGBTORGB24ROW_AVX512VBMI
#define HAS_CONVERT16TO8ROW_AVX512BW
#define HAS_MERGEUVROW_AVX512BW
#endif

// The following are available for AVX512 clang x64 platforms:
// TODO(fbarchard): Port to x86
#if !defined(LIBYUV_DISABLE_X86) && !defined(__clang__) && defined(__x86_64__) && \
    (defined(CLANG_HAS_AVX512))
#define HAS_I422TOARGBROW_AVX512BW
#endif

// The following are available on Neon platforms:
#if !defined(LIBYUV_DISABLE_NEON) && \
    (defined(__aarch64__) || defined(__ARM_NEON__) || defined(LIBYUV_NEON))
#define HAS_AB64TOARGBROW_NEON
#define HAS_ABGRTOUVJROW_NEON
#define HAS_ABGRTOUVROW_NEON
#define HAS_ABGRTOYJROW_NEON
#define HAS_ABGRTOYROW_NEON
#define HAS_AR64TOARGBROW_NEON
#define HAS_ARGB1555TOARGBROW_NEON
#define HAS_ARGB1555TOUVROW_NEON
#define HAS_ARGB1555TOYROW_NEON
#define HAS_ARGB4444TOARGBROW_NEON
#define HAS_ARGB4444TOUVROW_NEON
#define HAS_ARGB4444TOYROW_NEON
#define HAS_ARGBEXTRACTALPHAROW_NEON
#define HAS_ARGBSETROW_NEON
#define HAS_ARGBTOAB64ROW_NEON
#define HAS_ARGBTOAR64ROW_NEON
#define HAS_ARGBTOARGB1555ROW_NEON
#define HAS_ARGBTOARGB4444ROW_NEON
#define HAS_ARGBTORAWROW_NEON
#define HAS_ARGBTORGB24ROW_NEON
#define HAS_ARGBTORGB565DITHERROW_NEON
#define HAS_ARGBTORGB565ROW_NEON
#define HAS_ARGBTOUV444ROW_NEON
#define HAS_ARGBTOUVJ444ROW_NEON
#define HAS_ARGBTOUVJROW_NEON
#define HAS_ARGBTOUVROW_NEON
#define HAS_ARGBTOYJROW_NEON
#define HAS_ARGBTOYROW_NEON
#define HAS_AYUVTOUVROW_NEON
#define HAS_AYUVTOVUROW_NEON
#define HAS_AYUVTOYROW_NEON
#define HAS_BGRATOUVROW_NEON
#define HAS_BGRATOYROW_NEON
#define HAS_BYTETOFLOATROW_NEON
#define HAS_CONVERT16TO8ROW_NEON
#define HAS_CONVERT8TO8ROW_NEON
#define HAS_COPYROW_NEON
#define HAS_DETILEROW_16_NEON
#define HAS_DETILEROW_NEON
#define HAS_DETILESPLITUVROW_NEON
#define HAS_DETILETOYUY2_NEON
#define HAS_DIVIDEROW_16_NEON
#define HAS_HALFFLOATROW_NEON
#define HAS_HALFMERGEUVROW_NEON
#define HAS_I400TOARGBROW_NEON
#define HAS_I422ALPHATOARGBROW_NEON
#define HAS_I422TOARGB1555ROW_NEON
#define HAS_I422TOARGB4444ROW_NEON
#define HAS_I422TOARGBROW_NEON
#define HAS_I422TORGB24ROW_NEON
#define HAS_I422TORGB565ROW_NEON
#define HAS_I422TORGBAROW_NEON
#define HAS_I422TOUYVYROW_NEON
#define HAS_I422TOYUY2ROW_NEON
#define HAS_I444ALPHATOARGBROW_NEON
#define HAS_I444TOARGBROW_NEON
#define HAS_I444TORGB24ROW_NEON
#define HAS_INTERPOLATEROW_16_NEON
#define HAS_INTERPOLATEROW_NEON
#define HAS_J400TOARGBROW_NEON
#define HAS_MERGEAR64ROW_NEON
#define HAS_MERGEARGB16TO8ROW_NEON
#define HAS_MERGEARGBROW_NEON
#define HAS_MERGEUVROW_16_NEON
#define HAS_MERGEUVROW_NEON
#define HAS_MERGEXR30ROW_NEON
#define HAS_MERGEXR64ROW_NEON
#define HAS_MERGEXRGB16TO8ROW_NEON
#define HAS_MERGEXRGBROW_NEON
#define HAS_MIRRORROW_NEON
#define HAS_MIRRORSPLITUVROW_NEON
#define HAS_MIRRORUVROW_NEON
#define HAS_MULTIPLYROW_16_NEON
#define HAS_NV12TOARGBROW_NEON
#define HAS_NV12TORGB24ROW_NEON
#define HAS_NV12TORGB565ROW_NEON
#define HAS_NV21TOARGBROW_NEON
#define HAS_NV21TORGB24ROW_NEON
#define HAS_NV21TOYUV24ROW_NEON
#define HAS_RAWTOARGBROW_NEON
#define HAS_RAWTORGB24ROW_NEON
#define HAS_RAWTORGBAROW_NEON
#define HAS_RAWTOUVJROW_NEON
#define HAS_RAWTOUVROW_NEON
#define HAS_RAWTOYJROW_NEON
#define HAS_RAWTOYROW_NEON
#define HAS_RGB24TOARGBROW_NEON
#define HAS_RGB24TOUVJROW_NEON
#define HAS_RGB24TOUVROW_NEON
#define HAS_RGB24TOYJROW_NEON
#define HAS_RGB24TOYROW_NEON
#define HAS_RGB565TOARGBROW_NEON
#define HAS_RGB565TOUVROW_NEON
#define HAS_RGB565TOYROW_NEON
#define HAS_RGBATOUVROW_NEON
#define HAS_RGBATOYJROW_NEON
#define HAS_RGBATOYROW_NEON
#define HAS_SETROW_NEON
#define HAS_SPLITARGBROW_NEON
#define HAS_SPLITRGBROW_NEON
#define HAS_SPLITUVROW_16_NEON
#define HAS_SPLITUVROW_NEON
#define HAS_SPLITXRGBROW_NEON
#define HAS_SWAPUVROW_NEON
#define HAS_UNPACKMT2T_NEON
#define HAS_UYVYTOARGBROW_NEON
#define HAS_UYVYTOUV422ROW_NEON
#define HAS_UYVYTOUVROW_NEON
#define HAS_UYVYTOYROW_NEON
#define HAS_YUY2TOARGBROW_NEON
#define HAS_YUY2TONVUVROW_NEON
#define HAS_YUY2TOUV422ROW_NEON
#define HAS_YUY2TOUVROW_NEON
#define HAS_YUY2TOYROW_NEON

// Effects:
#define HAS_ARGBADDROW_NEON
#define HAS_ARGBATTENUATEROW_NEON
#define HAS_ARGBBLENDROW_NEON
#define HAS_ARGBCOLORMATRIXROW_NEON
#define HAS_ARGBGRAYROW_NEON
#define HAS_ARGBMIRRORROW_NEON
#define HAS_ARGBMULTIPLYROW_NEON
#define HAS_ARGBQUANTIZEROW_NEON
#define HAS_ARGBSEPIAROW_NEON
#define HAS_ARGBSHADEROW_NEON
#define HAS_ARGBSHUFFLEROW_NEON
#define HAS_ARGBSUBTRACTROW_NEON
#define HAS_RGB24MIRRORROW_NEON
#define HAS_SOBELROW_NEON
#define HAS_SOBELTOPLANEROW_NEON
#define HAS_SOBELXROW_NEON
#define HAS_SOBELXYROW_NEON
#define HAS_SOBELYROW_NEON
#endif

// The following are available on AArch64 platforms:
#if !defined(LIBYUV_DISABLE_NEON) && defined(__aarch64__)
#define HAS_ABGRTOAR30ROW_NEON
#define HAS_ARGBTOAR30ROW_NEON
#define HAS_CONVERT8TO16ROW_NEON
#define HAS_I210ALPHATOARGBROW_NEON
#define HAS_I210TOAR30ROW_NEON
#define HAS_I210TOARGBROW_NEON
#define HAS_I212TOAR30ROW_NEON
#define HAS_I212TOARGBROW_NEON
#define HAS_I410ALPHATOARGBROW_NEON
#define HAS_I410TOAR30ROW_NEON
#define HAS_I410TOARGBROW_NEON
#define HAS_I422TOAR30ROW_NEON
#define HAS_P210TOAR30ROW_NEON
#define HAS_P210TOARGBROW_NEON
#define HAS_P410TOAR30ROW_NEON
#define HAS_P410TOARGBROW_NEON

#define HAS_ABGRTOYJROW_NEON_DOTPROD
#define HAS_ABGRTOYROW_NEON_DOTPROD
#define HAS_ARGBGRAYROW_NEON_DOTPROD
#define HAS_ARGBSEPIAROW_NEON_DOTPROD
#define HAS_ARGBTOYJROW_NEON_DOTPROD
#define HAS_ARGBTOYROW_NEON_DOTPROD
#define HAS_BGRATOYROW_NEON_DOTPROD
#define HAS_RGBATOYJROW_NEON_DOTPROD
#define HAS_RGBATOYROW_NEON_DOTPROD

#define HAS_ABGRTOUVJROW_NEON_I8MM
#define HAS_ABGRTOUVROW_NEON_I8MM
#define HAS_ARGBCOLORMATRIXROW_NEON_I8MM
#define HAS_ARGBTOUV444ROW_NEON_I8MM
#define HAS_ARGBTOUVJ444ROW_NEON_I8MM
#define HAS_ARGBTOUVJROW_NEON_I8MM
#define HAS_ARGBTOUVROW_NEON_I8MM
#define HAS_BGRATOUVROW_NEON_I8MM
#define HAS_RGBATOUVROW_NEON_I8MM
#endif

// The following are available on AArch64 SVE platforms:
#if !defined(LIBYUV_DISABLE_SVE) && defined(__aarch64__)
#define HAS_ABGRTOUVJROW_SVE2
#define HAS_ABGRTOUVROW_SVE2
#define HAS_ARGB1555TOARGBROW_SVE2
#define HAS_ARGBTORAWROW_SVE2
#define HAS_ARGBTORGB24ROW_SVE2
#define HAS_ARGBTORGB565DITHERROW_SVE2
#define HAS_ARGBTORGB565ROW_SVE2
#define HAS_ARGBTOUVJROW_SVE2
#define HAS_ARGBTOUVROW_SVE2
#define HAS_AYUVTOUVROW_SVE2
#define HAS_AYUVTOVUROW_SVE2
#define HAS_BGRATOUVROW_SVE2
#define HAS_CONVERT8TO8ROW_SVE2
#define HAS_DIVIDEROW_16_SVE2
#define HAS_HALFFLOATROW_SVE2
#define HAS_I210ALPHATOARGBROW_SVE2
#define HAS_I210TOAR30ROW_SVE2
#define HAS_I210TOARGBROW_SVE2
#define HAS_I212TOAR30ROW_SVE2
#define HAS_I212TOARGBROW_SVE2
#define HAS_I400TOARGBROW_SVE2
#define HAS_I410ALPHATOARGBROW_SVE2
#define HAS_I410TOAR30ROW_SVE2
#define HAS_I410TOARGBROW_SVE2
#define HAS_I422ALPHATOARGBROW_SVE2
#define HAS_I422TOAR30ROW_SVE2
#define HAS_I422TOARGB1555ROW_SVE2
#define HAS_I422TOARGB4444ROW_SVE2
#define HAS_I422TOARGBROW_SVE2
#define HAS_I422TORGB24ROW_SVE2
#define HAS_I422TORGB565ROW_SVE2
#define HAS_I422TORGBAROW_SVE2
#define HAS_I444ALPHATOARGBROW_SVE2
#define HAS_I444TOARGBROW_SVE2
#define HAS_I444TORGB24ROW_SVE2
#define HAS_NV12TOARGBROW_SVE2
#define HAS_NV12TORGB24ROW_SVE2
#define HAS_NV21TOARGBROW_SVE2
#define HAS_NV21TORGB24ROW_SVE2
#define HAS_P210TOAR30ROW_SVE2
#define HAS_P210TOARGBROW_SVE2
#define HAS_P410TOAR30ROW_SVE2
#define HAS_P410TOARGBROW_SVE2
#define HAS_RAWTOARGBROW_SVE2
#define HAS_RAWTORGB24ROW_SVE2
#define HAS_RAWTORGBAROW_SVE2
#define HAS_RGB24TOARGBROW_SVE2
#define HAS_RGBATOUVROW_SVE2
#define HAS_UYVYTOARGBROW_SVE2
#define HAS_YUY2TOARGBROW_SVE2
#endif

// The following are available on AArch64 SME platforms:
#if !defined(LIBYUV_DISABLE_SME) && defined(CLANG_HAS_SME) && \
    defined(__aarch64__)
#define HAS_ABGRTOUVJROW_SME
#define HAS_ABGRTOUVROW_SME
#define HAS_ARGBMULTIPLYROW_SME
#define HAS_ARGBTOUVJROW_SME
#define HAS_ARGBTOUVROW_SME
#define HAS_BGRATOUVROW_SME
#define HAS_CONVERT16TO8ROW_SME
#define HAS_CONVERT8TO16ROW_SME
#define HAS_CONVERT8TO8ROW_SME
#define HAS_COPYROW_SME
#define HAS_I210ALPHATOARGBROW_SME
#define HAS_I210TOAR30ROW_SME
#define HAS_I210TOARGBROW_SME
#define HAS_I212TOAR30ROW_SME
#define HAS_I212TOARGBROW_SME
#define HAS_I400TOARGBROW_SME
#define HAS_I410ALPHATOARGBROW_SME
#define HAS_I410TOAR30ROW_SME
#define HAS_I410TOARGBROW_SME
#define HAS_I422ALPHATOARGBROW_SME
#define HAS_I422TOAR30ROW_SME
#define HAS_I422TOARGB1555ROW_SME
#define HAS_I422TOARGB4444ROW_SME
#define HAS_I422TOARGBROW_SME
#define HAS_I422TORGB24ROW_SME
#define HAS_I422TORGB565ROW_SME
#define HAS_I422TORGBAROW_SME
#define HAS_I444ALPHATOARGBROW_SME
#define HAS_I444TOARGBROW_SME
#define HAS_I444TORGB24ROW_SME
#define HAS_INTERPOLATEROW_16_SME
#define HAS_INTERPOLATEROW_16TO8_SME
#define HAS_INTERPOLATEROW_SME
#define HAS_MERGEUVROW_16_SME
#define HAS_MERGEUVROW_SME
#define HAS_MULTIPLYROW_16_SME
#define HAS_NV12TOARGBROW_SME
#define HAS_NV12TORGB24ROW_SME
#define HAS_NV21TOARGBROW_SME
#define HAS_NV21TORGB24ROW_SME
#define HAS_P210TOAR30ROW_SME
#define HAS_P210TOARGBROW_SME
#define HAS_P410TOAR30ROW_SME
#define HAS_P410TOARGBROW_SME
#define HAS_RGBATOUVROW_SME
#define HAS_YUY2TOARGBROW_SME
#endif

// The following are available on AArch64 platforms:
#if !defined(LIBYUV_DISABLE_NEON) && defined(__aarch64__)
#define HAS_GAUSSCOL_F32_NEON
#define HAS_GAUSSROW_F32_NEON
#define HAS_INTERPOLATEROW_16TO8_NEON
#define HAS_SCALESUMSAMPLES_NEON
#endif

#if !defined(LIBYUV_DISABLE_LSX) && defined(__loongarch_sx)
#define HAS_ABGRTOUVROW_LSX
#define HAS_ABGRTOYROW_LSX
#define HAS_ARGB1555TOARGBROW_LSX
#define HAS_ARGB1555TOUVROW_LSX
#define HAS_ARGB1555TOYROW_LSX
#define HAS_ARGB4444TOARGBROW_LSX
#define HAS_ARGBADDROW_LSX
#define HAS_ARGBATTENUATEROW_LSX
#define HAS_ARGBBLENDROW_LSX
#define HAS_ARGBCOLORMATRIXROW_LSX
#define HAS_ARGBEXTRACTALPHAROW_LSX
#define HAS_ARGBGRAYROW_LSX
#define HAS_ARGBSEPIAROW_LSX
#define HAS_ARGBSHADEROW_LSX
#define HAS_ARGBSHUFFLEROW_LSX
#define HAS_ARGBSUBTRACTROW_LSX
#define HAS_ARGBQUANTIZEROW_LSX
#define HAS_ARGBSETROW_LSX
#define HAS_ARGBTOARGB1555ROW_LSX
#define HAS_ARGBTOARGB4444ROW_LSX
#define HAS_ARGBTORAWROW_LSX
#define HAS_ARGBTORGB24ROW_LSX
#define HAS_ARGBTORGB565ROW_LSX
#define HAS_ARGBTORGB565DITHERROW_LSX
#define HAS_ARGBTOUVJROW_LSX
#define HAS_ARGBTOUV444ROW_LSX
#define HAS_ARGBTOUVROW_LSX
#define HAS_ARGBTOYJROW_LSX
#define HAS_ARGBMIRRORROW_LSX
// TODO: Re-enable once rounding behaviour is fixed.
// #define HAS_ARGBMULTIPLYROW_LSX
#define HAS_BGRATOUVROW_LSX
#define HAS_BGRATOYROW_LSX
#define HAS_I400TOARGBROW_LSX
#define HAS_I444TOARGBROW_LSX
#define HAS_INTERPOLATEROW_LSX
#define HAS_I422ALPHATOARGBROW_LSX
#define HAS_I422TOARGB1555ROW_LSX
#define HAS_I422TOARGB4444ROW_LSX
#define HAS_I422TORGB24ROW_LSX
#define HAS_I422TORGB565ROW_LSX
#define HAS_I422TORGBAROW_LSX
#define HAS_I422TOUYVYROW_LSX
#define HAS_I422TOYUY2ROW_LSX
#define HAS_J400TOARGBROW_LSX
#define HAS_MERGEUVROW_LSX
#define HAS_MIRRORROW_LSX
#define HAS_MIRRORUVROW_LSX
#define HAS_MIRRORSPLITUVROW_LSX
#define HAS_NV12TOARGBROW_LSX
#define HAS_NV12TORGB565ROW_LSX
#define HAS_NV21TOARGBROW_LSX
#define HAS_RAWTOARGBROW_LSX
#define HAS_RAWTORGB24ROW_LSX
#define HAS_RAWTOUVROW_LSX
#define HAS_RAWTOYROW_LSX
#define HAS_RGB24TOARGBROW_LSX
#define HAS_RGB24TOUVROW_LSX
#define HAS_RGB24TOYROW_LSX
#define HAS_RGB565TOARGBROW_LSX
#define HAS_RGB565TOUVROW_LSX
#define HAS_RGB565TOYROW_LSX
#define HAS_RGBATOUVROW_LSX
#define HAS_RGBATOYROW_LSX
#define HAS_SETROW_LSX
#define HAS_SOBELROW_LSX
#define HAS_SOBELTOPLANEROW_LSX
#define HAS_SOBELXYROW_LSX
#define HAS_SPLITUVROW_LSX
#define HAS_UYVYTOARGBROW_LSX
#define HAS_UYVYTOUV422ROW_LSX
#define HAS_UYVYTOUVROW_LSX
#define HAS_UYVYTOYROW_LSX
#define HAS_YUY2TOARGBROW_LSX
#define HAS_YUY2TOUVROW_LSX
#define HAS_YUY2TOUV422ROW_LSX
#define HAS_YUY2TOYROW_LSX
#define HAS_ARGBTOYROW_LSX
#define HAS_ABGRTOYJROW_LSX
#define HAS_RGBATOYJROW_LSX
#define HAS_RGB24TOYJROW_LSX
#define HAS_RAWTOYJROW_LSX
#endif

#if !defined(LIBYUV_DISABLE_LSX) && defined(__loongarch_sx)
#define HAS_I422TOARGBROW_LSX
#endif

#if !defined(LIBYUV_DISABLE_LASX) && defined(__loongarch_asx)
#define HAS_ARGB1555TOARGBROW_LASX
#define HAS_ARGB1555TOUVROW_LASX
#define HAS_ARGB1555TOYROW_LASX
#define HAS_ARGB4444TOARGBROW_LASX
#define HAS_ARGBADDROW_LASX
#define HAS_ARGBATTENUATEROW_LASX
#define HAS_ARGBGRAYROW_LASX
#define HAS_ARGBMIRRORROW_LASX
// TODO: Re-enable once rounding behaviour is fixed.
// #define HAS_ARGBMULTIPLYROW_LASX
#define HAS_ARGBSEPIAROW_LASX
#define HAS_ARGBSHADEROW_LASX
#define HAS_ARGBSHUFFLEROW_LASX
#define HAS_ARGBSUBTRACTROW_LASX
#define HAS_ARGBTOARGB1555ROW_LASX
#define HAS_ARGBTOARGB4444ROW_LASX
#define HAS_ARGBTORAWROW_LASX
#define HAS_ARGBTORGB24ROW_LASX
#define HAS_ARGBTORGB565DITHERROW_LASX
#define HAS_ARGBTORGB565ROW_LASX
#define HAS_ARGBTOUV444ROW_LASX
#define HAS_ARGBTOUVJROW_LASX
#define HAS_ARGBTOUVROW_LASX
#define HAS_ARGBTOYJROW_LASX
#define HAS_ARGBTOYROW_LASX
#define HAS_ABGRTOYJROW_LASX
#define HAS_ABGRTOYROW_LASX
#define HAS_I422ALPHATOARGBROW_LASX
#define HAS_I422TOARGB1555ROW_LASX
#define HAS_I422TOARGB4444ROW_LASX
#define HAS_I422TOARGBROW_LASX
#define HAS_I422TORGB24ROW_LASX
#define HAS_I422TORGB565ROW_LASX
#define HAS_I422TORGBAROW_LASX
#define HAS_I422TOUYVYROW_LASX
#define HAS_I422TOYUY2ROW_LASX
#define HAS_MIRRORROW_LASX
#define HAS_MIRRORUVROW_LASX
#define HAS_NV12TOARGBROW_LASX
#define HAS_NV12TORGB565ROW_LASX
#define HAS_NV21TOARGBROW_LASX
#define HAS_RAWTOARGBROW_LASX
#define HAS_RAWTOUVROW_LASX
#define HAS_RAWTOYROW_LASX
#define HAS_RGB24TOARGBROW_LASX
#define HAS_RGB24TOUVROW_LASX
#define HAS_RGB24TOYROW_LASX
#define HAS_RGB565TOARGBROW_LASX
#define HAS_RGB565TOUVROW_LASX
#define HAS_RGB565TOYROW_LASX
#define HAS_UYVYTOUV422ROW_LASX
#define HAS_UYVYTOUVROW_LASX
#define HAS_UYVYTOYROW_LASX
#define HAS_YUY2TOUV422ROW_LASX
#define HAS_YUY2TOUVROW_LASX
#define HAS_YUY2TOYROW_LASX
#define HAS_RGBATOYROW_LASX
#define HAS_RGBATOYJROW_LASX
#define HAS_BGRATOYROW_LASX
#define HAS_RGB24TOYJROW_LASX
#define HAS_RAWTOYJROW_LASX
#endif

#if !defined(LIBYUV_DISABLE_RVV) && defined(__riscv_vector)
#if defined(__riscv_v_intrinsic) && __riscv_v_intrinsic >= 100000
// Since v1.0, vcreate intrinsic is introduced
#define LIBYUV_RVV_HAS_VCREATE
#endif
#if defined(__riscv_v_intrinsic) && __riscv_v_intrinsic >= 12000
// Since v0.12, TUPLE_TYPE is introduced for segment load and store.
#define LIBYUV_RVV_HAS_TUPLE_TYPE
// Since v0.12, VXRM(fixed-point rounding mode) is included in arguments of
// fixed-point intrinsics.
#define LIBYUV_RVV_HAS_VXRM_ARG
#endif
#endif

#if !defined(LIBYUV_DISABLE_RVV) && defined(__riscv_vector)
#define HAS_ABGRTOYJROW_RVV
#define HAS_ABGRTOYROW_RVV
#define HAS_AR64TOARGBROW_RVV
#define HAS_ARGBCOPYYTOALPHAROW_RVV
#define HAS_ARGBEXTRACTALPHAROW_RVV
#define HAS_ARGBTOAR64ROW_RVV
#define HAS_ARGBTOYJROW_RVV
#define HAS_ARGBTOYMATRIXROW_RVV
#define HAS_ARGBTOYROW_RVV
#define HAS_BGRATOYROW_RVV
#define HAS_COPYROW_RVV
#define HAS_INTERPOLATEROW_RVV
#define HAS_RAWTOYJROW_RVV
#define HAS_RAWTOYROW_RVV
#define HAS_RGB24TOYJROW_RVV
#define HAS_RGB24TOYROW_RVV
#define HAS_RGBATOYJROW_RVV
#define HAS_RGBATOYMATRIXROW_RVV
#define HAS_RGBATOYROW_RVV
#define HAS_RGBTOYMATRIXROW_RVV
#define HAS_SPLITARGBROW_RVV
#define HAS_SPLITRGBROW_RVV
#define HAS_SPLITUVROW_RVV
#define HAS_SPLITXRGBROW_RVV

// The following are available for RVV v0.11 and RVV v1.0
// TODO(fbarchard): Port to RVV v0.12 (tuple)
// missing support for vcreate_v:
//  __riscv_vcreate_v_u16m2x2
//  __riscv_vcreate_v_u16m2x4
//  __riscv_vcreate_v_u16m4x2
//  __riscv_vcreate_v_u8m1x3
//  __riscv_vcreate_v_u8m1x4
//  __riscv_vcreate_v_u8m2x2
//  __riscv_vcreate_v_u8m2x3
//  __riscv_vcreate_v_u8m2x4
//  __riscv_vcreate_v_u8m4x2
#if !defined(LIBYUV_RVV_HAS_TUPLE_TYPE) || \
    (defined(LIBYUV_RVV_HAS_TUPLE_TYPE) && defined(LIBYUV_RVV_HAS_VCREATE))
#define HAS_AB64TOARGBROW_RVV
#define HAS_AR64TOAB64ROW_RVV
#define HAS_ARGBATTENUATEROW_RVV
#define HAS_ARGBBLENDROW_RVV
#define HAS_ARGBTOAB64ROW_RVV
#define HAS_ARGBTOABGRROW_RVV
#define HAS_ARGBTOBGRAROW_RVV
#define HAS_ARGBTORAWROW_RVV
#define HAS_ARGBTORGB24ROW_RVV
#define HAS_ARGBTORGBAROW_RVV
#define HAS_BLENDPLANEROW_RVV
#define HAS_I400TOARGBROW_RVV
#define HAS_I422ALPHATOARGBROW_RVV
#define HAS_I422TOARGBROW_RVV
#define HAS_I422TORGB24ROW_RVV
#define HAS_I422TORGBAROW_RVV
#define HAS_I444ALPHATOARGBROW_RVV
#define HAS_I444TOARGBROW_RVV
#define HAS_I444TORGB24ROW_RVV
#define HAS_J400TOARGBROW_RVV
#define HAS_MERGEARGBROW_RVV
#define HAS_MERGERGBROW_RVV
#define HAS_MERGEUVROW_RVV
#define HAS_MERGEXRGBROW_RVV
#define HAS_NV12TOARGBROW_RVV
#define HAS_NV12TORGB24ROW_RVV
#define HAS_NV21TOARGBROW_RVV
#define HAS_NV21TORGB24ROW_RVV
#define HAS_RAWTOARGBROW_RVV
#define HAS_RAWTORGB24ROW_RVV
#define HAS_RAWTORGBAROW_RVV
#define HAS_RGB24TOARGBROW_RVV
#define HAS_RGBATOARGBROW_RVV
#endif
#endif

#if defined(_MSC_VER) && !defined(__CLR_VER) && !defined(__clang__)
#if defined(VISUALC_HAS_AVX2)
#define SIMD_ALIGNED(var) __declspec(align(32)) var
#else
#define SIMD_ALIGNED(var) __declspec(align(16)) var
#endif
#define LIBYUV_NOINLINE __declspec(noinline)
        typedef __declspec(align(16)) int16_t vec16[8];
        typedef __declspec(align(16)) int32_t vec32[4];
        typedef __declspec(align(16)) float vecf32[4];
        typedef __declspec(align(16)) int8_t vec8[16];
        typedef __declspec(align(16)) uint16_t uvec16[8];
        typedef __declspec(align(16)) uint32_t uvec32[4];
        typedef __declspec(align(16)) uint8_t uvec8[16];
        typedef __declspec(align(32)) int16_t lvec16[16];
        typedef __declspec(align(32)) int32_t lvec32[8];
        typedef __declspec(align(32)) int8_t lvec8[32];
        typedef __declspec(align(32)) uint16_t ulvec16[16];
        typedef __declspec(align(32)) uint32_t ulvec32[8];
        typedef __declspec(align(32)) uint8_t ulvec8[32];
#elif !defined(__pnacl__) && (defined(__GNUC__) || defined(__clang__))
// Caveat GCC 4.2 to 4.7 have a known issue using vectors with const.
#if defined(CLANG_HAS_AVX2) || defined(GCC_HAS_AVX2)
#define SIMD_ALIGNED(var) var __attribute__((aligned(32)))
#else
#define SIMD_ALIGNED(var) var __attribute__((aligned(16)))
#endif
#define LIBYUV_NOINLINE __attribute__((noinline))
        typedef int16_t __attribute__((vector_size(16))) vec16;
        typedef int32_t __attribute__((vector_size(16))) vec32;
        typedef float __attribute__((vector_size(16))) vecf32;
        typedef int8_t __attribute__((vector_size(16))) vec8;
        typedef uint16_t __attribute__((vector_size(16))) uvec16;
        typedef uint32_t __attribute__((vector_size(16))) uvec32;
        typedef uint8_t __attribute__((vector_size(16))) uvec8;
        typedef int16_t __attribute__((vector_size(32))) lvec16;
        typedef int32_t __attribute__((vector_size(32))) lvec32;
        typedef int8_t __attribute__((vector_size(32))) lvec8;
        typedef uint16_t __attribute__((vector_size(32))) ulvec16;
        typedef uint32_t __attribute__((vector_size(32))) ulvec32;
        typedef uint8_t __attribute__((vector_size(32))) ulvec8;
#else
#define SIMD_ALIGNED(var) var
#define LIBYUV_NOINLINE
        typedef int16_t vec16[8];
        typedef int32_t vec32[4];
        typedef float vecf32[4];
        typedef int8_t vec8[16];
        typedef uint16_t uvec16[8];
        typedef uint32_t uvec32[4];
        typedef uint8_t uvec8[16];
        typedef int16_t lvec16[16];
        typedef int32_t lvec32[8];
        typedef int8_t lvec8[32];
        typedef uint16_t ulvec16[16];
        typedef uint32_t ulvec32[8];
        typedef uint8_t ulvec8[32];
#endif

#if defined(__aarch64__) || defined(__arm__) || defined(__riscv)
        // This struct is for ARM and RISC-V color conversion.
        struct YuvConstants {
            uvec8 kUVCoeff;
            vec16 kRGBCoeffBias;
        };
#else
        // This struct is for Intel color conversion.
        struct YuvConstants {
            uint8_t kUVToB[32];
            uint8_t kUVToG[32];
            uint8_t kUVToR[32];
            int16_t kYToRgb[16];
            int16_t kYBiasToRgb[16];
        };

        // Offsets into YuvConstants structure
#define KUVTOB 0
#define KUVTOG 32
#define KUVTOR 64
#define KYTORGB 96
#define KYBIASTORGB 128

#endif

#define IS_ALIGNED(p, a) (!((uintptr_t)(p) & ((a)-1)))

#define align_buffer_64(var, size)                                         \
  void* var##_mem = malloc((size) + 63);                      /* NOLINT */ \
  uint8_t* var = (uint8_t*)(((intptr_t)var##_mem + 63) & ~63) /* NOLINT */

#define free_aligned_buffer_64(var) \
  free(var##_mem);                  \
  var = NULL

#if defined(__APPLE__) || defined(__x86_64__) || defined(__llvm__)
#define OMITFP
#else
#define OMITFP __attribute__((optimize("omit-frame-pointer")))
#endif

// NaCL macros for GCC x86 and x64.
#if defined(__native_client__)
#define LABELALIGN ".p2align 5\n"
#else
#define LABELALIGN
#endif

// Intel Code Analizer markers.  Insert IACA_START IACA_END around code to be
// measured and then run with iaca -64 libyuv_unittest.
// IACA_ASM_START amd IACA_ASM_END are equivalents that can be used within
// inline assembly blocks.
// example of iaca:
// ~/iaca-lin64/bin/iaca.sh -64 -analysis LATENCY out/Release/libyuv_unittest

#if defined(__x86_64__) || defined(__i386__)

#define IACA_ASM_START  \
  ".byte 0x0F, 0x0B\n"  \
  " movl $111, %%ebx\n" \
  ".byte 0x64, 0x67, 0x90\n"

#define IACA_ASM_END         \
  " movl $222, %%ebx\n"      \
  ".byte 0x64, 0x67, 0x90\n" \
  ".byte 0x0F, 0x0B\n"

#define IACA_SSC_MARK(MARK_ID)                        \
  __asm__ __volatile__("\n\t  movl $" #MARK_ID        \
                       ", %%ebx"                      \
                       "\n\t  .byte 0x64, 0x67, 0x90" \
                       :                              \
                       :                              \
                       : "memory");

#define IACA_UD_BYTES __asm__ __volatile__("\n\t .byte 0x0F, 0x0B");

#else /* Visual C */
#define IACA_UD_BYTES \
  { __asm _emit 0x0F __asm _emit 0x0B }

#define IACA_SSC_MARK(x) \
  { __asm mov ebx, x __asm _emit 0x64 __asm _emit 0x67 __asm _emit 0x90 }

#define IACA_VC64_START __writegsbyte(111, 111);
#define IACA_VC64_END __writegsbyte(222, 222);
#endif

#define IACA_START     \
  {                    \
    IACA_UD_BYTES      \
    IACA_SSC_MARK(111) \
  }
#define IACA_END       \
  {                    \
    IACA_SSC_MARK(222) \
    IACA_UD_BYTES      \
  }

        void I210AlphaToARGBRow_NEON(const uint16_t* src_y,
            const uint16_t* src_u,
            const uint16_t* src_v,
            const uint16_t* src_a,
            uint8_t* rgb_buf,
            const struct YuvConstants* yuvconstants,
            int width);
        void I210AlphaToARGBRow_SVE2(const uint16_t* src_y,
            const uint16_t* src_u,
            const uint16_t* src_v,
            const uint16_t* src_a,
            uint8_t* rgb_buf,
            const struct YuvConstants* yuvconstants,
            int width);
        void I210AlphaToARGBRow_SME(const uint16_t* src_y,
            const uint16_t* src_u,
            const uint16_t* src_v,
            const uint16_t* src_a,
            uint8_t* rgb_buf,
            const struct YuvConstants* yuvconstants,
            int width);
        void I410AlphaToARGBRow_NEON(const uint16_t* src_y,
            const uint16_t* src_u,
            const uint16_t* src_v,
            const uint16_t* src_a,
            uint8_t* rgb_buf,
            const struct YuvConstants* yuvconstants,
            int width);
        void I410AlphaToARGBRow_SVE2(const uint16_t* src_y,
            const uint16_t* src_u,
            const uint16_t* src_v,
            const uint16_t* src_a,
            uint8_t* rgb_buf,
            const struct YuvConstants* yuvconstants,
            int width);
        void I410AlphaToARGBRow_SME(const uint16_t* src_y,
            const uint16_t* src_u,
            const uint16_t* src_v,
            const uint16_t* src_a,
            uint8_t* rgb_buf,
            const struct YuvConstants* yuvconstants,
            int width);
        void I444ToARGBRow_NEON(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void I444ToARGBRow_SVE2(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void I444ToARGBRow_SME(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void I444ToRGB24Row_NEON(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_rgb24,
            const struct YuvConstants* yuvconstants,
            int width);
        void I444ToRGB24Row_SVE2(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_rgb24,
            const struct YuvConstants* yuvconstants,
            int width);
        void I444ToRGB24Row_SME(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_rgb24,
            const struct YuvConstants* yuvconstants,
            int width);
        void I210ToARGBRow_NEON(const uint16_t* src_y,
            const uint16_t* src_u,
            const uint16_t* src_v,
            uint8_t* rgb_buf,
            const struct YuvConstants* yuvconstants,
            int width);
        void I210ToARGBRow_SVE2(const uint16_t* src_y,
            const uint16_t* src_u,
            const uint16_t* src_v,
            uint8_t* rgb_buf,
            const struct YuvConstants* yuvconstants,
            int width);
        void I210ToARGBRow_SME(const uint16_t* src_y,
            const uint16_t* src_u,
            const uint16_t* src_v,
            uint8_t* rgb_buf,
            const struct YuvConstants* yuvconstants,
            int width);
        void I410ToARGBRow_NEON(const uint16_t* src_y,
            const uint16_t* src_u,
            const uint16_t* src_v,
            uint8_t* rgb_buf,
            const struct YuvConstants* yuvconstants,
            int width);
        void I410ToARGBRow_SVE2(const uint16_t* src_y,
            const uint16_t* src_u,
            const uint16_t* src_v,
            uint8_t* rgb_buf,
            const struct YuvConstants* yuvconstants,
            int width);
        void I410ToARGBRow_SME(const uint16_t* src_y,
            const uint16_t* src_u,
            const uint16_t* src_v,
            uint8_t* rgb_buf,
            const struct YuvConstants* yuvconstants,
            int width);
        void I210ToAR30Row_NEON(const uint16_t* src_y,
            const uint16_t* src_u,
            const uint16_t* src_v,
            uint8_t* rgb_buf,
            const struct YuvConstants* yuvconstants,
            int width);
        void I210ToAR30Row_SVE2(const uint16_t* src_y,
            const uint16_t* src_u,
            const uint16_t* src_v,
            uint8_t* rgb_buf,
            const struct YuvConstants* yuvconstants,
            int width);
        void I210ToAR30Row_SME(const uint16_t* src_y,
            const uint16_t* src_u,
            const uint16_t* src_v,
            uint8_t* rgb_buf,
            const struct YuvConstants* yuvconstants,
            int width);
        void I410ToAR30Row_NEON(const uint16_t* src_y,
            const uint16_t* src_u,
            const uint16_t* src_v,
            uint8_t* rgb_buf,
            const struct YuvConstants* yuvconstants,
            int width);
        void I410ToAR30Row_SVE2(const uint16_t* src_y,
            const uint16_t* src_u,
            const uint16_t* src_v,
            uint8_t* rgb_buf,
            const struct YuvConstants* yuvconstants,
            int width);
        void I410ToAR30Row_SME(const uint16_t* src_y,
            const uint16_t* src_u,
            const uint16_t* src_v,
            uint8_t* rgb_buf,
            const struct YuvConstants* yuvconstants,
            int width);
        void I212ToARGBRow_NEON(const uint16_t* src_y,
            const uint16_t* src_u,
            const uint16_t* src_v,
            uint8_t* rgb_buf,
            const struct YuvConstants* yuvconstants,
            int width);
        void I212ToARGBRow_SVE2(const uint16_t* src_y,
            const uint16_t* src_u,
            const uint16_t* src_v,
            uint8_t* rgb_buf,
            const struct YuvConstants* yuvconstants,
            int width);
        void I212ToARGBRow_SME(const uint16_t* src_y,
            const uint16_t* src_u,
            const uint16_t* src_v,
            uint8_t* rgb_buf,
            const struct YuvConstants* yuvconstants,
            int width);
        void I212ToAR30Row_NEON(const uint16_t* src_y,
            const uint16_t* src_u,
            const uint16_t* src_v,
            uint8_t* rgb_buf,
            const struct YuvConstants* yuvconstants,
            int width);
        void I212ToAR30Row_SVE2(const uint16_t* src_y,
            const uint16_t* src_u,
            const uint16_t* src_v,
            uint8_t* rgb_buf,
            const struct YuvConstants* yuvconstants,
            int width);
        void I212ToAR30Row_SME(const uint16_t* src_y,
            const uint16_t* src_u,
            const uint16_t* src_v,
            uint8_t* rgb_buf,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToARGBRow_NEON(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToARGBRow_SVE2(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToARGBRow_SME(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToAR30Row_NEON(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToAR30Row_SVE2(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToAR30Row_SME(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void I444AlphaToARGBRow_NEON(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            const uint8_t* src_a,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void I444AlphaToARGBRow_SVE2(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            const uint8_t* src_a,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void I444AlphaToARGBRow_SME(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            const uint8_t* src_a,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422AlphaToARGBRow_NEON(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            const uint8_t* src_a,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422AlphaToARGBRow_SVE2(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            const uint8_t* src_a,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422AlphaToARGBRow_SME(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            const uint8_t* src_a,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToRGBARow_NEON(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_rgba,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToRGBARow_SVE2(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_rgba,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToRGBARow_SME(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_rgba,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToRGB24Row_NEON(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_rgb24,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToRGB24Row_SVE2(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_rgb24,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToRGB24Row_SME(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_rgb24,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToRGB565Row_NEON(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_rgb565,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToRGB565Row_SVE2(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_rgb565,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToRGB565Row_SME(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_rgb565,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToARGB1555Row_NEON(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_argb1555,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToARGB1555Row_SVE2(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_argb1555,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToARGB1555Row_SME(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_argb1555,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToARGB4444Row_NEON(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_argb4444,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToARGB4444Row_SVE2(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_argb4444,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToARGB4444Row_SME(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_argb4444,
            const struct YuvConstants* yuvconstants,
            int width);
        void NV12ToARGBRow_NEON(const uint8_t* src_y,
            const uint8_t* src_uv,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void NV12ToARGBRow_SVE2(const uint8_t* src_y,
            const uint8_t* src_uv,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void NV12ToARGBRow_SME(const uint8_t* src_y,
            const uint8_t* src_uv,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void NV12ToRGB565Row_NEON(const uint8_t* src_y,
            const uint8_t* src_uv,
            uint8_t* dst_rgb565,
            const struct YuvConstants* yuvconstants,
            int width);
        void NV21ToARGBRow_NEON(const uint8_t* src_y,
            const uint8_t* src_vu,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void NV21ToARGBRow_SVE2(const uint8_t* src_y,
            const uint8_t* src_vu,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void NV21ToARGBRow_SME(const uint8_t* src_y,
            const uint8_t* src_vu,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void NV12ToRGB24Row_NEON(const uint8_t* src_y,
            const uint8_t* src_uv,
            uint8_t* dst_rgb24,
            const struct YuvConstants* yuvconstants,
            int width);
        void NV12ToRGB24Row_SVE2(const uint8_t* src_y,
            const uint8_t* src_uv,
            uint8_t* dst_rgb24,
            const struct YuvConstants* yuvconstants,
            int width);
        void NV12ToRGB24Row_SME(const uint8_t* src_y,
            const uint8_t* src_uv,
            uint8_t* dst_rgb24,
            const struct YuvConstants* yuvconstants,
            int width);
        void NV21ToRGB24Row_NEON(const uint8_t* src_y,
            const uint8_t* src_vu,
            uint8_t* dst_rgb24,
            const struct YuvConstants* yuvconstants,
            int width);
        void NV21ToRGB24Row_SVE2(const uint8_t* src_y,
            const uint8_t* src_vu,
            uint8_t* dst_rgb24,
            const struct YuvConstants* yuvconstants,
            int width);
        void NV21ToRGB24Row_SME(const uint8_t* src_y,
            const uint8_t* src_vu,
            uint8_t* dst_rgb24,
            const struct YuvConstants* yuvconstants,
            int width);
        void NV21ToYUV24Row_NEON(const uint8_t* src_y,
            const uint8_t* src_vu,
            uint8_t* dst_yuv24,
            int width);
        void YUY2ToARGBRow_NEON(const uint8_t* src_yuy2,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void YUY2ToARGBRow_SVE2(const uint8_t* src_yuy2,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void YUY2ToARGBRow_SME(const uint8_t* src_yuy2,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void UYVYToARGBRow_NEON(const uint8_t* src_uyvy,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void UYVYToARGBRow_SVE2(const uint8_t* src_uyvy,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void UYVYToARGBRow_SME(const uint8_t* src_uyvy,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void I444ToARGBRow_RVV(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void I444AlphaToARGBRow_RVV(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            const uint8_t* src_a,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void I444ToRGB24Row_RVV(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_rgb24,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToARGBRow_RVV(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422AlphaToARGBRow_RVV(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            const uint8_t* src_a,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToRGBARow_RVV(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_rgba,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToRGB24Row_RVV(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_rgb24,
            const struct YuvConstants* yuvconstants,
            int width);
        void I444ToARGBRow_LSX(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);

        void I422ToARGBRow_LSX(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToARGBRow_LASX(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToRGBARow_LSX(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToRGBARow_LASX(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422AlphaToARGBRow_LSX(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            const uint8_t* src_a,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422AlphaToARGBRow_LASX(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            const uint8_t* src_a,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToRGB24Row_LSX(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToRGB24Row_LASX(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToRGB565Row_LSX(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_rgb565,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToRGB565Row_LASX(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_rgb565,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToARGB4444Row_LSX(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_argb4444,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToARGB4444Row_LASX(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_argb4444,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToARGB1555Row_LSX(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_argb1555,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToARGB1555Row_LASX(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_argb1555,
            const struct YuvConstants* yuvconstants,
            int width);

        void NV12ToARGBRow_LSX(const uint8_t* src_y,
            const uint8_t* src_uv,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void NV12ToARGBRow_LASX(const uint8_t* src_y,
            const uint8_t* src_uv,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void NV12ToRGB565Row_LSX(const uint8_t* src_y,
            const uint8_t* src_uv,
            uint8_t* dst_rgb565,
            const struct YuvConstants* yuvconstants,
            int width);
        void NV12ToRGB565Row_LASX(const uint8_t* src_y,
            const uint8_t* src_uv,
            uint8_t* dst_rgb565,
            const struct YuvConstants* yuvconstants,
            int width);
        void NV21ToARGBRow_LSX(const uint8_t* src_y,
            const uint8_t* src_vu,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void NV21ToARGBRow_LASX(const uint8_t* src_y,
            const uint8_t* src_vu,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void YUY2ToARGBRow_LSX(const uint8_t* src_yuy2,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void UYVYToARGBRow_LSX(const uint8_t* src_uyvy,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void NV12ToARGBRow_RVV(const uint8_t* src_y,
            const uint8_t* src_uv,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void NV21ToARGBRow_RVV(const uint8_t* src_y,
            const uint8_t* src_vu,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void NV12ToRGB24Row_RVV(const uint8_t* src_y,
            const uint8_t* src_uv,
            uint8_t* dst_rgb24,
            const struct YuvConstants* yuvconstants,
            int width);
        void NV21ToRGB24Row_RVV(const uint8_t* src_y,
            const uint8_t* src_vu,
            uint8_t* dst_rgb24,
            const struct YuvConstants* yuvconstants,
            int width);

        void ARGBToYRow_AVX2(const uint8_t* src_argb, uint8_t* dst_y, int width);
        void ARGBToYRow_Any_AVX2(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void ABGRToYRow_AVX2(const uint8_t* src_abgr, uint8_t* dst_y, int width);
        void ABGRToYRow_Any_AVX2(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void ARGBToYRow_SSSE3(const uint8_t* src_argb, uint8_t* dst_y, int width);
        void ARGBToYJRow_SSSE3(const uint8_t* src_argb, uint8_t* dst_y, int width);
        void ARGBToYJRow_AVX2(const uint8_t* src_argb, uint8_t* dst_y, int width);
        void ARGBToYJRow_Any_AVX2(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void ABGRToYRow_SSSE3(const uint8_t* src_abgr, uint8_t* dst_y, int width);
        void ABGRToYJRow_SSSE3(const uint8_t* src_abgr, uint8_t* dst_y, int width);
        void ABGRToYJRow_AVX2(const uint8_t* src_abgr, uint8_t* dst_y, int width);
        void ABGRToYJRow_Any_AVX2(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void RGBAToYJRow_AVX2(const uint8_t* src_rgba, uint8_t* dst_y, int width);
        void RGBAToYJRow_Any_AVX2(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void RGBAToYJRow_SSSE3(const uint8_t* src_rgba, uint8_t* dst_y, int width);
        void BGRAToYRow_SSSE3(const uint8_t* src_bgra, uint8_t* dst_y, int width);
        void ABGRToYRow_SSSE3(const uint8_t* src_abgr, uint8_t* dst_y, int width);
        void RGBAToYRow_SSSE3(const uint8_t* src_rgba, uint8_t* dst_y, int width);
        void RGB24ToYRow_SSSE3(const uint8_t* src_rgb24, uint8_t* dst_y, int width);
        void RGB24ToYJRow_SSSE3(const uint8_t* src_rgb24, uint8_t* dst_yj, int width);
        void RAWToYRow_SSSE3(const uint8_t* src_raw, uint8_t* dst_y, int width);
        void RAWToYJRow_SSSE3(const uint8_t* src_raw, uint8_t* dst_yj, int width);
        void RGB24ToYJRow_AVX2(const uint8_t* src_rgb24, uint8_t* dst_yj, int width);
        void RAWToYJRow_AVX2(const uint8_t* src_raw, uint8_t* dst_yj, int width);
        void ARGBToYRow_NEON(const uint8_t* src_argb, uint8_t* dst_y, int width);
        void ARGBToYJRow_NEON(const uint8_t* src_argb, uint8_t* dst_yj, int width);
        void ABGRToYJRow_NEON(const uint8_t* src_abgr, uint8_t* dst_yj, int width);
        void RGBAToYJRow_NEON(const uint8_t* src_rgba, uint8_t* dst_yj, int width);
        void ARGBToYRow_NEON_DotProd(const uint8_t* src_argb,
            uint8_t* dst_y,
            int width);
        void ARGBToYJRow_NEON_DotProd(const uint8_t* src_argb,
            uint8_t* dst_yj,
            int width);
        void ABGRToYJRow_NEON_DotProd(const uint8_t* src_abgr,
            uint8_t* dst_yj,
            int width);
        void RGBAToYJRow_NEON_DotProd(const uint8_t* src_rgba,
            uint8_t* dst_yj,
            int width);
        void ARGBToYRow_RVV(const uint8_t* src_argb, uint8_t* dst_y, int width);
        void ARGBToYJRow_RVV(const uint8_t* src_argb, uint8_t* dst_yj, int width);
        void ABGRToYJRow_RVV(const uint8_t* src_rgba, uint8_t* dst_yj, int width);
        void RGBAToYJRow_RVV(const uint8_t* src_rgba, uint8_t* dst_yj, int width);
        void ARGBToYRow_LSX(const uint8_t* src_argb0, uint8_t* dst_y, int width);
        void ARGBToYRow_LASX(const uint8_t* src_argb0, uint8_t* dst_y, int width);
        void ARGBToYJRow_LSX(const uint8_t* src_argb0, uint8_t* dst_y, int width);
        void ABGRToYJRow_LSX(const uint8_t* src_abgr, uint8_t* dst_yj, int width);
        void RGBAToYJRow_LSX(const uint8_t* src_rgba, uint8_t* dst_yj, int width);
        void ARGBToYJRow_LASX(const uint8_t* src_argb0, uint8_t* dst_y, int width);
        void ABGRToYJRow_LASX(const uint8_t* src_abgr, uint8_t* dst_yj, int width);
        void RGBAToYJRow_LASX(const uint8_t* src_rgba, uint8_t* dst_yj, int width);
        void ARGBToUV444Row_NEON(const uint8_t* src_argb,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ARGBToUV444Row_NEON_I8MM(const uint8_t* src_argb,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ARGBToUVJ444Row_NEON(const uint8_t* src_argb,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ARGBToUVJ444Row_NEON_I8MM(const uint8_t* src_argb,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ARGBToUVRow_NEON(const uint8_t* src_argb,
            int src_stride_argb,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ARGBToUVRow_NEON_I8MM(const uint8_t* src_argb,
            int src_stride_argb,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ARGBToUVRow_SVE2(const uint8_t* src_argb,
            int src_stride_argb,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ARGBToUVRow_SME(const uint8_t* src_argb,
            int src_stride_argb,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ARGBToUVRow_LSX(const uint8_t* src_argb,
            int src_stride_argb,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ARGBToUVRow_LASX(const uint8_t* src_argb,
            int src_stride_argb,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ARGBToUV444Row_LSX(const uint8_t* src_argb,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ARGBToUV444Row_LASX(const uint8_t* src_argb,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ARGBToUVJRow_NEON(const uint8_t* src_argb,
            int src_stride_argb,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ARGBToUVJRow_NEON_I8MM(const uint8_t* src_argb,
            int src_stride_argb,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ARGBToUVJRow_SVE2(const uint8_t* src_argb,
            int src_stride_argb,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ARGBToUVJRow_SME(const uint8_t* src_argb,
            int src_stride_argb,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ABGRToUVJRow_NEON(const uint8_t* src_abgr,
            int src_stride_abgr,
            uint8_t* dst_uj,
            uint8_t* dst_vj,
            int width);
        void ABGRToUVJRow_NEON_I8MM(const uint8_t* src_abgr,
            int src_stride_abgr,
            uint8_t* dst_uj,
            uint8_t* dst_vj,
            int width);
        void ABGRToUVJRow_SVE2(const uint8_t* src_abgr,
            int src_stride_abgr,
            uint8_t* dst_uj,
            uint8_t* dst_vj,
            int width);
        void ABGRToUVJRow_SME(const uint8_t* src_abgr,
            int src_stride_abgr,
            uint8_t* dst_uj,
            uint8_t* dst_vj,
            int width);
        void BGRAToUVRow_NEON(const uint8_t* src_bgra,
            int src_stride_bgra,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void BGRAToUVRow_NEON_I8MM(const uint8_t* src_bgra,
            int src_stride_bgra,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void BGRAToUVRow_SVE2(const uint8_t* src_bgra,
            int src_stride_bgra,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void BGRAToUVRow_SME(const uint8_t* src_bgra,
            int src_stride_bgra,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ABGRToUVRow_NEON(const uint8_t* src_abgr,
            int src_stride_abgr,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ABGRToUVRow_NEON_I8MM(const uint8_t* src_abgr,
            int src_stride_abgr,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ABGRToUVRow_SVE2(const uint8_t* src_abgr,
            int src_stride_abgr,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ABGRToUVRow_SME(const uint8_t* src_abgr,
            int src_stride_abgr,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void RGBAToUVRow_NEON(const uint8_t* src_rgba,
            int src_stride_rgba,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void RGBAToUVRow_NEON_I8MM(const uint8_t* src_rgba,
            int src_stride_rgba,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void RGBAToUVRow_SVE2(const uint8_t* src_rgba,
            int src_stride_rgba,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void RGBAToUVRow_SME(const uint8_t* src_rgba,
            int src_stride_rgba,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void RGB24ToUVRow_NEON(const uint8_t* src_rgb24,
            int src_stride_rgb24,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void RAWToUVRow_NEON(const uint8_t* src_raw,
            int src_stride_raw,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void RGB24ToUVJRow_NEON(const uint8_t* src_rgb24,
            int src_stride_rgb24,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void RAWToUVJRow_NEON(const uint8_t* src_raw,
            int src_stride_raw,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void RGB565ToUVRow_NEON(const uint8_t* src_rgb565,
            int src_stride_rgb565,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ARGB1555ToUVRow_NEON(const uint8_t* src_argb1555,
            int src_stride_argb1555,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ARGB4444ToUVRow_NEON(const uint8_t* src_argb4444,
            int src_stride_argb4444,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void BGRAToUVRow_LSX(const uint8_t* src_bgra,
            int src_stride_bgra,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ABGRToUVRow_LSX(const uint8_t* src_abgr,
            int src_stride_abgr,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void RGBAToUVRow_LSX(const uint8_t* src_rgba,
            int src_stride_rgba,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ARGBToUVJRow_LSX(const uint8_t* src_argb,
            int src_stride_argb,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ARGBToUVJRow_LASX(const uint8_t* src_argb,
            int src_stride_argb,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ARGB1555ToUVRow_LSX(const uint8_t* src_argb1555,
            int src_stride_argb1555,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ARGB1555ToUVRow_LASX(const uint8_t* src_argb1555,
            int src_stride_argb1555,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void RGB565ToUVRow_LSX(const uint8_t* src_rgb565,
            int src_stride_rgb565,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void RGB565ToUVRow_LASX(const uint8_t* src_rgb565,
            int src_stride_rgb565,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void RGB24ToUVRow_LSX(const uint8_t* src_rgb24,
            int src_stride_rgb24,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void RGB24ToUVRow_LASX(const uint8_t* src_rgb24,
            int src_stride_rgb24,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void RAWToUVRow_LSX(const uint8_t* src_raw,
            int src_stride_raw,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void RAWToUVRow_LASX(const uint8_t* src_raw,
            int src_stride_raw,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void BGRAToYRow_NEON(const uint8_t* src_bgra, uint8_t* dst_y, int width);
        void ABGRToYRow_NEON(const uint8_t* src_abgr, uint8_t* dst_y, int width);
        void RGBAToYRow_NEON(const uint8_t* src_rgba, uint8_t* dst_y, int width);
        void BGRAToYRow_NEON_DotProd(const uint8_t* src_bgra,
            uint8_t* dst_y,
            int width);
        void ABGRToYRow_NEON_DotProd(const uint8_t* src_abgr,
            uint8_t* dst_y,
            int width);
        void RGBAToYRow_NEON_DotProd(const uint8_t* src_rgba,
            uint8_t* dst_y,
            int width);
        void RGB24ToYRow_NEON(const uint8_t* src_rgb24, uint8_t* dst_y, int width);
        void RGB24ToYJRow_NEON(const uint8_t* src_rgb24, uint8_t* dst_yj, int width);
        void RAWToYRow_NEON(const uint8_t* src_raw, uint8_t* dst_y, int width);
        void RAWToYJRow_NEON(const uint8_t* src_raw, uint8_t* dst_yj, int width);
        void RGB565ToYRow_NEON(const uint8_t* src_rgb565, uint8_t* dst_y, int width);
        void ARGB1555ToYRow_NEON(const uint8_t* src_argb1555,
            uint8_t* dst_y,
            int width);
        void ARGB4444ToYRow_NEON(const uint8_t* src_argb4444,
            uint8_t* dst_y,
            int width);
        void BGRAToYRow_RVV(const uint8_t* src_bgra, uint8_t* dst_y, int width);
        void ABGRToYRow_RVV(const uint8_t* src_abgr, uint8_t* dst_y, int width);
        void RGBAToYRow_RVV(const uint8_t* src_rgba, uint8_t* dst_y, int width);
        void RGB24ToYRow_RVV(const uint8_t* src_rgb24, uint8_t* dst_y, int width);
        void RGB24ToYJRow_RVV(const uint8_t* src_rgb24, uint8_t* dst_yj, int width);
        void RAWToYRow_RVV(const uint8_t* src_raw, uint8_t* dst_y, int width);
        void RAWToYJRow_RVV(const uint8_t* src_raw, uint8_t* dst_yj, int width);

        void BGRAToYRow_LSX(const uint8_t* src_bgra, uint8_t* dst_y, int width);
        void ABGRToYRow_LSX(const uint8_t* src_abgr, uint8_t* dst_y, int width);
        void RGBAToYRow_LSX(const uint8_t* src_rgba, uint8_t* dst_y, int width);
        void ARGB1555ToYRow_LSX(const uint8_t* src_argb1555, uint8_t* dst_y, int width);
        void RGB24ToYJRow_LSX(const uint8_t* src_rgb24, uint8_t* dst_yj, int width);
        void ABGRToYRow_LASX(const uint8_t* src_abgr, uint8_t* dst_y, int width);
        void ARGB1555ToYRow_LASX(const uint8_t* src_argb1555,
            uint8_t* dst_y,
            int width);
        void RGB565ToYRow_LSX(const uint8_t* src_rgb565, uint8_t* dst_y, int width);
        void RGB565ToYRow_LASX(const uint8_t* src_rgb565, uint8_t* dst_y, int width);
        void RGB24ToYRow_LSX(const uint8_t* src_rgb24, uint8_t* dst_y, int width);
        void RGB24ToYRow_LASX(const uint8_t* src_rgb24, uint8_t* dst_y, int width);
        void RAWToYRow_LSX(const uint8_t* src_raw, uint8_t* dst_y, int width);
        void RAWToYRow_LASX(const uint8_t* src_raw, uint8_t* dst_y, int width);
        void RGBAToYRow_LASX(const uint8_t* src_rgba, uint8_t* dst_y, int width);
        void BGRAToYRow_LASX(const uint8_t* src_bgra, uint8_t* dst_y, int width);
        void RGB24ToYJRow_LASX(const uint8_t* src_rgb24, uint8_t* dst_yj, int width);
        void RAWToYJRow_LSX(const uint8_t* src_raw, uint8_t* dst_yj, int width);
        void RAWToYJRow_LASX(const uint8_t* src_raw, uint8_t* dst_yj, int width);

        void ARGBToYRow_C(const uint8_t* src_rgb, uint8_t* dst_y, int width);
        void ARGBToYJRow_C(const uint8_t* src_rgb, uint8_t* dst_y, int width);
        void ABGRToYJRow_C(const uint8_t* src_rgb, uint8_t* dst_y, int width);
        void RGBAToYJRow_C(const uint8_t* src_rgb, uint8_t* dst_y, int width);
        void BGRAToYRow_C(const uint8_t* src_rgb, uint8_t* dst_y, int width);
        void ABGRToYRow_C(const uint8_t* src_rgb, uint8_t* dst_y, int width);
        void RGBAToYRow_C(const uint8_t* src_rgb, uint8_t* dst_y, int width);
        void RGB24ToYRow_C(const uint8_t* src_rgb, uint8_t* dst_y, int width);
        void RGB24ToYJRow_C(const uint8_t* src_rgb, uint8_t* dst_y, int width);
        void RAWToYRow_C(const uint8_t* src_rgb, uint8_t* dst_y, int width);
        void RAWToYJRow_C(const uint8_t* src_rgb, uint8_t* dst_y, int width);
        void RGB565ToYRow_C(const uint8_t* src_rgb565, uint8_t* dst_y, int width);
        void ARGB1555ToYRow_C(const uint8_t* src_argb1555, uint8_t* dst_y, int width);
        void ARGB4444ToYRow_C(const uint8_t* src_argb4444, uint8_t* dst_y, int width);
        void ARGBToYRow_Any_SSSE3(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void ARGBToYJRow_Any_SSSE3(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void ABGRToYJRow_Any_SSSE3(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void RGBAToYJRow_Any_SSSE3(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void BGRAToYRow_Any_SSSE3(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void ABGRToYRow_Any_SSSE3(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void RGBAToYRow_Any_SSSE3(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void RGB24ToYRow_Any_SSSE3(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void RGB24ToYJRow_Any_SSSE3(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void RAWToYRow_Any_SSSE3(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void RAWToYJRow_Any_SSSE3(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void RGB24ToYJRow_Any_AVX2(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void RAWToYJRow_Any_AVX2(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void ARGBToYRow_Any_NEON(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void ARGBToYRow_Any_NEON_DotProd(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void ARGBToYJRow_Any_NEON(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void ARGBToYJRow_Any_NEON_DotProd(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void ABGRToYJRow_Any_NEON(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void ABGRToYJRow_Any_NEON_DotProd(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void RGBAToYJRow_Any_NEON(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void RGBAToYJRow_Any_NEON_DotProd(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void BGRAToYRow_Any_NEON(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void BGRAToYRow_Any_NEON_DotProd(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void ABGRToYRow_Any_NEON(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void ABGRToYRow_Any_NEON_DotProd(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void RGBAToYRow_Any_NEON(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void RGBAToYRow_Any_NEON_DotProd(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void RGB24ToYRow_Any_NEON(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void RGB24ToYJRow_Any_NEON(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void RAWToYRow_Any_NEON(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void RAWToYJRow_Any_NEON(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void RGB565ToYRow_Any_NEON(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void ARGB1555ToYRow_Any_NEON(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void ARGB4444ToYRow_Any_NEON(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);

        void BGRAToYRow_Any_LSX(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void ABGRToYRow_Any_LSX(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void RGBAToYRow_Any_LSX(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void ARGBToYRow_Any_LSX(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void ARGBToYJRow_Any_LSX(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void RGB24ToYRow_Any_LSX(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void RGB565ToYRow_Any_LSX(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void ABGRToYJRow_Any_LSX(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void RAWToYRow_Any_LSX(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void RGBAToYJRow_Any_LSX(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void RGB24ToYJRow_Any_LSX(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void RAWToYJRow_Any_LSX(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void ARGB1555ToYRow_Any_LSX(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);

        void RGB565ToYRow_Any_LASX(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void RGB24ToYRow_Any_LASX(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void ARGBToYJRow_Any_LASX(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void ARGBToYRow_Any_LASX(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void ABGRToYRow_Any_LASX(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void ABGRToYJRow_Any_LASX(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void RAWToYRow_Any_LASX(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void RGBAToYRow_Any_LASX(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void RGBAToYJRow_Any_LASX(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void BGRAToYRow_Any_LASX(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void RGB24ToYJRow_Any_LASX(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void RAWToYJRow_Any_LASX(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void ARGB1555ToYRow_Any_LASX(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);

        void ARGBToUVRow_AVX2(const uint8_t* src_argb,
            int src_stride_argb,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ABGRToUVRow_AVX2(const uint8_t* src_abgr,
            int src_stride_abgr,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ARGBToUVJRow_AVX2(const uint8_t* src_argb,
            int src_stride_argb,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ABGRToUVJRow_AVX2(const uint8_t* src_abgr,
            int src_stride_abgr,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ARGBToUVRow_SSSE3(const uint8_t* src_argb,
            int src_stride_argb,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ARGBToUVJRow_SSSE3(const uint8_t* src_argb,
            int src_stride_argb,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ABGRToUVJRow_SSSE3(const uint8_t* src_abgr,
            int src_stride_abgr,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void BGRAToUVRow_SSSE3(const uint8_t* src_bgra,
            int src_stride_bgra,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ABGRToUVRow_SSSE3(const uint8_t* src_abgr,
            int src_stride_abgr,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void RGBAToUVRow_SSSE3(const uint8_t* src_rgba,
            int src_stride_rgba,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ARGBToUVRow_Any_AVX2(const uint8_t* src_ptr,
            int src_stride,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ABGRToUVRow_Any_AVX2(const uint8_t* src_ptr,
            int src_stride,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ARGBToUVJRow_Any_AVX2(const uint8_t* src_ptr,
            int src_stride,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ABGRToUVJRow_Any_AVX2(const uint8_t* src_ptr,
            int src_stride,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ARGBToUVRow_Any_SSSE3(const uint8_t* src_ptr,
            int src_stride,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ARGBToUVJRow_Any_SSSE3(const uint8_t* src_ptr,
            int src_stride,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ABGRToUVJRow_Any_SSSE3(const uint8_t* src_ptr,
            int src_stride,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void BGRAToUVRow_Any_SSSE3(const uint8_t* src_ptr,
            int src_stride,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ABGRToUVRow_Any_SSSE3(const uint8_t* src_ptr,
            int src_stride,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void RGBAToUVRow_Any_SSSE3(const uint8_t* src_ptr,
            int src_stride,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ARGBToUV444Row_Any_NEON(const uint8_t* src_ptr,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ARGBToUV444Row_Any_NEON_I8MM(const uint8_t* src_ptr,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ARGBToUVJ444Row_Any_NEON(const uint8_t* src_ptr,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ARGBToUVJ444Row_Any_NEON_I8MM(const uint8_t* src_ptr,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ARGBToUVRow_Any_NEON(const uint8_t* src_ptr,
            int src_stride,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ARGBToUVRow_Any_NEON_I8MM(const uint8_t* src_ptr,
            int src_stride,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ARGBToUVRow_Any_SVE2(const uint8_t* src_ptr,
            int src_stride,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ARGBToUVRow_Any_SME(const uint8_t* src_ptr,
            int src_stride,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ARGBToUVRow_Any_LSX(const uint8_t* src_ptr,
            int src_stride_ptr,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ARGBToUVRow_Any_LASX(const uint8_t* src_ptr,
            int src_stride_ptr,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ARGBToUV444Row_Any_LSX(const uint8_t* src_ptr,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ARGBToUV444Row_Any_LASX(const uint8_t* src_ptr,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ARGBToUVJRow_Any_NEON(const uint8_t* src_ptr,
            int src_stride,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ARGBToUVJRow_Any_NEON_I8MM(const uint8_t* src_ptr,
            int src_stride,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ARGBToUVJRow_Any_SVE2(const uint8_t* src_ptr,
            int src_stride,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ARGBToUVJRow_Any_SME(const uint8_t* src_ptr,
            int src_stride,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ABGRToUVJRow_Any_NEON(const uint8_t* src_ptr,
            int src_stride,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ABGRToUVJRow_Any_NEON_I8MM(const uint8_t* src_ptr,
            int src_stride,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ABGRToUVJRow_Any_SVE2(const uint8_t* src_ptr,
            int src_stride,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ABGRToUVJRow_Any_SME(const uint8_t* src_ptr,
            int src_stride,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void BGRAToUVRow_Any_NEON(const uint8_t* src_ptr,
            int src_stride,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void BGRAToUVRow_Any_NEON_I8MM(const uint8_t* src_ptr,
            int src_stride,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void BGRAToUVRow_Any_SVE2(const uint8_t* src_ptr,
            int src_stride,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void BGRAToUVRow_Any_SME(const uint8_t* src_ptr,
            int src_stride,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ABGRToUVRow_Any_NEON(const uint8_t* src_ptr,
            int src_stride,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ABGRToUVRow_Any_NEON_I8MM(const uint8_t* src_ptr,
            int src_stride,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ABGRToUVRow_Any_SVE2(const uint8_t* src_ptr,
            int src_stride,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ABGRToUVRow_Any_SME(const uint8_t* src_ptr,
            int src_stride,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void RGBAToUVRow_Any_NEON(const uint8_t* src_ptr,
            int src_stride,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void RGBAToUVRow_Any_NEON_I8MM(const uint8_t* src_ptr,
            int src_stride,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void RGBAToUVRow_Any_SVE2(const uint8_t* src_ptr,
            int src_stride,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void RGBAToUVRow_Any_SME(const uint8_t* src_ptr,
            int src_stride,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void RGB24ToUVRow_Any_NEON(const uint8_t* src_ptr,
            int src_stride,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void RAWToUVRow_Any_NEON(const uint8_t* src_ptr,
            int src_stride,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void RGB24ToUVJRow_Any_NEON(const uint8_t* src_ptr,
            int src_stride,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void RAWToUVJRow_Any_NEON(const uint8_t* src_ptr,
            int src_stride,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void RGB565ToUVRow_Any_NEON(const uint8_t* src_ptr,
            int src_stride,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ARGB1555ToUVRow_Any_NEON(const uint8_t* src_ptr,
            int src_stride,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ARGB4444ToUVRow_Any_NEON(const uint8_t* src_ptr,
            int src_stride,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ABGRToUVRow_Any_LSX(const uint8_t* src_ptr,
            int src_stride_ptr,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void BGRAToUVRow_Any_LSX(const uint8_t* src_ptr,
            int src_stride_ptr,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void RGBAToUVRow_Any_LSX(const uint8_t* src_ptr,
            int src_stride_ptr,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ARGBToUVJRow_Any_LSX(const uint8_t* src_ptr,
            int src_stride_ptr,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ARGBToUVJRow_Any_LASX(const uint8_t* src_ptr,
            int src_stride_ptr,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ARGB1555ToUVRow_Any_LSX(const uint8_t* src_ptr,
            int src_stride_ptr,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ARGB1555ToUVRow_Any_LASX(const uint8_t* src_ptr,
            int src_stride_ptr,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void RGB565ToUVRow_Any_LSX(const uint8_t* src_ptr,
            int src_stride_ptr,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void RGB565ToUVRow_Any_LASX(const uint8_t* src_ptr,
            int src_stride_ptr,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void RGB24ToUVRow_Any_LSX(const uint8_t* src_ptr,
            int src_stride_ptr,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void RGB24ToUVRow_Any_LASX(const uint8_t* src_ptr,
            int src_stride_ptr,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void RAWToUVRow_Any_LSX(const uint8_t* src_ptr,
            int src_stride_ptr,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void RAWToUVRow_Any_LASX(const uint8_t* src_ptr,
            int src_stride_ptr,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ARGBToUVRow_C(const uint8_t* src_rgb,
            int src_stride_rgb,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ARGBToUVJRow_C(const uint8_t* src_rgb,
            int src_stride_rgb,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ABGRToUVJRow_C(const uint8_t* src_rgb,
            int src_stride_rgb,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ARGBToUVRow_C(const uint8_t* src_rgb,
            int src_stride_rgb,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void BGRAToUVRow_C(const uint8_t* src_rgb,
            int src_stride_rgb,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ABGRToUVRow_C(const uint8_t* src_rgb,
            int src_stride_rgb,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void RGBAToUVRow_C(const uint8_t* src_rgb,
            int src_stride_rgb,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void RGBAToUVJRow_C(const uint8_t* src_rgb,
            int src_stride_rgb,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void RGB24ToUVRow_C(const uint8_t* src_rgb,
            int src_stride_rgb,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void RAWToUVRow_C(const uint8_t* src_rgb,
            int src_stride_rgb,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void RGB24ToUVJRow_C(const uint8_t* src_rgb,
            int src_stride_rgb,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void RAWToUVJRow_C(const uint8_t* src_rgb,
            int src_stride_rgb,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void RGB565ToUVRow_C(const uint8_t* src_rgb565,
            int src_stride_rgb565,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ARGB1555ToUVRow_C(const uint8_t* src_argb1555,
            int src_stride_argb1555,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ARGB4444ToUVRow_C(const uint8_t* src_argb4444,
            int src_stride_argb4444,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);

        void ARGBToUV444Row_SSSE3(const uint8_t* src_argb,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ARGBToUV444Row_Any_SSSE3(const uint8_t* src_ptr,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);

        void ARGBToUVJ444Row_SSSE3(const uint8_t* src_argb,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ARGBToUVJ444Row_Any_SSSE3(const uint8_t* src_ptr,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);

        void ARGBToUV444Row_AVX2(const uint8_t* src_argb,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ARGBToUV444Row_Any_AVX2(const uint8_t* src_ptr,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);

        void ARGBToUVJ444Row_AVX2(const uint8_t* src_argb,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void ARGBToUVJ444Row_Any_AVX2(const uint8_t* src_ptr,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);

        void ARGBToUV444Row_C(const uint8_t* src_argb,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);

        void ARGBToUVJ444Row_C(const uint8_t* src_argb,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);

        void MirrorRow_AVX2(const uint8_t* src, uint8_t* dst, int width);
        void MirrorRow_SSSE3(const uint8_t* src, uint8_t* dst, int width);
        void MirrorRow_NEON(const uint8_t* src, uint8_t* dst, int width);
        void MirrorRow_LSX(const uint8_t* src, uint8_t* dst, int width);
        void MirrorRow_LASX(const uint8_t* src, uint8_t* dst, int width);
        void MirrorRow_C(const uint8_t* src, uint8_t* dst, int width);
        void MirrorRow_Any_AVX2(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void MirrorRow_Any_SSSE3(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void MirrorRow_Any_SSE2(const uint8_t* src, uint8_t* dst, int width);
        void MirrorRow_Any_NEON(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void MirrorRow_Any_LSX(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void MirrorRow_Any_LASX(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void MirrorUVRow_AVX2(const uint8_t* src_uv, uint8_t* dst_uv, int width);
        void MirrorUVRow_SSSE3(const uint8_t* src_uv, uint8_t* dst_uv, int width);
        void MirrorUVRow_NEON(const uint8_t* src_uv, uint8_t* dst_uv, int width);
        void MirrorUVRow_LSX(const uint8_t* src_uv, uint8_t* dst_uv, int width);
        void MirrorUVRow_LASX(const uint8_t* src_uv, uint8_t* dst_uv, int width);
        void MirrorUVRow_C(const uint8_t* src_uv, uint8_t* dst_uv, int width);
        void MirrorUVRow_Any_AVX2(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void MirrorUVRow_Any_SSSE3(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void MirrorUVRow_Any_NEON(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void MirrorUVRow_Any_LSX(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void MirrorUVRow_Any_LASX(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);

        void MirrorSplitUVRow_SSSE3(const uint8_t* src,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void MirrorSplitUVRow_NEON(const uint8_t* src_uv,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void MirrorSplitUVRow_LSX(const uint8_t* src_uv,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void MirrorSplitUVRow_C(const uint8_t* src_uv,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);

        void MirrorRow_16_C(const uint16_t* src, uint16_t* dst, int width);

        void ARGBMirrorRow_AVX2(const uint8_t* src, uint8_t* dst, int width);
        void ARGBMirrorRow_SSE2(const uint8_t* src, uint8_t* dst, int width);
        void ARGBMirrorRow_NEON(const uint8_t* src_argb, uint8_t* dst_argb, int width);
        void ARGBMirrorRow_LSX(const uint8_t* src, uint8_t* dst, int width);
        void ARGBMirrorRow_LASX(const uint8_t* src, uint8_t* dst, int width);
        void ARGBMirrorRow_C(const uint8_t* src, uint8_t* dst, int width);
        void ARGBMirrorRow_Any_AVX2(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void ARGBMirrorRow_Any_SSE2(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void ARGBMirrorRow_Any_NEON(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void ARGBMirrorRow_Any_LSX(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void ARGBMirrorRow_Any_LASX(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);

        void RGB24MirrorRow_SSSE3(const uint8_t* src_rgb24,
            uint8_t* dst_rgb24,
            int width);
        void RGB24MirrorRow_NEON(const uint8_t* src_rgb24,
            uint8_t* dst_rgb24,
            int width);
        void RGB24MirrorRow_C(const uint8_t* src_rgb24, uint8_t* dst_rgb24, int width);
        void RGB24MirrorRow_Any_SSSE3(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void RGB24MirrorRow_Any_NEON(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);

        void SplitUVRow_C(const uint8_t* src_uv,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void SplitUVRow_SSE2(const uint8_t* src_uv,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void SplitUVRow_AVX2(const uint8_t* src_uv,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void SplitUVRow_NEON(const uint8_t* src_uv,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void SplitUVRow_LSX(const uint8_t* src_uv,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void SplitUVRow_RVV(const uint8_t* src_uv,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void SplitUVRow_Any_SSE2(const uint8_t* src_ptr,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void SplitUVRow_Any_AVX2(const uint8_t* src_ptr,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void SplitUVRow_Any_NEON(const uint8_t* src_ptr,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void SplitUVRow_Any_LSX(const uint8_t* src_ptr,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void DetileRow_C(const uint8_t* src,
            ptrdiff_t src_tile_stride,
            uint8_t* dst,
            int width);
        void DetileRow_NEON(const uint8_t* src,
            ptrdiff_t src_tile_stride,
            uint8_t* dst,
            int width);
        void DetileRow_Any_NEON(const uint8_t* src,
            ptrdiff_t src_tile_stride,
            uint8_t* dst,
            int width);
        void DetileRow_SSE2(const uint8_t* src,
            ptrdiff_t src_tile_stride,
            uint8_t* dst,
            int width);
        void DetileRow_Any_SSE2(const uint8_t* src,
            ptrdiff_t src_tile_stride,
            uint8_t* dst,
            int width);
        void DetileRow_AVX(const uint8_t* src,
            ptrdiff_t src_tile_stride,
            uint8_t* dst,
            int width);
        void DetileRow_Any_AVX(const uint8_t* src,
            ptrdiff_t src_tile_stride,
            uint8_t* dst,
            int width);
        void DetileRow_16_C(const uint16_t* src,
            ptrdiff_t src_tile_stride,
            uint16_t* dst,
            int width);
        void DetileRow_16_NEON(const uint16_t* src,
            ptrdiff_t src_tile_stride,
            uint16_t* dst,
            int width);
        void DetileRow_16_Any_NEON(const uint16_t* src,
            ptrdiff_t src_tile_stride,
            uint16_t* dst,
            int width);
        void DetileRow_16_SSE2(const uint16_t* src,
            ptrdiff_t src_tile_stride,
            uint16_t* dst,
            int width);
        void DetileRow_16_Any_SSE2(const uint16_t* src,
            ptrdiff_t src_tile_stride,
            uint16_t* dst,
            int width);
        void DetileRow_16_AVX(const uint16_t* src,
            ptrdiff_t src_tile_stride,
            uint16_t* dst,
            int width);
        void DetileRow_16_Any_AVX(const uint16_t* src,
            ptrdiff_t src_tile_stride,
            uint16_t* dst,
            int width);
        void DetileSplitUVRow_C(const uint8_t* src_uv,
            ptrdiff_t src_tile_stride,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void DetileSplitUVRow_SSSE3(const uint8_t* src_uv,
            ptrdiff_t src_tile_stride,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void DetileSplitUVRow_Any_SSSE3(const uint8_t* src_uv,
            ptrdiff_t src_tile_stride,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void DetileSplitUVRow_NEON(const uint8_t* src_uv,
            ptrdiff_t src_tile_stride,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void DetileSplitUVRow_Any_NEON(const uint8_t* src_uv,
            ptrdiff_t src_tile_stride,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void DetileToYUY2_C(const uint8_t* src_y,
            ptrdiff_t src_y_tile_stride,
            const uint8_t* src_uv,
            ptrdiff_t src_uv_tile_stride,
            uint8_t* dst_yuy2,
            int width);
        void DetileToYUY2_SSE2(const uint8_t* src_y,
            ptrdiff_t src_y_tile_stride,
            const uint8_t* src_uv,
            ptrdiff_t src_uv_tile_stride,
            uint8_t* dst_yuy2,
            int width);
        void DetileToYUY2_Any_SSE2(const uint8_t* src_y,
            ptrdiff_t src_y_tile_stride,
            const uint8_t* src_uv,
            ptrdiff_t src_uv_tile_stride,
            uint8_t* dst_yuy2,
            int width);
        void DetileToYUY2_NEON(const uint8_t* src_y,
            ptrdiff_t src_y_tile_stride,
            const uint8_t* src_uv,
            ptrdiff_t src_uv_tile_stride,
            uint8_t* dst_yuy2,
            int width);
        void DetileToYUY2_Any_NEON(const uint8_t* src_y,
            ptrdiff_t src_y_tile_stride,
            const uint8_t* src_uv,
            ptrdiff_t src_uv_tile_stride,
            uint8_t* dst_yuy2,
            int width);
        void UnpackMT2T_C(const uint8_t* src, uint16_t* dst, size_t size);
        void UnpackMT2T_NEON(const uint8_t* src, uint16_t* dst, size_t size);
        void MergeUVRow_C(const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_uv,
            int width);
        void MergeUVRow_SSE2(const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_uv,
            int width);
        void MergeUVRow_AVX2(const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_uv,
            int width);
        void MergeUVRow_AVX512BW(const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_uv,
            int width);
        void MergeUVRow_NEON(const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_uv,
            int width);
        void MergeUVRow_SME(const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_uv,
            int width);
        void MergeUVRow_LSX(const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_uv,
            int width);
        void MergeUVRow_RVV(const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_uv,
            int width);
        void MergeUVRow_Any_SSE2(const uint8_t* y_buf,
            const uint8_t* uv_buf,
            uint8_t* dst_ptr,
            int width);
        void MergeUVRow_Any_AVX2(const uint8_t* y_buf,
            const uint8_t* uv_buf,
            uint8_t* dst_ptr,
            int width);
        void MergeUVRow_Any_AVX512BW(const uint8_t* y_buf,
            const uint8_t* uv_buf,
            uint8_t* dst_ptr,
            int width);
        void MergeUVRow_Any_NEON(const uint8_t* y_buf,
            const uint8_t* uv_buf,
            uint8_t* dst_ptr,
            int width);
        void MergeUVRow_Any_LSX(const uint8_t* y_buf,
            const uint8_t* uv_buf,
            uint8_t* dst_ptr,
            int width);

        void HalfMergeUVRow_C(const uint8_t* src_u,
            int src_stride_u,
            const uint8_t* src_v,
            int src_stride_v,
            uint8_t* dst_uv,
            int width);

        void HalfMergeUVRow_NEON(const uint8_t* src_u,
            int src_stride_u,
            const uint8_t* src_v,
            int src_stride_v,
            uint8_t* dst_uv,
            int width);

        void HalfMergeUVRow_SSSE3(const uint8_t* src_u,
            int src_stride_u,
            const uint8_t* src_v,
            int src_stride_v,
            uint8_t* dst_uv,
            int width);

        void HalfMergeUVRow_AVX2(const uint8_t* src_u,
            int src_stride_u,
            const uint8_t* src_v,
            int src_stride_v,
            uint8_t* dst_uv,
            int width);

        void SplitRGBRow_C(const uint8_t* src_rgb,
            uint8_t* dst_r,
            uint8_t* dst_g,
            uint8_t* dst_b,
            int width);
        void SplitRGBRow_SSSE3(const uint8_t* src_rgb,
            uint8_t* dst_r,
            uint8_t* dst_g,
            uint8_t* dst_b,
            int width);
        void SplitRGBRow_SSE41(const uint8_t* src_rgb,
            uint8_t* dst_r,
            uint8_t* dst_g,
            uint8_t* dst_b,
            int width);
        void SplitRGBRow_AVX2(const uint8_t* src_rgb,
            uint8_t* dst_r,
            uint8_t* dst_g,
            uint8_t* dst_b,
            int width);
        void SplitRGBRow_NEON(const uint8_t* src_rgb,
            uint8_t* dst_r,
            uint8_t* dst_g,
            uint8_t* dst_b,
            int width);
        void SplitRGBRow_RVV(const uint8_t* src_rgb,
            uint8_t* dst_r,
            uint8_t* dst_g,
            uint8_t* dst_b,
            int width);
        void SplitRGBRow_Any_SSSE3(const uint8_t* src_ptr,
            uint8_t* dst_r,
            uint8_t* dst_g,
            uint8_t* dst_b,
            int width);
        void SplitRGBRow_Any_SSE41(const uint8_t* src_ptr,
            uint8_t* dst_r,
            uint8_t* dst_g,
            uint8_t* dst_b,
            int width);
        void SplitRGBRow_Any_AVX2(const uint8_t* src_ptr,
            uint8_t* dst_r,
            uint8_t* dst_g,
            uint8_t* dst_b,
            int width);
        void SplitRGBRow_Any_NEON(const uint8_t* src_ptr,
            uint8_t* dst_r,
            uint8_t* dst_g,
            uint8_t* dst_b,
            int width);

        void MergeRGBRow_C(const uint8_t* src_r,
            const uint8_t* src_g,
            const uint8_t* src_b,
            uint8_t* dst_rgb,
            int width);
        void MergeRGBRow_SSSE3(const uint8_t* src_r,
            const uint8_t* src_g,
            const uint8_t* src_b,
            uint8_t* dst_rgb,
            int width);
        void MergeRGBRow_NEON(const uint8_t* src_r,
            const uint8_t* src_g,
            const uint8_t* src_b,
            uint8_t* dst_rgb,
            int width);
        void MergeRGBRow_RVV(const uint8_t* src_r,
            const uint8_t* src_g,
            const uint8_t* src_b,
            uint8_t* dst_rgb,
            int width);
        void MergeRGBRow_Any_SSSE3(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_ptr,
            int width);
        void MergeRGBRow_Any_NEON(const uint8_t* src_r,
            const uint8_t* src_g,
            const uint8_t* src_b,
            uint8_t* dst_rgb,
            int width);
        void MergeARGBRow_C(const uint8_t* src_r,
            const uint8_t* src_g,
            const uint8_t* src_b,
            const uint8_t* src_a,
            uint8_t* dst_argb,
            int width);
        void MergeARGBRow_SSE2(const uint8_t* src_r,
            const uint8_t* src_g,
            const uint8_t* src_b,
            const uint8_t* src_a,
            uint8_t* dst_argb,
            int width);
        void MergeARGBRow_AVX2(const uint8_t* src_r,
            const uint8_t* src_g,
            const uint8_t* src_b,
            const uint8_t* src_a,
            uint8_t* dst_argb,
            int width);
        void MergeARGBRow_NEON(const uint8_t* src_r,
            const uint8_t* src_g,
            const uint8_t* src_b,
            const uint8_t* src_a,
            uint8_t* dst_argb,
            int width);
        void MergeARGBRow_RVV(const uint8_t* src_r,
            const uint8_t* src_g,
            const uint8_t* src_b,
            const uint8_t* src_a,
            uint8_t* dst_argb,
            int width);
        void MergeARGBRow_Any_SSE2(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            const uint8_t* a_buf,
            uint8_t* dst_ptr,
            int width);
        void MergeARGBRow_Any_AVX2(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            const uint8_t* a_buf,
            uint8_t* dst_ptr,
            int width);
        void MergeARGBRow_Any_NEON(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            const uint8_t* a_buf,
            uint8_t* dst_ptr,
            int width);
        void SplitARGBRow_C(const uint8_t* src_argb,
            uint8_t* dst_r,
            uint8_t* dst_g,
            uint8_t* dst_b,
            uint8_t* dst_a,
            int width);
        void SplitARGBRow_SSE2(const uint8_t* src_argb,
            uint8_t* dst_r,
            uint8_t* dst_g,
            uint8_t* dst_b,
            uint8_t* dst_a,
            int width);
        void SplitARGBRow_SSSE3(const uint8_t* src_argb,
            uint8_t* dst_r,
            uint8_t* dst_g,
            uint8_t* dst_b,
            uint8_t* dst_a,
            int width);
        void SplitARGBRow_AVX2(const uint8_t* src_argb,
            uint8_t* dst_r,
            uint8_t* dst_g,
            uint8_t* dst_b,
            uint8_t* dst_a,
            int width);
        void SplitARGBRow_NEON(const uint8_t* src_rgba,
            uint8_t* dst_r,
            uint8_t* dst_g,
            uint8_t* dst_b,
            uint8_t* dst_a,
            int width);
        void SplitARGBRow_RVV(const uint8_t* src_rgba,
            uint8_t* dst_r,
            uint8_t* dst_g,
            uint8_t* dst_b,
            uint8_t* dst_a,
            int width);
        void SplitARGBRow_Any_SSE2(const uint8_t* src_ptr,
            uint8_t* dst_r,
            uint8_t* dst_g,
            uint8_t* dst_b,
            uint8_t* dst_a,
            int width);
        void SplitARGBRow_Any_SSSE3(const uint8_t* src_ptr,
            uint8_t* dst_r,
            uint8_t* dst_g,
            uint8_t* dst_b,
            uint8_t* dst_a,
            int width);
        void SplitARGBRow_Any_AVX2(const uint8_t* src_ptr,
            uint8_t* dst_r,
            uint8_t* dst_g,
            uint8_t* dst_b,
            uint8_t* dst_a,
            int width);
        void SplitARGBRow_Any_NEON(const uint8_t* src_ptr,
            uint8_t* dst_r,
            uint8_t* dst_g,
            uint8_t* dst_b,
            uint8_t* dst_a,
            int width);
        void MergeXRGBRow_C(const uint8_t* src_r,
            const uint8_t* src_g,
            const uint8_t* src_b,
            uint8_t* dst_argb,
            int width);
        void MergeXRGBRow_SSE2(const uint8_t* src_r,
            const uint8_t* src_g,
            const uint8_t* src_b,
            uint8_t* dst_argb,
            int width);
        void MergeXRGBRow_AVX2(const uint8_t* src_r,
            const uint8_t* src_g,
            const uint8_t* src_b,
            uint8_t* dst_argb,
            int width);
        void MergeXRGBRow_NEON(const uint8_t* src_r,
            const uint8_t* src_g,
            const uint8_t* src_b,
            uint8_t* dst_argb,
            int width);
        void MergeXRGBRow_RVV(const uint8_t* src_r,
            const uint8_t* src_g,
            const uint8_t* src_b,
            uint8_t* dst_argb,
            int width);
        void MergeXRGBRow_Any_SSE2(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_ptr,
            int width);
        void MergeXRGBRow_Any_AVX2(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_ptr,
            int width);
        void MergeXRGBRow_Any_NEON(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_ptr,
            int width);
        void SplitXRGBRow_C(const uint8_t* src_argb,
            uint8_t* dst_r,
            uint8_t* dst_g,
            uint8_t* dst_b,
            int width);
        void SplitXRGBRow_SSE2(const uint8_t* src_argb,
            uint8_t* dst_r,
            uint8_t* dst_g,
            uint8_t* dst_b,
            int width);
        void SplitXRGBRow_SSSE3(const uint8_t* src_argb,
            uint8_t* dst_r,
            uint8_t* dst_g,
            uint8_t* dst_b,
            int width);
        void SplitXRGBRow_AVX2(const uint8_t* src_argb,
            uint8_t* dst_r,
            uint8_t* dst_g,
            uint8_t* dst_b,
            int width);
        void SplitXRGBRow_NEON(const uint8_t* src_rgba,
            uint8_t* dst_r,
            uint8_t* dst_g,
            uint8_t* dst_b,
            int width);
        void SplitXRGBRow_RVV(const uint8_t* src_rgba,
            uint8_t* dst_r,
            uint8_t* dst_g,
            uint8_t* dst_b,
            int width);
        void SplitXRGBRow_Any_SSE2(const uint8_t* src_ptr,
            uint8_t* dst_r,
            uint8_t* dst_g,
            uint8_t* dst_b,
            int width);
        void SplitXRGBRow_Any_SSSE3(const uint8_t* src_ptr,
            uint8_t* dst_r,
            uint8_t* dst_g,
            uint8_t* dst_b,
            int width);
        void SplitXRGBRow_Any_AVX2(const uint8_t* src_ptr,
            uint8_t* dst_r,
            uint8_t* dst_g,
            uint8_t* dst_b,
            int width);
        void SplitXRGBRow_Any_NEON(const uint8_t* src_ptr,
            uint8_t* dst_r,
            uint8_t* dst_g,
            uint8_t* dst_b,
            int width);

        void MergeXR30Row_C(const uint16_t* src_r,
            const uint16_t* src_g,
            const uint16_t* src_b,
            uint8_t* dst_ar30,
            int depth,
            int width);
        void MergeAR64Row_C(const uint16_t* src_r,
            const uint16_t* src_g,
            const uint16_t* src_b,
            const uint16_t* src_a,
            uint16_t* dst_ar64,
            int depth,
            int width);
        void MergeARGB16To8Row_C(const uint16_t* src_r,
            const uint16_t* src_g,
            const uint16_t* src_b,
            const uint16_t* src_a,
            uint8_t* dst_argb,
            int depth,
            int width);
        void MergeXR64Row_C(const uint16_t* src_r,
            const uint16_t* src_g,
            const uint16_t* src_b,
            uint16_t* dst_ar64,
            int depth,
            int width);
        void MergeXRGB16To8Row_C(const uint16_t* src_r,
            const uint16_t* src_g,
            const uint16_t* src_b,
            uint8_t* dst_argb,
            int depth,
            int width);
        void MergeXR30Row_AVX2(const uint16_t* src_r,
            const uint16_t* src_g,
            const uint16_t* src_b,
            uint8_t* dst_ar30,
            int depth,
            int width);
        void MergeAR64Row_AVX2(const uint16_t* src_r,
            const uint16_t* src_g,
            const uint16_t* src_b,
            const uint16_t* src_a,
            uint16_t* dst_ar64,
            int depth,
            int width);
        void MergeARGB16To8Row_AVX2(const uint16_t* src_r,
            const uint16_t* src_g,
            const uint16_t* src_b,
            const uint16_t* src_a,
            uint8_t* dst_argb,
            int depth,
            int width);
        void MergeXR64Row_AVX2(const uint16_t* src_r,
            const uint16_t* src_g,
            const uint16_t* src_b,
            uint16_t* dst_ar64,
            int depth,
            int width);
        void MergeXRGB16To8Row_AVX2(const uint16_t* src_r,
            const uint16_t* src_g,
            const uint16_t* src_b,
            uint8_t* dst_argb,
            int depth,
            int width);
        void MergeXR30Row_NEON(const uint16_t* src_r,
            const uint16_t* src_g,
            const uint16_t* src_b,
            uint8_t* dst_ar30,
            int depth,
            int width);
        void MergeXR30Row_10_NEON(const uint16_t* src_r,
            const uint16_t* src_g,
            const uint16_t* src_b,
            uint8_t* dst_ar30,
            int /* depth */,
            int width);
        void MergeAR64Row_NEON(const uint16_t* src_r,
            const uint16_t* src_g,
            const uint16_t* src_b,
            const uint16_t* src_a,
            uint16_t* dst_ar64,
            int depth,
            int width);
        void MergeARGB16To8Row_NEON(const uint16_t* src_r,
            const uint16_t* src_g,
            const uint16_t* src_b,
            const uint16_t* src_a,
            uint8_t* dst_argb,
            int depth,
            int width);
        void MergeXR64Row_NEON(const uint16_t* src_r,
            const uint16_t* src_g,
            const uint16_t* src_b,
            uint16_t* dst_ar64,
            int depth,
            int width);
        void MergeXRGB16To8Row_NEON(const uint16_t* src_r,
            const uint16_t* src_g,
            const uint16_t* src_b,
            uint8_t* dst_argb,
            int depth,
            int width);
        void MergeXR30Row_Any_AVX2(const uint16_t* r_buf,
            const uint16_t* g_buf,
            const uint16_t* b_buf,
            uint8_t* dst_ptr,
            int depth,
            int width);
        void MergeAR64Row_Any_AVX2(const uint16_t* r_buf,
            const uint16_t* g_buf,
            const uint16_t* b_buf,
            const uint16_t* a_buf,
            uint16_t* dst_ptr,
            int depth,
            int width);
        void MergeXR64Row_Any_AVX2(const uint16_t* r_buf,
            const uint16_t* g_buf,
            const uint16_t* b_buf,
            uint16_t* dst_ptr,
            int depth,
            int width);
        void MergeARGB16To8Row_Any_AVX2(const uint16_t* r_buf,
            const uint16_t* g_buf,
            const uint16_t* b_buf,
            const uint16_t* a_buf,
            uint8_t* dst_ptr,
            int depth,
            int width);
        void MergeXRGB16To8Row_Any_AVX2(const uint16_t* r_buf,
            const uint16_t* g_buf,
            const uint16_t* b_buf,
            uint8_t* dst_ptr,
            int depth,
            int width);
        void MergeXR30Row_Any_NEON(const uint16_t* r_buf,
            const uint16_t* g_buf,
            const uint16_t* b_buf,
            uint8_t* dst_ptr,
            int depth,
            int width);
        void MergeXR30Row_10_Any_NEON(const uint16_t* r_buf,
            const uint16_t* g_buf,
            const uint16_t* b_buf,
            uint8_t* dst_ptr,
            int depth,
            int width);
        void MergeAR64Row_Any_NEON(const uint16_t* r_buf,
            const uint16_t* g_buf,
            const uint16_t* b_buf,
            const uint16_t* a_buf,
            uint16_t* dst_ptr,
            int depth,
            int width);
        void MergeARGB16To8Row_Any_NEON(const uint16_t* r_buf,
            const uint16_t* g_buf,
            const uint16_t* b_buf,
            const uint16_t* a_buf,
            uint8_t* dst_ptr,
            int depth,
            int width);
        void MergeXR64Row_Any_NEON(const uint16_t* r_buf,
            const uint16_t* g_buf,
            const uint16_t* b_buf,
            uint16_t* dst_ptr,
            int depth,
            int width);
        void MergeXRGB16To8Row_Any_NEON(const uint16_t* r_buf,
            const uint16_t* g_buf,
            const uint16_t* b_buf,
            uint8_t* dst_ptr,
            int depth,
            int width);

        void MergeUVRow_16_C(const uint16_t* src_u,
            const uint16_t* src_v,
            uint16_t* dst_uv,
            int depth,
            int width);
        void MergeUVRow_16_AVX2(const uint16_t* src_u,
            const uint16_t* src_v,
            uint16_t* dst_uv,
            int depth,
            int width);
        void MergeUVRow_16_Any_AVX2(const uint16_t* src_u,
            const uint16_t* src_v,
            uint16_t* dst_uv,
            int depth,
            int width);
        void MergeUVRow_16_NEON(const uint16_t* src_u,
            const uint16_t* src_v,
            uint16_t* dst_uv,
            int depth,
            int width);
        void MergeUVRow_16_Any_NEON(const uint16_t* src_u,
            const uint16_t* src_v,
            uint16_t* dst_uv,
            int depth,
            int width);
        void MergeUVRow_16_SME(const uint16_t* src_u,
            const uint16_t* src_v,
            uint16_t* dst_uv,
            int depth,
            int width);

        void SplitUVRow_16_C(const uint16_t* src_uv,
            uint16_t* dst_u,
            uint16_t* dst_v,
            int depth,
            int width);
        void SplitUVRow_16_AVX2(const uint16_t* src_uv,
            uint16_t* dst_u,
            uint16_t* dst_v,
            int depth,
            int width);
        void SplitUVRow_16_Any_AVX2(const uint16_t* src_uv,
            uint16_t* dst_u,
            uint16_t* dst_v,
            int depth,
            int width);
        void SplitUVRow_16_NEON(const uint16_t* src_uv,
            uint16_t* dst_u,
            uint16_t* dst_v,
            int depth,
            int width);
        void SplitUVRow_16_Any_NEON(const uint16_t* src_uv,
            uint16_t* dst_u,
            uint16_t* dst_v,
            int depth,
            int width);

        void MultiplyRow_16_C(const uint16_t* src_y,
            uint16_t* dst_y,
            int scale,
            int width);
        void MultiplyRow_16_AVX2(const uint16_t* src_y,
            uint16_t* dst_y,
            int scale,
            int width);
        void MultiplyRow_16_Any_AVX2(const uint16_t* src_ptr,
            uint16_t* dst_ptr,
            int scale,
            int width);
        void MultiplyRow_16_NEON(const uint16_t* src_y,
            uint16_t* dst_y,
            int scale,
            int width);
        void MultiplyRow_16_Any_NEON(const uint16_t* src_ptr,
            uint16_t* dst_ptr,
            int scale,
            int width);
        void MultiplyRow_16_SME(const uint16_t* src_y,
            uint16_t* dst_y,
            int scale,
            int width);

        void DivideRow_16_C(const uint16_t* src_y,
            uint16_t* dst_y,
            int scale,
            int width);
        void DivideRow_16_AVX2(const uint16_t* src_y,
            uint16_t* dst_y,
            int scale,
            int width);
        void DivideRow_16_Any_AVX2(const uint16_t* src_ptr,
            uint16_t* dst_ptr,
            int scale,
            int width);
        void DivideRow_16_NEON(const uint16_t* src_y,
            uint16_t* dst_y,
            int scale,
            int width);
        void DivideRow_16_SVE2(const uint16_t* src_y,
            uint16_t* dst_y,
            int scale,
            int width);
        void DivideRow_16_Any_NEON(const uint16_t* src_ptr,
            uint16_t* dst_ptr,
            int scale,
            int width);

        void Convert8To16Row_C(const uint8_t* src_y,
            uint16_t* dst_y,
            int scale,
            int width);
        void Convert8To16Row_SSE2(const uint8_t* src_y,
            uint16_t* dst_y,
            int scale,
            int width);
        void Convert8To16Row_AVX2(const uint8_t* src_y,
            uint16_t* dst_y,
            int scale,
            int width);
        void Convert8To16Row_Any_SSE2(const uint8_t* src_ptr,
            uint16_t* dst_ptr,
            int scale,
            int width);
        void Convert8To16Row_Any_AVX2(const uint8_t* src_ptr,
            uint16_t* dst_ptr,
            int scale,
            int width);
        void Convert8To16Row_NEON(const uint8_t* src_y,
            uint16_t* dst_y,
            int scale,
            int width);
        void Convert8To16Row_Any_NEON(const uint8_t* src_y,
            uint16_t* dst_y,
            int scale,
            int width);
        void Convert8To16Row_SME(const uint8_t* src_y,
            uint16_t* dst_y,
            int scale,
            int width);

        void Convert16To8Row_C(const uint16_t* src_y,
            uint8_t* dst_y,
            int scale,
            int width);
        void Convert16To8Row_SSSE3(const uint16_t* src_y,
            uint8_t* dst_y,
            int scale,
            int width);
        void Convert16To8Row_AVX2(const uint16_t* src_y,
            uint8_t* dst_y,
            int scale,
            int width);
        void Convert16To8Row_AVX512BW(const uint16_t* src_y,
            uint8_t* dst_y,
            int scale,
            int width);
        void Convert16To8Row_Any_SSSE3(const uint16_t* src_ptr,
            uint8_t* dst_ptr,
            int scale,
            int width);
        void Convert16To8Row_Any_AVX2(const uint16_t* src_ptr,
            uint8_t* dst_ptr,
            int scale,
            int width);
        void Convert16To8Row_Any_AVX512BW(const uint16_t* src_ptr,
            uint8_t* dst_ptr,
            int scale,
            int width);
        void Convert16To8Row_NEON(const uint16_t* src_y,
            uint8_t* dst_y,
            int scale,
            int width);
        void Convert16To8Row_Any_NEON(const uint16_t* src_ptr,
            uint8_t* dst_ptr,
            int scale,
            int width);
        void Convert16To8Row_SME(const uint16_t* src_y,
            uint8_t* dst_y,
            int scale,
            int width);

        void Convert8To8Row_C(const uint8_t* src_y,
            uint8_t* dst_y,
            int scale,
            int bias,
            int width);
        void Convert8To8Row_NEON(const uint8_t* src_y,
            uint8_t* dst_y,
            int scale,
            int bias,
            int width);
        void Convert8To8Row_Any_NEON(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int scale,
            int bias,
            int width);
        void Convert8To8Row_SVE2(const uint8_t* src_y,
            uint8_t* dst_y,
            int scale,
            int bias,
            int width);
        void Convert8To8Row_SME(const uint8_t* src_y,
            uint8_t* dst_y,
            int scale,
            int bias,
            int width);
        void Convert8To8Row_AVX2(const uint8_t* src_y,
            uint8_t* dst_y,
            int scale,
            int bias,
            int width);
        void Convert8To8Row_Any_AVX2(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int scale,
            int bias,
            int width);

        void CopyRow_SSE2(const uint8_t* src, uint8_t* dst, int width);
        void CopyRow_AVX(const uint8_t* src, uint8_t* dst, int width);
        void CopyRow_AVX512BW(const uint8_t* src, uint8_t* dst, int width);
        void CopyRow_ERMS(const uint8_t* src, uint8_t* dst, int width);
        void CopyRow_NEON(const uint8_t* src, uint8_t* dst, int width);
        void CopyRow_SME(const uint8_t* src, uint8_t* dst, int width);
        void CopyRow_RVV(const uint8_t* src, uint8_t* dst, int count);
        void CopyRow_C(const uint8_t* src, uint8_t* dst, int count);
        void CopyRow_Any_SSE2(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void CopyRow_Any_AVX(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void CopyRow_Any_AVX512BW(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void CopyRow_Any_NEON(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);

        void CopyRow_16_C(const uint16_t* src, uint16_t* dst, int count);

        void ARGBCopyAlphaRow_C(const uint8_t* src, uint8_t* dst, int width);
        void ARGBCopyAlphaRow_SSE2(const uint8_t* src, uint8_t* dst, int width);
        void ARGBCopyAlphaRow_AVX2(const uint8_t* src, uint8_t* dst, int width);
        void ARGBCopyAlphaRow_Any_SSE2(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void ARGBCopyAlphaRow_Any_AVX2(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);

        void ARGBExtractAlphaRow_C(const uint8_t* src_argb, uint8_t* dst_a, int width);
        void ARGBExtractAlphaRow_SSE2(const uint8_t* src_argb,
            uint8_t* dst_a,
            int width);
        void ARGBExtractAlphaRow_AVX2(const uint8_t* src_argb,
            uint8_t* dst_a,
            int width);
        void ARGBExtractAlphaRow_NEON(const uint8_t* src_argb,
            uint8_t* dst_a,
            int width);
        void ARGBExtractAlphaRow_LSX(const uint8_t* src_argb,
            uint8_t* dst_a,
            int width);
        void ARGBExtractAlphaRow_RVV(const uint8_t* src_argb,
            uint8_t* dst_a,
            int width);
        void ARGBExtractAlphaRow_Any_SSE2(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void ARGBExtractAlphaRow_Any_AVX2(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void ARGBExtractAlphaRow_Any_NEON(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void ARGBExtractAlphaRow_Any_LSX(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);

        void ARGBCopyYToAlphaRow_C(const uint8_t* src, uint8_t* dst, int width);
        void ARGBCopyYToAlphaRow_SSE2(const uint8_t* src, uint8_t* dst, int width);
        void ARGBCopyYToAlphaRow_AVX2(const uint8_t* src, uint8_t* dst, int width);
        void ARGBCopyYToAlphaRow_RVV(const uint8_t* src, uint8_t* dst, int width);
        void ARGBCopyYToAlphaRow_Any_SSE2(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void ARGBCopyYToAlphaRow_Any_AVX2(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);

        void SetRow_C(uint8_t* dst, uint8_t v8, int width);
        void SetRow_X86(uint8_t* dst, uint8_t v8, int width);
        void SetRow_ERMS(uint8_t* dst, uint8_t v8, int width);
        void SetRow_NEON(uint8_t* dst, uint8_t v8, int width);
        void SetRow_LSX(uint8_t* dst, uint8_t v8, int width);
        void SetRow_Any_X86(uint8_t* dst_ptr, uint8_t v32, int width);
        void SetRow_Any_NEON(uint8_t* dst_ptr, uint8_t v32, int width);
        void SetRow_Any_LSX(uint8_t* dst_ptr, uint8_t v32, int width);

        void ARGBSetRow_C(uint8_t* dst_argb, uint32_t v32, int width);
        void ARGBSetRow_X86(uint8_t* dst_argb, uint32_t v32, int width);
        void ARGBSetRow_NEON(uint8_t* dst, uint32_t v32, int width);
        void ARGBSetRow_Any_NEON(uint8_t* dst_ptr, uint32_t v32, int width);
        void ARGBSetRow_LSX(uint8_t* dst_argb, uint32_t v32, int width);
        void ARGBSetRow_Any_LSX(uint8_t* dst_ptr, uint32_t v32, int width);

        // ARGBShufflers for BGRAToARGB etc.
        void ARGBShuffleRow_C(const uint8_t* src_argb,
            uint8_t* dst_argb,
            const uint8_t* shuffler,
            int width);
        void ARGBShuffleRow_SSSE3(const uint8_t* src_argb,
            uint8_t* dst_argb,
            const uint8_t* shuffler,
            int width);
        void ARGBShuffleRow_AVX2(const uint8_t* src_argb,
            uint8_t* dst_argb,
            const uint8_t* shuffler,
            int width);
        void ARGBShuffleRow_NEON(const uint8_t* src_argb,
            uint8_t* dst_argb,
            const uint8_t* shuffler,
            int width);
        void ARGBShuffleRow_LSX(const uint8_t* src_argb,
            uint8_t* dst_argb,
            const uint8_t* shuffler,
            int width);
        void ARGBShuffleRow_LASX(const uint8_t* src_argb,
            uint8_t* dst_argb,
            const uint8_t* shuffler,
            int width);
        void ARGBShuffleRow_Any_SSSE3(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            const uint8_t* param,
            int width);
        void ARGBShuffleRow_Any_AVX2(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            const uint8_t* param,
            int width);
        void ARGBShuffleRow_Any_NEON(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            const uint8_t* param,
            int width);
        void ARGBShuffleRow_Any_LSX(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            const uint8_t* param,
            int width);
        void ARGBShuffleRow_Any_LASX(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            const uint8_t* param,
            int width);

        void RGB24ToARGBRow_SSSE3(const uint8_t* src_rgb24,
            uint8_t* dst_argb,
            int width);
        void RAWToARGBRow_SSSE3(const uint8_t* src_raw, uint8_t* dst_argb, int width);
        void RAWToARGBRow_AVX2(const uint8_t* src_raw, uint8_t* dst_argb, int width);
        void RAWToRGBARow_SSSE3(const uint8_t* src_raw, uint8_t* dst_rgba, int width);
        void RAWToRGB24Row_SSSE3(const uint8_t* src_raw, uint8_t* dst_rgb24, int width);
        void RGB565ToARGBRow_SSE2(const uint8_t* src, uint8_t* dst, int width);
        void ARGB1555ToARGBRow_SSE2(const uint8_t* src, uint8_t* dst, int width);
        void ARGB4444ToARGBRow_SSE2(const uint8_t* src, uint8_t* dst, int width);
        void RGB565ToARGBRow_AVX2(const uint8_t* src_rgb565,
            uint8_t* dst_argb,
            int width);
        void ARGB1555ToARGBRow_AVX2(const uint8_t* src_argb1555,
            uint8_t* dst_argb,
            int width);
        void ARGB4444ToARGBRow_AVX2(const uint8_t* src_argb4444,
            uint8_t* dst_argb,
            int width);

        void RGB24ToARGBRow_NEON(const uint8_t* src_rgb24,
            uint8_t* dst_argb,
            int width);
        void RGB24ToARGBRow_SVE2(const uint8_t* src_rgb24,
            uint8_t* dst_argb,
            int width);
        void RGB24ToARGBRow_LSX(const uint8_t* src_rgb24, uint8_t* dst_argb, int width);
        void RGB24ToARGBRow_LASX(const uint8_t* src_rgb24,
            uint8_t* dst_argb,
            int width);
        void RGB24ToARGBRow_RVV(const uint8_t* src_rgb24, uint8_t* dst_argb, int width);
        void RAWToARGBRow_NEON(const uint8_t* src_raw, uint8_t* dst_argb, int width);
        void RAWToARGBRow_SVE2(const uint8_t* src_raw, uint8_t* dst_argb, int width);
        void RAWToRGBARow_NEON(const uint8_t* src_raw, uint8_t* dst_rgba, int width);
        void RAWToRGBARow_SVE2(const uint8_t* src_raw, uint8_t* dst_rgba, int width);
        void RAWToARGBRow_LSX(const uint8_t* src_raw, uint8_t* dst_argb, int width);
        void RAWToARGBRow_LASX(const uint8_t* src_raw, uint8_t* dst_argb, int width);
        void RAWToARGBRow_RVV(const uint8_t* src_raw, uint8_t* dst_argb, int width);
        void RAWToRGBARow_RVV(const uint8_t* src_raw, uint8_t* dst_rgba, int width);
        void RAWToRGB24Row_NEON(const uint8_t* src_raw, uint8_t* dst_rgb24, int width);
        void RAWToRGB24Row_SVE2(const uint8_t* src_raw, uint8_t* dst_rgb24, int width);
        void RAWToRGB24Row_LSX(const uint8_t* src_raw, uint8_t* dst_rgb24, int width);
        void RAWToRGB24Row_RVV(const uint8_t* src_raw, uint8_t* dst_rgb24, int width);
        void RGB565ToARGBRow_NEON(const uint8_t* src_rgb565,
            uint8_t* dst_argb,
            int width);
        void RGB565ToARGBRow_LSX(const uint8_t* src_rgb565,
            uint8_t* dst_argb,
            int width);
        void RGB565ToARGBRow_LASX(const uint8_t* src_rgb565,
            uint8_t* dst_argb,
            int width);
        void ARGB1555ToARGBRow_NEON(const uint8_t* src_argb1555,
            uint8_t* dst_argb,
            int width);
        void ARGB1555ToARGBRow_SVE2(const uint8_t* src_argb1555,
            uint8_t* dst_argb,
            int width);
        void ARGB1555ToARGBRow_LSX(const uint8_t* src_argb1555,
            uint8_t* dst_argb,
            int width);
        void ARGB1555ToARGBRow_LASX(const uint8_t* src_argb1555,
            uint8_t* dst_argb,
            int width);
        void ARGB4444ToARGBRow_NEON(const uint8_t* src_argb4444,
            uint8_t* dst_argb,
            int width);
        void ARGB4444ToARGBRow_LSX(const uint8_t* src_argb4444,
            uint8_t* dst_argb,
            int width);
        void ARGB4444ToARGBRow_LASX(const uint8_t* src_argb4444,
            uint8_t* dst_argb,
            int width);
        void RGB24ToARGBRow_C(const uint8_t* src_rgb24, uint8_t* dst_argb, int width);
        void RAWToARGBRow_C(const uint8_t* src_raw, uint8_t* dst_argb, int width);
        void RAWToRGBARow_C(const uint8_t* src_raw, uint8_t* dst_rgba, int width);
        void RAWToRGB24Row_C(const uint8_t* src_raw, uint8_t* dst_rgb24, int width);
        void RGB565ToARGBRow_C(const uint8_t* src_rgb565, uint8_t* dst_argb, int width);
        void ARGB1555ToARGBRow_C(const uint8_t* src_argb1555,
            uint8_t* dst_argb,
            int width);
        void ARGB4444ToARGBRow_C(const uint8_t* src_argb4444,
            uint8_t* dst_argb,
            int width);
        void AR30ToARGBRow_C(const uint8_t* src_ar30, uint8_t* dst_argb, int width);
        void AR30ToABGRRow_C(const uint8_t* src_ar30, uint8_t* dst_abgr, int width);
        void ARGBToAR30Row_C(const uint8_t* src_argb, uint8_t* dst_ar30, int width);
        void AR30ToAB30Row_C(const uint8_t* src_ar30, uint8_t* dst_ab30, int width);

        void RGB24ToARGBRow_Any_SSSE3(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void RAWToARGBRow_Any_SSSE3(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void RAWToARGBRow_Any_AVX2(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void RAWToRGBARow_Any_SSSE3(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void RAWToRGB24Row_Any_SSSE3(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);

        void RGB565ToARGBRow_Any_SSE2(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void ARGB1555ToARGBRow_Any_SSE2(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void ARGB4444ToARGBRow_Any_SSE2(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void RGB565ToARGBRow_Any_AVX2(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void ARGB1555ToARGBRow_Any_AVX2(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void ARGB4444ToARGBRow_Any_AVX2(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);

        void RGB24ToARGBRow_Any_NEON(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void RGB24ToARGBRow_Any_LSX(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void RGB24ToARGBRow_Any_LASX(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void RAWToARGBRow_Any_NEON(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void RAWToRGBARow_Any_NEON(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void RAWToARGBRow_Any_LSX(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void RAWToARGBRow_Any_LASX(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void RAWToRGB24Row_Any_NEON(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void RAWToRGB24Row_Any_LSX(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void RGB565ToARGBRow_Any_NEON(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void RGB565ToARGBRow_Any_LSX(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void RGB565ToARGBRow_Any_LASX(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void ARGB1555ToARGBRow_Any_NEON(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void ARGB4444ToARGBRow_Any_NEON(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void ARGB1555ToARGBRow_Any_LSX(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void ARGB1555ToARGBRow_Any_LASX(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);

        void ARGB4444ToARGBRow_Any_LSX(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void ARGB4444ToARGBRow_Any_LASX(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);

        void ARGBToRGB24Row_SSSE3(const uint8_t* src, uint8_t* dst, int width);
        void ARGBToRAWRow_SSSE3(const uint8_t* src, uint8_t* dst, int width);
        void ARGBToRGB565Row_SSE2(const uint8_t* src, uint8_t* dst, int width);
        void ARGBToARGB1555Row_SSE2(const uint8_t* src, uint8_t* dst, int width);
        void ARGBToARGB4444Row_SSE2(const uint8_t* src, uint8_t* dst, int width);
        void ABGRToAR30Row_SSSE3(const uint8_t* src, uint8_t* dst, int width);
        void ARGBToAR30Row_SSSE3(const uint8_t* src, uint8_t* dst, int width);

        void ARGBToRAWRow_AVX2(const uint8_t* src, uint8_t* dst, int width);
        void ARGBToRGB24Row_AVX2(const uint8_t* src, uint8_t* dst, int width);

        void ARGBToRGB24Row_AVX512VBMI(const uint8_t* src, uint8_t* dst, int width);

        void ARGBToRGB565DitherRow_C(const uint8_t* src_argb,
            uint8_t* dst_rgb,
            uint32_t dither4,
            int width);
        void ARGBToRGB565DitherRow_SSE2(const uint8_t* src,
            uint8_t* dst,
            uint32_t dither4,
            int width);
        void ARGBToRGB565DitherRow_AVX2(const uint8_t* src,
            uint8_t* dst,
            uint32_t dither4,
            int width);

        void ARGBToRGB565Row_AVX2(const uint8_t* src_argb, uint8_t* dst_rgb, int width);
        void ARGBToARGB1555Row_AVX2(const uint8_t* src_argb,
            uint8_t* dst_rgb,
            int width);
        void ARGBToARGB4444Row_AVX2(const uint8_t* src_argb,
            uint8_t* dst_rgb,
            int width);
        void ABGRToAR30Row_AVX2(const uint8_t* src, uint8_t* dst, int width);
        void ARGBToAR30Row_AVX2(const uint8_t* src, uint8_t* dst, int width);

        void ARGBToRGB24Row_NEON(const uint8_t* src_argb,
            uint8_t* dst_rgb24,
            int width);
        void ARGBToRGB24Row_SVE2(const uint8_t* src_argb,
            uint8_t* dst_rgb24,
            int width);
        void ARGBToRAWRow_NEON(const uint8_t* src_argb, uint8_t* dst_raw, int width);
        void ARGBToRAWRow_SVE2(const uint8_t* src_argb, uint8_t* dst_raw, int width);
        void ARGBToRGB565Row_NEON(const uint8_t* src_argb,
            uint8_t* dst_rgb565,
            int width);
        void ARGBToRGB565Row_SVE2(const uint8_t* src_argb,
            uint8_t* dst_rgb565,
            int width);
        void ARGBToARGB1555Row_NEON(const uint8_t* src_argb,
            uint8_t* dst_argb1555,
            int width);
        void ARGBToARGB4444Row_NEON(const uint8_t* src_argb,
            uint8_t* dst_argb4444,
            int width);
        void ARGBToRGB565DitherRow_NEON(const uint8_t* src_argb,
            uint8_t* dst_rgb,
            uint32_t dither4,
            int width);
        void ARGBToRGB565DitherRow_SVE2(const uint8_t* src_argb,
            uint8_t* dst_rgb,
            uint32_t dither4,
            int width);
        void ARGBToRGB565DitherRow_LSX(const uint8_t* src_argb,
            uint8_t* dst_rgb,
            uint32_t dither4,
            int width);
        void ARGBToRGB565DitherRow_LASX(const uint8_t* src_argb,
            uint8_t* dst_rgb,
            uint32_t dither4,
            int width);

        void ARGBToRGB24Row_LSX(const uint8_t* src_argb, uint8_t* dst_rgb, int width);
        void ARGBToRGB24Row_LASX(const uint8_t* src_argb, uint8_t* dst_rgb, int width);
        void ARGBToRAWRow_LSX(const uint8_t* src_argb, uint8_t* dst_rgb, int width);
        void ARGBToRAWRow_LASX(const uint8_t* src_argb, uint8_t* dst_rgb, int width);
        void ARGBToRGB565Row_LSX(const uint8_t* src_argb, uint8_t* dst_rgb, int width);
        void ARGBToRGB565Row_LASX(const uint8_t* src_argb, uint8_t* dst_rgb, int width);
        void ARGBToARGB1555Row_LSX(const uint8_t* src_argb,
            uint8_t* dst_rgb,
            int width);
        void ARGBToARGB1555Row_LASX(const uint8_t* src_argb,
            uint8_t* dst_rgb,
            int width);
        void ARGBToARGB4444Row_LSX(const uint8_t* src_argb,
            uint8_t* dst_rgb,
            int width);
        void ARGBToARGB4444Row_LASX(const uint8_t* src_argb,
            uint8_t* dst_rgb,
            int width);

        void ARGBToRAWRow_RVV(const uint8_t* src_argb, uint8_t* dst_raw, int width);
        void ARGBToABGRRow_RVV(const uint8_t* src_argb, uint8_t* dst_abgr, int width);
        void ARGBToBGRARow_RVV(const uint8_t* src_argb, uint8_t* dst_rgba, int width);
        void ARGBToRGBARow_RVV(const uint8_t* src_argb, uint8_t* dst_rgb, int width);
        void ARGBToRGB24Row_RVV(const uint8_t* src_argb, uint8_t* dst_rgb24, int width);

        void ARGBToABGRRow_C(const uint8_t* src_argb, uint8_t* dst_abgr, int width);
        void ARGBToBGRARow_C(const uint8_t* src_argb, uint8_t* dst_bgra, int width);
        void ARGBToRGBARow_C(const uint8_t* src_argb, uint8_t* dst_rgb, int width);
        void ARGBToRGB24Row_C(const uint8_t* src_argb, uint8_t* dst_rgb, int width);
        void ARGBToRAWRow_C(const uint8_t* src_argb, uint8_t* dst_rgb, int width);
        void ARGBToRGB565Row_C(const uint8_t* src_argb, uint8_t* dst_rgb, int width);
        void ARGBToARGB1555Row_C(const uint8_t* src_argb, uint8_t* dst_rgb, int width);
        void ARGBToARGB4444Row_C(const uint8_t* src_argb, uint8_t* dst_rgb, int width);
        void ABGRToAR30Row_C(const uint8_t* src_abgr, uint8_t* dst_ar30, int width);
        void ARGBToAR30Row_C(const uint8_t* src_argb, uint8_t* dst_ar30, int width);

        void ARGBToAR64Row_C(const uint8_t* src_argb, uint16_t* dst_ar64, int width);
        void ARGBToAB64Row_C(const uint8_t* src_argb, uint16_t* dst_ab64, int width);
        void AR64ToARGBRow_C(const uint16_t* src_ar64, uint8_t* dst_argb, int width);
        void AB64ToARGBRow_C(const uint16_t* src_ab64, uint8_t* dst_argb, int width);
        void AR64ToAB64Row_C(const uint16_t* src_ar64, uint16_t* dst_ab64, int width);
        void RGBAToARGBRow_C(const uint8_t* src_rgba, uint8_t* dst_argb, int width);
        void AR64ShuffleRow_C(const uint8_t* src_ar64,
            uint8_t* dst_ar64,
            const uint8_t* shuffler,
            int width);
        void ARGBToAR64Row_SSSE3(const uint8_t* src_argb,
            uint16_t* dst_ar64,
            int width);
        void ARGBToAB64Row_SSSE3(const uint8_t* src_argb,
            uint16_t* dst_ab64,
            int width);
        void AR64ToARGBRow_SSSE3(const uint16_t* src_ar64,
            uint8_t* dst_argb,
            int width);
        void AB64ToARGBRow_SSSE3(const uint16_t* src_ab64,
            uint8_t* dst_argb,
            int width);
        void ARGBToAR64Row_AVX2(const uint8_t* src_argb, uint16_t* dst_ar64, int width);
        void ARGBToAB64Row_AVX2(const uint8_t* src_argb, uint16_t* dst_ab64, int width);
        void AR64ToARGBRow_AVX2(const uint16_t* src_ar64, uint8_t* dst_argb, int width);
        void AB64ToARGBRow_AVX2(const uint16_t* src_ab64, uint8_t* dst_argb, int width);
        void ARGBToAR64Row_NEON(const uint8_t* src_argb, uint16_t* dst_ar64, int width);
        void ARGBToAB64Row_NEON(const uint8_t* src_argb, uint16_t* dst_ab64, int width);
        void AR64ToARGBRow_NEON(const uint16_t* src_ar64, uint8_t* dst_argb, int width);
        void AB64ToARGBRow_NEON(const uint16_t* src_ab64, uint8_t* dst_argb, int width);
        void ARGBToAR64Row_RVV(const uint8_t* src_argb, uint16_t* dst_ar64, int width);
        void ARGBToAB64Row_RVV(const uint8_t* src_argb, uint16_t* dst_ab64, int width);
        void AR64ToARGBRow_RVV(const uint16_t* src_ar64, uint8_t* dst_argb, int width);
        void AB64ToARGBRow_RVV(const uint16_t* src_ab64, uint8_t* dst_argb, int width);
        void AR64ToAB64Row_RVV(const uint16_t* src_ar64, uint16_t* dst_ab64, int width);
        void RGBAToARGBRow_RVV(const uint8_t* src_rgba, uint8_t* dst_argb, int width);
        void ARGBToAR64Row_Any_SSSE3(const uint8_t* src_ptr,
            uint16_t* dst_ptr,
            int width);
        void ARGBToAB64Row_Any_SSSE3(const uint8_t* src_ptr,
            uint16_t* dst_ptr,
            int width);
        void AR64ToARGBRow_Any_SSSE3(const uint16_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void AB64ToARGBRow_Any_SSSE3(const uint16_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void ARGBToAR64Row_Any_AVX2(const uint8_t* src_ptr,
            uint16_t* dst_ptr,
            int width);
        void ARGBToAB64Row_Any_AVX2(const uint8_t* src_ptr,
            uint16_t* dst_ptr,
            int width);
        void AR64ToARGBRow_Any_AVX2(const uint16_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void AB64ToARGBRow_Any_AVX2(const uint16_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void ARGBToAR64Row_Any_NEON(const uint8_t* src_ptr,
            uint16_t* dst_ptr,
            int width);
        void ARGBToAB64Row_Any_NEON(const uint8_t* src_ptr,
            uint16_t* dst_ptr,
            int width);
        void AR64ToARGBRow_Any_NEON(const uint16_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void AB64ToARGBRow_Any_NEON(const uint16_t* src_ptr,
            uint8_t* dst_ptr,
            int width);

        void J400ToARGBRow_SSE2(const uint8_t* src_y, uint8_t* dst_argb, int width);
        void J400ToARGBRow_AVX2(const uint8_t* src_y, uint8_t* dst_argb, int width);
        void J400ToARGBRow_NEON(const uint8_t* src_y, uint8_t* dst_argb, int width);
        void J400ToARGBRow_LSX(const uint8_t* src_y, uint8_t* dst_argb, int width);
        void J400ToARGBRow_RVV(const uint8_t* src_y, uint8_t* dst_argb, int width);
        void J400ToARGBRow_C(const uint8_t* src_y, uint8_t* dst_argb, int width);
        void J400ToARGBRow_Any_SSE2(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void J400ToARGBRow_Any_AVX2(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void J400ToARGBRow_Any_NEON(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void J400ToARGBRow_Any_LSX(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);

        void I444ToARGBRow_C(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* rgb_buf,
            const struct YuvConstants* yuvconstants,
            int width);
        void I444ToRGB24Row_C(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* rgb_buf,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToARGBRow_C(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* rgb_buf,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToAR30Row_C(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* rgb_buf,
            const struct YuvConstants* yuvconstants,
            int width);
        void I210ToAR30Row_C(const uint16_t* src_y,
            const uint16_t* src_u,
            const uint16_t* src_v,
            uint8_t* rgb_buf,
            const struct YuvConstants* yuvconstants,
            int width);
        void I210ToARGBRow_C(const uint16_t* src_y,
            const uint16_t* src_u,
            const uint16_t* src_v,
            uint8_t* rgb_buf,
            const struct YuvConstants* yuvconstants,
            int width);
        void I212ToAR30Row_C(const uint16_t* src_y,
            const uint16_t* src_u,
            const uint16_t* src_v,
            uint8_t* rgb_buf,
            const struct YuvConstants* yuvconstants,
            int width);
        void I212ToARGBRow_C(const uint16_t* src_y,
            const uint16_t* src_u,
            const uint16_t* src_v,
            uint8_t* rgb_buf,
            const struct YuvConstants* yuvconstants,
            int width);
        void I410ToAR30Row_C(const uint16_t* src_y,
            const uint16_t* src_u,
            const uint16_t* src_v,
            uint8_t* rgb_buf,
            const struct YuvConstants* yuvconstants,
            int width);
        void I410ToARGBRow_C(const uint16_t* src_y,
            const uint16_t* src_u,
            const uint16_t* src_v,
            uint8_t* rgb_buf,
            const struct YuvConstants* yuvconstants,
            int width);
        void I210AlphaToARGBRow_C(const uint16_t* src_y,
            const uint16_t* src_u,
            const uint16_t* src_v,
            const uint16_t* src_a,
            uint8_t* rgb_buf,
            const struct YuvConstants* yuvconstants,
            int width);
        void I410AlphaToARGBRow_C(const uint16_t* src_y,
            const uint16_t* src_u,
            const uint16_t* src_v,
            const uint16_t* src_a,
            uint8_t* rgb_buf,
            const struct YuvConstants* yuvconstants,
            int width);
        void I444AlphaToARGBRow_C(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            const uint8_t* src_a,
            uint8_t* rgb_buf,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422AlphaToARGBRow_C(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            const uint8_t* src_a,
            uint8_t* rgb_buf,
            const struct YuvConstants* yuvconstants,
            int width);
        void NV12ToARGBRow_C(const uint8_t* src_y,
            const uint8_t* src_uv,
            uint8_t* rgb_buf,
            const struct YuvConstants* yuvconstants,
            int width);
        void NV12ToRGB565Row_C(const uint8_t* src_y,
            const uint8_t* src_uv,
            uint8_t* dst_rgb565,
            const struct YuvConstants* yuvconstants,
            int width);
        void NV21ToARGBRow_C(const uint8_t* src_y,
            const uint8_t* src_vu,
            uint8_t* rgb_buf,
            const struct YuvConstants* yuvconstants,
            int width);
        void NV12ToRGB24Row_C(const uint8_t* src_y,
            const uint8_t* src_uv,
            uint8_t* rgb_buf,
            const struct YuvConstants* yuvconstants,
            int width);
        void NV21ToRGB24Row_C(const uint8_t* src_y,
            const uint8_t* src_vu,
            uint8_t* rgb_buf,
            const struct YuvConstants* yuvconstants,
            int width);
        void NV21ToYUV24Row_C(const uint8_t* src_y,
            const uint8_t* src_vu,
            uint8_t* dst_yuv24,
            int width);
        void YUY2ToARGBRow_C(const uint8_t* src_yuy2,
            uint8_t* rgb_buf,
            const struct YuvConstants* yuvconstants,
            int width);
        void UYVYToARGBRow_C(const uint8_t* src_uyvy,
            uint8_t* rgb_buf,
            const struct YuvConstants* yuvconstants,
            int width);
        void P210ToARGBRow_C(const uint16_t* src_y,
            const uint16_t* src_uv,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void P410ToARGBRow_C(const uint16_t* src_y,
            const uint16_t* src_uv,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void P210ToAR30Row_C(const uint16_t* src_y,
            const uint16_t* src_uv,
            uint8_t* dst_ar30,
            const struct YuvConstants* yuvconstants,
            int width);
        void P410ToAR30Row_C(const uint16_t* src_y,
            const uint16_t* src_uv,
            uint8_t* dst_ar30,
            const struct YuvConstants* yuvconstants,
            int width);

        void I422ToRGBARow_C(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* rgb_buf,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToRGB24Row_C(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* rgb_buf,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToARGB4444Row_C(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_argb4444,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToARGB1555Row_C(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_argb1555,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToRGB565Row_C(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_rgb565,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToARGBRow_AVX2(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToARGBRow_AVX512BW(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToRGBARow_AVX2(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void I444ToARGBRow_SSSE3(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void I444ToARGBRow_AVX2(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void I444ToRGB24Row_SSSE3(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_rgb24,
            const struct YuvConstants* yuvconstants,
            int width);
        void I444ToRGB24Row_AVX2(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_rgb24,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToARGBRow_SSSE3(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);

        void I422ToAR30Row_SSSE3(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_ar30,
            const struct YuvConstants* yuvconstants,
            int width);
        void I210ToAR30Row_SSSE3(const uint16_t* y_buf,
            const uint16_t* u_buf,
            const uint16_t* v_buf,
            uint8_t* dst_ar30,
            const struct YuvConstants* yuvconstants,
            int width);
        void I210ToARGBRow_SSSE3(const uint16_t* y_buf,
            const uint16_t* u_buf,
            const uint16_t* v_buf,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void I212ToAR30Row_SSSE3(const uint16_t* y_buf,
            const uint16_t* u_buf,
            const uint16_t* v_buf,
            uint8_t* dst_ar30,
            const struct YuvConstants* yuvconstants,
            int width);
        void I212ToARGBRow_SSSE3(const uint16_t* y_buf,
            const uint16_t* u_buf,
            const uint16_t* v_buf,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void I410ToAR30Row_SSSE3(const uint16_t* y_buf,
            const uint16_t* u_buf,
            const uint16_t* v_buf,
            uint8_t* dst_ar30,
            const struct YuvConstants* yuvconstants,
            int width);
        void I410ToARGBRow_SSSE3(const uint16_t* y_buf,
            const uint16_t* u_buf,
            const uint16_t* v_buf,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void I210AlphaToARGBRow_SSSE3(const uint16_t* y_buf,
            const uint16_t* u_buf,
            const uint16_t* v_buf,
            const uint16_t* a_buf,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void I410AlphaToARGBRow_SSSE3(const uint16_t* y_buf,
            const uint16_t* u_buf,
            const uint16_t* v_buf,
            const uint16_t* a_buf,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToAR30Row_AVX2(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_ar30,
            const struct YuvConstants* yuvconstants,
            int width);
        void I210ToARGBRow_AVX2(const uint16_t* y_buf,
            const uint16_t* u_buf,
            const uint16_t* v_buf,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void I210ToAR30Row_AVX2(const uint16_t* y_buf,
            const uint16_t* u_buf,
            const uint16_t* v_buf,
            uint8_t* dst_ar30,
            const struct YuvConstants* yuvconstants,
            int width);
        void I212ToARGBRow_AVX2(const uint16_t* y_buf,
            const uint16_t* u_buf,
            const uint16_t* v_buf,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void I212ToAR30Row_AVX2(const uint16_t* y_buf,
            const uint16_t* u_buf,
            const uint16_t* v_buf,
            uint8_t* dst_ar30,
            const struct YuvConstants* yuvconstants,
            int width);
        void I410ToAR30Row_AVX2(const uint16_t* y_buf,
            const uint16_t* u_buf,
            const uint16_t* v_buf,
            uint8_t* dst_ar30,
            const struct YuvConstants* yuvconstants,
            int width);
        void I410ToARGBRow_AVX2(const uint16_t* y_buf,
            const uint16_t* u_buf,
            const uint16_t* v_buf,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void I210AlphaToARGBRow_AVX2(const uint16_t* y_buf,
            const uint16_t* u_buf,
            const uint16_t* v_buf,
            const uint16_t* a_buf,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void I410AlphaToARGBRow_AVX2(const uint16_t* y_buf,
            const uint16_t* u_buf,
            const uint16_t* v_buf,
            const uint16_t* a_buf,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void I444AlphaToARGBRow_SSSE3(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            const uint8_t* a_buf,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void I444AlphaToARGBRow_AVX2(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            const uint8_t* a_buf,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422AlphaToARGBRow_SSSE3(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            const uint8_t* a_buf,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422AlphaToARGBRow_AVX2(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            const uint8_t* a_buf,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void NV12ToARGBRow_SSSE3(const uint8_t* y_buf,
            const uint8_t* uv_buf,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void NV12ToARGBRow_AVX2(const uint8_t* y_buf,
            const uint8_t* uv_buf,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void NV12ToRGB24Row_SSSE3(const uint8_t* src_y,
            const uint8_t* src_uv,
            uint8_t* dst_rgb24,
            const struct YuvConstants* yuvconstants,
            int width);
        void NV21ToRGB24Row_SSSE3(const uint8_t* src_y,
            const uint8_t* src_vu,
            uint8_t* dst_rgb24,
            const struct YuvConstants* yuvconstants,
            int width);
        void NV12ToRGB565Row_SSSE3(const uint8_t* src_y,
            const uint8_t* src_uv,
            uint8_t* dst_rgb565,
            const struct YuvConstants* yuvconstants,
            int width);
        void NV12ToRGB24Row_AVX2(const uint8_t* src_y,
            const uint8_t* src_uv,
            uint8_t* dst_rgb24,
            const struct YuvConstants* yuvconstants,
            int width);
        void NV21ToRGB24Row_AVX2(const uint8_t* src_y,
            const uint8_t* src_vu,
            uint8_t* dst_rgb24,
            const struct YuvConstants* yuvconstants,
            int width);
        void NV21ToYUV24Row_SSSE3(const uint8_t* src_y,
            const uint8_t* src_vu,
            uint8_t* dst_yuv24,
            int width);
        void NV21ToYUV24Row_AVX2(const uint8_t* src_y,
            const uint8_t* src_vu,
            uint8_t* dst_yuv24,
            int width);
        void NV12ToRGB565Row_AVX2(const uint8_t* src_y,
            const uint8_t* src_uv,
            uint8_t* dst_rgb565,
            const struct YuvConstants* yuvconstants,
            int width);
        void NV21ToARGBRow_SSSE3(const uint8_t* y_buf,
            const uint8_t* vu_buf,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void NV21ToARGBRow_AVX2(const uint8_t* y_buf,
            const uint8_t* vu_buf,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void YUY2ToARGBRow_SSSE3(const uint8_t* yuy2_buf,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void UYVYToARGBRow_SSSE3(const uint8_t* uyvy_buf,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void YUY2ToARGBRow_AVX2(const uint8_t* yuy2_buf,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void UYVYToARGBRow_AVX2(const uint8_t* uyvy_buf,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);

        void P210ToARGBRow_SSSE3(const uint16_t* y_buf,
            const uint16_t* uv_buf,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void P410ToARGBRow_SSSE3(const uint16_t* y_buf,
            const uint16_t* uv_buf,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void P210ToAR30Row_SSSE3(const uint16_t* y_buf,
            const uint16_t* uv_buf,
            uint8_t* dst_ar30,
            const struct YuvConstants* yuvconstants,
            int width);
        void P410ToAR30Row_SSSE3(const uint16_t* y_buf,
            const uint16_t* uv_buf,
            uint8_t* dst_ar30,
            const struct YuvConstants* yuvconstants,
            int width);
        void P210ToARGBRow_AVX2(const uint16_t* y_buf,
            const uint16_t* uv_buf,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void P410ToARGBRow_AVX2(const uint16_t* y_buf,
            const uint16_t* uv_buf,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void P210ToAR30Row_AVX2(const uint16_t* y_buf,
            const uint16_t* uv_buf,
            uint8_t* dst_ar30,
            const struct YuvConstants* yuvconstants,
            int width);
        void P410ToAR30Row_AVX2(const uint16_t* y_buf,
            const uint16_t* uv_buf,
            uint8_t* dst_ar30,
            const struct YuvConstants* yuvconstants,
            int width);

        void I422ToRGBARow_SSSE3(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_rgba,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToARGB4444Row_SSSE3(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_argb4444,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToARGB4444Row_AVX2(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_argb4444,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToARGB1555Row_SSSE3(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_argb1555,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToARGB1555Row_AVX2(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_argb1555,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToRGB565Row_SSSE3(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_rgb565,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToRGB565Row_AVX2(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_rgb565,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToRGB24Row_SSSE3(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_rgb24,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToRGB24Row_AVX2(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_rgb24,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToARGBRow_Any_AVX2(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToARGBRow_Any_AVX512BW(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToRGBARow_Any_AVX2(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I444ToARGBRow_Any_SSSE3(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I444ToRGB24Row_Any_SSSE3(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I444ToARGBRow_Any_AVX2(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I444ToRGB24Row_Any_AVX2(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToARGBRow_Any_SSSE3(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToAR30Row_Any_SSSE3(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I210ToAR30Row_Any_SSSE3(const uint16_t* y_buf,
            const uint16_t* u_buf,
            const uint16_t* v_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I210ToARGBRow_Any_SSSE3(const uint16_t* y_buf,
            const uint16_t* u_buf,
            const uint16_t* v_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I212ToAR30Row_Any_SSSE3(const uint16_t* y_buf,
            const uint16_t* u_buf,
            const uint16_t* v_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I212ToARGBRow_Any_SSSE3(const uint16_t* y_buf,
            const uint16_t* u_buf,
            const uint16_t* v_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I410ToAR30Row_Any_SSSE3(const uint16_t* y_buf,
            const uint16_t* u_buf,
            const uint16_t* v_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I410ToARGBRow_Any_SSSE3(const uint16_t* y_buf,
            const uint16_t* u_buf,
            const uint16_t* v_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I210AlphaToARGBRow_Any_SSSE3(const uint16_t* y_buf,
            const uint16_t* u_buf,
            const uint16_t* v_buf,
            const uint16_t* a_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I410AlphaToARGBRow_Any_SSSE3(const uint16_t* y_buf,
            const uint16_t* u_buf,
            const uint16_t* v_buf,
            const uint16_t* a_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToAR30Row_Any_AVX2(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I210ToARGBRow_Any_AVX2(const uint16_t* y_buf,
            const uint16_t* u_buf,
            const uint16_t* v_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I210ToAR30Row_Any_AVX2(const uint16_t* y_buf,
            const uint16_t* u_buf,
            const uint16_t* v_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I212ToARGBRow_Any_AVX2(const uint16_t* y_buf,
            const uint16_t* u_buf,
            const uint16_t* v_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I212ToAR30Row_Any_AVX2(const uint16_t* y_buf,
            const uint16_t* u_buf,
            const uint16_t* v_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I410ToAR30Row_Any_AVX2(const uint16_t* y_buf,
            const uint16_t* u_buf,
            const uint16_t* v_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I410ToARGBRow_Any_AVX2(const uint16_t* y_buf,
            const uint16_t* u_buf,
            const uint16_t* v_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I210AlphaToARGBRow_Any_AVX2(const uint16_t* y_buf,
            const uint16_t* u_buf,
            const uint16_t* v_buf,
            const uint16_t* a_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I410AlphaToARGBRow_Any_AVX2(const uint16_t* y_buf,
            const uint16_t* u_buf,
            const uint16_t* v_buf,
            const uint16_t* a_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I444AlphaToARGBRow_Any_SSSE3(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            const uint8_t* a_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I444AlphaToARGBRow_Any_AVX2(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            const uint8_t* a_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422AlphaToARGBRow_Any_SSSE3(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            const uint8_t* a_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422AlphaToARGBRow_Any_AVX2(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            const uint8_t* a_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void NV12ToARGBRow_Any_SSSE3(const uint8_t* y_buf,
            const uint8_t* uv_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void NV12ToARGBRow_Any_AVX2(const uint8_t* y_buf,
            const uint8_t* uv_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void NV21ToARGBRow_Any_SSSE3(const uint8_t* y_buf,
            const uint8_t* uv_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void NV21ToARGBRow_Any_AVX2(const uint8_t* y_buf,
            const uint8_t* uv_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void NV12ToRGB24Row_Any_SSSE3(const uint8_t* y_buf,
            const uint8_t* uv_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void NV21ToRGB24Row_Any_SSSE3(const uint8_t* y_buf,
            const uint8_t* uv_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void NV12ToRGB24Row_Any_AVX2(const uint8_t* y_buf,
            const uint8_t* uv_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void NV21ToRGB24Row_Any_AVX2(const uint8_t* y_buf,
            const uint8_t* uv_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void NV21ToYUV24Row_Any_SSSE3(const uint8_t* y_buf,
            const uint8_t* uv_buf,
            uint8_t* dst_ptr,
            int width);
        void NV21ToYUV24Row_Any_AVX2(const uint8_t* y_buf,
            const uint8_t* uv_buf,
            uint8_t* dst_ptr,
            int width);
        void NV12ToRGB565Row_Any_SSSE3(const uint8_t* y_buf,
            const uint8_t* uv_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void NV12ToRGB565Row_Any_AVX2(const uint8_t* y_buf,
            const uint8_t* uv_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void YUY2ToARGBRow_Any_SSSE3(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void UYVYToARGBRow_Any_SSSE3(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void YUY2ToARGBRow_Any_AVX2(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void UYVYToARGBRow_Any_AVX2(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void P210ToARGBRow_Any_SSSE3(const uint16_t* y_buf,
            const uint16_t* uv_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void P410ToARGBRow_Any_SSSE3(const uint16_t* y_buf,
            const uint16_t* uv_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void P210ToAR30Row_Any_SSSE3(const uint16_t* y_buf,
            const uint16_t* uv_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void P410ToAR30Row_Any_SSSE3(const uint16_t* y_buf,
            const uint16_t* uv_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void P210ToARGBRow_Any_AVX2(const uint16_t* y_buf,
            const uint16_t* uv_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void P410ToARGBRow_Any_AVX2(const uint16_t* y_buf,
            const uint16_t* uv_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void P210ToAR30Row_Any_AVX2(const uint16_t* y_buf,
            const uint16_t* uv_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void P410ToAR30Row_Any_AVX2(const uint16_t* y_buf,
            const uint16_t* uv_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToRGBARow_Any_SSSE3(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToARGB4444Row_Any_SSSE3(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToARGB4444Row_Any_AVX2(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToARGB1555Row_Any_SSSE3(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToARGB1555Row_Any_AVX2(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToRGB565Row_Any_SSSE3(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToRGB565Row_Any_AVX2(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToRGB24Row_Any_SSSE3(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToRGB24Row_Any_AVX2(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);

        void I400ToARGBRow_C(const uint8_t* src_y,
            uint8_t* rgb_buf,
            const struct YuvConstants* yuvconstants,
            int width);
        void I400ToARGBRow_SSE2(const uint8_t* y_buf,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void I400ToARGBRow_AVX2(const uint8_t* y_buf,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void I400ToARGBRow_NEON(const uint8_t* src_y,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void I400ToARGBRow_SVE2(const uint8_t* src_y,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void I400ToARGBRow_SME(const uint8_t* src_y,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void I400ToARGBRow_LSX(const uint8_t* src_y,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void I400ToARGBRow_RVV(const uint8_t* src_y,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void I400ToARGBRow_Any_SSE2(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            const struct YuvConstants* param,
            int width);
        void I400ToARGBRow_Any_AVX2(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            const struct YuvConstants* param,
            int width);
        void I400ToARGBRow_Any_NEON(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            const struct YuvConstants* param,
            int width);
        void I400ToARGBRow_Any_LSX(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);

        // ARGB preattenuated alpha blend.
        void ARGBBlendRow_SSSE3(const uint8_t* src_argb,
            const uint8_t* src_argb1,
            uint8_t* dst_argb,
            int width);
        void ARGBBlendRow_NEON(const uint8_t* src_argb,
            const uint8_t* src_argb1,
            uint8_t* dst_argb,
            int width);
        void ARGBBlendRow_LSX(const uint8_t* src_argb0,
            const uint8_t* src_argb1,
            uint8_t* dst_argb,
            int width);
        void ARGBBlendRow_RVV(const uint8_t* src_argb0,
            const uint8_t* src_argb1,
            uint8_t* dst_argb,
            int width);
        void ARGBBlendRow_C(const uint8_t* src_argb,
            const uint8_t* src_argb1,
            uint8_t* dst_argb,
            int width);

        // Unattenuated planar alpha blend.
        void BlendPlaneRow_SSSE3(const uint8_t* src0,
            const uint8_t* src1,
            const uint8_t* alpha,
            uint8_t* dst,
            int width);
        void BlendPlaneRow_Any_SSSE3(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_ptr,
            int width);
        void BlendPlaneRow_AVX2(const uint8_t* src0,
            const uint8_t* src1,
            const uint8_t* alpha,
            uint8_t* dst,
            int width);
        void BlendPlaneRow_Any_AVX2(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_ptr,
            int width);
        void BlendPlaneRow_RVV(const uint8_t* src0,
            const uint8_t* src1,
            const uint8_t* alpha,
            uint8_t* dst,
            int width);
        void BlendPlaneRow_C(const uint8_t* src0,
            const uint8_t* src1,
            const uint8_t* alpha,
            uint8_t* dst,
            int width);

        // ARGB multiply images. Same API as Blend, but these require
        // pointer and width alignment for SSE2.
        void ARGBMultiplyRow_C(const uint8_t* src_argb,
            const uint8_t* src_argb1,
            uint8_t* dst_argb,
            int width);
        void ARGBMultiplyRow_SSE2(const uint8_t* src_argb,
            const uint8_t* src_argb1,
            uint8_t* dst_argb,
            int width);
        void ARGBMultiplyRow_Any_SSE2(const uint8_t* y_buf,
            const uint8_t* uv_buf,
            uint8_t* dst_ptr,
            int width);
        void ARGBMultiplyRow_AVX2(const uint8_t* src_argb,
            const uint8_t* src_argb1,
            uint8_t* dst_argb,
            int width);
        void ARGBMultiplyRow_Any_AVX2(const uint8_t* y_buf,
            const uint8_t* uv_buf,
            uint8_t* dst_ptr,
            int width);
        void ARGBMultiplyRow_NEON(const uint8_t* src_argb,
            const uint8_t* src_argb1,
            uint8_t* dst_argb,
            int width);
        void ARGBMultiplyRow_Any_NEON(const uint8_t* y_buf,
            const uint8_t* uv_buf,
            uint8_t* dst_ptr,
            int width);
        void ARGBMultiplyRow_SME(const uint8_t* src_argb,
            const uint8_t* src_argb1,
            uint8_t* dst_argb,
            int width);
        void ARGBMultiplyRow_LSX(const uint8_t* src_argb0,
            const uint8_t* src_argb1,
            uint8_t* dst_argb,
            int width);
        void ARGBMultiplyRow_LASX(const uint8_t* src_argb0,
            const uint8_t* src_argb1,
            uint8_t* dst_argb,
            int width);
        void ARGBMultiplyRow_Any_LSX(const uint8_t* y_buf,
            const uint8_t* uv_buf,
            uint8_t* dst_ptr,
            int width);
        void ARGBMultiplyRow_Any_LASX(const uint8_t* y_buf,
            const uint8_t* uv_buf,
            uint8_t* dst_ptr,
            int width);

        // ARGB add images.
        void ARGBAddRow_C(const uint8_t* src_argb,
            const uint8_t* src_argb1,
            uint8_t* dst_argb,
            int width);
        void ARGBAddRow_SSE2(const uint8_t* src_argb,
            const uint8_t* src_argb1,
            uint8_t* dst_argb,
            int width);
        void ARGBAddRow_Any_SSE2(const uint8_t* y_buf,
            const uint8_t* uv_buf,
            uint8_t* dst_ptr,
            int width);
        void ARGBAddRow_AVX2(const uint8_t* src_argb,
            const uint8_t* src_argb1,
            uint8_t* dst_argb,
            int width);
        void ARGBAddRow_Any_AVX2(const uint8_t* y_buf,
            const uint8_t* uv_buf,
            uint8_t* dst_ptr,
            int width);
        void ARGBAddRow_NEON(const uint8_t* src_argb,
            const uint8_t* src_argb1,
            uint8_t* dst_argb,
            int width);
        void ARGBAddRow_Any_NEON(const uint8_t* y_buf,
            const uint8_t* uv_buf,
            uint8_t* dst_ptr,
            int width);
        void ARGBAddRow_LSX(const uint8_t* src_argb0,
            const uint8_t* src_argb1,
            uint8_t* dst_argb,
            int width);
        void ARGBAddRow_LASX(const uint8_t* src_argb0,
            const uint8_t* src_argb1,
            uint8_t* dst_argb,
            int width);
        void ARGBAddRow_Any_LSX(const uint8_t* y_buf,
            const uint8_t* uv_buf,
            uint8_t* dst_ptr,
            int width);
        void ARGBAddRow_Any_LASX(const uint8_t* y_buf,
            const uint8_t* uv_buf,
            uint8_t* dst_ptr,
            int width);

        // ARGB subtract images. Same API as Blend, but these require
        // pointer and width alignment for SSE2.
        void ARGBSubtractRow_C(const uint8_t* src_argb,
            const uint8_t* src_argb1,
            uint8_t* dst_argb,
            int width);
        void ARGBSubtractRow_SSE2(const uint8_t* src_argb,
            const uint8_t* src_argb1,
            uint8_t* dst_argb,
            int width);
        void ARGBSubtractRow_Any_SSE2(const uint8_t* y_buf,
            const uint8_t* uv_buf,
            uint8_t* dst_ptr,
            int width);
        void ARGBSubtractRow_AVX2(const uint8_t* src_argb,
            const uint8_t* src_argb1,
            uint8_t* dst_argb,
            int width);
        void ARGBSubtractRow_Any_AVX2(const uint8_t* y_buf,
            const uint8_t* uv_buf,
            uint8_t* dst_ptr,
            int width);
        void ARGBSubtractRow_NEON(const uint8_t* src_argb,
            const uint8_t* src_argb1,
            uint8_t* dst_argb,
            int width);
        void ARGBSubtractRow_Any_NEON(const uint8_t* y_buf,
            const uint8_t* uv_buf,
            uint8_t* dst_ptr,
            int width);
        void ARGBSubtractRow_LSX(const uint8_t* src_argb0,
            const uint8_t* src_argb1,
            uint8_t* dst_argb,
            int width);
        void ARGBSubtractRow_LASX(const uint8_t* src_argb0,
            const uint8_t* src_argb1,
            uint8_t* dst_argb,
            int width);
        void ARGBSubtractRow_Any_LSX(const uint8_t* y_buf,
            const uint8_t* uv_buf,
            uint8_t* dst_ptr,
            int width);
        void ARGBSubtractRow_Any_LASX(const uint8_t* y_buf,
            const uint8_t* uv_buf,
            uint8_t* dst_ptr,
            int width);

        void ARGBToRGB24Row_Any_SSSE3(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void ARGBToRAWRow_Any_SSSE3(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void ARGBToRGB565Row_Any_SSE2(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void ARGBToARGB1555Row_Any_SSE2(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void ARGBToARGB4444Row_Any_SSE2(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void ABGRToAR30Row_Any_SSSE3(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void ARGBToAR30Row_Any_SSSE3(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void ARGBToRAWRow_Any_AVX2(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void ARGBToRGB24Row_Any_AVX2(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void ARGBToRGB24Row_Any_AVX512VBMI(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void ARGBToRGB565DitherRow_Any_SSE2(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            const uint32_t param,
            int width);
        void ARGBToRGB565DitherRow_Any_AVX2(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            const uint32_t param,
            int width);

        void ARGBToRGB565Row_Any_AVX2(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void ARGBToARGB1555Row_Any_AVX2(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void ARGBToARGB4444Row_Any_AVX2(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void ABGRToAR30Row_Any_AVX2(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void ARGBToAR30Row_Any_AVX2(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);

        void ARGBToRGB24Row_Any_NEON(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void ARGBToRAWRow_Any_NEON(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void ARGBToRGB565Row_Any_NEON(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void ARGBToARGB1555Row_Any_NEON(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void ARGBToARGB4444Row_Any_NEON(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void ARGBToRGB565DitherRow_Any_NEON(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            const uint32_t param,
            int width);
        void ARGBToRGB565DitherRow_Any_LSX(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            const uint32_t param,
            int width);
        void ARGBToRGB565DitherRow_Any_LASX(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            const uint32_t param,
            int width);
        void ARGBToRGB24Row_Any_LSX(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void ARGBToRGB24Row_Any_LASX(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void ARGBToRAWRow_Any_LSX(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void ARGBToRAWRow_Any_LASX(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void ARGBToRGB565Row_Any_LSX(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void ARGBToRGB565Row_Any_LASX(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void ARGBToARGB1555Row_Any_LSX(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void ARGBToARGB1555Row_Any_LASX(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void ARGBToARGB4444Row_Any_LSX(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void ARGBToARGB4444Row_Any_LASX(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);

        void I444ToARGBRow_Any_NEON(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I444ToRGB24Row_Any_NEON(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToARGBRow_Any_NEON(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I210ToARGBRow_Any_NEON(const uint16_t* y_buf,
            const uint16_t* u_buf,
            const uint16_t* v_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I410ToARGBRow_Any_NEON(const uint16_t* y_buf,
            const uint16_t* u_buf,
            const uint16_t* v_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I210ToAR30Row_Any_NEON(const uint16_t* y_buf,
            const uint16_t* u_buf,
            const uint16_t* v_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I410ToAR30Row_Any_NEON(const uint16_t* y_buf,
            const uint16_t* u_buf,
            const uint16_t* v_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I212ToARGBRow_Any_NEON(const uint16_t* y_buf,
            const uint16_t* u_buf,
            const uint16_t* v_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I212ToAR30Row_Any_NEON(const uint16_t* y_buf,
            const uint16_t* u_buf,
            const uint16_t* v_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I444AlphaToARGBRow_Any_NEON(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            const uint8_t* a_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422AlphaToARGBRow_Any_NEON(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            const uint8_t* a_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I410AlphaToARGBRow_Any_NEON(const uint16_t* y_buf,
            const uint16_t* u_buf,
            const uint16_t* v_buf,
            const uint16_t* a_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I210AlphaToARGBRow_Any_NEON(const uint16_t* y_buf,
            const uint16_t* u_buf,
            const uint16_t* v_buf,
            const uint16_t* a_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToRGBARow_Any_NEON(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToRGB24Row_Any_NEON(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToARGB4444Row_Any_NEON(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToARGB1555Row_Any_NEON(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToRGB565Row_Any_NEON(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToAR30Row_Any_NEON(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void NV12ToARGBRow_Any_NEON(const uint8_t* y_buf,
            const uint8_t* uv_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void NV21ToARGBRow_Any_NEON(const uint8_t* y_buf,
            const uint8_t* uv_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void NV12ToRGB24Row_Any_NEON(const uint8_t* y_buf,
            const uint8_t* uv_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void NV21ToRGB24Row_Any_NEON(const uint8_t* y_buf,
            const uint8_t* uv_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void NV21ToYUV24Row_Any_NEON(const uint8_t* y_buf,
            const uint8_t* uv_buf,
            uint8_t* dst_ptr,
            int width);
        void NV12ToRGB565Row_Any_NEON(const uint8_t* y_buf,
            const uint8_t* uv_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void YUY2ToARGBRow_Any_NEON(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void UYVYToARGBRow_Any_NEON(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void ARGBToAR30Row_NEON(const uint8_t* src, uint8_t* dst, int width);
        void ABGRToAR30Row_NEON(const uint8_t* src, uint8_t* dst, int width);
        void ABGRToAR30Row_Any_NEON(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void ARGBToAR30Row_Any_NEON(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void P210ToARGBRow_NEON(const uint16_t* y_buf,
            const uint16_t* uv_buf,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void P210ToARGBRow_SVE2(const uint16_t* y_buf,
            const uint16_t* uv_buf,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void P210ToARGBRow_SME(const uint16_t* y_buf,
            const uint16_t* uv_buf,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void P410ToARGBRow_NEON(const uint16_t* y_buf,
            const uint16_t* uv_buf,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void P410ToARGBRow_SVE2(const uint16_t* y_buf,
            const uint16_t* uv_buf,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void P410ToARGBRow_SME(const uint16_t* y_buf,
            const uint16_t* uv_buf,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void P210ToAR30Row_NEON(const uint16_t* y_buf,
            const uint16_t* uv_buf,
            uint8_t* dst_ar30,
            const struct YuvConstants* yuvconstants,
            int width);
        void P210ToAR30Row_SVE2(const uint16_t* y_buf,
            const uint16_t* uv_buf,
            uint8_t* dst_ar30,
            const struct YuvConstants* yuvconstants,
            int width);
        void P210ToAR30Row_SME(const uint16_t* y_buf,
            const uint16_t* uv_buf,
            uint8_t* dst_ar30,
            const struct YuvConstants* yuvconstants,
            int width);
        void P410ToAR30Row_NEON(const uint16_t* y_buf,
            const uint16_t* uv_buf,
            uint8_t* dst_ar30,
            const struct YuvConstants* yuvconstants,
            int width);
        void P410ToAR30Row_SVE2(const uint16_t* y_buf,
            const uint16_t* uv_buf,
            uint8_t* dst_ar30,
            const struct YuvConstants* yuvconstants,
            int width);
        void P410ToAR30Row_SME(const uint16_t* y_buf,
            const uint16_t* uv_buf,
            uint8_t* dst_ar30,
            const struct YuvConstants* yuvconstants,
            int width);
        void P210ToARGBRow_Any_NEON(const uint16_t* y_buf,
            const uint16_t* uv_buf,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void P410ToARGBRow_Any_NEON(const uint16_t* y_buf,
            const uint16_t* uv_buf,
            uint8_t* dst_argb,
            const struct YuvConstants* yuvconstants,
            int width);
        void P210ToAR30Row_Any_NEON(const uint16_t* y_buf,
            const uint16_t* uv_buf,
            uint8_t* dst_ar30,
            const struct YuvConstants* yuvconstants,
            int width);
        void P410ToAR30Row_Any_NEON(const uint16_t* y_buf,
            const uint16_t* uv_buf,
            uint8_t* dst_ar30,
            const struct YuvConstants* yuvconstants,
            int width);
        void I444ToARGBRow_Any_LSX(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToARGBRow_Any_LSX(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToARGBRow_Any_LASX(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToRGBARow_Any_LSX(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToRGBARow_Any_LASX(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422AlphaToARGBRow_Any_LSX(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            const uint8_t* a_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422AlphaToARGBRow_Any_LASX(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            const uint8_t* a_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToRGB24Row_Any_LSX(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToRGB24Row_Any_LASX(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToRGB565Row_Any_LSX(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToRGB565Row_Any_LASX(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToARGB4444Row_Any_LSX(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToARGB4444Row_Any_LASX(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToARGB1555Row_Any_LSX(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void I422ToARGB1555Row_Any_LASX(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);

        void NV12ToARGBRow_Any_LSX(const uint8_t* y_buf,
            const uint8_t* uv_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void NV12ToARGBRow_Any_LASX(const uint8_t* y_buf,
            const uint8_t* uv_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void NV12ToRGB565Row_Any_LSX(const uint8_t* y_buf,
            const uint8_t* uv_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void NV12ToRGB565Row_Any_LASX(const uint8_t* y_buf,
            const uint8_t* uv_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void NV21ToARGBRow_Any_LSX(const uint8_t* y_buf,
            const uint8_t* uv_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void NV21ToARGBRow_Any_LASX(const uint8_t* y_buf,
            const uint8_t* uv_buf,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void YUY2ToARGBRow_Any_LSX(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);
        void UYVYToARGBRow_Any_LSX(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            const struct YuvConstants* yuvconstants,
            int width);

        void YUY2ToYRow_AVX2(const uint8_t* src_yuy2, uint8_t* dst_y, int width);
        void YUY2ToUVRow_AVX2(const uint8_t* src_yuy2,
            int stride_yuy2,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void YUY2ToNVUVRow_AVX2(const uint8_t* src_yuy2,
            int stride_yuy2,
            uint8_t* dst_uv,
            int width);
        void YUY2ToUV422Row_AVX2(const uint8_t* src_yuy2,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void YUY2ToYRow_SSE2(const uint8_t* src_yuy2, uint8_t* dst_y, int width);
        void YUY2ToUVRow_SSE2(const uint8_t* src_yuy2,
            int stride_yuy2,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void YUY2ToNVUVRow_SSE2(const uint8_t* src_yuy2,
            int stride_yuy2,
            uint8_t* dst_uv,
            int width);
        void YUY2ToUV422Row_SSE2(const uint8_t* src_yuy2,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void YUY2ToYRow_NEON(const uint8_t* src_yuy2, uint8_t* dst_y, int width);
        void YUY2ToUVRow_NEON(const uint8_t* src_yuy2,
            int stride_yuy2,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void YUY2ToNVUVRow_NEON(const uint8_t* src_yuy2,
            int stride_yuy2,
            uint8_t* dst_uv,
            int width);
        void YUY2ToUV422Row_NEON(const uint8_t* src_yuy2,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void YUY2ToYRow_LSX(const uint8_t* src_yuy2, uint8_t* dst_y, int width);
        void YUY2ToYRow_LASX(const uint8_t* src_yuy2, uint8_t* dst_y, int width);
        void YUY2ToUVRow_LSX(const uint8_t* src_yuy2,
            int src_stride_yuy2,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void YUY2ToUVRow_LASX(const uint8_t* src_yuy2,
            int src_stride_yuy2,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void YUY2ToUV422Row_LSX(const uint8_t* src_yuy2,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void YUY2ToUV422Row_LASX(const uint8_t* src_yuy2,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void YUY2ToYRow_C(const uint8_t* src_yuy2, uint8_t* dst_y, int width);
        void YUY2ToUVRow_C(const uint8_t* src_yuy2,
            int src_stride_yuy2,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void YUY2ToNVUVRow_C(const uint8_t* src_yuy2,
            int src_stride_yuy2,
            uint8_t* dst_uv,
            int width);
        void YUY2ToUV422Row_C(const uint8_t* src_yuy2,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void YUY2ToYRow_Any_AVX2(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void YUY2ToUVRow_Any_AVX2(const uint8_t* src_ptr,
            int src_stride,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void YUY2ToNVUVRow_Any_AVX2(const uint8_t* src_yuy2,
            int stride_yuy2,
            uint8_t* dst_uv,
            int width);
        void YUY2ToUV422Row_Any_AVX2(const uint8_t* src_ptr,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void YUY2ToYRow_Any_SSE2(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void YUY2ToUVRow_Any_SSE2(const uint8_t* src_ptr,
            int src_stride,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void YUY2ToNVUVRow_Any_SSE2(const uint8_t* src_yuy2,
            int stride_yuy2,
            uint8_t* dst_uv,
            int width);
        void YUY2ToUV422Row_Any_SSE2(const uint8_t* src_ptr,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void YUY2ToYRow_Any_NEON(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void YUY2ToUVRow_Any_NEON(const uint8_t* src_ptr,
            int src_stride,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void YUY2ToNVUVRow_Any_NEON(const uint8_t* src_yuy2,
            int stride_yuy2,
            uint8_t* dst_uv,
            int width);
        void YUY2ToUV422Row_Any_NEON(const uint8_t* src_ptr,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void YUY2ToYRow_Any_LSX(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void YUY2ToYRow_Any_LASX(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void YUY2ToUVRow_Any_LSX(const uint8_t* src_ptr,
            int src_stride_ptr,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void YUY2ToUVRow_Any_LASX(const uint8_t* src_ptr,
            int src_stride_ptr,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void YUY2ToUV422Row_Any_LSX(const uint8_t* src_ptr,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void YUY2ToUV422Row_Any_LASX(const uint8_t* src_ptr,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void UYVYToYRow_AVX2(const uint8_t* src_uyvy, uint8_t* dst_y, int width);
        void UYVYToUVRow_AVX2(const uint8_t* src_uyvy,
            int stride_uyvy,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void UYVYToUV422Row_AVX2(const uint8_t* src_uyvy,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void UYVYToYRow_SSE2(const uint8_t* src_uyvy, uint8_t* dst_y, int width);
        void UYVYToUVRow_SSE2(const uint8_t* src_uyvy,
            int stride_uyvy,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void UYVYToUV422Row_SSE2(const uint8_t* src_uyvy,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void UYVYToYRow_AVX2(const uint8_t* src_uyvy, uint8_t* dst_y, int width);
        void UYVYToUVRow_AVX2(const uint8_t* src_uyvy,
            int stride_uyvy,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void UYVYToUV422Row_AVX2(const uint8_t* src_uyvy,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void UYVYToYRow_NEON(const uint8_t* src_uyvy, uint8_t* dst_y, int width);
        void UYVYToUVRow_NEON(const uint8_t* src_uyvy,
            int stride_uyvy,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void UYVYToUV422Row_NEON(const uint8_t* src_uyvy,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void UYVYToYRow_LSX(const uint8_t* src_uyvy, uint8_t* dst_y, int width);
        void UYVYToYRow_LASX(const uint8_t* src_uyvy, uint8_t* dst_y, int width);
        void UYVYToUVRow_LSX(const uint8_t* src_uyvy,
            int src_stride_uyvy,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void UYVYToUVRow_LASX(const uint8_t* src_uyvy,
            int src_stride_uyvy,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void UYVYToUV422Row_LSX(const uint8_t* src_uyvy,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void UYVYToUV422Row_LASX(const uint8_t* src_uyvy,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);

        void UYVYToYRow_C(const uint8_t* src_uyvy, uint8_t* dst_y, int width);
        void UYVYToUVRow_C(const uint8_t* src_uyvy,
            int src_stride_uyvy,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void UYVYToUV422Row_C(const uint8_t* src_uyvy,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void UYVYToYRow_Any_AVX2(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void UYVYToUVRow_Any_AVX2(const uint8_t* src_ptr,
            int src_stride,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void UYVYToUV422Row_Any_AVX2(const uint8_t* src_ptr,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void UYVYToYRow_Any_SSE2(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void UYVYToUVRow_Any_SSE2(const uint8_t* src_ptr,
            int src_stride,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void UYVYToUV422Row_Any_SSE2(const uint8_t* src_ptr,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void UYVYToYRow_Any_NEON(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void UYVYToUVRow_Any_NEON(const uint8_t* src_ptr,
            int src_stride,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void UYVYToUV422Row_Any_NEON(const uint8_t* src_ptr,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void UYVYToYRow_Any_LSX(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void UYVYToYRow_Any_LASX(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void UYVYToUVRow_Any_LSX(const uint8_t* src_ptr,
            int src_stride_ptr,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void UYVYToUVRow_Any_LASX(const uint8_t* src_ptr,
            int src_stride_ptr,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void UYVYToUV422Row_Any_LSX(const uint8_t* src_ptr,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void UYVYToUV422Row_Any_LASX(const uint8_t* src_ptr,
            uint8_t* dst_u,
            uint8_t* dst_v,
            int width);
        void SwapUVRow_C(const uint8_t* src_uv, uint8_t* dst_vu, int width);
        void SwapUVRow_NEON(const uint8_t* src_uv, uint8_t* dst_vu, int width);
        void SwapUVRow_Any_NEON(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void SwapUVRow_SSSE3(const uint8_t* src_uv, uint8_t* dst_vu, int width);
        void SwapUVRow_Any_SSSE3(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void SwapUVRow_AVX2(const uint8_t* src_uv, uint8_t* dst_vu, int width);
        void SwapUVRow_Any_AVX2(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void AYUVToYRow_C(const uint8_t* src_ayuv, uint8_t* dst_y, int width);
        void AYUVToUVRow_C(const uint8_t* src_ayuv,
            int src_stride_ayuv,
            uint8_t* dst_uv,
            int width);
        void AYUVToVURow_C(const uint8_t* src_ayuv,
            int src_stride_ayuv,
            uint8_t* dst_vu,
            int width);
        void AYUVToYRow_NEON(const uint8_t* src_ayuv, uint8_t* dst_y, int width);
        void AYUVToUVRow_NEON(const uint8_t* src_ayuv,
            int src_stride_ayuv,
            uint8_t* dst_uv,
            int width);
        void AYUVToUVRow_SVE2(const uint8_t* src_ayuv,
            int src_stride_ayuv,
            uint8_t* dst_uv,
            int width);
        void AYUVToVURow_NEON(const uint8_t* src_ayuv,
            int src_stride_ayuv,
            uint8_t* dst_vu,
            int width);
        void AYUVToVURow_SVE2(const uint8_t* src_ayuv,
            int src_stride_ayuv,
            uint8_t* dst_vu,
            int width);
        void AYUVToYRow_Any_NEON(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
        void AYUVToUVRow_Any_NEON(const uint8_t* src_ptr,
            int src_stride,
            uint8_t* dst_vu,
            int width);
        void AYUVToUVRow_Any_SVE2(const uint8_t* src_ptr,
            int src_stride,
            uint8_t* dst_vu,
            int width);
        void AYUVToVURow_Any_NEON(const uint8_t* src_ptr,
            int src_stride,
            uint8_t* dst_vu,
            int width);
        void AYUVToVURow_Any_SVE2(const uint8_t* src_ptr,
            int src_stride,
            uint8_t* dst_vu,
            int width);

        void I422ToYUY2Row_C(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_frame,
            int width);
        void I422ToUYVYRow_C(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_frame,
            int width);
        void I422ToYUY2Row_SSE2(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_yuy2,
            int width);
        void I422ToUYVYRow_SSE2(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_uyvy,
            int width);
        void I422ToYUY2Row_Any_SSE2(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_ptr,
            int width);
        void I422ToUYVYRow_Any_SSE2(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_ptr,
            int width);
        void I422ToYUY2Row_AVX2(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_yuy2,
            int width);
        void I422ToUYVYRow_AVX2(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_uyvy,
            int width);
        void I422ToYUY2Row_Any_AVX2(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_ptr,
            int width);
        void I422ToUYVYRow_Any_AVX2(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_ptr,
            int width);
        void I422ToYUY2Row_NEON(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_yuy2,
            int width);
        void I422ToUYVYRow_NEON(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_uyvy,
            int width);
        void I422ToYUY2Row_Any_NEON(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_ptr,
            int width);
        void I422ToUYVYRow_Any_NEON(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_ptr,
            int width);
        void I422ToYUY2Row_LSX(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_yuy2,
            int width);
        void I422ToYUY2Row_LASX(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_yuy2,
            int width);
        void I422ToUYVYRow_LSX(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_uyvy,
            int width);
        void I422ToUYVYRow_LASX(const uint8_t* src_y,
            const uint8_t* src_u,
            const uint8_t* src_v,
            uint8_t* dst_uyvy,
            int width);
        void I422ToYUY2Row_Any_LSX(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_ptr,
            int width);
        void I422ToYUY2Row_Any_LASX(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_ptr,
            int width);
        void I422ToUYVYRow_Any_LSX(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_ptr,
            int width);
        void I422ToUYVYRow_Any_LASX(const uint8_t* y_buf,
            const uint8_t* u_buf,
            const uint8_t* v_buf,
            uint8_t* dst_ptr,
            int width);

        // Effects related row functions.
        void ARGBAttenuateRow_C(const uint8_t* src_argb, uint8_t* dst_argb, int width);
        void ARGBAttenuateRow_SSSE3(const uint8_t* src_argb,
            uint8_t* dst_argb,
            int width);
        void ARGBAttenuateRow_AVX2(const uint8_t* src_argb,
            uint8_t* dst_argb,
            int width);
        void ARGBAttenuateRow_NEON(const uint8_t* src_argb,
            uint8_t* dst_argb,
            int width);
        void ARGBAttenuateRow_LSX(const uint8_t* src_argb,
            uint8_t* dst_argb,
            int width);
        void ARGBAttenuateRow_LASX(const uint8_t* src_argb,
            uint8_t* dst_argb,
            int width);
        void ARGBAttenuateRow_RVV(const uint8_t* src_argb,
            uint8_t* dst_argb,
            int width);
        void ARGBAttenuateRow_Any_SSSE3(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void ARGBAttenuateRow_Any_AVX2(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void ARGBAttenuateRow_Any_NEON(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void ARGBAttenuateRow_Any_LSX(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void ARGBAttenuateRow_Any_LASX(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);

        // Inverse table for unattenuate, shared by C and SSE2.
        extern const uint32_t fixed_invtbl8[256];
        void ARGBUnattenuateRow_C(const uint8_t* src_argb,
            uint8_t* dst_argb,
            int width);
        void ARGBUnattenuateRow_SSE2(const uint8_t* src_argb,
            uint8_t* dst_argb,
            int width);
        void ARGBUnattenuateRow_AVX2(const uint8_t* src_argb,
            uint8_t* dst_argb,
            int width);
        void ARGBUnattenuateRow_Any_SSE2(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);
        void ARGBUnattenuateRow_Any_AVX2(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int width);

        void ARGBGrayRow_C(const uint8_t* src_argb, uint8_t* dst_argb, int width);
        void ARGBGrayRow_SSSE3(const uint8_t* src_argb, uint8_t* dst_argb, int width);
        void ARGBGrayRow_NEON(const uint8_t* src_argb, uint8_t* dst_argb, int width);
        void ARGBGrayRow_NEON_DotProd(const uint8_t* src_argb,
            uint8_t* dst_argb,
            int width);
        void ARGBGrayRow_LSX(const uint8_t* src_argb, uint8_t* dst_argb, int width);
        void ARGBGrayRow_LASX(const uint8_t* src_argb, uint8_t* dst_argb, int width);

        void ARGBSepiaRow_C(uint8_t* dst_argb, int width);
        void ARGBSepiaRow_SSSE3(uint8_t* dst_argb, int width);
        void ARGBSepiaRow_NEON(uint8_t* dst_argb, int width);
        void ARGBSepiaRow_NEON_DotProd(uint8_t* dst_argb, int width);
        void ARGBSepiaRow_LSX(uint8_t* dst_argb, int width);
        void ARGBSepiaRow_LASX(uint8_t* dst_argb, int width);

        void ARGBColorMatrixRow_C(const uint8_t* src_argb,
            uint8_t* dst_argb,
            const int8_t* matrix_argb,
            int width);
        void ARGBColorMatrixRow_SSSE3(const uint8_t* src_argb,
            uint8_t* dst_argb,
            const int8_t* matrix_argb,
            int width);
        void ARGBColorMatrixRow_NEON(const uint8_t* src_argb,
            uint8_t* dst_argb,
            const int8_t* matrix_argb,
            int width);
        void ARGBColorMatrixRow_NEON_I8MM(const uint8_t* src_argb,
            uint8_t* dst_argb,
            const int8_t* matrix_argb,
            int width);
        void ARGBColorMatrixRow_LSX(const uint8_t* src_argb,
            uint8_t* dst_argb,
            const int8_t* matrix_argb,
            int width);

        void ARGBColorTableRow_C(uint8_t* dst_argb,
            const uint8_t* table_argb,
            int width);
        void ARGBColorTableRow_X86(uint8_t* dst_argb,
            const uint8_t* table_argb,
            int width);

        void RGBColorTableRow_C(uint8_t* dst_argb,
            const uint8_t* table_argb,
            int width);
        void RGBColorTableRow_X86(uint8_t* dst_argb,
            const uint8_t* table_argb,
            int width);

        void ARGBQuantizeRow_C(uint8_t* dst_argb,
            int scale,
            int interval_size,
            int interval_offset,
            int width);
        void ARGBQuantizeRow_SSE2(uint8_t* dst_argb,
            int scale,
            int interval_size,
            int interval_offset,
            int width);
        void ARGBQuantizeRow_NEON(uint8_t* dst_argb,
            int scale,
            int interval_size,
            int interval_offset,
            int width);
        void ARGBQuantizeRow_LSX(uint8_t* dst_argb,
            int scale,
            int interval_size,
            int interval_offset,
            int width);

        void ARGBShadeRow_C(const uint8_t* src_argb,
            uint8_t* dst_argb,
            int width,
            uint32_t value);
        void ARGBShadeRow_SSE2(const uint8_t* src_argb,
            uint8_t* dst_argb,
            int width,
            uint32_t value);
        void ARGBShadeRow_NEON(const uint8_t* src_argb,
            uint8_t* dst_argb,
            int width,
            uint32_t value);
        void ARGBShadeRow_LSX(const uint8_t* src_argb,
            uint8_t* dst_argb,
            int width,
            uint32_t value);
        void ARGBShadeRow_LASX(const uint8_t* src_argb,
            uint8_t* dst_argb,
            int width,
            uint32_t value);

        // Used for blur.
        void CumulativeSumToAverageRow_SSE2(const int32_t* topleft,
            const int32_t* botleft,
            int width,
            int area,
            uint8_t* dst,
            int count);
        void ComputeCumulativeSumRow_SSE2(const uint8_t* row,
            int32_t* cumsum,
            const int32_t* previous_cumsum,
            int width);

        void CumulativeSumToAverageRow_C(const int32_t* tl,
            const int32_t* bl,
            int w,
            int area,
            uint8_t* dst,
            int count);
        void ComputeCumulativeSumRow_C(const uint8_t* row,
            int32_t* cumsum,
            const int32_t* previous_cumsum,
            int width);

        LIBYUV_API
            void ARGBAffineRow_C(const uint8_t* src_argb,
                int src_argb_stride,
                uint8_t* dst_argb,
                const float* uv_dudv,
                int width);
        LIBYUV_API
            void ARGBAffineRow_SSE2(const uint8_t* src_argb,
                int src_argb_stride,
                uint8_t* dst_argb,
                const float* src_dudv,
                int width);

        // Used for I420Scale, ARGBScale, and ARGBInterpolate.
        void InterpolateRow_C(uint8_t* dst_ptr,
            const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            int width,
            int source_y_fraction);
        void InterpolateRow_SSSE3(uint8_t* dst_ptr,
            const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            int dst_width,
            int source_y_fraction);
        void InterpolateRow_AVX2(uint8_t* dst_ptr,
            const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            int dst_width,
            int source_y_fraction);
        void InterpolateRow_NEON(uint8_t* dst_ptr,
            const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            int dst_width,
            int source_y_fraction);
        void InterpolateRow_SME(uint8_t* dst_ptr,
            const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            int dst_width,
            int source_y_fraction);
        void InterpolateRow_LSX(uint8_t* dst_ptr,
            const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            int width,
            int source_y_fraction);
        void InterpolateRow_RVV(uint8_t* dst_ptr,
            const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            int width,
            int source_y_fraction);
        void InterpolateRow_Any_NEON(uint8_t* dst_ptr,
            const uint8_t* src_ptr,
            ptrdiff_t src_stride_ptr,
            int width,
            int source_y_fraction);
        void InterpolateRow_Any_SSSE3(uint8_t* dst_ptr,
            const uint8_t* src_ptr,
            ptrdiff_t src_stride_ptr,
            int width,
            int source_y_fraction);
        void InterpolateRow_Any_AVX2(uint8_t* dst_ptr,
            const uint8_t* src_ptr,
            ptrdiff_t src_stride_ptr,
            int width,
            int source_y_fraction);
        void InterpolateRow_Any_LSX(uint8_t* dst_ptr,
            const uint8_t* src_ptr,
            ptrdiff_t src_stride_ptr,
            int width,
            int source_y_fraction);

        void InterpolateRow_16_C(uint16_t* dst_ptr,
            const uint16_t* src_ptr,
            ptrdiff_t src_stride,
            int width,
            int source_y_fraction);
        void InterpolateRow_16_NEON(uint16_t* dst_ptr,
            const uint16_t* src_ptr,
            ptrdiff_t src_stride,
            int width,
            int source_y_fraction);
        void InterpolateRow_16_Any_NEON(uint16_t* dst_ptr,
            const uint16_t* src_ptr,
            ptrdiff_t src_stride,
            int width,
            int source_y_fraction);
        void InterpolateRow_16_SME(uint16_t* dst_ptr,
            const uint16_t* src_ptr,
            ptrdiff_t src_stride,
            int width,
            int source_y_fraction);

        void InterpolateRow_16To8_C(uint8_t* dst_ptr,
            const uint16_t* src_ptr,
            ptrdiff_t src_stride,
            int scale,
            int width,
            int source_y_fraction);
        void InterpolateRow_16To8_NEON(uint8_t* dst_ptr,
            const uint16_t* src_ptr,
            ptrdiff_t src_stride,
            int scale,
            int width,
            int source_y_fraction);
        void InterpolateRow_16To8_Any_NEON(uint8_t* dst_ptr,
            const uint16_t* src_ptr,
            ptrdiff_t src_stride,
            int scale,
            int width,
            int source_y_fraction);
        void InterpolateRow_16To8_SME(uint8_t* dst_ptr,
            const uint16_t* src_ptr,
            ptrdiff_t src_stride,
            int scale,
            int width,
            int source_y_fraction);
        void InterpolateRow_16To8_AVX2(uint8_t* dst_ptr,
            const uint16_t* src_ptr,
            ptrdiff_t src_stride,
            int scale,
            int width,
            int source_y_fraction);
        void InterpolateRow_16To8_Any_AVX2(uint8_t* dst_ptr,
            const uint16_t* src_ptr,
            ptrdiff_t src_stride,
            int scale,
            int width,
            int source_y_fraction);

        // Sobel images.
        void SobelXRow_C(const uint8_t* src_y0,
            const uint8_t* src_y1,
            const uint8_t* src_y2,
            uint8_t* dst_sobelx,
            int width);
        void SobelXRow_SSE2(const uint8_t* src_y0,
            const uint8_t* src_y1,
            const uint8_t* src_y2,
            uint8_t* dst_sobelx,
            int width);
        void SobelXRow_NEON(const uint8_t* src_y0,
            const uint8_t* src_y1,
            const uint8_t* src_y2,
            uint8_t* dst_sobelx,
            int width);
        void SobelYRow_C(const uint8_t* src_y0,
            const uint8_t* src_y1,
            uint8_t* dst_sobely,
            int width);
        void SobelYRow_SSE2(const uint8_t* src_y0,
            const uint8_t* src_y1,
            uint8_t* dst_sobely,
            int width);
        void SobelYRow_NEON(const uint8_t* src_y0,
            const uint8_t* src_y1,
            uint8_t* dst_sobely,
            int width);
        void SobelRow_C(const uint8_t* src_sobelx,
            const uint8_t* src_sobely,
            uint8_t* dst_argb,
            int width);
        void SobelRow_SSE2(const uint8_t* src_sobelx,
            const uint8_t* src_sobely,
            uint8_t* dst_argb,
            int width);
        void SobelRow_NEON(const uint8_t* src_sobelx,
            const uint8_t* src_sobely,
            uint8_t* dst_argb,
            int width);
        void SobelRow_LSX(const uint8_t* src_sobelx,
            const uint8_t* src_sobely,
            uint8_t* dst_argb,
            int width);
        void SobelToPlaneRow_C(const uint8_t* src_sobelx,
            const uint8_t* src_sobely,
            uint8_t* dst_y,
            int width);
        void SobelToPlaneRow_SSE2(const uint8_t* src_sobelx,
            const uint8_t* src_sobely,
            uint8_t* dst_y,
            int width);
        void SobelToPlaneRow_NEON(const uint8_t* src_sobelx,
            const uint8_t* src_sobely,
            uint8_t* dst_y,
            int width);
        void SobelToPlaneRow_LSX(const uint8_t* src_sobelx,
            const uint8_t* src_sobely,
            uint8_t* dst_y,
            int width);
        void SobelXYRow_C(const uint8_t* src_sobelx,
            const uint8_t* src_sobely,
            uint8_t* dst_argb,
            int width);
        void SobelXYRow_SSE2(const uint8_t* src_sobelx,
            const uint8_t* src_sobely,
            uint8_t* dst_argb,
            int width);
        void SobelXYRow_NEON(const uint8_t* src_sobelx,
            const uint8_t* src_sobely,
            uint8_t* dst_argb,
            int width);
        void SobelXYRow_LSX(const uint8_t* src_sobelx,
            const uint8_t* src_sobely,
            uint8_t* dst_argb,
            int width);
        void SobelRow_Any_SSE2(const uint8_t* y_buf,
            const uint8_t* uv_buf,
            uint8_t* dst_ptr,
            int width);
        void SobelRow_Any_NEON(const uint8_t* y_buf,
            const uint8_t* uv_buf,
            uint8_t* dst_ptr,
            int width);
        void SobelRow_Any_LSX(const uint8_t* y_buf,
            const uint8_t* uv_buf,
            uint8_t* dst_ptr,
            int width);
        void SobelToPlaneRow_Any_SSE2(const uint8_t* y_buf,
            const uint8_t* uv_buf,
            uint8_t* dst_ptr,
            int width);
        void SobelToPlaneRow_Any_NEON(const uint8_t* y_buf,
            const uint8_t* uv_buf,
            uint8_t* dst_ptr,
            int width);
        void SobelToPlaneRow_Any_LSX(const uint8_t* y_buf,
            const uint8_t* uv_buf,
            uint8_t* dst_ptr,
            int width);
        void SobelXYRow_Any_SSE2(const uint8_t* y_buf,
            const uint8_t* uv_buf,
            uint8_t* dst_ptr,
            int width);
        void SobelXYRow_Any_NEON(const uint8_t* y_buf,
            const uint8_t* uv_buf,
            uint8_t* dst_ptr,
            int width);
        void SobelXYRow_Any_LSX(const uint8_t* y_buf,
            const uint8_t* uv_buf,
            uint8_t* dst_ptr,
            int width);

        void ARGBPolynomialRow_C(const uint8_t* src_argb,
            uint8_t* dst_argb,
            const float* poly,
            int width);
        void ARGBPolynomialRow_SSE2(const uint8_t* src_argb,
            uint8_t* dst_argb,
            const float* poly,
            int width);
        void ARGBPolynomialRow_AVX2(const uint8_t* src_argb,
            uint8_t* dst_argb,
            const float* poly,
            int width);

        // Scale and convert to half float.
        void HalfFloatRow_C(const uint16_t* src, uint16_t* dst, float scale, int width);
        void HalfFloatRow_SSE2(const uint16_t* src,
            uint16_t* dst,
            float scale,
            int width);
        void HalfFloatRow_Any_SSE2(const uint16_t* src_ptr,
            uint16_t* dst_ptr,
            float param,
            int width);
        void HalfFloatRow_AVX2(const uint16_t* src,
            uint16_t* dst,
            float scale,
            int width);
        void HalfFloatRow_Any_AVX2(const uint16_t* src_ptr,
            uint16_t* dst_ptr,
            float param,
            int width);
        void HalfFloatRow_F16C(const uint16_t* src,
            uint16_t* dst,
            float scale,
            int width);
        void HalfFloatRow_Any_F16C(const uint16_t* src,
            uint16_t* dst,
            float scale,
            int width);
        void HalfFloat1Row_F16C(const uint16_t* src,
            uint16_t* dst,
            float scale,
            int width);
        void HalfFloat1Row_Any_F16C(const uint16_t* src,
            uint16_t* dst,
            float scale,
            int width);
        void HalfFloatRow_NEON(const uint16_t* src,
            uint16_t* dst,
            float scale,
            int width);
        void HalfFloatRow_Any_NEON(const uint16_t* src_ptr,
            uint16_t* dst_ptr,
            float param,
            int width);
        void HalfFloatRow_SVE2(const uint16_t* src,
            uint16_t* dst,
            float scale,
            int width);
        void HalfFloat1Row_SVE2(const uint16_t* src,
            uint16_t* dst,
            float scale,
            int width);
        void HalfFloatRow_LSX(const uint16_t* src,
            uint16_t* dst,
            float scale,
            int width);
        void HalfFloatRow_Any_LSX(const uint16_t* src_ptr,
            uint16_t* dst_ptr,
            float param,
            int width);
        void ByteToFloatRow_C(const uint8_t* src, float* dst, float scale, int width);
        void ByteToFloatRow_NEON(const uint8_t* src,
            float* dst,
            float scale,
            int width);
        void ByteToFloatRow_Any_NEON(const uint8_t* src_ptr,
            float* dst_ptr,
            float param,
            int width);
        // Convert FP16 Half Floats to FP32 Floats
        void ConvertFP16ToFP32Row_NEON(const uint16_t* src,  // fp16
            float* dst,
            int width);
        // Convert a column of FP16 Half Floats to a row of FP32 Floats
        void ConvertFP16ToFP32Column_NEON(const uint16_t* src,  // fp16
            int src_stride,       // stride in elements
            float* dst,
            int width);
        // Convert FP32 Floats to FP16 Half Floats
        void ConvertFP32ToFP16Row_NEON(const float* src,
            uint16_t* dst,  // fp16
            int width);
        void ARGBLumaColorTableRow_C(const uint8_t* src_argb,
            uint8_t* dst_argb,
            int width,
            const uint8_t* luma,
            uint32_t lumacoeff);
        void ARGBLumaColorTableRow_SSSE3(const uint8_t* src_argb,
            uint8_t* dst_argb,
            int width,
            const uint8_t* luma,
            uint32_t lumacoeff);

        float ScaleMaxSamples_C(const float* src, float* dst, float scale, int width);
        float ScaleMaxSamples_NEON(const float* src,
            float* dst,
            float scale,
            int width);
        float ScaleSumSamples_C(const float* src, float* dst, float scale, int width);
        float ScaleSumSamples_NEON(const float* src,
            float* dst,
            float scale,
            int width);
        void ScaleSamples_C(const float* src, float* dst, float scale, int width);
        void ScaleSamples_NEON(const float* src, float* dst, float scale, int width);

        void GaussRow_F32_NEON(const float* src, float* dst, int width);
        void GaussRow_F32_C(const float* src, float* dst, int width);

        void GaussCol_F32_NEON(const float* src0,
            const float* src1,
            const float* src2,
            const float* src3,
            const float* src4,
            float* dst,
            int width);

        void GaussCol_F32_C(const float* src0,
            const float* src1,
            const float* src2,
            const float* src3,
            const float* src4,
            float* dst,
            int width);

        void GaussRow_C(const uint32_t* src, uint16_t* dst, int width);
        void GaussRow_NEON(const uint32_t* src, uint16_t* dst, int width);
        void GaussCol_C(const uint16_t* src0,
            const uint16_t* src1,
            const uint16_t* src2,
            const uint16_t* src3,
            const uint16_t* src4,
            uint32_t* dst,
            int width);
        void GaussCol_NEON(const uint16_t* src0,
            const uint16_t* src1,
            const uint16_t* src2,
            const uint16_t* src3,
            const uint16_t* src4,
            uint32_t* dst,
            int width);

        void ClampFloatToZero_SSE2(const float* src_x, float* dst_y, int width);

#ifdef __cplusplus
    }  // extern "C"
}  // namespace libyuv
#endif


#ifdef __cplusplus
namespace libyuv {
    extern "C" {
#endif

        LIBYUV_API
            int ARGBScale(const uint8_t* src_argb,
                int src_stride_argb,
                int src_width,
                int src_height,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int dst_width,
                int dst_height,
                enum FilterMode filtering);

        // Clipped scale takes destination rectangle coordinates for clip values.
        LIBYUV_API
            int ARGBScaleClip(const uint8_t* src_argb,
                int src_stride_argb,
                int src_width,
                int src_height,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int dst_width,
                int dst_height,
                int clip_x,
                int clip_y,
                int clip_width,
                int clip_height,
                enum FilterMode filtering);

        // Scale with YUV conversion to ARGB and clipping.
        LIBYUV_API
            int YUVToARGBScaleClip(const uint8_t* src_y,
                int src_stride_y,
                const uint8_t* src_u,
                int src_stride_u,
                const uint8_t* src_v,
                int src_stride_v,
                uint32_t src_fourcc,
                int src_width,
                int src_height,
                uint8_t* dst_argb,
                int dst_stride_argb,
                uint32_t dst_fourcc,
                int dst_width,
                int dst_height,
                int clip_x,
                int clip_y,
                int clip_width,
                int clip_height,
                enum FilterMode filtering);

#ifdef __cplusplus
    }  // extern "C"
}  // namespace libyuv
#endif



#ifdef __cplusplus
namespace libyuv {
    extern "C" {
#endif

        // The following are available on all x86 platforms:
#if !defined(LIBYUV_DISABLE_X86) &&  !defined(__clang__)  &&                             \
    (defined(_M_IX86) ||                                        \
     (defined(__x86_64__) && !defined(LIBYUV_ENABLE_ROWWIN)) || \
     defined(__i386__))
#define HAS_FIXEDDIV1_X86
#define HAS_FIXEDDIV_X86
#define HAS_SCALEADDROW_SSE2
#define HAS_SCALEARGBCOLS_SSE2
#define HAS_SCALEARGBCOLSUP2_SSE2
#define HAS_SCALEARGBFILTERCOLS_SSSE3
#define HAS_SCALEARGBROWDOWN2_SSE2
#define HAS_SCALEARGBROWDOWNEVEN_SSE2
#define HAS_SCALECOLSUP2_SSE2
#define HAS_SCALEFILTERCOLS_SSSE3
#define HAS_SCALEROWDOWN2_SSSE3
#define HAS_SCALEROWDOWN34_SSSE3
#define HAS_SCALEROWDOWN38_SSSE3
#define HAS_SCALEROWDOWN4_SSSE3
#endif

// The following are available for gcc/clang x86 platforms:
// TODO(fbarchard): Port to Visual C
#if !defined(LIBYUV_DISABLE_X86)&&  !defined(__clang__)  &&               \
    (defined(__x86_64__) || defined(__i386__)) && \
    !defined(LIBYUV_ENABLE_ROWWIN)
#define HAS_SCALEUVROWDOWN2BOX_SSSE3
#define HAS_SCALEROWUP2_LINEAR_SSE2
#define HAS_SCALEROWUP2_LINEAR_SSSE3
#define HAS_SCALEROWUP2_BILINEAR_SSE2
#define HAS_SCALEROWUP2_BILINEAR_SSSE3
#define HAS_SCALEROWUP2_LINEAR_12_SSSE3
#define HAS_SCALEROWUP2_BILINEAR_12_SSSE3
#define HAS_SCALEROWUP2_LINEAR_16_SSE2
#define HAS_SCALEROWUP2_BILINEAR_16_SSE2
#define HAS_SCALEUVROWUP2_LINEAR_SSSE3
#define HAS_SCALEUVROWUP2_BILINEAR_SSSE3
#define HAS_SCALEUVROWUP2_LINEAR_16_SSE41
#define HAS_SCALEUVROWUP2_BILINEAR_16_SSE41
#endif

// The following are available for gcc/clang x86 platforms, but
// require clang 3.4 or gcc 4.7.
// TODO(fbarchard): Port to Visual C
#if !defined(LIBYUV_DISABLE_X86)&&  !defined(__clang__)  &&               \
    (defined(__x86_64__) || defined(__i386__)) && \
    (defined(CLANG_HAS_AVX2) || defined(GCC_HAS_AVX2))
#define HAS_SCALEUVROWDOWN2BOX_AVX2
#define HAS_SCALEROWUP2_LINEAR_AVX2
#define HAS_SCALEROWUP2_BILINEAR_AVX2
#define HAS_SCALEROWUP2_LINEAR_12_AVX2
#define HAS_SCALEROWUP2_BILINEAR_12_AVX2
#define HAS_SCALEROWUP2_LINEAR_16_AVX2
#define HAS_SCALEROWUP2_BILINEAR_16_AVX2
#define HAS_SCALEUVROWUP2_LINEAR_AVX2
#define HAS_SCALEUVROWUP2_BILINEAR_AVX2
#define HAS_SCALEUVROWUP2_LINEAR_16_AVX2
#define HAS_SCALEUVROWUP2_BILINEAR_16_AVX2
#endif

// The following are available on all x86 platforms, but
// require VS2012, clang 3.4 or gcc 4.7.
// The code supports NaCL but requires a new compiler and validator.
#if !defined(LIBYUV_DISABLE_X86)&&  !defined(__clang__)  &&                          \
    (defined(VISUALC_HAS_AVX2) || defined(CLANG_HAS_AVX2) || \
     defined(GCC_HAS_AVX2))
#define HAS_SCALEADDROW_AVX2
#define HAS_SCALEROWDOWN2_AVX2
#define HAS_SCALEROWDOWN4_AVX2
#endif

// The following are available on Neon platforms:
#if !defined(LIBYUV_DISABLE_NEON) && \
    (defined(__ARM_NEON__) || defined(LIBYUV_NEON) || defined(__aarch64__))
#define HAS_SCALEADDROW_NEON
#define HAS_SCALEARGBCOLS_NEON
#define HAS_SCALEARGBFILTERCOLS_NEON
#define HAS_SCALEARGBROWDOWN2_NEON
#define HAS_SCALEARGBROWDOWNEVEN_NEON
#define HAS_SCALEFILTERCOLS_NEON
#define HAS_SCALEROWDOWN2_NEON
#define HAS_SCALEROWDOWN34_NEON
#define HAS_SCALEROWDOWN38_NEON
#define HAS_SCALEROWDOWN4_NEON
#define HAS_SCALEUVROWDOWN2_NEON
#define HAS_SCALEUVROWDOWN2LINEAR_NEON
#define HAS_SCALEUVROWDOWN2BOX_NEON
#define HAS_SCALEUVROWDOWNEVEN_NEON
#define HAS_SCALEROWUP2_LINEAR_NEON
#define HAS_SCALEROWUP2_BILINEAR_NEON
#define HAS_SCALEROWUP2_LINEAR_12_NEON
#define HAS_SCALEROWUP2_BILINEAR_12_NEON
#define HAS_SCALEROWUP2_LINEAR_16_NEON
#define HAS_SCALEROWUP2_BILINEAR_16_NEON
#define HAS_SCALEUVROWUP2_LINEAR_NEON
#define HAS_SCALEUVROWUP2_BILINEAR_NEON
#define HAS_SCALEUVROWUP2_LINEAR_16_NEON
#define HAS_SCALEUVROWUP2_BILINEAR_16_NEON
#endif

// The following are available on AArch64 Neon platforms:
#if !defined(LIBYUV_DISABLE_NEON) && defined(__aarch64__)
#define HAS_SCALEROWDOWN2_16_NEON
#endif

// The following are available on AArch64 SME platforms:
#if !defined(LIBYUV_DISABLE_SME) && defined(CLANG_HAS_SME) && \
    defined(__aarch64__)
#define HAS_SCALEARGBROWDOWN2_SME
#define HAS_SCALEROWDOWN2_16_SME
#define HAS_SCALEROWDOWN2_SME
#define HAS_SCALEUVROWDOWN2BOX_SME
#define HAS_SCALEUVROWDOWN2LINEAR_SME
#define HAS_SCALEUVROWDOWN2_SME
#endif

#if !defined(LIBYUV_DISABLE_LSX) && defined(__loongarch_sx)
#define HAS_SCALEARGBROWDOWN2_LSX
#define HAS_SCALEARGBROWDOWNEVEN_LSX
#define HAS_SCALEROWDOWN2_LSX
#define HAS_SCALEROWDOWN4_LSX
#define HAS_SCALEROWDOWN38_LSX
#define HAS_SCALEFILTERCOLS_LSX
#define HAS_SCALEADDROW_LSX
#define HAS_SCALEARGBCOLS_LSX
#define HAS_SCALEARGBFILTERCOLS_LSX
#define HAS_SCALEROWDOWN34_LSX
#endif

// The following are available on RVV with 64 bit elements
// TODO: Update compiler to support 64 bit
#if !defined(LIBYUV_DISABLE_RVV) && defined(__riscv_vector) && \
    defined(__riscv_zve64x)
#define HAS_SCALEUVROWDOWN4_RVV
#define HAS_SCALEARGBROWDOWN2_RVV
#endif

#if !defined(LIBYUV_DISABLE_RVV) && defined(__riscv_vector) && \
    defined(__riscv_v_intrinsic)
// The following are available on RVV v0.11 and RVV v1.0
// TODO: Port to RVV v0.12
#if __riscv_v_intrinsic == 11000 || __riscv_v_intrinsic >= 100000
#define HAS_SCALEROWDOWN34_0_BOX_RVV
#define HAS_SCALEROWDOWN34_1_BOX_RVV
#define HAS_SCALEROWDOWN38_2_BOX_RVV
#define HAS_SCALEROWDOWN38_3_BOX_RVV
#define HAS_SCALEUVROWUP2_BILINEAR_RVV
#define HAS_SCALEUVROWUP2_LINEAR_RVV
#define HAS_SCALEROWDOWN34_RVV
#define HAS_SCALEROWDOWN38_RVV
#define HAS_SCALEROWUP2_BILINEAR_RVV
#define HAS_SCALEROWUP2_LINEAR_RVV

#define HAS_SCALEARGBROWDOWN2BOX_RVV
#define HAS_SCALEARGBROWDOWN2LINEAR_RVV
#define HAS_SCALEARGBROWDOWNEVENBOX_RVV
#define HAS_SCALEROWDOWN2BOX_RVV
#define HAS_SCALEADDROW_RVV
// TODO: Test ScaleARGBRowDownEven_RVV and enable it
// #define HAS_SCALEARGBROWDOWNEVEN_RVV
#define HAS_SCALEUVROWDOWNEVEN_RVV
#define HAS_SCALEROWDOWN2_RVV
#define HAS_SCALEROWDOWN2LINEAR_RVV
#define HAS_SCALEROWDOWN4_RVV
#define HAS_SCALEROWDOWN4BOX_RVV
#define HAS_SCALEUVROWDOWN2_RVV
#define HAS_SCALEUVROWDOWN2BOX_RVV
#define HAS_SCALEUVROWDOWN2LINEAR_RVV
#endif

// The following are available on RVV v0.11
#if __riscv_v_intrinsic == 11000
#define HAS_SCALEARGBFILTERCOLS_RVV
#endif
#endif

// Scale ARGB vertically with bilinear interpolation.
        void ScalePlaneVertical(int src_height,
            int dst_width,
            int dst_height,
            int src_stride,
            int dst_stride,
            const uint8_t* src_argb,
            uint8_t* dst_argb,
            int x,
            int y,
            int dy,
            int bpp,
            enum FilterMode filtering);

        void ScalePlaneVertical_16(int src_height,
            int dst_width,
            int dst_height,
            int src_stride,
            int dst_stride,
            const uint16_t* src_argb,
            uint16_t* dst_argb,
            int x,
            int y,
            int dy,
            int wpp,
            enum FilterMode filtering);

        void ScalePlaneVertical_16To8(int src_height,
            int dst_width,
            int dst_height,
            int src_stride,
            int dst_stride,
            const uint16_t* src_argb,
            uint8_t* dst_argb,
            int x,
            int y,
            int dy,
            int wpp,
            int scale,
            enum FilterMode filtering);

        void ScalePlaneDown2_16To8(int src_width,
            int src_height,
            int dst_width,
            int dst_height,
            int src_stride,
            int dst_stride,
            const uint16_t* src_ptr,
            uint8_t* dst_ptr,
            int scale,
            enum FilterMode filtering);

        // Simplify the filtering based on scale factors.
        enum FilterMode ScaleFilterReduce(int src_width,
            int src_height,
            int dst_width,
            int dst_height,
            enum FilterMode filtering);

        // Divide num by div and return as 16.16 fixed point result.
        int FixedDiv_C(int num, int div);
        int FixedDiv_X86(int num, int div);
        // Divide num - 1 by div - 1 and return as 16.16 fixed point result.
        int FixedDiv1_C(int num, int div);
        int FixedDiv1_X86(int num, int div);
#ifdef HAS_FIXEDDIV_X86
#define FixedDiv FixedDiv_X86
#define FixedDiv1 FixedDiv1_X86
#else
#define FixedDiv FixedDiv_C
#define FixedDiv1 FixedDiv1_C
#endif

        // Compute slope values for stepping.
        void ScaleSlope(int src_width,
            int src_height,
            int dst_width,
            int dst_height,
            enum FilterMode filtering,
            int* x,
            int* y,
            int* dx,
            int* dy);

        void ScaleRowDown2_C(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst,
            int dst_width);
        void ScaleRowDown2_16_C(const uint16_t* src_ptr,
            ptrdiff_t src_stride,
            uint16_t* dst,
            int dst_width);
        void ScaleRowDown2_16To8_C(const uint16_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst,
            int dst_width,
            int scale);
        void ScaleRowDown2_16To8_Odd_C(const uint16_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst,
            int dst_width,
            int scale);
        void ScaleRowDown2Linear_C(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst,
            int dst_width);
        void ScaleRowDown2Linear_16_C(const uint16_t* src_ptr,
            ptrdiff_t src_stride,
            uint16_t* dst,
            int dst_width);
        void ScaleRowDown2Linear_16To8_C(const uint16_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst,
            int dst_width,
            int scale);
        void ScaleRowDown2Linear_16To8_Odd_C(const uint16_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst,
            int dst_width,
            int scale);
        void ScaleRowDown2Box_C(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst,
            int dst_width);
        void ScaleRowDown2Box_Odd_C(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst,
            int dst_width);
        void ScaleRowDown2Box_16_C(const uint16_t* src_ptr,
            ptrdiff_t src_stride,
            uint16_t* dst,
            int dst_width);
        void ScaleRowDown2Box_16To8_C(const uint16_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst,
            int dst_width,
            int scale);
        void ScaleRowDown2Box_16To8_Odd_C(const uint16_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst,
            int dst_width,
            int scale);
        void ScaleRowDown4_C(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst,
            int dst_width);
        void ScaleRowDown4_16_C(const uint16_t* src_ptr,
            ptrdiff_t src_stride,
            uint16_t* dst,
            int dst_width);
        void ScaleRowDown4Box_C(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst,
            int dst_width);
        void ScaleRowDown4Box_16_C(const uint16_t* src_ptr,
            ptrdiff_t src_stride,
            uint16_t* dst,
            int dst_width);
        void ScaleRowDown34_C(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst,
            int dst_width);
        void ScaleRowDown34_16_C(const uint16_t* src_ptr,
            ptrdiff_t src_stride,
            uint16_t* dst,
            int dst_width);
        void ScaleRowDown34_0_Box_C(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* d,
            int dst_width);
        void ScaleRowDown34_0_Box_16_C(const uint16_t* src_ptr,
            ptrdiff_t src_stride,
            uint16_t* d,
            int dst_width);
        void ScaleRowDown34_1_Box_C(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* d,
            int dst_width);
        void ScaleRowDown34_1_Box_16_C(const uint16_t* src_ptr,
            ptrdiff_t src_stride,
            uint16_t* d,
            int dst_width);

        void ScaleRowUp2_Linear_C(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowUp2_Bilinear_C(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            ptrdiff_t dst_stride,
            int dst_width);
        void ScaleRowUp2_Linear_16_C(const uint16_t* src_ptr,
            uint16_t* dst_ptr,
            int dst_width);
        void ScaleRowUp2_Bilinear_16_C(const uint16_t* src_ptr,
            ptrdiff_t src_stride,
            uint16_t* dst_ptr,
            ptrdiff_t dst_stride,
            int dst_width);
        void ScaleRowUp2_Linear_Any_C(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowUp2_Bilinear_Any_C(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            ptrdiff_t dst_stride,
            int dst_width);
        void ScaleRowUp2_Linear_16_Any_C(const uint16_t* src_ptr,
            uint16_t* dst_ptr,
            int dst_width);
        void ScaleRowUp2_Bilinear_16_Any_C(const uint16_t* src_ptr,
            ptrdiff_t src_stride,
            uint16_t* dst_ptr,
            ptrdiff_t dst_stride,
            int dst_width);

        void ScaleCols_C(uint8_t* dst_ptr,
            const uint8_t* src_ptr,
            int dst_width,
            int x,
            int dx);
        void ScaleCols_16_C(uint16_t* dst_ptr,
            const uint16_t* src_ptr,
            int dst_width,
            int x,
            int dx);
        void ScaleColsUp2_C(uint8_t* dst_ptr,
            const uint8_t* src_ptr,
            int dst_width,
            int,
            int);
        void ScaleColsUp2_16_C(uint16_t* dst_ptr,
            const uint16_t* src_ptr,
            int dst_width,
            int,
            int);
        void ScaleFilterCols_C(uint8_t* dst_ptr,
            const uint8_t* src_ptr,
            int dst_width,
            int x,
            int dx);
        void ScaleFilterCols_16_C(uint16_t* dst_ptr,
            const uint16_t* src_ptr,
            int dst_width,
            int x,
            int dx);
        void ScaleFilterCols64_C(uint8_t* dst_ptr,
            const uint8_t* src_ptr,
            int dst_width,
            int x32,
            int dx);
        void ScaleFilterCols64_16_C(uint16_t* dst_ptr,
            const uint16_t* src_ptr,
            int dst_width,
            int x32,
            int dx);
        void ScaleRowDown38_C(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst,
            int dst_width);
        void ScaleRowDown38_16_C(const uint16_t* src_ptr,
            ptrdiff_t src_stride,
            uint16_t* dst,
            int dst_width);
        void ScaleRowDown38_3_Box_C(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowDown38_3_Box_16_C(const uint16_t* src_ptr,
            ptrdiff_t src_stride,
            uint16_t* dst_ptr,
            int dst_width);
        void ScaleRowDown38_2_Box_C(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowDown38_2_Box_16_C(const uint16_t* src_ptr,
            ptrdiff_t src_stride,
            uint16_t* dst_ptr,
            int dst_width);
        void ScaleAddRow_C(const uint8_t* src_ptr, uint16_t* dst_ptr, int src_width);
        void ScaleAddRow_16_C(const uint16_t* src_ptr,
            uint32_t* dst_ptr,
            int src_width);
        void ScaleARGBRowDown2_C(const uint8_t* src_argb,
            ptrdiff_t src_stride,
            uint8_t* dst_argb,
            int dst_width);
        void ScaleARGBRowDown2Linear_C(const uint8_t* src_argb,
            ptrdiff_t src_stride,
            uint8_t* dst_argb,
            int dst_width);
        void ScaleARGBRowDown2Box_C(const uint8_t* src_argb,
            ptrdiff_t src_stride,
            uint8_t* dst_argb,
            int dst_width);
        void ScaleARGBRowDownEven_C(const uint8_t* src_argb,
            ptrdiff_t src_stride,
            int src_stepx,
            uint8_t* dst_argb,
            int dst_width);
        void ScaleARGBRowDownEvenBox_C(const uint8_t* src_argb,
            ptrdiff_t src_stride,
            int src_stepx,
            uint8_t* dst_argb,
            int dst_width);
        void ScaleARGBCols_C(uint8_t* dst_argb,
            const uint8_t* src_argb,
            int dst_width,
            int x,
            int dx);
        void ScaleARGBCols64_C(uint8_t* dst_argb,
            const uint8_t* src_argb,
            int dst_width,
            int x32,
            int dx);
        void ScaleARGBColsUp2_C(uint8_t* dst_argb,
            const uint8_t* src_argb,
            int dst_width,
            int,
            int);
        void ScaleARGBFilterCols_C(uint8_t* dst_argb,
            const uint8_t* src_argb,
            int dst_width,
            int x,
            int dx);
        void ScaleARGBFilterCols64_C(uint8_t* dst_argb,
            const uint8_t* src_argb,
            int dst_width,
            int x32,
            int dx);
        void ScaleUVRowDown2_C(const uint8_t* src_uv,
            ptrdiff_t src_stride,
            uint8_t* dst_uv,
            int dst_width);
        void ScaleUVRowDown2Linear_C(const uint8_t* src_uv,
            ptrdiff_t src_stride,
            uint8_t* dst_uv,
            int dst_width);
        void ScaleUVRowDown2Box_C(const uint8_t* src_uv,
            ptrdiff_t src_stride,
            uint8_t* dst_uv,
            int dst_width);
        void ScaleUVRowDownEven_C(const uint8_t* src_uv,
            ptrdiff_t src_stride,
            int src_stepx,
            uint8_t* dst_uv,
            int dst_width);
        void ScaleUVRowDownEvenBox_C(const uint8_t* src_uv,
            ptrdiff_t src_stride,
            int src_stepx,
            uint8_t* dst_uv,
            int dst_width);

        void ScaleUVRowUp2_Linear_C(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleUVRowUp2_Bilinear_C(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            ptrdiff_t dst_stride,
            int dst_width);
        void ScaleUVRowUp2_Linear_Any_C(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleUVRowUp2_Bilinear_Any_C(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            ptrdiff_t dst_stride,
            int dst_width);
        void ScaleUVRowUp2_Linear_16_C(const uint16_t* src_ptr,
            uint16_t* dst_ptr,
            int dst_width);
        void ScaleUVRowUp2_Bilinear_16_C(const uint16_t* src_ptr,
            ptrdiff_t src_stride,
            uint16_t* dst_ptr,
            ptrdiff_t dst_stride,
            int dst_width);
        void ScaleUVRowUp2_Linear_16_Any_C(const uint16_t* src_ptr,
            uint16_t* dst_ptr,
            int dst_width);
        void ScaleUVRowUp2_Bilinear_16_Any_C(const uint16_t* src_ptr,
            ptrdiff_t src_stride,
            uint16_t* dst_ptr,
            ptrdiff_t dst_stride,
            int dst_width);

        void ScaleUVCols_C(uint8_t* dst_uv,
            const uint8_t* src_uv,
            int dst_width,
            int x,
            int dx);
        void ScaleUVCols64_C(uint8_t* dst_uv,
            const uint8_t* src_uv,
            int dst_width,
            int x32,
            int dx);
        void ScaleUVColsUp2_C(uint8_t* dst_uv,
            const uint8_t* src_uv,
            int dst_width,
            int,
            int);
        void ScaleUVFilterCols_C(uint8_t* dst_uv,
            const uint8_t* src_uv,
            int dst_width,
            int x,
            int dx);
        void ScaleUVFilterCols64_C(uint8_t* dst_uv,
            const uint8_t* src_uv,
            int dst_width,
            int x32,
            int dx);

        // Specialized scalers for x86.
        void ScaleRowDown2_SSSE3(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowDown2Linear_SSSE3(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowDown2Box_SSSE3(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowDown2_AVX2(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowDown2Linear_AVX2(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowDown2Box_AVX2(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowDown4_SSSE3(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowDown4Box_SSSE3(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowDown4_AVX2(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowDown4Box_AVX2(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);

        void ScaleRowDown34_SSSE3(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowDown34_1_Box_SSSE3(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowDown34_0_Box_SSSE3(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowDown38_SSSE3(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowDown38_3_Box_SSSE3(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowDown38_2_Box_SSSE3(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);

        void ScaleRowUp2_Linear_SSE2(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowUp2_Bilinear_SSE2(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            ptrdiff_t dst_stride,
            int dst_width);
        void ScaleRowUp2_Linear_12_SSSE3(const uint16_t* src_ptr,
            uint16_t* dst_ptr,
            int dst_width);
        void ScaleRowUp2_Bilinear_12_SSSE3(const uint16_t* src_ptr,
            ptrdiff_t src_stride,
            uint16_t* dst_ptr,
            ptrdiff_t dst_stride,
            int dst_width);
        void ScaleRowUp2_Linear_16_SSE2(const uint16_t* src_ptr,
            uint16_t* dst_ptr,
            int dst_width);
        void ScaleRowUp2_Bilinear_16_SSE2(const uint16_t* src_ptr,
            ptrdiff_t src_stride,
            uint16_t* dst_ptr,
            ptrdiff_t dst_stride,
            int dst_width);
        void ScaleRowUp2_Linear_SSSE3(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowUp2_Bilinear_SSSE3(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            ptrdiff_t dst_stride,
            int dst_width);
        void ScaleRowUp2_Linear_AVX2(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowUp2_Bilinear_AVX2(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            ptrdiff_t dst_stride,
            int dst_width);
        void ScaleRowUp2_Linear_12_AVX2(const uint16_t* src_ptr,
            uint16_t* dst_ptr,
            int dst_width);
        void ScaleRowUp2_Bilinear_12_AVX2(const uint16_t* src_ptr,
            ptrdiff_t src_stride,
            uint16_t* dst_ptr,
            ptrdiff_t dst_stride,
            int dst_width);
        void ScaleRowUp2_Linear_16_AVX2(const uint16_t* src_ptr,
            uint16_t* dst_ptr,
            int dst_width);
        void ScaleRowUp2_Bilinear_16_AVX2(const uint16_t* src_ptr,
            ptrdiff_t src_stride,
            uint16_t* dst_ptr,
            ptrdiff_t dst_stride,
            int dst_width);
        void ScaleRowUp2_Linear_Any_SSE2(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowUp2_Bilinear_Any_SSE2(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            ptrdiff_t dst_stride,
            int dst_width);
        void ScaleRowUp2_Linear_12_Any_SSSE3(const uint16_t* src_ptr,
            uint16_t* dst_ptr,
            int dst_width);
        void ScaleRowUp2_Bilinear_12_Any_SSSE3(const uint16_t* src_ptr,
            ptrdiff_t src_stride,
            uint16_t* dst_ptr,
            ptrdiff_t dst_stride,
            int dst_width);
        void ScaleRowUp2_Linear_16_Any_SSE2(const uint16_t* src_ptr,
            uint16_t* dst_ptr,
            int dst_width);
        void ScaleRowUp2_Bilinear_16_Any_SSE2(const uint16_t* src_ptr,
            ptrdiff_t src_stride,
            uint16_t* dst_ptr,
            ptrdiff_t dst_stride,
            int dst_width);
        void ScaleRowUp2_Linear_Any_SSSE3(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowUp2_Bilinear_Any_SSSE3(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            ptrdiff_t dst_stride,
            int dst_width);
        void ScaleRowUp2_Linear_Any_AVX2(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowUp2_Bilinear_Any_AVX2(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            ptrdiff_t dst_stride,
            int dst_width);
        void ScaleRowUp2_Linear_12_Any_AVX2(const uint16_t* src_ptr,
            uint16_t* dst_ptr,
            int dst_width);
        void ScaleRowUp2_Bilinear_12_Any_AVX2(const uint16_t* src_ptr,
            ptrdiff_t src_stride,
            uint16_t* dst_ptr,
            ptrdiff_t dst_stride,
            int dst_width);
        void ScaleRowUp2_Linear_16_Any_AVX2(const uint16_t* src_ptr,
            uint16_t* dst_ptr,
            int dst_width);
        void ScaleRowUp2_Bilinear_16_Any_AVX2(const uint16_t* src_ptr,
            ptrdiff_t src_stride,
            uint16_t* dst_ptr,
            ptrdiff_t dst_stride,
            int dst_width);

        void ScaleRowDown2_Any_SSSE3(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowDown2Linear_Any_SSSE3(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowDown2Box_Any_SSSE3(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowDown2Box_Odd_SSSE3(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowDown2_Any_AVX2(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowDown2Linear_Any_AVX2(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowDown2Box_Any_AVX2(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowDown2Box_Odd_AVX2(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowDown4_Any_SSSE3(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowDown4Box_Any_SSSE3(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowDown4_Any_AVX2(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowDown4Box_Any_AVX2(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);

        void ScaleRowDown34_Any_SSSE3(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowDown34_1_Box_Any_SSSE3(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowDown34_0_Box_Any_SSSE3(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowDown38_Any_SSSE3(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowDown38_3_Box_Any_SSSE3(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowDown38_2_Box_Any_SSSE3(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);

        void ScaleAddRow_SSE2(const uint8_t* src_ptr, uint16_t* dst_ptr, int src_width);
        void ScaleAddRow_AVX2(const uint8_t* src_ptr, uint16_t* dst_ptr, int src_width);
        void ScaleAddRow_Any_SSE2(const uint8_t* src_ptr,
            uint16_t* dst_ptr,
            int src_width);
        void ScaleAddRow_Any_AVX2(const uint8_t* src_ptr,
            uint16_t* dst_ptr,
            int src_width);

        void ScaleFilterCols_SSSE3(uint8_t* dst_ptr,
            const uint8_t* src_ptr,
            int dst_width,
            int x,
            int dx);
        void ScaleColsUp2_SSE2(uint8_t* dst_ptr,
            const uint8_t* src_ptr,
            int dst_width,
            int x,
            int dx);

        // ARGB Column functions
        void ScaleARGBCols_SSE2(uint8_t* dst_argb,
            const uint8_t* src_argb,
            int dst_width,
            int x,
            int dx);
        void ScaleARGBFilterCols_SSSE3(uint8_t* dst_argb,
            const uint8_t* src_argb,
            int dst_width,
            int x,
            int dx);
        void ScaleARGBColsUp2_SSE2(uint8_t* dst_argb,
            const uint8_t* src_argb,
            int dst_width,
            int x,
            int dx);
        void ScaleARGBFilterCols_NEON(uint8_t* dst_argb,
            const uint8_t* src_argb,
            int dst_width,
            int x,
            int dx);
        void ScaleARGBCols_NEON(uint8_t* dst_argb,
            const uint8_t* src_argb,
            int dst_width,
            int x,
            int dx);
        void ScaleARGBFilterCols_Any_NEON(uint8_t* dst_ptr,
            const uint8_t* src_ptr,
            int dst_width,
            int x,
            int dx);
        void ScaleARGBCols_Any_NEON(uint8_t* dst_ptr,
            const uint8_t* src_ptr,
            int dst_width,
            int x,
            int dx);
        void ScaleARGBFilterCols_RVV(uint8_t* dst_argb,
            const uint8_t* src_argb,
            int dst_width,
            int x,
            int dx);

        // ARGB Row functions
        void ScaleARGBRowDown2_SSE2(const uint8_t* src_argb,
            ptrdiff_t src_stride,
            uint8_t* dst_argb,
            int dst_width);
        void ScaleARGBRowDown2Linear_SSE2(const uint8_t* src_argb,
            ptrdiff_t src_stride,
            uint8_t* dst_argb,
            int dst_width);
        void ScaleARGBRowDown2Box_SSE2(const uint8_t* src_argb,
            ptrdiff_t src_stride,
            uint8_t* dst_argb,
            int dst_width);
        void ScaleARGBRowDown2_NEON(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst,
            int dst_width);
        void ScaleARGBRowDown2_SME(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst,
            int dst_width);
        void ScaleARGBRowDown2Linear_NEON(const uint8_t* src_argb,
            ptrdiff_t src_stride,
            uint8_t* dst_argb,
            int dst_width);
        void ScaleARGBRowDown2Linear_SME(const uint8_t* src_argb,
            ptrdiff_t src_stride,
            uint8_t* dst_argb,
            int dst_width);
        void ScaleARGBRowDown2Box_NEON(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst,
            int dst_width);
        void ScaleARGBRowDown2Box_SME(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst,
            int dst_width);
        void ScaleARGBRowDown2_RVV(const uint8_t* src_argb,
            ptrdiff_t src_stride,
            uint8_t* dst_argb,
            int dst_width);
        void ScaleARGBRowDown2Linear_RVV(const uint8_t* src_argb,
            ptrdiff_t src_stride,
            uint8_t* dst_argb,
            int dst_width);
        void ScaleARGBRowDown2Box_RVV(const uint8_t* src_argb,
            ptrdiff_t src_stride,
            uint8_t* dst_argb,
            int dst_width);
        void ScaleARGBRowDown2_LSX(const uint8_t* src_argb,
            ptrdiff_t src_stride,
            uint8_t* dst_argb,
            int dst_width);
        void ScaleARGBRowDown2Linear_LSX(const uint8_t* src_argb,
            ptrdiff_t src_stride,
            uint8_t* dst_argb,
            int dst_width);
        void ScaleARGBRowDown2Box_LSX(const uint8_t* src_argb,
            ptrdiff_t src_stride,
            uint8_t* dst_argb,
            int dst_width);
        void ScaleARGBRowDown2_Any_SSE2(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleARGBRowDown2Linear_Any_SSE2(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleARGBRowDown2Box_Any_SSE2(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleARGBRowDown2_Any_NEON(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleARGBRowDown2Linear_Any_NEON(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleARGBRowDown2Box_Any_NEON(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleARGBRowDown2_Any_LSX(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleARGBRowDown2Linear_Any_LSX(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleARGBRowDown2Box_Any_LSX(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleARGBRowDownEven_SSE2(const uint8_t* src_argb,
            ptrdiff_t src_stride,
            int src_stepx,
            uint8_t* dst_argb,
            int dst_width);
        void ScaleARGBRowDownEvenBox_SSE2(const uint8_t* src_argb,
            ptrdiff_t src_stride,
            int src_stepx,
            uint8_t* dst_argb,
            int dst_width);
        void ScaleARGBRowDownEven_NEON(const uint8_t* src_argb,
            ptrdiff_t src_stride,
            int src_stepx,
            uint8_t* dst_argb,
            int dst_width);
        void ScaleARGBRowDownEvenBox_NEON(const uint8_t* src_argb,
            ptrdiff_t src_stride,
            int src_stepx,
            uint8_t* dst_argb,
            int dst_width);
        void ScaleARGBRowDownEven_LSX(const uint8_t* src_argb,
            ptrdiff_t src_stride,
            int32_t src_stepx,
            uint8_t* dst_argb,
            int dst_width);
        void ScaleARGBRowDownEvenBox_LSX(const uint8_t* src_argb,
            ptrdiff_t src_stride,
            int src_stepx,
            uint8_t* dst_argb,
            int dst_width);
        void ScaleARGBRowDownEven_RVV(const uint8_t* src_argb,
            ptrdiff_t src_stride,
            int32_t src_stepx,
            uint8_t* dst_argb,
            int dst_width);
        void ScaleARGBRowDownEvenBox_RVV(const uint8_t* src_argb,
            ptrdiff_t src_stride,
            int src_stepx,
            uint8_t* dst_argb,
            int dst_width);
        void ScaleARGBRowDownEven_Any_SSE2(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            int src_stepx,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleARGBRowDownEvenBox_Any_SSE2(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            int src_stepx,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleARGBRowDownEven_Any_NEON(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            int src_stepx,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleARGBRowDownEvenBox_Any_NEON(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            int src_stepx,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleARGBRowDownEven_Any_LSX(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            int32_t src_stepx,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleARGBRowDownEvenBox_Any_LSX(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            int src_stepx,
            uint8_t* dst_ptr,
            int dst_width);

        // UV Row functions
        void ScaleUVRowDown2_SSSE3(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_uv,
            int dst_width);
        void ScaleUVRowDown2Linear_SSSE3(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_uv,
            int dst_width);
        void ScaleUVRowDown2Box_SSSE3(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_uv,
            int dst_width);
        void ScaleUVRowDown2Box_AVX2(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_uv,
            int dst_width);
        void ScaleUVRowDown2_NEON(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst,
            int dst_width);
        void ScaleUVRowDown2_SME(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst,
            int dst_width);
        void ScaleUVRowDown2Linear_NEON(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_uv,
            int dst_width);
        void ScaleUVRowDown2Linear_SME(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_uv,
            int dst_width);
        void ScaleUVRowDown2Box_NEON(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst,
            int dst_width);
        void ScaleUVRowDown2Box_SME(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst,
            int dst_width);
        void ScaleUVRowDown2_RVV(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_uv,
            int dst_width);
        void ScaleUVRowDown2Linear_RVV(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_uv,
            int dst_width);
        void ScaleUVRowDown2Box_RVV(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst,
            int dst_width);
        void ScaleUVRowDown2_Any_SSSE3(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleUVRowDown2Linear_Any_SSSE3(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleUVRowDown2Box_Any_SSSE3(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleUVRowDown2Box_Any_AVX2(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleUVRowDown2_Any_NEON(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleUVRowDown2Linear_Any_NEON(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleUVRowDown2Box_Any_NEON(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleUVRowDownEven_SSSE3(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            int src_stepx,
            uint8_t* dst_uv,
            int dst_width);
        void ScaleUVRowDownEvenBox_SSSE3(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            int src_stepx,
            uint8_t* dst_uv,
            int dst_width);
        void ScaleUVRowDownEven_NEON(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            int src_stepx,
            uint8_t* dst_uv,
            int dst_width);
        void ScaleUVRowDownEvenBox_NEON(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            int src_stepx,
            uint8_t* dst_uv,
            int dst_width);
        void ScaleUVRowDown4_RVV(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            int32_t src_stepx,
            uint8_t* dst_uv,
            int dst_width);
        void ScaleUVRowDownEven_RVV(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            int32_t src_stepx,
            uint8_t* dst_uv,
            int dst_width);
        void ScaleUVRowDownEven_Any_SSSE3(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            int src_stepx,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleUVRowDownEvenBox_Any_SSSE3(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            int src_stepx,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleUVRowDownEven_Any_NEON(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            int src_stepx,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleUVRowDownEvenBox_Any_NEON(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            int src_stepx,
            uint8_t* dst_ptr,
            int dst_width);

        void ScaleUVRowUp2_Linear_SSSE3(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleUVRowUp2_Bilinear_SSSE3(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            ptrdiff_t dst_stride,
            int dst_width);
        void ScaleUVRowUp2_Linear_Any_SSSE3(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleUVRowUp2_Bilinear_Any_SSSE3(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            ptrdiff_t dst_stride,
            int dst_width);
        void ScaleUVRowUp2_Linear_AVX2(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleUVRowUp2_Bilinear_AVX2(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            ptrdiff_t dst_stride,
            int dst_width);
        void ScaleUVRowUp2_Linear_Any_AVX2(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleUVRowUp2_Bilinear_Any_AVX2(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            ptrdiff_t dst_stride,
            int dst_width);
        void ScaleUVRowUp2_Linear_NEON(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleUVRowUp2_Bilinear_NEON(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            ptrdiff_t dst_stride,
            int dst_width);
        void ScaleUVRowUp2_Linear_Any_NEON(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleUVRowUp2_Bilinear_Any_NEON(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            ptrdiff_t dst_stride,
            int dst_width);
        void ScaleUVRowUp2_Linear_RVV(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleUVRowUp2_Bilinear_RVV(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            ptrdiff_t dst_stride,
            int dst_width);
        void ScaleUVRowUp2_Linear_16_SSE41(const uint16_t* src_ptr,
            uint16_t* dst_ptr,
            int dst_width);
        void ScaleUVRowUp2_Bilinear_16_SSE41(const uint16_t* src_ptr,
            ptrdiff_t src_stride,
            uint16_t* dst_ptr,
            ptrdiff_t dst_stride,
            int dst_width);
        void ScaleUVRowUp2_Linear_16_Any_SSE41(const uint16_t* src_ptr,
            uint16_t* dst_ptr,
            int dst_width);
        void ScaleUVRowUp2_Bilinear_16_Any_SSE41(const uint16_t* src_ptr,
            ptrdiff_t src_stride,
            uint16_t* dst_ptr,
            ptrdiff_t dst_stride,
            int dst_width);
        void ScaleUVRowUp2_Linear_16_AVX2(const uint16_t* src_ptr,
            uint16_t* dst_ptr,
            int dst_width);
        void ScaleUVRowUp2_Bilinear_16_AVX2(const uint16_t* src_ptr,
            ptrdiff_t src_stride,
            uint16_t* dst_ptr,
            ptrdiff_t dst_stride,
            int dst_width);
        void ScaleUVRowUp2_Linear_16_Any_AVX2(const uint16_t* src_ptr,
            uint16_t* dst_ptr,
            int dst_width);
        void ScaleUVRowUp2_Bilinear_16_Any_AVX2(const uint16_t* src_ptr,
            ptrdiff_t src_stride,
            uint16_t* dst_ptr,
            ptrdiff_t dst_stride,
            int dst_width);
        void ScaleUVRowUp2_Linear_16_NEON(const uint16_t* src_ptr,
            uint16_t* dst_ptr,
            int dst_width);
        void ScaleUVRowUp2_Bilinear_16_NEON(const uint16_t* src_ptr,
            ptrdiff_t src_stride,
            uint16_t* dst_ptr,
            ptrdiff_t dst_stride,
            int dst_width);
        void ScaleUVRowUp2_Linear_16_Any_NEON(const uint16_t* src_ptr,
            uint16_t* dst_ptr,
            int dst_width);
        void ScaleUVRowUp2_Bilinear_16_Any_NEON(const uint16_t* src_ptr,
            ptrdiff_t src_stride,
            uint16_t* dst_ptr,
            ptrdiff_t dst_stride,
            int dst_width);

        // ScaleRowDown2Box also used by planar functions
        // NEON/SME downscalers with interpolation.
        void ScaleRowDown2_NEON(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst,
            int dst_width);
        void ScaleRowDown2_SME(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst,
            int dst_width);
        void ScaleRowDown2_16_NEON(const uint16_t* src_ptr,
            ptrdiff_t src_stride,
            uint16_t* dst,
            int dst_width);
        void ScaleRowDown2_16_SME(const uint16_t* src_ptr,
            ptrdiff_t src_stride,
            uint16_t* dst,
            int dst_width);
        void ScaleRowDown2Linear_NEON(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst,
            int dst_width);
        void ScaleRowDown2Linear_SME(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst,
            int dst_width);
        void ScaleRowDown2Linear_16_NEON(const uint16_t* src_ptr,
            ptrdiff_t src_stride,
            uint16_t* dst,
            int dst_width);
        void ScaleRowDown2Linear_16_SME(const uint16_t* src_ptr,
            ptrdiff_t src_stride,
            uint16_t* dst,
            int dst_width);
        void ScaleRowDown2Box_NEON(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst,
            int dst_width);
        void ScaleRowDown2Box_SME(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst,
            int dst_width);
        void ScaleRowDown2Box_16_NEON(const uint16_t* src_ptr,
            ptrdiff_t src_stride,
            uint16_t* dst,
            int dst_width);
        void ScaleRowDown2Box_16_SME(const uint16_t* src_ptr,
            ptrdiff_t src_stride,
            uint16_t* dst,
            int dst_width);

        void ScaleRowDown4_NEON(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowDown4Box_NEON(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);

        // Down scale from 4 to 3 pixels. Use the neon multilane read/write
        //  to load up the every 4th pixel into a 4 different registers.
        // Point samples 32 pixels to 24 pixels.
        void ScaleRowDown34_NEON(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowDown34_0_Box_NEON(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowDown34_1_Box_NEON(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);

        // 32 -> 12
        void ScaleRowDown38_NEON(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        // 32x3 -> 12x1
        void ScaleRowDown38_3_Box_NEON(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        // 32x2 -> 12x1
        void ScaleRowDown38_2_Box_NEON(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);

        void ScaleRowDown2_Any_NEON(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowDown2Linear_Any_NEON(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowDown2Box_Any_NEON(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowDown2Box_Odd_NEON(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowDown4_Any_NEON(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowDown4Box_Any_NEON(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowDown34_Any_NEON(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowDown34_0_Box_Any_NEON(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowDown34_1_Box_Any_NEON(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        // 32 -> 12
        void ScaleRowDown38_Any_NEON(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        // 32x3 -> 12x1
        void ScaleRowDown38_3_Box_Any_NEON(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        // 32x2 -> 12x1
        void ScaleRowDown38_2_Box_Any_NEON(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);

        void ScaleRowUp2_Linear_NEON(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowUp2_Bilinear_NEON(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            ptrdiff_t dst_stride,
            int dst_width);
        void ScaleRowUp2_Linear_12_NEON(const uint16_t* src_ptr,
            uint16_t* dst_ptr,
            int dst_width);
        void ScaleRowUp2_Bilinear_12_NEON(const uint16_t* src_ptr,
            ptrdiff_t src_stride,
            uint16_t* dst_ptr,
            ptrdiff_t dst_stride,
            int dst_width);
        void ScaleRowUp2_Linear_16_NEON(const uint16_t* src_ptr,
            uint16_t* dst_ptr,
            int dst_width);
        void ScaleRowUp2_Bilinear_16_NEON(const uint16_t* src_ptr,
            ptrdiff_t src_stride,
            uint16_t* dst_ptr,
            ptrdiff_t dst_stride,
            int dst_width);
        void ScaleRowUp2_Linear_Any_NEON(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowUp2_Bilinear_Any_NEON(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            ptrdiff_t dst_stride,
            int dst_width);
        void ScaleRowUp2_Linear_12_Any_NEON(const uint16_t* src_ptr,
            uint16_t* dst_ptr,
            int dst_width);
        void ScaleRowUp2_Bilinear_12_Any_NEON(const uint16_t* src_ptr,
            ptrdiff_t src_stride,
            uint16_t* dst_ptr,
            ptrdiff_t dst_stride,
            int dst_width);
        void ScaleRowUp2_Linear_16_Any_NEON(const uint16_t* src_ptr,
            uint16_t* dst_ptr,
            int dst_width);
        void ScaleRowUp2_Bilinear_16_Any_NEON(const uint16_t* src_ptr,
            ptrdiff_t src_stride,
            uint16_t* dst_ptr,
            ptrdiff_t dst_stride,
            int dst_width);

        void ScaleAddRow_NEON(const uint8_t* src_ptr, uint16_t* dst_ptr, int src_width);
        void ScaleAddRow_Any_NEON(const uint8_t* src_ptr,
            uint16_t* dst_ptr,
            int src_width);

        void ScaleFilterCols_NEON(uint8_t* dst_ptr,
            const uint8_t* src_ptr,
            int dst_width,
            int x,
            int dx);

        void ScaleFilterCols_Any_NEON(uint8_t* dst_ptr,
            const uint8_t* src_ptr,
            int dst_width,
            int x,
            int dx);

        void ScaleRowDown2_LSX(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst,
            int dst_width);
        void ScaleRowDown2Linear_LSX(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst,
            int dst_width);
        void ScaleRowDown2Box_LSX(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst,
            int dst_width);
        void ScaleRowDown4_LSX(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst,
            int dst_width);
        void ScaleRowDown4Box_LSX(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst,
            int dst_width);
        void ScaleRowDown38_LSX(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst,
            int dst_width);
        void ScaleRowDown38_2_Box_LSX(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowDown38_3_Box_LSX(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleAddRow_LSX(const uint8_t* src_ptr, uint16_t* dst_ptr, int src_width);
        void ScaleFilterCols_LSX(uint8_t* dst_ptr,
            const uint8_t* src_ptr,
            int dst_width,
            int x,
            int dx);
        void ScaleARGBFilterCols_LSX(uint8_t* dst_argb,
            const uint8_t* src_argb,
            int dst_width,
            int x,
            int dx);
        void ScaleARGBCols_LSX(uint8_t* dst_argb,
            const uint8_t* src_argb,
            int dst_width,
            int x,
            int dx);
        void ScaleRowDown34_LSX(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst,
            int dst_width);
        void ScaleRowDown34_0_Box_LSX(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* d,
            int dst_width);
        void ScaleRowDown34_1_Box_LSX(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* d,
            int dst_width);
        void ScaleRowDown2_Any_LSX(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowDown2Linear_Any_LSX(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowDown2Box_Any_LSX(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowDown4_Any_LSX(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowDown4Box_Any_LSX(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowDown38_Any_LSX(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowDown38_2_Box_Any_LSX(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowDown38_3_Box_Any_LSX(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleAddRow_Any_LSX(const uint8_t* src_ptr,
            uint16_t* dst_ptr,
            int src_width);
        void ScaleFilterCols_Any_LSX(uint8_t* dst_ptr,
            const uint8_t* src_ptr,
            int dst_width,
            int x,
            int dx);
        void ScaleARGBCols_Any_LSX(uint8_t* dst_ptr,
            const uint8_t* src_ptr,
            int dst_width,
            int x,
            int dx);
        void ScaleARGBFilterCols_Any_LSX(uint8_t* dst_ptr,
            const uint8_t* src_ptr,
            int dst_width,
            int x,
            int dx);
        void ScaleRowDown34_Any_LSX(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowDown34_0_Box_Any_LSX(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowDown34_1_Box_Any_LSX(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);

        void ScaleAddRow_RVV(const uint8_t* src_ptr, uint16_t* dst_ptr, int src_width);
        void ScaleRowDown2_RVV(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst,
            int dst_width);
        void ScaleRowDown2Linear_RVV(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst,
            int dst_width);
        void ScaleRowDown2Box_RVV(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst,
            int dst_width);

        void ScaleRowDown4_RVV(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowDown4Box_RVV(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowDown34_RVV(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowDown34_0_Box_RVV(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowDown34_1_Box_RVV(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowDown38_RVV(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst,
            int dst_width);
        void ScaleRowDown38_3_Box_RVV(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowDown38_2_Box_RVV(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            int dst_width);

        void ScaleRowUp2_Linear_RVV(const uint8_t* src_ptr,
            uint8_t* dst_ptr,
            int dst_width);
        void ScaleRowUp2_Bilinear_RVV(const uint8_t* src_ptr,
            ptrdiff_t src_stride,
            uint8_t* dst_ptr,
            ptrdiff_t dst_stride,
            int dst_width);
#ifdef __cplusplus
    }  // extern "C"
}  // namespace libyuv
#endif



#ifdef __cplusplus
namespace libyuv {
    extern "C" {
#endif

        LIBYUV_API
            int UVScale(const uint8_t* src_uv,
                int src_stride_uv,
                int src_width,
                int src_height,
                uint8_t* dst_uv,
                int dst_stride_uv,
                int dst_width,
                int dst_height,
                enum FilterMode filtering);

        // Scale a 16 bit UV image.
        // This function is currently incomplete, it can't handle all cases.
        LIBYUV_API
            int UVScale_16(const uint16_t* src_uv,
                int src_stride_uv,
                int src_width,
                int src_height,
                uint16_t* dst_uv,
                int dst_stride_uv,
                int dst_width,
                int dst_height,
                enum FilterMode filtering);

#ifdef __cplusplus
    }  // extern "C"
}  // namespace libyuv
#endif

#define LIBYUV_VERSION 1922




#ifdef __cplusplus
namespace libyuv {
    extern "C" {
#endif

        //////////////////////////////////////////////////////////////////////////////
        // Definition of FourCC codes
        //////////////////////////////////////////////////////////////////////////////

        // Convert four characters to a FourCC code.
        // Needs to be a macro otherwise the OS X compiler complains when the kFormat*
        // constants are used in a switch.
#ifdef __cplusplus
#define FOURCC(a, b, c, d)                                        \
  ((static_cast<uint32_t>(a)) | (static_cast<uint32_t>(b) << 8) | \
   (static_cast<uint32_t>(c) << 16) | /* NOLINT */                \
   (static_cast<uint32_t>(d) << 24))  /* NOLINT */
#else
#define FOURCC(a, b, c, d)                                     \
  (((uint32_t)(a)) | ((uint32_t)(b) << 8) |       /* NOLINT */ \
   ((uint32_t)(c) << 16) | ((uint32_t)(d) << 24)) /* NOLINT */
#endif

// Some pages discussing FourCC codes:
//   http://www.fourcc.org/yuv.php
//   http://v4l2spec.bytesex.org/spec/book1.htm
//   http://developer.apple.com/quicktime/icefloe/dispatch020.html
//   http://msdn.microsoft.com/library/windows/desktop/dd206750.aspx#nv12
//   http://people.xiph.org/~xiphmont/containers/nut/nut4cc.txt

// FourCC codes grouped according to implementation efficiency.
// Primary formats should convert in 1 efficient step.
// Secondary formats are converted in 2 steps.
// Auxilliary formats call primary converters.
        enum FourCC {
            // 10 Primary YUV formats: 5 planar, 2 biplanar, 2 packed.
            FOURCC_I420 = FOURCC('I', '4', '2', '0'),
            FOURCC_I422 = FOURCC('I', '4', '2', '2'),
            FOURCC_I444 = FOURCC('I', '4', '4', '4'),
            FOURCC_I400 = FOURCC('I', '4', '0', '0'),
            FOURCC_NV21 = FOURCC('N', 'V', '2', '1'),
            FOURCC_NV12 = FOURCC('N', 'V', '1', '2'),
            FOURCC_YUY2 = FOURCC('Y', 'U', 'Y', '2'),
            FOURCC_UYVY = FOURCC('U', 'Y', 'V', 'Y'),
            FOURCC_I010 = FOURCC('I', '0', '1', '0'),  // bt.601 10 bit 420
            FOURCC_I210 = FOURCC('I', '2', '1', '0'),  // bt.601 10 bit 422

            // 1 Secondary YUV format: row biplanar.  deprecated.
            FOURCC_M420 = FOURCC('M', '4', '2', '0'),

            // 13 Primary RGB formats: 4 32 bpp, 2 24 bpp, 3 16 bpp, 1 10 bpc 2 64 bpp
            FOURCC_ARGB = FOURCC('A', 'R', 'G', 'B'),
            FOURCC_BGRA = FOURCC('B', 'G', 'R', 'A'),
            FOURCC_ABGR = FOURCC('A', 'B', 'G', 'R'),
            FOURCC_AR30 = FOURCC('A', 'R', '3', '0'),  // 10 bit per channel. 2101010.
            FOURCC_AB30 = FOURCC('A', 'B', '3', '0'),  // ABGR version of 10 bit
            FOURCC_AR64 = FOURCC('A', 'R', '6', '4'),  // 16 bit per channel.
            FOURCC_AB64 = FOURCC('A', 'B', '6', '4'),  // ABGR version of 16 bit
            FOURCC_24BG = FOURCC('2', '4', 'B', 'G'),
            FOURCC_RAW = FOURCC('r', 'a', 'w', ' '),
            FOURCC_RGBA = FOURCC('R', 'G', 'B', 'A'),
            FOURCC_RGBP = FOURCC('R', 'G', 'B', 'P'),  // rgb565 LE.
            FOURCC_RGBO = FOURCC('R', 'G', 'B', 'O'),  // argb1555 LE.
            FOURCC_R444 = FOURCC('R', '4', '4', '4'),  // argb4444 LE.

            // 1 Primary Compressed YUV format.
            FOURCC_MJPG = FOURCC('M', 'J', 'P', 'G'),

            // 14 Auxiliary YUV variations: 3 with U and V planes are swapped, 1 Alias.
            FOURCC_YV12 = FOURCC('Y', 'V', '1', '2'),
            FOURCC_YV16 = FOURCC('Y', 'V', '1', '6'),
            FOURCC_YV24 = FOURCC('Y', 'V', '2', '4'),
            FOURCC_YU12 = FOURCC('Y', 'U', '1', '2'),  // Linux version of I420.
            FOURCC_J420 =
            FOURCC('J', '4', '2', '0'),  // jpeg (bt.601 full), unofficial fourcc
            FOURCC_J422 =
            FOURCC('J', '4', '2', '2'),  // jpeg (bt.601 full), unofficial fourcc
            FOURCC_J444 =
            FOURCC('J', '4', '4', '4'),  // jpeg (bt.601 full), unofficial fourcc
            FOURCC_J400 =
            FOURCC('J', '4', '0', '0'),  // jpeg (bt.601 full), unofficial fourcc
            FOURCC_F420 = FOURCC('F', '4', '2', '0'),  // bt.709 full, unofficial fourcc
            FOURCC_F422 = FOURCC('F', '4', '2', '2'),  // bt.709 full, unofficial fourcc
            FOURCC_F444 = FOURCC('F', '4', '4', '4'),  // bt.709 full, unofficial fourcc
            FOURCC_H420 = FOURCC('H', '4', '2', '0'),  // bt.709, unofficial fourcc
            FOURCC_H422 = FOURCC('H', '4', '2', '2'),  // bt.709, unofficial fourcc
            FOURCC_H444 = FOURCC('H', '4', '4', '4'),  // bt.709, unofficial fourcc
            FOURCC_U420 = FOURCC('U', '4', '2', '0'),  // bt.2020, unofficial fourcc
            FOURCC_U422 = FOURCC('U', '4', '2', '2'),  // bt.2020, unofficial fourcc
            FOURCC_U444 = FOURCC('U', '4', '4', '4'),  // bt.2020, unofficial fourcc
            FOURCC_F010 = FOURCC('F', '0', '1', '0'),  // bt.709 full range 10 bit 420
            FOURCC_H010 = FOURCC('H', '0', '1', '0'),  // bt.709 10 bit 420
            FOURCC_U010 = FOURCC('U', '0', '1', '0'),  // bt.2020 10 bit 420
            FOURCC_F210 = FOURCC('F', '2', '1', '0'),  // bt.709 full range 10 bit 422
            FOURCC_H210 = FOURCC('H', '2', '1', '0'),  // bt.709 10 bit 422
            FOURCC_U210 = FOURCC('U', '2', '1', '0'),  // bt.2020 10 bit 422
            LIBYUV_FOURCC_P010 = FOURCC('P', '0', '1', '0'),
            LIBYUV_FOURCC_P210 = FOURCC('P', '2', '1', '0'),

            // 14 Auxiliary aliases.  CanonicalFourCC() maps these to canonical fourcc.
            FOURCC_IYUV = FOURCC('I', 'Y', 'U', 'V'),  // Alias for I420.
            FOURCC_YU16 = FOURCC('Y', 'U', '1', '6'),  // Alias for I422.
            FOURCC_YU24 = FOURCC('Y', 'U', '2', '4'),  // Alias for I444.
            FOURCC_YUYV = FOURCC('Y', 'U', 'Y', 'V'),  // Alias for YUY2.
            FOURCC_YUVS = FOURCC('y', 'u', 'v', 's'),  // Alias for YUY2 on Mac.
            FOURCC_HDYC = FOURCC('H', 'D', 'Y', 'C'),  // Alias for UYVY.
            FOURCC_2VUY = FOURCC('2', 'v', 'u', 'y'),  // Alias for UYVY on Mac.
            FOURCC_JPEG = FOURCC('J', 'P', 'E', 'G'),  // Alias for MJPG.
            FOURCC_DMB1 = FOURCC('d', 'm', 'b', '1'),  // Alias for MJPG on Mac.
            FOURCC_BA81 = FOURCC('B', 'A', '8', '1'),  // Alias for BGGR.
            FOURCC_RGB3 = FOURCC('R', 'G', 'B', '3'),  // Alias for RAW.
            FOURCC_BGR3 = FOURCC('B', 'G', 'R', '3'),  // Alias for 24BG.
            FOURCC_CM32 = FOURCC(0, 0, 0, 32),  // Alias for BGRA kCMPixelFormat_32ARGB
            FOURCC_CM24 = FOURCC(0, 0, 0, 24),  // Alias for RAW kCMPixelFormat_24RGB
            FOURCC_L555 = FOURCC('L', '5', '5', '5'),  // Alias for RGBO.
            FOURCC_L565 = FOURCC('L', '5', '6', '5'),  // Alias for RGBP.
            FOURCC_5551 = FOURCC('5', '5', '5', '1'),  // Alias for RGBO.

            // deprecated formats.  Not supported, but defined for backward compatibility.
            FOURCC_I411 = FOURCC('I', '4', '1', '1'),
            FOURCC_Q420 = FOURCC('Q', '4', '2', '0'),
            FOURCC_RGGB = FOURCC('R', 'G', 'G', 'B'),
            FOURCC_BGGR = FOURCC('B', 'G', 'G', 'R'),
            FOURCC_GRBG = FOURCC('G', 'R', 'B', 'G'),
            FOURCC_GBRG = FOURCC('G', 'B', 'R', 'G'),
            FOURCC_H264 = FOURCC('H', '2', '6', '4'),

            // Match any fourcc.
            FOURCC_ANY = -1,
        };

        enum FourCCBpp {
            // Canonical fourcc codes used in our code.
            FOURCC_BPP_I420 = 12,
            FOURCC_BPP_I422 = 16,
            FOURCC_BPP_I444 = 24,
            FOURCC_BPP_I411 = 12,
            FOURCC_BPP_I400 = 8,
            FOURCC_BPP_NV21 = 12,
            FOURCC_BPP_NV12 = 12,
            FOURCC_BPP_YUY2 = 16,
            FOURCC_BPP_UYVY = 16,
            FOURCC_BPP_M420 = 12,  // deprecated
            FOURCC_BPP_Q420 = 12,
            FOURCC_BPP_ARGB = 32,
            FOURCC_BPP_BGRA = 32,
            FOURCC_BPP_ABGR = 32,
            FOURCC_BPP_RGBA = 32,
            FOURCC_BPP_AR30 = 32,
            FOURCC_BPP_AB30 = 32,
            FOURCC_BPP_AR64 = 64,
            FOURCC_BPP_AB64 = 64,
            FOURCC_BPP_24BG = 24,
            FOURCC_BPP_RAW = 24,
            FOURCC_BPP_RGBP = 16,
            FOURCC_BPP_RGBO = 16,
            FOURCC_BPP_R444 = 16,
            FOURCC_BPP_RGGB = 8,
            FOURCC_BPP_BGGR = 8,
            FOURCC_BPP_GRBG = 8,
            FOURCC_BPP_GBRG = 8,
            FOURCC_BPP_YV12 = 12,
            FOURCC_BPP_YV16 = 16,
            FOURCC_BPP_YV24 = 24,
            FOURCC_BPP_YU12 = 12,
            FOURCC_BPP_J420 = 12,
            FOURCC_BPP_J400 = 8,
            FOURCC_BPP_H420 = 12,
            FOURCC_BPP_H422 = 16,
            FOURCC_BPP_I010 = 15,
            FOURCC_BPP_I210 = 20,
            FOURCC_BPP_H010 = 15,
            FOURCC_BPP_H210 = 20,
            FOURCC_BPP_P010 = 15,
            FOURCC_BPP_P210 = 20,
            FOURCC_BPP_MJPG = 0,  // 0 means unknown.
            FOURCC_BPP_H264 = 0,
            FOURCC_BPP_IYUV = 12,
            FOURCC_BPP_YU16 = 16,
            FOURCC_BPP_YU24 = 24,
            FOURCC_BPP_YUYV = 16,
            FOURCC_BPP_YUVS = 16,
            FOURCC_BPP_HDYC = 16,
            FOURCC_BPP_2VUY = 16,
            FOURCC_BPP_JPEG = 1,
            FOURCC_BPP_DMB1 = 1,
            FOURCC_BPP_BA81 = 8,
            FOURCC_BPP_RGB3 = 24,
            FOURCC_BPP_BGR3 = 24,
            FOURCC_BPP_CM32 = 32,
            FOURCC_BPP_CM24 = 24,

            // Match any fourcc.
            FOURCC_BPP_ANY = 0,  // 0 means unknown.
        };

        // Converts fourcc aliases into canonical ones.
        LIBYUV_API uint32_t CanonicalFourCC(uint32_t fourcc);

#ifdef __cplusplus
    }  // extern "C"
}  // namespace libyuv
#endif



#ifdef __cplusplus
namespace libyuv {
    extern "C" {
#endif

        // The following are available for Visual C and GCC:
#if !defined(LIBYUV_DISABLE_X86) && !defined(__clang__) &&                            \
    ((defined(__x86_64__) && !defined(LIBYUV_ENABLE_ROWWIN)) || \
     defined(__i386__) || defined(_M_IX86))
#define HAS_HASHDJB2_SSE41
#define HAS_SUMSQUAREERROR_SSE2
#define HAS_HAMMINGDISTANCE_SSE42
#endif

// The following are available for Visual C and clangcl 32 bit:
#if !defined(LIBYUV_DISABLE_X86) && defined(_M_IX86) && defined(_MSC_VER) && \
    !defined(__clang__) &&                                                   \
    (defined(VISUALC_HAS_AVX2) || defined(CLANG_HAS_AVX2))
#define HAS_HASHDJB2_AVX2
#define HAS_SUMSQUAREERROR_AVX2
#endif

// The following are available for GCC and clangcl:
#if !defined(LIBYUV_DISABLE_X86) &&  !(_MSC_VER)  &&  \
    (defined(__x86_64__) || defined(__i386__)) && \
    !defined(LIBYUV_ENABLE_ROWWIN)
#define HAS_HAMMINGDISTANCE_SSSE3
#endif

// The following are available for GCC and clangcl:
#if !defined(LIBYUV_DISABLE_X86) && !defined(_MSC_VER)&& defined(CLANG_HAS_AVX2) && \
    (defined(__x86_64__) || defined(__i386__)) &&              \
    !defined(LIBYUV_ENABLE_ROWWIN)
#define HAS_HAMMINGDISTANCE_AVX2
#endif

// The following are available for Neon:
#if !defined(LIBYUV_DISABLE_NEON) && \
    (defined(__ARM_NEON__) || defined(LIBYUV_NEON) || defined(__aarch64__))
#define HAS_HAMMINGDISTANCE_NEON
#define HAS_SUMSQUAREERROR_NEON
#endif

// The following are available for AArch64 Neon:
#if !defined(LIBYUV_DISABLE_NEON) && defined(__aarch64__)
#define HAS_HASHDJB2_NEON

#define HAS_HAMMINGDISTANCE_NEON_DOTPROD
#define HAS_SUMSQUAREERROR_NEON_DOTPROD
#endif

        uint32_t HammingDistance_C(const uint8_t* src_a,
            const uint8_t* src_b,
            int count);
        uint32_t HammingDistance_SSE42(const uint8_t* src_a,
            const uint8_t* src_b,
            int count);
        uint32_t HammingDistance_SSSE3(const uint8_t* src_a,
            const uint8_t* src_b,
            int count);
        uint32_t HammingDistance_AVX2(const uint8_t* src_a,
            const uint8_t* src_b,
            int count);
        uint32_t HammingDistance_NEON(const uint8_t* src_a,
            const uint8_t* src_b,
            int count);
        uint32_t HammingDistance_NEON_DotProd(const uint8_t* src_a,
            const uint8_t* src_b,
            int count);
        uint32_t SumSquareError_C(const uint8_t* src_a,
            const uint8_t* src_b,
            int count);
        uint32_t SumSquareError_SSE2(const uint8_t* src_a,
            const uint8_t* src_b,
            int count);
        uint32_t SumSquareError_AVX2(const uint8_t* src_a,
            const uint8_t* src_b,
            int count);
        uint32_t SumSquareError_NEON(const uint8_t* src_a,
            const uint8_t* src_b,
            int count);
        uint32_t SumSquareError_NEON_DotProd(const uint8_t* src_a,
            const uint8_t* src_b,
            int count);

        uint32_t HashDjb2_C(const uint8_t* src, int count, uint32_t seed);
        uint32_t HashDjb2_SSE41(const uint8_t* src, int count, uint32_t seed);
        uint32_t HashDjb2_AVX2(const uint8_t* src, int count, uint32_t seed);
        uint32_t HashDjb2_NEON(const uint8_t* src, int count, uint32_t seed);

#ifdef __cplusplus
    }  // extern "C"
}  // namespace libyuv
#endif




#ifdef __cplusplus
namespace libyuv {
    extern "C" {
#endif

        // RGB can be RAW, RGB24 or YUV24
        // RGB scales 24 bit images by converting a row at a time to ARGB
        // and using ARGB row functions to scale, then convert to RGB.
        // TODO(fbarchard): Allow input/output formats to be specified.
        LIBYUV_API
            int RGBScale(const uint8_t* src_rgb,
                int src_stride_rgb,
                int src_width,
                int src_height,
                uint8_t* dst_rgb,
                int dst_stride_rgb,
                int dst_width,
                int dst_height,
                enum FilterMode filtering);

#ifdef __cplusplus
    }  // extern "C"
}  // namespace libyuv
#endif



#ifdef __cplusplus
namespace libyuv {
    extern "C" {
#endif

        // The following are available for Visual C 32 bit:
        // TODO - port to clangcl on rotate_win
#if !defined(LIBYUV_DISABLE_X86) && defined(_M_IX86) && defined(_MSC_VER) && \
    !defined(__clang__)
#define HAS_TRANSPOSEWX8_SSSE3
#define HAS_TRANSPOSEUVWX8_SSE2
#endif

// The following are available for GCC 32 or 64 bit:
#if !defined(LIBYUV_DISABLE_X86) &&  !defined(__clang__) &&             \
    (defined(__i386__) || defined(__x86_64__)) && \
    !defined(LIBYUV_ENABLE_ROWWIN)
#define HAS_TRANSPOSEWX8_SSSE3
#define HAS_TRANSPOSE4X4_32_SSE2
#define HAS_TRANSPOSE4X4_32_AVX2
#endif

// The following are available for 64 bit GCC:
#if !defined(LIBYUV_DISABLE_X86) && !defined(__clang__) && defined(__x86_64__) && \
    !defined(LIBYUV_ENABLE_ROWWIN)
#define HAS_TRANSPOSEWX8_FAST_SSSE3
#define HAS_TRANSPOSEUVWX8_SSE2
#endif

#if !defined(LIBYUV_DISABLE_NEON) && \
    (defined(__ARM_NEON__) || defined(LIBYUV_NEON) || defined(__aarch64__))
#if defined(__aarch64__)
#define HAS_TRANSPOSEWX16_NEON
#else
#define HAS_TRANSPOSEWX8_NEON
#endif
#define HAS_TRANSPOSEUVWX8_NEON
#define HAS_TRANSPOSE4X4_32_NEON
#endif

#if !defined(LIBYUV_DISABLE_SME) && defined(CLANG_HAS_SME) && \
    defined(__aarch64__)
#define HAS_TRANSPOSEWXH_SME
#define HAS_TRANSPOSEUVWXH_SME
#endif

#if !defined(LIBYUV_DISABLE_LSX) && defined(__loongarch_sx)
#define HAS_TRANSPOSEWX16_LSX
#define HAS_TRANSPOSEUVWX16_LSX
#endif

        void TransposeWxH_C(const uint8_t* src,
            int src_stride,
            uint8_t* dst,
            int dst_stride,
            int width,
            int height);

        void TransposeWx8_C(const uint8_t* src,
            int src_stride,
            uint8_t* dst,
            int dst_stride,
            int width);
        void TransposeWx16_C(const uint8_t* src,
            int src_stride,
            uint8_t* dst,
            int dst_stride,
            int width);
        void TransposeWx8_NEON(const uint8_t* src,
            int src_stride,
            uint8_t* dst,
            int dst_stride,
            int width);
        void TransposeWx16_NEON(const uint8_t* src,
            int src_stride,
            uint8_t* dst,
            int dst_stride,
            int width);
        void TransposeWxH_SME(const uint8_t* src,
            int src_stride,
            uint8_t* dst,
            int dst_stride,
            int width,
            int height);
        void TransposeWx8_SSSE3(const uint8_t* src,
            int src_stride,
            uint8_t* dst,
            int dst_stride,
            int width);
        void TransposeWx8_Fast_SSSE3(const uint8_t* src,
            int src_stride,
            uint8_t* dst,
            int dst_stride,
            int width);
        void TransposeWx16_LSX(const uint8_t* src,
            int src_stride,
            uint8_t* dst,
            int dst_stride,
            int width);

        void TransposeWx8_Any_NEON(const uint8_t* src,
            int src_stride,
            uint8_t* dst,
            int dst_stride,
            int width);
        void TransposeWx16_Any_NEON(const uint8_t* src,
            int src_stride,
            uint8_t* dst,
            int dst_stride,
            int width);
        void TransposeWx8_Any_SSSE3(const uint8_t* src,
            int src_stride,
            uint8_t* dst,
            int dst_stride,
            int width);
        void TransposeWx8_Fast_Any_SSSE3(const uint8_t* src,
            int src_stride,
            uint8_t* dst,
            int dst_stride,
            int width);
        void TransposeWx16_Any_LSX(const uint8_t* src,
            int src_stride,
            uint8_t* dst,
            int dst_stride,
            int width);

        void TransposeUVWxH_C(const uint8_t* src,
            int src_stride,
            uint8_t* dst_a,
            int dst_stride_a,
            uint8_t* dst_b,
            int dst_stride_b,
            int width,
            int height);

        void TransposeUVWx8_C(const uint8_t* src,
            int src_stride,
            uint8_t* dst_a,
            int dst_stride_a,
            uint8_t* dst_b,
            int dst_stride_b,
            int width);
        void TransposeUVWx16_C(const uint8_t* src,
            int src_stride,
            uint8_t* dst_a,
            int dst_stride_a,
            uint8_t* dst_b,
            int dst_stride_b,
            int width);
        void TransposeUVWx8_SSE2(const uint8_t* src,
            int src_stride,
            uint8_t* dst_a,
            int dst_stride_a,
            uint8_t* dst_b,
            int dst_stride_b,
            int width);
        void TransposeUVWx8_NEON(const uint8_t* src,
            int src_stride,
            uint8_t* dst_a,
            int dst_stride_a,
            uint8_t* dst_b,
            int dst_stride_b,
            int width);
        void TransposeUVWxH_SME(const uint8_t* src,
            int src_stride,
            uint8_t* dst_a,
            int dst_stride_a,
            uint8_t* dst_b,
            int dst_stride_b,
            int width,
            int height);
        void TransposeUVWx16_LSX(const uint8_t* src,
            int src_stride,
            uint8_t* dst_a,
            int dst_stride_a,
            uint8_t* dst_b,
            int dst_stride_b,
            int width);

        void TransposeUVWx8_Any_SSE2(const uint8_t* src,
            int src_stride,
            uint8_t* dst_a,
            int dst_stride_a,
            uint8_t* dst_b,
            int dst_stride_b,
            int width);
        void TransposeUVWx8_Any_NEON(const uint8_t* src,
            int src_stride,
            uint8_t* dst_a,
            int dst_stride_a,
            uint8_t* dst_b,
            int dst_stride_b,
            int width);
        void TransposeUVWx16_Any_LSX(const uint8_t* src,
            int src_stride,
            uint8_t* dst_a,
            int dst_stride_a,
            uint8_t* dst_b,
            int dst_stride_b,
            int width);
        void TransposeWxH_16_C(const uint16_t* src,
            int src_stride,
            uint16_t* dst,
            int dst_stride,
            int width,
            int height);

        void TransposeWx8_16_C(const uint16_t* src,
            int src_stride,
            uint16_t* dst,
            int dst_stride,
            int width);
        void TransposeWx1_16_C(const uint16_t* src,
            int src_stride,
            uint16_t* dst,
            int dst_stride,
            int width);

        // Transpose 32 bit values (ARGB)
        void Transpose4x4_32_NEON(const uint8_t* src,
            int src_stride,
            uint8_t* dst,
            int dst_stride,
            int width);

        void Transpose4x4_32_SSE2(const uint8_t* src,
            int src_stride,
            uint8_t* dst,
            int dst_stride,
            int width);

        void Transpose4x4_32_AVX2(const uint8_t* src,
            int src_stride,
            uint8_t* dst,
            int dst_stride,
            int width);

        void Transpose4x4_32_C(const uint8_t* src,
            int src_stride,
            uint8_t* dst,
            int dst_stride,
            int width);

#ifdef __cplusplus
    }  // extern "C"
}  // namespace libyuv
#endif




#endif  // INCLUDE_LIBYUV_H_
