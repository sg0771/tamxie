#include "ffmpeg-config.h"

#include "libavutil/arm/cpu.h"
#include "libavutil/attributes.h"
#include "libavutil/internal.h"
#include "libavcodec/fft.h"
#include "libavcodec/synth_filter.h"

void ff_synth_filter_float_vfp(FFTContext *imdct,
                               float *synth_buf_ptr, int *synth_buf_offset,
                               float synth_buf2[32], const float window[512],
                               float out[32], const float in[32],
                               float scale);

void ff_synth_filter_float_neon(FFTContext *imdct,
                                float *synth_buf_ptr, int *synth_buf_offset,
                                float synth_buf2[32], const float window[512],
                                float out[32], const float in[32],
                                float scale);

av_cold void ff_synth_filter_init_arm(SynthFilterContext *s)
{
    int cpu_flags = av_get_cpu_flags();

    if (have_vfp_vm(cpu_flags))
        s->synth_filter_float = ff_synth_filter_float_vfp;
    if (have_neon(cpu_flags))
        s->synth_filter_float = ff_synth_filter_float_neon;
}
