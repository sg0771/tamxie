#ifdef __arm64__

#include <stdint.h>

#include "libavutil/attributes.h"
#include "libavutil/aarch64/cpu.h"
#include "libavcodec/avcodec.h"
#include "libavcodec/fmtconvert.h"

void ff_int32_to_float_fmul_array8_neon(FmtConvertContext *c, float *dst,
                                        const int32_t *src, const float *mul,
                                        int len);
void ff_int32_to_float_fmul_scalar_neon(float *dst, const int32_t *src,
                                        float mul, int len);

av_cold void ff_fmt_convert_init_aarch64(FmtConvertContext *c,
                                         AVCodecContext *avctx)
{
    int cpu_flags = av_get_cpu_flags();

    if (have_neon(cpu_flags)) {
        c->int32_to_float_fmul_array8 = ff_int32_to_float_fmul_array8_neon;
        c->int32_to_float_fmul_scalar = ff_int32_to_float_fmul_scalar_neon;
    }
}


#endif