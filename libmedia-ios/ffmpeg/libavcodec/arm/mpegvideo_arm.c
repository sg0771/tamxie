#ifdef __arm__

#include "libavutil/internal.h"
#include "libavutil/arm/cpu.h"
#include "libavcodec/avcodec.h"
#include "libavcodec/mpegvideo.h"
#include "mpegvideo_arm.h"
#include "asm-offsets.h"

#if HAVE_NEON
AV_CHECK_OFFSET(MpegEncContext, y_dc_scale,       Y_DC_SCALE);
AV_CHECK_OFFSET(MpegEncContext, c_dc_scale,       C_DC_SCALE);
AV_CHECK_OFFSET(MpegEncContext, ac_pred,          AC_PRED);
AV_CHECK_OFFSET(MpegEncContext, block_last_index, BLOCK_LAST_INDEX);
AV_CHECK_OFFSET(MpegEncContext, inter_scantable.raster_end,
                INTER_SCANTAB_RASTER_END);
AV_CHECK_OFFSET(MpegEncContext, h263_aic,         H263_AIC);
#endif

void ff_dct_unquantize_h263_inter_neon(MpegEncContext *s, int16_t *block,
                                       int n, int qscale);
void ff_dct_unquantize_h263_intra_neon(MpegEncContext *s, int16_t *block,
                                       int n, int qscale);

av_cold void ff_mpv_common_init_arm(MpegEncContext *s)
{
    int cpu_flags = av_get_cpu_flags();

    if (have_armv5te(cpu_flags))
        ff_mpv_common_init_armv5te(s);

    if (have_neon(cpu_flags)) {
        s->dct_unquantize_h263_intra = ff_dct_unquantize_h263_intra_neon;
        s->dct_unquantize_h263_inter = ff_dct_unquantize_h263_inter_neon;
    }
}

#endif
