/*
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "config.h"

#include "libavutil/aarch64/cpu.h"
#include "libavcodec/aacpsdsp.h"

#include <stdint.h>

void ff_ps_add_squares_neon(float *dst, const float (*src)[2], int n);
void ff_ps_mul_pair_single_neon(float (*dst)[2], float (*src0)[2],
                                float *src1, int n);
void ff_ps_hybrid_analysis_neon(float (*out)[2], float (*in)[2],
                                const float (*filter)[8][2],
                                ptrdiff_t stride, int n);
void ff_ps_stereo_interpolate_neon(float (*l)[2], float (*r)[2],
                                   float h[2][4], float h_step[2][4],
                                   int len);
void ff_ps_stereo_interpolate_ipdopd_neon(float (*l)[2], float (*r)[2],
                                          float h[2][4], float h_step[2][4],
                                          int len);

av_cold void ff_psdsp_init_aarch64(PSDSPContext *s)
{
    int cpu_flags = av_get_cpu_flags();

    if (have_neon(cpu_flags)) {
        s->add_squares           = ff_ps_add_squares_neon;
        s->mul_pair_single       = ff_ps_mul_pair_single_neon;
        s->hybrid_analysis       = ff_ps_hybrid_analysis_neon;
        s->stereo_interpolate[0] = ff_ps_stereo_interpolate_neon;
        s->stereo_interpolate[1] = ff_ps_stereo_interpolate_ipdopd_neon;
    }
}


// 使用 __attribute__((aligned(16))) 代替 alignas
// 确保不加 static，这样汇编代码才能作为外部符号访问它
const uint8_t my_scan8[] __attribute__((aligned(16))) = {
    4+ 1*8, 5+ 1*8, 4+ 2*8, 5+ 2*8,
    6+ 1*8, 7+ 1*8, 6+ 2*8, 7+ 2*8,
    4+ 3*8, 5+ 3*8, 4+ 4*8, 5+ 4*8,
    6+ 3*8, 7+ 3*8, 6+ 4*8, 7+ 4*8,
    4+ 6*8, 5+ 6*8, 4+ 7*8, 5+ 7*8,
    6+ 6*8, 7+ 6*8, 6+ 7*8, 7+ 7*8,
    4+ 8*8, 5+ 8*8, 4+ 9*8, 5+ 9*8,
    6+ 8*8, 7+ 8*8, 6+ 9*8, 7+ 9*8,
    4+11*8, 5+11*8, 4+12*8, 5+12*8,
    6+11*8, 7+11*8, 6+12*8, 7+12*8,
    4+13*8, 5+13*8, 4+14*8, 5+14*8,
    6+13*8, 7+13*8, 6+14*8, 7+14*8
};