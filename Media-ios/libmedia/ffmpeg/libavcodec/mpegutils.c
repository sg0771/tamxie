#include <stdint.h>
#include "libavutil/common.h"
#include "libavutil/frame.h"
#include "libavutil/pixdesc.h"
#include "avcodec.h"
#include "mpegutils.h"

void ff_draw_horiz_band(AVCodecContext *avctx,
                        AVFrame *cur, AVFrame *last,
                        int y, int h, int picture_structure,
                        int first_field, int low_delay)
{
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(avctx->pix_fmt);
    int vshift = desc->log2_chroma_h;
    const int field_pic = picture_structure != PICT_FRAME;
    if (field_pic) {
        h <<= 1;
        y <<= 1;
    }

    h = FFMIN(h, avctx->height - y);

    if (field_pic && first_field &&
        !(avctx->slice_flags & SLICE_FLAG_ALLOW_FIELD))
        return;

    if (avctx->draw_horiz_band) {
        AVFrame *src;
        int offset[AV_NUM_DATA_POINTERS];
        int i;

        if (cur->pict_type == AV_PICTURE_TYPE_B || low_delay ||
           (avctx->slice_flags & SLICE_FLAG_CODED_ORDER))
            src = cur;
        else if (last)
            src = last;
        else
            return;

        if (cur->pict_type == AV_PICTURE_TYPE_B &&
            picture_structure == PICT_FRAME &&
            avctx->codec_id != AV_CODEC_ID_SVQ3) {
            for (i = 0; i < AV_NUM_DATA_POINTERS; i++)
                offset[i] = 0;
        } else {
            offset[0]= y * src->linesize[0];
            offset[1]=
            offset[2]= (y >> vshift) * src->linesize[1];
            for (i = 3; i < AV_NUM_DATA_POINTERS; i++)
                offset[i] = 0;
        }

        emms_c();

        avctx->draw_horiz_band(avctx, src, offset,
                               y, picture_structure, h);
    }
}
