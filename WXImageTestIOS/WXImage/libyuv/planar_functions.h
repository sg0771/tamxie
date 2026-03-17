/*
 *  Copyright 2011 The LibYuv Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS. All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef INCLUDE_LIBYUV_PLANAR_FUNCTIONS_H_  // NOLINT
#define INCLUDE_LIBYUV_PLANAR_FUNCTIONS_H_

#include "./basic_types.h"

// TODO(fbarchard): Remove the following headers includes.
#include "./convert.h"
#include "./convert_argb.h"

#ifdef __cplusplus
namespace libyuv {
extern "C" {
#endif

// Copy a plane of data.

void CopyPlane(const uint8* src_y, int src_stride_y,
               uint8* dst_y, int dst_stride_y,
               int width, int height);


void CopyPlane_16(const uint16* src_y, int src_stride_y,
                  uint16* dst_y, int dst_stride_y,
                  int width, int height);

// Set a plane of data to a 32 bit value.

void SetPlane(uint8* dst_y, int dst_stride_y,
              int width, int height,
              uint32 value);

// Copy I400.  Supports inverting.

int I400ToI400(const uint8* src_y, int src_stride_y,
               uint8* dst_y, int dst_stride_y,
               int width, int height);

#define J400ToJ400 I400ToI400

// Copy I422 to I422.
#define I422ToI422 I422Copy

int I422Copy(const uint8* src_y, int src_stride_y,
             const uint8* src_u, int src_stride_u,
             const uint8* src_v, int src_stride_v,
             uint8* dst_y, int dst_stride_y,
             uint8* dst_u, int dst_stride_u,
             uint8* dst_v, int dst_stride_v,
             int width, int height);

// Copy I444 to I444.
#define I444ToI444 I444Copy

int I444Copy(const uint8* src_y, int src_stride_y,
             const uint8* src_u, int src_stride_u,
             const uint8* src_v, int src_stride_v,
             uint8* dst_y, int dst_stride_y,
             uint8* dst_u, int dst_stride_u,
             uint8* dst_v, int dst_stride_v,
             int width, int height);

// Convert YUY2 to I422.

int YUY2ToI422(const uint8* src_yuy2, int src_stride_yuy2,
               uint8* dst_y, int dst_stride_y,
               uint8* dst_u, int dst_stride_u,
               uint8* dst_v, int dst_stride_v,
               int width, int height);

// Convert UYVY to I422.

int UYVYToI422(const uint8* src_uyvy, int src_stride_uyvy,
               uint8* dst_y, int dst_stride_y,
               uint8* dst_u, int dst_stride_u,
               uint8* dst_v, int dst_stride_v,
               int width, int height);


// Convert I420 to I400. (calls CopyPlane ignoring u/v).

int I420ToI400(const uint8* src_y, int src_stride_y,
               const uint8* src_u, int src_stride_u,
               const uint8* src_v, int src_stride_v,
               uint8* dst_y, int dst_stride_y,
               int width, int height);

// Alias
#define J420ToJ400 I420ToI400
#define I420ToI420Mirror I420Mirror

// I420 mirror.

int I420Mirror(const uint8* src_y, int src_stride_y,
               const uint8* src_u, int src_stride_u,
               const uint8* src_v, int src_stride_v,
               uint8* dst_y, int dst_stride_y,
               uint8* dst_u, int dst_stride_u,
               uint8* dst_v, int dst_stride_v,
               int width, int height);

// Alias
#define I400ToI400Mirror I400Mirror

// I400 mirror.  A single plane is mirrored horizontally.
// Pass negative height to achieve 180 degree rotation.

int I400Mirror(const uint8* src_y, int src_stride_y,
               uint8* dst_y, int dst_stride_y,
               int width, int height);

// Alias
#define ARGBToARGBMirror ARGBMirror

// ARGB mirror.

int ARGBMirror(const uint8* src_argb, int src_stride_argb,
               uint8* dst_argb, int dst_stride_argb,
               int width, int height);

// Convert NV12 to RGB565.

int NV12ToRGB565(const uint8* src_y, int src_stride_y,
                 const uint8* src_uv, int src_stride_uv,
                 uint8* dst_rgb565, int dst_stride_rgb565,
                 int width, int height);

// Convert NV21 to RGB565.

int NV21ToRGB565(const uint8* src_y, int src_stride_y,
                 const uint8* src_uv, int src_stride_uv,
                 uint8* dst_rgb565, int dst_stride_rgb565,
                 int width, int height);

// I422ToARGB is in convert_argb.h
// Convert I422 to BGRA.

int I422ToBGRA(const uint8* src_y, int src_stride_y,
               const uint8* src_u, int src_stride_u,
               const uint8* src_v, int src_stride_v,
               uint8* dst_bgra, int dst_stride_bgra,
               int width, int height);

// Convert I422 to ABGR.

int I422ToABGR(const uint8* src_y, int src_stride_y,
               const uint8* src_u, int src_stride_u,
               const uint8* src_v, int src_stride_v,
               uint8* dst_abgr, int dst_stride_abgr,
               int width, int height);

// Convert I422 to RGBA.

int I422ToRGBA(const uint8* src_y, int src_stride_y,
               const uint8* src_u, int src_stride_u,
               const uint8* src_v, int src_stride_v,
               uint8* dst_rgba, int dst_stride_rgba,
               int width, int height);

// Draw a rectangle into I420.

int I420Rect(uint8* dst_y, int dst_stride_y,
             uint8* dst_u, int dst_stride_u,
             uint8* dst_v, int dst_stride_v,
             int x, int y, int width, int height,
             int value_y, int value_u, int value_v);

// Draw a rectangle into ARGB.

int ARGBRect(uint8* dst_argb, int dst_stride_argb,
             int x, int y, int width, int height, uint32 value);

// Convert ARGB to gray scale ARGB.

int ARGBGrayTo(const uint8* src_argb, int src_stride_argb,
               uint8* dst_argb, int dst_stride_argb,
               int width, int height);

// Make a rectangle of ARGB gray scale.

int ARGBGray(uint8* dst_argb, int dst_stride_argb,
             int x, int y, int width, int height);

// Make a rectangle of ARGB Sepia tone.

int ARGBSepia(uint8* dst_argb, int dst_stride_argb,
              int x, int y, int width, int height);

// Apply a matrix rotation to each ARGB pixel.
// matrix_argb is 4 signed ARGB values. -128 to 127 representing -2 to 2.
// The first 4 coefficients apply to B, G, R, A and produce B of the output.
// The next 4 coefficients apply to B, G, R, A and produce G of the output.
// The next 4 coefficients apply to B, G, R, A and produce R of the output.
// The last 4 coefficients apply to B, G, R, A and produce A of the output.

int ARGBColorMatrix(const uint8* src_argb, int src_stride_argb,
                    uint8* dst_argb, int dst_stride_argb,
                    const int8* matrix_argb,
                    int width, int height);

// Deprecated. Use ARGBColorMatrix instead.
// Apply a matrix rotation to each ARGB pixel.
// matrix_argb is 3 signed ARGB values. -128 to 127 representing -1 to 1.
// The first 4 coefficients apply to B, G, R, A and produce B of the output.
// The next 4 coefficients apply to B, G, R, A and produce G of the output.
// The last 4 coefficients apply to B, G, R, A and produce R of the output.

int RGBColorMatrix(uint8* dst_argb, int dst_stride_argb,
                   const int8* matrix_rgb,
                   int x, int y, int width, int height);

// Apply a color table each ARGB pixel.
// Table contains 256 ARGB values.

int ARGBColorTable(uint8* dst_argb, int dst_stride_argb,
                   const uint8* table_argb,
                   int x, int y, int width, int height);

// Apply a color table each ARGB pixel but preserve destination alpha.
// Table contains 256 ARGB values.

int RGBColorTable(uint8* dst_argb, int dst_stride_argb,
                  const uint8* table_argb,
                  int x, int y, int width, int height);

// Apply a luma/color table each ARGB pixel but preserve destination alpha.
// Table contains 32768 values indexed by [Y][C] where 7 it 7 bit luma from
// RGB (YJ style) and C is an 8 bit color component (R, G or B).

int ARGBLumaColorTable(const uint8* src_argb, int src_stride_argb,
                       uint8* dst_argb, int dst_stride_argb,
                       const uint8* luma_rgb_table,
                       int width, int height);

// Apply a 3 term polynomial to ARGB values.
// poly points to a 4x4 matrix.  The first row is constants.  The 2nd row is
// coefficients for b, g, r and a.  The 3rd row is coefficients for b squared,
// g squared, r squared and a squared.  The 4rd row is coefficients for b to
// the 3, g to the 3, r to the 3 and a to the 3.  The values are summed and
// result clamped to 0 to 255.
// A polynomial approximation can be dirived using software such as 'R'.


int ARGBPolynomial(const uint8* src_argb, int src_stride_argb,
                   uint8* dst_argb, int dst_stride_argb,
                   const float* poly,
                   int width, int height);

// Quantize a rectangle of ARGB. Alpha unaffected.
// scale is a 16 bit fractional fixed point scaler between 0 and 65535.
// interval_size should be a value between 1 and 255.
// interval_offset should be a value between 0 and 255.

int ARGBQuantize(uint8* dst_argb, int dst_stride_argb,
                 int scale, int interval_size, int interval_offset,
                 int x, int y, int width, int height);

// Copy ARGB to ARGB.

int ARGBCopy(const uint8* src_argb, int src_stride_argb,
             uint8* dst_argb, int dst_stride_argb,
             int width, int height);

// Copy ARGB to ARGB.

int ARGBCopyAlpha(const uint8* src_argb, int src_stride_argb,
                  uint8* dst_argb, int dst_stride_argb,
                  int width, int height);

// Copy ARGB to ARGB.

int ARGBCopyYToAlpha(const uint8* src_y, int src_stride_y,
                     uint8* dst_argb, int dst_stride_argb,
                     int width, int height);

typedef void (*ARGBBlendRow)(const uint8* src_argb0, const uint8* src_argb1,
                             uint8* dst_argb, int width);

// Get function to Alpha Blend ARGB pixels and store to destination.

ARGBBlendRow GetARGBBlend();

// Alpha Blend ARGB images and store to destination.
// Alpha of destination is set to 255.

int ARGBBlend(const uint8* src_argb0, int src_stride_argb0,
              const uint8* src_argb1, int src_stride_argb1,
              uint8* dst_argb, int dst_stride_argb,
              int width, int height);

// Multiply ARGB image by ARGB image. Shifted down by 8. Saturates to 255.

int ARGBMultiply(const uint8* src_argb0, int src_stride_argb0,
                 const uint8* src_argb1, int src_stride_argb1,
                 uint8* dst_argb, int dst_stride_argb,
                 int width, int height);

// Add ARGB image with ARGB image. Saturates to 255.

int ARGBAdd(const uint8* src_argb0, int src_stride_argb0,
            const uint8* src_argb1, int src_stride_argb1,
            uint8* dst_argb, int dst_stride_argb,
            int width, int height);

// Subtract ARGB image (argb1) from ARGB image (argb0). Saturates to 0.

int ARGBSubtract(const uint8* src_argb0, int src_stride_argb0,
                 const uint8* src_argb1, int src_stride_argb1,
                 uint8* dst_argb, int dst_stride_argb,
                 int width, int height);

// Convert I422 to YUY2.

int I422ToYUY2(const uint8* src_y, int src_stride_y,
               const uint8* src_u, int src_stride_u,
               const uint8* src_v, int src_stride_v,
               uint8* dst_frame, int dst_stride_frame,
               int width, int height);

// Convert I422 to UYVY.

int I422ToUYVY(const uint8* src_y, int src_stride_y,
               const uint8* src_u, int src_stride_u,
               const uint8* src_v, int src_stride_v,
               uint8* dst_frame, int dst_stride_frame,
               int width, int height);

// Convert unattentuated ARGB to preattenuated ARGB.

int ARGBAttenuate(const uint8* src_argb, int src_stride_argb,
                  uint8* dst_argb, int dst_stride_argb,
                  int width, int height);

// Convert preattentuated ARGB to unattenuated ARGB.

int ARGBUnattenuate(const uint8* src_argb, int src_stride_argb,
                    uint8* dst_argb, int dst_stride_argb,
                    int width, int height);

// Convert MJPG to ARGB.



// Internal function - do not call directly.
// Computes table of cumulative sum for image where the value is the sum
// of all values above and to the left of the entry. Used by ARGBBlur.

int ARGBComputeCumulativeSum(const uint8* src_argb, int src_stride_argb,
                             int32* dst_cumsum, int dst_stride32_cumsum,
                             int width, int height);

// Blur ARGB image.
// dst_cumsum table of width * (height + 1) * 16 bytes aligned to
//   16 byte boundary.
// dst_stride32_cumsum is number of ints in a row (width * 4).
// radius is number of pixels around the center.  e.g. 1 = 3x3. 2=5x5.
// Blur is optimized for radius of 5 (11x11) or less.

int ARGBBlur(const uint8* src_argb, int src_stride_argb,
             uint8* dst_argb, int dst_stride_argb,
             int32* dst_cumsum, int dst_stride32_cumsum,
             int width, int height, int radius);

// Multiply ARGB image by ARGB value.

int ARGBShade(const uint8* src_argb, int src_stride_argb,
              uint8* dst_argb, int dst_stride_argb,
              int width, int height, uint32 value);

// Interpolate between two ARGB images using specified amount of interpolation
// (0 to 255) and store to destination.
// 'interpolation' is specified as 8 bit fraction where 0 means 100% src_argb0
// and 255 means 1% src_argb0 and 99% src_argb1.
// Internally uses ARGBScale bilinear filtering.
// Caveat: This function will write up to 16 bytes beyond the end of dst_argb.

int ARGBInterpolate(const uint8* src_argb0, int src_stride_argb0,
                    const uint8* src_argb1, int src_stride_argb1,
                    uint8* dst_argb, int dst_stride_argb,
                    int width, int height, int interpolation);

#if defined(__pnacl__) || defined(__CLR_VER) || defined(COVERAGE_ENABLED) || \
    defined(TARGET_IPHONE_SIMULATOR) || \
    (defined(__i386__) && !defined(__SSE2__)) || \
    ((defined(_MSC_VER) && !defined(__clang__)) && defined(__clang__))
#define LIBYUV_DISABLE_X86
#endif

// Row functions for copying a pixels from a source with a slope to a row
// of destination. Useful for scaling, rotation, mirror, texture mapping.

void ARGBAffineRow_C(const uint8* src_argb, int src_argb_stride,
                     uint8* dst_argb, const float* uv_dudv, int width);
// The following are available on all x86 platforms:
#if !defined(LIBYUV_DISABLE_X86) && \
    (defined(_M_IX86) || defined(__x86_64__) || defined(__i386__))

void ARGBAffineRow_SSE2(const uint8* src_argb, int src_argb_stride,
                        uint8* dst_argb, const float* uv_dudv, int width);
#define HAS_ARGBAFFINEROW_SSE2
#endif  // LIBYUV_DISABLE_X86

// Shuffle ARGB channel order.  e.g. BGRA to ARGB.
// shuffler is 16 bytes and must be aligned.

int ARGBShuffle(const uint8* src_bgra, int src_stride_bgra,
                uint8* dst_argb, int dst_stride_argb,
                const uint8* shuffler, int width, int height);

// Sobel ARGB effect with planar output.

int ARGBSobelToPlane(const uint8* src_argb, int src_stride_argb,
                     uint8* dst_y, int dst_stride_y,
                     int width, int height);

// Sobel ARGB effect.

int ARGBSobel(const uint8* src_argb, int src_stride_argb,
              uint8* dst_argb, int dst_stride_argb,
              int width, int height);

// Sobel ARGB effect w/ Sobel X, Sobel, Sobel Y in ARGB.

int ARGBSobelXY(const uint8* src_argb, int src_stride_argb,
                uint8* dst_argb, int dst_stride_argb,
                int width, int height);

#ifdef __cplusplus
}  // extern "C"
}  // namespace libyuv
#endif

#endif  // INCLUDE_LIBYUV_PLANAR_FUNCTIONS_H_  NOLINT
