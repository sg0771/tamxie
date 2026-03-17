/*
以静态库方式调用frei0r的filter
*/

#include "config.h"
#if CONFIG_FREI0R

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "config.h"
#include "libavutil/avstring.h"
#include "libavutil/common.h"
#include "libavutil/eval.h"
#include "libavutil/imgutils.h"
#include "libavutil/internal.h"
#include "libavutil/mathematics.h"
#include "libavutil/mem.h"
#include "libavutil/opt.h"
#include "libavutil/parseutils.h"
#include "avfilter.h"
#include "formats.h"
#include "internal.h"
#include "video.h"


#include <frei0r.h>
//内置滤镜

#ifdef _WIN32
#include <string.h>
#define strcasecmp stricmp
#define strncasecmp strnicmp
#endif

extern f0r_plugin_info_t _3dflippoInfo;
extern f0r_plugin_info_t _BInfo;
extern f0r_plugin_info_t _GInfo;
extern f0r_plugin_info_t _IIRblurInfo;
extern f0r_plugin_info_t _RInfo;
extern f0r_plugin_info_t _RGBInfo;
extern f0r_plugin_info_t _alpha0psInfo;
extern f0r_plugin_info_t _alphagradInfo;
extern f0r_plugin_info_t _alphainjectionInfo;
extern f0r_plugin_info_t _alphaspotInfo;
extern f0r_plugin_info_t _balanc0rInfo;
extern f0r_plugin_info_t _bgsubtract0rInfo;
extern f0r_plugin_info_t _brightnessInfo;
extern f0r_plugin_info_t _bw0rInfo;
extern f0r_plugin_info_t _c0rnersInfo;
extern f0r_plugin_info_t _clusterInfo;
extern f0r_plugin_info_t _colgateInfo;
extern f0r_plugin_info_t _coloradj_RGBInfo;
extern f0r_plugin_info_t _colordistanceInfo;
extern f0r_plugin_info_t _colorhalftoneInfo;
extern f0r_plugin_info_t _colorizeInfo;
extern f0r_plugin_info_t _colortapInfo;
extern f0r_plugin_info_t _compositionInfo;
extern f0r_plugin_info_t _contrast0rInfo;
extern f0r_plugin_info_t _curvesInfo;
extern f0r_plugin_info_t _defish0rInfo;
extern f0r_plugin_info_t _distort0rInfo;
extern f0r_plugin_info_t _ditherInfo;
extern f0r_plugin_info_t _embossInfo;
extern f0r_plugin_info_t _flippoInfo;
extern f0r_plugin_info_t _gammaInfo;
extern f0r_plugin_info_t _glitch0rInfo;
extern f0r_plugin_info_t _glowInfo;
extern f0r_plugin_info_t _host_param_testInfo;
extern f0r_plugin_info_t _hqdn3dInfo;
extern f0r_plugin_info_t _hueshift0rInfo;
extern f0r_plugin_info_t _invert0rInfo;
extern f0r_plugin_info_t _ising0rInfo;
extern f0r_plugin_info_t _keyspillm0pupInfo;
extern f0r_plugin_info_t _lenscorrectionInfo;
extern f0r_plugin_info_t _letterb0xedInfo;
extern f0r_plugin_info_t _levelsInfo;
extern f0r_plugin_info_t _luminanceInfo;
extern f0r_plugin_info_t _mask0mateInfo;
extern f0r_plugin_info_t _mediansInfo;
extern f0r_plugin_info_t _perspectiveInfo;
extern f0r_plugin_info_t _pixeliz0rInfo;
extern f0r_plugin_info_t _posterizeInfo;
extern f0r_plugin_info_t _pr0beInfo;
extern f0r_plugin_info_t _pr0fileInfo;
extern f0r_plugin_info_t _rgbnoiseInfo;
extern f0r_plugin_info_t _rgbsplit0rInfo;
extern f0r_plugin_info_t _saturat0rInfo;
extern f0r_plugin_info_t _select0rInfo;
extern f0r_plugin_info_t _sharpnessInfo;
extern f0r_plugin_info_t _sigmoidaltransferInfo;
extern f0r_plugin_info_t _softglowInfo;
extern f0r_plugin_info_t _spillsupressInfo;
extern f0r_plugin_info_t _squareblurInfo;
extern f0r_plugin_info_t _tehRoxx0rInfo;
extern f0r_plugin_info_t _test_pat_BInfo;
extern f0r_plugin_info_t _test_pat_CInfo;
extern f0r_plugin_info_t _test_pat_GInfo;
extern f0r_plugin_info_t _test_pat_IInfo;
extern f0r_plugin_info_t _test_pat_LInfo;
extern f0r_plugin_info_t _test_pat_RInfo;
extern f0r_plugin_info_t _three_point_balanceInfo;
extern f0r_plugin_info_t _threshold0rInfo;
extern f0r_plugin_info_t _tint0rInfo;
extern f0r_plugin_info_t _transparencyInfo;
extern f0r_plugin_info_t _uvmapInfo;
extern f0r_plugin_info_t _vertigoInfo;
extern f0r_plugin_info_t _additionInfo;
extern f0r_plugin_info_t _addition_alphaInfo;
extern f0r_plugin_info_t _alphaatopInfo;
extern f0r_plugin_info_t _alphainInfo;
extern f0r_plugin_info_t _alphaoutInfo;
extern f0r_plugin_info_t _alphaoverInfo;
extern f0r_plugin_info_t _alphaxorInfo;
extern f0r_plugin_info_t _baltanInfo;
extern f0r_plugin_info_t _blendInfo;
extern f0r_plugin_info_t _bluescreen0rInfo;
extern f0r_plugin_info_t _burnInfo;
extern f0r_plugin_info_t _cartoonInfo;
extern f0r_plugin_info_t _color_onlyInfo;
extern f0r_plugin_info_t _d90stairsteppingfixInfo;
extern f0r_plugin_info_t _darkenInfo;
extern f0r_plugin_info_t _delay0rInfo;
extern f0r_plugin_info_t _delaygrabInfo;
extern f0r_plugin_info_t _differenceInfo;
extern f0r_plugin_info_t _divideInfo;
extern f0r_plugin_info_t _dodgeInfo;
extern f0r_plugin_info_t _edgeglowInfo;
extern f0r_plugin_info_t _equaliz0rInfo;
extern f0r_plugin_info_t _grain_extractInfo;
extern f0r_plugin_info_t _grain_mergeInfo;
extern f0r_plugin_info_t _hardlightInfo;
extern f0r_plugin_info_t _hueInfo;
extern f0r_plugin_info_t _lightenInfo;
extern f0r_plugin_info_t _lightgraffitiInfo;
extern f0r_plugin_info_t _lissajous0rInfo;
extern f0r_plugin_info_t _multiplyInfo;
extern f0r_plugin_info_t _ndviInfo;
extern f0r_plugin_info_t _nervousInfo;
extern f0r_plugin_info_t _nois0rInfo;
extern f0r_plugin_info_t _nosync0rInfo;
extern f0r_plugin_info_t _onecol0rInfo;
extern f0r_plugin_info_t _overlayInfo;
extern f0r_plugin_info_t _partik0lInfo;
extern f0r_plugin_info_t _plasmaInfo;
extern f0r_plugin_info_t _primariesInfo;
extern f0r_plugin_info_t _saturationInfo;
extern f0r_plugin_info_t _scanline0rInfo;
extern f0r_plugin_info_t _screenInfo;
extern f0r_plugin_info_t _sobelInfo;
extern f0r_plugin_info_t _softlightInfo;
extern f0r_plugin_info_t _sopsatInfo;
extern f0r_plugin_info_t _subtractInfo;
extern f0r_plugin_info_t _threelay0rInfo;
extern f0r_plugin_info_t _timeoutInfo;
extern f0r_plugin_info_t _tutorialInfo;
extern f0r_plugin_info_t _twolay0rInfo;
extern f0r_plugin_info_t _valueInfo;
extern f0r_plugin_info_t _vignetteInfo;
extern f0r_plugin_info_t _xfade0rInfo;

#define _FILTER_NUMBERS 125
static f0r_plugin_info_t *s_array[_FILTER_NUMBERS] = {
	&_3dflippoInfo,
	&_BInfo,
	&_GInfo,
	&_IIRblurInfo,
	&_RInfo,
	&_RGBInfo,
	&_alpha0psInfo,
	&_alphagradInfo,
	&_alphainjectionInfo,
	&_alphaspotInfo,
	&_balanc0rInfo,
	&_bgsubtract0rInfo,
	&_brightnessInfo,
	&_bw0rInfo,
	&_c0rnersInfo,
	&_clusterInfo,
	&_colgateInfo,
	&_coloradj_RGBInfo,
	&_colordistanceInfo,
	&_colorhalftoneInfo,
	&_colorizeInfo,
	&_colortapInfo,
	&_compositionInfo,
	&_contrast0rInfo,
	&_curvesInfo,
	&_defish0rInfo,
	&_distort0rInfo,
	&_ditherInfo,
	&_embossInfo,
	&_flippoInfo,
	&_gammaInfo,
	&_glitch0rInfo,
	&_glowInfo,
	&_host_param_testInfo,
	&_hqdn3dInfo,
	&_hueshift0rInfo,
	&_invert0rInfo,
	&_ising0rInfo,
	&_keyspillm0pupInfo,
	&_lenscorrectionInfo,
	&_letterb0xedInfo,
	&_levelsInfo,
	&_luminanceInfo,
	&_mask0mateInfo,
	&_mediansInfo,
	&_perspectiveInfo,
	&_pixeliz0rInfo,
	&_posterizeInfo,
	&_pr0beInfo,
	&_pr0fileInfo,
	&_rgbnoiseInfo,
	&_rgbsplit0rInfo,
	&_saturat0rInfo,
	&_select0rInfo,
	&_sharpnessInfo,
	&_sigmoidaltransferInfo,
	&_softglowInfo,
	&_spillsupressInfo,
	&_squareblurInfo,
	&_tehRoxx0rInfo,
	&_test_pat_BInfo,
	&_test_pat_CInfo,
	&_test_pat_GInfo,
	&_test_pat_IInfo,
	&_test_pat_LInfo,
	&_test_pat_RInfo,
	&_three_point_balanceInfo,
	&_threshold0rInfo,
	&_tint0rInfo,
	&_transparencyInfo,
	&_uvmapInfo,
	&_vertigoInfo,
	&_additionInfo,
	&_addition_alphaInfo,
	&_alphaatopInfo,
	&_alphainInfo,
	&_alphaoutInfo,
	&_alphaoverInfo,
	&_alphaxorInfo,
	&_baltanInfo,
	&_blendInfo,
	&_bluescreen0rInfo,
	&_burnInfo,
	&_cartoonInfo,
	&_color_onlyInfo,
	&_d90stairsteppingfixInfo,
	&_darkenInfo,
	&_delay0rInfo,
	&_delaygrabInfo,
	&_differenceInfo,
	&_divideInfo,
	&_dodgeInfo,
	&_edgeglowInfo,
	&_equaliz0rInfo,
	&_grain_extractInfo,
	&_grain_mergeInfo,
	&_hardlightInfo,
	&_hueInfo,
	&_lightenInfo,
	&_lightgraffitiInfo,
	&_lissajous0rInfo,
	&_multiplyInfo,
	&_ndviInfo,
	&_nervousInfo,
	&_nois0rInfo,
	&_nosync0rInfo,
	&_onecol0rInfo,
	&_overlayInfo,
	&_partik0lInfo,
	&_plasmaInfo,
	&_primariesInfo,
	&_saturationInfo,
	&_scanline0rInfo,
	&_screenInfo,
	&_sobelInfo,
	&_softlightInfo,
	&_sopsatInfo,
	&_subtractInfo,
	&_threelay0rInfo,
	&_timeoutInfo,
	&_tutorialInfo,
	&_twolay0rInfo,
	&_valueInfo,
	&_vignetteInfo,
	&_xfade0rInfo,
};

static  f0r_plugin_info_t* avfilter_Feri0r_GetInfo(const char*name) {
	for (int i = 0;i < _FILTER_NUMBERS;i++) {
		av_log(NULL, AV_LOG_ERROR, "Find '%s  s[%d]=%s'.\n",name, i,  s_array[i]->name);
		if (strcasecmp(s_array[i]->name, name) == 0) {
			return s_array[i];
		}
	}
	return NULL;
}

typedef struct Frei0rContext {
	const AVClass *class;
	f0r_instance_t instance;
	f0r_plugin_info_t *m_info; //Create By Array
	char *dl_name;
	char *params;
	AVRational framerate;

	/* only used by the source */
	int w, h;
	AVRational time_base;
	uint64_t pts;
} Frei0rContext;


static int set_param(AVFilterContext *ctx, f0r_param_info_t info, int index, char *param)
{
    Frei0rContext *s = ctx->priv;
    union {
        double d;
        f0r_param_color_t col;
        f0r_param_position_t pos;
    } val;
    char *tail;
    uint8_t rgba[4];

    switch (info.type) {
    case F0R_PARAM_BOOL:
        if      (!strcmp(param, "y")) val.d = 1.0;
        else if (!strcmp(param, "n")) val.d = 0.0;
        else goto fail;
        break;

    case F0R_PARAM_DOUBLE:
        val.d = av_strtod(param, &tail);
        if (*tail || val.d == HUGE_VAL)
            goto fail;
        break;

    case F0R_PARAM_COLOR:
        if (sscanf(param, "%f/%f/%f", &val.col.r, &val.col.g, &val.col.b) != 3) {
            if (av_parse_color(rgba, param, -1, ctx) < 0)
                goto fail;
            val.col.r = rgba[0] / 255.0;
            val.col.g = rgba[1] / 255.0;
            val.col.b = rgba[2] / 255.0;
        }
        break;

    case F0R_PARAM_POSITION:
        if (sscanf(param, "%lf/%lf", &val.pos.x, &val.pos.y) != 2)
            goto fail;
        break;
    }

av_log(ctx, AV_LOG_ERROR, "set_param f_set_param_value OK.\n");
    s->m_info->f_set_param_value(s->instance, &val, index);
    return 0;

fail:
    av_log(ctx, AV_LOG_ERROR, "Invalid value '%s' for parameter '%s'.\n",
           param, info.name);
    return AVERROR(EINVAL);
}

static int set_params(AVFilterContext *ctx, const char *params){
    Frei0rContext *s = ctx->priv;
    int i;
    if (!params)
        return 0;
    for (i = 0; i < s->m_info->num_params; i++) {
        f0r_param_info_t info;
        char *param;
        int ret;
	s->m_info->f_get_param_info(&info, i);
        if (*params) {
            if (!(param = av_get_token(&params, "|")))
                return AVERROR(ENOMEM);
            if (*params)
                params++;               /* skip ':' */
            ret = set_param(ctx, info, i, param);
            av_free(param);
            if (ret < 0)
                return ret;
        }
    }
    return 0;
}

static av_cold int frei0r_init(AVFilterContext *ctx,const char *dl_name, int type)
{
	Frei0rContext *s = ctx->priv;
	s->m_info = avfilter_Feri0r_GetInfo(s->dl_name);//根据名字，知道filter的相关描述
	if (s->m_info == NULL) {
		av_log(ctx, AV_LOG_ERROR, "Could not Find the frei0r module.  =  %s\n", s->dl_name);
		return AVERROR(EINVAL);
	}else{
		av_log(ctx, AV_LOG_ERROR, "!!!!!!!!!!!!!!!Find the frei0r module.  =  %s\n", s->dl_name);
		s->m_info->f_get_plugin_info(s->m_info);//获取具体属性
		
		av_log(ctx, AV_LOG_ERROR, "____ Name =  %s name=%s author==%s  EXT=%s\n", 
		s->dl_name,
		s->m_info->name,
		s->m_info->author,
		s->m_info->explanation);
	}
	
	if (s->m_info->f_init() < 0) { //filter 初始化函数
		av_log(ctx, AV_LOG_ERROR, "Could not init the frei0r module.\n");
		return AVERROR(EINVAL);
	}else{
		av_log(ctx, AV_LOG_ERROR, "_____INIT the frei0r module.\n");
	} 
	
	av_log(ctx, AV_LOG_ERROR, "_filter init func.\n");
		
	if (s->m_info->plugin_type != type) {
		av_log(ctx, AV_LOG_ERROR,
               "Invalid type '%s' for this plugin\n",
               s->m_info->plugin_type == F0R_PLUGIN_TYPE_FILTER ? "filter" :
               s->m_info->plugin_type == F0R_PLUGIN_TYPE_SOURCE ? "source" :
               s->m_info->plugin_type == F0R_PLUGIN_TYPE_MIXER2 ? "mixer2" :
               s->m_info->plugin_type == F0R_PLUGIN_TYPE_MIXER3 ? "mixer3" : "unknown");
		return AVERROR(EINVAL);
	}
	av_log(ctx, AV_LOG_ERROR, "__ddddd___INIT the frei0r module  OKOKOK.\n");
	return 0;
}

static av_cold int filter_init(AVFilterContext *ctx){
	Frei0rContext *s = ctx->priv;
	av_log(ctx, AV_LOG_ERROR, "__filter_init.\n");
	return frei0r_init(ctx, s->dl_name, F0R_PLUGIN_TYPE_FILTER);
}

static av_cold void uninit(AVFilterContext *ctx)
{
	Frei0rContext *s = ctx->priv;

        av_log(ctx, AV_LOG_ERROR, "uninit.\n");
        
	if (s->m_info->f_destruct && s->instance)
		s->m_info->f_destruct(s->instance);
		
	if (s->m_info->f_deinit)
		s->m_info->f_deinit();
}

static int config_input_props(AVFilterLink *inlink)
{
  
        
    AVFilterContext *ctx = inlink->dst;
    Frei0rContext *s = ctx->priv;

      av_log(NULL, AV_LOG_ERROR, "config_input_props.\n");
      
    if (s->m_info->f_destruct && s->instance)
		s->m_info->f_destruct(s->instance);

    if (!(s->instance = s->m_info->f_construct(inlink->w, inlink->h))) { //构造filter对象
        av_log(ctx, AV_LOG_ERROR, "Impossible to load frei0r instance.\n");
        return AVERROR(EINVAL);
    }else{
            av_log(ctx, AV_LOG_ERROR, "f_construct  OK.\n");
    }

    return set_params(ctx, s->params);
}

static int query_formats(AVFilterContext *ctx)
{

    Frei0rContext *s = ctx->priv;
    AVFilterFormats *formats = NULL;
    int ret;

    if        (s->m_info->color_model == F0R_COLOR_MODEL_BGRA8888) {
        if ((ret = ff_add_format(&formats, AV_PIX_FMT_BGRA)) < 0)
            return ret;
    } else if (s->m_info->color_model == F0R_COLOR_MODEL_RGBA8888) {
        if ((ret = ff_add_format(&formats, AV_PIX_FMT_RGBA)) < 0)
            return ret;
    } else {                                   /* F0R_COLOR_MODEL_PACKED32 */
        static const enum AVPixelFormat pix_fmts[] = {
            AV_PIX_FMT_BGRA, AV_PIX_FMT_ARGB, AV_PIX_FMT_ABGR, AV_PIX_FMT_ARGB, AV_PIX_FMT_NONE
        };
        formats = ff_make_format_list(pix_fmts);
    }

    if (!formats)
        return AVERROR(ENOMEM);

    return ff_set_common_formats(ctx, formats);
}

static int filter_frame(AVFilterLink *inlink, AVFrame *in){
	Frei0rContext *s = inlink->dst->priv;
	AVFilterLink *outlink = inlink->dst->outputs[0];
	AVFrame *out;

	out = ff_get_video_buffer(outlink, outlink->w, outlink->h);
	if (!out) {
		av_frame_free(&in);
		return AVERROR(ENOMEM);
	}
	av_frame_copy_props(out, in);
	s->m_info->f_update(s->instance, 
			in->pts * av_q2d(inlink->time_base) * 1000,
                   (const uint32_t *)in->data[0],
                   (uint32_t *)out->data[0]);
	av_frame_free(&in);
	return ff_filter_frame(outlink, out);
}

#define OFFSET(x) offsetof(Frei0rContext, x)
#define FLAGS AV_OPT_FLAG_VIDEO_PARAM | AV_OPT_FLAG_FILTERING_PARAM
static const AVOption frei0r_options[] = {
    { "filter_name",   NULL, OFFSET(dl_name), AV_OPT_TYPE_STRING, .flags = FLAGS },
    { "filter_params", NULL, OFFSET(params),  AV_OPT_TYPE_STRING, .flags = FLAGS },
    { NULL }
};

AVFILTER_DEFINE_CLASS(frei0r);

static const AVFilterPad avfilter_vf_frei0r_inputs[] = {
    {
        .name         = "default",
        .type         = AVMEDIA_TYPE_VIDEO,
        .config_props = config_input_props,
        .filter_frame = filter_frame,
    },
    { NULL }
};

static const AVFilterPad avfilter_vf_frei0r_outputs[] = {
    {
        .name = "default",
        .type = AVMEDIA_TYPE_VIDEO,
    },
    { NULL }
};

AVFilter ff_vf_frei0r = {
    .name          = "frei0r",
    .description   = NULL_IF_CONFIG_SMALL("Apply a frei0r effect."),
    .query_formats = query_formats,
    .init          = filter_init,
    .uninit        = uninit,
    .priv_size     = sizeof(Frei0rContext),
    .priv_class    = &frei0r_class,
    .inputs        = avfilter_vf_frei0r_inputs,
    .outputs       = avfilter_vf_frei0r_outputs,
};

static av_cold int source_init(AVFilterContext *ctx)
{
    Frei0rContext *s = ctx->priv;

    s->time_base.num = s->framerate.den;
    s->time_base.den = s->framerate.num;

    return frei0r_init(ctx, s->dl_name, F0R_PLUGIN_TYPE_SOURCE);
}

static int source_config_props(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    Frei0rContext *s = ctx->priv;

    if (av_image_check_size(s->w, s->h, 0, ctx) < 0)
        return AVERROR(EINVAL);
    outlink->w = s->w;
    outlink->h = s->h;
    outlink->time_base = s->time_base;
    outlink->frame_rate = av_inv_q(s->time_base);
    outlink->sample_aspect_ratio = (AVRational){1,1};

            av_log(ctx, AV_LOG_ERROR, "source_config_props  OK.\n");
            
            
    if (s->m_info->f_destruct && s->instance)
		s->m_info->f_destruct(s->instance);
    if (!(s->instance = s->m_info->f_construct(outlink->w, outlink->h))) {
        av_log(ctx, AV_LOG_ERROR, "Impossible to load frei0r instance.\n");
        return AVERROR(EINVAL);
    }
    if (!s->params) {
        av_log(ctx, AV_LOG_ERROR, "frei0r filter parameters not set.\n");
        return AVERROR(EINVAL);
    }

    return set_params(ctx, s->params);
}

static int source_request_frame(AVFilterLink *outlink)
{
    Frei0rContext *s = outlink->src->priv;
    AVFrame *frame = ff_get_video_buffer(outlink, outlink->w, outlink->h);

    if (!frame)
        return AVERROR(ENOMEM);

    frame->sample_aspect_ratio = (AVRational) {1, 1};
    frame->pts = s->pts++;

av_log(NULL, AV_LOG_ERROR, "source_request_frame  OK.\n");
	s->m_info->f_update(s->instance, av_rescale_q(frame->pts, s->time_base, (AVRational){1,1000}),
                   NULL, (uint32_t *)frame->data[0]);

    return ff_filter_frame(outlink, frame);
}

static const AVOption frei0r_src_options[] = {
    { "size",          "Dimensions of the generated video.", OFFSET(w),         AV_OPT_TYPE_IMAGE_SIZE, { .str = "320x240" }, .flags = FLAGS },
    { "framerate",     NULL,                                 OFFSET(framerate), AV_OPT_TYPE_VIDEO_RATE, { .str = "25" }, 0, INT_MAX, .flags = FLAGS },
    { "filter_name",   NULL,                                 OFFSET(dl_name),   AV_OPT_TYPE_STRING,                  .flags = FLAGS },
    { "filter_params", NULL,                                 OFFSET(params),    AV_OPT_TYPE_STRING,                  .flags = FLAGS },
    { NULL },
};

AVFILTER_DEFINE_CLASS(frei0r_src);

static const AVFilterPad avfilter_vsrc_frei0r_src_outputs[] = {
    {
        .name          = "default",
        .type          = AVMEDIA_TYPE_VIDEO,
        .request_frame = source_request_frame,
        .config_props  = source_config_props
    },
    { NULL }
};

AVFilter ff_vsrc_frei0r_src = {
    .name          = "frei0r_src",
    .description   = NULL_IF_CONFIG_SMALL("Generate a frei0r source."),
    .priv_size     = sizeof(Frei0rContext),
    .priv_class    = &frei0r_src_class,
    .init          = source_init,
    .uninit        = uninit,
    .query_formats = query_formats,
    .inputs        = NULL,
    .outputs       = avfilter_vsrc_frei0r_src_outputs,
};
#endif
