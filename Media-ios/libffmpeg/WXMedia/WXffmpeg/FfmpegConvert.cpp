/*
ffmpeg 视频转换功能封装
首先要 执行初始化函数 WXMediaFfmpeg_Init()
如果对ffmpeg命令行属性，可以直接调用
WXFFMPEG_CAPI int64_t WXMediaFfmpeg_Convert(int argc, char **argv);
相当于 ffmpeg(argc,argv);
*/


//类成员函数
#include "FfmpegConvertClass.h"


FfmpegConvert::~FfmpegConvert()
{
	if (m_newFrame != nullptr)
		av_free(&m_newFrame);
}

int64_t FfmpegConvert::GetCurrTime() {
	return m_ptsCurr;
}

int64_t FfmpegConvert::GetTotalTime() {
	return m_ptsTotal;
}

int FfmpegConvert::GetState() {
	return 	avffmpeg_state;
}

void FfmpegConvert::Break() {
	received_sigterm = 1;//中断信号
}

void FfmpegConvert::exit_program(int ret)
{
	m_error = ret;
	throw ret;
}

int FfmpegConvert::Convert(int argc, char **argv)
{
	init_dynload();
	avcodec_register_all();
	//avdevice_register_all();
	avfilter_register_all();
	av_register_all();
	avformat_network_init();


	try    //定义异常
	{
		/* parse options and open all input/output files */
		int ret = ffmpeg_parse_options(argc, argv);
		if (ret < 0)
			exit_program(2);

		if (nb_output_files <= 0 && nb_input_files == 0) {
			exit_program(2);
		}

		if (nb_output_files <= 0) {
			exit_program(2);
		}


		for (int i = 0; i < nb_input_files; i++) {
			if (input_files[i]->ctx->duration > 0)
				m_ptsTotal = input_files[0]->ctx->duration / 1000;//总长度
		}

		if (transcode() < 0)
			exit_program(2);

		ffmpeg_cleanup(0);

		av_log(NULL, AV_LOG_ERROR, "OK\r\n");
	}
	catch (...) {
		av_log(NULL, AV_LOG_ERROR, "error\r\n");
		ffmpeg_cleanup(1);

		return -1;
	}
	return 0;
}





void FfmpegConvert::init_dynload(void)
{
#ifdef _WIN32
	/* Calling SetDllDirectory with the empty string (but not NULL) removes the
	* current working directory from the DLL search path as a security pre-caution. */
	SetDllDirectoryA("");
#endif
}



double FfmpegConvert::parse_number_or_die(const char *context, const char *numstr, int type,
	double min, double max)
{
	char *tail;
	const char *error;
	double d = av_strtod(numstr, &tail);
	if (*tail)
		error = "Expected number for %s but found: %s\n";
	else if (d < min || d > max)
		error = "The value for %s was %s which is not within %f - %f\n";
	else if (type == OPT_INT64 && (int64_t)d != d)
		error = "Expected int64 for %s but found %s\n";
	else if (type == OPT_INT && (int)d != d)
		error = "Expected int for %s but found %s\n";
	else
		return d;
	return 0;
}

int64_t FfmpegConvert::parse_time_or_die(const char *context, const char *timestr,
	int is_duration)
{
	int64_t us = 0;
	av_parse_time(&us, timestr, is_duration);
	return us;
}

const OptionDef* FfmpegConvert::find_option(const OptionDef *po, const char *name)
{
	const char *p = strchr(name, ':');
	int len = p ? p - name : strlen(name);

	while (po->name) {
		if (!strncmp(name, po->name, len) && strlen(po->name) == len)
			break;
		po++;
	}
	return po;
}


int FfmpegConvert::write_option(void *optctx, const OptionDef *po, const char *opt,
	const char *arg)
{
	/* new-style options contain an offset into optctx, old-style address of
	* a global var*/
	void *dst = po->flags & (OPT_OFFSET | OPT_SPEC) ?
		(uint8_t *)optctx + po->off : po->dst_ptr;
	int *dstcount;

	if (po->flags & OPT_SPEC) {
		SpecifierOpt **so = (SpecifierOpt **)dst;
		char *p = (char*)strchr(opt, ':');
		char *str;

		dstcount = (int *)(so + 1);
		*so = (SpecifierOpt *)grow_array(*so, sizeof(**so), dstcount, *dstcount + 1);
		str = av_strdup(p ? p + 1 : "");
		if (!str)
			return AVERROR(ENOMEM);
		(*so)[*dstcount - 1].specifier = str;
		dst = &(*so)[*dstcount - 1].u;
	}

	if (po->flags & OPT_STRING) {
		char *str;
		str = av_strdup(arg);
		av_freep(dst);
		if (!str)
			return AVERROR(ENOMEM);
		*(char **)dst = str;
	}
	else if (po->flags & OPT_BOOL || po->flags & OPT_INT) {
		*(int *)dst = parse_number_or_die(opt, arg, OPT_INT64, INT_MIN, INT_MAX);
	}
	else if (po->flags & OPT_INT64) {
		*(int64_t *)dst = parse_number_or_die(opt, arg, OPT_INT64, INT64_MIN, INT64_MAX);
	}
	else if (po->flags & OPT_TIME) {
		*(int64_t *)dst = parse_time_or_die(opt, arg, 1);
	}
	else if (po->flags & OPT_FLOAT) {
		*(float *)dst = parse_number_or_die(opt, arg, OPT_FLOAT, -INFINITY, INFINITY);
	}
	else if (po->flags & OPT_DOUBLE) {
		*(double *)dst = parse_number_or_die(opt, arg, OPT_DOUBLE, -INFINITY, INFINITY);
	}
	else if (po->func_arg) {
		int ret = po->func_arg(this, optctx, opt, arg);
		if (ret < 0) {
			return ret;
		}
	}
	return 0;
}

int FfmpegConvert::parse_option(void *optctx, const char *opt, const char *arg,
	const OptionDef *options)
{
	const OptionDef *po;
	int ret;

	po = find_option(options, opt);
	if (!po->name && opt[0] == 'n' && opt[1] == 'o') {
		/* handle 'no' bool option */
		po = find_option(options, opt + 2);
		if ((po->name && (po->flags & OPT_BOOL)))
			arg = "0";
	}
	else if (po->flags & OPT_BOOL)
		arg = "1";

	if (!po->name)
		po = find_option(options, "default");
	if (!po->name) {
		av_log(NULL, AV_LOG_ERROR, "Unrecognized option '%s'\n", opt);
		return AVERROR(EINVAL);
	}
	if (po->flags & HAS_ARG && !arg) {
		av_log(NULL, AV_LOG_ERROR, "Missing argument for option '%s'\n", opt);
		return AVERROR(EINVAL);
	}

	ret = write_option(optctx, po, opt, arg);
	if (ret < 0)
		return ret;

	return !!(po->flags & HAS_ARG);
}

void FfmpegConvert::parse_options(void *optctx, int argc, char **argv, const OptionDef *options,
	void(*parse_arg_function)(void *, const char*))
{
	const char *opt;
	int optindex, handleoptions = 1, ret;

	/* perform system-dependent conversions for arguments list */


	/* parse options */
	optindex = 1;
	while (optindex < argc) {
		opt = argv[optindex++];

		if (handleoptions && opt[0] == '-' && opt[1] != '\0') {
			if (opt[1] == '-' && opt[2] == '\0') {
				handleoptions = 0;
				continue;
			}
			opt++;

			if ((ret = parse_option(optctx, opt, argv[optindex], options)) < 0)
				exit_program(ret);
			optindex += ret;
		}
		else {
			if (parse_arg_function)
				parse_arg_function(optctx, opt);
		}
	}
}

int FfmpegConvert::parse_optgroup(void *optctx, OptionGroup *g)
{
	int i, ret;
	for (i = 0; i < g->nb_opts; i++) {
		Option *o = &g->opts[i];

		if (g->group_def->flags &&
			!(g->group_def->flags & o->opt->flags)) {
			return AVERROR(EINVAL);
		}
		ret = write_option(optctx, o->opt, o->key, o->val);
		if (ret < 0)
			return ret;
	}

	return 0;
}


void FfmpegConvert::check_options(const OptionDef *po)
{
	while (po->name) {
		if (po->flags & OPT_PERFILE)
			av_assert0(po->flags & (OPT_INPUT | OPT_OUTPUT));
		po++;
	}
}

const AVOption* FfmpegConvert::opt_find(void *obj, const char *name, const char *unit,
	int opt_flags, int search_flags)
{
	const AVOption *o = av_opt_find(obj, name, unit, opt_flags, search_flags);
	if (o && !o->flags)
		return NULL;
	return o;
}

int FfmpegConvert::match_group_separator(const OptionGroupDef *groups, int nb_groups,
	const char *opt)
{
	int i;

	for (i = 0; i < nb_groups; i++) {
		const OptionGroupDef *p = &groups[i];
		if (p->sep && !strcmp(p->sep, opt))
			return i;
	}

	return -1;
}

void FfmpegConvert::finish_group(OptionParseContext *octx, int group_idx,
	const char *arg)
{
	OptionGroupList *l = &octx->groups[group_idx];
	OptionGroup *g;

	GROW_ARRAY(l->groups, l->nb_groups, OptionGroup*);
	g = &l->groups[l->nb_groups - 1];

	*g = octx->cur_group;
	g->arg = arg;
	g->group_def = l->group_def;
	g->sws_dict = sws_dict;
	g->swr_opts = swr_opts;
	g->codec_opts = codec_opts;
	g->format_opts = format_opts;
	g->resample_opts = resample_opts;

	codec_opts = NULL;
	format_opts = NULL;
	resample_opts = NULL;
	sws_dict = NULL;
	swr_opts = NULL;
	av_dict_set(&sws_dict, "flags", "bicubic", 0);

	memset(&octx->cur_group, 0, sizeof(octx->cur_group));
}

void FfmpegConvert::add_opt(OptionParseContext *octx, const OptionDef *opt,
	const char *key, const char *val)
{
	int global = !(opt->flags & (OPT_PERFILE | OPT_SPEC | OPT_OFFSET));
	OptionGroup *g = global ? &octx->global_opts : &octx->cur_group;

	GROW_ARRAY(g->opts, g->nb_opts, Option*);
	g->opts[g->nb_opts - 1].opt = opt;
	g->opts[g->nb_opts - 1].key = key;
	g->opts[g->nb_opts - 1].val = val;
}

void FfmpegConvert::init_parse_context(OptionParseContext *octx,
	const OptionGroupDef *groups, int nb_groups)
{

	int i;

	memset(octx, 0, sizeof(*octx));

	octx->nb_groups = nb_groups;
	octx->groups = (OptionGroupList*)av_mallocz_array(octx->nb_groups, sizeof(*octx->groups));

	for (i = 0; i < octx->nb_groups; i++)
		octx->groups[i].group_def = &groups[i];

	octx->global_opts.group_def = &global_group;
	octx->global_opts.arg = "";

	av_dict_set(&sws_dict, "flags", "bicubic", 0);
}

void FfmpegConvert::uninit_parse_context(OptionParseContext *octx)
{
	int i, j;

	for (i = 0; i < octx->nb_groups; i++) {
		OptionGroupList *l = &octx->groups[i];

		for (j = 0; j < l->nb_groups; j++) {
			av_freep(&l->groups[j].opts);
			av_dict_free(&l->groups[j].codec_opts);
			av_dict_free(&l->groups[j].format_opts);
			av_dict_free(&l->groups[j].resample_opts);

			av_dict_free(&l->groups[j].sws_dict);
			av_dict_free(&l->groups[j].swr_opts);
		}
		av_freep(&l->groups);
	}
	av_freep(&octx->groups);

	av_freep(&octx->cur_group.opts);
	av_freep(&octx->global_opts.opts);

	av_dict_free(&swr_opts);
	av_dict_free(&sws_dict);
	av_dict_free(&format_opts);
	av_dict_free(&codec_opts);
	av_dict_free(&resample_opts);

}

int FfmpegConvert::split_commandline(OptionParseContext *octx, int argc, char *argv[],
	const OptionDef *options,
	const OptionGroupDef *groups, int nb_groups)
{
	int optindex = 1;
	int dashdash = -2;

	/* perform system-dependent conversions for arguments list */

	init_parse_context(octx, groups, nb_groups);

    	while (optindex < argc) {
		const char *opt = argv[optindex++], *arg;
		const OptionDef *po;
		int ret;

		if (opt[0] == '-' && opt[1] == '-' && !opt[2]) {
			dashdash = optindex;
			continue;
		}
		/* unnamed group separators, e.g. output filename */
		if (opt[0] != '-' || !opt[1] || dashdash + 1 == optindex) {
			finish_group(octx, 0, opt);
			continue;
		}
		opt++;

		/* named group separators, e.g. -i */
		if ((ret = match_group_separator(groups, nb_groups, opt)) >= 0) {
			do {
				arg = argv[optindex++];
				if (!arg) {
					return AVERROR(EINVAL);
				}
			} while (0);

			finish_group(octx, ret, arg);
			continue;
		}

		/* normal options */
		po = find_option(options, opt);
		if (po->name) {
			if (po->flags & OPT_EXIT) {
				/* optional argument, e.g. -h */
				arg = argv[optindex++];
			}
			else if (po->flags & HAS_ARG) {
				do {
					arg = argv[optindex++];
					if (!arg) {
						av_log(NULL, AV_LOG_ERROR, "Missing argument for option '%s'.\n", opt);
						return AVERROR(EINVAL);
					}
				} while (0);
			}
			else {
				arg = "1";
			}
			add_opt(octx, po, opt, arg);
			continue;
		}

		/* AVOptions */
		if (argv[optindex]) {
			ret = opt_default(this, NULL, opt, argv[optindex]);
			if (ret >= 0) {
				optindex++;
				continue;
			}
			else if (ret != AVERROR_OPTION_NOT_FOUND) {
				return ret;
			}
		}

		/* boolean -nofoo options */
		if (opt[0] == 'n' && opt[1] == 'o' &&
			(po = find_option(options, opt + 2)) &&
			po->name && po->flags & OPT_BOOL) {
			add_opt(octx, po, opt, "0");
			av_log(NULL, AV_LOG_DEBUG, " matched as option '%s' (%s) with "
				"argument 0.\n", po->name, po->help);
			continue;
		}
		return AVERROR_OPTION_NOT_FOUND;
	}

	return 0;
}


char FfmpegConvert::get_media_type_char(enum AVMediaType type)
{
	switch (type) {
	case AVMEDIA_TYPE_VIDEO:    return 'V';
	case AVMEDIA_TYPE_AUDIO:    return 'A';
	case AVMEDIA_TYPE_DATA:     return 'D';
	case AVMEDIA_TYPE_SUBTITLE: return 'S';
	case AVMEDIA_TYPE_ATTACHMENT:return 'T';
	default:                    return '?';
	}
}

const AVCodec* FfmpegConvert::next_codec_for_id(enum AVCodecID id, const AVCodec *prev,
	int encoder)
{
	while ((prev = av_codec_next(prev))) {
		if (prev->id == id &&
			(encoder ? av_codec_is_encoder(prev) : av_codec_is_decoder(prev)))
			return prev;
	}
	return NULL;
}

unsigned FfmpegConvert::get_codecs_sorted(const AVCodecDescriptor ***rcodecs)
{
	const AVCodecDescriptor *desc = NULL;
	const AVCodecDescriptor **codecs;
	unsigned nb_codecs = 0, i = 0;

	while ((desc = avcodec_descriptor_next(desc)))
		nb_codecs++;
	codecs = (const AVCodecDescriptor **)av_calloc(nb_codecs, sizeof(*codecs));
	desc = NULL;
	while ((desc = avcodec_descriptor_next(desc)))
		codecs[i++] = desc;
	av_assert0(i == nb_codecs);
	qsort(codecs, nb_codecs, sizeof(*codecs), compare_codec_desc);
	*rcodecs = codecs;
	return nb_codecs;
}

FILE* FfmpegConvert::get_preset_file(char *filename, size_t filename_size,
	const char *preset_name, int is_path,
	const char *codec_name)
{
	FILE *f = NULL;
	int i;
	const char *base[3] = { getenv("FFMPEG_DATADIR"),
		getenv("HOME"),
		FFMPEG_DATADIR, };

	if (is_path) {
		av_strlcpy(filename, preset_name, filename_size);
		f = fopen(filename, "r");
	}
	else {
#ifdef _WIN32
		char datadir[MAX_PATH], *ls;
		base[2] = NULL;

		if (GetModuleFileNameA(GetModuleHandleA(NULL), datadir, sizeof(datadir) - 1))
		{
			for (ls = datadir; ls < datadir + strlen(datadir); ls++)
				if (*ls == '\\') *ls = '/';

			if (ls = strrchr(datadir, '/'))
			{
				*ls = 0;
				strncat(datadir, "/ffpresets", sizeof(datadir) - 1 - strlen(datadir));
				base[2] = datadir;
			}
		}
#endif
		for (i = 0; i < 3 && !f; i++) {
			if (!base[i])
				continue;
			snprintf(filename, filename_size, "%s%s/%s.ffpreset", base[i],
				i != 1 ? "" : "/.ffmpeg", preset_name);
			f = fopen(filename, "r");
			if (!f && codec_name) {
				snprintf(filename, filename_size,
					"%s%s/%s-%s.ffpreset",
					base[i], i != 1 ? "" : "/.ffmpeg", codec_name,
					preset_name);
				f = fopen(filename, "r");
			}
		}
	}

	return f;
}

int FfmpegConvert::check_stream_specifier(AVFormatContext *s, AVStream *st, const char *spec)
{
	int ret = avformat_match_stream_specifier(s, st, spec);
	if (ret < 0)
		av_log(s, AV_LOG_ERROR, "Invalid stream specifier: %s.\n", spec);
	return ret;
}

AVDictionary* FfmpegConvert::filter_codec_opts(AVDictionary *opts, enum AVCodecID codec_id,
	AVFormatContext *s, AVStream *st, AVCodec *codec)
{
	AVDictionary    *ret = NULL;
	AVDictionaryEntry *t = NULL;
	int            flags = s->oformat ? AV_OPT_FLAG_ENCODING_PARAM
		: AV_OPT_FLAG_DECODING_PARAM;
	char          prefix = 0;
	const AVClass    *cc = avcodec_get_class();

	if (!codec)
		codec = s->oformat ? avcodec_find_encoder(codec_id)
		: avcodec_find_decoder(codec_id);

	switch (st->codecpar->codec_type) {
	case AVMEDIA_TYPE_VIDEO:
		prefix = 'v';
		flags |= AV_OPT_FLAG_VIDEO_PARAM;
		break;
	case AVMEDIA_TYPE_AUDIO:
		prefix = 'a';
		flags |= AV_OPT_FLAG_AUDIO_PARAM;
		break;
	case AVMEDIA_TYPE_SUBTITLE:
		prefix = 's';
		flags |= AV_OPT_FLAG_SUBTITLE_PARAM;
		break;
	}

	while (t = av_dict_get(opts, "", t, AV_DICT_IGNORE_SUFFIX)) {
		char *p = strchr(t->key, ':');

		/* check stream specification in opt name */
		if (p)
			switch (check_stream_specifier(s, st, p + 1)) {
			case  1: *p = 0; break;
			case  0:         continue;
			default:         exit_program(1);
			}

		if (av_opt_find(&cc, t->key, NULL, flags, AV_OPT_SEARCH_FAKE_OBJ) ||
			!codec ||
			(codec->priv_class &&
				av_opt_find(&codec->priv_class, t->key, NULL, flags,
					AV_OPT_SEARCH_FAKE_OBJ)))
			av_dict_set(&ret, t->key, t->value, 0);
		else if (t->key[0] == prefix &&
			av_opt_find(&cc, t->key + 1, NULL, flags,
				AV_OPT_SEARCH_FAKE_OBJ))
			av_dict_set(&ret, t->key + 1, t->value, 0);

		if (p)
			*p = ':';
	}
	return ret;
}

AVDictionary** FfmpegConvert::setup_find_stream_info_opts(AVFormatContext *s,
	AVDictionary *codec_opts)
{
	int i;
	AVDictionary **opts;

	if (!s->nb_streams)
		return NULL;
	opts = (AVDictionary **)av_mallocz_array(s->nb_streams, sizeof(*opts));

	for (i = 0; i < s->nb_streams; i++)
		opts[i] = filter_codec_opts(codec_opts, s->streams[i]->codecpar->codec_id,
			s, s->streams[i], NULL);
	return opts;
}

void* FfmpegConvert::grow_array(void *array, int elem_size, int *size, int new_size)
{
	if (new_size >= INT_MAX / elem_size) {
		av_log(NULL, AV_LOG_ERROR, "Array too big.\n");
		exit_program(100);
	}
	if (*size < new_size) {
		uint8_t *tmp = (uint8_t *)av_realloc_array(array, new_size, elem_size);
		memset(tmp + *size*elem_size, 0, (new_size - *size) * elem_size);
		*size = new_size;
		return tmp;
	}
	return array;
}

double FfmpegConvert::get_rotation(AVStream *st)
{
	uint8_t* displaymatrix = av_stream_get_side_data(st,
		AV_PKT_DATA_DISPLAYMATRIX, NULL);
	double theta = 0;
	if (displaymatrix)
		theta = -av_display_rotation_get((int32_t*)displaymatrix);

	theta -= 360 * floor(theta / 360 + 0.9 / 360);

	if (fabs(theta - 90 * round(theta / 90)) > 2)
		av_log(NULL, AV_LOG_WARNING, "Odd rotation angle.\n"
			"If you want to help, upload a sample "
			"of this file to ftp://upload.ffmpeg.org/incoming/ "
			"and contact the ffmpeg-devel mailing list. (ffmpeg-devel@ffmpeg.org)");

	return theta;
}


int FfmpegConvert::sub2video_get_blank_frame(InputStream *ist)
{
	int ret;
	AVFrame *frame = ist->sub2video.frame;

	av_frame_unref(frame);
	ist->sub2video.frame->width = ist->dec_ctx->width ? ist->dec_ctx->width : ist->sub2video.w;
	ist->sub2video.frame->height = ist->dec_ctx->height ? ist->dec_ctx->height : ist->sub2video.h;
	ist->sub2video.frame->format = AV_PIX_FMT_RGB32;
	if ((ret = av_frame_get_buffer(frame, 32)) < 0)
		return ret;
	memset(frame->data[0], 0, frame->height * frame->linesize[0]);
	return 0;
}

void FfmpegConvert::sub2video_copy_rect(uint8_t *dst, int dst_linesize, int w, int h,
	AVSubtitleRect *r)
{
	uint32_t *pal, *dst2;
	uint8_t *src, *src2;
	int x, y;

	if (r->type != SUBTITLE_BITMAP) {
		av_log(NULL, AV_LOG_WARNING, "sub2video: non-bitmap subtitle\n");
		return;
	}
	if (r->x < 0 || r->x + r->w > w || r->y < 0 || r->y + r->h > h) {
		av_log(NULL, AV_LOG_WARNING, "sub2video: rectangle (%d %d %d %d) overflowing %d %d\n",
			r->x, r->y, r->w, r->h, w, h
			);
		return;
	}

	dst += r->y * dst_linesize + r->x * 4;
	src = r->data[0];
	pal = (uint32_t *)r->data[1];
	for (y = 0; y < r->h; y++) {
		dst2 = (uint32_t *)dst;
		src2 = src;
		for (x = 0; x < r->w; x++)
			*(dst2++) = pal[*(src2++)];
		dst += dst_linesize;
		src += r->linesize[0];
	}
}

void FfmpegConvert::sub2video_push_ref(InputStream *ist, int64_t pts)
{
	AVFrame *frame = ist->sub2video.frame;
	int i;
	int ret;

	av_assert1(frame->data[0]);
	ist->sub2video.last_pts = frame->pts = pts;
	for (i = 0; i < ist->nb_filters; i++) {
		ret = av_buffersrc_add_frame_flags(ist->filters[i]->filter, frame,
			AV_BUFFERSRC_FLAG_KEEP_REF |
			AV_BUFFERSRC_FLAG_PUSH);
	}
}

void FfmpegConvert::sub2video_update(InputStream *ist, AVSubtitle *sub)
{
	AVFrame *frame = ist->sub2video.frame;
	uint8_t *dst;
	int     dst_linesize;
	int num_rects, i;
	int64_t pts, end_pts;

	if (!frame)
		return;
	if (sub) {
		pts = av_rescale_q(sub->pts + sub->start_display_time * 1000LL,
			AV_TIME_BASE_Q, ist->st->time_base);
		end_pts = av_rescale_q(sub->pts + sub->end_display_time * 1000LL,
			AV_TIME_BASE_Q, ist->st->time_base);
		num_rects = sub->num_rects;
	}
	else {
		pts = ist->sub2video.end_pts;
		end_pts = INT64_MAX;
		num_rects = 0;
	}
	if (sub2video_get_blank_frame(ist) < 0) {
		av_log(ist->dec_ctx, AV_LOG_ERROR,
			"Impossible to get a blank canvas.\n");
		return;
	}
	dst = (uint8_t *)frame->data[0];
	dst_linesize = frame->linesize[0];
	for (i = 0; i < num_rects; i++)
		sub2video_copy_rect(dst, dst_linesize, frame->width, frame->height, sub->rects[i]);
	sub2video_push_ref(ist, pts);
	ist->sub2video.end_pts = end_pts;
}

void FfmpegConvert::sub2video_heartbeat(InputStream *ist, int64_t pts)
{
	InputFile *infile = input_files[ist->file_index];
	int i, j, nb_reqs;
	int64_t pts2;

	/* When a frame is read from a file, examine all sub2video streams in
	the same file and send the sub2video frame again. Otherwise, decoded
	video frames could be accumulating in the filter graph while a filter
	(possibly overlay) is desperately waiting for a subtitle frame. */
	for (i = 0; i < infile->nb_streams; i++) {
		InputStream *ist2 = input_streams[infile->ist_index + i];
		if (!ist2->sub2video.frame)
			continue;
		/* subtitles seem to be usually muxed ahead of other streams;
		if not, subtracting a larger time here is necessary */
		pts2 = av_rescale_q(pts, ist->st->time_base, ist2->st->time_base) - 1;
		/* do not send the heartbeat frame if the subtitle is already ahead */
		if (pts2 <= ist2->sub2video.last_pts)
			continue;
		if (pts2 >= ist2->sub2video.end_pts || !ist2->sub2video.frame->data[0])
			sub2video_update(ist2, NULL);
		for (j = 0, nb_reqs = 0; j < ist2->nb_filters; j++)
			nb_reqs += av_buffersrc_get_nb_failed_requests(ist2->filters[j]->filter);
		if (nb_reqs)
			sub2video_push_ref(ist2, pts2);
	}
}

void FfmpegConvert::sub2video_flush(InputStream *ist)
{
	int i;
	int ret;

	if (ist->sub2video.end_pts < INT64_MAX)
		sub2video_update(ist, NULL);
	for (i = 0; i < ist->nb_filters; i++) {
		ret = av_buffersrc_add_frame(ist->filters[i]->filter, NULL);
		if (ret != AVERROR_EOF && ret < 0)
			av_log(NULL, AV_LOG_WARNING, "Flush the frame error.\n");
	}
}

void FfmpegConvert::ffmpeg_cleanup(int ret)
{
	int i, j;

	for (i = 0; i < nb_filtergraphs; i++) {
		FilterGraph *fg = filtergraphs[i];
		avfilter_graph_free(&fg->graph);
		for (j = 0; j < fg->nb_inputs; j++) {
			while (av_fifo_size(fg->inputs[j]->frame_queue)) {
				AVFrame *frame;
				av_fifo_generic_read(fg->inputs[j]->frame_queue, &frame,
					sizeof(frame), NULL);
				av_frame_free(&frame);
			}
			av_fifo_freep(&fg->inputs[j]->frame_queue);
			if (fg->inputs[j]->ist->sub2video.sub_queue) {
				while (av_fifo_size(fg->inputs[j]->ist->sub2video.sub_queue)) {
					AVSubtitle sub;
					av_fifo_generic_read(fg->inputs[j]->ist->sub2video.sub_queue,
						&sub, sizeof(sub), NULL);
					avsubtitle_free(&sub);
				}
				av_fifo_freep(&fg->inputs[j]->ist->sub2video.sub_queue);
			}
			av_buffer_unref(&fg->inputs[j]->hw_frames_ctx);
			av_freep(&fg->inputs[j]->name);
			av_freep(&fg->inputs[j]);
		}
		av_freep(&fg->inputs);
		for (j = 0; j < fg->nb_outputs; j++) {
			av_freep(&fg->outputs[j]->name);
			av_freep(&fg->outputs[j]->formats);
			av_freep(&fg->outputs[j]->channel_layouts);
			av_freep(&fg->outputs[j]->sample_rates);
			av_freep(&fg->outputs[j]);
		}
		av_freep(&fg->outputs);
		av_freep(&fg->graph_desc);

		av_freep(&filtergraphs[i]);
	}
	av_freep(&filtergraphs);

	av_freep(&subtitle_out);

	/* close files */
	for (i = 0; i < nb_output_files; i++) {
		OutputFile *of = output_files[i];
		AVFormatContext *s;
		if (!of)
			continue;
		s = of->ctx;
		if (s && s->oformat && !(s->oformat->flags & AVFMT_NOFILE))
			avio_closep(&s->pb);
		avformat_free_context(s);
		av_dict_free(&of->opts);

		av_freep(&output_files[i]);
	}
	for (i = 0; i < nb_output_streams; i++) {
		OutputStream *ost = output_streams[i];

		if (!ost)
			continue;

		for (j = 0; j < ost->nb_bitstream_filters; j++)
			av_bsf_free(&ost->bsf_ctx[j]);
		av_freep(&ost->bsf_ctx);

		av_frame_free(&ost->filtered_frame);
		av_frame_free(&ost->last_frame);
		av_dict_free(&ost->encoder_opts);

		av_parser_close(ost->parser);
		avcodec_free_context(&ost->parser_avctx);

		av_freep(&ost->forced_keyframes);
		av_expr_free(ost->forced_keyframes_pexpr);
		av_freep(&ost->avfilter);
		av_freep(&ost->logfile_prefix);

		av_freep(&ost->audio_channels_map);
		ost->audio_channels_mapped = 0;

		av_dict_free(&ost->sws_dict);

		avcodec_free_context(&ost->enc_ctx);
		avcodec_parameters_free(&ost->ref_par);

		if (ost->muxing_queue) {
			while (av_fifo_size(ost->muxing_queue)) {
				AVPacket pkt;
				av_fifo_generic_read(ost->muxing_queue, &pkt, sizeof(pkt), NULL);
				av_packet_unref(&pkt);
			}
			av_fifo_freep(&ost->muxing_queue);
		}

		av_freep(&output_streams[i]);
	}

	for (i = 0; i < nb_input_files; i++) {
		avformat_close_input(&input_files[i]->ctx);
		av_freep(&input_files[i]);
	}
	for (i = 0; i < nb_input_streams; i++) {
		InputStream *ist = input_streams[i];

		av_frame_free(&ist->decoded_frame);
		av_frame_free(&ist->filter_frame);
		av_dict_free(&ist->decoder_opts);
		avsubtitle_free(&ist->prev_sub.subtitle);
		av_frame_free(&ist->sub2video.frame);
		av_freep(&ist->filters);
		av_freep(&ist->hwaccel_device);
		av_freep(&ist->dts_buffer);

		avcodec_free_context(&ist->dec_ctx);

		av_freep(&input_streams[i]);
	}

	av_freep(&input_streams);
	av_freep(&input_files);
	av_freep(&output_streams);
	av_freep(&output_files);

	av_dict_free(&swr_opts);
	av_dict_free(&sws_dict);
	av_dict_free(&format_opts);
	av_dict_free(&codec_opts);
	av_dict_free(&resample_opts);
}

void FfmpegConvert::remove_avoptions(AVDictionary **a, AVDictionary *b)
{
	AVDictionaryEntry *t = NULL;

	while ((t = av_dict_get(b, "", t, AV_DICT_IGNORE_SUFFIX))) {
		av_dict_set(a, t->key, NULL, AV_DICT_MATCH_CASE);
	}
}

void FfmpegConvert::assert_avoptions(AVDictionary *a)
{
	AVDictionaryEntry *t;
	if ((t = av_dict_get(a, "", NULL, AV_DICT_IGNORE_SUFFIX))) {
		av_log(NULL, AV_LOG_FATAL, "Option %s not found.\n", t->key);
		exit_program(1);
	}
}

void FfmpegConvert::abort_codec_experimental(AVCodec *c, int encoder)
{
	exit_program(3);
}

void FfmpegConvert::close_all_output_streams(OutputStream *ost, OSTFinished this_stream, OSTFinished others)
{
	int i;
	for (i = 0; i < nb_output_streams; i++) {
		OutputStream *ost2 = output_streams[i];
		ost2->finished |= ost == ost2 ? this_stream : others;
	}
}

void FfmpegConvert::write_packet(OutputFile *of, AVPacket *pkt, OutputStream *ost, int unqueue)
{
	AVFormatContext *s = of->ctx;
	AVStream *st = ost->st;
	int ret;

	/*
	* Audio encoders may split the packets --  #frames in != #packets out.
	* But there is no reordering, so we can limit the number of output packets
	* by simply dropping them here.
	* Counting encoded video frames needs to be done separately because of
	* reordering, see do_video_out().
	* Do not count the packet when unqueued because it has been counted when queued.
	*/
	if (!(st->codecpar->codec_type == AVMEDIA_TYPE_VIDEO && ost->encoding_needed) && !unqueue) {
		if (ost->frame_number >= ost->max_frames) {
			av_packet_unref(pkt);
			return;
		}
		ost->frame_number++;
	}

	if (!of->header_written) {
		AVPacket tmp_pkt = { 0 };
		/* the muxer is not initialized yet, buffer the packet */
		if (!av_fifo_space(ost->muxing_queue)) {
			int new_size = FFMIN(2 * av_fifo_size(ost->muxing_queue),
				ost->max_muxing_queue_size);
			if (new_size <= av_fifo_size(ost->muxing_queue)) {
				av_log(NULL, AV_LOG_ERROR,
					"Too many packets buffered for output stream %d:%d.\n",
					ost->file_index, ost->st->index);
				exit_program(3);
			}
			ret = av_fifo_realloc2(ost->muxing_queue, new_size);
			if (ret < 0)
				exit_program(3);
		}
		ret = av_packet_ref(&tmp_pkt, pkt);
		if (ret < 0)
			exit_program(3);
		av_fifo_generic_write(ost->muxing_queue, &tmp_pkt, sizeof(tmp_pkt), NULL);
		av_packet_unref(pkt);
		return;
	}

	if ((st->codecpar->codec_type == AVMEDIA_TYPE_VIDEO && video_sync_method == VSYNC_DROP) ||
		(st->codecpar->codec_type == AVMEDIA_TYPE_AUDIO && audio_sync_method < 0))
		pkt->pts = pkt->dts = AV_NOPTS_VALUE;

	if (st->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
		int i;
		uint8_t *sd = av_packet_get_side_data(pkt, AV_PKT_DATA_QUALITY_STATS,
			NULL);
		ost->quality = sd ? AV_RL32(sd) : -1;
		ost->pict_type = sd ? sd[4] : AV_PICTURE_TYPE_NONE;

		for (i = 0; i<FF_ARRAY_ELEMS(ost->error); i++) {
			if (sd && i < sd[5])
				ost->error[i] = AV_RL64(sd + 8 + 8 * i);
			else
				ost->error[i] = -1;
		}

		if (ost->frame_rate.num && ost->is_cfr) {
			if (pkt->duration > 0)
				av_log(NULL, AV_LOG_WARNING, "Overriding packet duration by frame rate, this should not happen\n");
			pkt->duration = av_rescale_q(1, av_inv_q(ost->frame_rate),
				ost->mux_timebase);
		}
	}

	av_packet_rescale_ts(pkt, ost->mux_timebase, ost->st->time_base);

	if (!(s->oformat->flags & AVFMT_NOTIMESTAMPS)) {
		if (pkt->dts != AV_NOPTS_VALUE &&
			pkt->pts != AV_NOPTS_VALUE &&
			pkt->dts > pkt->pts) {
			av_log(s, AV_LOG_WARNING, "Invalid DTS: %" PRId64" PTS: %" PRId64" in output stream %d:%d, replacing by guess\n",
				pkt->dts, pkt->pts,
				ost->file_index, ost->st->index);
			pkt->pts =
				pkt->dts = pkt->pts + pkt->dts + ost->last_mux_dts + 1
				- FFMIN3(pkt->pts, pkt->dts, ost->last_mux_dts + 1)
				- FFMAX3(pkt->pts, pkt->dts, ost->last_mux_dts + 1);
		}
		if ((st->codecpar->codec_type == AVMEDIA_TYPE_AUDIO || st->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) &&
			pkt->dts != AV_NOPTS_VALUE &&
			!(st->codecpar->codec_id == AV_CODEC_ID_VP9 && ost->stream_copy) &&
			ost->last_mux_dts != AV_NOPTS_VALUE) {
			int64_t max = ost->last_mux_dts + !(s->oformat->flags & AVFMT_TS_NONSTRICT);
			if (pkt->dts < max) {
				if (pkt->pts >= pkt->dts)
					pkt->pts = FFMAX(pkt->pts, max);
				pkt->dts = max;
			}
		}
	}
	ost->last_mux_dts = pkt->dts;

	ost->data_size += pkt->size;
	ost->packets_written++;

	pkt->stream_index = ost->index;

	ret = av_interleaved_write_frame(s, pkt);
	if (ret < 0) {
		close_all_output_streams(ost, MUXER_FINISHED | ENCODER_FINISHED, ENCODER_FINISHED);
	}
	av_packet_unref(pkt);
}

void FfmpegConvert::close_output_stream(OutputStream *ost)
{
	OutputFile *of = output_files[ost->file_index];

	ost->finished |= ENCODER_FINISHED;
	if (of->shortest) {
		int64_t end = av_rescale_q(ost->sync_opts - ost->first_pts, ost->enc_ctx->time_base, AV_TIME_BASE_Q);
		of->recording_time = FFMIN(of->recording_time, end);
	}
}

void FfmpegConvert::output_packet(OutputFile *of, AVPacket *pkt,
	OutputStream *ost, int eof)
{
	int ret = 0;

	/* apply the output bitstream filters, if any */
	if (ost->nb_bitstream_filters) {
		int idx;

		ret = av_bsf_send_packet(ost->bsf_ctx[0], eof ? NULL : pkt);
		if (ret < 0)
			goto finish;

		eof = 0;
		idx = 1;
		while (idx) {
			/* get a packet from the previous filter up the chain */
			ret = av_bsf_receive_packet(ost->bsf_ctx[idx - 1], pkt);
			if (ret == AVERROR(EAGAIN)) {
				ret = 0;
				idx--;
				continue;
			}
			else if (ret == AVERROR_EOF) {
				eof = 1;
			}
			else if (ret < 0)
				goto finish;

			/* send it to the next filter down the chain or to the muxer */
			if (idx < ost->nb_bitstream_filters) {
				ret = av_bsf_send_packet(ost->bsf_ctx[idx], eof ? NULL : pkt);
				if (ret < 0)
					goto finish;
				idx++;
				eof = 0;
			}
			else if (eof)
				goto finish;
			else
				write_packet(of, pkt, ost, 0);
		}
	}
	else if (!eof)
		write_packet(of, pkt, ost, 0);

finish:
	return;
}

int FfmpegConvert::check_recording_time(OutputStream *ost)
{
	OutputFile *of = output_files[ost->file_index];

	if (of->recording_time != INT64_MAX &&
		av_compare_ts(ost->sync_opts - ost->first_pts, ost->enc_ctx->time_base, of->recording_time,
			AV_TIME_BASE_Q) >= 0) {
		close_output_stream(ost);
		return 0;
	}
	return 1;
}

void FfmpegConvert::do_audio_out(OutputFile *of, OutputStream *ost,
	AVFrame *frame)
{
	AVCodecContext *enc = ost->enc_ctx;
	AVPacket pkt;
	int ret;

	av_init_packet(&pkt);
	pkt.data = NULL;
	pkt.size = 0;

	if (!check_recording_time(ost))
		return;

	if (frame->pts == AV_NOPTS_VALUE || audio_sync_method < 0)
		frame->pts = ost->sync_opts;
	ost->sync_opts = frame->pts + frame->nb_samples;
	ost->samples_encoded += frame->nb_samples;
	ost->frames_encoded++;

	av_assert0(pkt.size || !pkt.data);

	if (m_cbVideo != nullptr) { //编码前的数据回调
		if (m_newFrame == nullptr)
			m_newFrame = av_frame_alloc();
		m_newFrame->format = frame->format;
		m_newFrame->width = frame->width;
		m_newFrame->height = frame->height;
		av_frame_get_buffer(m_newFrame, 32);
		av_frame_copy_props(m_newFrame, frame);
		av_frame_copy(m_newFrame, frame);

		int64_t ptsVideo = m_newFrame->pts;
		ptsVideo = ptsVideo * 1000 * enc->time_base.num / enc->time_base.den;
		if (avffmpeg_owner != nullptr)
			(*m_cbVideo)(avffmpeg_owner, m_newFrame->width, m_newFrame->height, m_newFrame->data, m_newFrame->linesize, ptsVideo);

		ret = avcodec_send_frame(enc, frame);
	}
	else {
		ret = avcodec_send_frame(enc, frame);
	}

	if (ret < 0)
		goto error;

	while (1) {
		ret = avcodec_receive_packet(enc, &pkt);
		if (ret == AVERROR(EAGAIN))
			break;
		if (ret < 0)
			goto error;
		av_packet_rescale_ts(&pkt, enc->time_base, ost->mux_timebase);
		output_packet(of, &pkt, ost, 0);
	}

	return;
error:
	av_log(NULL, AV_LOG_FATAL, "Audio encoding failed\n");
	exit_program(1);
}

void FfmpegConvert::do_subtitle_out(OutputFile *of,
	OutputStream *ost,
	AVSubtitle *sub)
{
	int subtitle_out_max_size = 1024 * 1024;
	int subtitle_out_size, nb, i;
	AVCodecContext *enc;
	AVPacket pkt;
	int64_t pts;

	if (sub->pts == AV_NOPTS_VALUE) {
		return;
	}

	enc = ost->enc_ctx;

	if (!subtitle_out) {
		subtitle_out = (uint8_t*)av_malloc(subtitle_out_max_size);
	}

	/* Note: DVB subtitle need one packet to draw them and one other
	packet to clear them */
	/* XXX: signal it in the codec context ? */
	if (enc->codec_id == AV_CODEC_ID_DVB_SUBTITLE)
		nb = 2;
	else
		nb = 1;

	/* shift timestamp to honor -ss and make check_recording_time() work with -t */
	pts = sub->pts;
	if (output_files[ost->file_index]->start_time != AV_NOPTS_VALUE)
		pts -= output_files[ost->file_index]->start_time;
	for (i = 0; i < nb; i++) {
		unsigned save_num_rects = sub->num_rects;

		ost->sync_opts = av_rescale_q(pts, AV_TIME_BASE_Q, enc->time_base);
		if (!check_recording_time(ost))
			return;

		sub->pts = pts;
		// start_display_time is required to be 0
		sub->pts += av_rescale_q(sub->start_display_time, AV_TIME_BASE_1_1000, AV_TIME_BASE_Q);
		sub->end_display_time -= sub->start_display_time;
		sub->start_display_time = 0;
		if (i == 1)
			sub->num_rects = 0;

		ost->frames_encoded++;

		subtitle_out_size = avcodec_encode_subtitle(enc, subtitle_out,
			subtitle_out_max_size, sub);
		if (i == 1)
			sub->num_rects = save_num_rects;
		if (subtitle_out_size < 0) {
			av_log(NULL, AV_LOG_FATAL, "Subtitle encoding failed\n");
			exit_program(1);
		}

		av_init_packet(&pkt);
		pkt.data = subtitle_out;
		pkt.size = subtitle_out_size;
		pkt.pts = av_rescale_q(sub->pts, AV_TIME_BASE_Q, ost->mux_timebase);
		pkt.duration = av_rescale_q(sub->end_display_time, AV_TIME_BASE_1_1000, ost->mux_timebase);
		if (enc->codec_id == AV_CODEC_ID_DVB_SUBTITLE) {
			/* XXX: the pts correction is handled here. Maybe handling
			it in the codec would be better */
			if (i == 0)
				pkt.pts += av_rescale_q(sub->start_display_time, AV_TIME_BASE_1_1000, ost->mux_timebase);
			else
				pkt.pts += av_rescale_q(sub->end_display_time, AV_TIME_BASE_1_1000, ost->mux_timebase);
		}
		pkt.dts = pkt.pts;
		output_packet(of, &pkt, ost, 0);
	}
}

void FfmpegConvert::do_video_out(OutputFile *of,
	OutputStream *ost,
	AVFrame *next_picture,
	double sync_ipts)
{
	int ret, format_video_sync;
	AVPacket pkt;
	AVCodecContext *enc = ost->enc_ctx;
	AVCodecParameters *mux_par = ost->st->codecpar;
	AVRational frame_rate;
	int nb_frames, nb0_frames, i;
	double delta, delta0;
	double duration = 0;
	int frame_size = 0;
	InputStream *ist = NULL;
	AVFilterContext *filter = ost->filter->filter;

	if (ost->source_index >= 0)
		ist = input_streams[ost->source_index];

	frame_rate = av_buffersink_get_frame_rate(filter);
	if (frame_rate.num > 0 && frame_rate.den > 0)
		duration = 1 / (av_q2d(frame_rate) * av_q2d(enc->time_base));

	if (ist && ist->st->start_time != AV_NOPTS_VALUE && ist->st->first_dts != AV_NOPTS_VALUE && ost->frame_rate.num)
		duration = FFMIN(duration, 1 / (av_q2d(ost->frame_rate) * av_q2d(enc->time_base)));

	if (!ost->filters_script &&
		!ost->filters &&
		next_picture &&
		ist &&
		lrintf(next_picture->pkt_duration * av_q2d(ist->st->time_base) / av_q2d(enc->time_base)) > 0) {
		duration = lrintf(next_picture->pkt_duration * av_q2d(ist->st->time_base) / av_q2d(enc->time_base));
	}

	if (!next_picture) {
		//end, flushing
		nb0_frames = nb_frames = mid_pred(ost->last_nb0_frames[0],
			ost->last_nb0_frames[1],
			ost->last_nb0_frames[2]);
	}
	else {
		delta0 = sync_ipts - ost->sync_opts; // delta0 is the "drift" between the input frame (next_picture) and where it would fall in the output.
		delta = delta0 + duration;

		/* by default, we output a single frame */
		nb0_frames = 0; // tracks the number of times the PREVIOUS frame should be duplicated, mostly for variable framerate (VFR)
		nb_frames = 1;

		format_video_sync = video_sync_method;
		if (format_video_sync == VSYNC_AUTO) {
			if (!strcmp(of->ctx->oformat->name, "avi")) {
				format_video_sync = VSYNC_VFR;
			}
			else
				format_video_sync = (of->ctx->oformat->flags & AVFMT_VARIABLE_FPS) ? ((of->ctx->oformat->flags & AVFMT_NOTIMESTAMPS) ? VSYNC_PASSTHROUGH : VSYNC_VFR) : VSYNC_CFR;
			if (ist
				&& format_video_sync == VSYNC_CFR
				&& input_files[ist->file_index]->ctx->nb_streams == 1
				&& input_files[ist->file_index]->input_ts_offset == 0) {
				format_video_sync = VSYNC_VSCFR;
			}
			if (format_video_sync == VSYNC_CFR && copy_ts) {
				format_video_sync = VSYNC_VSCFR;
			}
		}
		ost->is_cfr = (format_video_sync == VSYNC_CFR || format_video_sync == VSYNC_VSCFR);

		if (delta0 < 0 &&
			delta > 0 &&
			format_video_sync != VSYNC_PASSTHROUGH &&
			format_video_sync != VSYNC_DROP) {
			if (delta0 < -0.6) {
				av_log(NULL, AV_LOG_WARNING, "Past duration %f too large\n", -delta0);
			}
			else
				av_log(NULL, AV_LOG_DEBUG, "Clipping frame in rate conversion by %f\n", -delta0);
			sync_ipts = ost->sync_opts;
			duration += delta0;
			delta0 = 0;
		}

		switch (format_video_sync) {
		case VSYNC_VSCFR:
			if (ost->frame_number == 0 && delta0 >= 0.5) {
				av_log(NULL, AV_LOG_DEBUG, "Not duplicating %d initial frames\n", (int)lrintf(delta0));
				delta = duration;
				delta0 = 0;
				ost->sync_opts = lrint(sync_ipts);
			}
		case VSYNC_CFR:
			// FIXME set to 0.5 after we fix some dts/pts bugs like in avidec.c
			if (frame_drop_threshold && delta < frame_drop_threshold && ost->frame_number) {
				nb_frames = 0;
			}
			else if (delta < -1.1)
				nb_frames = 0;
			else if (delta > 1.1) {
				nb_frames = lrintf(delta);
				if (delta0 > 1.1)
					nb0_frames = lrintf(delta0 - 0.6);
			}
			break;
		case VSYNC_VFR:
			if (delta <= -0.6)
				nb_frames = 0;
			else if (delta > 0.6)
				ost->sync_opts = lrint(sync_ipts);
			break;
		case VSYNC_DROP:
		case VSYNC_PASSTHROUGH:
			ost->sync_opts = lrint(sync_ipts);
			break;
		default:
			av_assert0(0);
		}
	}

	nb_frames = FFMIN(nb_frames, ost->max_frames - ost->frame_number);
	nb0_frames = FFMIN(nb0_frames, nb_frames);

	memmove(ost->last_nb0_frames + 1,
		ost->last_nb0_frames,
		sizeof(ost->last_nb0_frames[0]) * (FF_ARRAY_ELEMS(ost->last_nb0_frames) - 1));
	ost->last_nb0_frames[0] = nb0_frames;



	ost->last_dropped = nb_frames == nb0_frames && next_picture;

	/* duplicates frame if needed */
	for (i = 0; i < nb_frames; i++) {
		AVFrame *in_picture;
		av_init_packet(&pkt);
		pkt.data = NULL;
		pkt.size = 0;

		if (i < nb0_frames && ost->last_frame) {
			in_picture = ost->last_frame;
		}
		else
			in_picture = next_picture;

		if (!in_picture)
			return;

		in_picture->pts = ost->sync_opts;

#if 1
		if (!check_recording_time(ost))
#else
		if (ost->frame_number >= ost->max_frames)
#endif
			return;

		{
			int forced_keyframe = 0;
			double pts_time;

			if (enc->flags & (AV_CODEC_FLAG_INTERLACED_DCT | AV_CODEC_FLAG_INTERLACED_ME) &&
				ost->top_field_first >= 0)
				in_picture->top_field_first = !!ost->top_field_first;

			if (in_picture->interlaced_frame) {
				if (enc->codec->id == AV_CODEC_ID_MJPEG)
					mux_par->field_order = in_picture->top_field_first ? AV_FIELD_TT : AV_FIELD_BB;
				else
					mux_par->field_order = in_picture->top_field_first ? AV_FIELD_TB : AV_FIELD_BT;
			}
			else
				mux_par->field_order = AV_FIELD_PROGRESSIVE;

			in_picture->quality = enc->global_quality;
			in_picture->pict_type = (AVPictureType)0;

			pts_time = in_picture->pts != AV_NOPTS_VALUE ?
				in_picture->pts * av_q2d(enc->time_base) : NAN;
			if (ost->forced_kf_index < ost->forced_kf_count &&
				in_picture->pts >= ost->forced_kf_pts[ost->forced_kf_index]) {
				ost->forced_kf_index++;
				forced_keyframe = 1;
			}
			else if (ost->forced_keyframes_pexpr) {
				double res;
				ost->forced_keyframes_expr_const_values[FKF_T] = pts_time;
				res = av_expr_eval(ost->forced_keyframes_pexpr,
					ost->forced_keyframes_expr_const_values, NULL);
				if (res) {
					forced_keyframe = 1;
					ost->forced_keyframes_expr_const_values[FKF_PREV_FORCED_N] =
						ost->forced_keyframes_expr_const_values[FKF_N];
					ost->forced_keyframes_expr_const_values[FKF_PREV_FORCED_T] =
						ost->forced_keyframes_expr_const_values[FKF_T];
					ost->forced_keyframes_expr_const_values[FKF_N_FORCED] += 1;
				}

				ost->forced_keyframes_expr_const_values[FKF_N] += 1;
			}
			else if (ost->forced_keyframes
				&& !strncmp(ost->forced_keyframes, "source", 6)
				&& in_picture->key_frame == 1) {
				forced_keyframe = 1;
			}

			if (forced_keyframe) {
				in_picture->pict_type = AV_PICTURE_TYPE_I;
				av_log(NULL, AV_LOG_DEBUG, "Forced keyframe at time %f\n", pts_time);
			}

			ost->frames_encoded++;

			ret = avcodec_send_frame(enc, in_picture);
			if (ret < 0)
				goto error;

			while (1) {
				ret = avcodec_receive_packet(enc, &pkt);
				if (ret == AVERROR(EAGAIN))
					break;
				if (ret < 0)
					goto error;

				if (pkt.pts == AV_NOPTS_VALUE && !(enc->codec->capabilities & AV_CODEC_CAP_DELAY))
					pkt.pts = ost->sync_opts;

				av_packet_rescale_ts(&pkt, enc->time_base, ost->mux_timebase);


				frame_size = pkt.size;
				output_packet(of, &pkt, ost, 0);

				/* if two pass, output log */
				if (ost->logfile && enc->stats_out) {
					fprintf(ost->logfile, "%s", enc->stats_out);
				}
			}
		}
		ost->sync_opts++;
		/*
		* For video, number of frames in == number of packets out.
		* But there may be reordering, so we can't throw away frames on encoder
		* flush, we need to limit them here, before they go into encoder.
		*/
		ost->frame_number++;
	}

	if (!ost->last_frame)
		ost->last_frame = av_frame_alloc();
	av_frame_unref(ost->last_frame);
	if (next_picture && ost->last_frame)
		av_frame_ref(ost->last_frame, next_picture);
	else
		av_frame_free(&ost->last_frame);

	return;
error:
	av_log(NULL, AV_LOG_FATAL, "Video encoding failed\n");
	exit_program(1);
}

void FfmpegConvert::finish_output_stream(OutputStream *ost)
{
	OutputFile *of = output_files[ost->file_index];
	int i;

	ost->finished = ENCODER_FINISHED | MUXER_FINISHED;

	if (of->shortest) {
		for (i = 0; i < of->ctx->nb_streams; i++)
			output_streams[of->ost_index + i]->finished = ENCODER_FINISHED | MUXER_FINISHED;
	}
}

int FfmpegConvert::reap_filters(int flush)
{
	AVFrame *filtered_frame = NULL;
	int i;

	/* Reap all buffers present in the buffer sinks */
	for (i = 0; i < nb_output_streams; i++) {
		OutputStream *ost = output_streams[i];
		OutputFile    *of = output_files[ost->file_index];
		AVFilterContext *filter;
		AVCodecContext *enc = ost->enc_ctx;
		int ret = 0;

		if (!ost->filter || !ost->filter->graph->graph)
			continue;
		filter = ost->filter->filter;

		if (!ost->initialized) {
			char error[1024] = "";
			ret = init_output_stream(ost, error, sizeof(error));
			if (ret < 0) {
				av_log(NULL, AV_LOG_ERROR, "Error initializing output stream %d:%d -- %s\n",
					ost->file_index, ost->index, error);
				exit_program(1);
			}
		}

		if (!ost->filtered_frame && !(ost->filtered_frame = av_frame_alloc())) {
			return AVERROR(ENOMEM);
		}
		filtered_frame = ost->filtered_frame;

		while (1) {
			double float_pts = AV_NOPTS_VALUE; // this is identical to filtered_frame.pts but with higher precision
			ret = av_buffersink_get_frame_flags(filter, filtered_frame,
				AV_BUFFERSINK_FLAG_NO_REQUEST);
			if (ret < 0) {
				if (ret != AVERROR(EAGAIN) && ret != AVERROR_EOF) {

				}
				else if (flush && ret == AVERROR_EOF) {
					if (av_buffersink_get_type(filter) == AVMEDIA_TYPE_VIDEO)
						do_video_out(of, ost, NULL, AV_NOPTS_VALUE);
				}
				break;
			}
			if (ost->finished) {
				av_frame_unref(filtered_frame);
				continue;
			}
			if (filtered_frame->pts != AV_NOPTS_VALUE) {
				int64_t start_time = (of->start_time == AV_NOPTS_VALUE) ? 0 : of->start_time;
				AVRational filter_tb = av_buffersink_get_time_base(filter);
				AVRational tb = enc->time_base;
				int extra_bits = av_clip(29 - av_log2(tb.den), 0, 16);

				tb.den <<= extra_bits;
				float_pts =
					av_rescale_q(filtered_frame->pts, filter_tb, tb) -
					av_rescale_q(start_time, AV_TIME_BASE_Q, tb);
				float_pts /= 1 << extra_bits;
				// avoid exact midoints to reduce the chance of rounding differences, this can be removed in case the fps code is changed to work with integers
				float_pts += FFSIGN(float_pts) * 1.0 / (1 << 17);

				filtered_frame->pts =
					av_rescale_q(filtered_frame->pts, filter_tb, enc->time_base) -
					av_rescale_q(start_time, AV_TIME_BASE_Q, enc->time_base);
			}

			switch (av_buffersink_get_type(filter)) {
			case AVMEDIA_TYPE_VIDEO:
				if (!ost->frame_aspect_ratio.num)
					enc->sample_aspect_ratio = filtered_frame->sample_aspect_ratio;

				do_video_out(of, ost, filtered_frame, float_pts);
				break;
			case AVMEDIA_TYPE_AUDIO:
				if (!(enc->codec->capabilities & AV_CODEC_CAP_PARAM_CHANGE) &&
					enc->channels != filtered_frame->channels) {
					av_log(NULL, AV_LOG_ERROR,
						"Audio filter graph output is not normalized and encoder does not support parameter changes\n");
					break;
				}
				do_audio_out(of, ost, filtered_frame);
				break;
			default:
				// TODO support subtitle filters
				av_assert0(0);
			}

			av_frame_unref(filtered_frame);
		}
	}

	return 0;
}

void FfmpegConvert::flush_encoders(void)
{
	int i, ret;

	for (i = 0; i < nb_output_streams; i++) {
		OutputStream   *ost = output_streams[i];
		AVCodecContext *enc = ost->enc_ctx;
		OutputFile      *of = output_files[ost->file_index];

		if (!ost->encoding_needed)
			continue;

		// Try to enable encoding with no input frames.
		// Maybe we should just let encoding fail instead.
		if (!ost->initialized) {
			FilterGraph *fg = ost->filter->graph;
			char error[1024] = "";

			av_log(NULL, AV_LOG_WARNING,
				"Finishing stream %d:%d without any data written to it.\n",
				ost->file_index, ost->st->index);

			if (ost->filter && !fg->graph) {
				int x;
				for (x = 0; x < fg->nb_inputs; x++) {
					InputFilter *ifilter = fg->inputs[x];
					if (ifilter->format < 0) {
						AVCodecParameters *par = ifilter->ist->st->codecpar;
						// We never got any input. Set a fake format, which will
						// come from libavformat.
						ifilter->format = par->format;
						ifilter->sample_rate = par->sample_rate;
						ifilter->channels = par->channels;
						ifilter->channel_layout = par->channel_layout;
						ifilter->width = par->width;
						ifilter->height = par->height;
						ifilter->sample_aspect_ratio = par->sample_aspect_ratio;
					}
				}

				if (!ifilter_has_all_input_formats(fg))
					continue;

				ret = configure_filtergraph(fg);
				if (ret < 0) {
					av_log(NULL, AV_LOG_ERROR, "Error configuring filter graph\n");
					exit_program(1);
				}

				finish_output_stream(ost);
			}

			ret = init_output_stream(ost, error, sizeof(error));
			if (ret < 0) {
				av_log(NULL, AV_LOG_ERROR, "Error initializing output stream %d:%d -- %s\n",
					ost->file_index, ost->index, error);
				exit_program(1);
			}
		}

		if (enc->codec_type == AVMEDIA_TYPE_AUDIO && enc->frame_size <= 1)
			continue;

		if (enc->codec_type != AVMEDIA_TYPE_VIDEO && enc->codec_type != AVMEDIA_TYPE_AUDIO)
			continue;

		for (;;) {
			const char *desc = NULL;
			AVPacket pkt;
			int pkt_size;

			switch (enc->codec_type) {
			case AVMEDIA_TYPE_AUDIO:
				desc = "audio";
				break;
			case AVMEDIA_TYPE_VIDEO:
				desc = "video";
				break;
			default:
				av_assert0(0);
			}

			av_init_packet(&pkt);
			pkt.data = NULL;
			pkt.size = 0;
			while ((ret = avcodec_receive_packet(enc, &pkt)) == AVERROR(EAGAIN)) {
				ret = avcodec_send_frame(enc, NULL);
				if (ret < 0) {
					exit_program(1);
				}
			}

			if (ret < 0 && ret != AVERROR_EOF) {
				exit_program(1);
			}
			if (ost->logfile && enc->stats_out) {
				fprintf(ost->logfile, "%s", enc->stats_out);
			}
			if (ret == AVERROR_EOF) {
				output_packet(of, &pkt, ost, 1);
				break;
			}
			if (ost->finished & MUXER_FINISHED) {
				av_packet_unref(&pkt);
				continue;
			}
			av_packet_rescale_ts(&pkt, enc->time_base, ost->mux_timebase);
			pkt_size = pkt.size;
			output_packet(of, &pkt, ost, 0);
		}
	}
}

int FfmpegConvert::check_output_constraints(InputStream *ist, OutputStream *ost)
{
	OutputFile *of = output_files[ost->file_index];
	int ist_index = input_files[ist->file_index]->ist_index + ist->st->index;

	if (ost->source_index != ist_index)
		return 0;

	if (ost->finished)
		return 0;

	if (of->start_time != AV_NOPTS_VALUE && ist->pts < of->start_time)
		return 0;

	return 1;
}

void FfmpegConvert::do_streamcopy(InputStream *ist, OutputStream *ost, const AVPacket *pkt)
{
	OutputFile *of = output_files[ost->file_index];
	InputFile   *f = input_files[ist->file_index];
	int64_t start_time = (of->start_time == AV_NOPTS_VALUE) ? 0 : of->start_time;
	int64_t ost_tb_start_time = av_rescale_q(start_time, AV_TIME_BASE_Q, ost->mux_timebase);
	AVPacket opkt = { 0 };

	av_init_packet(&opkt);

	// EOF: flush output bitstream filters.
	if (!pkt) {
		output_packet(of, &opkt, ost, 1);
		return;
	}

	if ((!ost->frame_number && !(pkt->flags & AV_PKT_FLAG_KEY)) &&
		!ost->copy_initial_nonkeyframes)
		return;

	if (!ost->frame_number && !ost->copy_prior_start) {
		int64_t comp_start = start_time;
		if (copy_ts && f->start_time != AV_NOPTS_VALUE)
			comp_start = FFMAX(start_time, f->start_time + f->ts_offset);
		if (pkt->pts == AV_NOPTS_VALUE ?
			ist->pts < comp_start :
			pkt->pts < av_rescale_q(comp_start, AV_TIME_BASE_Q, ist->st->time_base))
			return;
	}

	if (of->recording_time != INT64_MAX &&
		ist->pts >= of->recording_time + start_time) {
		close_output_stream(ost);
		return;
	}

	if (f->recording_time != INT64_MAX) {
		start_time = f->ctx->start_time;
		if (f->start_time != AV_NOPTS_VALUE && copy_ts)
			start_time += f->start_time;
		if (ist->pts >= f->recording_time + start_time) {
			close_output_stream(ost);
			return;
		}
	}

	/* force the input stream PTS */
	if (ost->enc_ctx->codec_type == AVMEDIA_TYPE_VIDEO)
		ost->sync_opts++;

	if (pkt->pts != AV_NOPTS_VALUE)
		opkt.pts = av_rescale_q(pkt->pts, ist->st->time_base, ost->mux_timebase) - ost_tb_start_time;
	else
		opkt.pts = AV_NOPTS_VALUE;

	if (pkt->dts == AV_NOPTS_VALUE)
		opkt.dts = av_rescale_q(ist->dts, AV_TIME_BASE_Q, ost->mux_timebase);
	else
		opkt.dts = av_rescale_q(pkt->dts, ist->st->time_base, ost->mux_timebase);
	opkt.dts -= ost_tb_start_time;

	if (ost->st->codecpar->codec_type == AVMEDIA_TYPE_AUDIO && pkt->dts != AV_NOPTS_VALUE) {
		int duration = av_get_audio_frame_duration(ist->dec_ctx, pkt->size);
		if (!duration)
			duration = ist->dec_ctx->frame_size;
		AVRational AVRK{ 1, ist->dec_ctx->sample_rate };
		opkt.dts = opkt.pts = av_rescale_delta(ist->st->time_base, pkt->dts,
			AVRK, duration, &ist->filter_in_rescale_delta_last,
			ost->mux_timebase) - ost_tb_start_time;
	}

	opkt.duration = av_rescale_q(pkt->duration, ist->st->time_base, ost->mux_timebase);

	opkt.flags = pkt->flags;
	// FIXME remove the following 2 lines they shall be replaced by the bitstream filters
	if (ost->st->codecpar->codec_id != AV_CODEC_ID_H264
		&& ost->st->codecpar->codec_id != AV_CODEC_ID_MPEG1VIDEO
		&& ost->st->codecpar->codec_id != AV_CODEC_ID_MPEG2VIDEO
		&& ost->st->codecpar->codec_id != AV_CODEC_ID_VC1
		) {
		int ret = av_parser_change(ost->parser, ost->parser_avctx,
			&opkt.data, &opkt.size,
			pkt->data, pkt->size,
			pkt->flags & AV_PKT_FLAG_KEY);
		if (ret < 0) {
			exit_program(1);
		}
		if (ret) {
			opkt.buf = av_buffer_create(opkt.data, opkt.size, av_buffer_default_free, NULL, 0);
			if (!opkt.buf)
				exit_program(1);
		}
	}
	else {
		opkt.data = pkt->data;
		opkt.size = pkt->size;
	}
	av_copy_packet_side_data(&opkt, pkt);

	output_packet(of, &opkt, ost, 0);
}

int FfmpegConvert::guess_input_channel_layout(InputStream *ist)
{
	AVCodecContext *dec = ist->dec_ctx;

	if (!dec->channel_layout) {
		char layout_name[256];

		if (dec->channels > ist->guess_layout_max)
			return 0;
		dec->channel_layout = av_get_default_channel_layout(dec->channels);
		if (!dec->channel_layout)
			return 0;
		av_get_channel_layout_string(layout_name, sizeof(layout_name),
			dec->channels, dec->channel_layout);
		av_log(NULL, AV_LOG_WARNING, "Guessed Channel Layout for Input Stream "
			"#%d.%d : %s\n", ist->file_index, ist->st->index, layout_name);
	}
	return 1;
}

// Filters can be configured only if the formats of all inputs are known.
int FfmpegConvert::ifilter_has_all_input_formats(FilterGraph *fg)
{
	int i;
	for (i = 0; i < fg->nb_inputs; i++) {
		if (fg->inputs[i]->format < 0 && (fg->inputs[i]->type == AVMEDIA_TYPE_AUDIO ||
			fg->inputs[i]->type == AVMEDIA_TYPE_VIDEO))
			return 0;
	}
	return 1;
}

int FfmpegConvert::ifilter_send_frame(InputFilter *ifilter, AVFrame *frame)
{
	FilterGraph *fg = ifilter->graph;
	int need_reinit, ret, i;

	/* determine if the parameters for this input changed */
	need_reinit = ifilter->format != frame->format;
	if (!!ifilter->hw_frames_ctx != !!frame->hw_frames_ctx ||
		(ifilter->hw_frames_ctx && ifilter->hw_frames_ctx->data != frame->hw_frames_ctx->data))
		need_reinit = 1;

	switch (ifilter->ist->st->codecpar->codec_type) {
	case AVMEDIA_TYPE_AUDIO:
		need_reinit |= ifilter->sample_rate != frame->sample_rate ||
			ifilter->channels != frame->channels ||
			ifilter->channel_layout != frame->channel_layout;
		break;
	case AVMEDIA_TYPE_VIDEO:
		need_reinit |= ifilter->width != frame->width ||
			ifilter->height != frame->height;
		break;
	}

	if (need_reinit) {
		ret = ifilter_parameters_from_frame(ifilter, frame);
		if (ret < 0)
			return ret;
	}

	/* (re)init the graph if possible, otherwise buffer the frame and return */
	if (need_reinit || !fg->graph) {
		for (i = 0; i < fg->nb_inputs; i++) {
			if (!ifilter_has_all_input_formats(fg)) {
				AVFrame *tmp = av_frame_clone(frame);
				if (!tmp)
					return AVERROR(ENOMEM);
				av_frame_unref(frame);

				if (!av_fifo_space(ifilter->frame_queue)) {
					ret = av_fifo_realloc2(ifilter->frame_queue, 2 * av_fifo_size(ifilter->frame_queue));
					if (ret < 0) {
						av_frame_free(&tmp);
						return ret;
					}
				}
				av_fifo_generic_write(ifilter->frame_queue, &tmp, sizeof(tmp), NULL);
				return 0;
			}
		}

		ret = reap_filters(1);
		if (ret < 0 && ret != AVERROR_EOF) {
			char errbuf[128];
			av_strerror(ret, errbuf, sizeof(errbuf));

			av_log(NULL, AV_LOG_ERROR, "Error while filtering: %s\n", errbuf);
			return ret;
		}

		ret = configure_filtergraph(fg);
		if (ret < 0) {
			av_log(NULL, AV_LOG_ERROR, "Error reinitializing filters!\n");
			return ret;
		}
	}

	ret = av_buffersrc_add_frame_flags(ifilter->filter, frame, AV_BUFFERSRC_FLAG_PUSH);
	if (ret < 0) {
		return ret;
	}

	return 0;
}

int FfmpegConvert::ifilter_send_eof(InputFilter *ifilter, int64_t pts)
{
	int i, j, ret;

	ifilter->eof = 1;

	if (ifilter->filter) {
		ret = av_buffersrc_close(ifilter->filter, pts, AV_BUFFERSRC_FLAG_PUSH);
		if (ret < 0)
			return ret;
	}
	else {
		// the filtergraph was never configured
		FilterGraph *fg = ifilter->graph;
		for (i = 0; i < fg->nb_inputs; i++)
			if (!fg->inputs[i]->eof)
				break;
		if (i == fg->nb_inputs) {
			// All the input streams have finished without the filtergraph
			// ever being configured.
			// Mark the output streams as finished.
			for (j = 0; j < fg->nb_outputs; j++)
				finish_output_stream(fg->outputs[j]->ost);
		}
	}

	return 0;
}

int FfmpegConvert::decode(AVCodecContext *avctx, AVFrame *frame, int *got_frame, AVPacket *pkt)
{
	int ret;

	*got_frame = 0;

	if (pkt) {
		ret = avcodec_send_packet(avctx, pkt);
		// In particular, we don't expect AVERROR(EAGAIN), because we read all
		// decoded frames with avcodec_receive_frame() until done.
		if (ret < 0 && ret != AVERROR_EOF)
			return ret;
	}

	ret = avcodec_receive_frame(avctx, frame);
	if (ret < 0 && ret != AVERROR(EAGAIN))
		return ret;
	if (ret >= 0)
		*got_frame = 1;

	return 0;
}

int FfmpegConvert::send_frame_to_filters(InputStream *ist, AVFrame *decoded_frame)
{
	int i, ret;
	AVFrame *f;

	av_assert1(ist->nb_filters > 0); /* ensure ret is initialized */
	for (i = 0; i < ist->nb_filters; i++) {
		if (i < ist->nb_filters - 1) {
			f = ist->filter_frame;
			ret = av_frame_ref(f, decoded_frame);
			if (ret < 0)
				break;
		}
		else
			f = decoded_frame;
		ret = ifilter_send_frame(ist->filters[i], f);
		if (ret == AVERROR_EOF)
			ret = 0; /* ignore */
		if (ret < 0) {
			break;
		}
	}
	return ret;
}

int FfmpegConvert::decode_audio(InputStream *ist, AVPacket *pkt, int *got_output,
	int *decode_failed)
{
	AVFrame *decoded_frame;
	AVCodecContext *avctx = ist->dec_ctx;
	int ret, err = 0;
	AVRational decoded_frame_tb;

	if (!ist->decoded_frame && !(ist->decoded_frame = av_frame_alloc()))
		return AVERROR(ENOMEM);
	if (!ist->filter_frame && !(ist->filter_frame = av_frame_alloc()))
		return AVERROR(ENOMEM);
	decoded_frame = ist->decoded_frame;

	ret = decode(avctx, decoded_frame, got_output, pkt);
	if (ret < 0)
		*decode_failed = 1;

	if (ret >= 0 && avctx->sample_rate <= 0) {
		av_log(avctx, AV_LOG_ERROR, "Sample rate %d invalid\n", avctx->sample_rate);
		ret = AVERROR_INVALIDDATA;
	}



	if (!*got_output || ret < 0)
		return ret;

	ist->samples_decoded += decoded_frame->nb_samples;
	ist->frames_decoded++;

#if 1
	/* increment next_dts to use for the case where the input stream does not
	have timestamps or there are multiple frames in the packet */
	ist->next_pts += ((int64_t)AV_TIME_BASE * decoded_frame->nb_samples) /
		avctx->sample_rate;
	ist->next_dts += ((int64_t)AV_TIME_BASE * decoded_frame->nb_samples) /
		avctx->sample_rate;
#endif

	if (decoded_frame->pts != AV_NOPTS_VALUE) {
		decoded_frame_tb = ist->st->time_base;
	}
	else if (pkt && pkt->pts != AV_NOPTS_VALUE) {
		decoded_frame->pts = pkt->pts;
		decoded_frame_tb = ist->st->time_base;
	}
	else {
		decoded_frame->pts = ist->dts;
		decoded_frame_tb = AV_TIME_BASE_Q;
	}

	AVRational AVRQ{ 1, avctx->sample_rate };
	if (decoded_frame->pts != AV_NOPTS_VALUE)
		decoded_frame->pts = av_rescale_delta(decoded_frame_tb, decoded_frame->pts,
			AVRQ, decoded_frame->nb_samples, &ist->filter_in_rescale_delta_last,
			AVRQ);
	ist->nb_samples = decoded_frame->nb_samples;
	err = send_frame_to_filters(ist, decoded_frame);

	av_frame_unref(ist->filter_frame);
	av_frame_unref(decoded_frame);
	return err < 0 ? err : ret;
}

int FfmpegConvert::decode_video(InputStream *ist, AVPacket *pkt, int *got_output, int64_t *duration_pts, int eof,
	int *decode_failed)
{
	AVFrame *decoded_frame;
	int i, ret = 0, err = 0;
	int64_t best_effort_timestamp;
	int64_t dts = AV_NOPTS_VALUE;
	AVPacket avpkt;

	// With fate-indeo3-2, we're getting 0-sized packets before EOF for some
	// reason. This seems like a semi-critical bug. Don't trigger EOF, and
	// skip the packet.
	if (!eof && pkt && pkt->size == 0)
		return 0;

	if (!ist->decoded_frame && !(ist->decoded_frame = av_frame_alloc()))
		return AVERROR(ENOMEM);
	if (!ist->filter_frame && !(ist->filter_frame = av_frame_alloc()))
		return AVERROR(ENOMEM);
	decoded_frame = ist->decoded_frame;
	if (ist->dts != AV_NOPTS_VALUE)
		dts = av_rescale_q(ist->dts, AV_TIME_BASE_Q, ist->st->time_base);
	if (pkt) {
		avpkt = *pkt;
		avpkt.dts = dts; // ffmpeg.c probably shouldn't do this
	}

	// The old code used to set dts on the drain packet, which does not work
	// with the new API anymore.
	if (eof) {
		void *new_buffer = av_realloc_array(ist->dts_buffer, ist->nb_dts_buffer + 1, sizeof(ist->dts_buffer[0]));
		ist->dts_buffer = (int64_t *)new_buffer;
		ist->dts_buffer[ist->nb_dts_buffer++] = dts;
	}

	ret = decode(ist->dec_ctx, decoded_frame, got_output, pkt ? &avpkt : NULL);
	if (ret < 0)
		*decode_failed = 1;

	// The following line may be required in some cases where there is no parser
	// or the parser does not has_b_frames correctly
	if (ist->st->codecpar->video_delay < ist->dec_ctx->has_b_frames) {
		if (ist->dec_ctx->codec_id == AV_CODEC_ID_H264) {
			ist->st->codecpar->video_delay = ist->dec_ctx->has_b_frames;
		}
	}

	if (!*got_output || ret < 0)
		return ret;

	if (ist->top_field_first >= 0)
		decoded_frame->top_field_first = ist->top_field_first;

	ist->frames_decoded++;

	if (ist->hwaccel_retrieve_data && decoded_frame->format == ist->hwaccel_pix_fmt) {
		err = ist->hwaccel_retrieve_data(ist->dec_ctx, decoded_frame);
		if (err < 0)
			goto fail;
	}
	ist->hwaccel_retrieved_pix_fmt = (AVPixelFormat)decoded_frame->format;

	best_effort_timestamp = decoded_frame->best_effort_timestamp;
	*duration_pts = decoded_frame->pkt_duration;

	if (ist->framerate.num)
		best_effort_timestamp = ist->cfr_next_pts++;

	if (eof && best_effort_timestamp == AV_NOPTS_VALUE && ist->nb_dts_buffer > 0) {
		best_effort_timestamp = ist->dts_buffer[0];

		for (i = 0; i < ist->nb_dts_buffer - 1; i++)
			ist->dts_buffer[i] = ist->dts_buffer[i + 1];
		ist->nb_dts_buffer--;
	}

	if (best_effort_timestamp != AV_NOPTS_VALUE) {
		int64_t ts = av_rescale_q(decoded_frame->pts = best_effort_timestamp, ist->st->time_base, AV_TIME_BASE_Q);

		if (ts != AV_NOPTS_VALUE)
			ist->next_pts = ist->pts = ts;
	}

	if (ist->st->sample_aspect_ratio.num)
		decoded_frame->sample_aspect_ratio = ist->st->sample_aspect_ratio;

	err = send_frame_to_filters(ist, decoded_frame);

fail:
	av_frame_unref(ist->filter_frame);
	av_frame_unref(decoded_frame);
	return err < 0 ? err : ret;
}

int FfmpegConvert::transcode_subtitles(InputStream *ist, AVPacket *pkt, int *got_output,
	int *decode_failed)
{
	AVSubtitle subtitle;
	int free_sub = 1;
	int i, ret = avcodec_decode_subtitle2(ist->dec_ctx,
		&subtitle, got_output, pkt);

	if (ret < 0 || !*got_output) {
		*decode_failed = 1;
		if (!pkt->size)
			sub2video_flush(ist);
		return ret;
	}

	if (ist->fix_sub_duration) {
		int end = 1;
		if (ist->prev_sub.got_output) {
			end = av_rescale(subtitle.pts - ist->prev_sub.subtitle.pts,
				1000, AV_TIME_BASE);
			if (end < ist->prev_sub.subtitle.end_display_time) {
				ist->prev_sub.subtitle.end_display_time = end;
			}
		}
		FFSWAP(int, *got_output, ist->prev_sub.got_output);
		FFSWAP(int, ret, ist->prev_sub.ret);
		FFSWAP(AVSubtitle, subtitle, ist->prev_sub.subtitle);
		if (end <= 0)
			goto out;
	}

	if (!*got_output)
		return ret;

	if (ist->sub2video.frame) {
		sub2video_update(ist, &subtitle);
	}
	else if (ist->nb_filters) {
		if (!ist->sub2video.sub_queue)
			ist->sub2video.sub_queue = av_fifo_alloc(8 * sizeof(AVSubtitle));
		if (!ist->sub2video.sub_queue)
			exit_program(1);
		if (!av_fifo_space(ist->sub2video.sub_queue)) {
			ret = av_fifo_realloc2(ist->sub2video.sub_queue, 2 * av_fifo_size(ist->sub2video.sub_queue));
			if (ret < 0)
				exit_program(1);
		}
		av_fifo_generic_write(ist->sub2video.sub_queue, &subtitle, sizeof(subtitle), NULL);
		free_sub = 0;
	}

	if (!subtitle.num_rects)
		goto out;

	ist->frames_decoded++;

	for (i = 0; i < nb_output_streams; i++) {
		OutputStream *ost = output_streams[i];

		if (!check_output_constraints(ist, ost) || !ost->encoding_needed
			|| ost->enc->type != AVMEDIA_TYPE_SUBTITLE)
			continue;

		do_subtitle_out(output_files[ost->file_index], ost, &subtitle);
	}

out:
	if (free_sub)
		avsubtitle_free(&subtitle);
	return ret;
}

int FfmpegConvert::send_filter_eof(InputStream *ist)
{
	int i, ret;
	/* TODO keep pts also in stream time base to avoid converting back */
	int64_t pts = av_rescale_q_rnd(ist->pts, AV_TIME_BASE_Q, ist->st->time_base,
		(AVRounding)((int)AV_ROUND_NEAR_INF | (int)AV_ROUND_PASS_MINMAX));

	for (i = 0; i < ist->nb_filters; i++) {
		ret = ifilter_send_eof(ist->filters[i], pts);
		if (ret < 0)
			return ret;
	}
	return 0;
}

/* pkt = NULL means EOF (needed to flush decoder buffers) */
int FfmpegConvert::process_input_packet(InputStream *ist, const AVPacket *pkt, int no_eof)
{
	int ret = 0, i;
	int repeating = 0;
	int eof_reached = 0;

	AVPacket avpkt;
	if (!ist->saw_first_ts) {
		ist->dts = ist->st->avg_frame_rate.num ? -ist->dec_ctx->has_b_frames * AV_TIME_BASE / av_q2d(ist->st->avg_frame_rate) : 0;
		ist->pts = 0;
		if (pkt && pkt->pts != AV_NOPTS_VALUE && !ist->decoding_needed) {
			ist->dts += av_rescale_q(pkt->pts, ist->st->time_base, AV_TIME_BASE_Q);
			ist->pts = ist->dts; //unused but better to set it to a value thats not totally wrong
		}
		ist->saw_first_ts = 1;
	}

	if (ist->next_dts == AV_NOPTS_VALUE)
		ist->next_dts = ist->dts;
	if (ist->next_pts == AV_NOPTS_VALUE)
		ist->next_pts = ist->pts;

	if (!pkt) {
		/* EOF handling */
		av_init_packet(&avpkt);
		avpkt.data = NULL;
		avpkt.size = 0;
	}
	else {
		avpkt = *pkt;
	}

	if (pkt && pkt->dts != AV_NOPTS_VALUE) {
		ist->next_dts = ist->dts = av_rescale_q(pkt->dts, ist->st->time_base, AV_TIME_BASE_Q);
		if (ist->dec_ctx->codec_type != AVMEDIA_TYPE_VIDEO || !ist->decoding_needed)
			ist->next_pts = ist->pts = ist->dts;
	}

	// while we have more to decode or while the decoder did output something on EOF
	while (ist->decoding_needed) {
		int64_t duration_dts = 0;
		int64_t duration_pts = 0;
		int got_output = 0;
		int decode_failed = 0;

		ist->pts = ist->next_pts;
		ist->dts = ist->next_dts;

		switch (ist->dec_ctx->codec_type) {
		case AVMEDIA_TYPE_AUDIO:
			ret = decode_audio(ist, repeating ? NULL : &avpkt, &got_output,
				&decode_failed);
			break;
		case AVMEDIA_TYPE_VIDEO:
			ret = decode_video(ist, repeating ? NULL : &avpkt, &got_output, &duration_pts, !pkt,
				&decode_failed);
			if (!repeating || !pkt || got_output) {
				if (pkt && pkt->duration) {
					duration_dts = av_rescale_q(pkt->duration, ist->st->time_base, AV_TIME_BASE_Q);
				}
				else if (ist->dec_ctx->framerate.num != 0 && ist->dec_ctx->framerate.den != 0) {
					int ticks = av_stream_get_parser(ist->st) ? av_stream_get_parser(ist->st)->repeat_pict + 1 : ist->dec_ctx->ticks_per_frame;
					duration_dts = ((int64_t)AV_TIME_BASE *
						ist->dec_ctx->framerate.den * ticks) /
						ist->dec_ctx->framerate.num / ist->dec_ctx->ticks_per_frame;
				}

				if (ist->dts != AV_NOPTS_VALUE && duration_dts) {
					ist->next_dts += duration_dts;
				}
				else
					ist->next_dts = AV_NOPTS_VALUE;
			}

			if (got_output) {
				if (duration_pts > 0) {
					ist->next_pts += av_rescale_q(duration_pts, ist->st->time_base, AV_TIME_BASE_Q);
				}
				else {
					ist->next_pts += duration_dts;
				}
			}
			break;
		case AVMEDIA_TYPE_SUBTITLE:
			if (repeating)
				break;
			ret = transcode_subtitles(ist, &avpkt, &got_output, &decode_failed);
			if (!pkt && ret >= 0)
				ret = AVERROR_EOF;
			break;
		default:
			return -1;
		}

		if (ret == AVERROR_EOF) {
			eof_reached = 1;
			break;
		}

		if (ret < 0) {
			if (!decode_failed)
				exit_program(1);
			break;
		}

		if (got_output)
			ist->got_output = 1;

		if (!got_output)
			break;

		if (!pkt)
			break;

		repeating = 1;
	}

	/* after flushing, send an EOF on all the filter inputs attached to the stream */
	/* except when looping we need to flush but not to send an EOF */
	if (!pkt && ist->decoding_needed && eof_reached && !no_eof) {
		int ret = send_filter_eof(ist);
		if (ret < 0) {
			av_log(NULL, AV_LOG_FATAL, "Error marking filters as finished\n");
			exit_program(1);
		}
	}

	/* handle stream copy */
	if (!ist->decoding_needed && pkt) {
		ist->dts = ist->next_dts;
		switch (ist->dec_ctx->codec_type) {
		case AVMEDIA_TYPE_AUDIO:
			ist->next_dts += ((int64_t)AV_TIME_BASE * ist->dec_ctx->frame_size) /
				ist->dec_ctx->sample_rate;
			break;
		case AVMEDIA_TYPE_VIDEO:
			if (ist->framerate.num) {
				// TODO: Remove work-around for c99-to-c89 issue 7
				AVRational time_base_q = AV_TIME_BASE_Q;
				int64_t next_dts = av_rescale_q(ist->next_dts, time_base_q, av_inv_q(ist->framerate));
				ist->next_dts = av_rescale_q(next_dts + 1, av_inv_q(ist->framerate), time_base_q);
			}
			else if (pkt->duration) {
				ist->next_dts += av_rescale_q(pkt->duration, ist->st->time_base, AV_TIME_BASE_Q);
			}
			else if (ist->dec_ctx->framerate.num != 0) {
				int ticks = av_stream_get_parser(ist->st) ? av_stream_get_parser(ist->st)->repeat_pict + 1 : ist->dec_ctx->ticks_per_frame;
				ist->next_dts += ((int64_t)AV_TIME_BASE *
					ist->dec_ctx->framerate.den * ticks) /
					ist->dec_ctx->framerate.num / ist->dec_ctx->ticks_per_frame;
			}
			break;
		}
		ist->pts = ist->dts;
		ist->next_pts = ist->next_dts;
	}
	for (i = 0; i < nb_output_streams; i++) {
		OutputStream *ost = output_streams[i];

		if (!check_output_constraints(ist, ost) || ost->encoding_needed)
			continue;

		do_streamcopy(ist, ost, pkt);
	}

	return !eof_reached;
}

int FfmpegConvert::init_input_stream(int ist_index, char *error, int error_len)
{
	int ret;
	InputStream *ist = input_streams[ist_index];

	if (ist->decoding_needed) {
		AVCodec *codec = ist->dec;
		if (!codec) {
			snprintf(error, error_len, "Decoder (codec %s) not found for input stream #%d:%d",
				avcodec_get_name(ist->dec_ctx->codec_id), ist->file_index, ist->st->index);
			return AVERROR(EINVAL);
		}

		ist->dec_ctx->opaque = ist;
		ist->dec_ctx->get_format = get_format;
		ist->dec_ctx->get_buffer2 = get_buffer;
		ist->dec_ctx->thread_safe_callbacks = 1;

		av_opt_set_int(ist->dec_ctx, "refcounted_frames", 1, 0);
		if (ist->dec_ctx->codec_id == AV_CODEC_ID_DVB_SUBTITLE &&
			(ist->decoding_needed & DECODING_FOR_OST)) {
			av_dict_set(&ist->decoder_opts, "compute_edt", "1", AV_DICT_DONT_OVERWRITE);
			if (ist->decoding_needed & DECODING_FOR_FILTER)
				av_log(NULL, AV_LOG_WARNING, "Warning using DVB subtitles for filtering and output at the same time is not fully supported, also see -compute_edt [0|1]\n");
		}

		av_dict_set(&ist->decoder_opts, "sub_text_format", "ass", AV_DICT_DONT_OVERWRITE);

		/* Useful for subtitles retiming by lavf (FIXME), skipping samples in
		* audio, and video decoders such as cuvid or mediacodec */
		ist->dec_ctx->pkt_timebase = ist->st->time_base;

		if (!av_dict_get(ist->decoder_opts, "threads", NULL, 0))
			av_dict_set(&ist->decoder_opts, "threads", "auto", 0);
		/* Attached pics are sparse, therefore we would not want to delay their decoding till EOF. */
		if (ist->st->disposition & AV_DISPOSITION_ATTACHED_PIC)
			av_dict_set(&ist->decoder_opts, "threads", "1", 0);

		if ((ret = avcodec_open2(ist->dec_ctx, codec, &ist->decoder_opts)) < 0) {
			if (ret == AVERROR_EXPERIMENTAL)
				abort_codec_experimental(codec, 0);
			return ret;
		}
		assert_avoptions(ist->decoder_opts);
	}

	ist->next_pts = AV_NOPTS_VALUE;
	ist->next_dts = AV_NOPTS_VALUE;

	return 0;
}

InputStream* FfmpegConvert::get_input_stream(OutputStream *ost)
{
	if (ost->source_index >= 0)
		return input_streams[ost->source_index];
	return NULL;
}

/* open the muxer when all the streams are initialized */
int FfmpegConvert::check_init_output_file(OutputFile *of, int file_index)
{
	int ret, i;

	for (i = 0; i < of->ctx->nb_streams; i++) {
		OutputStream *ost = output_streams[of->ost_index + i];
		if (!ost->initialized)
			return 0;
	}

	of->ctx->interrupt_callback = int_cb;

	ret = avformat_write_header(of->ctx, &of->opts);
	if (ret < 0) {
		return ret;
	}
	//assert_avoptions(of->opts);
	of->header_written = 1;

	av_dump_format(of->ctx, file_index, of->ctx->url, 1);

	/* flush the muxing queues */
	for (i = 0; i < of->ctx->nb_streams; i++) {
		OutputStream *ost = output_streams[of->ost_index + i];

		/* try to improve muxing time_base (only possible if nothing has been written yet) */
		if (!av_fifo_size(ost->muxing_queue))
			ost->mux_timebase = ost->st->time_base;

		while (av_fifo_size(ost->muxing_queue)) {
			AVPacket pkt;
			av_fifo_generic_read(ost->muxing_queue, &pkt, sizeof(pkt), NULL);
			write_packet(of, &pkt, ost, 1);
		}
	}

	return 0;
}

int FfmpegConvert::init_output_bsfs(OutputStream *ost)
{
	AVBSFContext *ctx;
	int i, ret;

	if (!ost->nb_bitstream_filters)
		return 0;

	for (i = 0; i < ost->nb_bitstream_filters; i++) {
		ctx = ost->bsf_ctx[i];

		ret = avcodec_parameters_copy(ctx->par_in,
			i ? ost->bsf_ctx[i - 1]->par_out : ost->st->codecpar);
		if (ret < 0)
			return ret;

		ctx->time_base_in = i ? ost->bsf_ctx[i - 1]->time_base_out : ost->st->time_base;

		ret = av_bsf_init(ctx);
		if (ret < 0) {
			av_log(NULL, AV_LOG_ERROR, "Error initializing bitstream filter: %s\n",
				ost->bsf_ctx[i]->filter->name);
			return ret;
		}
	}

	ctx = ost->bsf_ctx[ost->nb_bitstream_filters - 1];
	ret = avcodec_parameters_copy(ost->st->codecpar, ctx->par_out);
	if (ret < 0)
		return ret;

	ost->st->time_base = ctx->time_base_out;

	return 0;
}

int FfmpegConvert::init_output_stream_streamcopy(OutputStream *ost)
{
	OutputFile *of = output_files[ost->file_index];
	InputStream *ist = get_input_stream(ost);
	AVCodecParameters *par_dst = ost->st->codecpar;
	AVCodecParameters *par_src = ost->ref_par;
	AVRational sar;
	int i, ret;
	uint32_t codec_tag = par_dst->codec_tag;

	av_assert0(ist && !ost->filter);

	ret = avcodec_parameters_to_context(ost->enc_ctx, ist->st->codecpar);
	if (ret >= 0)
		ret = av_opt_set_dict(ost->enc_ctx, &ost->encoder_opts);
	if (ret < 0) {
		av_log(NULL, AV_LOG_FATAL,
			"Error setting up codec context options.\n");
		return ret;
	}
	avcodec_parameters_from_context(par_src, ost->enc_ctx);

	if (!codec_tag) {
		unsigned int codec_tag_tmp;
		if (!of->ctx->oformat->codec_tag ||
			av_codec_get_id(of->ctx->oformat->codec_tag, par_src->codec_tag) == par_src->codec_id ||
			!av_codec_get_tag2(of->ctx->oformat->codec_tag, par_src->codec_id, &codec_tag_tmp))
			codec_tag = par_src->codec_tag;
	}

	ret = avcodec_parameters_copy(par_dst, par_src);
	if (ret < 0)
		return ret;

	par_dst->codec_tag = codec_tag;

	if (!ost->frame_rate.num)
		ost->frame_rate = ist->framerate;
	ost->st->avg_frame_rate = ost->frame_rate;

	ret = avformat_transfer_internal_stream_timing_info(of->ctx->oformat, ost->st, ist->st, (AVTimebaseSource)copy_tb);
	if (ret < 0)
		return ret;

	// copy timebase while removing common factors
	if (ost->st->time_base.num <= 0 || ost->st->time_base.den <= 0)
		ost->st->time_base = av_add_q(av_stream_get_codec_timebase(ost->st), AV_TIME_BASE_0_1);

	// copy estimated duration as a hint to the muxer
	if (ost->st->duration <= 0 && ist->st->duration > 0)
		ost->st->duration = av_rescale_q(ist->st->duration, ist->st->time_base, ost->st->time_base);

	// copy disposition
	ost->st->disposition = ist->st->disposition;

	if (ist->st->nb_side_data) {
		for (i = 0; i < ist->st->nb_side_data; i++) {
			const AVPacketSideData *sd_src = &ist->st->side_data[i];
			uint8_t *dst_data;

			dst_data = av_stream_new_side_data(ost->st, sd_src->type, sd_src->size);
			if (!dst_data)
				return AVERROR(ENOMEM);
			memcpy(dst_data, sd_src->data, sd_src->size);
		}
	}

	if (ost->rotate_overridden) {
		uint8_t *sd = av_stream_new_side_data(ost->st, AV_PKT_DATA_DISPLAYMATRIX,
			sizeof(int32_t) * 9);
		if (sd)
			av_display_rotation_set((int32_t *)sd, -ost->rotate_override_value);
	}

	ost->parser = av_parser_init(par_dst->codec_id);
	ost->parser_avctx = avcodec_alloc_context3(NULL);
	if (!ost->parser_avctx)
		return AVERROR(ENOMEM);

	switch (par_dst->codec_type) {
	case AVMEDIA_TYPE_AUDIO:
		if (audio_volume != 256) {
			av_log(NULL, AV_LOG_FATAL, "-acodec copy and -vol are incompatible (frames are not decoded)\n");
			exit_program(1);
		}
		if ((par_dst->block_align == 1 || par_dst->block_align == 1152 || par_dst->block_align == 576) && par_dst->codec_id == AV_CODEC_ID_MP3)
			par_dst->block_align = 0;
		if (par_dst->codec_id == AV_CODEC_ID_AC3)
			par_dst->block_align = 0;
		break;
	case AVMEDIA_TYPE_VIDEO:
		if (ost->frame_aspect_ratio.num) { // overridden by the -aspect cli option

			AVRational AVR_WH{ par_dst->height, par_dst->width };
			sar = av_mul_q(ost->frame_aspect_ratio, AVR_WH);
			av_log(NULL, AV_LOG_WARNING, "Overriding aspect ratio "
				"with stream copy may produce invalid files\n");
		}
		else if (ist->st->sample_aspect_ratio.num)
			sar = ist->st->sample_aspect_ratio;
		else
			sar = par_src->sample_aspect_ratio;
		ost->st->sample_aspect_ratio = par_dst->sample_aspect_ratio = sar;
		ost->st->avg_frame_rate = ist->st->avg_frame_rate;
		ost->st->r_frame_rate = ist->st->r_frame_rate;
		break;
	}

	ost->mux_timebase = ist->st->time_base;

	return 0;
}

void FfmpegConvert::set_encoder_id(OutputFile *of, OutputStream *ost)
{
	AVDictionaryEntry *e;

	uint8_t *encoder_string;
	int encoder_string_len;
	int format_flags = 0;
	int codec_flags = ost->enc_ctx->flags;

	if (av_dict_get(ost->st->metadata, "encoder", NULL, 0))
		return;

	e = av_dict_get(of->opts, "fflags", NULL, 0);
	if (e) {
		const AVOption *o = av_opt_find(of->ctx, "fflags", NULL, 0, 0);
		if (!o)
			return;
		av_opt_eval_flags(of->ctx, o, e->value, &format_flags);
	}
	e = av_dict_get(ost->encoder_opts, "flags", NULL, 0);
	if (e) {
		const AVOption *o = av_opt_find(ost->enc_ctx, "flags", NULL, 0, 0);
		if (!o)
			return;
		av_opt_eval_flags(ost->enc_ctx, o, e->value, &codec_flags);
	}

	encoder_string_len = sizeof(LIBAVCODEC_IDENT) + strlen(ost->enc->name) + 2;
	encoder_string = (uint8_t*)av_mallocz(encoder_string_len);


	if (!(format_flags & AVFMT_FLAG_BITEXACT) && !(codec_flags & AV_CODEC_FLAG_BITEXACT))
		av_strlcpy((char *)encoder_string, LIBAVCODEC_IDENT " ", encoder_string_len);
	else
		av_strlcpy((char *)encoder_string, "Lavc ", encoder_string_len);
	av_strlcat((char *)encoder_string, ost->enc->name, encoder_string_len);
	av_dict_set(&ost->st->metadata, "encoder", (const char *)encoder_string,
		AV_DICT_DONT_STRDUP_VAL | AV_DICT_DONT_OVERWRITE);
}

void FfmpegConvert::parse_forced_key_frames(char *kf, OutputStream *ost,
	AVCodecContext *avctx)
{
	char *p;
	int n = 1, i, size, index = 0;
	int64_t t, *pts;

	for (p = kf; *p; p++)
		if (*p == ',')
			n++;
	size = n;
	pts = (int64_t*)av_malloc_array(size, sizeof(*pts));


	p = kf;
	for (i = 0; i < n; i++) {
		char *next = strchr(p, ',');

		if (next)
			*next++ = 0;

		if (!memcmp(p, "chapters", 8)) {

			AVFormatContext *avf = output_files[ost->file_index]->ctx;
			int j;

			if (avf->nb_chapters > INT_MAX - size ||
				!(pts = (int64_t*)av_realloc_f(pts, size += avf->nb_chapters - 1,
					sizeof(*pts)))) {
				av_log(NULL, AV_LOG_FATAL,
					"Could not allocate forced key frames array.\n");
				exit_program(1);
			}
			t = p[8] ? parse_time_or_die("force_key_frames", p + 8, 1) : 0;
			t = av_rescale_q(t, AV_TIME_BASE_Q, avctx->time_base);

			for (j = 0; j < avf->nb_chapters; j++) {
				AVChapter *c = avf->chapters[j];
				av_assert1(index < size);
				pts[index++] = av_rescale_q(c->start, c->time_base,
					avctx->time_base) + t;
			}
		}
		else {
			t = parse_time_or_die("force_key_frames", p, 1);
			av_assert1(index < size);
			pts[index++] = av_rescale_q(t, AV_TIME_BASE_Q, avctx->time_base);
		}
		p = next;
	}

	av_assert0(index == size);
	qsort(pts, size, sizeof(*pts), compare_int64);
	ost->forced_kf_count = size;
	ost->forced_kf_pts = pts;
}

void FfmpegConvert::init_encoder_time_base(OutputStream *ost, AVRational default_time_base)
{
	InputStream *ist = get_input_stream(ost);
	AVCodecContext *enc_ctx = ost->enc_ctx;
	AVFormatContext *oc;

	if (ost->enc_timebase.num > 0) {
		enc_ctx->time_base = ost->enc_timebase;
		return;
	}

	if (ost->enc_timebase.num < 0) {
		if (ist) {
			enc_ctx->time_base = ist->st->time_base;
			return;
		}

		oc = output_files[ost->file_index]->ctx;
		av_log(oc, AV_LOG_WARNING, "Input stream data not available, using default time base\n");
	}

	enc_ctx->time_base = default_time_base;
}

int FfmpegConvert::init_output_stream_encode(OutputStream *ost)
{
	InputStream *ist = get_input_stream(ost);
	AVCodecContext *enc_ctx = ost->enc_ctx;
	AVCodecContext *dec_ctx = NULL;
	AVFormatContext *oc = output_files[ost->file_index]->ctx;
	int j, ret;

	set_encoder_id(output_files[ost->file_index], ost);

	// Muxers use AV_PKT_DATA_DISPLAYMATRIX to signal rotation. On the other
	// hand, the legacy API makes demuxers set "rotate" metadata entries,
	// which have to be filtered out to prevent leaking them to output files.
	av_dict_set(&ost->st->metadata, "rotate", NULL, 0);

	if (ist) {
		ost->st->disposition = ist->st->disposition;

		dec_ctx = ist->dec_ctx;

		enc_ctx->chroma_sample_location = dec_ctx->chroma_sample_location;
	}
	else {
		for (j = 0; j < oc->nb_streams; j++) {
			AVStream *st = oc->streams[j];
			if (st != ost->st && st->codecpar->codec_type == ost->st->codecpar->codec_type)
				break;
		}
		if (j == oc->nb_streams)
			if (ost->st->codecpar->codec_type == AVMEDIA_TYPE_AUDIO ||
				ost->st->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
				ost->st->disposition = AV_DISPOSITION_DEFAULT;
	}

	if (enc_ctx->codec_type == AVMEDIA_TYPE_VIDEO) {
		if (!ost->frame_rate.num)
			ost->frame_rate = av_buffersink_get_frame_rate(ost->filter->filter);
		if (ist && !ost->frame_rate.num)
			ost->frame_rate = ist->framerate;
		if (ist && !ost->frame_rate.num)
			ost->frame_rate = ist->st->r_frame_rate;
		if (ist && !ost->frame_rate.num) {
			ost->frame_rate = AV_TIME_BASE_25_1;
		}
		if (ost->enc->supported_framerates && !ost->force_fps) {
			int idx = av_find_nearest_q_idx(ost->frame_rate, ost->enc->supported_framerates);
			ost->frame_rate = ost->enc->supported_framerates[idx];
		}
		// reduce frame rate for mpeg4 to be within the spec limits
		if (enc_ctx->codec_id == AV_CODEC_ID_MPEG4) {
			av_reduce(&ost->frame_rate.num, &ost->frame_rate.den,
				ost->frame_rate.num, ost->frame_rate.den, 65535);
		}
	}

	AVRational AVR_WH{ enc_ctx->height, enc_ctx->width };

	switch (enc_ctx->codec_type) {
	case AVMEDIA_TYPE_AUDIO:
		enc_ctx->sample_fmt = (AVSampleFormat)av_buffersink_get_format(ost->filter->filter);
		if (dec_ctx)
			enc_ctx->bits_per_raw_sample = FFMIN(dec_ctx->bits_per_raw_sample,
				av_get_bytes_per_sample(enc_ctx->sample_fmt) << 3);
		enc_ctx->sample_rate = av_buffersink_get_sample_rate(ost->filter->filter);
		enc_ctx->channel_layout = av_buffersink_get_channel_layout(ost->filter->filter);
		enc_ctx->channels = av_buffersink_get_channels(ost->filter->filter);

		init_encoder_time_base(ost, av_make_q(1, enc_ctx->sample_rate));
		break;

	case AVMEDIA_TYPE_VIDEO:
		init_encoder_time_base(ost, av_inv_q(ost->frame_rate));

		if (!(enc_ctx->time_base.num && enc_ctx->time_base.den))
			enc_ctx->time_base = av_buffersink_get_time_base(ost->filter->filter);
		if (av_q2d(enc_ctx->time_base) < 0.001 && video_sync_method != VSYNC_PASSTHROUGH
			&& (video_sync_method == VSYNC_CFR || video_sync_method == VSYNC_VSCFR || (video_sync_method == VSYNC_AUTO && !(oc->oformat->flags & AVFMT_VARIABLE_FPS)))) {
			av_log(oc, AV_LOG_WARNING, "Frame rate very high for a muxer not efficiently supporting it.\n"
				"Please consider specifying a lower framerate, a different muxer or -vsync 2\n");
		}
		for (j = 0; j < ost->forced_kf_count; j++)
			ost->forced_kf_pts[j] = av_rescale_q(ost->forced_kf_pts[j],
				AV_TIME_BASE_Q,
				enc_ctx->time_base);

		enc_ctx->width = av_buffersink_get_w(ost->filter->filter);
		enc_ctx->height = av_buffersink_get_h(ost->filter->filter);
		AVR_WH.den = enc_ctx->width;
		AVR_WH.num = enc_ctx->height;
		enc_ctx->sample_aspect_ratio = ost->st->sample_aspect_ratio =
			ost->frame_aspect_ratio.num ? // overridden by the -aspect cli option
			av_mul_q(ost->frame_aspect_ratio, AVR_WH) :
			av_buffersink_get_sample_aspect_ratio(ost->filter->filter);

		enc_ctx->pix_fmt = (AVPixelFormat)av_buffersink_get_format(ost->filter->filter);
		if (dec_ctx)
			enc_ctx->bits_per_raw_sample = FFMIN(dec_ctx->bits_per_raw_sample,
				av_pix_fmt_desc_get(enc_ctx->pix_fmt)->comp[0].depth);

		enc_ctx->framerate = ost->frame_rate;

		ost->st->avg_frame_rate = ost->frame_rate;

		if (!dec_ctx ||
			enc_ctx->width != dec_ctx->width ||
			enc_ctx->height != dec_ctx->height ||
			enc_ctx->pix_fmt != dec_ctx->pix_fmt) {
			enc_ctx->bits_per_raw_sample = frame_bits_per_raw_sample;
		}

		if (ost->forced_keyframes) {
			if (!strncmp(ost->forced_keyframes, "expr:", 5)) {
				ret = av_expr_parse(&ost->forced_keyframes_pexpr, ost->forced_keyframes + 5,
					forced_keyframes_const_names, NULL, NULL, NULL, NULL, 0, NULL);
				if (ret < 0) {
					av_log(NULL, AV_LOG_ERROR,
						"Invalid force_key_frames expression '%s'\n", ost->forced_keyframes + 5);
					return ret;
				}
				ost->forced_keyframes_expr_const_values[FKF_N] = 0;
				ost->forced_keyframes_expr_const_values[FKF_N_FORCED] = 0;
				ost->forced_keyframes_expr_const_values[FKF_PREV_FORCED_N] = NAN;
				ost->forced_keyframes_expr_const_values[FKF_PREV_FORCED_T] = NAN;

				// Don't parse the 'forced_keyframes' in case of 'keep-source-keyframes',
				// parse it only for kf timings
			}
			else if (strncmp(ost->forced_keyframes, "source", 6)) {
				parse_forced_key_frames(ost->forced_keyframes, ost, ost->enc_ctx);
			}
		}
		break;
	case AVMEDIA_TYPE_SUBTITLE:
		enc_ctx->time_base = AV_TIME_BASE_Q;
		if (!enc_ctx->width) {
			enc_ctx->width = input_streams[ost->source_index]->st->codecpar->width;
			enc_ctx->height = input_streams[ost->source_index]->st->codecpar->height;
		}
		break;
	case AVMEDIA_TYPE_DATA:
		break;
	default:
		abort();
		break;
	}

	ost->mux_timebase = enc_ctx->time_base;

	return 0;
}

int FfmpegConvert::init_output_stream(OutputStream *ost, char *error, int error_len)
{
	int ret = 0;

	if (ost->encoding_needed) {
		AVCodec      *codec = ost->enc;
		AVCodecContext *dec = NULL;
		InputStream *ist;

		ret = init_output_stream_encode(ost);
		if (ret < 0)
			return ret;

		if ((ist = get_input_stream(ost)))
			dec = ist->dec_ctx;
		if (dec && dec->subtitle_header) {
			/* ASS code assumes this buffer is null terminated so add extra byte. */
			ost->enc_ctx->subtitle_header = (uint8_t *)av_mallocz(dec->subtitle_header_size + 1);
			memcpy(ost->enc_ctx->subtitle_header, dec->subtitle_header, dec->subtitle_header_size);
			ost->enc_ctx->subtitle_header_size = dec->subtitle_header_size;
		}
		if (!av_dict_get(ost->encoder_opts, "threads", NULL, 0))
			av_dict_set(&ost->encoder_opts, "threads", "auto", 0);
		if (ost->enc->type == AVMEDIA_TYPE_AUDIO &&
			!codec->defaults &&
			!av_dict_get(ost->encoder_opts, "b", NULL, 0) &&
			!av_dict_get(ost->encoder_opts, "ab", NULL, 0))
			av_dict_set(&ost->encoder_opts, "b", "128000", 0);


		//X264 编码器
		if (stricmp(codec->name, "libx264") == 0) {
			ost->enc_ctx->qmin = 10;
			ost->enc_ctx->qmax = 51;
			av_opt_set(ost->enc_ctx->priv_data, "chromaoffset", "-2", 0);
			//av_opt_set(ost->enc_ctx->priv_data, "profile", "Baseline", 0);
			av_opt_set(ost->enc_ctx->priv_data, "tune", "zerolatency", 0);
			ost->enc_ctx->thread_count = 1;
			ost->enc_ctx->thread_type = FF_THREAD_SLICE;
			ost->enc_ctx->max_b_frames = 0;
			ost->enc_ctx->refs = 1;
		}

		if ((ret = avcodec_open2(ost->enc_ctx, codec, &ost->encoder_opts)) < 0) {
			if (ret == AVERROR_EXPERIMENTAL)
				abort_codec_experimental(codec, 1);

			return ret;
		}
		if (ost->enc->type == AVMEDIA_TYPE_AUDIO &&
			!(ost->enc->capabilities & AV_CODEC_CAP_VARIABLE_FRAME_SIZE))
			av_buffersink_set_frame_size(ost->filter->filter,
				ost->enc_ctx->frame_size);
		assert_avoptions(ost->encoder_opts);
		if (ost->enc_ctx->bit_rate && ost->enc_ctx->bit_rate < 1000)
			av_log(NULL, AV_LOG_WARNING, "The bitrate parameter is set too low."
				" It takes bits/s as argument, not kbits/s\n");

		ret = avcodec_parameters_from_context(ost->st->codecpar, ost->enc_ctx);
		if (ret < 0) {
			av_log(NULL, AV_LOG_FATAL,
				"Error initializing the output stream codec context.\n");
			exit_program(1);
		}
		/*
		* FIXME: ost->st->codec should't be needed here anymore.
		*/
		ret = avcodec_copy_context(ost->st->codec, ost->enc_ctx);
		if (ret < 0)
			return ret;

		if (ost->enc_ctx->nb_coded_side_data) {
			int i;

			for (i = 0; i < ost->enc_ctx->nb_coded_side_data; i++) {
				const AVPacketSideData *sd_src = &ost->enc_ctx->coded_side_data[i];
				uint8_t *dst_data;

				dst_data = av_stream_new_side_data(ost->st, sd_src->type, sd_src->size);
				if (!dst_data)
					return AVERROR(ENOMEM);
				memcpy(dst_data, sd_src->data, sd_src->size);
			}
		}

		/*
		* Add global input side data. For now this is naive, and copies it
		* from the input stream's global side data. All side data should
		* really be funneled over AVFrame and libavfilter, then added back to
		* packet side data, and then potentially using the first packet for
		* global side data.
		*/
		if (ist) {
			int i;
			for (i = 0; i < ist->st->nb_side_data; i++) {
				AVPacketSideData *sd = &ist->st->side_data[i];
				uint8_t *dst = av_stream_new_side_data(ost->st, sd->type, sd->size);
				if (!dst)
					return AVERROR(ENOMEM);
				memcpy(dst, sd->data, sd->size);
				if (ist->autorotate && sd->type == AV_PKT_DATA_DISPLAYMATRIX)
					av_display_rotation_set((int32_t *)dst, 0);
			}
		}

		// copy timebase while removing common factors
		if (ost->st->time_base.num <= 0 || ost->st->time_base.den <= 0)
			ost->st->time_base = av_add_q(ost->enc_ctx->time_base, AV_TIME_BASE_0_1);

		// copy estimated duration as a hint to the muxer
		if (ost->st->duration <= 0 && ist && ist->st->duration > 0)
			ost->st->duration = av_rescale_q(ist->st->duration, ist->st->time_base, ost->st->time_base);

		ost->st->codec->codec = ost->enc_ctx->codec;
	}
	else if (ost->stream_copy) {
		ret = init_output_stream_streamcopy(ost);
		if (ret < 0)
			return ret;

		/*
		* FIXME: will the codec context used by the parser during streamcopy
		* This should go away with the new parser API.
		*/
		ret = avcodec_parameters_to_context(ost->parser_avctx, ost->st->codecpar);
		if (ret < 0)
			return ret;
	}

	// parse user provided disposition, and update stream values
	if (ost->disposition) {
		const AVOption opts[] = {
			{ "disposition"         , NULL, 0, AV_OPT_TYPE_FLAGS,{ 0 }, INT64_MIN, INT64_MAX, 0,"flags" },
			{ "default"             , NULL, 0, AV_OPT_TYPE_CONST,{ AV_DISPOSITION_DEFAULT },  0,0,0, "flags" },
			{ "dub"                 , NULL, 0, AV_OPT_TYPE_CONST,{ AV_DISPOSITION_DUB },  0,0,0, "flags" },
			{ "original"            , NULL, 0, AV_OPT_TYPE_CONST,{ AV_DISPOSITION_ORIGINAL },  0,0,0,"flags" },
			{ "comment"             , NULL, 0, AV_OPT_TYPE_CONST,{ AV_DISPOSITION_COMMENT },  0,0,0, "flags" },
			{ "lyrics"              , NULL, 0, AV_OPT_TYPE_CONST,{ AV_DISPOSITION_LYRICS },  0,0,0, "flags" },
			{ "karaoke"             , NULL, 0, AV_OPT_TYPE_CONST,{ AV_DISPOSITION_KARAOKE },  0,0,0, "flags" },
			{ "forced"              , NULL, 0, AV_OPT_TYPE_CONST,{ AV_DISPOSITION_FORCED },  0,0,0, "flags" },
			{ "hearing_impaired"    , NULL, 0, AV_OPT_TYPE_CONST,{ AV_DISPOSITION_HEARING_IMPAIRED },  0,0,0, "flags" },
			{ "visual_impaired"     , NULL, 0, AV_OPT_TYPE_CONST,{ AV_DISPOSITION_VISUAL_IMPAIRED },  0,0,0, "flags" },
			{ "clean_effects"       , NULL, 0, AV_OPT_TYPE_CONST,{ AV_DISPOSITION_CLEAN_EFFECTS },  0,0,0, "flags" },
			{ "captions"            , NULL, 0, AV_OPT_TYPE_CONST,{ AV_DISPOSITION_CAPTIONS },  0,0,0, "flags" },
			{ "descriptions"        , NULL, 0, AV_OPT_TYPE_CONST,{ AV_DISPOSITION_DESCRIPTIONS },  0,0,0,"flags" },
			{ "metadata"            , NULL, 0, AV_OPT_TYPE_CONST,{ AV_DISPOSITION_METADATA },  0,0,0, "flags" },
			{ NULL },
		};
		const AVClass _class = {
			"",
			av_default_item_name,
			opts,
			LIBAVUTIL_VERSION_INT,
		};
		const AVClass *pclass = &_class;

		ret = av_opt_eval_flags(&pclass, &opts[0], ost->disposition, &ost->st->disposition);
		if (ret < 0)
			return ret;
	}

	/* initialize bitstream filters for the output stream
	* needs to be done here, because the codec id for streamcopy is not
	* known until now */
	ret = init_output_bsfs(ost);
	if (ret < 0)
		return ret;

	ost->initialized = 1;

	ret = check_init_output_file(output_files[ost->file_index], ost->file_index);
	if (ret < 0)
		return ret;

	return ret;
}

void FfmpegConvert::report_new_stream(int input_index, AVPacket *pkt)
{
	InputFile *file = input_files[input_index];
	AVStream *st = file->ctx->streams[pkt->stream_index];

	if (pkt->stream_index < file->nb_streams_warn)
		return;
	file->nb_streams_warn = pkt->stream_index + 1;
}

int FfmpegConvert::transcode_init(void)
{
	int ret = 0, i, j, k;
	AVFormatContext *oc;
	OutputStream *ost;
	InputStream *ist;
	char error[1024] = { 0 };

	for (i = 0; i < nb_filtergraphs; i++) {
		FilterGraph *fg = filtergraphs[i];
		for (j = 0; j < fg->nb_outputs; j++) {
			OutputFilter *ofilter = fg->outputs[j];
			if (!ofilter->ost || ofilter->ost->source_index >= 0)
				continue;
			if (fg->nb_inputs != 1)
				continue;
			for (k = nb_input_streams - 1; k >= 0; k--)
				if (fg->inputs[0]->ist == input_streams[k])
					break;
			ofilter->ost->source_index = k;
		}
	}

	/* init framerate emulation */
	for (i = 0; i < nb_input_files; i++) {
		InputFile *ifile = input_files[i];
		if (ifile->rate_emu)
			for (j = 0; j < ifile->nb_streams; j++)
				input_streams[j + ifile->ist_index]->start = av_gettime_relative();
	}

	/* init input streams */
	for (i = 0; i < nb_input_streams; i++)
		if ((ret = init_input_stream(i, error, sizeof(error))) < 0) {
			for (i = 0; i < nb_output_streams; i++) {
				ost = output_streams[i];
				avcodec_close(ost->enc_ctx);
			}
			goto dump_format;
		}

	/* open each encoder */
	for (i = 0; i < nb_output_streams; i++) {
		// skip streams fed from filtergraphs until we have a frame for them
		if (output_streams[i]->filter)
			continue;

		ret = init_output_stream(output_streams[i], error, sizeof(error));
		if (ret < 0)
			goto dump_format;
	}

	/* discard unused programs */
	for (i = 0; i < nb_input_files; i++) {
		InputFile *ifile = input_files[i];
		for (j = 0; j < ifile->ctx->nb_programs; j++) {
			AVProgram *p = ifile->ctx->programs[j];
			int discard = AVDISCARD_ALL;

			for (k = 0; k < p->nb_stream_indexes; k++)
				if (!input_streams[ifile->ist_index + p->stream_index[k]]->discard) {
					discard = AVDISCARD_DEFAULT;
					break;
				}
			p->discard = (AVDiscard)discard;
		}
	}

	/* write headers for files with no streams */
	for (i = 0; i < nb_output_files; i++) {
		oc = output_files[i]->ctx;
		if (oc->oformat->flags & AVFMT_NOSTREAMS && oc->nb_streams == 0) {
			ret = check_init_output_file(output_files[i], i);
			if (ret < 0)
				goto dump_format;
		}
	}

dump_format:
	/* dump the stream mapping */
	av_log(NULL, AV_LOG_INFO, "Stream mapping:\n");
	for (i = 0; i < nb_input_streams; i++) {
		ist = input_streams[i];

		for (j = 0; j < ist->nb_filters; j++) {
			if (!filtergraph_is_simple(ist->filters[j]->graph)) {
				av_log(NULL, AV_LOG_INFO, "  Stream #%d:%d (%s) -> %s",
					ist->file_index, ist->st->index, ist->dec ? ist->dec->name : "?",
					ist->filters[j]->name);
				if (nb_filtergraphs > 1)
					av_log(NULL, AV_LOG_INFO, " (graph %d)", ist->filters[j]->graph->index);
				av_log(NULL, AV_LOG_INFO, "\n");
			}
		}
	}

	for (i = 0; i < nb_output_streams; i++) {
		ost = output_streams[i];

		if (ost->attachment_filename) {
			/* an attached file */
			av_log(NULL, AV_LOG_INFO, "  File %s -> Stream #%d:%d\n",
				ost->attachment_filename, ost->file_index, ost->index);
			continue;
		}

		if (ost->filter && !filtergraph_is_simple(ost->filter->graph)) {
			/* output from a complex graph */
			av_log(NULL, AV_LOG_INFO, "  %s", ost->filter->name);
			if (nb_filtergraphs > 1)
				av_log(NULL, AV_LOG_INFO, " (graph %d)", ost->filter->graph->index);

			av_log(NULL, AV_LOG_INFO, " -> Stream #%d:%d (%s)\n", ost->file_index,
				ost->index, ost->enc ? ost->enc->name : "?");
			continue;
		}

		av_log(NULL, AV_LOG_INFO, "  Stream #%d:%d -> #%d:%d",
			input_streams[ost->source_index]->file_index,
			input_streams[ost->source_index]->st->index,
			ost->file_index,
			ost->index);
		if (ost->sync_ist != input_streams[ost->source_index])
			av_log(NULL, AV_LOG_INFO, " [sync #%d:%d]",
				ost->sync_ist->file_index,
				ost->sync_ist->st->index);
		if (ost->stream_copy)
			av_log(NULL, AV_LOG_INFO, " (copy)");
		else {
			const AVCodec *in_codec = input_streams[ost->source_index]->dec;
			const AVCodec *out_codec = ost->enc;
			const char *decoder_name = "?";
			const char *in_codec_name = "?";
			const char *encoder_name = "?";
			const char *out_codec_name = "?";
			const AVCodecDescriptor *desc;

			if (in_codec) {
				decoder_name = in_codec->name;
				desc = avcodec_descriptor_get(in_codec->id);
				if (desc)
					in_codec_name = desc->name;
				if (!strcmp(decoder_name, in_codec_name))
					decoder_name = "native";
			}

			if (out_codec) {
				encoder_name = out_codec->name;
				desc = avcodec_descriptor_get(out_codec->id);
				if (desc)
					out_codec_name = desc->name;
				if (!strcmp(encoder_name, out_codec_name))
					encoder_name = "native";
			}

			av_log(NULL, AV_LOG_INFO, " (%s (%s) -> %s (%s))",
				in_codec_name, decoder_name,
				out_codec_name, encoder_name);
		}
		av_log(NULL, AV_LOG_INFO, "\n");
	}

	if (ret) {
		av_log(NULL, AV_LOG_ERROR, "%s\n", error);
		return ret;
	}
	return 0;
}

/* Return 1 if there remain streams where more output is wanted, 0 otherwise. */
int FfmpegConvert::need_output(void)
{
	int i;

	for (i = 0; i < nb_output_streams; i++) {
		OutputStream *ost = output_streams[i];
		OutputFile *of = output_files[ost->file_index];
		AVFormatContext *os = output_files[ost->file_index]->ctx;

		if (ost->finished ||
			(os->pb && avio_tell(os->pb) >= of->limit_filesize))
			continue;
		if (ost->frame_number >= ost->max_frames) {
			int j;
			for (j = 0; j < of->ctx->nb_streams; j++)
				close_output_stream(output_streams[of->ost_index + j]);
			continue;
		}

		return 1;
	}

	return 0;
}

OutputStream* FfmpegConvert::choose_output(void)
{
	int i;
	int64_t opts_min = INT64_MAX;
	OutputStream *ost_min = NULL;

	for (i = 0; i < nb_output_streams; i++) {
		OutputStream *ost = output_streams[i];
		int64_t opts = ost->st->cur_dts == AV_NOPTS_VALUE ? INT64_MIN :
			av_rescale_q(ost->st->cur_dts, ost->st->time_base,
				AV_TIME_BASE_Q);
		if (ost->st->cur_dts == AV_NOPTS_VALUE)
			av_log(NULL, AV_LOG_DEBUG, "cur_dts is invalid (this is harmless if it occurs once at the start per stream)\n");

		if (!ost->initialized && !ost->inputs_done)
			return ost;

		if (!ost->finished && opts < opts_min) {
			opts_min = opts;
			ost_min = ost->unavailable ? NULL : ost;
		}
	}
	return ost_min;
}



int FfmpegConvert::get_input_packet(InputFile *f, AVPacket *pkt)
{
	if (f->rate_emu) {
		int i;
		for (i = 0; i < f->nb_streams; i++) {
			InputStream *ist = input_streams[f->ist_index + i];
			int64_t pts = av_rescale(ist->dts, 1000000, AV_TIME_BASE);
			int64_t now = av_gettime_relative() - ist->start;
			if (pts > now)
				return AVERROR(EAGAIN);
		}
	}


	return av_read_frame(f->ctx, pkt);
}

int FfmpegConvert::got_eagain(void)
{
	int i;
	for (i = 0; i < nb_output_streams; i++)
		if (output_streams[i]->unavailable)
			return 1;
	return 0;
}

void FfmpegConvert::reset_eagain(void)
{
	int i;
	for (i = 0; i < nb_input_files; i++)
		input_files[i]->eagain = 0;
	for (i = 0; i < nb_output_streams; i++)
		output_streams[i]->unavailable = 0;
}

// set duration to max(tmp, duration) in a proper time base and return duration's time_base
AVRational FfmpegConvert::duration_max(int64_t tmp, int64_t *duration, AVRational tmp_time_base,
	AVRational time_base)
{
	int ret;

	if (!*duration) {
		*duration = tmp;
		return tmp_time_base;
	}

	ret = av_compare_ts(*duration, time_base, tmp, tmp_time_base);
	if (ret < 0) {
		*duration = tmp;
		return tmp_time_base;
	}

	return time_base;
}

int FfmpegConvert::seek_to_start(InputFile *ifile, AVFormatContext *is)
{
	InputStream *ist;
	AVCodecContext *avctx;
	int i, ret, has_audio = 0;
	int64_t duration = 0;

	ret = av_seek_frame(is, -1, is->start_time, 0);
	if (ret < 0)
		return ret;

	for (i = 0; i < ifile->nb_streams; i++) {
		ist = input_streams[ifile->ist_index + i];
		avctx = ist->dec_ctx;

		// flush decoders
		if (ist->decoding_needed) {
			process_input_packet(ist, NULL, 1);
			avcodec_flush_buffers(avctx);
		}

		/* duration is the length of the last frame in a stream
		* when audio stream is present we don't care about
		* last video frame length because it's not defined exactly */
		if (avctx->codec_type == AVMEDIA_TYPE_AUDIO && ist->nb_samples)
			has_audio = 1;
	}

	for (i = 0; i < ifile->nb_streams; i++) {
		ist = input_streams[ifile->ist_index + i];
		avctx = ist->dec_ctx;

		if (has_audio) {
			if (avctx->codec_type == AVMEDIA_TYPE_AUDIO && ist->nb_samples) {
				AVRational sample_rate = { 1, avctx->sample_rate };

				duration = av_rescale_q(ist->nb_samples, sample_rate, ist->st->time_base);
			}
			else {
				continue;
			}
		}
		else {
			if (ist->framerate.num) {
				duration = av_rescale_q(1, av_inv_q(ist->framerate), ist->st->time_base);
			}
			else if (ist->st->avg_frame_rate.num) {
				duration = av_rescale_q(1, av_inv_q(ist->st->avg_frame_rate), ist->st->time_base);
			}
			else {
				duration = 1;
			}
		}
		if (!ifile->duration)
			ifile->time_base = ist->st->time_base;
		/* the total duration of the stream, max_pts - min_pts is
		* the duration of the stream without the last frame */
		duration += ist->max_pts - ist->min_pts;
		ifile->time_base = duration_max(duration, &ifile->duration, ist->st->time_base,
			ifile->time_base);
	}

	if (ifile->loop > 0)
		ifile->loop--;

	return ret;
}

int FfmpegConvert::process_input(int file_index)
{
	InputFile *ifile = input_files[file_index];
	AVFormatContext *is;
	InputStream *ist;
	AVPacket pkt;
	int ret, i, j;
	int64_t duration;
	int64_t pkt_dts;

	is = ifile->ctx;
	ret = get_input_packet(ifile, &pkt);

	if (ret == AVERROR(EAGAIN)) {
		ifile->eagain = 1;
		return ret;
	}
	if (ret < 0 && ifile->loop) {
		ret = seek_to_start(ifile, is);
		if (ret < 0)
			av_log(NULL, AV_LOG_WARNING, "Seek to start failed.\n");
		else
			ret = get_input_packet(ifile, &pkt);
		if (ret == AVERROR(EAGAIN)) {
			ifile->eagain = 1;
			return ret;
		}
	}
	if (ret < 0) {
		for (i = 0; i < ifile->nb_streams; i++) {
			ist = input_streams[ifile->ist_index + i];
			if (ist->decoding_needed) {
				ret = process_input_packet(ist, NULL, 0);
				if (ret>0)
					return 0;
			}

			/* mark all outputs that don't go through lavfi as finished */
			for (j = 0; j < nb_output_streams; j++) {
				OutputStream *ost = output_streams[j];

				if (ost->source_index == ifile->ist_index + i &&
					(ost->stream_copy || ost->enc->type == AVMEDIA_TYPE_SUBTITLE))
					finish_output_stream(ost);
			}
		}

		ifile->eof_reached = 1;
		return AVERROR(EAGAIN);
	}

	reset_eagain();

	/* the following test is needed in case new streams appear
	dynamically in stream : we ignore them */
	if (pkt.stream_index >= ifile->nb_streams) {
		report_new_stream(file_index, &pkt);
		goto discard_packet;
	}

	ist = input_streams[ifile->ist_index + pkt.stream_index];

	ist->data_size += pkt.size;
	ist->nb_packets++;

	if (ist->discard)
		goto discard_packet;

	if (!ist->wrap_correction_done && is->start_time != AV_NOPTS_VALUE && ist->st->pts_wrap_bits < 64) {
		int64_t stime, stime2;
		// Correcting starttime based on the enabled streams
		// FIXME this ideally should be done before the first use of starttime but we do not know which are the enabled streams at that point.
		//       so we instead do it here as part of discontinuity handling
		if (ist->next_dts == AV_NOPTS_VALUE
			&& ifile->ts_offset == -is->start_time
			&& (is->iformat->flags & AVFMT_TS_DISCONT)) {
			int64_t new_start_time = INT64_MAX;
			for (i = 0; i<is->nb_streams; i++) {
				AVStream *st = is->streams[i];
				if (st->discard == AVDISCARD_ALL || st->start_time == AV_NOPTS_VALUE)
					continue;
				new_start_time = FFMIN(new_start_time, av_rescale_q(st->start_time, st->time_base, AV_TIME_BASE_Q));
			}
			if (new_start_time > is->start_time) {
				av_log(is, AV_LOG_VERBOSE, "Correcting start time by %" PRId64"\n", new_start_time - is->start_time);
				ifile->ts_offset = -new_start_time;
			}
		}

		stime = av_rescale_q(is->start_time, AV_TIME_BASE_Q, ist->st->time_base);
		stime2 = stime + (1ULL << ist->st->pts_wrap_bits);
		ist->wrap_correction_done = 1;

		if (stime2 > stime && pkt.dts != AV_NOPTS_VALUE && pkt.dts > stime + (1LL << (ist->st->pts_wrap_bits - 1))) {
			pkt.dts -= 1ULL << ist->st->pts_wrap_bits;
			ist->wrap_correction_done = 0;
		}
		if (stime2 > stime && pkt.pts != AV_NOPTS_VALUE && pkt.pts > stime + (1LL << (ist->st->pts_wrap_bits - 1))) {
			pkt.pts -= 1ULL << ist->st->pts_wrap_bits;
			ist->wrap_correction_done = 0;
		}
	}

	/* add the stream-global side data to the first packet */
	if (ist->nb_packets == 1) {
		for (i = 0; i < ist->st->nb_side_data; i++) {
			AVPacketSideData *src_sd = &ist->st->side_data[i];
			uint8_t *dst_data;

			if (src_sd->type == AV_PKT_DATA_DISPLAYMATRIX)
				continue;

			if (av_packet_get_side_data(&pkt, src_sd->type, NULL))
				continue;

			dst_data = av_packet_new_side_data(&pkt, src_sd->type, src_sd->size);
			if (!dst_data)
				exit_program(1);

			memcpy(dst_data, src_sd->data, src_sd->size);
		}
	}

	if (pkt.dts != AV_NOPTS_VALUE)
		pkt.dts += av_rescale_q(ifile->ts_offset, AV_TIME_BASE_Q, ist->st->time_base);
	if (pkt.pts != AV_NOPTS_VALUE)
		pkt.pts += av_rescale_q(ifile->ts_offset, AV_TIME_BASE_Q, ist->st->time_base);

	if (pkt.pts != AV_NOPTS_VALUE)
		pkt.pts *= ist->ts_scale;
	if (pkt.dts != AV_NOPTS_VALUE)
		pkt.dts *= ist->ts_scale;

	pkt_dts = av_rescale_q_rnd(pkt.dts, ist->st->time_base, AV_TIME_BASE_Q, (AVRounding)((int)AV_ROUND_NEAR_INF | (int)AV_ROUND_PASS_MINMAX));
	if ((ist->dec_ctx->codec_type == AVMEDIA_TYPE_VIDEO ||
		ist->dec_ctx->codec_type == AVMEDIA_TYPE_AUDIO) &&
		pkt_dts != AV_NOPTS_VALUE && ist->next_dts == AV_NOPTS_VALUE && !copy_ts
		&& (is->iformat->flags & AVFMT_TS_DISCONT) && ifile->last_ts != AV_NOPTS_VALUE) {
		int64_t delta = pkt_dts - ifile->last_ts;
		if (delta < -1LL * dts_delta_threshold*AV_TIME_BASE ||
			delta >  1LL * dts_delta_threshold*AV_TIME_BASE) {
			ifile->ts_offset -= delta;
			av_log(NULL, AV_LOG_DEBUG,
				"Inter stream timestamp discontinuity %" PRId64", new offset= %" PRId64"\n",
				delta, ifile->ts_offset);
			pkt.dts -= av_rescale_q(delta, AV_TIME_BASE_Q, ist->st->time_base);
			if (pkt.pts != AV_NOPTS_VALUE)
				pkt.pts -= av_rescale_q(delta, AV_TIME_BASE_Q, ist->st->time_base);
		}
	}

	duration = av_rescale_q(ifile->duration, ifile->time_base, ist->st->time_base);
	if (pkt.pts != AV_NOPTS_VALUE) {
		pkt.pts += duration;
		ist->max_pts = FFMAX(pkt.pts, ist->max_pts);
		ist->min_pts = FFMIN(pkt.pts, ist->min_pts);
	}

	if (pkt.dts != AV_NOPTS_VALUE)
		pkt.dts += duration;

	pkt_dts = av_rescale_q_rnd(pkt.dts, ist->st->time_base, AV_TIME_BASE_Q, (AVRounding)((int)AV_ROUND_NEAR_INF | (int)AV_ROUND_PASS_MINMAX));
	if ((ist->dec_ctx->codec_type == AVMEDIA_TYPE_VIDEO ||
		ist->dec_ctx->codec_type == AVMEDIA_TYPE_AUDIO) &&
		pkt_dts != AV_NOPTS_VALUE && ist->next_dts != AV_NOPTS_VALUE &&
		!copy_ts) {
		int64_t delta = pkt_dts - ist->next_dts;
		if (is->iformat->flags & AVFMT_TS_DISCONT) {
			if (delta < -1LL * dts_delta_threshold*AV_TIME_BASE ||
				delta >  1LL * dts_delta_threshold*AV_TIME_BASE ||
				pkt_dts + AV_TIME_BASE / 10 < FFMAX(ist->pts, ist->dts)) {
				ifile->ts_offset -= delta;
				av_log(NULL, AV_LOG_DEBUG,
					"timestamp discontinuity %" PRId64", new offset= %" PRId64"\n",
					delta, ifile->ts_offset);
				pkt.dts -= av_rescale_q(delta, AV_TIME_BASE_Q, ist->st->time_base);
				if (pkt.pts != AV_NOPTS_VALUE)
					pkt.pts -= av_rescale_q(delta, AV_TIME_BASE_Q, ist->st->time_base);
			}
		}
		else {
			if (delta < -1LL * dts_error_threshold*AV_TIME_BASE ||
				delta >  1LL * dts_error_threshold*AV_TIME_BASE) {
				av_log(NULL, AV_LOG_WARNING, "DTS %" PRId64", next:%" PRId64" st:%d invalid dropping\n", pkt.dts, ist->next_dts, pkt.stream_index);
				pkt.dts = AV_NOPTS_VALUE;
			}
			if (pkt.pts != AV_NOPTS_VALUE) {
				int64_t pkt_pts = av_rescale_q(pkt.pts, ist->st->time_base, AV_TIME_BASE_Q);
				delta = pkt_pts - ist->next_dts;
				if (delta < -1LL * dts_error_threshold*AV_TIME_BASE ||
					delta >  1LL * dts_error_threshold*AV_TIME_BASE) {
					av_log(NULL, AV_LOG_WARNING, "PTS %" PRId64", next:%" PRId64" invalid dropping st:%d\n", pkt.pts, ist->next_dts, pkt.stream_index);
					pkt.pts = AV_NOPTS_VALUE;
				}
			}
		}
	}

	if (pkt.dts != AV_NOPTS_VALUE)
		ifile->last_ts = av_rescale_q(pkt.dts, ist->st->time_base, AV_TIME_BASE_Q);

	sub2video_heartbeat(ist, pkt.pts);

	process_input_packet(ist, &pkt, 0);

discard_packet:
	av_packet_unref(&pkt);

	return 0;
}

int FfmpegConvert::transcode_from_filter(FilterGraph *graph, InputStream **best_ist)
{
	int i, ret;
	int nb_requests, nb_requests_max = 0;
	InputFilter *ifilter;
	InputStream *ist;

	*best_ist = NULL;
	ret = avfilter_graph_request_oldest(graph->graph);
	if (ret >= 0)
		return reap_filters(0);

	if (ret == AVERROR_EOF) {
		ret = reap_filters(1);
		for (i = 0; i < graph->nb_outputs; i++)
			close_output_stream(graph->outputs[i]->ost);
		return ret;
	}
	if (ret != AVERROR(EAGAIN))
		return ret;

	for (i = 0; i < graph->nb_inputs; i++) {
		ifilter = graph->inputs[i];
		ist = ifilter->ist;
		if (input_files[ist->file_index]->eagain ||
			input_files[ist->file_index]->eof_reached)
			continue;
		nb_requests = av_buffersrc_get_nb_failed_requests(ifilter->filter);
		if (nb_requests > nb_requests_max) {
			nb_requests_max = nb_requests;
			*best_ist = ist;
		}
	}

	if (!*best_ist)
		for (i = 0; i < graph->nb_outputs; i++)
			graph->outputs[i]->ost->unavailable = 1;

	return 0;
}

int FfmpegConvert::transcode_step(void)
{
	OutputStream *ost;
	InputStream  *ist = NULL;
	int ret;

	ost = choose_output();
	if (!ost) {
		if (got_eagain()) {
			reset_eagain();
			av_usleep(10000);
			return 0;
		}
		av_log(NULL, AV_LOG_VERBOSE, "No more inputs to read from, finishing.\n");
		return AVERROR_EOF;
	}

	if (ost->filter && !ost->filter->graph->graph) {
		if (ifilter_has_all_input_formats(ost->filter->graph)) {
			ret = configure_filtergraph(ost->filter->graph);
			if (ret < 0) {
				av_log(NULL, AV_LOG_ERROR, "Error reinitializing filters!\n");
				return ret;
			}
		}
	}

	if (ost->filter && ost->filter->graph->graph) {
		if (!ost->initialized) {
			char error[1024] = { 0 };
			ret = init_output_stream(ost, error, sizeof(error));
			if (ret < 0) {
				av_log(NULL, AV_LOG_ERROR, "Error initializing output stream %d:%d -- %s\n",
					ost->file_index, ost->index, error);
				exit_program(1);
			}
		}
		if ((ret = transcode_from_filter(ost->filter->graph, &ist)) < 0)
			return ret;
		if (!ist)
			return 0;
	}
	else if (ost->filter) {
		int i;
		for (i = 0; i < ost->filter->graph->nb_inputs; i++) {
			InputFilter *ifilter = ost->filter->graph->inputs[i];
			if (!ifilter->ist->got_output && !input_files[ifilter->ist->file_index]->eof_reached) {
				ist = ifilter->ist;
				break;
			}
		}
		if (!ist) {
			ost->inputs_done = 1;
			return 0;
		}
	}
	else {
		av_assert0(ost->source_index >= 0);
		ist = input_streams[ost->source_index];
	}

	ret = process_input(ist->file_index);
	if (ret == AVERROR(EAGAIN)) {
		if (input_files[ist->file_index]->eagain)
			ost->unavailable = 1;
		return 0;
	}

	if (ret < 0)
		return ret == AVERROR_EOF ? 0 : ret;

	return reap_filters(0);
}

int FfmpegConvert::transcode(void)
{
	int ret, i;
	AVFormatContext *os;
	OutputStream *ost;
	InputStream *ist;
	int64_t total_packets_written = 0;

	ret = transcode_init();
	if (ret < 0)
		goto fail;


	while (!received_sigterm) {

		/* check if there's any stream where output is still needed */
		if (!need_output()) {
			av_log(NULL, AV_LOG_VERBOSE, "No more output streams to write to, finishing.\n");
			break;
		}

		ret = transcode_step();

		if (ret < 0 && ret != AVERROR_EOF) {
			break;
		}

		for (int i = 0; i <nb_output_streams; i++) {
			OutputStream *ost = output_streams[i];
			if (av_stream_get_end_pts(ost->st) != AV_NOPTS_VALUE) {
				int64_t pts = INT64_MIN + 1;
				pts = FFMAX(pts, av_rescale_q(av_stream_get_end_pts(ost->st), ost->st->time_base, AV_TIME_BASE_Q));
				m_ptsCurr = pts / 1000;//Curr Time 
			}
		}

	}


	/* at the end of stream, we must flush the decoder buffers */
	for (i = 0; i < nb_input_streams; i++) {
		ist = input_streams[i];
		if (!input_files[ist->file_index]->eof_reached) {
			process_input_packet(ist, NULL, 0);
		}
	}
	flush_encoders();


	/* write the trailer if needed and close file */
	for (i = 0; i < nb_output_files; i++) {
		os = output_files[i]->ctx;
		if (!output_files[i]->header_written) {
			av_log(NULL, AV_LOG_ERROR,
				"Nothing was written into output file %d (%s), because "
				"at least one of its streams received no packets.\n",
				i, os->url);
			continue;
		}
		av_write_trailer(os);
	}

	/* close each encoder */
	for (i = 0; i < nb_output_streams; i++) {
		ost = output_streams[i];
		if (ost->encoding_needed) {
			av_freep(&ost->enc_ctx->stats_in);
		}
		total_packets_written += ost->packets_written;
	}

	/* close each decoder */
	for (i = 0; i < nb_input_streams; i++) {
		ist = input_streams[i];
		if (ist->decoding_needed) {
			avcodec_close(ist->dec_ctx);
			if (ist->hwaccel_uninit)
				ist->hwaccel_uninit(ist->dec_ctx);
		}
	}

	/* finished ! */
	ret = 0;

fail:


	if (output_streams) {
		for (i = 0; i < nb_output_streams; i++) {
			ost = output_streams[i];
			if (ost) {
				if (ost->logfile) {
					fclose(ost->logfile);
					ost->logfile = NULL;
				}
				av_freep(&ost->forced_kf_pts);
				av_freep(&ost->apad);
				av_freep(&ost->disposition);
				av_dict_free(&ost->encoder_opts);
				av_dict_free(&ost->sws_dict);
				av_dict_free(&ost->swr_opts);
				av_dict_free(&ost->resample_opts);
			}
		}
	}
	return ret;
}


static const enum AVPixelFormat mjpeg_formats[] =
{ AV_PIX_FMT_YUVJ420P, AV_PIX_FMT_YUVJ422P, AV_PIX_FMT_YUVJ444P,
AV_PIX_FMT_YUV420P,  AV_PIX_FMT_YUV422P,  AV_PIX_FMT_YUV444P,
AV_PIX_FMT_NONE };
static const enum AVPixelFormat ljpeg_formats[] =
{ AV_PIX_FMT_BGR24   , AV_PIX_FMT_BGRA    , AV_PIX_FMT_BGR0,
AV_PIX_FMT_YUVJ420P, AV_PIX_FMT_YUVJ444P, AV_PIX_FMT_YUVJ422P,
AV_PIX_FMT_YUV420P , AV_PIX_FMT_YUV444P , AV_PIX_FMT_YUV422P,
AV_PIX_FMT_NONE };

const enum AVPixelFormat* FfmpegConvert::get_compliance_unofficial_pix_fmts(enum AVCodecID codec_id, const enum AVPixelFormat default_formats[])
{
	if (codec_id == AV_CODEC_ID_MJPEG) {
		return mjpeg_formats;
	}
	else if (codec_id == AV_CODEC_ID_LJPEG) {
		return ljpeg_formats;
	}
	else {
		return default_formats;
	}
}

enum AVPixelFormat FfmpegConvert::choose_pixel_fmt(AVStream *st, AVCodecContext *enc_ctx, AVCodec *codec, enum AVPixelFormat target)
{
	if (codec && codec->pix_fmts) {
		const enum AVPixelFormat *p = codec->pix_fmts;
		const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(target);
		int has_alpha = desc ? desc->nb_components % 2 == 0 : 0;
		enum AVPixelFormat best = AV_PIX_FMT_NONE;

		if (enc_ctx->strict_std_compliance <= FF_COMPLIANCE_UNOFFICIAL) {
			p = get_compliance_unofficial_pix_fmts(enc_ctx->codec_id, p);
		}
		for (; *p != AV_PIX_FMT_NONE; p++) {
			best = avcodec_find_best_pix_fmt_of_2(best, *p, target, has_alpha, NULL);
			if (*p == target)
				break;
		}
		if (*p == AV_PIX_FMT_NONE) {
			return best;
		}
	}
	return target;
}

void FfmpegConvert::choose_sample_fmt(AVStream *st, AVCodec *codec)
{
	if (codec && codec->sample_fmts) {
		const enum AVSampleFormat *p = codec->sample_fmts;
		for (; *p != -1; p++) {
			if (*p == st->codecpar->format)
				break;
		}
		if (*p == -1) {
			if ((codec->capabilities & AV_CODEC_CAP_LOSSLESS) && av_get_sample_fmt_name((AVSampleFormat)st->codecpar->format) > av_get_sample_fmt_name(codec->sample_fmts[0]))
				av_log(NULL, AV_LOG_ERROR, "Conversion will not be lossless.\n");
			if (av_get_sample_fmt_name((AVSampleFormat)st->codecpar->format))
				av_log(NULL, AV_LOG_WARNING,
					"Incompatible sample format '%s' for codec '%s', auto-selecting format '%s'\n",
					av_get_sample_fmt_name((AVSampleFormat)st->codecpar->format),
					codec->name,
					av_get_sample_fmt_name(codec->sample_fmts[0]));
			st->codecpar->format = codec->sample_fmts[0];
		}
	}
}

char* FfmpegConvert::choose_pix_fmts(OutputFilter *ofilter)
{
	OutputStream *ost = ofilter->ost;
	AVDictionaryEntry *strict_dict = av_dict_get(ost->encoder_opts, "strict", NULL, 0);
	if (strict_dict)
		// used by choose_pixel_fmt() and below
		av_opt_set(ost->enc_ctx, "strict", strict_dict->value, 0);

	if (ost->keep_pix_fmt) {
		avfilter_graph_set_auto_convert(ofilter->graph->graph,
			AVFILTER_AUTO_CONVERT_NONE);
		if (ost->enc_ctx->pix_fmt == AV_PIX_FMT_NONE)
			return NULL;
		return av_strdup(av_get_pix_fmt_name(ost->enc_ctx->pix_fmt));
	}
	if (ost->enc_ctx->pix_fmt != AV_PIX_FMT_NONE) {
		return av_strdup(av_get_pix_fmt_name(choose_pixel_fmt(ost->st, ost->enc_ctx, ost->enc, ost->enc_ctx->pix_fmt)));
	}
	else if (ost->enc && ost->enc->pix_fmts) {
		const enum AVPixelFormat *p;
		AVIOContext *s = NULL;
		uint8_t *ret;
		int len;

		if (avio_open_dyn_buf(&s) < 0)
			exit_program(1);

		p = ost->enc->pix_fmts;
		if (ost->enc_ctx->strict_std_compliance <= FF_COMPLIANCE_UNOFFICIAL) {
			p = get_compliance_unofficial_pix_fmts(ost->enc_ctx->codec_id, p);
		}

		for (; *p != AV_PIX_FMT_NONE; p++) {
			const char *name = av_get_pix_fmt_name(*p);
			avio_printf(s, "%s|", name);
		}
		len = avio_close_dyn_buf(s, &ret);
		ret[len - 1] = 0;
		return (char *)ret;
	}
	else
		return NULL;
}

char *FfmpegConvert::choose_sample_fmts(OutputFilter *ofilter) {
	if (ofilter->format != AV_SAMPLE_FMT_NONE) {
		const char *name = av_get_sample_fmt_name((AVSampleFormat)ofilter->format);
		return av_strdup(name);
	}
	else if (ofilter->formats) {
		const enum AVSampleFormat *p;
		AVIOContext *s = NULL;
		uint8_t *ret;
		int len;
		if (avio_open_dyn_buf(&s) < 0)
			exit_program(1);
		for (p = (const AVSampleFormat *)ofilter->formats; *p != AV_SAMPLE_FMT_NONE; p++) {
			const char *name = av_get_sample_fmt_name(*p);
			avio_printf(s, "%s|", name);
		}
		len = avio_close_dyn_buf(s, &ret);
		ret[len - 1] = 0;
		return (char *)ret;
	}
	else
		return NULL;
}

char *FfmpegConvert::choose_sample_rates(OutputFilter *ofilter) {
	if (ofilter->sample_rate != 0) {
		char name[16];
		snprintf(name, sizeof(name), "%d", ofilter->sample_rate);
		return av_strdup(name);
	}
	else if (ofilter->sample_rates) {
		const int *p;
		AVIOContext *s = NULL;
		uint8_t *ret;
		int len;
		if (avio_open_dyn_buf(&s) < 0)
			exit_program(1);
		for (p = ofilter->sample_rates; *p != 0; p++) {
			char name[16];
			snprintf(name, sizeof(name), "%d", *p);
			avio_printf(s, "%s|", name);
		}
		len = avio_close_dyn_buf(s, &ret);
		ret[len - 1] = 0;
		return (char *)ret;
	}
	else
		return NULL;
}

char *FfmpegConvert::choose_channel_layouts(OutputFilter *ofilter) {
	if (ofilter->channel_layout != 0) {
		char name[16];
		snprintf(name, sizeof(name), "0x%" PRIx64, ofilter->channel_layout);
		return av_strdup(name);
	}
	else if (ofilter->channel_layouts) {
		const uint64_t *p;
		AVIOContext *s = NULL;
		uint8_t *ret;
		int len;
		if (avio_open_dyn_buf(&s) < 0)
			exit_program(1);
		for (p = ofilter->channel_layouts; *p != 0; p++) {
			char name[16];
			snprintf(name, sizeof(name), "0x%" PRIx64, *p);
			avio_printf(s, "%s|", name);
		}
		len = avio_close_dyn_buf(s, &ret);
		ret[len - 1] = 0;
		return (char *)ret;
	}
	else
		return NULL;
}

int FfmpegConvert::init_simple_filtergraph(InputStream *ist, OutputStream *ost)
{
	FilterGraph *fg = (FilterGraph *)av_mallocz(sizeof(*fg));

	fg->index = nb_filtergraphs;

	GROW_ARRAY(fg->outputs, fg->nb_outputs, OutputFilter**);
	fg->outputs[0] = (OutputFilter*)av_mallocz(sizeof(*fg->outputs[0]));
	fg->outputs[0]->ost = ost;
	fg->outputs[0]->graph = fg;
	fg->outputs[0]->format = -1;

	ost->filter = fg->outputs[0];

	GROW_ARRAY(fg->inputs, fg->nb_inputs, InputFilter**);
	fg->inputs[0] = (InputFilter *)av_mallocz(sizeof(*fg->inputs[0]));
	fg->inputs[0]->ist = ist;
	fg->inputs[0]->graph = fg;
	fg->inputs[0]->format = -1;

	fg->inputs[0]->frame_queue = av_fifo_alloc(8 * sizeof(AVFrame*));
	if (!fg->inputs[0]->frame_queue)
		exit_program(1);

	GROW_ARRAY(ist->filters, ist->nb_filters, InputFilter**);
	ist->filters[ist->nb_filters - 1] = fg->inputs[0];

	GROW_ARRAY(filtergraphs, nb_filtergraphs, FilterGraph**);
	filtergraphs[nb_filtergraphs - 1] = fg;

	return 0;
}

char *FfmpegConvert::describe_filter_link(FilterGraph *fg, AVFilterInOut *inout, int in)
{
	AVFilterContext *ctx = inout->filter_ctx;
	AVFilterPad *pads = in ? ctx->input_pads : ctx->output_pads;
	int       nb_pads = in ? ctx->nb_inputs : ctx->nb_outputs;
	AVIOContext *pb;
	uint8_t *res = NULL;

	if (avio_open_dyn_buf(&pb) < 0)
		exit_program(1);

	avio_printf(pb, "%s", ctx->filter->name);
	if (nb_pads > 1)
		avio_printf(pb, ":%s", avfilter_pad_get_name(pads, inout->pad_idx));
	avio_w8(pb, 0);
	avio_close_dyn_buf(pb, &res);
	return (char*)res;
}

void FfmpegConvert::init_input_filter(FilterGraph *fg, AVFilterInOut *in)
{
	InputStream *ist = NULL;
	enum AVMediaType type = avfilter_pad_get_type(in->filter_ctx->input_pads, in->pad_idx);
	int i;

	// TODO: support other filter types
	if (type != AVMEDIA_TYPE_VIDEO && type != AVMEDIA_TYPE_AUDIO) {
		av_log(NULL, AV_LOG_FATAL, "Only video and audio filters supported "
			"currently.\n");
		exit_program(1);
	}

	if (in->name) {
		AVFormatContext *s;
		AVStream       *st = NULL;
		char *p;
		int file_idx = strtol(in->name, &p, 0);

		if (file_idx < 0 || file_idx >= nb_input_files) {
			av_log(NULL, AV_LOG_FATAL, "Invalid file index %d in filtergraph description %s.\n",
				file_idx, fg->graph_desc);
			exit_program(1);
		}
		s = input_files[file_idx]->ctx;

		for (i = 0; i < s->nb_streams; i++) {
			enum AVMediaType stream_type = s->streams[i]->codecpar->codec_type;
			if (stream_type != type &&
				!(stream_type == AVMEDIA_TYPE_SUBTITLE &&
					type == AVMEDIA_TYPE_VIDEO /* sub2video hack */))
				continue;
			if (check_stream_specifier(s, s->streams[i], *p == ':' ? p + 1 : p) == 1) {
				st = s->streams[i];
				break;
			}
		}
		if (!st) {
			av_log(NULL, AV_LOG_FATAL, "Stream specifier '%s' in filtergraph description %s "
				"matches no streams.\n", p, fg->graph_desc);
			exit_program(1);
		}
		ist = input_streams[input_files[file_idx]->ist_index + st->index];
	}
	else {
		/* find the first unused stream of corresponding type */
		for (i = 0; i < nb_input_streams; i++) {
			ist = input_streams[i];
			if (ist->dec_ctx->codec_type == type && ist->discard)
				break;
		}
		if (i == nb_input_streams) {
			av_log(NULL, AV_LOG_FATAL, "Cannot find a matching stream for "
				"unlabeled input pad %d on filter %s\n", in->pad_idx,
				in->filter_ctx->name);
			exit_program(1);
		}
	}
	av_assert0(ist);

	ist->discard = 0;
	ist->decoding_needed |= DECODING_FOR_FILTER;
	ist->st->discard = AVDISCARD_NONE;

	GROW_ARRAY(fg->inputs, fg->nb_inputs, InputFilter**);
	fg->inputs[fg->nb_inputs - 1] = (InputFilter*)av_mallocz(sizeof(*fg->inputs[0]));
	fg->inputs[fg->nb_inputs - 1]->ist = ist;
	fg->inputs[fg->nb_inputs - 1]->graph = fg;
	fg->inputs[fg->nb_inputs - 1]->format = -1;
	fg->inputs[fg->nb_inputs - 1]->type = ist->st->codecpar->codec_type;
	fg->inputs[fg->nb_inputs - 1]->name = (uint8_t*)describe_filter_link(fg, in, 1);

	fg->inputs[fg->nb_inputs - 1]->frame_queue = av_fifo_alloc(8 * sizeof(AVFrame*));
	if (!fg->inputs[fg->nb_inputs - 1]->frame_queue)
		exit_program(1);

	GROW_ARRAY(ist->filters, ist->nb_filters, InputFilter**);
	ist->filters[ist->nb_filters - 1] = fg->inputs[fg->nb_inputs - 1];
}

int FfmpegConvert::init_complex_filtergraph(FilterGraph *fg)
{
	AVFilterInOut *inputs, *outputs, *cur;
	AVFilterGraph *graph;
	int ret = 0;

	/* this graph is only used for determining the kinds of inputs
	* and outputs we have, and is discarded on exit from this function */
	graph = avfilter_graph_alloc();
	if (!graph)
		return AVERROR(ENOMEM);
	graph->nb_threads = 1;

	ret = avfilter_graph_parse2(graph, fg->graph_desc, &inputs, &outputs);
	if (ret < 0)
		goto fail;

	for (cur = inputs; cur; cur = cur->next)
		init_input_filter(fg, cur);

	for (cur = outputs; cur;) {
		GROW_ARRAY(fg->outputs, fg->nb_outputs, OutputFilter**);
		fg->outputs[fg->nb_outputs - 1] = (OutputFilter*)av_mallocz(sizeof(*fg->outputs[0]));

		fg->outputs[fg->nb_outputs - 1]->graph = fg;
		fg->outputs[fg->nb_outputs - 1]->out_tmp = cur;
		fg->outputs[fg->nb_outputs - 1]->type = avfilter_pad_get_type(cur->filter_ctx->output_pads,
			cur->pad_idx);
		fg->outputs[fg->nb_outputs - 1]->name = (uint8_t*)describe_filter_link(fg, cur, 0);
		cur = cur->next;
		fg->outputs[fg->nb_outputs - 1]->out_tmp->next = NULL;
	}

fail:
	avfilter_inout_free(&inputs);
	avfilter_graph_free(&graph);
	return ret;
}

int FfmpegConvert::insert_trim(int64_t start_time, int64_t duration,
	AVFilterContext **last_filter, int *pad_idx,
	const char *filter_name)
{
	AVFilterGraph *graph = (*last_filter)->graph;
	AVFilterContext *ctx;
	const AVFilter *trim;
	enum AVMediaType type = avfilter_pad_get_type((*last_filter)->output_pads, *pad_idx);
	const char *name = (type == AVMEDIA_TYPE_VIDEO) ? "trim" : "atrim";
	int ret = 0;

	if (duration == INT64_MAX && start_time == AV_NOPTS_VALUE)
		return 0;

	trim = avfilter_get_by_name(name);
	if (!trim) {
		av_log(NULL, AV_LOG_ERROR, "%s filter not present, cannot limit "
			"recording time.\n", name);
		return AVERROR_FILTER_NOT_FOUND;
	}

	ctx = avfilter_graph_alloc_filter(graph, trim, filter_name);
	if (!ctx)
		return AVERROR(ENOMEM);

	if (duration != INT64_MAX) {
		ret = av_opt_set_int(ctx, "durationi", duration,
			AV_OPT_SEARCH_CHILDREN);
	}
	if (ret >= 0 && start_time != AV_NOPTS_VALUE) {
		ret = av_opt_set_int(ctx, "starti", start_time,
			AV_OPT_SEARCH_CHILDREN);
	}
	if (ret < 0) {
		av_log(ctx, AV_LOG_ERROR, "Error configuring the %s filter", name);
		return ret;
	}

	ret = avfilter_init_str(ctx, NULL);
	if (ret < 0)
		return ret;

	ret = avfilter_link(*last_filter, *pad_idx, ctx, 0);
	if (ret < 0)
		return ret;

	*last_filter = ctx;
	*pad_idx = 0;
	return 0;
}

int FfmpegConvert::insert_filter(AVFilterContext **last_filter, int *pad_idx,
	const char *filter_name, const char *args)
{
	AVFilterGraph *graph = (*last_filter)->graph;
	AVFilterContext *ctx;
	int ret;

	ret = avfilter_graph_create_filter(&ctx,
		avfilter_get_by_name(filter_name),
		filter_name, args, NULL, graph);
	if (ret < 0)
		return ret;

	ret = avfilter_link(*last_filter, *pad_idx, ctx, 0);
	if (ret < 0)
		return ret;

	*last_filter = ctx;
	*pad_idx = 0;
	return 0;
}

int FfmpegConvert::configure_output_video_filter(FilterGraph *fg, OutputFilter *ofilter, AVFilterInOut *out)
{
	char *pix_fmts;
	OutputStream *ost = ofilter->ost;
	OutputFile    *of = output_files[ost->file_index];
	AVFilterContext *last_filter = out->filter_ctx;
	int pad_idx = out->pad_idx;
	int ret;
	char name[255];

	snprintf(name, sizeof(name), "out_%d_%d", ost->file_index, ost->index);
	ret = avfilter_graph_create_filter(&ofilter->filter,
		avfilter_get_by_name("buffersink"),
		name, NULL, NULL, fg->graph);

	if (ret < 0)
		return ret;

	if (ofilter->width || ofilter->height) {
		char args[255];
		AVFilterContext *filter;
		AVDictionaryEntry *e = NULL;

		snprintf(args, sizeof(args), "%d:%d",
			ofilter->width, ofilter->height);

		while ((e = av_dict_get(ost->sws_dict, "", e,
			AV_DICT_IGNORE_SUFFIX))) {
			av_strlcatf(args, sizeof(args), ":%s=%s", e->key, e->value);
		}

		snprintf(name, sizeof(name), "scaler_out_%d_%d",
			ost->file_index, ost->index);
		if ((ret = avfilter_graph_create_filter(&filter, avfilter_get_by_name("scale"),
			name, args, NULL, fg->graph)) < 0)
			return ret;
		if ((ret = avfilter_link(last_filter, pad_idx, filter, 0)) < 0)
			return ret;

		last_filter = filter;
		pad_idx = 0;
	}

	if ((pix_fmts = choose_pix_fmts(ofilter))) {
		AVFilterContext *filter;
		snprintf(name, sizeof(name), "format_out_%d_%d",
			ost->file_index, ost->index);
		ret = avfilter_graph_create_filter(&filter,
			avfilter_get_by_name("format"),
			"format", pix_fmts, NULL, fg->graph);
		av_freep(&pix_fmts);
		if (ret < 0)
			return ret;
		if ((ret = avfilter_link(last_filter, pad_idx, filter, 0)) < 0)
			return ret;

		last_filter = filter;
		pad_idx = 0;
	}

	if (ost->frame_rate.num && 0) {
		AVFilterContext *fps;
		char args[255];

		snprintf(args, sizeof(args), "fps=%d/%d", ost->frame_rate.num,
			ost->frame_rate.den);
		snprintf(name, sizeof(name), "fps_out_%d_%d",
			ost->file_index, ost->index);
		ret = avfilter_graph_create_filter(&fps, avfilter_get_by_name("fps"),
			name, args, NULL, fg->graph);
		if (ret < 0)
			return ret;

		ret = avfilter_link(last_filter, pad_idx, fps, 0);
		if (ret < 0)
			return ret;
		last_filter = fps;
		pad_idx = 0;
	}

	snprintf(name, sizeof(name), "trim_out_%d_%d",
		ost->file_index, ost->index);
	ret = insert_trim(of->start_time, of->recording_time,
		&last_filter, &pad_idx, name);
	if (ret < 0)
		return ret;


	if ((ret = avfilter_link(last_filter, pad_idx, ofilter->filter, 0)) < 0)
		return ret;

	return 0;
}

int FfmpegConvert::configure_output_audio_filter(FilterGraph *fg, OutputFilter *ofilter, AVFilterInOut *out)
{
	OutputStream *ost = ofilter->ost;
	OutputFile    *of = output_files[ost->file_index];
	AVCodecContext *codec = ost->enc_ctx;
	AVFilterContext *last_filter = out->filter_ctx;
	int pad_idx = out->pad_idx;
	char *sample_fmts, *sample_rates, *channel_layouts;
	char name[255];
	int ret;

	snprintf(name, sizeof(name), "out_%d_%d", ost->file_index, ost->index);
	ret = avfilter_graph_create_filter(&ofilter->filter,
		avfilter_get_by_name("abuffersink"),
		name, NULL, NULL, fg->graph);
	if (ret < 0)
		return ret;
	if ((ret = av_opt_set_int(ofilter->filter, "all_channel_counts", 1, AV_OPT_SEARCH_CHILDREN)) < 0)
		return ret;

#define AUTO_INSERT_FILTER(opt_name, filter_name, arg) do {                 \
AVFilterContext *filt_ctx;                                              \
                                                                        \
av_log(NULL, AV_LOG_INFO, opt_name " is forwarded to lavfi "            \
        "similarly to -af " filter_name "=%s.\n", arg);                  \
                                                                        \
ret = avfilter_graph_create_filter(&filt_ctx,                           \
                                    avfilter_get_by_name(filter_name),   \
                                    filter_name, arg, NULL, fg->graph);  \
if (ret < 0)                                                            \
    return ret;                                                         \
                                                                        \
ret = avfilter_link(last_filter, pad_idx, filt_ctx, 0);                 \
if (ret < 0)                                                            \
    return ret;                                                         \
                                                                        \
last_filter = filt_ctx;                                                 \
pad_idx = 0;                                                            \
} while (0)
	if (ost->audio_channels_mapped) {
		int i;
		AVBPrint pan_buf;
		av_bprint_init(&pan_buf, 256, 8192);
		av_bprintf(&pan_buf, "0x%" PRIx64,
			av_get_default_channel_layout(ost->audio_channels_mapped));
		for (i = 0; i < ost->audio_channels_mapped; i++)
			if (ost->audio_channels_map[i] != -1)
				av_bprintf(&pan_buf, "|c%d=c%d", i, ost->audio_channels_map[i]);

		AUTO_INSERT_FILTER("-map_channel", "pan", pan_buf.str);
		av_bprint_finalize(&pan_buf, NULL);
	}

	if (codec->channels && !codec->channel_layout)
		codec->channel_layout = av_get_default_channel_layout(codec->channels);

	sample_fmts = choose_sample_fmts(ofilter);
	sample_rates = choose_sample_rates(ofilter);
	channel_layouts = choose_channel_layouts(ofilter);
	if (sample_fmts || sample_rates || channel_layouts) {
		AVFilterContext *format;
		char args[256];
		args[0] = 0;

		if (sample_fmts)
			av_strlcatf(args, sizeof(args), "sample_fmts=%s:",
				sample_fmts);
		if (sample_rates)
			av_strlcatf(args, sizeof(args), "sample_rates=%s:",
				sample_rates);
		if (channel_layouts)
			av_strlcatf(args, sizeof(args), "channel_layouts=%s:",
				channel_layouts);

		av_freep(&sample_fmts);
		av_freep(&sample_rates);
		av_freep(&channel_layouts);

		snprintf(name, sizeof(name), "format_out_%d_%d",
			ost->file_index, ost->index);
		ret = avfilter_graph_create_filter(&format,
			avfilter_get_by_name("aformat"),
			name, args, NULL, fg->graph);
		if (ret < 0)
			return ret;

		ret = avfilter_link(last_filter, pad_idx, format, 0);
		if (ret < 0)
			return ret;

		last_filter = format;
		pad_idx = 0;
	}

	if (audio_volume != 256 && 0) {
		char args[256];

		snprintf(args, sizeof(args), "%f", audio_volume / 256.);
		AUTO_INSERT_FILTER("-vol", "volume", args);
	}

	if (ost->apad && of->shortest) {
		char args[256];
		int i;

		for (i = 0; i<of->ctx->nb_streams; i++)
			if (of->ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
				break;

		if (i<of->ctx->nb_streams) {
			snprintf(args, sizeof(args), "%s", ost->apad);
			AUTO_INSERT_FILTER("-apad", "apad", args);
		}
	}

	snprintf(name, sizeof(name), "trim for output stream %d:%d",
		ost->file_index, ost->index);
	ret = insert_trim(of->start_time, of->recording_time,
		&last_filter, &pad_idx, name);
	if (ret < 0)
		return ret;

	if ((ret = avfilter_link(last_filter, pad_idx, ofilter->filter, 0)) < 0)
		return ret;

	return 0;
}

int FfmpegConvert::configure_output_filter(FilterGraph *fg, OutputFilter *ofilter, AVFilterInOut *out)
{
	if (!ofilter->ost) {
		av_log(NULL, AV_LOG_FATAL, "Filter %s has an unconnected output\n", ofilter->name);
		exit_program(1);
	}

	switch (avfilter_pad_get_type(out->filter_ctx->output_pads, out->pad_idx)) {
	case AVMEDIA_TYPE_VIDEO: return configure_output_video_filter(fg, ofilter, out);
	case AVMEDIA_TYPE_AUDIO: return configure_output_audio_filter(fg, ofilter, out);
	default: av_assert0(0);
	}
}

void FfmpegConvert::check_filter_outputs(void)
{
	int i;
	for (i = 0; i < nb_filtergraphs; i++) {
		int n;
		for (n = 0; n < filtergraphs[i]->nb_outputs; n++) {
			OutputFilter *output = filtergraphs[i]->outputs[n];
			if (!output->ost) {
				av_log(NULL, AV_LOG_FATAL, "Filter %s has an unconnected output\n", output->name);
				exit_program(1);
			}
		}
	}
}

int FfmpegConvert::sub2video_prepare(InputStream *ist, InputFilter *ifilter)
{
	AVFormatContext *avf = input_files[ist->file_index]->ctx;
	int i, w, h;

	/* Compute the size of the canvas for the subtitles stream.
	If the subtitles codecpar has set a size, use it. Otherwise use the
	maximum dimensions of the video streams in the same file. */
	w = ifilter->width;
	h = ifilter->height;
	if (!(w && h)) {
		for (i = 0; i < avf->nb_streams; i++) {
			if (avf->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
				w = FFMAX(w, avf->streams[i]->codecpar->width);
				h = FFMAX(h, avf->streams[i]->codecpar->height);
			}
		}
		if (!(w && h)) {
			w = FFMAX(w, 720);
			h = FFMAX(h, 576);
		}
		av_log(avf, AV_LOG_INFO, "sub2video: using %dx%d canvas\n", w, h);
	}
	ist->sub2video.w = ifilter->width = w;
	ist->sub2video.h = ifilter->height = h;

	ifilter->width = ist->dec_ctx->width ? ist->dec_ctx->width : ist->sub2video.w;
	ifilter->height = ist->dec_ctx->height ? ist->dec_ctx->height : ist->sub2video.h;

	/* rectangles are AV_PIX_FMT_PAL8, but we have no guarantee that the
	palettes for all rectangles are identical or compatible */
	ifilter->format = AV_PIX_FMT_RGB32;

	ist->sub2video.frame = av_frame_alloc();
	if (!ist->sub2video.frame)
		return AVERROR(ENOMEM);
	ist->sub2video.last_pts = INT64_MIN;
	return 0;
}

int FfmpegConvert::configure_input_video_filter(FilterGraph *fg, InputFilter *ifilter,
	AVFilterInOut *in)
{
	AVFilterContext *last_filter;
	const AVFilter *buffer_filt = avfilter_get_by_name("buffer");
	InputStream *ist = ifilter->ist;
	InputFile     *f = input_files[ist->file_index];
	AVRational tb = ist->framerate.num ? av_inv_q(ist->framerate) :
		ist->st->time_base;
	AVRational fr = ist->framerate;
	AVRational sar;
	AVBPrint args;
	char name[255];
	int ret, pad_idx = 0;
	int64_t tsoffset = 0;
	AVBufferSrcParameters *par = av_buffersrc_parameters_alloc();

	if (!par)
		return AVERROR(ENOMEM);
	memset(par, 0, sizeof(*par));
	par->format = AV_PIX_FMT_NONE;

	if (ist->dec_ctx->codec_type == AVMEDIA_TYPE_AUDIO) {
		av_log(NULL, AV_LOG_ERROR, "Cannot connect video filter to audio input\n");
		ret = AVERROR(EINVAL);
		goto fail;
	}

	if (!fr.num)
		fr = av_guess_frame_rate(input_files[ist->file_index]->ctx, ist->st, NULL);

	if (ist->dec_ctx->codec_type == AVMEDIA_TYPE_SUBTITLE) {
		ret = sub2video_prepare(ist, ifilter);
		if (ret < 0)
			goto fail;
	}

	sar = ifilter->sample_aspect_ratio;
	if (!sar.den)
		sar = AV_TIME_BASE_0_1;
	av_bprint_init(&args, 0, 1);
	av_bprintf(&args,
		"video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:"
		"pixel_aspect=%d/%d:sws_param=flags=%d",
		ifilter->width, ifilter->height, ifilter->format,
		tb.num, tb.den, sar.num, sar.den,
		SWS_BILINEAR + ((ist->dec_ctx->flags&AV_CODEC_FLAG_BITEXACT) ? SWS_BITEXACT : 0));
	if (fr.num && fr.den)
		av_bprintf(&args, ":frame_rate=%d/%d", fr.num, fr.den);
	snprintf(name, sizeof(name), "graph %d input from stream %d:%d", fg->index,
		ist->file_index, ist->st->index);


	if ((ret = avfilter_graph_create_filter(&ifilter->filter, buffer_filt, name,
		args.str, NULL, fg->graph)) < 0)
		goto fail;
	par->hw_frames_ctx = ifilter->hw_frames_ctx;
	ret = av_buffersrc_parameters_set(ifilter->filter, par);
	if (ret < 0)
		goto fail;
	av_freep(&par);
	last_filter = ifilter->filter;

	if (ist->autorotate) {
		double theta = get_rotation(ist->st);

		if (fabs(theta - 90) < 1.0) {
			ret = insert_filter(&last_filter, &pad_idx, "transpose", "clock");
		}
		else if (fabs(theta - 180) < 1.0) {
			ret = insert_filter(&last_filter, &pad_idx, "hflip", NULL);
			if (ret < 0)
				return ret;
			ret = insert_filter(&last_filter, &pad_idx, "vflip", NULL);
		}
		else if (fabs(theta - 270) < 1.0) {
			ret = insert_filter(&last_filter, &pad_idx, "transpose", "cclock");
		}
		else if (fabs(theta) > 1.0) {
			char rotate_buf[64];
			snprintf(rotate_buf, sizeof(rotate_buf), "%f*PI/180", theta);
			ret = insert_filter(&last_filter, &pad_idx, "rotate", rotate_buf);
		}
		if (ret < 0)
			return ret;
	}

	if (do_deinterlace) {
		AVFilterContext *yadif;

		snprintf(name, sizeof(name), "deinterlace_in_%d_%d",
			ist->file_index, ist->st->index);
		if ((ret = avfilter_graph_create_filter(&yadif,
			avfilter_get_by_name("yadif"),
			name, "", NULL,
			fg->graph)) < 0)
			return ret;

		if ((ret = avfilter_link(last_filter, 0, yadif, 0)) < 0)
			return ret;

		last_filter = yadif;
	}

	snprintf(name, sizeof(name), "trim_in_%d_%d",
		ist->file_index, ist->st->index);
	if (copy_ts) {
		tsoffset = f->start_time == AV_NOPTS_VALUE ? 0 : f->start_time;
		if (!start_at_zero && f->ctx->start_time != AV_NOPTS_VALUE)
			tsoffset += f->ctx->start_time;
	}
	ret = insert_trim(((f->start_time == AV_NOPTS_VALUE) || !f->accurate_seek) ?
		AV_NOPTS_VALUE : tsoffset, f->recording_time,
		&last_filter, &pad_idx, name);
	if (ret < 0)
		return ret;

	if ((ret = avfilter_link(last_filter, 0, in->filter_ctx, in->pad_idx)) < 0)
		return ret;
	return 0;
fail:
	av_freep(&par);

	return ret;
}

int FfmpegConvert::configure_input_audio_filter(FilterGraph *fg, InputFilter *ifilter,
	AVFilterInOut *in)
{
	AVFilterContext *last_filter;
	const AVFilter *abuffer_filt = avfilter_get_by_name("abuffer");
	InputStream *ist = ifilter->ist;
	InputFile     *f = input_files[ist->file_index];
	AVBPrint args;
	char name[255];
	int ret, pad_idx = 0;
	int64_t tsoffset = 0;

	if (ist->dec_ctx->codec_type != AVMEDIA_TYPE_AUDIO) {
		av_log(NULL, AV_LOG_ERROR, "Cannot connect audio filter to non audio input\n");
		return AVERROR(EINVAL);
	}

	av_bprint_init(&args, 0, AV_BPRINT_SIZE_AUTOMATIC);
	av_bprintf(&args, "time_base=%d/%d:sample_rate=%d:sample_fmt=%s",
		1, ifilter->sample_rate,
		ifilter->sample_rate,
		av_get_sample_fmt_name((AVSampleFormat)ifilter->format));
	if (ifilter->channel_layout)
		av_bprintf(&args, ":channel_layout=0x%" PRIx64,
			ifilter->channel_layout);
	else
		av_bprintf(&args, ":channels=%d", ifilter->channels);
	snprintf(name, sizeof(name), "graph_%d_in_%d_%d", fg->index,
		ist->file_index, ist->st->index);

	if ((ret = avfilter_graph_create_filter(&ifilter->filter, abuffer_filt,
		name, args.str, NULL,
		fg->graph)) < 0)
		return ret;
	last_filter = ifilter->filter;

#define AUTO_INSERT_FILTER_INPUT(opt_name, filter_name, arg) do {                 \
AVFilterContext *filt_ctx;                                              \
                                                                        \
av_log(NULL, AV_LOG_INFO, opt_name " is forwarded to lavfi "            \
        "similarly to -af " filter_name "=%s.\n", arg);                  \
                                                                        \
snprintf(name, sizeof(name), "graph_%d_%s_in_%d_%d",      \
            fg->index, filter_name, ist->file_index, ist->st->index);   \
ret = avfilter_graph_create_filter(&filt_ctx,                           \
                                    avfilter_get_by_name(filter_name),   \
                                    name, arg, NULL, fg->graph);         \
if (ret < 0)                                                            \
    return ret;                                                         \
                                                                        \
ret = avfilter_link(last_filter, 0, filt_ctx, 0);                       \
if (ret < 0)                                                            \
    return ret;                                                         \
                                                                        \
last_filter = filt_ctx;                                                 \
} while (0)

	if (audio_sync_method > 0) {
		char args[256] = { 0 };

		av_strlcatf(args, sizeof(args), "async=%d", audio_sync_method);
		if (audio_drift_threshold != 0.1)
			av_strlcatf(args, sizeof(args), ":min_hard_comp=%f", audio_drift_threshold);
		if (!fg->reconfiguration)
			av_strlcatf(args, sizeof(args), ":first_pts=0");
		AUTO_INSERT_FILTER_INPUT("-async", "aresample", args);
	}

	//     if (ost->audio_channels_mapped) {
	//         int i;
	//         AVBPrint pan_buf;
	//         av_bprint_init(&pan_buf, 256, 8192);
	//         av_bprintf(&pan_buf, "0x%" PRIx64,
	//                    av_get_default_channel_layout(ost->audio_channels_mapped));
	//         for (i = 0; i < ost->audio_channels_mapped; i++)
	//             if (ost->audio_channels_map[i] != -1)
	//                 av_bprintf(&pan_buf, ":c%d=c%d", i, ost->audio_channels_map[i]);
	//         AUTO_INSERT_FILTER_INPUT("-map_channel", "pan", pan_buf.str);
	//         av_bprint_finalize(&pan_buf, NULL);
	//     }

	if (audio_volume != 256) {
		char args[256];

		av_log(NULL, AV_LOG_WARNING, "-vol has been deprecated. Use the volume "
			"audio filter instead.\n");

		snprintf(args, sizeof(args), "%f", audio_volume / 256.);
		AUTO_INSERT_FILTER_INPUT("-vol", "volume", args);
	}

	snprintf(name, sizeof(name), "trim for input stream %d:%d",
		ist->file_index, ist->st->index);
	if (copy_ts) {
		tsoffset = f->start_time == AV_NOPTS_VALUE ? 0 : f->start_time;
		if (!start_at_zero && f->ctx->start_time != AV_NOPTS_VALUE)
			tsoffset += f->ctx->start_time;
	}
	ret = insert_trim(((f->start_time == AV_NOPTS_VALUE) || !f->accurate_seek) ?
		AV_NOPTS_VALUE : tsoffset, f->recording_time,
		&last_filter, &pad_idx, name);
	if (ret < 0)
		return ret;

	if ((ret = avfilter_link(last_filter, 0, in->filter_ctx, in->pad_idx)) < 0)
		return ret;

	return 0;
}

int FfmpegConvert::configure_input_filter(FilterGraph *fg, InputFilter *ifilter,
	AVFilterInOut *in)
{
	if (!ifilter->ist->dec) {
		av_log(NULL, AV_LOG_ERROR,
			"No decoder for stream #%d:%d, filtering impossible\n",
			ifilter->ist->file_index, ifilter->ist->st->index);
		return AVERROR_DECODER_NOT_FOUND;
	}
	switch (avfilter_pad_get_type(in->filter_ctx->input_pads, in->pad_idx)) {
	case AVMEDIA_TYPE_VIDEO: return configure_input_video_filter(fg, ifilter, in);
	case AVMEDIA_TYPE_AUDIO: return configure_input_audio_filter(fg, ifilter, in);
	default: av_assert0(0);
	}
}

void FfmpegConvert::cleanup_filtergraph(FilterGraph *fg)
{
	int i;
	for (i = 0; i < fg->nb_outputs; i++)
		fg->outputs[i]->filter = (AVFilterContext *)NULL;
	for (i = 0; i < fg->nb_inputs; i++)
		fg->inputs[i]->filter = (AVFilterContext *)NULL;
	avfilter_graph_free(&fg->graph);
}

int FfmpegConvert::configure_filtergraph(FilterGraph *fg)
{
	AVFilterInOut *inputs, *outputs, *cur;
	int ret, i, simple = filtergraph_is_simple(fg);
	const char *graph_desc = simple ? fg->outputs[0]->ost->avfilter :
		fg->graph_desc;

	cleanup_filtergraph(fg);
	if (!(fg->graph = avfilter_graph_alloc()))
		return AVERROR(ENOMEM);

	if (simple) {
		OutputStream *ost = fg->outputs[0]->ost;
		char args[512];
		AVDictionaryEntry *e = NULL;

		fg->graph->nb_threads = filter_nbthreads;

		args[0] = 0;
		while ((e = av_dict_get(ost->sws_dict, "", e,
			AV_DICT_IGNORE_SUFFIX))) {
			av_strlcatf(args, sizeof(args), "%s=%s:", e->key, e->value);
		}
		if (strlen(args))
			args[strlen(args) - 1] = 0;
		fg->graph->scale_sws_opts = av_strdup(args);

		args[0] = 0;
		while ((e = av_dict_get(ost->swr_opts, "", e,
			AV_DICT_IGNORE_SUFFIX))) {
			av_strlcatf(args, sizeof(args), "%s=%s:", e->key, e->value);
		}
		if (strlen(args))
			args[strlen(args) - 1] = 0;
		av_opt_set(fg->graph, "aresample_swr_opts", args, 0);

		args[0] = '\0';
		while ((e = av_dict_get(fg->outputs[0]->ost->resample_opts, "", e,
			AV_DICT_IGNORE_SUFFIX))) {
			av_strlcatf(args, sizeof(args), "%s=%s:", e->key, e->value);
		}
		if (strlen(args))
			args[strlen(args) - 1] = '\0';

		e = av_dict_get(ost->encoder_opts, "threads", NULL, 0);
		if (e)
			av_opt_set(fg->graph, "threads", e->value, 0);
	}
	else {
		fg->graph->nb_threads = filter_complex_nbthreads;
	}

	if ((ret = avfilter_graph_parse2(fg->graph, graph_desc, &inputs, &outputs)) < 0)
		goto fail;



	if (simple && (!inputs || inputs->next || !outputs || outputs->next)) {
		const char *num_inputs;
		const char *num_outputs;
		if (!outputs) {
			num_outputs = "0";
		}
		else if (outputs->next) {
			num_outputs = ">1";
		}
		else {
			num_outputs = "1";
		}
		if (!inputs) {
			num_inputs = "0";
		}
		else if (inputs->next) {
			num_inputs = ">1";
		}
		else {
			num_inputs = "1";
		}
		av_log(NULL, AV_LOG_ERROR, "Simple filtergraph '%s' was expected "
			"to have exactly 1 input and 1 output."
			" However, it had %s input(s) and %s output(s)."
			" Please adjust, or use a complex filtergraph (-filter_complex) instead.\n",
			graph_desc, num_inputs, num_outputs);
		ret = AVERROR(EINVAL);
		goto fail;
	}

	for (cur = inputs, i = 0; cur; cur = cur->next, i++)
		if ((ret = configure_input_filter(fg, fg->inputs[i], cur)) < 0) {
			avfilter_inout_free(&inputs);
			avfilter_inout_free(&outputs);
			goto fail;
		}
	avfilter_inout_free(&inputs);

	for (cur = outputs, i = 0; cur; cur = cur->next, i++)
		configure_output_filter(fg, fg->outputs[i], cur);
	avfilter_inout_free(&outputs);

	if ((ret = avfilter_graph_config(fg->graph, NULL)) < 0)
		goto fail;

	/* limit the lists of allowed formats to the ones selected, to
	* make sure they stay the same if the filtergraph is reconfigured later */
	for (i = 0; i < fg->nb_outputs; i++) {
		OutputFilter *ofilter = fg->outputs[i];
		AVFilterContext *sink = ofilter->filter;

		ofilter->format = av_buffersink_get_format(sink);

		ofilter->width = av_buffersink_get_w(sink);
		ofilter->height = av_buffersink_get_h(sink);

		ofilter->sample_rate = av_buffersink_get_sample_rate(sink);
		ofilter->channel_layout = av_buffersink_get_channel_layout(sink);
	}

	fg->reconfiguration = 1;

	for (i = 0; i < fg->nb_outputs; i++) {
		OutputStream *ost = fg->outputs[i]->ost;
		if (!ost->enc) {
			/* identical to the same check in ffmpeg.c, needed because
			complex filter graphs are initialized earlier */
			av_log(NULL, AV_LOG_ERROR, "Encoder (codec %s) not found for output stream #%d:%d\n",
				avcodec_get_name(ost->st->codecpar->codec_id), ost->file_index, ost->index);
			ret = AVERROR(EINVAL);
			goto fail;
		}
		if (ost->enc->type == AVMEDIA_TYPE_AUDIO &&
			!(ost->enc->capabilities & AV_CODEC_CAP_VARIABLE_FRAME_SIZE))
			av_buffersink_set_frame_size(ost->filter->filter,
				ost->enc_ctx->frame_size);
	}

	for (i = 0; i < fg->nb_inputs; i++) {
		while (av_fifo_size(fg->inputs[i]->frame_queue)) {
			AVFrame *tmp;
			av_fifo_generic_read(fg->inputs[i]->frame_queue, &tmp, sizeof(tmp), NULL);
			ret = av_buffersrc_add_frame(fg->inputs[i]->filter, tmp);
			av_frame_free(&tmp);
			if (ret < 0)
				goto fail;
		}
	}

	/* send the EOFs for the finished inputs */
	for (i = 0; i < fg->nb_inputs; i++) {
		if (fg->inputs[i]->eof) {
			ret = av_buffersrc_add_frame(fg->inputs[i]->filter, NULL);
			if (ret < 0)
				goto fail;
		}
	}

	/* process queued up subtitle packets */
	for (i = 0; i < fg->nb_inputs; i++) {
		InputStream *ist = fg->inputs[i]->ist;
		if (ist->sub2video.sub_queue && ist->sub2video.frame) {
			while (av_fifo_size(ist->sub2video.sub_queue)) {
				AVSubtitle tmp;
				av_fifo_generic_read(ist->sub2video.sub_queue, &tmp, sizeof(tmp), NULL);
				sub2video_update(ist, &tmp);
				avsubtitle_free(&tmp);
			}
		}
	}

	return 0;

fail:
	cleanup_filtergraph(fg);
	return ret;
}

int FfmpegConvert::ifilter_parameters_from_frame(InputFilter *ifilter, const AVFrame *frame)
{
	av_buffer_unref(&ifilter->hw_frames_ctx);

	ifilter->format = frame->format;

	ifilter->width = frame->width;
	ifilter->height = frame->height;
	ifilter->sample_aspect_ratio = frame->sample_aspect_ratio;

	ifilter->sample_rate = frame->sample_rate;
	ifilter->channels = frame->channels;
	ifilter->channel_layout = frame->channel_layout;

	if (frame->hw_frames_ctx) {
		ifilter->hw_frames_ctx = av_buffer_ref(frame->hw_frames_ctx);
		if (!ifilter->hw_frames_ctx)
			return AVERROR(ENOMEM);
	}

	return 0;
}

int FfmpegConvert::ist_in_filtergraph(FilterGraph *fg, InputStream *ist)
{
	int i;
	for (i = 0; i < fg->nb_inputs; i++)
		if (fg->inputs[i]->ist == ist)
			return 1;
	return 0;
}

int FfmpegConvert::filtergraph_is_simple(FilterGraph *fg)
{
	return !fg->graph_desc;
}

void FfmpegConvert::uninit_options(OptionsContext *o)
{
	const OptionDef *po = options;
	int i;

	/* all OPT_SPEC and OPT_STRING can be freed in generic way */
	while (po->name) {
		void *dst = (uint8_t*)o + po->off;

		if (po->flags & OPT_SPEC) {
			SpecifierOpt **so = (SpecifierOpt **)dst;
			int i, *count = (int*)(so + 1);
			for (i = 0; i < *count; i++) {
				av_freep(&(*so)[i].specifier);
				if (po->flags & OPT_STRING)
					av_freep(&(*so)[i].u.str);
			}
			av_freep(so);
			*count = 0;
		}
		else if (po->flags & OPT_OFFSET && po->flags & OPT_STRING)
			av_freep(dst);
		po++;
	}

	for (i = 0; i < o->nb_stream_maps; i++)
		av_freep(&o->stream_maps[i].linklabel);
	av_freep(&o->stream_maps);
	av_freep(&o->audio_channel_maps);
	av_freep(&o->streamid_map);
	av_freep(&o->attachments);
}

void FfmpegConvert::init_options(OptionsContext *o)
{
	memset(o, 0, sizeof(*o));
	o->stop_time = INT64_MAX;
	o->mux_max_delay = 0.7;
	o->start_time = AV_NOPTS_VALUE;
	o->start_time_eof = AV_NOPTS_VALUE;
	o->recording_time = INT64_MAX;
	o->limit_filesize = UINT64_MAX;
	o->chapters_input_file = INT_MAX;
	o->accurate_seek = 1;
}

AVDictionary *FfmpegConvert::strip_specifiers(AVDictionary *dict)
{
	AVDictionaryEntry *e = NULL;
	AVDictionary    *ret = NULL;

	while ((e = av_dict_get(dict, "", e, AV_DICT_IGNORE_SUFFIX))) {
		char *p = strchr(e->key, ':');

		if (p)
			*p = 0;
		av_dict_set(&ret, e->key, e->value, 0);
		if (p)
			*p = ':';
	}
	return ret;
}



void FfmpegConvert::parse_meta_type(char *arg, char *type, int *index, const char **stream_spec)
{
	if (*arg) {
		*type = *arg;
		switch (*arg) {
		case 'g':
			break;
		case 's':
			if (*(++arg) && *arg != ':') {
				av_log(NULL, AV_LOG_FATAL, "Invalid metadata specifier %s.\n", arg);
				exit_program(1);
			}
			*stream_spec = *arg == ':' ? arg + 1 : "";
			break;
		case 'c':
		case 'p':
			if (*(++arg) == ':')
				*index = strtol(++arg, NULL, 0);
			break;
		default:
			av_log(NULL, AV_LOG_FATAL, "Invalid metadata type %c.\n", *arg);
			exit_program(1);
		}
	}
	else
		*type = 'g';
}

int FfmpegConvert::copy_metadata(char *outspec, char *inspec, AVFormatContext *oc, AVFormatContext *ic, OptionsContext *o)
{
	AVDictionary **meta_in = NULL;
	AVDictionary **meta_out = NULL;
	int i, ret = 0;
	char type_in, type_out;
	const char *istream_spec = NULL, *ostream_spec = NULL;
	int idx_in = 0, idx_out = 0;

	parse_meta_type(inspec, &type_in, &idx_in, &istream_spec);
	parse_meta_type(outspec, &type_out, &idx_out, &ostream_spec);

	if (!ic) {
		if (type_out == 'g' || !*outspec)
			o->metadata_global_manual = 1;
		if (type_out == 's' || !*outspec)
			o->metadata_streams_manual = 1;
		if (type_out == 'c' || !*outspec)
			o->metadata_chapters_manual = 1;
		return 0;
	}

	if (type_in == 'g' || type_out == 'g')
		o->metadata_global_manual = 1;
	if (type_in == 's' || type_out == 's')
		o->metadata_streams_manual = 1;
	if (type_in == 'c' || type_out == 'c')
		o->metadata_chapters_manual = 1;

	/* ic is NULL when just disabling automatic mappings */
	if (!ic)
		return 0;

#define METADATA_CHECK_INDEX(index, nb_elems, desc)\
if ((index) < 0 || (index) >= (nb_elems)) {\
    exit_program(1);\
}

#define SET_DICT(type, meta, context, index)\
    switch (type) {\
    case 'g':\
        meta = &context->metadata;\
        break;\
    case 'c':\
        METADATA_CHECK_INDEX(index, context->nb_chapters, "chapter")\
        meta = &context->chapters[index]->metadata;\
        break;\
    case 'p':\
        METADATA_CHECK_INDEX(index, context->nb_programs, "program")\
        meta = &context->programs[index]->metadata;\
        break;\
    case 's':\
        break; /* handled separately below */ \
    default: av_assert0(0);\
    }\

	SET_DICT(type_in, meta_in, ic, idx_in);
	SET_DICT(type_out, meta_out, oc, idx_out);

	/* for input streams choose first matching stream */
	if (type_in == 's') {
		for (i = 0; i < ic->nb_streams; i++) {
			if ((ret = check_stream_specifier(ic, ic->streams[i], istream_spec)) > 0) {
				meta_in = &ic->streams[i]->metadata;
				break;
			}
			else if (ret < 0)
				exit_program(1);
		}
		if (!meta_in) {
			av_log(NULL, AV_LOG_FATAL, "Stream specifier %s does not match  any streams.\n", istream_spec);
			exit_program(1);
		}
	}

	if (type_out == 's') {
		for (i = 0; i < oc->nb_streams; i++) {
			if ((ret = check_stream_specifier(oc, oc->streams[i], ostream_spec)) > 0) {
				meta_out = &oc->streams[i]->metadata;
				av_dict_copy(meta_out, *meta_in, AV_DICT_DONT_OVERWRITE);
			}
			else if (ret < 0)
				exit_program(1);
		}
	}
	else
		av_dict_copy(meta_out, *meta_in, AV_DICT_DONT_OVERWRITE);

	return 0;
}


AVCodec *FfmpegConvert::find_codec_or_die(const char *name, enum AVMediaType type, int encoder)
{
	const AVCodecDescriptor *desc;
	const char *codec_string = encoder ? "encoder" : "decoder";
	AVCodec *codec;

	codec = encoder ?
		avcodec_find_encoder_by_name(name) :
		avcodec_find_decoder_by_name(name);

	if (!codec && (desc = avcodec_descriptor_get_by_name(name))) {
		codec = encoder ? avcodec_find_encoder(desc->id) :
			avcodec_find_decoder(desc->id);
		if (codec)
			av_log(NULL, AV_LOG_VERBOSE, "Matched %s '%s' for codec '%s'.\n",
				codec_string, codec->name, desc->name);
	}

	if (!codec) {
		av_log(NULL, AV_LOG_FATAL, "Unknown %s '%s'\n", codec_string, name);
		exit_program(1);
	}
	if (codec->type != type) {
		av_log(NULL, AV_LOG_FATAL, "Invalid %s type '%s'\n", codec_string, name);
		exit_program(1);
	}
	return codec;
}

AVCodec *FfmpegConvert::choose_decoder(OptionsContext *o, AVFormatContext *s, AVStream *st)
{
	char *codec_name = NULL;

	MATCH_PER_STREAM_OPT(codec_names, str, codec_name, s, st, char*);
	if (codec_name) {
		AVCodec *codec = find_codec_or_die(codec_name, st->codecpar->codec_type, 0);
		st->codecpar->codec_id = codec->id;
		return codec;
	}
	else
		return avcodec_find_decoder(st->codecpar->codec_id);
}

/* Add all the streams from the given input file to the global
* list of input streams. */
void FfmpegConvert::add_input_streams(OptionsContext *o, AVFormatContext *ic)
{
	int i, ret;

	for (i = 0; i < ic->nb_streams; i++) {
		AVStream *st = ic->streams[i];
		AVCodecParameters *par = st->codecpar;
		InputStream *ist = (InputStream *)av_mallocz(sizeof(*ist));
		char *framerate = NULL, *hwaccel_device = NULL;
		const char *hwaccel = NULL;
		char *hwaccel_output_format = NULL;
		char *codec_tag = NULL;
		char *next;
		char *discard_str = NULL;
		const AVClass *cc = avcodec_get_class();
		const AVOption *discard_opt = av_opt_find(&cc, "skip_frame", NULL, 0, 0);

		if (!ist)
			exit_program(1);

		GROW_ARRAY(input_streams, nb_input_streams, InputStream **);
		input_streams[nb_input_streams - 1] = ist;

		ist->st = st;
		ist->file_index = nb_input_files;
		ist->discard = 1;
		st->discard = AVDISCARD_ALL;
		ist->nb_samples = 0;
		ist->min_pts = INT64_MAX;
		ist->max_pts = INT64_MIN;

		ist->ts_scale = 1.0;
		MATCH_PER_STREAM_OPT(ts_scale, dbl, ist->ts_scale, ic, st, double);

		ist->autorotate = 1;
		MATCH_PER_STREAM_OPT(autorotate, i, ist->autorotate, ic, st, int);

		MATCH_PER_STREAM_OPT(codec_tags, str, codec_tag, ic, st, char*);
		if (codec_tag) {
			uint32_t tag = strtol(codec_tag, &next, 0);
			if (*next)
				tag = AV_RL32(codec_tag);
			st->codecpar->codec_tag = tag;
		}

		ist->dec = choose_decoder(o, ic, st);
		ist->decoder_opts = filter_codec_opts(o->g->codec_opts, ist->st->codecpar->codec_id, ic, st, ist->dec);

		ist->reinit_filters = -1;
		MATCH_PER_STREAM_OPT(reinit_filters, i, ist->reinit_filters, ic, st, int);

		MATCH_PER_STREAM_OPT(discard, str, discard_str, ic, st, char*);
		ist->user_set_discard = AVDISCARD_NONE;
		if (discard_str && av_opt_eval_int(&cc, discard_opt, discard_str, &ist->user_set_discard) < 0) {
			av_log(NULL, AV_LOG_ERROR, "Error parsing discard %s.\n",
				discard_str);
			exit_program(1);
		}

		ist->filter_in_rescale_delta_last = AV_NOPTS_VALUE;

		ist->dec_ctx = avcodec_alloc_context3(ist->dec);
		if (!ist->dec_ctx) {
			av_log(NULL, AV_LOG_ERROR, "Error allocating the decoder context.\n");
			exit_program(1);
		}

		ret = avcodec_parameters_to_context(ist->dec_ctx, par);
		if (ret < 0) {
			av_log(NULL, AV_LOG_ERROR, "Error initializing the decoder context.\n");
			exit_program(1);
		}

		if (o->bitexact)
			ist->dec_ctx->flags |= AV_CODEC_FLAG_BITEXACT;

		switch (par->codec_type) {
		case AVMEDIA_TYPE_VIDEO:
			if (!ist->dec)
				ist->dec = avcodec_find_decoder(par->codec_id);
#if FF_API_LOWRES
			if (st->codec->lowres) {
				ist->dec_ctx->lowres = st->codec->lowres;
				ist->dec_ctx->width = st->codec->width;
				ist->dec_ctx->height = st->codec->height;
				ist->dec_ctx->coded_width = st->codec->coded_width;
				ist->dec_ctx->coded_height = st->codec->coded_height;
			}
#endif

			// avformat_find_stream_info() doesn't set this for us anymore.
			ist->dec_ctx->framerate = st->avg_frame_rate;

			MATCH_PER_STREAM_OPT(frame_rates, str, framerate, ic, st, char*);
			if (framerate && av_parse_video_rate(&ist->framerate,
				framerate) < 0) {
				av_log(NULL, AV_LOG_ERROR, "Error parsing framerate %s.\n",
					framerate);
				exit_program(1);
			}

			ist->top_field_first = -1;
			MATCH_PER_STREAM_OPT(top_field_first, i, ist->top_field_first, ic, st, int);


			MATCH_PER_STREAM_OPT(hwaccel_devices, str, hwaccel_device, ic, st, char*);
			if (hwaccel_device) {
				ist->hwaccel_device = av_strdup(hwaccel_device);
				if (!ist->hwaccel_device)
					exit_program(1);
			}

			MATCH_PER_STREAM_OPT(hwaccel_output_formats, str, hwaccel_output_format, ic, st, char*);

			if (hwaccel_output_format) {
				ist->hwaccel_output_format = av_get_pix_fmt(hwaccel_output_format);
				if (ist->hwaccel_output_format == AV_PIX_FMT_NONE) {
					av_log(NULL, AV_LOG_FATAL, "Unrecognised hwaccel output "
						"format: %s", hwaccel_output_format);
				}
			}
			else {
				ist->hwaccel_output_format = AV_PIX_FMT_NONE;
			}

			ist->hwaccel_pix_fmt = AV_PIX_FMT_NONE;

			break;
		case AVMEDIA_TYPE_AUDIO:
			ist->guess_layout_max = INT_MAX;
			MATCH_PER_STREAM_OPT(guess_layout_max, i, ist->guess_layout_max, ic, st, int);
			guess_input_channel_layout(ist);
			break;
		case AVMEDIA_TYPE_DATA:
		case AVMEDIA_TYPE_SUBTITLE: {
			char *canvas_size = NULL;
			if (!ist->dec)
				ist->dec = avcodec_find_decoder(par->codec_id);
			MATCH_PER_STREAM_OPT(fix_sub_duration, i, ist->fix_sub_duration, ic, st, int);
			MATCH_PER_STREAM_OPT(canvas_sizes, str, canvas_size, ic, st, char*);
			if (canvas_size &&
				av_parse_video_size(&ist->dec_ctx->width, &ist->dec_ctx->height, canvas_size) < 0) {
				av_log(NULL, AV_LOG_FATAL, "Invalid canvas size: %s.\n", canvas_size);
				exit_program(1);
			}
			break;
		}
		case AVMEDIA_TYPE_ATTACHMENT:
		case AVMEDIA_TYPE_UNKNOWN:
			break;
		default:
			abort();
		}

		ret = avcodec_parameters_from_context(par, ist->dec_ctx);
		if (ret < 0) {
			av_log(NULL, AV_LOG_ERROR, "Error initializing the decoder context.\n");
			exit_program(1);
		}
	}
}


void FfmpegConvert::dump_attachment(AVStream *st, const char *filename)
{
	int ret;
	AVIOContext *out = NULL;
	AVDictionaryEntry *e;

	if (!st->codecpar->extradata_size) {
		av_log(NULL, AV_LOG_WARNING, "No extradata to dump in stream #%d:%d.\n",
			nb_input_files - 1, st->index);
		return;
	}
	if (!*filename && (e = av_dict_get(st->metadata, "filename", NULL, 0)))
		filename = e->value;
	if (!*filename) {
		av_log(NULL, AV_LOG_FATAL, "No filename specified and no 'filename' tag"
			"in stream #%d:%d.\n", nb_input_files - 1, st->index);
		exit_program(1);
	}


	if ((ret = avio_open2(&out, filename, AVIO_FLAG_WRITE, &int_cb, NULL)) < 0) {
		av_log(NULL, AV_LOG_FATAL, "Could not open file %s for writing.\n",
			filename);
		exit_program(1);
	}

	avio_write(out, st->codecpar->extradata, st->codecpar->extradata_size);
	avio_flush(out);
	avio_close(out);
}


uint8_t *FfmpegConvert::get_line(AVIOContext *s)
{
	AVIOContext *line;
	uint8_t *buf;
	char c;

	if (avio_open_dyn_buf(&line) < 0) {
		av_log(NULL, AV_LOG_FATAL, "Could not alloc buffer for reading preset.\n");
		exit_program(1);
	}

	while ((c = avio_r8(s)) && c != '\n')
		avio_w8(line, c);
	avio_w8(line, 0);
	avio_close_dyn_buf(line, &buf);

	return buf;
}

int FfmpegConvert::get_preset_file_2(const char *preset_name, const char *codec_name, AVIOContext **s)
{
	int i, ret = -1;
	char filename[1000];
	const char *base[3] = { getenv("AVCONV_DATADIR"),
		getenv("HOME"),
		AVCONV_DATADIR,
	};

	for (i = 0; i < FF_ARRAY_ELEMS(base) && ret < 0; i++) {
		if (!base[i])
			continue;
		if (codec_name) {
			snprintf(filename, sizeof(filename), "%s%s/%s-%s.avpreset", base[i],
				i != 1 ? "" : "/.avconv", codec_name, preset_name);
			ret = avio_open2(s, filename, AVIO_FLAG_READ, &int_cb, NULL);
		}
		if (ret < 0) {
			snprintf(filename, sizeof(filename), "%s%s/%s.avpreset", base[i],
				i != 1 ? "" : "/.avconv", preset_name);
			ret = avio_open2(s, filename, AVIO_FLAG_READ, &int_cb, NULL);
		}
	}
	return ret;
}

int FfmpegConvert::choose_encoder(OptionsContext *o, AVFormatContext *s, OutputStream *ost)
{
	enum AVMediaType type = ost->st->codecpar->codec_type;
	char *codec_name = NULL;

	if (type == AVMEDIA_TYPE_VIDEO || type == AVMEDIA_TYPE_AUDIO || type == AVMEDIA_TYPE_SUBTITLE) {
		MATCH_PER_STREAM_OPT(codec_names, str, codec_name, s, ost->st, char*);
		if (!codec_name) {
			ost->st->codecpar->codec_id = av_guess_codec(s->oformat, NULL, s->url,
				NULL, ost->st->codecpar->codec_type);
			ost->enc = avcodec_find_encoder(ost->st->codecpar->codec_id);
			if (!ost->enc) {
				av_log(NULL, AV_LOG_FATAL, "Automatic encoder selection failed for "
					"output stream #%d:%d. Default encoder for format %s (codec %s) is "
					"probably disabled. Please choose an encoder manually.\n",
					ost->file_index, ost->index, s->oformat->name,
					avcodec_get_name(ost->st->codecpar->codec_id));
				return AVERROR_ENCODER_NOT_FOUND;
			}
		}
		else if (!strcmp(codec_name, "copy"))
			ost->stream_copy = 1;
		else {
			ost->enc = find_codec_or_die(codec_name, ost->st->codecpar->codec_type, 1);
			ost->st->codecpar->codec_id = ost->enc->id;
		}
		ost->encoding_needed = !ost->stream_copy;
	}
	else {
		/* no encoding supported for other media types */
		ost->stream_copy = 1;
		ost->encoding_needed = 0;
	}

	return 0;
}

OutputStream *FfmpegConvert::new_output_stream(OptionsContext *o,
	AVFormatContext *oc,
enum AVMediaType type,
	int source_index)
{
	OutputStream *ost;
	AVStream *st = avformat_new_stream(oc, NULL);
	int idx = oc->nb_streams - 1, ret = 0;
	const char *bsfs = NULL, *time_base = NULL;
	char *next, *codec_tag = NULL;
	double qscale = -1;
	int i;

	if (!st) {
		av_log(NULL, AV_LOG_FATAL, "Could not alloc stream.\n");
		exit_program(1);
	}

	if (oc->nb_streams - 1 < o->nb_streamid_map)
		st->id = o->streamid_map[oc->nb_streams - 1];

	GROW_ARRAY(output_streams, nb_output_streams, OutputStream**);
	ost = (OutputStream *)av_mallocz(sizeof(*ost));
	output_streams[nb_output_streams - 1] = ost;

	ost->file_index = nb_output_files - 1;
	ost->index = idx;
	ost->st = st;
	st->codecpar->codec_type = type;

	ret = choose_encoder(o, oc, ost);
	if (ret < 0) {
		av_log(NULL, AV_LOG_FATAL, "Error selecting an encoder for stream "
			"%d:%d\n", ost->file_index, ost->index);
		exit_program(1);
	}

	ost->enc_ctx = avcodec_alloc_context3(ost->enc);
	if (!ost->enc_ctx) {
		av_log(NULL, AV_LOG_ERROR, "Error allocating the encoding context.\n");
		exit_program(1);
	}
	ost->enc_ctx->codec_type = type;

	ost->ref_par = avcodec_parameters_alloc();
	if (!ost->ref_par) {
		av_log(NULL, AV_LOG_ERROR, "Error allocating the encoding parameters.\n");
		exit_program(1);
	}

	if (ost->enc) {
		AVIOContext *s = NULL;
		char *buf = NULL, *arg = NULL, *preset = NULL;

		ost->encoder_opts = filter_codec_opts(o->g->codec_opts, ost->enc->id, oc, st, ost->enc);

		MATCH_PER_STREAM_OPT(presets, str, preset, oc, st, char*);
		if (preset && (!(ret = get_preset_file_2(preset, ost->enc->name, &s)))) {
			do {
				buf = (char*)get_line(s);
				if (!buf[0] || buf[0] == '#') {
					av_free(buf);
					continue;
				}
				if (!(arg = strchr(buf, '='))) {
					av_log(NULL, AV_LOG_FATAL, "Invalid line found in the preset file.\n");
					exit_program(1);
				}
				*arg++ = 0;
				av_dict_set(&ost->encoder_opts, buf, arg, AV_DICT_DONT_OVERWRITE);
				av_free(buf);
			} while (!s->eof_reached);
			avio_closep(&s);
		}
		if (ret) {
			av_log(NULL, AV_LOG_FATAL,
				"Preset %s specified for stream %d:%d, but could not be opened.\n",
				preset, ost->file_index, ost->index);
			exit_program(1);
		}
	}
	else {
		ost->encoder_opts = filter_codec_opts(o->g->codec_opts, AV_CODEC_ID_NONE, oc, st, NULL);
	}


	if (o->bitexact)
		ost->enc_ctx->flags |= AV_CODEC_FLAG_BITEXACT;

	MATCH_PER_STREAM_OPT(time_bases, str, time_base, oc, st, const char*);
	if (time_base) {
		AVRational q;
		if (av_parse_ratio(&q, time_base, INT_MAX, 0, NULL) < 0 ||
			q.num <= 0 || q.den <= 0) {
			av_log(NULL, AV_LOG_FATAL, "Invalid time base: %s\n", time_base);
			exit_program(1);
		}
		st->time_base = q;
	}

	MATCH_PER_STREAM_OPT(enc_time_bases, str, time_base, oc, st, const char*);
	if (time_base) {
		AVRational q;
		if (av_parse_ratio(&q, time_base, INT_MAX, 0, NULL) < 0 ||
			q.den <= 0) {
			av_log(NULL, AV_LOG_FATAL, "Invalid time base: %s\n", time_base);
			exit_program(1);
		}
		ost->enc_timebase = q;
	}

	ost->max_frames = INT64_MAX;
	MATCH_PER_STREAM_OPT(max_frames, i64, ost->max_frames, oc, st, int64_t);
	for (i = 0; i<o->nb_max_frames; i++) {
		char *p = o->max_frames[i].specifier;
		if (!*p && type != AVMEDIA_TYPE_VIDEO) {
			av_log(NULL, AV_LOG_WARNING, "Applying unspecific -frames to non video streams, maybe you meant -vframes ?\n");
			break;
		}
	}

	ost->copy_prior_start = -1;
	MATCH_PER_STREAM_OPT(copy_prior_start, i, ost->copy_prior_start, oc, st, int);

	MATCH_PER_STREAM_OPT(bitstream_filters, str, bsfs, oc, st, const char*);
	while (bsfs && *bsfs) {
		const AVBitStreamFilter *filter;
		char *bsf, *bsf_options_str, *bsf_name;

		bsf = av_get_token(&bsfs, ",");
		if (!bsf)
			exit_program(1);
		bsf_name = av_strtok(bsf, "=", &bsf_options_str);
		if (!bsf_name)
			exit_program(1);

		filter = av_bsf_get_by_name(bsf_name);
		if (!filter) {
			av_log(NULL, AV_LOG_FATAL, "Unknown bitstream filter %s\n", bsf_name);
			exit_program(1);
		}

		ost->bsf_ctx = (AVBSFContext **)av_realloc_array(ost->bsf_ctx,
			ost->nb_bitstream_filters + 1,
			sizeof(*ost->bsf_ctx));


		ret = av_bsf_alloc(filter, &ost->bsf_ctx[ost->nb_bitstream_filters]);
		if (ret < 0) {
			av_log(NULL, AV_LOG_ERROR, "Error allocating a bitstream filter context\n");
			exit_program(1);
		}

		ost->nb_bitstream_filters++;

		if (bsf_options_str && filter->priv_class) {
			const AVOption *opt = av_opt_next(ost->bsf_ctx[ost->nb_bitstream_filters - 1]->priv_data, NULL);
			const char * shorthand[2] = { NULL };

			if (opt)
				shorthand[0] = opt->name;

			ret = av_opt_set_from_string(ost->bsf_ctx[ost->nb_bitstream_filters - 1]->priv_data, bsf_options_str, shorthand, "=", ":");
			if (ret < 0) {
				av_log(NULL, AV_LOG_ERROR, "Error parsing options for bitstream filter %s\n", bsf_name);
				exit_program(1);
			}
		}
		av_freep(&bsf);

		if (*bsfs)
			bsfs++;
	}

	MATCH_PER_STREAM_OPT(codec_tags, str, codec_tag, oc, st, char*);
	if (codec_tag) {
		uint32_t tag = strtol(codec_tag, &next, 0);
		if (*next)
			tag = AV_RL32(codec_tag);
		ost->st->codecpar->codec_tag =
			ost->enc_ctx->codec_tag = tag;
	}

	MATCH_PER_STREAM_OPT(qscale, dbl, qscale, oc, st, double);
	if (qscale >= 0) {
		ost->enc_ctx->flags |= AV_CODEC_FLAG_QSCALE;
		ost->enc_ctx->global_quality = FF_QP2LAMBDA * qscale;
	}

	MATCH_PER_STREAM_OPT(disposition, str, ost->disposition, oc, st, char*);
	ost->disposition = av_strdup(ost->disposition);

	ost->max_muxing_queue_size = 128;
	MATCH_PER_STREAM_OPT(max_muxing_queue_size, i, ost->max_muxing_queue_size, oc, st, int);
	ost->max_muxing_queue_size *= sizeof(AVPacket);

	if (oc->oformat->flags & AVFMT_GLOBALHEADER)
		ost->enc_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

	av_dict_copy(&ost->sws_dict, o->g->sws_dict, 0);

	av_dict_copy(&ost->swr_opts, o->g->swr_opts, 0);
	if (ost->enc && av_get_exact_bits_per_sample(ost->enc->id) == 24)
		av_dict_set(&ost->swr_opts, "output_sample_bits", "24", 0);

	av_dict_copy(&ost->resample_opts, o->g->resample_opts, 0);

	ost->source_index = source_index;
	if (source_index >= 0) {
		ost->sync_ist = input_streams[source_index];
		input_streams[source_index]->discard = 0;
		input_streams[source_index]->st->discard = (AVDiscard)input_streams[source_index]->user_set_discard;
	}
	ost->last_mux_dts = AV_NOPTS_VALUE;

	ost->muxing_queue = av_fifo_alloc(8 * sizeof(AVPacket));
	if (!ost->muxing_queue)
		exit_program(1);

	return ost;
}

void FfmpegConvert::parse_matrix_coeffs(uint16_t *dest, const char *str)
{
	int i;
	const char *p = str;
	for (i = 0;; i++) {
		dest[i] = atoi(p);
		if (i == 63)
			break;
		p = strchr(p, ',');
		if (!p) {
			av_log(NULL, AV_LOG_FATAL, "Syntax error in matrix \"%s\" at coeff %d\n", str, i);
			exit_program(1);
		}
		p++;
	}
}

/* read file contents into a string */
uint8_t *FfmpegConvert::read_file(const char *filename)
{
	AVIOContext *pb = NULL;
	AVIOContext *dyn_buf = NULL;
	int ret = avio_open(&pb, filename, AVIO_FLAG_READ);
	uint8_t buf[1024], *str;

	if (ret < 0) {
		av_log(NULL, AV_LOG_ERROR, "Error opening file %s.\n", filename);
		return NULL;
	}

	ret = avio_open_dyn_buf(&dyn_buf);
	if (ret < 0) {
		avio_closep(&pb);
		return NULL;
	}
	while ((ret = avio_read(pb, buf, sizeof(buf))) > 0)
		avio_write(dyn_buf, buf, ret);
	avio_w8(dyn_buf, 0);
	avio_closep(&pb);

	ret = avio_close_dyn_buf(dyn_buf, &str);
	if (ret < 0)
		return NULL;
	return str;
}

char *FfmpegConvert::get_ost_filters(OptionsContext *o, AVFormatContext *oc,
	OutputStream *ost)
{
	AVStream *st = ost->st;

	if (ost->filters_script && ost->filters) {
		av_log(NULL, AV_LOG_ERROR, "Both -filter and -filter_script set for "
			"output stream #%d:%d.\n", nb_output_files, st->index);
		exit_program(1);
	}

	if (ost->filters_script)
		return (char*)read_file(ost->filters_script);
	else if (ost->filters)
		return av_strdup(ost->filters);

	return av_strdup(st->codecpar->codec_type == AVMEDIA_TYPE_VIDEO ?
		"null" : "anull");
}

void FfmpegConvert::check_streamcopy_filters(OptionsContext *o, AVFormatContext *oc,
	const OutputStream *ost, enum AVMediaType type)
{
	if (ost->filters_script || ost->filters) {
		av_log(NULL, AV_LOG_ERROR,
			"%s '%s' was defined for %s output stream %d:%d but codec copy was selected.\n"
			"Filtering and streamcopy cannot be used together.\n",
			ost->filters ? "Filtergraph" : "Filtergraph script",
			ost->filters ? ost->filters : ost->filters_script,
			av_get_media_type_string(type), ost->file_index, ost->index);
		exit_program(1);
	}
}

OutputStream *FfmpegConvert::new_video_stream(OptionsContext *o, AVFormatContext *oc, int source_index)
{
	AVStream *st;
	OutputStream *ost;
	AVCodecContext *video_enc;
	char *frame_rate = NULL, *frame_aspect_ratio = NULL;

	ost = new_output_stream(o, oc, AVMEDIA_TYPE_VIDEO, source_index);
	st = ost->st;
	video_enc = ost->enc_ctx;

	MATCH_PER_STREAM_OPT(frame_rates, str, frame_rate, oc, st, char*);
	if (frame_rate && av_parse_video_rate(&ost->frame_rate, frame_rate) < 0) {
		av_log(NULL, AV_LOG_FATAL, "Invalid framerate value: %s\n", frame_rate);
		exit_program(1);
	}
	if (frame_rate && video_sync_method == VSYNC_PASSTHROUGH)
		av_log(NULL, AV_LOG_ERROR, "Using -vsync 0 and -r can produce invalid output files\n");

	MATCH_PER_STREAM_OPT(frame_aspect_ratios, str, frame_aspect_ratio, oc, st, char*);
	if (frame_aspect_ratio) {
		AVRational q;
		if (av_parse_ratio(&q, frame_aspect_ratio, 255, 0, NULL) < 0 ||
			q.num <= 0 || q.den <= 0) {
			av_log(NULL, AV_LOG_FATAL, "Invalid aspect ratio: %s\n", frame_aspect_ratio);
			exit_program(1);
		}
		ost->frame_aspect_ratio = q;
	}

	MATCH_PER_STREAM_OPT(filter_scripts, str, ost->filters_script, oc, st, char*);
	MATCH_PER_STREAM_OPT(filters, str, ost->filters, oc, st, char*);

	if (!ost->stream_copy) {
		const char *p = NULL;
		char *frame_size = NULL;
		char *frame_pix_fmt = NULL;
		char *intra_matrix = NULL, *inter_matrix = NULL;
		char *chroma_intra_matrix = NULL;
		int do_pass = 0;
		int i;

		MATCH_PER_STREAM_OPT(frame_sizes, str, frame_size, oc, st, char*);
		if (frame_size && av_parse_video_size(&video_enc->width, &video_enc->height, frame_size) < 0) {
			av_log(NULL, AV_LOG_FATAL, "Invalid frame size: %s.\n", frame_size);
			exit_program(1);
		}

		video_enc->bits_per_raw_sample = frame_bits_per_raw_sample;
		MATCH_PER_STREAM_OPT(frame_pix_fmts, str, frame_pix_fmt, oc, st, char*);
		if (frame_pix_fmt && *frame_pix_fmt == '+') {
			ost->keep_pix_fmt = 1;
			if (!*++frame_pix_fmt)
				frame_pix_fmt = NULL;
		}
		if (frame_pix_fmt && (video_enc->pix_fmt = av_get_pix_fmt(frame_pix_fmt)) == AV_PIX_FMT_NONE) {
			av_log(NULL, AV_LOG_FATAL, "Unknown pixel format requested: %s.\n", frame_pix_fmt);
			exit_program(1);
		}
		st->sample_aspect_ratio = video_enc->sample_aspect_ratio;

		MATCH_PER_STREAM_OPT(intra_matrices, str, intra_matrix, oc, st, char*);
		if (intra_matrix) {
			if (!(video_enc->intra_matrix = (uint16_t *)av_mallocz(sizeof(*video_enc->intra_matrix) * 64))) {
				av_log(NULL, AV_LOG_FATAL, "Could not allocate memory for intra matrix.\n");
				exit_program(1);
			}
			parse_matrix_coeffs(video_enc->intra_matrix, intra_matrix);
		}
		MATCH_PER_STREAM_OPT(chroma_intra_matrices, str, chroma_intra_matrix, oc, st, char*);
		if (chroma_intra_matrix) {
			uint16_t *p = (uint16_t *)av_mallocz(sizeof(*video_enc->chroma_intra_matrix) * 64);
			video_enc->chroma_intra_matrix = p;
			parse_matrix_coeffs(p, chroma_intra_matrix);
		}
		MATCH_PER_STREAM_OPT(inter_matrices, str, inter_matrix, oc, st, char*);
		if (inter_matrix) {
			if (!(video_enc->inter_matrix = (uint16_t*)av_mallocz(sizeof(*video_enc->inter_matrix) * 64))) {
				av_log(NULL, AV_LOG_FATAL, "Could not allocate memory for inter matrix.\n");
				exit_program(1);
			}
			parse_matrix_coeffs(video_enc->inter_matrix, inter_matrix);
		}

		MATCH_PER_STREAM_OPT(rc_overrides, str, p, oc, st, const char*);
		for (i = 0; p; i++) {
			int start, end, q;
			int e = sscanf(p, "%d,%d,%d", &start, &end, &q);
			if (e != 3) {
				av_log(NULL, AV_LOG_FATAL, "error parsing rc_override\n");
				exit_program(1);
			}
			video_enc->rc_override =
				(RcOverride *)av_realloc_array(video_enc->rc_override,
					i + 1, sizeof(RcOverride));
			if (!video_enc->rc_override) {
				av_log(NULL, AV_LOG_FATAL, "Could not (re)allocate memory for rc_override.\n");
				exit_program(1);
			}
			video_enc->rc_override[i].start_frame = start;
			video_enc->rc_override[i].end_frame = end;
			if (q > 0) {
				video_enc->rc_override[i].qscale = q;
				video_enc->rc_override[i].quality_factor = 1.0;
			}
			else {
				video_enc->rc_override[i].qscale = 0;
				video_enc->rc_override[i].quality_factor = -q / 100.0;
			}
			p = strchr(p, '/');
			if (p) p++;
		}
		video_enc->rc_override_count = i;

		/* two pass mode */
		MATCH_PER_STREAM_OPT(pass, i, do_pass, oc, st, int);
		if (do_pass) {
			if (do_pass & 1) {
				video_enc->flags |= AV_CODEC_FLAG_PASS1;
				av_dict_set(&ost->encoder_opts, "flags", "+pass1", AV_DICT_APPEND);
			}
			if (do_pass & 2) {
				video_enc->flags |= AV_CODEC_FLAG_PASS2;
				av_dict_set(&ost->encoder_opts, "flags", "+pass2", AV_DICT_APPEND);
			}
		}

		MATCH_PER_STREAM_OPT(passlogfiles, str, ost->logfile_prefix, oc, st, char*);
		if (ost->logfile_prefix &&
			!(ost->logfile_prefix = av_strdup(ost->logfile_prefix)))
			exit_program(1);

		if (do_pass) {
			char logfilename[1024];
			FILE *f;

			snprintf(logfilename, sizeof(logfilename), "%s-%d.log",
				ost->logfile_prefix ? ost->logfile_prefix :
				DEFAULT_PASS_LOGFILENAME_PREFIX,
				i);
			if (!strcmp(ost->enc->name, "libx264")) {
				av_dict_set(&ost->encoder_opts, "stats", logfilename, AV_DICT_DONT_OVERWRITE);
			}
			else {
				if (video_enc->flags & AV_CODEC_FLAG_PASS2) {
					char  *logbuffer = (char*)read_file(logfilename);

					if (!logbuffer) {
						av_log(NULL, AV_LOG_FATAL, "Error reading log file '%s' for pass-2 encoding\n",
							logfilename);
						exit_program(1);
					}
					video_enc->stats_in = logbuffer;
				}
				if (video_enc->flags & AV_CODEC_FLAG_PASS1) {
					f = av_fopen_utf8(logfilename, "wb");
					if (!f) {
						av_log(NULL, AV_LOG_FATAL,
							"Cannot write log file '%s' for pass-1 encoding: %s\n",
							logfilename, strerror(errno));
						exit_program(1);
					}
					ost->logfile = f;
				}
			}
		}

		MATCH_PER_STREAM_OPT(forced_key_frames, str, ost->forced_keyframes, oc, st, char*);
		if (ost->forced_keyframes)
			ost->forced_keyframes = av_strdup(ost->forced_keyframes);

		MATCH_PER_STREAM_OPT(force_fps, i, ost->force_fps, oc, st, int);

		ost->top_field_first = -1;
		MATCH_PER_STREAM_OPT(top_field_first, i, ost->top_field_first, oc, st, int);


		ost->avfilter = get_ost_filters(o, oc, ost);
		if (!ost->avfilter)
			exit_program(1);
	}
	else {
		MATCH_PER_STREAM_OPT(copy_initial_nonkeyframes, i, ost->copy_initial_nonkeyframes, oc, st, int);
	}

	if (ost->stream_copy)
		check_streamcopy_filters(o, oc, ost, AVMEDIA_TYPE_VIDEO);

	return ost;
}

OutputStream *FfmpegConvert::new_audio_stream(OptionsContext *o, AVFormatContext *oc, int source_index)
{
	int n;
	AVStream *st;
	OutputStream *ost;
	AVCodecContext *audio_enc;

	ost = new_output_stream(o, oc, AVMEDIA_TYPE_AUDIO, source_index);
	st = ost->st;

	audio_enc = ost->enc_ctx;
	audio_enc->codec_type = AVMEDIA_TYPE_AUDIO;

	MATCH_PER_STREAM_OPT(filter_scripts, str, ost->filters_script, oc, st, char*);
	MATCH_PER_STREAM_OPT(filters, str, ost->filters, oc, st, char*);

	if (!ost->stream_copy) {
		char *sample_fmt = NULL;

		MATCH_PER_STREAM_OPT(audio_channels, i, audio_enc->channels, oc, st, int);

		MATCH_PER_STREAM_OPT(sample_fmts, str, sample_fmt, oc, st, char*);
		if (sample_fmt &&
			(audio_enc->sample_fmt = av_get_sample_fmt(sample_fmt)) == AV_SAMPLE_FMT_NONE) {
			av_log(NULL, AV_LOG_FATAL, "Invalid sample format '%s'\n", sample_fmt);
			exit_program(1);
		}

		MATCH_PER_STREAM_OPT(audio_sample_rate, i, audio_enc->sample_rate, oc, st, int);

		MATCH_PER_STREAM_OPT(apad, str, ost->apad, oc, st, char*);
		ost->apad = av_strdup(ost->apad);

		ost->avfilter = get_ost_filters(o, oc, ost);
		if (!ost->avfilter)
			exit_program(1);

		/* check for channel mapping for this audio stream */
		for (n = 0; n < o->nb_audio_channel_maps; n++) {
			AudioChannelMap *map = &o->audio_channel_maps[n];
			if ((map->ofile_idx == -1 || ost->file_index == map->ofile_idx) &&
				(map->ostream_idx == -1 || ost->st->index == map->ostream_idx)) {
				InputStream *ist;

				if (map->channel_idx == -1) {
					ist = NULL;
				}
				else if (ost->source_index < 0) {
					av_log(NULL, AV_LOG_FATAL, "Cannot determine input stream for channel mapping %d.%d\n",
						ost->file_index, ost->st->index);
					continue;
				}
				else {
					ist = input_streams[ost->source_index];
				}

				if (!ist || (ist->file_index == map->file_idx && ist->st->index == map->stream_idx)) {
					if (av_reallocp_array(&ost->audio_channels_map,
						ost->audio_channels_mapped + 1,
						sizeof(*ost->audio_channels_map)
						) < 0)
						exit_program(1);

					ost->audio_channels_map[ost->audio_channels_mapped++] = map->channel_idx;
				}
			}
		}
	}

	if (ost->stream_copy)
		check_streamcopy_filters(o, oc, ost, AVMEDIA_TYPE_AUDIO);

	return ost;
}

OutputStream *FfmpegConvert::new_data_stream(OptionsContext *o, AVFormatContext *oc, int source_index)
{
	OutputStream *ost;

	ost = new_output_stream(o, oc, AVMEDIA_TYPE_DATA, source_index);
	if (!ost->stream_copy) {
		av_log(NULL, AV_LOG_FATAL, "Data stream encoding not supported yet (only streamcopy)\n");
		exit_program(1);
	}

	return ost;
}

OutputStream *FfmpegConvert::new_unknown_stream(OptionsContext *o, AVFormatContext *oc, int source_index)
{
	OutputStream *ost;

	ost = new_output_stream(o, oc, AVMEDIA_TYPE_UNKNOWN, source_index);
	if (!ost->stream_copy) {
		av_log(NULL, AV_LOG_FATAL, "Unknown stream encoding not supported yet (only streamcopy)\n");
		exit_program(1);
	}

	return ost;
}

OutputStream *FfmpegConvert::new_attachment_stream(OptionsContext *o, AVFormatContext *oc, int source_index)
{
	OutputStream *ost = new_output_stream(o, oc, AVMEDIA_TYPE_ATTACHMENT, source_index);
	ost->stream_copy = 1;
	ost->finished = 1;
	return ost;
}

OutputStream *FfmpegConvert::new_subtitle_stream(OptionsContext *o, AVFormatContext *oc, int source_index)
{
	AVStream *st;
	OutputStream *ost;
	AVCodecContext *subtitle_enc;

	ost = new_output_stream(o, oc, AVMEDIA_TYPE_SUBTITLE, source_index);
	st = ost->st;
	subtitle_enc = ost->enc_ctx;

	subtitle_enc->codec_type = AVMEDIA_TYPE_SUBTITLE;

	MATCH_PER_STREAM_OPT(copy_initial_nonkeyframes, i, ost->copy_initial_nonkeyframes, oc, st, int);

	if (!ost->stream_copy) {
		char *frame_size = NULL;

		MATCH_PER_STREAM_OPT(frame_sizes, str, frame_size, oc, st, char*);
		if (frame_size && av_parse_video_size(&subtitle_enc->width, &subtitle_enc->height, frame_size) < 0) {
			av_log(NULL, AV_LOG_FATAL, "Invalid frame size: %s.\n", frame_size);
			exit_program(1);
		}
	}

	return ost;
}


int FfmpegConvert::copy_chapters(InputFile *ifile, OutputFile *ofile, int copy_metadata)
{
	AVFormatContext *is = ifile->ctx;
	AVFormatContext *os = ofile->ctx;
	AVChapter **tmp;
	int i;

	tmp = (AVChapter **)av_realloc_f(os->chapters, is->nb_chapters + os->nb_chapters, sizeof(*os->chapters));
	os->chapters = tmp;

	for (i = 0; i < is->nb_chapters; i++) {
		AVChapter *in_ch = is->chapters[i], *out_ch;
		int64_t start_time = (ofile->start_time == AV_NOPTS_VALUE) ? 0 : ofile->start_time;
		int64_t ts_off = av_rescale_q(start_time - ifile->ts_offset,
			AV_TIME_BASE_Q, in_ch->time_base);
		int64_t rt = (ofile->recording_time == INT64_MAX) ? INT64_MAX :
			av_rescale_q(ofile->recording_time, AV_TIME_BASE_Q, in_ch->time_base);


		if (in_ch->end < ts_off)
			continue;
		if (rt != INT64_MAX && in_ch->start > rt + ts_off)
			break;

		out_ch = (AVChapter *)av_mallocz(sizeof(AVChapter));
		out_ch->id = in_ch->id;
		out_ch->time_base = in_ch->time_base;
		out_ch->start = FFMAX(0, in_ch->start - ts_off);
		out_ch->end = FFMIN(rt, in_ch->end - ts_off);

		if (copy_metadata)
			av_dict_copy(&out_ch->metadata, in_ch->metadata, 0);

		os->chapters[os->nb_chapters++] = out_ch;
	}
	return 0;
}

void FfmpegConvert::init_output_filter(OutputFilter *ofilter, OptionsContext *o,
	AVFormatContext *oc)
{
	OutputStream *ost;

	switch (ofilter->type) {
	case AVMEDIA_TYPE_VIDEO: ost = new_video_stream(o, oc, -1); break;
	case AVMEDIA_TYPE_AUDIO: ost = new_audio_stream(o, oc, -1); break;
	default:
		av_log(NULL, AV_LOG_FATAL, "Only video and audio filters are supported "
			"currently.\n");
		exit_program(1);
	}

	ost->source_index = -1;
	ost->filter = ofilter;

	ofilter->ost = ost;
	ofilter->format = -1;

	if (ost->stream_copy) {
		av_log(NULL, AV_LOG_ERROR, "Streamcopy requested for output stream %d:%d, "
			"which is fed from a complex filtergraph. Filtering and streamcopy "
			"cannot be used together.\n", ost->file_index, ost->index);
		exit_program(1);
	}

	if (ost->avfilter && (ost->filters || ost->filters_script)) {
		const char *opt = ost->filters ? "-vf/-af/-filter" : "-filter_script";
		av_log(NULL, AV_LOG_ERROR,
			"%s '%s' was specified through the %s option "
			"for output stream %d:%d, which is fed from a complex filtergraph.\n"
			"%s and -filter_complex cannot be used together for the same stream.\n",
			ost->filters ? "Filtergraph" : "Filtergraph script",
			ost->filters ? ost->filters : ost->filters_script,
			opt, ost->file_index, ost->index, opt);
		exit_program(1);
	}

	avfilter_inout_free(&ofilter->out_tmp);
}

int FfmpegConvert::init_complex_filters(void)
{
	int i, ret = 0;

	for (i = 0; i < nb_filtergraphs; i++) {
		ret = init_complex_filtergraph(filtergraphs[i]);
		if (ret < 0)
			return ret;
	}
	return 0;
}



int FfmpegConvert::open_files(OptionGroupList *l, const char *inout,
	int(*open_file)(FfmpegConvert*, OptionsContext*, const char*))
{
	int i, ret;

	for (i = 0; i < l->nb_groups; i++) {
		OptionGroup *g = &l->groups[i];
		OptionsContext o;

		init_options(&o);
		o.g = g;
		o.m_owner = this;

		ret = parse_optgroup(&o, g);
		if (ret < 0) {
			return ret;
		}

		ret = open_file(this, &o, g->arg);
		uninit_options(&o);
		if (ret < 0) {
			av_log(NULL, AV_LOG_ERROR, "Error opening %s file %s.\n",
				inout, g->arg);
			return ret;
		}
		av_log(NULL, AV_LOG_DEBUG, "Successfully opened the file.\n");
	}

	return 0;
}

int FfmpegConvert::ffmpeg_parse_options(int argc, char **argv)
{
	OptionParseContext octx;
	int ret;
	memset(&octx, 0, sizeof(octx));
	memset(&octx.global_opts, 0, sizeof(octx.global_opts));

	/* split the commandline into an internal representation */
	ret = split_commandline(&octx, argc, argv, options, url_groups, 2);
	if (ret < 0) {
		av_log(NULL, AV_LOG_FATAL, "Error splitting the argument list: ");
		goto fail;
	}

	/* apply global options */
	ret = parse_optgroup(NULL, &octx.global_opts);
	if (ret < 0) {
		av_log(NULL, AV_LOG_FATAL, "Error parsing global options: ");
		goto fail;
	}

	/* open input files */
	ret = open_files(&octx.groups[GROUP_INFILE], "input", open_input_file);
	if (ret < 0) {
		av_log(NULL, AV_LOG_FATAL, "Error opening input files: ");
		goto fail;
	}

	/* create the complex filtergraphs */
	ret = init_complex_filters();
	if (ret < 0) {
		av_log(NULL, AV_LOG_FATAL, "Error initializing complex filters.\n");
		goto fail;
	}

	/* open output files */
	ret = open_files(&octx.groups[GROUP_OUTFILE], "output", open_output_file);
	if (ret < 0) {
		av_log(NULL, AV_LOG_FATAL, "Error opening output files: ");
		goto fail;
	}

	check_filter_outputs();

fail:
	uninit_parse_context(&octx);
	return ret;
}
