#ifndef WX__FfmpegClass__H
#define WX__FfmpegClass__H
#include "FfmpegConvert.h"

//类声明， 静态函数 ，
class  FfmpegConvert :public WXLocker
{
public:
	FfmpegConvert() {}
	virtual ~FfmpegConvert();
	int     avffmpeg_state;// = FFMPEG_ERROR_OK;//未处理
						   //	WXFfmpegOnEvent avffmpeg_onEvent;// = NULL;
						   //	WXCTSTR avffmpeg_event_id;// = 0;

	int64_t m_ptsCurr = 0;//已经处理的时间戳
	int64_t m_ptsTotal;//要处理的总时间
	int m_iVideoMode = 1; //H264编码质量 0 Fast 1 Normal 2 Best

	int  m_error = 0;
	AVFrame *m_newFrame = nullptr;



public:

	int64_t GetCurrTime();

	int64_t GetTotalTime();

	int GetState();
public:
	void Break();

	void exit_program(int ret);

	int Convert(int argc, char **argv);

public:
	float audio_drift_threshold = 0.1;
	float dts_delta_threshold = 10;
	float dts_error_threshold = 3600 * 30;

	int audio_volume = 256;
	int audio_sync_method = 0;
	int video_sync_method = VSYNC_AUTO;
	float frame_drop_threshold = 0;
	int do_deinterlace = 0;
	int copy_ts = 0;
	int start_at_zero = 0;
	int copy_tb = -1;
	int frame_bits_per_raw_sample = 0;
	float max_error_rate = 2.0 / 3;
	int filter_nbthreads = 0;
	int filter_complex_nbthreads = 0;

	int input_sync;
	int input_stream_potentially_available = 0;
	int ignore_unknown_streams = 0;
	int copy_unknown_streams = 0;
	int find_stream_info = 1;


	uint8_t *subtitle_out = NULL;

	InputStream **input_streams = NULL;
	int        nb_input_streams = 0;
	InputFile   **input_files = NULL;
	int        nb_input_files = 0;

	OutputStream **output_streams = NULL;
	int         nb_output_streams = 0;
	OutputFile   **output_files = NULL;
	int         nb_output_files = 0;

	FilterGraph **filtergraphs = NULL;
	int        nb_filtergraphs = NULL;

	AVDictionary *sws_dict = NULL;
	AVDictionary *swr_opts = NULL;
	AVDictionary *format_opts = NULL;
	AVDictionary *codec_opts = NULL;
	AVDictionary *resample_opts = NULL;

	volatile int received_sigterm = 0;
	volatile int received_nb_signals = 0;

private:
	onFfmpegVideoData m_cbVideo = nullptr;
	void *avffmpeg_owner = nullptr;// = NULL;
public:

	void SetFfmpegVideoData(onFfmpegVideoData cb)
	{
		m_cbVideo = cb;
	}

	void SetFfmpegOwner(void* pOwner)
	{
		avffmpeg_owner = pOwner;
	}



	inline void prepare_app_arguments(int *argc_ptr, char ***argv_ptr)
	{
		/* nothing to do */
	}

	void init_dynload(void);



	double parse_number_or_die(const char *context, const char *numstr, int type,
		double min, double max);

	int64_t parse_time_or_die(const char *context, const char *timestr,
		int is_duration);

	const OptionDef *find_option(const OptionDef *po, const char *name);


	int write_option(void *optctx, const OptionDef *po, const char *opt,
		const char *arg);
		
	void init_input_filter(FilterGraph *fg, AVFilterInOut *in);

	int parse_option(void *optctx, const char *opt, const char *arg,
		const OptionDef *options);

	AVDictionary *strip_specifiers(AVDictionary *dict);

	int get_input_packet(InputFile *f, AVPacket *pkt);

	void parse_options(void *optctx, int argc, char **argv, const OptionDef *options,
		void(*parse_arg_function)(void *, const char*));

	int parse_optgroup(void *optctx, OptionGroup *g);


	void check_options(const OptionDef *po);

	const AVOption *opt_find(void *obj, const char *name, const char *unit,
		int opt_flags, int search_flags);

	int match_group_separator(const OptionGroupDef *groups, int nb_groups,
		const char *opt);
	void finish_group(OptionParseContext *octx, int group_idx,
		const char *arg);

	void add_opt(OptionParseContext *octx, const OptionDef *opt,
		const char *key, const char *val);

	void init_parse_context(OptionParseContext *octx,
		const OptionGroupDef *groups, int nb_groups);

	void uninit_parse_context(OptionParseContext *octx);
	
	

	int split_commandline(OptionParseContext *octx, int argc, char *argv[],
		const OptionDef *options,
		const OptionGroupDef *groups, int nb_groups);



	char get_media_type_char(enum AVMediaType type);

	const AVCodec *next_codec_for_id(enum AVCodecID id, const AVCodec *prev,
		int encoder);

	unsigned get_codecs_sorted(const AVCodecDescriptor ***rcodecs);

	FILE *get_preset_file(char *filename, size_t filename_size,
		const char *preset_name, int is_path,
		const char *codec_name);

	int check_stream_specifier(AVFormatContext *s, AVStream *st, const char *spec);
	AVDictionary *filter_codec_opts(AVDictionary *opts, enum AVCodecID codec_id,
		AVFormatContext *s, AVStream *st, AVCodec *codec);

	AVDictionary **setup_find_stream_info_opts(AVFormatContext *s,
		AVDictionary *codec_opts);

	void *grow_array(void *array, int elem_size, int *size, int new_size);

	double get_rotation(AVStream *st);

	int sub2video_get_blank_frame(InputStream *ist);

	void sub2video_copy_rect(uint8_t *dst, int dst_linesize, int w, int h,
		AVSubtitleRect *r);

	void sub2video_push_ref(InputStream *ist, int64_t pts);

	void sub2video_update(InputStream *ist, AVSubtitle *sub);
	

	void sub2video_heartbeat(InputStream *ist, int64_t pts);

	void sub2video_flush(InputStream *ist);

	void ffmpeg_cleanup(int ret);
		
	

	
	
	void remove_avoptions(AVDictionary **a, AVDictionary *b);
	void assert_avoptions(AVDictionary *a);

	void abort_codec_experimental(AVCodec *c, int encoder);

	void close_all_output_streams(OutputStream *ost, OSTFinished this_stream, OSTFinished others);

	void write_packet(OutputFile *of, AVPacket *pkt, OutputStream *ost, int unqueue);

	void close_output_stream(OutputStream *ost);

	void output_packet(OutputFile *of, AVPacket *pkt,
		OutputStream *ost, int eof);

	int check_recording_time(OutputStream *ost);

	void do_audio_out(OutputFile *of, OutputStream *ost,
		AVFrame *frame);

	void do_subtitle_out(OutputFile *of,
		OutputStream *ost,
		AVSubtitle *sub);

			

	void do_video_out(OutputFile *of,
		OutputStream *ost,
		AVFrame *next_picture,
		double sync_ipts);
			
			

	void finish_output_stream(OutputStream *ost);

	int reap_filters(int flush);

	void flush_encoders(void);

	int check_output_constraints(InputStream *ist, OutputStream *ost);

	void do_streamcopy(InputStream *ist, OutputStream *ost, const AVPacket *pkt);
	

	

	int guess_input_channel_layout(InputStream *ist);

	// Filters can be configured only if the formats of all inputs are known.
	int ifilter_has_all_input_formats(FilterGraph *fg);

	int ifilter_send_frame(InputFilter *ifilter, AVFrame *frame);

	int ifilter_send_eof(InputFilter *ifilter, int64_t pts);

	int decode(AVCodecContext *avctx, AVFrame *frame, int *got_frame, AVPacket *pkt);

	int send_frame_to_filters(InputStream *ist, AVFrame *decoded_frame);

	int decode_audio(InputStream *ist, AVPacket *pkt, int *got_output,
		int *decode_failed);

	int decode_video(InputStream *ist, AVPacket *pkt, int *got_output, int64_t *duration_pts, int eof,
		int *decode_failed);

	int transcode_subtitles(InputStream *ist, AVPacket *pkt, int *got_output,
		int *decode_failed);

	int send_filter_eof(InputStream *ist);

	/* pkt = NULL means EOF (needed to flush decoder buffers) */
	int process_input_packet(InputStream *ist, const AVPacket *pkt, int no_eof);

	int init_input_stream(int ist_index, char *error, int error_len);

	InputStream *get_input_stream(OutputStream *ost);

	/* open the muxer when all the streams are initialized */
	int check_init_output_file(OutputFile *of, int file_index);

	int init_output_bsfs(OutputStream *ost);

	int init_output_stream_streamcopy(OutputStream *ost);

	void set_encoder_id(OutputFile *of, OutputStream *ost);

	void parse_forced_key_frames(char *kf, OutputStream *ost,
		AVCodecContext *avctx);

	void init_encoder_time_base(OutputStream *ost, AVRational default_time_base);

	int init_output_stream_encode(OutputStream *ost);


	int init_output_stream(OutputStream *ost, char *error, int error_len);

	void report_new_stream(int input_index, AVPacket *pkt);

	int transcode_init(void);


	/* Return 1 if there remain streams where more output is wanted, 0 otherwise. */
	int need_output(void);
		

	OutputStream *choose_output(void);


	void *input_thread(void *arg);

	void free_input_threads(void);

	int init_input_threads(void);

	int got_eagain(void);

	void reset_eagain(void);

	// set duration to max(tmp, duration) in a proper time base and return duration's time_base
	AVRational duration_max(int64_t tmp, int64_t *duration, AVRational tmp_time_base,
		AVRational time_base);

	int seek_to_start(InputFile *ifile, AVFormatContext *is);

	int process_input(int file_index);

	int transcode_from_filter(FilterGraph *graph, InputStream **best_ist);
		
	int transcode_step(void);


	int transcode(void);

	const enum AVPixelFormat *get_compliance_unofficial_pix_fmts(enum AVCodecID codec_id, const enum AVPixelFormat default_formats[]);

	enum AVPixelFormat choose_pixel_fmt(AVStream *st, AVCodecContext *enc_ctx, AVCodec *codec, enum AVPixelFormat target);

	void choose_sample_fmt(AVStream *st, AVCodec *codec);

	char *choose_pix_fmts(OutputFilter *ofilter);
		

	char *choose_sample_fmts(OutputFilter *ofilter);

	char *choose_sample_rates(OutputFilter *ofilter);

	char *choose_channel_layouts(OutputFilter *ofilter);

	int init_simple_filtergraph(InputStream *ist, OutputStream *ost);

	char *describe_filter_link(FilterGraph *fg, AVFilterInOut *inout, int in);

	int init_complex_filtergraph(FilterGraph *fg);

	int insert_trim(int64_t start_time, int64_t duration,
		AVFilterContext **last_filter, int *pad_idx,
		const char *filter_name);

	int insert_filter(AVFilterContext **last_filter, int *pad_idx,
		const char *filter_name, const char *args);

	int configure_output_video_filter(FilterGraph *fg, OutputFilter *ofilter, AVFilterInOut *out);
	

	int configure_output_audio_filter(FilterGraph *fg, OutputFilter *ofilter, AVFilterInOut *out);
	

	int configure_output_filter(FilterGraph *fg, OutputFilter *ofilter, AVFilterInOut *out);

	void check_filter_outputs(void);

	int sub2video_prepare(InputStream *ist, InputFilter *ifilter);

	int configure_input_video_filter(FilterGraph *fg, InputFilter *ifilter,
		AVFilterInOut *in);

	int configure_input_audio_filter(FilterGraph *fg, InputFilter *ifilter,
		AVFilterInOut *in);

	int configure_input_filter(FilterGraph *fg, InputFilter *ifilter,
		AVFilterInOut *in);

	void cleanup_filtergraph(FilterGraph *fg);

	int configure_filtergraph(FilterGraph *fg);

	int ifilter_parameters_from_frame(InputFilter *ifilter, const AVFrame *frame);

	int ist_in_filtergraph(FilterGraph *fg, InputStream *ist);

	int filtergraph_is_simple(FilterGraph *fg);

	void uninit_options(OptionsContext *o);
	void init_options(OptionsContext *o);

	void parse_meta_type(char *arg, char *type, int *index, const char **stream_spec);

	int copy_metadata(char *outspec, char *inspec, AVFormatContext *oc, AVFormatContext *ic, OptionsContext *o);

	AVCodec *find_codec_or_die(const char *name, enum AVMediaType type, int encoder);

	AVCodec *choose_decoder(OptionsContext *o, AVFormatContext *s, AVStream *st);

	/* Add all the streams from the given input file to the global
	* list of input streams. */
	void add_input_streams(OptionsContext *o, AVFormatContext *ic);


	void dump_attachment(AVStream *st, const char *filename);

	uint8_t *get_line(AVIOContext *s);

	int get_preset_file_2(const char *preset_name, const char *codec_name, AVIOContext **s);

	int choose_encoder(OptionsContext *o, AVFormatContext *s, OutputStream *ost);

	OutputStream *new_output_stream(OptionsContext *o, AVFormatContext *oc, enum AVMediaType type, int source_index);

	void parse_matrix_coeffs(uint16_t *dest, const char *str);
	/* read file contents into a string */
	uint8_t *read_file(const char *filename);

	char *get_ost_filters(OptionsContext *o, AVFormatContext *oc, OutputStream *ost);


	void check_streamcopy_filters(OptionsContext *o, AVFormatContext *oc, const OutputStream *ost, enum AVMediaType type);


	OutputStream *new_video_stream(OptionsContext *o, AVFormatContext *oc, int source_index);

	OutputStream *new_audio_stream(OptionsContext *o, AVFormatContext *oc, int source_index);

	OutputStream *new_data_stream(OptionsContext *o, AVFormatContext *oc, int source_index);

	OutputStream *new_unknown_stream(OptionsContext *o, AVFormatContext *oc, int source_index);

	OutputStream *new_attachment_stream(OptionsContext *o, AVFormatContext *oc, int source_index);

	OutputStream *new_subtitle_stream(OptionsContext *o, AVFormatContext *oc, int source_index);

	int copy_chapters(InputFile *ifile, OutputFile *ofile, int copy_metadata);

	void init_output_filter(OutputFilter *ofilter, OptionsContext *o, AVFormatContext *oc);
	

	int init_complex_filters(void);

	int open_files(OptionGroupList *l, const char *inout, int(*open_file)(FfmpegConvert*, OptionsContext*, const char*));

	int ffmpeg_parse_options(int argc, char **argv);

















	static int opt_video_channel(FfmpegConvert* ctx, void *optctx, const char *opt, const char *arg)
	{
		av_log(NULL, AV_LOG_WARNING, "This option is deprecated, use -channel.\n");
		return opt_default(ctx, optctx, "channel", arg);
	}

	static int opt_video_standard(FfmpegConvert* ctx, void *optctx, const char *opt, const char *arg)
	{
		av_log(NULL, AV_LOG_WARNING, "This option is deprecated, use -standard.\n");
		return opt_default(ctx, optctx, "standard", arg);
	}

	static int opt_audio_codec(FfmpegConvert* ctx, void *optctx, const char *opt, const char *arg)
	{
		OptionsContext *o = (OptionsContext *)optctx;
		return ctx->parse_option(o, "codec:a", arg, ctx->options);
	}

	static int opt_video_codec(FfmpegConvert* ctx, void *optctx, const char *opt, const char *arg)
	{
		OptionsContext *o = (OptionsContext *)optctx;
		return ctx->parse_option(o, "codec:v", arg, ctx->options);
	}

	static int opt_subtitle_codec(FfmpegConvert* ctx, void *optctx, const char *opt, const char *arg)
	{
		OptionsContext *o = (OptionsContext *)optctx;
		return ctx->parse_option(o, "codec:s", arg, ctx->options);
	}

	static int opt_data_codec(FfmpegConvert* ctx, void *optctx, const char *opt, const char *arg)
	{
		OptionsContext *o = (OptionsContext *)optctx;
		return ctx->parse_option(o, "codec:d", arg, ctx->options);
	}

	static int opt_map(FfmpegConvert* ctx, void *optctx, const char *opt, const char *arg)
	{
		OptionsContext *o = (OptionsContext *)optctx;
		StreamMap *m = NULL;
		int i, negative = 0, file_idx;
		int sync_file_idx = -1, sync_stream_idx = 0;
		char *p, *sync;
		char *map;
		char *allow_unused;

		if (*arg == '-') {
			negative = 1;
			arg++;
		}
		map = av_strdup(arg);
		if (!map)
			return AVERROR(ENOMEM);

		/* parse sync stream first, just pick first matching stream */
		if (sync = strchr(map, ',')) {
			*sync = 0;
			sync_file_idx = strtol(sync + 1, &sync, 0);
			if (sync_file_idx >= ctx->nb_input_files || sync_file_idx < 0) {
				av_log(NULL, AV_LOG_FATAL, "Invalid sync file index: %d.\n", sync_file_idx);
				ctx->exit_program(1);
			}
			if (*sync)
				sync++;
			for (i = 0; i < ctx->input_files[sync_file_idx]->nb_streams; i++)
				if (ctx->check_stream_specifier(ctx->input_files[sync_file_idx]->ctx,
					ctx->input_files[sync_file_idx]->ctx->streams[i], sync) == 1) {
					sync_stream_idx = i;
					break;
				}
			if (i == ctx->input_files[sync_file_idx]->nb_streams) {
				av_log(NULL, AV_LOG_FATAL, "Sync stream specification in map %s does not "
					"match any streams.\n", arg);
				ctx->exit_program(1);
			}
		}


		if (map[0] == '[') {
			/* this mapping refers to lavfi output */
			const char *c = map + 1;
			GROW_ARRAY_STATIC(o->stream_maps, o->nb_stream_maps, StreamMap*);
			m = &o->stream_maps[o->nb_stream_maps - 1];
			m->linklabel = av_get_token(&c, "]");
			if (!m->linklabel) {
				av_log(NULL, AV_LOG_ERROR, "Invalid output link label: %s.\n", map);
				ctx->exit_program(1);
			}
		}
		else {
			if (allow_unused = strchr(map, '?'))
				*allow_unused = 0;
			file_idx = strtol(map, &p, 0);
			if (file_idx >= ctx->nb_input_files || file_idx < 0) {
				av_log(NULL, AV_LOG_FATAL, "Invalid input file index: %d.\n", file_idx);
				ctx->exit_program(1);
			}
			if (negative)
				/* disable some already defined maps */
				for (i = 0; i < o->nb_stream_maps; i++) {
					m = &o->stream_maps[i];
					if (file_idx == m->file_index &&
						ctx->check_stream_specifier(ctx->input_files[m->file_index]->ctx,
							ctx->input_files[m->file_index]->ctx->streams[m->stream_index],
							*p == ':' ? p + 1 : p) > 0)
						m->disabled = 1;
				}
			else
				for (i = 0; i < ctx->input_files[file_idx]->nb_streams; i++) {
					if (ctx->check_stream_specifier(ctx->input_files[file_idx]->ctx, ctx->input_files[file_idx]->ctx->streams[i],
						*p == ':' ? p + 1 : p) <= 0)
						continue;
					GROW_ARRAY_STATIC(o->stream_maps, o->nb_stream_maps, StreamMap*);
					m = &o->stream_maps[o->nb_stream_maps - 1];

					m->file_index = file_idx;
					m->stream_index = i;

					if (sync_file_idx >= 0) {
						m->sync_file_index = sync_file_idx;
						m->sync_stream_index = sync_stream_idx;
					}
					else {
						m->sync_file_index = file_idx;
						m->sync_stream_index = i;
					}
				}
		}

		if (!m) {
			if (allow_unused) {
				av_log(NULL, AV_LOG_VERBOSE, "Stream map '%s' matches no streams; ignoring.\n", arg);
			}
			else {
				av_log(NULL, AV_LOG_FATAL, "Stream map '%s' matches no streams.\n"
					"To ignore this, add a trailing '?' to the map.\n", arg);
				ctx->exit_program(1);
			}
		}

		av_freep(&map);
		return 0;
	}

	static int opt_attach(FfmpegConvert* ctx, void *optctx, const char *opt, const char *arg)
	{
		OptionsContext *o = (OptionsContext *)optctx;
		GROW_ARRAY_STATIC(o->attachments, o->nb_attachments, const char**);
		o->attachments[o->nb_attachments - 1] = arg;
		return 0;
	}

	static int opt_map_channel(FfmpegConvert* ctx, void *optctx, const char *opt, const char *arg)
	{
		OptionsContext *o = (OptionsContext *)optctx;
		int n;
		AVStream *st;
		AudioChannelMap *m;
		char *allow_unused;
		char *mapchan;
		mapchan = av_strdup(arg);
		if (!mapchan)
			return AVERROR(ENOMEM);

		GROW_ARRAY_STATIC(o->audio_channel_maps, o->nb_audio_channel_maps, AudioChannelMap *);
		m = &o->audio_channel_maps[o->nb_audio_channel_maps - 1];

		/* muted channel syntax */
		n = sscanf(arg, "%d:%d.%d", &m->channel_idx, &m->ofile_idx, &m->ostream_idx);
		if ((n == 1 || n == 3) && m->channel_idx == -1) {
			m->file_idx = m->stream_idx = -1;
			if (n == 1)
				m->ofile_idx = m->ostream_idx = -1;
			av_free(mapchan);
			return 0;
		}

		/* normal syntax */
		n = sscanf(arg, "%d.%d.%d:%d.%d",
			&m->file_idx, &m->stream_idx, &m->channel_idx,
			&m->ofile_idx, &m->ostream_idx);

		if (n != 3 && n != 5) {
			av_log(NULL, AV_LOG_FATAL, "Syntax error, mapchan usage: "
				"[file.stream.channel|-1][:syncfile:syncstream]\n");
			ctx->exit_program(1);
		}

		if (n != 5) // only file.stream.channel specified
			m->ofile_idx = m->ostream_idx = -1;

		/* check input */
		if (m->file_idx < 0 || m->file_idx >= ctx->nb_input_files) {
			av_log(NULL, AV_LOG_FATAL, "mapchan: invalid input file index: %d\n",
				m->file_idx);
			ctx->exit_program(1);
		}
		if (m->stream_idx < 0 ||
			m->stream_idx >= ctx->input_files[m->file_idx]->nb_streams) {
			av_log(NULL, AV_LOG_FATAL, "mapchan: invalid input file stream index #%d.%d\n",
				m->file_idx, m->stream_idx);
			ctx->exit_program(1);
		}
		st = ctx->input_files[m->file_idx]->ctx->streams[m->stream_idx];
		if (st->codecpar->codec_type != AVMEDIA_TYPE_AUDIO) {
			av_log(NULL, AV_LOG_FATAL, "mapchan: stream #%d.%d is not an audio stream.\n",
				m->file_idx, m->stream_idx);
			ctx->exit_program(1);
		}
		/* allow trailing ? to map_channel */
		if (allow_unused = strchr(mapchan, '?'))
			*allow_unused = 0;
		if (m->channel_idx < 0 || m->channel_idx >= st->codecpar->channels) {
			if (allow_unused) {
				av_log(NULL, AV_LOG_VERBOSE, "mapchan: invalid audio channel #%d.%d.%d\n",
					m->file_idx, m->stream_idx, m->channel_idx);
			}
			else {
				av_log(NULL, AV_LOG_FATAL, "mapchan: invalid audio channel #%d.%d.%d\n"
					"To ignore this, add a trailing '?' to the map_channel.\n",
					m->file_idx, m->stream_idx, m->channel_idx);
				ctx->exit_program(1);
			}

		}
		av_free(mapchan);
		return 0;
	}

	static int open_output_file(FfmpegConvert* ctx, OptionsContext *o, const char *filename)
	{
		AVFormatContext *oc;
		int i, j, err;
		OutputFile *of;
		OutputStream *ost;
		InputStream  *ist;
		AVDictionary *unused_opts = NULL;
		AVDictionaryEntry *e = NULL;
		int format_flags = 0;

		if (o->stop_time != INT64_MAX && o->recording_time != INT64_MAX) {
			o->stop_time = INT64_MAX;
			av_log(NULL, AV_LOG_WARNING, "-t and -to cannot be used together; using -t.\n");
		}

		if (o->stop_time != INT64_MAX && o->recording_time == INT64_MAX) {
			int64_t start_time = o->start_time == AV_NOPTS_VALUE ? 0 : o->start_time;
			if (o->stop_time <= start_time) {
				av_log(NULL, AV_LOG_ERROR, "-to value smaller than -ss; aborting.\n");
				ctx->exit_program(1);
			}
			else {
				o->recording_time = o->stop_time - start_time;
			}
		}

		GROW_ARRAY_STATIC(ctx->output_files, ctx->nb_output_files, OutputFile**);
		of = (OutputFile *)av_mallocz(sizeof(*of));
		ctx->output_files[ctx->nb_output_files - 1] = of;

		of->ost_index = ctx->nb_output_streams;
		of->recording_time = o->recording_time;
		of->start_time = o->start_time;
		of->limit_filesize = o->limit_filesize;
		of->shortest = o->shortest;
		av_dict_copy(&of->opts, o->g->format_opts, 0);

		if (!strcmp(filename, "-"))
			filename = "pipe:";

		err = avformat_alloc_output_context2(&oc, NULL, o->format, filename);
		if (!oc) {
			ctx->exit_program(1);
		}

		of->ctx = oc;
		if (o->recording_time != INT64_MAX)
			oc->duration = o->recording_time;

		oc->interrupt_callback = int_cb;

		e = av_dict_get(o->g->format_opts, "fflags", NULL, 0);
		if (e) {
			const AVOption *o = av_opt_find(oc, "fflags", NULL, 0, 0);
			av_opt_eval_flags(oc, o, e->value, &format_flags);
		}
		if (o->bitexact) {
			format_flags |= AVFMT_FLAG_BITEXACT;
			oc->flags |= AVFMT_FLAG_BITEXACT;
		}

		/* create streams for all unlabeled output pads */
		for (i = 0; i < ctx->nb_filtergraphs; i++) {
			FilterGraph *fg = ctx->filtergraphs[i];
			for (j = 0; j < fg->nb_outputs; j++) {
				OutputFilter *ofilter = fg->outputs[j];

				if (!ofilter->out_tmp || ofilter->out_tmp->name)
					continue;

				switch (ofilter->type) {
				case AVMEDIA_TYPE_VIDEO:    o->video_disable = 1; break;
				case AVMEDIA_TYPE_AUDIO:    o->audio_disable = 1; break;
				case AVMEDIA_TYPE_SUBTITLE: o->subtitle_disable = 1; break;
				}
				ctx->init_output_filter(ofilter, o, oc);
			}
		}

		if (!o->nb_stream_maps) {
			char *subtitle_codec_name = NULL;
			/* pick the "best" stream of each type */

			/* video: highest resolution */
			if (!o->video_disable && av_guess_codec(oc->oformat, NULL, filename, NULL, AVMEDIA_TYPE_VIDEO) != AV_CODEC_ID_NONE) {
				int area = 0, idx = -1;
				int qcr = avformat_query_codec(oc->oformat, oc->oformat->video_codec, 0);
				for (i = 0; i < ctx->nb_input_streams; i++) {
					int new_area;
					ist = ctx->input_streams[i];
					new_area = ist->st->codecpar->width * ist->st->codecpar->height + 100000000 * !!ist->st->codec_info_nb_frames;
					if ((qcr != MKTAG('A', 'P', 'I', 'C')) && (ist->st->disposition & AV_DISPOSITION_ATTACHED_PIC))
						new_area = 1;
					if (ist->st->codecpar->codec_type == AVMEDIA_TYPE_VIDEO &&
						new_area > area) {
						if ((qcr == MKTAG('A', 'P', 'I', 'C')) && !(ist->st->disposition & AV_DISPOSITION_ATTACHED_PIC))
							continue;
						area = new_area;
						idx = i;
					}
				}
				if (idx >= 0)
					ctx->new_video_stream(o, oc, idx);
			}

			/* audio: most channels */
			if (!o->audio_disable && av_guess_codec(oc->oformat, NULL, filename, NULL, AVMEDIA_TYPE_AUDIO) != AV_CODEC_ID_NONE) {
				int best_score = 0, idx = -1;
				for (i = 0; i < ctx->nb_input_streams; i++) {
					int score;
					ist = ctx->input_streams[i];
					score = ist->st->codecpar->channels + 100000000 * !!ist->st->codec_info_nb_frames;
					if (ist->st->codecpar->codec_type == AVMEDIA_TYPE_AUDIO &&
						score > best_score) {
						best_score = score;
						idx = i;
					}
				}
				if (idx >= 0)
					ctx->new_audio_stream(o, oc, idx);
			}

			/* subtitles: pick first */
			MATCH_PER_TYPE_OPT(codec_names, str, subtitle_codec_name, oc, "s", char*);
			if (!o->subtitle_disable && (avcodec_find_encoder(oc->oformat->subtitle_codec) || subtitle_codec_name)) {
				for (i = 0; i < ctx->nb_input_streams; i++)
					if (ctx->input_streams[i]->st->codecpar->codec_type == AVMEDIA_TYPE_SUBTITLE) {
						AVCodecDescriptor const *input_descriptor =
							avcodec_descriptor_get(ctx->input_streams[i]->st->codecpar->codec_id);
						AVCodecDescriptor const *output_descriptor = NULL;
						AVCodec const *output_codec =
							avcodec_find_encoder(oc->oformat->subtitle_codec);
						int input_props = 0, output_props = 0;
						if (output_codec)
							output_descriptor = avcodec_descriptor_get(output_codec->id);
						if (input_descriptor)
							input_props = input_descriptor->props & (AV_CODEC_PROP_TEXT_SUB | AV_CODEC_PROP_BITMAP_SUB);
						if (output_descriptor)
							output_props = output_descriptor->props & (AV_CODEC_PROP_TEXT_SUB | AV_CODEC_PROP_BITMAP_SUB);
						if (subtitle_codec_name ||
							input_props & output_props ||
							// Map dvb teletext which has neither property to any output subtitle encoder
							input_descriptor && output_descriptor &&
							(!input_descriptor->props ||
								!output_descriptor->props)) {
							ctx->new_subtitle_stream(o, oc, i);
							break;
						}
					}
			}
			/* Data only if codec id match */
			if (!o->data_disable) {
				enum AVCodecID codec_id = av_guess_codec(oc->oformat, NULL, filename, NULL, AVMEDIA_TYPE_DATA);
				for (i = 0; codec_id != AV_CODEC_ID_NONE && i < ctx->nb_input_streams; i++) {
					if (ctx->input_streams[i]->st->codecpar->codec_type == AVMEDIA_TYPE_DATA
						&& ctx->input_streams[i]->st->codecpar->codec_id == codec_id)
						ctx->new_data_stream(o, oc, i);
				}
			}
		}
		else {
			for (i = 0; i < o->nb_stream_maps; i++) {
				StreamMap *map = &o->stream_maps[i];

				if (map->disabled)
					continue;

				if (map->linklabel) {
					FilterGraph *fg;
					OutputFilter *ofilter = NULL;
					int j, k;

					for (j = 0; j < ctx->nb_filtergraphs; j++) {
						fg = ctx->filtergraphs[j];
						for (k = 0; k < fg->nb_outputs; k++) {
							AVFilterInOut *out = fg->outputs[k]->out_tmp;
							if (out && !strcmp(out->name, map->linklabel)) {
								ofilter = fg->outputs[k];
								goto loop_end;
							}
						}
					}
				loop_end:
					if (!ofilter) {
						av_log(NULL, AV_LOG_FATAL, "Output with label '%s' does not exist "
							"in any defined filter graph, or was already used elsewhere.\n", map->linklabel);
						ctx->exit_program(1);
					}
					ctx->init_output_filter(ofilter, o, oc);
				}
				else {
					int src_idx = ctx->input_files[map->file_index]->ist_index + map->stream_index;

					ist = ctx->input_streams[ctx->input_files[map->file_index]->ist_index + map->stream_index];
					if (o->subtitle_disable && ist->st->codecpar->codec_type == AVMEDIA_TYPE_SUBTITLE)
						continue;
					if (o->audio_disable && ist->st->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
						continue;
					if (o->video_disable && ist->st->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
						continue;
					if (o->data_disable && ist->st->codecpar->codec_type == AVMEDIA_TYPE_DATA)
						continue;

					ost = NULL;
					switch (ist->st->codecpar->codec_type) {
					case AVMEDIA_TYPE_VIDEO:      ost = ctx->new_video_stream(o, oc, src_idx); break;
					case AVMEDIA_TYPE_AUDIO:      ost = ctx->new_audio_stream(o, oc, src_idx); break;
					case AVMEDIA_TYPE_SUBTITLE:   ost = ctx->new_subtitle_stream(o, oc, src_idx); break;
					case AVMEDIA_TYPE_DATA:       ost = ctx->new_data_stream(o, oc, src_idx); break;
					case AVMEDIA_TYPE_ATTACHMENT: ost = ctx->new_attachment_stream(o, oc, src_idx); break;
					case AVMEDIA_TYPE_UNKNOWN:
						if (ctx->copy_unknown_streams) {
							ost = ctx->new_unknown_stream(o, oc, src_idx);
							break;
						}
					default:
						if (!ctx->ignore_unknown_streams) {
							ctx->exit_program(1);
						}
					}
					if (ost)
						ost->sync_ist = ctx->input_streams[ctx->input_files[map->sync_file_index]->ist_index
						+ map->sync_stream_index];
				}
			}
		}

		/* handle attached files */
		for (i = 0; i < o->nb_attachments; i++) {
			AVIOContext *pb;
			uint8_t *attachment;
			const char *p;
			int64_t len;

			if ((err = avio_open2(&pb, o->attachments[i], AVIO_FLAG_READ, &int_cb, NULL)) < 0) {
				av_log(NULL, AV_LOG_FATAL, "Could not open attachment file %s.\n",
					o->attachments[i]);
				ctx->exit_program(1);
			}
			if ((len = avio_size(pb)) <= 0) {
				av_log(NULL, AV_LOG_FATAL, "Could not get size of the attachment %s.\n",
					o->attachments[i]);
				ctx->exit_program(1);
			}
			attachment = (uint8_t *)av_malloc(len);
			avio_read(pb, attachment, len);

			ost = ctx->new_attachment_stream(o, oc, -1);
			ost->stream_copy = 0;
			ost->attachment_filename = o->attachments[i];
			ost->st->codecpar->extradata = attachment;
			ost->st->codecpar->extradata_size = len;

			p = strrchr(o->attachments[i], '/');
			av_dict_set(&ost->st->metadata, "filename", (p && *p) ? p + 1 : o->attachments[i], AV_DICT_DONT_OVERWRITE);
			avio_closep(&pb);
		}

#if FF_API_LAVF_AVCTX
		for (i = ctx->nb_output_streams - oc->nb_streams; i < ctx->nb_output_streams; i++) { //for all streams of this output file
			AVDictionaryEntry *e;
			ost = ctx->output_streams[i];

			if ((ost->stream_copy || ost->attachment_filename)
				&& (e = av_dict_get(o->g->codec_opts, "flags", NULL, AV_DICT_IGNORE_SUFFIX))
				&& (!e->key[5] || ctx->check_stream_specifier(oc, ost->st, e->key + 6)))
				if (av_opt_set(ost->st->codec, "flags", e->value, 0) < 0)
					ctx->exit_program(1);
		}
#endif

		if (!oc->nb_streams && !(oc->oformat->flags & AVFMT_NOSTREAMS)) {
			av_dump_format(oc, ctx->nb_output_files - 1, oc->url, 1);
			ctx->exit_program(1);
		}

		/* check if all codec options have been used */
		unused_opts = ctx->strip_specifiers(o->g->codec_opts);
		for (i = of->ost_index; i < ctx->nb_output_streams; i++) {
			e = NULL;
			while ((e = av_dict_get(ctx->output_streams[i]->encoder_opts, "", e,
				AV_DICT_IGNORE_SUFFIX)))
				av_dict_set(&unused_opts, e->key, NULL, 0);
		}

		e = NULL;
		while ((e = av_dict_get(unused_opts, "", e, AV_DICT_IGNORE_SUFFIX))) {
			const AVClass *_class = avcodec_get_class();
			const AVOption *option = av_opt_find(&_class, e->key, NULL, 0,
				AV_OPT_SEARCH_CHILDREN | AV_OPT_SEARCH_FAKE_OBJ);
			const AVClass *fclass = avformat_get_class();
			const AVOption *foption = av_opt_find(&fclass, e->key, NULL, 0,
				AV_OPT_SEARCH_CHILDREN | AV_OPT_SEARCH_FAKE_OBJ);
			if (!option || foption)
				continue;


			if (!(option->flags & AV_OPT_FLAG_ENCODING_PARAM)) {
				ctx->exit_program(1);
			}

			// gop_timecode is injected by generic code but not always used
			if (!strcmp(e->key, "gop_timecode"))
				continue;
		}
		av_dict_free(&unused_opts);

		/* set the decoding_needed flags and create simple filtergraphs */
		for (i = of->ost_index; i < ctx->nb_output_streams; i++) {
			OutputStream *ost = ctx->output_streams[i];

			if (ost->encoding_needed && ost->source_index >= 0) {
				InputStream *ist = ctx->input_streams[ost->source_index];
				ist->decoding_needed |= DECODING_FOR_OST;

				if (ost->st->codecpar->codec_type == AVMEDIA_TYPE_VIDEO ||
					ost->st->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
					err = ctx->init_simple_filtergraph(ist, ost);
					if (err < 0) {
						ctx->exit_program(1);
					}
				}
			}

			/* set the filter output constraints */
			if (ost->filter) {
				OutputFilter *f = ost->filter;
				int count;
				switch (ost->enc_ctx->codec_type) {
				case AVMEDIA_TYPE_VIDEO:
					f->frame_rate = ost->frame_rate;
					f->width = ost->enc_ctx->width;
					f->height = ost->enc_ctx->height;
					if (ost->enc_ctx->pix_fmt != AV_PIX_FMT_NONE) {
						f->format = ost->enc_ctx->pix_fmt;
					}
					else if (ost->enc->pix_fmts) {
						count = 0;
						while (ost->enc->pix_fmts[count] != AV_PIX_FMT_NONE)
							count++;
						f->formats = (int*)av_mallocz_array(count + 1, sizeof(*f->formats));
						memcpy(f->formats, ost->enc->pix_fmts, (count + 1) * sizeof(*f->formats));
					}
					break;
				case AVMEDIA_TYPE_AUDIO:
					if (ost->enc_ctx->sample_fmt != AV_SAMPLE_FMT_NONE) {
						f->format = ost->enc_ctx->sample_fmt;
					}
					else if (ost->enc->sample_fmts) {
						count = 0;
						while (ost->enc->sample_fmts[count] != AV_SAMPLE_FMT_NONE)
							count++;
						f->formats = (int*)av_mallocz_array(count + 1, sizeof(*f->formats));
						memcpy(f->formats, ost->enc->sample_fmts, (count + 1) * sizeof(*f->formats));
					}
					if (ost->enc_ctx->sample_rate) {
						f->sample_rate = ost->enc_ctx->sample_rate;
					}
					else if (ost->enc->supported_samplerates) {
						count = 0;
						while (ost->enc->supported_samplerates[count])
							count++;
						f->sample_rates = (int*)av_mallocz_array(count + 1, sizeof(*f->sample_rates));
						memcpy(f->sample_rates, ost->enc->supported_samplerates,
							(count + 1) * sizeof(*f->sample_rates));
					}
					if (ost->enc_ctx->channels) {
						f->channel_layout = av_get_default_channel_layout(ost->enc_ctx->channels);
					}
					else if (ost->enc->channel_layouts) {
						count = 0;
						while (ost->enc->channel_layouts[count])
							count++;
						f->channel_layouts = (uint64_t*)av_mallocz_array(count + 1, sizeof(*f->channel_layouts));
						memcpy(f->channel_layouts, ost->enc->channel_layouts,
							(count + 1) * sizeof(*f->channel_layouts));
					}
					break;
				}
			}
		}

		/* check filename in case of an image number is expected */
		if (oc->oformat->flags & AVFMT_NEEDNUMBER) {
			if (!av_filename_number_test(oc->url)) {
				ctx->exit_program(1);
			}
		}

		if (!(oc->oformat->flags & AVFMT_NOSTREAMS) && !ctx->input_stream_potentially_available) {
			ctx->exit_program(1);
		}

		if (!(oc->oformat->flags & AVFMT_NOFILE)) {

			/* open the file */
			if ((err = avio_open2(&oc->pb, filename, AVIO_FLAG_WRITE,
				&oc->interrupt_callback,
				&of->opts)) < 0) {
				ctx->exit_program(1);
			}
		}

		if (o->mux_preload) {
			av_dict_set_int(&of->opts, "preload", o->mux_preload*AV_TIME_BASE, 0);
		}
		oc->max_delay = (int)(o->mux_max_delay * AV_TIME_BASE);

		/* copy metadata */
		for (i = 0; i < o->nb_metadata_map; i++) {
			char *p;
			int in_file_index = strtol((const char *)o->metadata_map[i].u.str, &p, 0);

			if (in_file_index >= ctx->nb_input_files) {
				av_log(NULL, AV_LOG_FATAL, "Invalid input file index %d while processing metadata maps\n", in_file_index);
				ctx->exit_program(1);
			}
			ctx->copy_metadata(o->metadata_map[i].specifier, *p ? p + 1 : p, oc,
				in_file_index >= 0 ?
				ctx->input_files[in_file_index]->ctx : NULL, o);
		}

		/* copy chapters */
		if (o->chapters_input_file >= ctx->nb_input_files) {
			if (o->chapters_input_file == INT_MAX) {
				/* copy chapters from the first input file that has them*/
				o->chapters_input_file = -1;
				for (i = 0; i < ctx->nb_input_files; i++)
					if (ctx->input_files[i]->ctx->nb_chapters) {
						o->chapters_input_file = i;
						break;
					}
			}
			else {
				av_log(NULL, AV_LOG_FATAL, "Invalid input file index %d in chapter mapping.\n",
					o->chapters_input_file);
				ctx->exit_program(1);
			}
		}
		if (o->chapters_input_file >= 0)
			ctx->copy_chapters(ctx->input_files[o->chapters_input_file], of,
				!o->metadata_chapters_manual);

		/* copy global metadata by default */
		if (!o->metadata_global_manual && ctx->nb_input_files) {
			av_dict_copy(&oc->metadata, ctx->input_files[0]->ctx->metadata,
				AV_DICT_DONT_OVERWRITE);
			if (o->recording_time != INT64_MAX)
				av_dict_set(&oc->metadata, "duration", NULL, 0);
			av_dict_set(&oc->metadata, "creation_time", NULL, 0);
		}
		if (!o->metadata_streams_manual)
			for (i = of->ost_index; i < ctx->nb_output_streams; i++) {
				InputStream *ist;
				if (ctx->output_streams[i]->source_index < 0)         /* this is true e.g. for attached files */
					continue;
				ist = ctx->input_streams[ctx->output_streams[i]->source_index];
				av_dict_copy(&ctx->output_streams[i]->st->metadata, ist->st->metadata, AV_DICT_DONT_OVERWRITE);
				if (!ctx->output_streams[i]->stream_copy) {
					av_dict_set(&ctx->output_streams[i]->st->metadata, "encoder", NULL, 0);
				}
			}

		/* process manually set programs */
		for (i = 0; i < o->nb_program; i++) {
			const char *p = (const char *)o->program[i].u.str;
			int progid = i + 1;
			AVProgram *program;

			while (*p) {
				const char *p2 = av_get_token(&p, ":");
				const char *to_dealloc = p2;
				char *key;
				if (!p2)
					break;

				if (*p) p++;

				key = av_get_token(&p2, "=");
				if (!key || !*p2) {
					av_freep(&to_dealloc);
					av_freep(&key);
					break;
				}
				p2++;

				if (!strcmp(key, "program_num"))
					progid = strtol(p2, NULL, 0);
				av_freep(&to_dealloc);
				av_freep(&key);
			}

			program = av_new_program(oc, progid);

			p = (const char *)o->program[i].u.str;
			while (*p) {
				const char *p2 = av_get_token(&p, ":");
				const char *to_dealloc = p2;
				char *key;
				if (!p2)
					break;
				if (*p) p++;

				key = av_get_token(&p2, "=");
				if (!key) {
					av_log(NULL, AV_LOG_FATAL,
						"No '=' character in program string %s.\n",
						p2);
					ctx->exit_program(1);
				}
				if (!*p2)
					ctx->exit_program(1);
				p2++;

				if (!strcmp(key, "title")) {
					av_dict_set(&program->metadata, "title", p2, 0);
				}
				else if (!strcmp(key, "program_num")) {
				}
				else if (!strcmp(key, "st")) {
					int st_num = strtol(p2, NULL, 0);
					av_program_add_stream_index(oc, progid, st_num);
				}
				else {
					av_log(NULL, AV_LOG_FATAL, "Unknown program key %s.\n", key);
					ctx->exit_program(1);
				}
				av_freep(&to_dealloc);
				av_freep(&key);
			}
		}

		/* process manually set metadata */
		for (i = 0; i < o->nb_metadata; i++) {
			AVDictionary **m;
			char type, *val;
			const char *stream_spec;
			int index = 0, j, ret = 0;

			val = (char*)strchr((const char *)o->metadata[i].u.str, '=');
			if (!val) {
				av_log(NULL, AV_LOG_FATAL, "No '=' character in metadata string %s.\n",
					o->metadata[i].u.str);
				ctx->exit_program(1);
			}
			*val++ = 0;

			ctx->parse_meta_type(o->metadata[i].specifier, &type, &index, &stream_spec);
			if (type == 's') {
				for (j = 0; j < oc->nb_streams; j++) {
					ost = ctx->output_streams[ctx->nb_output_streams - oc->nb_streams + j];
					if ((ret = ctx->check_stream_specifier(oc, oc->streams[j], stream_spec)) > 0) {
						if (!strcmp((const char *)o->metadata[i].u.str, "rotate")) {
							char *tail;
							double theta = av_strtod(val, &tail);
							if (!*tail) {
								ost->rotate_overridden = 1;
								ost->rotate_override_value = theta;
							}
						}
						else {
							av_dict_set(&oc->streams[j]->metadata, (const char *)o->metadata[i].u.str, *val ? val : NULL, 0);
						}
					}
					else if (ret < 0)
						ctx->exit_program(1);
				}
			}
			else {
				switch (type) {
				case 'g':
					m = &oc->metadata;
					break;
				case 'c':
					if (index < 0 || index >= oc->nb_chapters) {
						av_log(NULL, AV_LOG_FATAL, "Invalid chapter index %d in metadata specifier.\n", index);
						ctx->exit_program(1);
					}
					m = &oc->chapters[index]->metadata;
					break;
				case 'p':
					if (index < 0 || index >= oc->nb_programs) {
						av_log(NULL, AV_LOG_FATAL, "Invalid program index %d in metadata specifier.\n", index);
						ctx->exit_program(1);
					}
					m = &oc->programs[index]->metadata;
					break;
				default:
					av_log(NULL, AV_LOG_FATAL, "Invalid metadata specifier %s.\n", o->metadata[i].specifier);
					ctx->exit_program(1);
				}
				av_dict_set(m, (const char *)o->metadata[i].u.str, *val ? val : NULL, 0);
			}
		}

		return 0;
	}

	static int opt_target(FfmpegConvert* ctx, void *optctx, const char *opt, const char *arg)
	{
		OptionsContext *o = (OptionsContext *)optctx;
		enum { PAL, NTSC, FILM, UNKNOWN } norm = UNKNOWN;
		const char *const frame_rates[] = { "25", "30000/1001", "24000/1001" };

		if (!strncmp(arg, "pal-", 4)) {
			norm = PAL;
			arg += 4;
		}
		else if (!strncmp(arg, "ntsc-", 5)) {
			norm = NTSC;
			arg += 5;
		}
		else if (!strncmp(arg, "film-", 5)) {
			norm = FILM;
			arg += 5;
		}
		else {
			/* Try to determine PAL/NTSC by peeking in the input files */
			if (ctx->nb_input_files) {
				int i, j, fr;
				for (j = 0; j < ctx->nb_input_files; j++) {
					for (i = 0; i < ctx->input_files[j]->nb_streams; i++) {
						AVStream *st = ctx->input_files[j]->ctx->streams[i];
						if (st->codecpar->codec_type != AVMEDIA_TYPE_VIDEO)
							continue;
						fr = st->time_base.den * 1000 / st->time_base.num;
						if (fr == 25000) {
							norm = PAL;
							break;
						}
						else if ((fr == 29970) || (fr == 23976)) {
							norm = NTSC;
							break;
						}
					}
					if (norm != UNKNOWN)
						break;
				}
			}
			if (norm != UNKNOWN)
				av_log(NULL, AV_LOG_INFO, "Assuming %s for target.\n", norm == PAL ? "PAL" : "NTSC");
		}

		if (norm == UNKNOWN) {
			av_log(NULL, AV_LOG_FATAL, "Could not determine norm (PAL/NTSC/NTSC-Film) for target.\n");
			av_log(NULL, AV_LOG_FATAL, "Please prefix target with \"pal-\", \"ntsc-\" or \"film-\",\n");
			av_log(NULL, AV_LOG_FATAL, "or set a framerate with \"-r xxx\".\n");
			ctx->exit_program(1);
		}

		if (!strcmp(arg, "vcd")) {
			opt_video_codec(ctx, o, "c:v", "mpeg1video");
			opt_audio_codec(ctx, o, "c:a", "mp2");
			ctx->parse_option(o, "f", "vcd", ctx->options);

			ctx->parse_option(o, "s", norm == PAL ? "352x288" : "352x240", ctx->options);
			ctx->parse_option(o, "r", frame_rates[norm], ctx->options);
			opt_default(ctx, optctx, "g", norm == PAL ? "15" : "18");

			opt_default(ctx, optctx, "b:v", "1150000");
			opt_default(ctx, optctx, "maxrate:v", "1150000");
			opt_default(ctx, optctx, "minrate:v", "1150000");
			opt_default(ctx, optctx, "bufsize:v", "327680"); // 40*1024*8;

			opt_default(ctx, optctx, "b:a", "224000");
			ctx->parse_option(o, "ar", "44100", ctx->options);
			ctx->parse_option(o, "ac", "2", ctx->options);

			opt_default(ctx, optctx, "packetsize", "2324");
			opt_default(ctx, optctx, "muxrate", "1411200");
			o->mux_preload = (36000 + 3 * 1200) / 90000.0; // 0.44
		}
		else if (!strcmp(arg, "svcd")) {

			opt_video_codec(ctx, o, "c:v", "mpeg2video");
			opt_audio_codec(ctx, o, "c:a", "mp2");
			ctx->parse_option(o, "f", "svcd", ctx->options);

			ctx->parse_option(o, "s", norm == PAL ? "480x576" : "480x480", ctx->options);
			ctx->parse_option(o, "r", frame_rates[norm], ctx->options);
			ctx->parse_option(o, "pix_fmt", "yuv420p", ctx->options);
			opt_default(ctx, optctx, "g", norm == PAL ? "15" : "18");

			opt_default(ctx, optctx, "b:v", "2040000");
			opt_default(ctx, optctx, "maxrate:v", "2516000");
			opt_default(ctx, optctx, "minrate:v", "0"); // 1145000;
			opt_default(ctx, optctx, "bufsize:v", "1835008"); // 224*1024*8;
			opt_default(ctx, optctx, "scan_offset", "1");

			opt_default(ctx, optctx, "b:a", "224000");
			ctx->parse_option(o, "ar", "44100", ctx->options);

			opt_default(ctx, optctx, "packetsize", "2324");

		}
		else if (!strcmp(arg, "dvd")) {

			opt_video_codec(ctx, o, "c:v", "mpeg2video");
			opt_audio_codec(ctx, o, "c:a", "ac3");
			ctx->parse_option(o, "f", "dvd", ctx->options);

			ctx->parse_option(o, "s", norm == PAL ? "720x576" : "720x480", ctx->options);
			ctx->parse_option(o, "r", frame_rates[norm], ctx->options);
			ctx->parse_option(o, "pix_fmt", "yuv420p", ctx->options);
			opt_default(ctx, optctx, "g", norm == PAL ? "15" : "18");

			opt_default(ctx, optctx, "b:v", "6000000");
			opt_default(ctx, optctx, "maxrate:v", "9000000");
			opt_default(ctx, optctx, "minrate:v", "0"); // 1500000;
			opt_default(ctx, optctx, "bufsize:v", "1835008"); // 224*1024*8;

			opt_default(ctx, optctx, "packetsize", "2048");  // from www.mpucoder.com: DVD sectors contain 2048 bytes of data, this is also the size of one pack.
			opt_default(ctx, optctx, "muxrate", "10080000"); // from mplex project: data_rate = 1260000. mux_rate = data_rate * 8

			opt_default(ctx, optctx, "b:a", "448000");
			ctx->parse_option(o, "ar", "48000", ctx->options);

		}
		else if (!strncmp(arg, "dv", 2)) {

			ctx->parse_option(o, "f", "dv", ctx->options);

			ctx->parse_option(o, "s", norm == PAL ? "720x576" : "720x480", ctx->options);
			ctx->parse_option(o, "pix_fmt", !strncmp(arg, "dv50", 4) ? "yuv422p" :
				norm == PAL ? "yuv420p" : "yuv411p", ctx->options);
			ctx->parse_option(o, "r", frame_rates[norm], ctx->options);

			ctx->parse_option(o, "ar", "48000", ctx->options);
			ctx->parse_option(o, "ac", "2", ctx->options);

		}
		else {
			av_log(NULL, AV_LOG_ERROR, "Unknown target: %s\n", arg);
			return AVERROR(EINVAL);
		}

		av_dict_copy(&o->g->codec_opts, ctx->codec_opts, AV_DICT_DONT_OVERWRITE);
		av_dict_copy(&o->g->format_opts, ctx->format_opts, AV_DICT_DONT_OVERWRITE);

		return 0;
	}

	static int opt_video_frames(FfmpegConvert* ctx, void *optctx, const char *opt, const char *arg)
	{
		OptionsContext *o = (OptionsContext *)optctx;
		return ctx->parse_option(o, "frames:v", arg, ctx->options);
	}

	static int opt_audio_frames(FfmpegConvert* ctx, void *optctx, const char *opt, const char *arg)
	{
		OptionsContext *o = (OptionsContext *)optctx;
		return ctx->parse_option(o, "frames:a", arg, ctx->options);
	}

	static int opt_data_frames(FfmpegConvert* ctx, void *optctx, const char *opt, const char *arg)
	{
		OptionsContext *o = (OptionsContext *)optctx;
		return ctx->parse_option(o, "frames:d", arg, ctx->options);
	}

	static int opt_default_new(FfmpegConvert* ctx, OptionsContext *o, const char *opt, const char *arg)
	{
		int ret;
		AVDictionary *cbak = ctx->codec_opts;
		AVDictionary *fbak = ctx->format_opts;
		ctx->codec_opts = NULL;
		ctx->format_opts = NULL;

		ret = opt_default(ctx, o, opt, arg);

		av_dict_copy(&o->g->codec_opts, ctx->codec_opts, 0);
		av_dict_copy(&o->g->format_opts, ctx->format_opts, 0);
		av_dict_free(&ctx->codec_opts);
		av_dict_free(&ctx->format_opts);
		ctx->codec_opts = cbak;
		ctx->format_opts = fbak;

		return ret;
	}

	static int opt_preset(FfmpegConvert* ctx, void *optctx, const char *opt, const char *arg)
	{
		OptionsContext *o = (OptionsContext *)optctx;
		FILE *f = NULL;
		char filename[1000], line[1000], tmp_line[1000];
		const char *codec_name = NULL;

		tmp_line[0] = *opt;
		tmp_line[1] = 0;
		MATCH_PER_TYPE_OPT(codec_names, str, codec_name, NULL, tmp_line, const char *);

		if (!(f = ctx->get_preset_file(filename, sizeof(filename), arg, *opt == 'f', codec_name))) {
			if (!strncmp(arg, "libx264-lossless", strlen("libx264-lossless"))) {
				av_log(NULL, AV_LOG_FATAL, "Please use -preset <speed> -qp 0\n");
			}
			else
				av_log(NULL, AV_LOG_FATAL, "File for preset '%s' not found\n", arg);
			ctx->exit_program(1);
		}

		while (fgets(line, sizeof(line), f)) {
			char *key = tmp_line, *value, *endptr;

			if (strcspn(line, "#\n\r") == 0)
				continue;
			av_strlcpy(tmp_line, line, sizeof(tmp_line));
			if (!av_strtok(key, "=", &value) ||
				!av_strtok(value, "\r\n", &endptr)) {
				av_log(NULL, AV_LOG_FATAL, "%s: Invalid syntax: '%s'\n", filename, line);
				ctx->exit_program(1);
			}
			av_log(NULL, AV_LOG_DEBUG, "ffpreset[%s]: set '%s' = '%s'\n", filename, key, value);

			if (!strcmp(key, "acodec")) opt_audio_codec(ctx, o, key, value);
			else if (!strcmp(key, "vcodec")) opt_video_codec(ctx, o, key, value);
			else if (!strcmp(key, "scodec")) opt_subtitle_codec(ctx, o, key, value);
			else if (!strcmp(key, "dcodec")) opt_data_codec(ctx, o, key, value);
			else if (opt_default_new(ctx, o, key, value) < 0) {
				ctx->exit_program(1);
			}
		}

		fclose(f);

		return 0;
	}

	static int opt_old2new(FfmpegConvert* ctx, void *optctx, const char *opt, const char *arg)
	{
		OptionsContext *o = (OptionsContext *)optctx;
		char *s = av_asprintf("%s:%c", opt + 1, *opt);
		int ret = ctx->parse_option(o, s, arg, ctx->options);
		av_free(s);
		return ret;
	}

	static int opt_bitrate(FfmpegConvert* ctx, void *optctx, const char *opt, const char *arg)
	{
		OptionsContext *o = (OptionsContext *)optctx;

		if (!strcmp(opt, "ab")) {
			av_dict_set(&o->g->codec_opts, "b:a", arg, 0);
			return 0;
		}
		else if (!strcmp(opt, "b")) {
			av_log(NULL, AV_LOG_WARNING, "Please use -b:a or -b:v, -b is ambiguous\n");
			av_dict_set(&o->g->codec_opts, "b:v", arg, 0);
			return 0;
		}
		av_dict_set(&o->g->codec_opts, opt, arg, 0);
		return 0;
	}

	static int opt_qscale(FfmpegConvert* ctx, void *optctx, const char *opt, const char *arg)
	{
		OptionsContext *o = (OptionsContext *)optctx;
		char *s;
		int ret;
		if (!strcmp(opt, "qscale")) {
			av_log(NULL, AV_LOG_WARNING, "Please use -q:a or -q:v, -qscale is ambiguous\n");
			return ctx->parse_option(o, "q:v", arg, ctx->options);
		}
		s = av_asprintf("q%s", opt + 6);
		ret = ctx->parse_option(o, s, arg, ctx->options);
		av_free(s);
		return ret;
	}

	static int opt_profile(FfmpegConvert* ctx, void *optctx, const char *opt, const char *arg)
	{
		OptionsContext *o = (OptionsContext *)optctx;
		if (!strcmp(opt, "profile")) {
			av_log(NULL, AV_LOG_WARNING, "Please use -profile:a or -profile:v, -profile is ambiguous\n");
			av_dict_set(&o->g->codec_opts, "profile:v", arg, 0);
			return 0;
		}
		av_dict_set(&o->g->codec_opts, opt, arg, 0);
		return 0;
	}

	static int opt_video_filters(FfmpegConvert* ctx, void *optctx, const char *opt, const char *arg)
	{
		OptionsContext *o = (OptionsContext *)optctx;
		return ctx->parse_option(o, "filter:v", arg, ctx->options);
	}

	static int opt_audio_filters(FfmpegConvert* ctx, void *optctx, const char *opt, const char *arg)
	{
		OptionsContext *o = (OptionsContext *)optctx;
		return ctx->parse_option(o, "filter:a", arg, ctx->options);
	}

	static int opt_vsync(FfmpegConvert* ctx, void *optctx, const char *opt, const char *arg)
	{
		OptionsContext *o = (OptionsContext *)optctx;
		if (!av_strcasecmp(arg, "cfr"))         ctx->video_sync_method = VSYNC_CFR;
		else if (!av_strcasecmp(arg, "vfr"))         ctx->video_sync_method = VSYNC_VFR;
		else if (!av_strcasecmp(arg, "passthrough")) ctx->video_sync_method = VSYNC_PASSTHROUGH;
		else if (!av_strcasecmp(arg, "drop"))        ctx->video_sync_method = VSYNC_DROP;

		if (ctx->video_sync_method == VSYNC_AUTO)
			ctx->video_sync_method = ctx->parse_number_or_die("vsync", arg, OPT_INT, VSYNC_AUTO, VSYNC_VFR);
		return 0;
	}

	static int opt_timecode(FfmpegConvert* ctx, void *optctx, const char *opt, const char *arg)
	{
		OptionsContext *o = (OptionsContext *)optctx;
		char *tcr = av_asprintf("timecode=%s", arg);
		int ret = ctx->parse_option(o, "metadata:g", tcr, ctx->options);
		if (ret >= 0)
			ret = av_dict_set(&o->g->codec_opts, "gop_timecode", arg, 0);
		av_free(tcr);
		return ret;
	}

	static int opt_channel_layout(FfmpegConvert* ctx, void *optctx, const char *opt, const char *arg)
	{
		OptionsContext *o = (OptionsContext *)optctx;
		char layout_str[32];
		char *stream_str;
		char *ac_str;
		int ret, channels, ac_str_size;
		uint64_t layout;

		layout = av_get_channel_layout(arg);
		if (!layout) {
			av_log(NULL, AV_LOG_ERROR, "Unknown channel layout: %s\n", arg);
			return AVERROR(EINVAL);
		}
		snprintf(layout_str, sizeof(layout_str), "%" PRIu64, layout);
		ret = opt_default_new(ctx, o, opt, layout_str);
		if (ret < 0)
			return ret;

		/* set 'ac' option based on channel layout */
		channels = av_get_channel_layout_nb_channels(layout);
		snprintf(layout_str, sizeof(layout_str), "%d", channels);
		stream_str = (char*)strchr(opt, ':');
		ac_str_size = 3 + (stream_str ? strlen(stream_str) : 0);
		ac_str = (char*)av_mallocz(ac_str_size);

		av_strlcpy(ac_str, "ac", 3);
		if (stream_str)
			av_strlcat(ac_str, stream_str, ac_str_size);
		ret = ctx->parse_option(o, ac_str, layout_str, ctx->options);
		av_free(ac_str);

		return ret;
	}

	static int opt_audio_qscale(FfmpegConvert* ctx, void *optctx, const char *opt, const char *arg)
	{
		OptionsContext *o = (OptionsContext *)optctx;
		return ctx->parse_option(o, "q:a", arg, ctx->options);
	}

	static int opt_filter_complex(FfmpegConvert* ctx, void *optctx, const char *opt, const char *arg)
	{
		OptionsContext *o = (OptionsContext *)optctx;
		GROW_ARRAY_STATIC(ctx->filtergraphs, ctx->nb_filtergraphs, FilterGraph**);
		ctx->filtergraphs[ctx->nb_filtergraphs - 1] = (FilterGraph *)av_mallocz(sizeof(*ctx->filtergraphs[0]));
		ctx->filtergraphs[ctx->nb_filtergraphs - 1]->index = ctx->nb_filtergraphs - 1;
		ctx->filtergraphs[ctx->nb_filtergraphs - 1]->graph_desc = av_strdup(arg);
		if (!ctx->filtergraphs[ctx->nb_filtergraphs - 1]->graph_desc)
			return AVERROR(ENOMEM);

		ctx->input_stream_potentially_available = 1;

		return 0;
	}

	static int opt_filter_complex_script(FfmpegConvert* ctx, void *optctx, const char *opt, const char *arg)
	{
		OptionsContext *o = (OptionsContext *)optctx;
		uint8_t *graph_desc = ctx->read_file(arg);
		if (!graph_desc)
			return AVERROR(EINVAL);

		GROW_ARRAY_STATIC(ctx->filtergraphs, ctx->nb_filtergraphs, FilterGraph**);
		ctx->filtergraphs[ctx->nb_filtergraphs - 1] = (FilterGraph *)av_mallocz(sizeof(*ctx->filtergraphs[0]));
		ctx->filtergraphs[ctx->nb_filtergraphs - 1]->index = ctx->nb_filtergraphs - 1;
		ctx->filtergraphs[ctx->nb_filtergraphs - 1]->graph_desc = (const char*)graph_desc;

		ctx->input_stream_potentially_available = 1;

		return 0;
	}

	static int opt_recording_timestamp(FfmpegConvert* ctx, void *optctx, const char *opt, const char *arg)
	{
		OptionsContext *o = (OptionsContext *)optctx;
		char buf[128];
		int64_t recording_timestamp = ctx->parse_time_or_die(opt, arg, 0) / 1E6;
		struct tm time = *gmtime((time_t*)&recording_timestamp);
		if (!strftime(buf, sizeof(buf), "creation_time=%Y-%m-%dT%H:%M:%S%z", &time))
			return -1;
		ctx->parse_option(o, "metadata", buf, ctx->options);
		return 0;
	}

	/* arg format is "output-stream-index:streamid-value". */
	static int opt_streamid(FfmpegConvert* ctx, void *optctx, const char *opt, const char *arg)
	{
		OptionsContext *o = (OptionsContext *)optctx;
		int idx;
		char *p;
		char idx_str[16];

		av_strlcpy(idx_str, arg, sizeof(idx_str));
		p = strchr(idx_str, ':');
		if (!p) {
			av_log(NULL, AV_LOG_FATAL,
				"Invalid value '%s' for option '%s', required syntax is 'index:value'\n",
				arg, opt);
			ctx->exit_program(1);
		}
		*p++ = '\0';
		idx = ctx->parse_number_or_die(opt, idx_str, OPT_INT, 0, MAX_STREAMS - 1);
		o->streamid_map = (int*)ctx->grow_array(o->streamid_map, sizeof(*o->streamid_map), &o->nb_streamid_map, idx + 1);
		o->streamid_map[idx] = ctx->parse_number_or_die(opt, p, OPT_INT, 0, INT_MAX);
		return 0;
	}

	static int open_input_file(FfmpegConvert* ctx, OptionsContext *o, const char *filename)
	{
		InputFile *f;
		AVFormatContext *ic;
		AVInputFormat *file_iformat = NULL;
		int err, i, ret;
		int64_t timestamp;
		AVDictionary *unused_opts = NULL;
		AVDictionaryEntry *e = NULL;
		char *   video_codec_name = NULL;
		char *   audio_codec_name = NULL;
		char *subtitle_codec_name = NULL;
		char *    data_codec_name = NULL;
		int scan_all_pmts_set = 0;

		if (o->stop_time != INT64_MAX && o->recording_time != INT64_MAX) {
			o->stop_time = INT64_MAX;
			av_log(NULL, AV_LOG_WARNING, "-t and -to cannot be used together; using -t.\n");
		}

		if (o->stop_time != INT64_MAX && o->recording_time == INT64_MAX) {
			int64_t start_time = o->start_time == AV_NOPTS_VALUE ? 0 : o->start_time;
			if (o->stop_time <= start_time) {
				av_log(NULL, AV_LOG_ERROR, "-to value smaller than -ss; aborting.\n");
				ctx->exit_program(1);
			}
			else {
				o->recording_time = o->stop_time - start_time;
			}
		}

		if (o->format) {
			if (!(file_iformat = av_find_input_format(o->format))) {
				av_log(NULL, AV_LOG_FATAL, "Unknown input format: '%s'\n", o->format);
				ctx->exit_program(1);
			}
		}

		if (!strcmp(filename, "-"))
			filename = "pipe:";


		/* get default parameters from command line */
		ic = avformat_alloc_context();
		if (!ic) {
			ctx->exit_program(1);
		}
		if (o->nb_audio_sample_rate) {
			av_dict_set_int(&o->g->format_opts, "sample_rate", o->audio_sample_rate[o->nb_audio_sample_rate - 1].u.i, 0);
		}
		if (o->nb_audio_channels) {
			/* because we set audio_channels based on both the "ac" and
			* "channel_layout" options, we need to check that the specified
			* demuxer actually has the "channels" option before setting it */
			if (file_iformat && file_iformat->priv_class &&
				av_opt_find(&file_iformat->priv_class, "channels", NULL, 0,
					AV_OPT_SEARCH_FAKE_OBJ)) {
				av_dict_set_int(&o->g->format_opts, "channels", o->audio_channels[o->nb_audio_channels - 1].u.i, 0);
			}
		}
		if (o->nb_frame_rates) {
			/* set the format-level framerate option;
			* this is important for video grabbers, e.g. x11 */
			if (file_iformat && file_iformat->priv_class &&
				av_opt_find(&file_iformat->priv_class, "framerate", NULL, 0,
					AV_OPT_SEARCH_FAKE_OBJ)) {
				av_dict_set(&o->g->format_opts, "framerate",
					(const char*)o->frame_rates[o->nb_frame_rates - 1].u.str, 0);
			}
		}
		if (o->nb_frame_sizes) {
			av_dict_set(&o->g->format_opts, "video_size", (const char*)o->frame_sizes[o->nb_frame_sizes - 1].u.str, 0);
		}
		if (o->nb_frame_pix_fmts)
			av_dict_set(&o->g->format_opts, "pixel_format", (const char*)o->frame_pix_fmts[o->nb_frame_pix_fmts - 1].u.str, 0);

		MATCH_PER_TYPE_OPT(codec_names, str, video_codec_name, ic, "v", char*);
		MATCH_PER_TYPE_OPT(codec_names, str, audio_codec_name, ic, "a", char*);
		MATCH_PER_TYPE_OPT(codec_names, str, subtitle_codec_name, ic, "s", char*);
		MATCH_PER_TYPE_OPT(codec_names, str, data_codec_name, ic, "d", char*);

		if (video_codec_name)
			ic->video_codec = ctx->find_codec_or_die(video_codec_name, AVMEDIA_TYPE_VIDEO, 0);
		if (audio_codec_name)
			ic->audio_codec = ctx->find_codec_or_die(audio_codec_name, AVMEDIA_TYPE_AUDIO, 0);
		if (subtitle_codec_name)
			ic->subtitle_codec = ctx->find_codec_or_die(subtitle_codec_name, AVMEDIA_TYPE_SUBTITLE, 0);
		if (data_codec_name)
			ic->data_codec = ctx->find_codec_or_die(data_codec_name, AVMEDIA_TYPE_DATA, 0);

		ic->video_codec_id = video_codec_name ? ic->video_codec->id : AV_CODEC_ID_NONE;
		ic->audio_codec_id = audio_codec_name ? ic->audio_codec->id : AV_CODEC_ID_NONE;
		ic->subtitle_codec_id = subtitle_codec_name ? ic->subtitle_codec->id : AV_CODEC_ID_NONE;
		ic->data_codec_id = data_codec_name ? ic->data_codec->id : AV_CODEC_ID_NONE;

		ic->flags |= AVFMT_FLAG_NONBLOCK;
		if (o->bitexact)
			ic->flags |= AVFMT_FLAG_BITEXACT;
		ic->interrupt_callback = int_cb;

		if (!av_dict_get(o->g->format_opts, "scan_all_pmts", NULL, AV_DICT_MATCH_CASE)) {
			av_dict_set(&o->g->format_opts, "scan_all_pmts", "1", AV_DICT_DONT_OVERWRITE);
			scan_all_pmts_set = 1;
		}
		/* open the input file with generic avformat function */
		err = avformat_open_input(&ic, filename, file_iformat, &o->g->format_opts);
		if (err < 0) {
			if (err == AVERROR_PROTOCOL_NOT_FOUND)
				av_log(NULL, AV_LOG_ERROR, "Did you mean file:%s?\n", filename);
			ctx->exit_program(1);
		}
		if (scan_all_pmts_set)
			av_dict_set(&o->g->format_opts, "scan_all_pmts", NULL, AV_DICT_MATCH_CASE);
		ctx->remove_avoptions(&o->g->format_opts, o->g->codec_opts);
		ctx->assert_avoptions(o->g->format_opts);

		/* apply forced codec ids */
		for (i = 0; i < ic->nb_streams; i++)
			ctx->choose_decoder(o, ic, ic->streams[i]);

		if (ctx->find_stream_info) {
			AVDictionary **opts = ctx->setup_find_stream_info_opts(ic, o->g->codec_opts);
			int orig_nb_streams = ic->nb_streams;

			/* If not enough info to get the stream parameters, we decode the
			first frames to get it. (used in mpeg case for example) */
			ret = avformat_find_stream_info(ic, opts);

			for (i = 0; i < orig_nb_streams; i++)
				av_dict_free(&opts[i]);
			av_freep(&opts);

			if (ret < 0) {
				av_log(NULL, AV_LOG_FATAL, "%s: could not find codec parameters\n", filename);
				if (ic->nb_streams == 0) {
					avformat_close_input(&ic);
					ctx->exit_program(1);
				}
			}
		}

		if (o->start_time_eof != AV_NOPTS_VALUE) {
			if (ic->duration>0) {
				o->start_time = o->start_time_eof + ic->duration;
			}
			else
				av_log(NULL, AV_LOG_WARNING, "Cannot use -sseof, duration of %s not known\n", filename);
		}
		timestamp = (o->start_time == AV_NOPTS_VALUE) ? 0 : o->start_time;
		/* add the stream start time */
		if (!o->seek_timestamp && ic->start_time != AV_NOPTS_VALUE)
			timestamp += ic->start_time;

		/* if seeking requested, we execute it */
		if (o->start_time != AV_NOPTS_VALUE) {
			int64_t seek_timestamp = timestamp;

			if (!(ic->iformat->flags & AVFMT_SEEK_TO_PTS)) {
				int dts_heuristic = 0;
				for (i = 0; i<ic->nb_streams; i++) {
					const AVCodecParameters *par = ic->streams[i]->codecpar;
					if (par->video_delay)
						dts_heuristic = 1;
				}
				if (dts_heuristic) {
					seek_timestamp -= 3 * AV_TIME_BASE / 23;
				}
			}
			ret = avformat_seek_file(ic, -1, INT64_MIN, seek_timestamp, seek_timestamp, 0);
			if (ret < 0) {
				av_log(NULL, AV_LOG_WARNING, "%s: could not seek to position %0.3f\n",
					filename, (double)timestamp / AV_TIME_BASE);
			}
		}

		/* update the current parameters so that they match the one of the input stream */
		ctx->add_input_streams(o, ic);

		/* dump the file content */
		av_dump_format(ic, ctx->nb_input_files, filename, 0);

		GROW_ARRAY_STATIC(ctx->input_files, ctx->nb_input_files, InputFile**);
		f = (InputFile *)av_mallocz(sizeof(*f));
		ctx->input_files[ctx->nb_input_files - 1] = f;

		f->ctx = ic;
		f->ist_index = ctx->nb_input_streams - ic->nb_streams;
		f->start_time = o->start_time;
		f->recording_time = o->recording_time;
		f->input_ts_offset = o->input_ts_offset;
		f->ts_offset = o->input_ts_offset - (ctx->copy_ts ? (ctx->start_at_zero && ic->start_time != AV_NOPTS_VALUE ? ic->start_time : 0) : timestamp);
		f->nb_streams = ic->nb_streams;
		f->rate_emu = o->rate_emu;
		f->accurate_seek = o->accurate_seek;
		f->loop = o->loop;
		f->duration = 0;
		f->time_base = AV_TIME_BASE_1_1;


		/* check if all codec options have been used */
		unused_opts = ctx->strip_specifiers(o->g->codec_opts);
		for (i = f->ist_index; i < ctx->nb_input_streams; i++) {
			e = NULL;
			while ((e = av_dict_get(ctx->input_streams[i]->decoder_opts, "", e,
				AV_DICT_IGNORE_SUFFIX)))
				av_dict_set(&unused_opts, e->key, NULL, 0);
		}

		e = NULL;
		while ((e = av_dict_get(unused_opts, "", e, AV_DICT_IGNORE_SUFFIX))) {
			const AVClass *_class = avcodec_get_class();
			const AVOption *option = av_opt_find(&_class, e->key, NULL, 0,
				AV_OPT_SEARCH_CHILDREN | AV_OPT_SEARCH_FAKE_OBJ);
			const AVClass *fclass = avformat_get_class();
			const AVOption *foption = av_opt_find(&fclass, e->key, NULL, 0,
				AV_OPT_SEARCH_CHILDREN | AV_OPT_SEARCH_FAKE_OBJ);
			if (!option || foption)
				continue;


			if (!(option->flags & AV_OPT_FLAG_DECODING_PARAM)) {
				ctx->exit_program(1);
			}
		}
		av_dict_free(&unused_opts);

		for (i = 0; i < o->nb_dump_attachment; i++) {
			int j;

			for (j = 0; j < ic->nb_streams; j++) {
				AVStream *st = ic->streams[j];

				if (ctx->check_stream_specifier(ic, st, o->dump_attachment[i].specifier) == 1)
					ctx->dump_attachment(st, (const char *)o->dump_attachment[i].u.str);
			}
		}

		ctx->input_stream_potentially_available = 1;

		return 0;
	}

#define FLAGS (o->type == AV_OPT_TYPE_FLAGS && (arg[0]=='-' || arg[0]=='+')) ? AV_DICT_APPEND : 0

	static int opt_default(FfmpegConvert* ctx, void *optctx, const char *opt, const char *arg)
	{
		OptionsContext *oc = (OptionsContext *)optctx;

		const AVOption *o;
		int consumed = 0;
		char opt_stripped[128];
		const char *p;
		const AVClass *cc = avcodec_get_class(), *fc = avformat_get_class();

		const AVClass *sc = sws_get_class();
		const AVClass *swr_class = swr_get_class();

		if (!strcmp(opt, "debug") || !strcmp(opt, "fdebug"))
			av_log_set_level(AV_LOG_DEBUG);

		if (!(p = strchr(opt, ':')))
			p = opt + strlen(opt);
		av_strlcpy(opt_stripped, opt, FFMIN(sizeof(opt_stripped), p - opt + 1));

		if ((o = ctx->opt_find(&cc, opt_stripped, NULL, 0,
			AV_OPT_SEARCH_CHILDREN | AV_OPT_SEARCH_FAKE_OBJ)) ||
			((opt[0] == 'v' || opt[0] == 'a' || opt[0] == 's') &&
				(o = oc->m_owner->opt_find(&cc, opt + 1, NULL, 0, AV_OPT_SEARCH_FAKE_OBJ)))) {
			av_dict_set(&oc->m_owner->codec_opts, opt, arg, FLAGS);
			consumed = 1;
		}
		if ((o = ctx->opt_find(&fc, opt, NULL, 0,
			AV_OPT_SEARCH_CHILDREN | AV_OPT_SEARCH_FAKE_OBJ))) {
			av_dict_set(&ctx->format_opts, opt, arg, FLAGS);
			if (consumed)
				av_log(NULL, AV_LOG_VERBOSE, "Routing option %s to both codec and muxer layer\n", opt);
			consumed = 1;
		}
		if (!consumed && (o = ctx->opt_find(&sc, opt, NULL, 0,
			AV_OPT_SEARCH_CHILDREN | AV_OPT_SEARCH_FAKE_OBJ))) {
			struct SwsContext *sws = sws_alloc_context();
			int ret = av_opt_set(sws, opt, arg, 0);
			sws_freeContext(sws);
			if (!strcmp(opt, "srcw") || !strcmp(opt, "srch") ||
				!strcmp(opt, "dstw") || !strcmp(opt, "dsth") ||
				!strcmp(opt, "src_format") || !strcmp(opt, "dst_format")) {
				av_log(NULL, AV_LOG_ERROR, "Directly using swscale dimensions/format options is not supported, please use the -s or -pix_fmt options\n");
				return AVERROR(EINVAL);
			}
			if (ret < 0) {
				av_log(NULL, AV_LOG_ERROR, "Error setting option %s.\n", opt);
				return ret;
			}

			av_dict_set(&ctx->sws_dict, opt, arg, FLAGS);

			consumed = 1;
		}

		if (!consumed && (o = ctx->opt_find(&swr_class, opt, NULL, 0,
			AV_OPT_SEARCH_CHILDREN | AV_OPT_SEARCH_FAKE_OBJ))) {
			struct SwrContext *swr = swr_alloc();
			int ret = av_opt_set(swr, opt, arg, 0);
			swr_free(&swr);
			if (ret < 0) {
				av_log(NULL, AV_LOG_ERROR, "Error setting option %s.\n", opt);
				return ret;
			}
			av_dict_set(&ctx->swr_opts, opt, arg, FLAGS);
			consumed = 1;
		}

		if (consumed)
			return 0;
		return AVERROR_OPTION_NOT_FOUND;
	}



#define OFFSET(x) offsetof(OptionsContext, x)
	const OptionDef options[180] = {
		/* main options */
		{ "L",           OPT_EXIT,NULL, show_help , NULL,     "show license" },
		{ "h",           OPT_EXIT,NULL, show_help , NULL,        "show help", "topic" },
		{ "?",           OPT_EXIT,NULL, show_help , NULL,        "show help", "topic" },
		{ "help",        OPT_EXIT,NULL, show_help , NULL,        "show help", "topic" },
		{ "-help",       OPT_EXIT,NULL, show_help , NULL,        "show help", "topic" },
		{ "version",     OPT_EXIT,NULL, show_help , NULL,     "show version" },
		{ "buildconf",   OPT_EXIT,NULL, show_help , NULL,   "show build configuration" },
		{ "formats",     OPT_EXIT,NULL, show_help , NULL,     "show available formats" },
		{ "muxers",      OPT_EXIT,NULL, show_help , NULL,      "show available muxers" },
		{ "demuxers",    OPT_EXIT,NULL, show_help , NULL,    "show available demuxers" },
		{ "devices",     OPT_EXIT,NULL, show_help , NULL,     "show available devices" },
		{ "codecs",      OPT_EXIT,NULL, show_help , NULL,      "show available codecs" },
		{ "decoders",    OPT_EXIT,NULL, show_help , NULL,    "show available decoders" },
		{ "encoders",    OPT_EXIT,NULL, show_help , NULL ,    "show available encoders" },
		{ "bsfs",        OPT_EXIT,NULL, show_help , NULL,        "show available bit stream filters" },
		{ "protocols",   OPT_EXIT,NULL, show_help , NULL,   "show available protocols" },
		{ "filters",     OPT_EXIT,NULL, show_help , NULL,     "show available filters" },
		{ "pix_fmts",    OPT_EXIT,NULL, show_help , NULL,    "show available pixel formats" },
		{ "layouts",     OPT_EXIT,NULL, show_help , NULL,     "show standard channel layouts" },
		{ "sample_fmts", OPT_EXIT,NULL, show_help , NULL, "show available audio sample formats" },
		{ "colors",      OPT_EXIT,NULL, show_help , NULL,      "show available color names" },
		{ "loglevel",    HAS_ARG, NULL, opt_dummy , NULL,     "set logging level", "loglevel" },
		{ "v",           HAS_ARG, NULL, opt_dummy , NULL,     "set logging level", "loglevel" },
		{ "report",      0,       NULL, opt_dummy,NULL,            "generate a report" },
		{ "max_alloc",   HAS_ARG, NULL, opt_dummy , NULL, "set maximum size of a single allocated block", "bytes" },
		{ "cpuflags",    HAS_ARG | OPT_EXPERT,NULL, opt_dummy , NULL,     "force specific cpu flags", "flags" },
		{ "hide_banner", OPT_BOOL | OPT_EXPERT,&dummy, NULL,NULL,     "do not show program banner", "hide_banner" },
		{ "sources"    , OPT_EXIT | HAS_ARG,NULL, show_help , NULL,"list sources of the input device", "device" },
		{ "sinks"      , OPT_EXIT | HAS_ARG,NULL, show_help , NULL,"list sinks of the output device", "device" },
		{ "f",              HAS_ARG | OPT_STRING | OPT_OFFSET | OPT_INPUT | OPT_OUTPUT,NULL,NULL,OFFSET(format),"force format", "fmt" },
		{ "y",              OPT_BOOL,&dummy, NULL,NULL,	"overwrite output files" },
		{ "n",              OPT_BOOL,&dummy, NULL,NULL,"never overwrite output files" },
		{ "ignore_unknown", OPT_BOOL,&ignore_unknown_streams ,NULL,NULL,"Ignore unknown stream types" },
		{ "copy_unknown",   OPT_BOOL | OPT_EXPERT,&copy_unknown_streams ,NULL,NULL,"Copy unknown stream types" },
		{ "c",              HAS_ARG | OPT_STRING | OPT_SPEC | OPT_INPUT | OPT_OUTPUT,NULL,NULL,OFFSET(codec_names) ,"codec name", "codec" },
		{ "codec",          HAS_ARG | OPT_STRING | OPT_SPEC | OPT_INPUT | OPT_OUTPUT,NULL,NULL,OFFSET(codec_names) ,"codec name", "codec" },
		{ "pre",            HAS_ARG | OPT_STRING | OPT_SPEC | OPT_OUTPUT,NULL,NULL,OFFSET(presets) ,"preset name", "preset" },
		{ "map",            HAS_ARG | OPT_EXPERT | OPT_PERFILE | OPT_OUTPUT,NULL, opt_map , NULL,"set input stream mapping","[-]input_file_id[:stream_specifier][,sync_file_id[:stream_specifier]]" },
		{ "map_channel",    HAS_ARG | OPT_EXPERT | OPT_PERFILE | OPT_OUTPUT,NULL, opt_map_channel , NULL,"map an audio channel from one stream to another", "file.stream.channel[:syncfile.syncstream]" },
		{ "map_metadata",   HAS_ARG | OPT_STRING | OPT_SPEC | OPT_OUTPUT,NULL,NULL,OFFSET(metadata_map) ,"set metadata information of outfile from infile","outfile[,metadata]:infile[,metadata]" },
		{ "map_chapters",   HAS_ARG | OPT_INT | OPT_EXPERT | OPT_OFFSET | OPT_OUTPUT,NULL,NULL, OFFSET(chapters_input_file) ,"set chapters mapping", "input_file_index" },
		{ "t",              HAS_ARG | OPT_TIME | OPT_OFFSET | OPT_INPUT | OPT_OUTPUT,NULL,NULL, OFFSET(recording_time) ,"record or transcode \"duration\" seconds of audio/video","duration" },
		{ "to",             HAS_ARG | OPT_TIME | OPT_OFFSET | OPT_INPUT | OPT_OUTPUT,NULL,NULL, OFFSET(stop_time) ,"record or transcode stop time", "time_stop" },
		{ "fs",             HAS_ARG | OPT_INT64 | OPT_OFFSET | OPT_OUTPUT,NULL,NULL, OFFSET(limit_filesize) ,"set the limit file size in bytes", "limit_size" },
		{ "ss",             HAS_ARG | OPT_TIME | OPT_OFFSET | OPT_INPUT | OPT_OUTPUT,NULL,NULL,OFFSET(start_time) ,"set the start time offset", "time_off" },
		{ "sseof",          HAS_ARG | OPT_TIME | OPT_OFFSET | OPT_INPUT | OPT_OUTPUT,NULL,NULL,OFFSET(start_time_eof) ,"set the start time offset relative to EOF", "time_off" },
		{ "seek_timestamp", HAS_ARG | OPT_INT | OPT_OFFSET | OPT_INPUT,NULL,NULL, OFFSET(seek_timestamp) ,"enable/disable seeking by timestamp with -ss" },
		{ "accurate_seek",  OPT_BOOL | OPT_OFFSET | OPT_EXPERT | OPT_INPUT,NULL,NULL, OFFSET(accurate_seek) ,"enable/disable accurate seeking with -ss" },
		{ "itsoffset",      HAS_ARG | OPT_TIME | OPT_OFFSET | OPT_EXPERT | OPT_INPUT,NULL,NULL, OFFSET(input_ts_offset) ,"set the input ts offset", "time_off" },
		{ "itsscale",       HAS_ARG | OPT_DOUBLE | OPT_SPEC | OPT_EXPERT | OPT_INPUT,NULL,NULL, OFFSET(ts_scale) ,"set the input ts scale", "scale" },
		{ "timestamp",      HAS_ARG | OPT_PERFILE | OPT_OUTPUT,NULL, opt_recording_timestamp , NULL,"set the recording timestamp ('now' to set the current time)", "time" },
		{ "metadata",       HAS_ARG | OPT_STRING | OPT_SPEC | OPT_OUTPUT,NULL,NULL, OFFSET(metadata) ,"add metadata", "string=string" },
		{ "program",        HAS_ARG | OPT_STRING | OPT_SPEC | OPT_OUTPUT,NULL,NULL, OFFSET(program),"add program with specified streams", "title=string:st=number..." },
		{ "dframes",        HAS_ARG | OPT_PERFILE | OPT_EXPERT | OPT_OUTPUT,NULL, opt_data_frames , NULL,"set the number of data frames to output", "number" },
		{ "benchmark",      OPT_BOOL | OPT_EXPERT,&dummy, NULL,NULL,"add timings for benchmarking" },
		{ "benchmark_all",  OPT_BOOL | OPT_EXPERT,&dummy, NULL,NULL,"add timings for each task" },
		{ "progress",       HAS_ARG | OPT_EXPERT,NULL, opt_dummy , NULL,"write program-readable progress information", "url" },
		{ "stdin",          OPT_BOOL | OPT_EXPERT,&dummy, NULL,NULL,"enable or disable interaction on standard input" },
		{ "timelimit",      HAS_ARG | OPT_EXPERT,NULL, opt_dummy ,NULL,"set max runtime in seconds", "limit" },
		{ "dump",           OPT_BOOL | OPT_EXPERT,&dummy, NULL,NULL,"dump each input packet" },
		{ "hex",            OPT_BOOL | OPT_EXPERT,&dummy, NULL,NULL, "when dumping packets, also dump the payload" },
		{ "re",             OPT_BOOL | OPT_EXPERT | OPT_OFFSET | OPT_INPUT,NULL,NULL,OFFSET(rate_emu) ,"read input at native frame rate", "" },
		{ "target",         HAS_ARG | OPT_PERFILE | OPT_OUTPUT,NULL, opt_target , NULL,"specify target file type (\"vcd\", \"svcd\", \"dvd\", \"dv\" or \"dv50\" ""with optional prefixes \"pal-\", \"ntsc-\" or \"film-\")", "type" },
		{ "vsync",          HAS_ARG | OPT_EXPERT,NULL, opt_vsync , NULL,"video sync method", "" },
		{ "frame_drop_threshold", HAS_ARG | OPT_FLOAT | OPT_EXPERT, &frame_drop_threshold ,NULL,NULL,"frame drop threshold", "" },
		{ "async",          HAS_ARG | OPT_INT | OPT_EXPERT, &audio_sync_method ,NULL,NULL,"audio sync method", "async" },
		{ "adrift_threshold", HAS_ARG | OPT_FLOAT | OPT_EXPERT, &audio_drift_threshold ,NULL,NULL,"audio drift threshold", "threshold" },
		{ "copyts",         OPT_BOOL | OPT_EXPERT, &copy_ts,NULL,NULL,"copy timestamps" },
		{ "start_at_zero",  OPT_BOOL | OPT_EXPERT,&start_at_zero,NULL,NULL,"shift input timestamps to start at 0 when using copyts" },
		{ "copytb",         HAS_ARG | OPT_INT | OPT_EXPERT,{ &copy_tb },NULL,NULL,"copy input stream time base when stream copying", "mode" },
		{ "shortest",       OPT_BOOL | OPT_EXPERT | OPT_OFFSET | OPT_OUTPUT,NULL,NULL, OFFSET(shortest) ,"finish encoding within shortest input" },
		{ "bitexact",       OPT_BOOL | OPT_EXPERT | OPT_OFFSET | OPT_OUTPUT | OPT_INPUT,NULL,NULL,OFFSET(bitexact) ,"bitexact mode" },
		{ "apad",           OPT_STRING | HAS_ARG | OPT_SPEC | OPT_OUTPUT,NULL,NULL,OFFSET(apad) ,"audio pad", "" },
		{ "dts_delta_threshold", HAS_ARG | OPT_FLOAT | OPT_EXPERT,&dts_delta_threshold ,NULL,NULL,"timestamp discontinuity delta threshold", "threshold" },
		{ "dts_error_threshold", HAS_ARG | OPT_FLOAT | OPT_EXPERT ,&dts_error_threshold ,NULL,NULL,"timestamp error delta threshold", "threshold" },
		{ "xerror",         OPT_BOOL | OPT_EXPERT,&dummy, NULL,NULL,"exit on error", "error" },
		{ "abort_on",       HAS_ARG | OPT_EXPERT,NULL, opt_dummy , NULL,"abort on the specified condition flags", "flags" },
		{ "copyinkf",       OPT_BOOL | OPT_EXPERT | OPT_SPEC | OPT_OUTPUT,NULL,NULL,OFFSET(copy_initial_nonkeyframes) ,"copy initial non-keyframes" },
		{ "copypriorss",    OPT_INT | HAS_ARG | OPT_EXPERT | OPT_SPEC | OPT_OUTPUT,NULL,NULL, OFFSET(copy_prior_start) ,"copy or discard frames before start time" },
		{ "frames",         OPT_INT64 | HAS_ARG | OPT_SPEC | OPT_OUTPUT,NULL,NULL,OFFSET(max_frames) ,"set the number of frames to output", "number" },
		{ "tag",            OPT_STRING | HAS_ARG | OPT_SPEC | OPT_EXPERT | OPT_OUTPUT | OPT_INPUT,NULL,NULL, OFFSET(codec_tags) ,"force codec tag/fourcc", "fourcc/tag" },
		{ "q",              HAS_ARG | OPT_EXPERT | OPT_DOUBLE | OPT_SPEC | OPT_OUTPUT,NULL,NULL,OFFSET(qscale) ,"use fixed quality scale (VBR)", "q" },
		{ "qscale",         HAS_ARG | OPT_EXPERT | OPT_PERFILE | OPT_OUTPUT,NULL, opt_qscale , NULL,"use fixed quality scale (VBR)", "q" },
		{ "profile",        HAS_ARG | OPT_EXPERT | OPT_PERFILE | OPT_OUTPUT,NULL, opt_profile , NULL,"set profile", "profile" },
		{ "filter",         HAS_ARG | OPT_STRING | OPT_SPEC | OPT_OUTPUT,NULL,NULL,OFFSET(filters) ,"set stream filtergraph", "filter_graph" },
		{ "filter_threads",  HAS_ARG | OPT_INT, &filter_nbthreads ,NULL,NULL,"number of non-complex filter threads" },
		{ "filter_script",  HAS_ARG | OPT_STRING | OPT_SPEC | OPT_OUTPUT,NULL,NULL,OFFSET(filter_scripts) ,"read stream filtergraph description from a file", "filename" },
		{ "reinit_filter",  HAS_ARG | OPT_INT | OPT_SPEC | OPT_INPUT,NULL,NULL,OFFSET(reinit_filters) ,"reinit filtergraph on input parameter changes", "" },
		{ "filter_complex", HAS_ARG | OPT_EXPERT,NULL, opt_filter_complex , NULL,"create a complex filtergraph", "graph_description" },
		{ "filter_complex_threads", HAS_ARG | OPT_INT,&filter_complex_nbthreads ,NULL,  NULL,"number of threads for -filter_complex" },
		{ "lavfi",          HAS_ARG | OPT_EXPERT,NULL, opt_filter_complex , NULL,"create a complex filtergraph", "graph_description" },
		{ "filter_complex_script", HAS_ARG | OPT_EXPERT,NULL, opt_filter_complex_script , NULL,"read complex filtergraph description from a file", "filename" },
		{ "stats",          OPT_BOOL,&dummy, NULL,NULL,"print progress report during encoding", },
		{ "attach",         HAS_ARG | OPT_PERFILE | OPT_EXPERT | OPT_OUTPUT,NULL, opt_attach , NULL,"add an attachment to the output file", "filename" },
		{ "dump_attachment", HAS_ARG | OPT_STRING | OPT_SPEC | OPT_EXPERT | OPT_INPUT,NULL,NULL,OFFSET(dump_attachment) ,"extract an attachment into a file", "filename" },
		{ "stream_loop", OPT_INT | HAS_ARG | OPT_EXPERT | OPT_INPUT | OPT_OFFSET,NULL,NULL,OFFSET(loop) , "set number of times input stream shall be looped", "loop count" },
		{ "debug_ts",       OPT_BOOL | OPT_EXPERT,&dummy, NULL,NULL,"print timestamp debugging info" },
		{ "max_error_rate",  HAS_ARG | OPT_FLOAT, &max_error_rate,NULL,NULL,"maximum error rate", "ratio of errors (0.0: no errors, 1.0: 100% errors) above which ffmpeg returns an error instead of success." },
		{ "discard",        OPT_STRING | HAS_ARG | OPT_SPEC | OPT_INPUT,NULL,NULL,OFFSET(discard) ,"discard", "" },
		{ "disposition",    OPT_STRING | HAS_ARG | OPT_SPEC | OPT_OUTPUT,NULL,NULL,OFFSET(disposition),"disposition", "" },
		{ "thread_queue_size", HAS_ARG | OPT_INT | OPT_OFFSET | OPT_EXPERT | OPT_INPUT,NULL,NULL,OFFSET(thread_queue_size) ,"set the maximum number of queued packets from the demuxer" },
		{ "find_stream_info", OPT_BOOL | OPT_PERFILE | OPT_INPUT | OPT_EXPERT, &find_stream_info ,NULL,NULL,"read and decode the streams to fill missing information with heuristics" },

		/* video options */
		{ "vframes",      OPT_VIDEO | HAS_ARG | OPT_PERFILE | OPT_OUTPUT,NULL, opt_video_frames , NULL,"set the number of video frames to output", "number" },
		{ "r",            OPT_VIDEO | HAS_ARG | OPT_STRING | OPT_SPEC | OPT_INPUT | OPT_OUTPUT,NULL,NULL,OFFSET(frame_rates) ,"set frame rate (Hz value, fraction or abbreviation)", "rate" },
		{ "s",            OPT_VIDEO | HAS_ARG | OPT_SUBTITLE | OPT_STRING | OPT_SPEC | OPT_INPUT | OPT_OUTPUT,NULL,NULL, OFFSET(frame_sizes) ,"set frame size (WxH or abbreviation)", "size" },
		{ "aspect",       OPT_VIDEO | HAS_ARG | OPT_STRING | OPT_SPEC | OPT_OUTPUT,NULL,NULL, OFFSET(frame_aspect_ratios) ,"set aspect ratio (4:3, 16:9 or 1.3333, 1.7777)", "aspect" },
		{ "pix_fmt",      OPT_VIDEO | HAS_ARG | OPT_EXPERT | OPT_STRING | OPT_SPEC | OPT_INPUT | OPT_OUTPUT,NULL,NULL,OFFSET(frame_pix_fmts),"set pixel format", "format" },
		{ "bits_per_raw_sample", OPT_VIDEO | OPT_INT | HAS_ARG,&frame_bits_per_raw_sample,NULL,NULL,"set the number of bits per raw sample", "number" },
		{ "intra",        OPT_VIDEO | OPT_BOOL | OPT_EXPERT,&dummy, NULL,NULL,"deprecated use -g 1" },
		{ "vn",           OPT_VIDEO | OPT_BOOL | OPT_OFFSET | OPT_INPUT | OPT_OUTPUT,NULL,NULL, OFFSET(video_disable),"disable video" },
		{ "rc_override",  OPT_VIDEO | HAS_ARG | OPT_EXPERT | OPT_STRING | OPT_SPEC | OPT_OUTPUT,NULL,NULL, OFFSET(rc_overrides),"rate control override for specific intervals", "override" },
		{ "vcodec",       OPT_VIDEO | HAS_ARG | OPT_PERFILE | OPT_INPUT | OPT_OUTPUT,NULL, opt_video_codec , NULL,"force video codec ('copy' to copy stream)", "codec" },
		{ "sameq",        OPT_VIDEO | OPT_EXPERT ,NULL, opt_dummy , NULL,"Removed" },
		{ "same_quant",   OPT_VIDEO | OPT_EXPERT ,NULL, opt_dummy , NULL,"Removed" },
		{ "timecode",     OPT_VIDEO | HAS_ARG | OPT_PERFILE | OPT_OUTPUT,NULL, opt_timecode , NULL,"set initial TimeCode value.", "hh:mm:ss[:;.]ff" },
		{ "pass",         OPT_VIDEO | HAS_ARG | OPT_SPEC | OPT_INT | OPT_OUTPUT,NULL,NULL, OFFSET(pass),"select the pass number (1 to 3)", "n" },
		{ "passlogfile",  OPT_VIDEO | HAS_ARG | OPT_STRING | OPT_EXPERT | OPT_SPEC | OPT_OUTPUT,NULL,NULL, OFFSET(passlogfiles) ,"select two pass log file name prefix", "prefix" },
		{ "deinterlace",  OPT_VIDEO | OPT_BOOL | OPT_EXPERT,{ &do_deinterlace },NULL,NULL,"this option is deprecated, use the yadif filter instead" },
		{ "psnr",         OPT_VIDEO | OPT_BOOL | OPT_EXPERT,&dummy, NULL,NULL,"calculate PSNR of compressed frames" },
		{ "vstats",       OPT_VIDEO | OPT_EXPERT ,NULL, opt_dummy , NULL,"dump video coding statistics to file" },
		{ "vstats_file",  OPT_VIDEO | HAS_ARG | OPT_EXPERT ,NULL, opt_dummy , NULL,"dump video coding statistics to file", "file" },
		{ "vstats_version",  OPT_VIDEO | OPT_INT | HAS_ARG | OPT_EXPERT ,&dummy, NULL,NULL,"Version of the vstats format to use." },
		{ "vf",           OPT_VIDEO | HAS_ARG | OPT_PERFILE | OPT_OUTPUT,NULL, opt_video_filters , NULL,"set video filters", "filter_graph" },
		{ "intra_matrix", OPT_VIDEO | HAS_ARG | OPT_EXPERT | OPT_STRING | OPT_SPEC | OPT_OUTPUT,NULL,NULL, OFFSET(intra_matrices) ,"specify intra matrix coeffs", "matrix" },
		{ "inter_matrix", OPT_VIDEO | HAS_ARG | OPT_EXPERT | OPT_STRING | OPT_SPEC | OPT_OUTPUT,NULL,NULL,OFFSET(inter_matrices) ,"specify inter matrix coeffs", "matrix" },
		{ "chroma_intra_matrix", OPT_VIDEO | HAS_ARG | OPT_EXPERT | OPT_STRING | OPT_SPEC | OPT_OUTPUT,NULL,NULL, OFFSET(chroma_intra_matrices) ,"specify intra matrix coeffs", "matrix" },
		{ "top",          OPT_VIDEO | HAS_ARG | OPT_EXPERT | OPT_INT | OPT_SPEC | OPT_INPUT | OPT_OUTPUT,NULL,NULL,OFFSET(top_field_first) ,"top=1/bottom=0/auto=-1 field first", "" },
		{ "vtag",         OPT_VIDEO | HAS_ARG | OPT_EXPERT | OPT_PERFILE | OPT_INPUT | OPT_OUTPUT,NULL, opt_old2new , NULL,"force video tag/fourcc", "fourcc/tag" },
		{ "qphist",       OPT_VIDEO | OPT_BOOL | OPT_EXPERT ,&dummy, NULL,NULL,"show QP histogram" },
		{ "force_fps",    OPT_VIDEO | OPT_BOOL | OPT_EXPERT | OPT_SPEC | OPT_OUTPUT,NULL,NULL, OFFSET(force_fps) ,"force the selected framerate, disable the best supported framerate selection" },
		{ "streamid",     OPT_VIDEO | HAS_ARG | OPT_EXPERT | OPT_PERFILE | OPT_OUTPUT,NULL, opt_streamid , NULL,"set the value of an outfile streamid", "streamIndex:value" },
		{ "force_key_frames", OPT_VIDEO | OPT_STRING | HAS_ARG | OPT_EXPERT | OPT_SPEC | OPT_OUTPUT,NULL,NULL,OFFSET(forced_key_frames) ,"force key frames at specified timestamps", "timestamps" },
		{ "ab",           OPT_VIDEO | HAS_ARG | OPT_PERFILE | OPT_OUTPUT,NULL, opt_bitrate , NULL,"audio bitrate (please use -b:a)", "bitrate" },
		{ "b",            OPT_VIDEO | HAS_ARG | OPT_PERFILE | OPT_OUTPUT,NULL, opt_bitrate , NULL,"video bitrate (please use -b:v)", "bitrate" },
		{ "hwaccel",          OPT_VIDEO | OPT_STRING | HAS_ARG | OPT_EXPERT | OPT_SPEC | OPT_INPUT,NULL,NULL,OFFSET(hwaccels) ,"use HW accelerated decoding", "hwaccel name" },
		{ "hwaccel_device",   OPT_VIDEO | OPT_STRING | HAS_ARG | OPT_EXPERT | OPT_SPEC | OPT_INPUT,NULL,NULL, OFFSET(hwaccel_devices) ,"select a device for HW acceleration", "devicename" },
		{ "hwaccel_output_format", OPT_VIDEO | OPT_STRING | HAS_ARG | OPT_EXPERT | OPT_SPEC | OPT_INPUT,NULL,NULL, OFFSET(hwaccel_output_formats) ,"select output format used with HW accelerated decoding", "format" },
#if CONFIG_VIDEOTOOLBOX
	{ "videotoolbox_pixfmt", HAS_ARG | OPT_STRING | OPT_EXPERT, &dummy ,NULL,NULL, "" },
#endif
	{ "hwaccels",         OPT_EXIT,NULL, show_help , NULL,"show available HW acceleration methods" },
	{ "autorotate",       HAS_ARG | OPT_BOOL | OPT_SPEC | OPT_EXPERT | OPT_INPUT,NULL,NULL,OFFSET(autorotate) ,"automatically insert correct rotate filters" },

		/* audio options */
	{ "aframes",        OPT_AUDIO | HAS_ARG | OPT_PERFILE | OPT_OUTPUT,NULL, opt_audio_frames , NULL,"set the number of audio frames to output", "number" },
	{ "aq",             OPT_AUDIO | HAS_ARG | OPT_PERFILE | OPT_OUTPUT,NULL, opt_audio_qscale , NULL,"set audio quality (codec-specific)", "quality", },
	{ "ar",             OPT_AUDIO | HAS_ARG | OPT_INT | OPT_SPEC | OPT_INPUT | OPT_OUTPUT,NULL,NULL, OFFSET(audio_sample_rate) ,"set audio sampling rate (in Hz)", "rate" },
	{ "ac",             OPT_AUDIO | HAS_ARG | OPT_INT | OPT_SPEC | OPT_INPUT | OPT_OUTPUT,NULL,NULL, OFFSET(audio_channels) ,"set number of audio channels", "channels" },
	{ "an",             OPT_AUDIO | OPT_BOOL | OPT_OFFSET | OPT_INPUT | OPT_OUTPUT,NULL,NULL, OFFSET(audio_disable) ,"disable audio" },
	{ "acodec",         OPT_AUDIO | HAS_ARG | OPT_PERFILE | OPT_INPUT | OPT_OUTPUT,NULL, opt_audio_codec , NULL,"force audio codec ('copy' to copy stream)", "codec" },
	{ "atag",           OPT_AUDIO | HAS_ARG | OPT_EXPERT | OPT_PERFILE | OPT_OUTPUT,NULL, opt_old2new , NULL,"force audio tag/fourcc", "fourcc/tag" },
	{ "vol",            OPT_AUDIO | HAS_ARG | OPT_INT,&audio_volume ,NULL,NULL,"change audio volume (256=normal)" , "volume" },
	{ "sample_fmt",     OPT_AUDIO | HAS_ARG | OPT_EXPERT | OPT_SPEC | OPT_STRING | OPT_INPUT | OPT_OUTPUT,NULL,NULL, OFFSET(sample_fmts) ,"set sample format", "format" },
	{ "channel_layout", OPT_AUDIO | HAS_ARG | OPT_EXPERT | OPT_PERFILE | OPT_INPUT | OPT_OUTPUT,NULL, opt_channel_layout , NULL,"set channel layout", "layout" },
	{ "af",             OPT_AUDIO | HAS_ARG | OPT_PERFILE | OPT_OUTPUT,NULL, opt_audio_filters , NULL,"set audio filters", "filter_graph" },
	{ "guess_layout_max", OPT_AUDIO | HAS_ARG | OPT_INT | OPT_SPEC | OPT_EXPERT | OPT_INPUT,NULL,NULL, OFFSET(guess_layout_max) ,"set the maximum number of channels to try to guess the channel layout" },

		/* subtitle options */
	{ "sn",     OPT_SUBTITLE | OPT_BOOL | OPT_OFFSET | OPT_INPUT | OPT_OUTPUT,NULL,NULL, OFFSET(subtitle_disable),"disable subtitle" },
	{ "scodec", OPT_SUBTITLE | HAS_ARG | OPT_PERFILE | OPT_INPUT | OPT_OUTPUT,NULL,  opt_subtitle_codec , NULL,"force subtitle codec ('copy' to copy stream)", "codec" },
	{ "stag",   OPT_SUBTITLE | HAS_ARG | OPT_EXPERT | OPT_PERFILE | OPT_OUTPUT,NULL, opt_old2new , NULL, "force subtitle tag/fourcc", "fourcc/tag" },
	{ "fix_sub_duration", OPT_BOOL | OPT_EXPERT | OPT_SUBTITLE | OPT_SPEC | OPT_INPUT,NULL,NULL,OFFSET(fix_sub_duration) ,"fix subtitles duration" },
	{ "canvas_size", OPT_SUBTITLE | HAS_ARG | OPT_STRING | OPT_SPEC | OPT_INPUT,NULL,NULL,OFFSET(canvas_sizes) ,"set canvas size (WxH or abbreviation)", "size" },

		/* grab options */
	{ "vc", HAS_ARG | OPT_EXPERT | OPT_VIDEO,NULL, opt_video_channel , NULL,"deprecated, use -channel", "channel" },
	{ "tvstd", HAS_ARG | OPT_EXPERT | OPT_VIDEO,NULL, opt_video_standard , NULL,"deprecated, use -standard", "standard" },
	{ "isync", OPT_BOOL | OPT_EXPERT,{ &input_sync },NULL,NULL, "this option is deprecated and does nothing", "" },

		/* muxer options */
	{ "muxdelay",   OPT_FLOAT | HAS_ARG | OPT_EXPERT | OPT_OFFSET | OPT_OUTPUT,NULL,NULL, OFFSET(mux_max_delay) ,"set the maximum demux-decode delay", "seconds" },
	{ "muxpreload", OPT_FLOAT | HAS_ARG | OPT_EXPERT | OPT_OFFSET | OPT_OUTPUT,NULL,NULL, OFFSET(mux_preload) ,"set the initial demux-decode delay", "seconds" },
	{ "sdp_file", HAS_ARG | OPT_EXPERT | OPT_OUTPUT,NULL, opt_dummy , NULL,"specify a file in which to print sdp information", "file" },

	{ "time_base", HAS_ARG | OPT_STRING | OPT_EXPERT | OPT_SPEC | OPT_OUTPUT,NULL,NULL,OFFSET(time_bases) ,"set the desired time base hint for output stream (1:24, 1:48000 or 0.04166, 2.0833e-5)", "ratio" },
	{ "enc_time_base", HAS_ARG | OPT_STRING | OPT_EXPERT | OPT_SPEC | OPT_OUTPUT,NULL,NULL, OFFSET(enc_time_bases) ,"set the desired time base for the encoder (1:24, 1:48000 or 0.04166, 2.0833e-5). "
		"two special values are defined - "
		"0 = use frame rate (video) or sample rate (audio),"
		"-1 = match source time base", "ratio" },

		{ "bsf", HAS_ARG | OPT_STRING | OPT_SPEC | OPT_EXPERT | OPT_OUTPUT,NULL,NULL,OFFSET(bitstream_filters) ,"A comma-separated list of bitstream filters", "bitstream_filters" },
		{ "absf", HAS_ARG | OPT_AUDIO | OPT_EXPERT | OPT_PERFILE | OPT_OUTPUT,NULL, opt_old2new , NULL,"deprecated", "audio bitstream_filters" },
		{ "vbsf", OPT_VIDEO | HAS_ARG | OPT_EXPERT | OPT_PERFILE | OPT_OUTPUT,NULL, opt_old2new , NULL,"deprecated", "video bitstream_filters" },

		{ "apre", HAS_ARG | OPT_AUDIO | OPT_EXPERT | OPT_PERFILE | OPT_OUTPUT,NULL, opt_preset , NULL,
		"set the audio options to the indicated preset", "preset" },
		{ "vpre", OPT_VIDEO | HAS_ARG | OPT_EXPERT | OPT_PERFILE | OPT_OUTPUT,NULL, opt_preset , NULL,
		"set the video options to the indicated preset", "preset" },
		{ "spre", HAS_ARG | OPT_SUBTITLE | OPT_EXPERT | OPT_PERFILE | OPT_OUTPUT,NULL, opt_preset , NULL,
		"set the subtitle options to the indicated preset", "preset" },
		{ "fpre", HAS_ARG | OPT_EXPERT | OPT_PERFILE | OPT_OUTPUT,NULL, opt_preset , NULL,
		"set options from indicated preset file", "filename" },

		{ "max_muxing_queue_size", HAS_ARG | OPT_INT | OPT_SPEC | OPT_EXPERT | OPT_OUTPUT,NULL,NULL, OFFSET(max_muxing_queue_size) ,"maximum number of packets that can be buffered while waiting for all streams to initialize", "packets" },

		/* data codec support */
		{ "dcodec", HAS_ARG | OPT_DATA | OPT_PERFILE | OPT_EXPERT | OPT_INPUT | OPT_OUTPUT,NULL, opt_data_codec , NULL,"force data codec ('copy' to copy stream)", "codec" },
		{ "dn", OPT_BOOL | OPT_VIDEO | OPT_OFFSET | OPT_INPUT | OPT_OUTPUT,NULL,NULL, OFFSET(data_disable) ,"disable data" },
		{ "qsv_device", HAS_ARG | OPT_STRING | OPT_EXPERT, &dummy ,NULL,NULL,"set QSV hardware device (DirectX adapter index, DRM path or X11 display name)", "device" },
		{ "init_hw_device", HAS_ARG | OPT_EXPERT,NULL, opt_dummy , NULL,"initialise hardware device", "args" },
		{ "filter_hw_device", HAS_ARG | OPT_EXPERT,NULL, opt_dummy , NULL,"set hardware device used when filtering", "device" },

		{ NULL, },
	};


};



#endif