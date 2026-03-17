/*
 *  Copyright 2012 The LibYuv Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS. All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef INCLUDE_LIBYUV_CONVERT_FROM_ARGB_H_  // NOLINT
#define INCLUDE_LIBYUV_CONVERT_FROM_ARGB_H_

#include "./basic_types.h"

#ifdef __cplusplus
namespace libyuv {
extern "C" {
#endif

// Copy ARGB to ARGB.
#define ARGBToARGB ARGBCopy

int ARGBCopy(const uint8* src_argb, int src_stride_argb,
             uint8* dst_argb, int dst_stride_argb,
             int width, int height);

// Convert ARGB To BGRA.

int ARGBToBGRA(const uint8* src_argb, int src_stride_argb,
               uint8* dst_bgra, int dst_stride_bgra,
               int width, int height);

// Convert ARGB To ABGR.

int ARGBToABGR(const uint8* src_argb, int src_stride_argb,
               uint8* dst_abgr, int dst_stride_abgr,
               int width, int height);

// Convert ARGB To RGBA.

int ARGBToRGBA(const uint8* src_argb, int src_stride_argb,
               uint8* dst_rgba, int dst_stride_rgba,
               int width, int height);

// Convert ARGB To RGB24.

int ARGBToRGB24(const uint8* src_argb, int src_stride_argb,
                uint8* dst_rgb24, int dst_stride_rgb24,
                int width, int height);

// Convert ARGB To RAW.

int ARGBToRAW(const uint8* src_argb, int src_stride_argb,
              uint8* dst_rgb, int dst_stride_rgb,
              int width, int height);

// Convert ARGB To RGB565.

int ARGBToRGB565(const uint8* src_argb, int src_stride_argb,
                 uint8* dst_rgb565, int dst_stride_rgb565,
                 int width, int height);

// Convert ARGB To RGB565 with 4x4 dither matrix (16 bytes).
// Values in dither matrix from 0 to 7 recommended.
// The order of the dither matrix is first byte is upper left.
// TODO(fbarchard): Consider pointer to 2d array for dither4x4.
// const uint8(*dither)[4][4];

int ARGBToRGB565Dither(const uint8* src_argb, int src_stride_argb,
                       uint8* dst_rgb565, int dst_stride_rgb565,
                       const uint8* dither4x4, int width, int height);

// Convert ARGB To ARGB1555.

int ARGBToARGB1555(const uint8* src_argb, int src_stride_argb,
                   uint8* dst_argb1555, int dst_stride_argb1555,
                   int width, int height);

// Convert ARGB To ARGB4444.

int ARGBToARGB4444(const uint8* src_argb, int src_stride_argb,
                   uint8* dst_argb4444, int dst_stride_argb4444,
                   int width, int height);

// Convert ARGB To I444.

int ARGBToI444(const uint8* src_argb, int src_stride_argb,
               uint8* dst_y, int dst_stride_y,
               uint8* dst_u, int dst_stride_u,
               uint8* dst_v, int dst_stride_v,
               int width, int height);

// Convert ARGB To I422.

int ARGBToI422(const uint8* src_argb, int src_stride_argb,
               uint8* dst_y, int dst_stride_y,
               uint8* dst_u, int dst_stride_u,
               uint8* dst_v, int dst_stride_v,
               int width, int height);

// Convert ARGB To I420. (also in convert.h)

int ARGBToI420(const uint8* src_argb, int src_stride_argb,
               uint8* dst_y, int dst_stride_y,
               uint8* dst_u, int dst_stride_u,
               uint8* dst_v, int dst_stride_v,
               int width, int height);

// Convert ARGB to J420. (JPeg full range I420).

int ARGBToJ420(const uint8* src_argb, int src_stride_argb,
               uint8* dst_yj, int dst_stride_yj,
               uint8* dst_u, int dst_stride_u,
               uint8* dst_v, int dst_stride_v,
               int width, int height);

// Convert ARGB to J422.

int ARGBToJ422(const uint8* src_argb, int src_stride_argb,
               uint8* dst_yj, int dst_stride_yj,
               uint8* dst_u, int dst_stride_u,
               uint8* dst_v, int dst_stride_v,
               int width, int height);

// Convert ARGB To I411.

int ARGBToI411(const uint8* src_argb, int src_stride_argb,
               uint8* dst_y, int dst_stride_y,
               uint8* dst_u, int dst_stride_u,
               uint8* dst_v, int dst_stride_v,
               int width, int height);

// Convert ARGB to J400. (JPeg full range).

int ARGBToJ400(const uint8* src_argb, int src_stride_argb,
               uint8* dst_yj, int dst_stride_yj,
               int width, int height);

// Convert ARGB to I400.

int ARGBToI400(const uint8* src_argb, int src_stride_argb,
               uint8* dst_y, int dst_stride_y,
               int width, int height);

// Convert ARGB to G. (Reverse of J400toARGB, which replicates G back to ARGB)

int ARGBToG(const uint8* src_argb, int src_stride_argb,
            uint8* dst_g, int dst_stride_g,
            int width, int height);

// Convert ARGB To NV12.

int ARGBToNV12(const uint8* src_argb, int src_stride_argb,
               uint8* dst_y, int dst_stride_y,
               uint8* dst_uv, int dst_stride_uv,
               int width, int height);

// Convert ARGB To NV21.

int ARGBToNV21(const uint8* src_argb, int src_stride_argb,
               uint8* dst_y, int dst_stride_y,
               uint8* dst_vu, int dst_stride_vu,
               int width, int height);

// Convert ARGB To NV21.

int ARGBToNV21(const uint8* src_argb, int src_stride_argb,
               uint8* dst_y, int dst_stride_y,
               uint8* dst_vu, int dst_stride_vu,
               int width, int height);

// Convert ARGB To YUY2.

int ARGBToYUY2(const uint8* src_argb, int src_stride_argb,
               uint8* dst_yuy2, int dst_stride_yuy2,
               int width, int height);

// Convert ARGB To UYVY.

int ARGBToUYVY(const uint8* src_argb, int src_stride_argb,
               uint8* dst_uyvy, int dst_stride_uyvy,
               int width, int height);

#ifdef __cplusplus
}  // extern "C"
}  // namespace libyuv
#endif

#endif  // INCLUDE_LIBYUV_CONVERT_FROM_ARGB_H_  NOLINT
