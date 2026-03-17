#ifdef __arm64__

#include "ffmpeg-config.h"

#include "libavutil/aarch64/cpu.h"
#include "libavutil/attributes.h"
#include "libavutil/internal.h"
#include "libavcodec/fft.h"
#include "libavcodec/synth_filter.h"

#include "asm-offsets.h"

#if HAVE_NEON || HAVE_VFP
AV_CHECK_OFFSET(FFTContext, imdct_half, IMDCT_HALF);
#endif

void ff_synth_filter_float_neon(FFTContext *imdct,
                                float *synth_buf_ptr, int *synth_buf_offset,
                                float synth_buf2[32], const float window[512],
                                float out[32], const float in[32],
                                float scale);

av_cold void ff_synth_filter_init_aarch64(SynthFilterContext *s)
{
    int cpu_flags = av_get_cpu_flags();

    if (have_neon(cpu_flags))
        s->synth_filter_float = ff_synth_filter_float_neon;
}

#endif
