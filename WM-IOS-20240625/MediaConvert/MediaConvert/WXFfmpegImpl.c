/*
LAV 版本ffmpeg ，视频转换
*/

#include "WXFFmpegConvertAPI.h"
#include "FfmpegDefines.h"
#ifdef _WIN32
#include "ffmpeg-config.h"
#else
#include "config.h"
#endif

static const char *const forced_keyframes_const_names[] = {
	"n",
	"n_forced",
	"prev_forced_n",
	"prev_forced_t",
	"t",
	NULL
};

static const enum AVPixelFormat mjpeg_formats[] ={
	AV_PIX_FMT_YUVJ420P, 
	AV_PIX_FMT_YUVJ422P,
	AV_PIX_FMT_YUVJ444P,
    AV_PIX_FMT_YUV420P,
    AV_PIX_FMT_YUV422P,
    AV_PIX_FMT_YUV444P,
    AV_PIX_FMT_NONE
};

static const enum AVPixelFormat ljpeg_formats[] ={ AV_PIX_FMT_BGR24 ,
    AV_PIX_FMT_BGRA ,
    AV_PIX_FMT_BGR0,
    AV_PIX_FMT_YUVJ420P,
    AV_PIX_FMT_YUVJ444P,
    AV_PIX_FMT_YUVJ422P,
    AV_PIX_FMT_YUV420P ,
    AV_PIX_FMT_YUV444P ,
    AV_PIX_FMT_YUV422P,
    AV_PIX_FMT_NONE
};

enum HWAccelID {
	HWACCEL_NONE = 0,
	HWACCEL_AUTO,
	HWACCEL_GENERIC,
	HWACCEL_VIDEOTOOLBOX,
	HWACCEL_QSV,
	HWACCEL_CUVID,
};

enum forced_keyframes_const {
	FKF_N,
	FKF_N_FORCED,
	FKF_PREV_FORCED_N,
	FKF_PREV_FORCED_T,
	FKF_T,
	FKF_NB
};

typedef enum {
	ENCODER_FINISHED = 1,
	MUXER_FINISHED = 2,
} OSTFinished;

enum OptGroup {
	GROUP_OUTFILE,
	GROUP_INFILE,
};

static const char *const frame_rates[] = { "25", "30000/1001", "24000/1001" };

typedef struct OptionGroupDef {
	const char *name;
	const char *sep;
	int flags;
} OptionGroupDef;

static const OptionGroupDef groups[2] = {
	{ "output url", NULL, OPT_OUTPUT },
	{ "input url", "i", OPT_INPUT },
};

typedef struct SpecifierOpt {
	char *specifier;
	union {
		uint8_t *str;
		int        i;
		int64_t  i64;
		uint64_t ui64;
		float      f;
		double   dbl;
	} u;
} SpecifierOpt;

typedef struct WXCtx WXCtx;
typedef struct OptionDef {
	const char *name;
	int flags;
	union {
		void *dst_ptr;
		int(*func_arg)(WXCtx * ,void *, const char *, const char *);
		size_t off;
	} u;
	const char *help;
	const char *argname;
} OptionDef;

typedef struct Option {
	const OptionDef  *opt;
	const char       *key;
	const char       *val;
} Option;

typedef struct HWAccel {
	const char *name;
	int(*init)(AVCodecContext *s);
	enum HWAccelID id;
	enum AVPixelFormat pix_fmt;
} HWAccel;

typedef struct HWDevice {
	char *name;
	enum AVHWDeviceType type;
	AVBufferRef *device_ref;
} HWDevice;

/* select an input stream for an output stream */
typedef struct StreamMap {
	int disabled;           /* 1 is this mapping is disabled by a negative map */
	int file_index;
	int stream_index;
	int sync_file_index;
	int sync_stream_index;
	char *linklabel;       /* name of an output link, for mapping lavfi outputs */
} StreamMap;

typedef struct {
	int  file_idx, stream_idx, channel_idx; // input
	int ofile_idx, ostream_idx;               // output
} AudioChannelMap;


static const HWAccel hwaccels[] = {
	{ 0 },
};

static int decode_interrupt_cb(void *ctx) {
	return 0;
}
static const AVIOInterruptCB int_cb = { decode_interrupt_cb, NULL };

static double get_rotation(AVStream *st) {
	uint8_t* displaymatrix = av_stream_get_side_data(st, AV_PKT_DATA_DISPLAYMATRIX, NULL);
	double theta = 0;
	if (displaymatrix) theta = -av_display_rotation_get((int32_t*)displaymatrix);
	theta -= 360 * floor(theta / 360 + 0.9 / 360);
	return theta;
}


static void *grow_array(void *array, int elem_size, int *size, int new_size) {
	if (*size < new_size) {
		uint8_t *tmp = av_realloc_array(array, new_size, elem_size);
		memset(tmp + *size*elem_size, 0, (new_size - *size) * elem_size);
		*size = new_size;
		return tmp;
	}
	return array;
}

static AVDictionary *strip_specifiers(AVDictionary *dict)
{
	AVDictionaryEntry *e = NULL;
	AVDictionary *ret = NULL;

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

static int opt_abort_on(WXCtx *octx, void *optctx, const char *opt, const char *arg) {	return 0;}
static int opt_sameq(WXCtx *octx, void *optctx, const char *opt, const char *arg) {	return AVERROR(EINVAL);}
static int opt_sdp_file(WXCtx *octx, void *optctx, const char *opt, const char *arg){return 0;}
static int opt_vstats_file(WXCtx *octx, void *optctx, const char *opt, const char *arg){return 0;}
static int opt_vstats(WXCtx *octx, void *optctx, const char *opt, const char *arg){	return 0;}
static int opt_progress(WXCtx *octx, void *optctx, const char *opt, const char *arg){	return 0;}
static int dummy = 0;
static int show_help(WXCtx *octx,void *optctx, const char *opt, const char *arg) {	return 0;}

double parse_number_or_die(const char *numstr, int type,
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

int64_t parse_time_or_die(const char *timestr, int is_duration) {
	int64_t us = 0;
	av_parse_time(&us, timestr, is_duration);
	return us;
}

static int opt_cpuflags(WXCtx *octx, void *optctx, const char *opt, const char *arg) { return 0; }
int opt_loglevel(WXCtx *octx, void *optctx, const char *opt, const char *arg) { return 0; }
static int opt_report(WXCtx *octx, const char *opt) { return 0; }
static int opt_max_alloc(WXCtx *octx, void *optctx, const char *opt, const char *arg) { return 0; }
static int opt_timelimit(WXCtx *octx, void *optctx, const char *opt, const char *arg) { return 0; }

static char get_media_type_char(enum AVMediaType type){
	switch (type) {
	case AVMEDIA_TYPE_VIDEO: return 'V';
	case AVMEDIA_TYPE_AUDIO: return 'A';
	case AVMEDIA_TYPE_DATA: return 'D';
	case AVMEDIA_TYPE_SUBTITLE: return 'S';
	case AVMEDIA_TYPE_ATTACHMENT:return 'T';
	default: return '?';
	}
}

static int compare_codec_desc(const void *a, const void *b){
	const AVCodecDescriptor * const *da = a;
	const AVCodecDescriptor * const *db = b;
	return (*da)->type != (*db)->type ? FFDIFFSIGN((*da)->type, (*db)->type) :
		strcmp((*da)->name, (*db)->name);
}

static AVRational duration_max(int64_t tmp, int64_t *duration, AVRational tmp_time_base, AVRational time_base) {
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

typedef struct OptionGroup {
	const OptionGroupDef *group_def;
	const char *arg;
	Option *opts;
	int  nb_opts;
	AVDictionary *codec_opts;
	AVDictionary *format_opts;
	AVDictionary *resample_opts;
	AVDictionary *sws_dict;
	AVDictionary *swr_opts;
} OptionGroup;

typedef struct OptionGroupList {
	const OptionGroupDef *group_def;
	OptionGroup *groups;
	int       nb_groups;
} OptionGroupList;

typedef struct  InputStream InputStream;
typedef struct  OutputStream OutputStream;
typedef struct  InputFile InputFile;
typedef struct  OutputFile OutputFile;
typedef struct  FilterGraph FilterGraph;

struct WXCtx {
	OptionGroup global_opts;

	OptionGroupList *groups;
	int           nb_groups;

	/* parsing state */
	OptionGroup cur_group;

	//全局数据
	AVBufferRef *hw_device_ctx;// = NULL;
	HWDevice *filter_hw_device;// = NULL;
	int nb_hw_devices;// = 0;
	HWDevice **hw_devices;// = NULL;
	int input_stream_potentially_available;// = 0;
	uint8_t *subtitle_out;// = 0;
	InputStream **input_streams;// = NULL;
	int nb_input_streams;// = 0;
	InputFile **input_files;// = NULL;
	int nb_input_files;// = 0;
	OutputStream **output_streams;// = NULL;
	int nb_output_streams;// = 0;
	OutputFile **output_files;// = NULL;
	int nb_output_files;// = 0;
	FilterGraph **filtergraphs;// = NULL;
	int nb_filtergraphs;// = 0;
	AVDictionary *sws_dict;// = NULL;
	AVDictionary *swr_opts;// = NULL;
	AVDictionary *format_opts;// = NULL;
	AVDictionary *codec_opts;// = NULL;
	AVDictionary *resample_opts;// = NULL;

	int received_sigterm;// = 0;//外部中断变量
	int audio_volume;// = 256;// 转换后的音量，默认256

	jmp_buf avffmpeg_jmpbuf;
	int     avffmpeg_state;// = FFMPEG_ERROR_OK;//未处理
	int64_t avffmpeg_pts_curr;// = 0;//
	int64_t avffmpeg_pts_total;// = 0;
	void *avffmpeg_owner;// = NULL;
	WXFfmpegOnEvent avffmpeg_onEvent;// = NULL;

	WXCTSTR avffmpeg_event_id;// = 0;
	int m_iVideoMode; //0 Fast 1 Normal 2 Best

	//struct SwsContext *m_swsCtx;
	//struct AVFrame *m_pYUV420P;

	onFfmpegVideoData m_cbVideo;
};

void avffmpeg_OnError(WXCtx *octx, int code) {
	if (octx->avffmpeg_onEvent) {
		octx->avffmpeg_onEvent(octx->avffmpeg_owner, octx->avffmpeg_event_id, code, WXFfmpegGetError(code));
	}
}

WXDELOGO_CAPI void    avffmpeg_setEventOwner(WXCtx *octx, void *owner) {
	octx->avffmpeg_owner = owner;
}

WXDELOGO_CAPI void    avffmpeg_setRotate(WXCtx *octx, int rotate) {

}

WXDELOGO_CAPI void    avffmpeg_setEvent(WXCtx *octx, WXFfmpegOnEvent cb) {
	octx->avffmpeg_onEvent = cb;
}

WXDELOGO_CAPI void    avffmpeg_setVideoCb(WXCtx *octx, onFfmpegVideoData cb) {
	octx->m_cbVideo = cb;
}

WXDELOGO_CAPI void avffmpeg_setEventID(WXCtx *octx, WXCTSTR szID) {
	if (octx->avffmpeg_event_id) {
		free(octx->avffmpeg_event_id);
		octx->avffmpeg_event_id = NULL;
	}
#ifdef _WIN32
	octx->avffmpeg_event_id = wcsdup(szID);
#else
    octx->avffmpeg_event_id = strdup(szID);
#endif
}

WXDELOGO_CAPI int  avffmpeg_getState(WXCtx *octx) {
	return octx->avffmpeg_state;
}

WXDELOGO_CAPI int64_t avffmpeg_getCurrTime(WXCtx *octx) {
	return octx->avffmpeg_pts_curr;
}

WXDELOGO_CAPI int64_t avffmpeg_getTotalTime(WXCtx *octx) {
	return octx->avffmpeg_pts_total;
}

WXDELOGO_CAPI WXCtx * avffmpeg_create() {
	WXCtx *octx = (WXCtx *)av_malloc(sizeof(WXCtx));
	octx->m_iVideoMode = 1;
	octx->groups = NULL;
	octx->nb_groups = 0;
	memset(&octx->global_opts, 0, sizeof(OptionGroup));
	memset(&octx->cur_group, 0, sizeof(OptionGroup));

	octx->hw_device_ctx= NULL;
	octx->filter_hw_device = NULL;
	octx->nb_hw_devices = 0;
	octx->hw_devices = NULL;
	octx->input_stream_potentially_available = 0;
	octx->subtitle_out = 0;
	octx->input_streams = NULL;
	octx->nb_input_streams = 0;
	octx->input_files = NULL;
	octx->nb_input_files = 0;
	octx->output_streams= NULL;
	octx->nb_output_streams = 0;
	octx->output_files = NULL;
	octx->nb_output_files= 0;
	octx->filtergraphs = NULL;
	octx->nb_filtergraphs = 0;
	octx->sws_dict = NULL;
	octx->swr_opts = NULL;
	octx->format_opts = NULL;
	octx->codec_opts = NULL;
	octx->resample_opts = NULL;

	octx->received_sigterm = 0;//外部中断变量
	octx->avffmpeg_pts_curr = 0;//
	octx->avffmpeg_pts_total = 0;
	octx->avffmpeg_owner = NULL;
	octx->avffmpeg_onEvent = NULL;
	octx->avffmpeg_event_id = 0;
	octx->avffmpeg_state = FFMPEG_ERROR_PROCESS;//运行
	octx->audio_volume = 256;
	octx->m_cbVideo = NULL;
	return octx;
}

WXDELOGO_CAPI void    avffmpeg_destroy(WXCtx *octx) {
	if (octx) {
		if (octx->avffmpeg_event_id) {
			free(octx->avffmpeg_event_id);
			octx->avffmpeg_event_id = NULL;
		}
		av_free(octx);
	}
}


WXDELOGO_CAPI void avffmpeg_set_video_encode_mode(WXCtx *octx, int mode) {
	if (octx) {
		octx->m_iVideoMode = av_clip(mode,0,2);
	}
}


void avffmpeg_interrupt(WXCtx *octx) {
	octx->received_sigterm = 1;
	avffmpeg_OnError(octx, FFMPEG_ERROR_BREADK);//用戶中断
}


typedef struct OptionsContext {
	OptionGroup *g;

	/* input/output options */
	int64_t start_time;
	int64_t start_time_eof;
	int seek_timestamp;
	const char *format;

	SpecifierOpt *codec_names;
	int        nb_codec_names;
	SpecifierOpt *audio_channels;
	int        nb_audio_channels;
	SpecifierOpt *audio_sample_rate;
	int        nb_audio_sample_rate;
	SpecifierOpt *frame_rates;
	int        nb_frame_rates;
	SpecifierOpt *frame_sizes;
	int        nb_frame_sizes;
	SpecifierOpt *frame_pix_fmts;
	int        nb_frame_pix_fmts;

	/* input options */
	int64_t input_ts_offset;
	int loop;
	int rate_emu;
	int accurate_seek;
	int thread_queue_size;

	SpecifierOpt *ts_scale;
	int        nb_ts_scale;
	SpecifierOpt *dump_attachment;
	int        nb_dump_attachment;
	SpecifierOpt *hwaccels;
	int        nb_hwaccels;
	SpecifierOpt *hwaccel_devices;
	int        nb_hwaccel_devices;
	SpecifierOpt *hwaccel_output_formats;
	int        nb_hwaccel_output_formats;
	SpecifierOpt *autorotate;
	int        nb_autorotate;

	/* output options */
	StreamMap *stream_maps;
	int     nb_stream_maps;
	AudioChannelMap *audio_channel_maps; /* one info entry per -map_channel */
	int           nb_audio_channel_maps; /* number of (valid) -map_channel settings */
	int metadata_global_manual;
	int metadata_streams_manual;
	int metadata_chapters_manual;
	const char **attachments;
	int       nb_attachments;

	int chapters_input_file;

	int64_t recording_time;
	int64_t stop_time;
	uint64_t limit_filesize;
	float mux_preload;
	float mux_max_delay;
	int shortest;
	int bitexact;

	int video_disable;
	int audio_disable;
	int subtitle_disable;
	int data_disable;

	/* indexed by output file stream index */
	int   *streamid_map;
	int nb_streamid_map;

	SpecifierOpt *metadata;
	int        nb_metadata;
	SpecifierOpt *max_frames;
	int        nb_max_frames;
	SpecifierOpt *bitstream_filters;
	int        nb_bitstream_filters;
	SpecifierOpt *codec_tags;
	int        nb_codec_tags;
	SpecifierOpt *sample_fmts;
	int        nb_sample_fmts;
	SpecifierOpt *qscale;
	int        nb_qscale;
	SpecifierOpt *forced_key_frames;
	int        nb_forced_key_frames;
	SpecifierOpt *force_fps;
	int        nb_force_fps;
	SpecifierOpt *frame_aspect_ratios;
	int        nb_frame_aspect_ratios;
	SpecifierOpt *rc_overrides;
	int        nb_rc_overrides;
	SpecifierOpt *intra_matrices;
	int        nb_intra_matrices;
	SpecifierOpt *inter_matrices;
	int        nb_inter_matrices;
	SpecifierOpt *chroma_intra_matrices;
	int        nb_chroma_intra_matrices;
	SpecifierOpt *top_field_first;
	int        nb_top_field_first;
	SpecifierOpt *metadata_map;
	int        nb_metadata_map;
	SpecifierOpt *presets;
	int        nb_presets;
	SpecifierOpt *copy_initial_nonkeyframes;
	int        nb_copy_initial_nonkeyframes;
	SpecifierOpt *copy_prior_start;
	int        nb_copy_prior_start;
	SpecifierOpt *filters;
	int        nb_filters;
	SpecifierOpt *filter_scripts;
	int        nb_filter_scripts;
	SpecifierOpt *reinit_filters;
	int        nb_reinit_filters;
	SpecifierOpt *fix_sub_duration;
	int        nb_fix_sub_duration;
	SpecifierOpt *canvas_sizes;
	int        nb_canvas_sizes;
	SpecifierOpt *pass;
	int        nb_pass;
	SpecifierOpt *passlogfiles;
	int        nb_passlogfiles;
	SpecifierOpt *max_muxing_queue_size;
	int        nb_max_muxing_queue_size;
	SpecifierOpt *guess_layout_max;
	int        nb_guess_layout_max;
	SpecifierOpt *apad;
	int        nb_apad;
	SpecifierOpt *discard;
	int        nb_discard;
	SpecifierOpt *disposition;
	int        nb_disposition;
	SpecifierOpt *program;
	int        nb_program;
	SpecifierOpt *time_bases;
	int        nb_time_bases;
	SpecifierOpt *enc_time_bases;
	int        nb_enc_time_bases;

	WXCtx *octx;
} OptionsContext;

//输入Filter
typedef struct InputFilter {
	AVFilterContext    *filter;
	struct InputStream *ist;
	struct FilterGraph *graph;
	uint8_t            *name;
	enum AVMediaType    type;   // AVMEDIA_TYPE_SUBTITLE for sub2video

	AVFifoBuffer *frame_queue;

	// parameters configured for this input
	int format;

	int width, height;
	AVRational sample_aspect_ratio;

	int sample_rate;
	int channels;
	uint64_t channel_layout;

	AVBufferRef *hw_frames_ctx;

	int eof;
	WXCtx *octx;
} InputFilter;

//输出Filter
typedef struct OutputFilter {
	AVFilterContext     *filter;
	struct OutputStream *ost;
	struct FilterGraph  *graph;
	uint8_t             *name;

	/* temporary storage until stream maps are processed */
	AVFilterInOut       *out_tmp;
	enum AVMediaType     type;

	/* desired output stream properties */
	int width, height;
	AVRational frame_rate;
	int format;
	int sample_rate;
	uint64_t channel_layout;

	// those are only set if no format is specified and the encoder gives us multiple options
	int *formats;
	uint64_t *channel_layouts;
	int *sample_rates;
	WXCtx *octx;
} OutputFilter;

struct FilterGraph {
	int            index;
	const char    *graph_desc;

	AVFilterGraph *graph;
	int reconfiguration;

	InputFilter   **inputs;
	int          nb_inputs;
	OutputFilter **outputs;
	int         nb_outputs;
	WXCtx *octx;
};

struct InputStream {
	int file_index;
	AVStream *st;
	int discard;             /* true if stream data should be discarded */
	int user_set_discard;
	int decoding_needed;     /* non zero if the packets must be decoded in 'raw_fifo', see DECODING_FOR_* */

	AVCodecContext *dec_ctx;
	AVCodec *dec;
	AVFrame *decoded_frame;
	AVFrame *filter_frame; /* a ref of decoded_frame, to be sent to filters */

	int64_t       start;     /* time when read started */
							 /* predicted dts of the next packet read for this stream or (when there are
							 * several frames in a packet) of the next frame in current packet (in AV_TIME_BASE units) */
	int64_t       next_dts;
	int64_t       dts;       ///< dts of the last packet read for this stream (in AV_TIME_BASE units)

	int64_t       next_pts;  ///< synthetic pts for the next decode frame (in AV_TIME_BASE units)
	int64_t       pts;       ///< current pts of the decoded frame  (in AV_TIME_BASE units)
	int           wrap_correction_done;

	int64_t filter_in_rescale_delta_last;

	int64_t min_pts; /* pts with the smallest value in a current stream */
	int64_t max_pts; /* pts with the higher value in a current stream */

					 // when forcing constant input framerate through -r,
					 // this contains the pts that will be given to the next decoded frame
	int64_t cfr_next_pts;

	int64_t nb_samples; /* number of samples in the last decoded audio frame before looping */

	double ts_scale;
	int saw_first_ts;
	AVDictionary *decoder_opts;
	AVRational framerate;               /* framerate forced with -r */
	int top_field_first;
	int guess_layout_max;

	int autorotate;

	int fix_sub_duration;
	struct { /* previous decoded subtitle and related variables */
		int got_output;
		int ret;
		AVSubtitle subtitle;
	} prev_sub;

	struct sub2video {
		int64_t last_pts;
		int64_t end_pts;
		AVFifoBuffer *sub_queue;    ///< queue of AVSubtitle* before filter init
		AVFrame *frame;
		int w, h;
	} sub2video;

	int dr1;

	/* decoded data from this stream goes into all those filters
	* currently video and audio only */
	InputFilter **filters;
	int        nb_filters;

	int reinit_filters;

	/* hwaccel options */
	enum HWAccelID hwaccel_id;
	enum AVHWDeviceType hwaccel_device_type;
	char  *hwaccel_device;
	enum AVPixelFormat hwaccel_output_format;

	/* hwaccel context */
	void  *hwaccel_ctx;
	void(*hwaccel_uninit)(AVCodecContext *s);
	int(*hwaccel_get_buffer)(AVCodecContext *s, AVFrame *frame, int flags);
	int(*hwaccel_retrieve_data)(AVCodecContext *s, AVFrame *frame);
	enum AVPixelFormat hwaccel_pix_fmt;
	enum AVPixelFormat hwaccel_retrieved_pix_fmt;
	AVBufferRef *hw_frames_ctx;

	/* stats */
	// combined size of all the packets read
	uint64_t data_size;
	/* number of packets successfully read for this stream */
	uint64_t nb_packets;
	// number of frames/samples retrieved from the decoder
	uint64_t frames_decoded;
	uint64_t samples_decoded;

	int64_t *dts_buffer;
	int nb_dts_buffer;

	int got_output;
	WXCtx *octx;
};

struct InputFile {
	AVFormatContext *ctx;
	int eof_reached;      /* true if eof reached */
	int eagain;           /* true if last read attempt returned EAGAIN */
	int ist_index;        /* index of first stream in input_streams */
	int loop;             /* set number of times input stream should be looped */
	int64_t duration;     /* actual duration of the longest stream in a file
						  at the moment when looping happens */
	AVRational time_base; /* time base of the duration */
	int64_t input_ts_offset;

	int64_t ts_offset;
	int64_t last_ts;
	int64_t start_time;   /* user-specified start time in AV_TIME_BASE or AV_NOPTS_VALUE */
	int seek_timestamp;
	int64_t recording_time;
	int nb_streams;       /* number of stream that ffmpeg is aware of; may be different
						  from ctx.nb_streams if new streams appear during av_read_frame() */
	int nb_streams_warn;  /* number of streams that the user was warned of */
	int rate_emu;
	int accurate_seek;

#if HAVE_THREADS
	AVThreadMessageQueue *in_thread_queue;
	pthread_t thread;           /* thread reading from this file */
	int non_blocking;           /* reading packets from the thread should not block */
	int joined;                 /* the thread has been joined */
	int thread_queue_size;      /* maximum number of queued packets */
#endif
	WXCtx *octx;
};

struct OutputStream {
	int file_index;          /* file index */
	int index;               /* stream index in the output file */
	int source_index;        /* InputStream index */
	AVStream *st;            /* stream in the output file */
	int encoding_needed;     /* true if encoding needed for this stream */
	int frame_number;
	/* input pts and corresponding output pts
	for A/V sync */
	struct InputStream *sync_ist; /* input stream to sync against */
	int64_t sync_opts;       /* output frame counter, could be changed to some true timestamp */ // FIXME look at frame_number
																								 /* pts of the first frame encoded for this stream, used for limiting
																								 * recording time */
	int64_t first_pts;
	/* dts of the last packet sent to the muxer */
	int64_t last_mux_dts;
	// the timebase of the packets sent to the muxer
	AVRational mux_timebase;
	AVRational enc_timebase;

	int                    nb_bitstream_filters;
	AVBSFContext            **bsf_ctx;

	AVCodecContext *enc_ctx;
	AVCodecParameters *ref_par; /* associated input codec parameters with encoders options applied */
	AVCodec *enc;
	int64_t max_frames;
	AVFrame *filtered_frame;
	AVFrame *last_frame;
	int last_dropped;
	int last_nb0_frames[3];

	void  *hwaccel_ctx;

	/* video only */
	AVRational frame_rate;
	int is_cfr;
	int force_fps;
	int top_field_first;
	int rotate_overridden;
	double rotate_override_value;

	AVRational frame_aspect_ratio;

	/* forced key frames */
	int64_t *forced_kf_pts;
	int forced_kf_count;
	int forced_kf_index;
	char *forced_keyframes;
	AVExpr *forced_keyframes_pexpr;
	double forced_keyframes_expr_const_values[FKF_NB];

	/* audio only */
	int *audio_channels_map;             /* list of the channels id to pick from the source stream */
	int audio_channels_mapped;           /* number of channels in audio_channels_map */

	char *logfile_prefix;
	FILE *logfile;

	OutputFilter *filter;
	char *avfilter;
	char *filters;         ///< filtergraph associated to the -filter option
	char *filters_script;  ///< filtergraph script associated to the -filter_script option

	AVDictionary *encoder_opts;
	AVDictionary *sws_dict;
	AVDictionary *swr_opts;
	AVDictionary *resample_opts;
	char *apad;
	OSTFinished finished;        /* no more packets should be written for this stream */
	int unavailable;                     /* true if the steram is unavailable (possibly temporarily) */
	int stream_copy;

	// init_output_stream() has been called for this stream
	// The encoder and the bitstream filters have been initialized and the stream
	// parameters are set in the AVStream.
	int initialized;

	int inputs_done;

	const char *attachment_filename;
	int copy_initial_nonkeyframes;
	int copy_prior_start;
	char *disposition;

	int keep_pix_fmt;

	AVCodecParserContext *parser;
	AVCodecContext       *parser_avctx;

	/* stats */
	// combined size of all the packets written
	uint64_t data_size;
	// number of packets send to the muxer
	uint64_t packets_written;
	// number of frames/samples sent to the encoder
	uint64_t frames_encoded;
	uint64_t samples_encoded;

	/* packet quality factor */
	int quality;

	int max_muxing_queue_size;

	/* the packets are buffered here until the muxer is ready to be initialized */
	AVFifoBuffer *muxing_queue;

	/* packet picture type */
	int pict_type;

	/* frame encode sum of squared error values */
	int64_t error[4];
	WXCtx *octx;
};

struct OutputFile {
	AVFormatContext *ctx;
	AVDictionary *opts;
	int ost_index;       /* index of the first stream in output_streams */
	int64_t recording_time;  ///< desired length of the resulting file in microseconds == AV_TIME_BASE units
	int64_t start_time;      ///< start time in microseconds == AV_TIME_BASE units
	uint64_t limit_filesize; /* filesize limit expressed in bytes */

	int shortest;

	int header_written;
	WXCtx *octx;
};

extern const OptionDef ffmpeg_options[177];

void ffmpeg_cleanup(WXCtx *octx, int ret);
void exit_program(WXCtx *octx, int ret);

int parse_optgroup(WXCtx *octx, void *optctx, OptionGroup *g);

int split_commandline(WXCtx *octx, int argc, char *argv[],
	const OptionDef *options,
	const OptionGroupDef *groups, int nb_groups);

void uninit_parse_context(WXCtx *octx);

int check_stream_specifier(AVFormatContext *s, AVStream *st, const char *spec);

AVDictionary *filter_codec_opts(WXCtx *octx, AVDictionary *opts, enum AVCodecID codec_id,
	AVFormatContext *s, AVStream *st, AVCodec *codec);

AVDictionary **setup_find_stream_info_opts(WXCtx *octx, AVFormatContext *s,
	AVDictionary *codec_opts);

FILE *get_preset_file(char *filename, size_t filename_size,
	const char *preset_name, int is_path, const char *codec_name);

void *grow_array(void *array, int elem_size, int *size, int new_size);

void init_opts(WXCtx *octx);
void uninit_opts(WXCtx *octx);
int opt_cpuflags(WXCtx *octx, void *optctx, const char *opt, const char *arg);
int opt_default(WXCtx *octx, void *optctx, const char *opt, const char *arg);
int opt_loglevel(WXCtx *octx, void *optctx, const char *opt, const char *arg);
int opt_report(WXCtx *octx, const char *opt);
int opt_max_alloc(WXCtx *octx, void *optctx, const char *opt, const char *arg);
int opt_timelimit(WXCtx *octx, void *optctx, const char *opt, const char *arg);
void parse_options(WXCtx *octx, void *optctx, int argc, char **argv, const OptionDef *options, void(*parse_arg_function)(void *, const char*));
int parse_option(WXCtx *octx, void *optctx, const char *opt, const char *arg, const OptionDef *options);
static int ifilter_has_all_input_formats(FilterGraph *fg);
void remove_avoptions(AVDictionary **a, AVDictionary *b);
void assert_avoptions(WXCtx *octx, AVDictionary *m);
int guess_input_channel_layout(InputStream *ist);
enum AVPixelFormat choose_pixel_fmt(AVStream *st, AVCodecContext *avctx, AVCodec *codec, enum AVPixelFormat target);
void choose_sample_fmt(AVStream *st, AVCodec *codec);
int configure_filtergraph(FilterGraph *fg);
int configure_output_filter(FilterGraph *fg, OutputFilter *ofilter, AVFilterInOut *out);
void check_filter_outputs(WXCtx *octx);
int ist_in_filtergraph(FilterGraph *fg, InputStream *ist);
int filtergraph_is_simple(FilterGraph *fg);
int init_simple_filtergraph(InputStream *ist, OutputStream *ost);
int init_complex_filtergraph(FilterGraph *fg);
void sub2video_update(InputStream *ist, AVSubtitle *sub);
int ifilter_parameters_from_frame(InputFilter *ifilter, const AVFrame *frame);
int ffmpeg_parse_options(WXCtx *octx, int argc, char **argv);

HWDevice *hw_device_get_by_name(WXCtx *octx, const char *name);
int hw_device_init_from_string(WXCtx *octx, const char *arg, HWDevice **dev);
void hw_device_free_all(WXCtx *octx);
int hw_device_setup_for_decode(InputStream *ist);
int hw_device_setup_for_encode(OutputStream *ost);
int hwaccel_decode_init(AVCodecContext *avctx);
static void uninit_options(OptionsContext *o)
{
	const OptionDef *po = ffmpeg_options;
	int i;


	while (po->name) {
		void *dst = (uint8_t*)o + po->u.off;

		if (po->flags & OPT_SPEC) {
			SpecifierOpt **so = dst;
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
static void init_options(OptionsContext *o) {
	memset(o, 0, sizeof(OptionsContext));
	o->stop_time = INT64_MAX;
	o->mux_max_delay = 0.7f;
	o->start_time = AV_NOPTS_VALUE;
	o->start_time_eof = AV_NOPTS_VALUE;
	o->recording_time = INT64_MAX;
	o->limit_filesize = UINT64_MAX;
	o->chapters_input_file = INT_MAX;
	o->accurate_seek = 1;
}

static int opt_video_channel(WXCtx *octx, void *optctx, const char *opt, const char *arg) {
	OptionsContext *o = optctx;
	return opt_default(octx, o, "channel", arg);
}

static int opt_video_standard(WXCtx *octx, void *optctx, const char *opt, const char *arg) {
	OptionsContext *o = optctx;
	return opt_default(octx, o, "standard", arg);
}

static int opt_audio_codec(WXCtx *octx, void *optctx, const char *opt, const char *arg) {
	OptionsContext *o = optctx;
	return parse_option(octx, o, "codec:a", arg, ffmpeg_options);
}

static int opt_video_codec(WXCtx *octx, void *optctx, const char *opt, const char *arg)
{
	OptionsContext *o = optctx;
	return parse_option(octx, o, "codec:v", arg, ffmpeg_options);
}

static int opt_subtitle_codec(WXCtx *octx, void *optctx, const char *opt, const char *arg)
{
	OptionsContext *o = optctx;
	return parse_option(octx, o, "codec:s", arg, ffmpeg_options);
}

static int opt_data_codec(WXCtx *octx, void *optctx, const char *opt, const char *arg)
{
	OptionsContext *o = optctx;
	return parse_option(octx, o, "codec:d", arg, ffmpeg_options);
}

static int opt_map(WXCtx *octx, void *optctx, const char *opt, const char *arg)
{
	OptionsContext *o = optctx;
	StreamMap *m = NULL;
	int i, negative = 0, file_idx;
	int sync_file_idx = -1, sync_stream_idx = 0;
	char *p, *sync;
	char *map;
	char *allow_unused = 0;

	if (*arg == '-') {
		negative = 1;
		arg++;
	}
	map = av_strdup(arg);
	if (!map)
		return AVERROR(ENOMEM);

	if (sync = strchr(map, ',')) {
		*sync = 0;
		sync_file_idx = strtol(sync + 1, &sync, 0);
		if (sync_file_idx >= octx->nb_input_files || sync_file_idx < 0) {
			av_log(NULL, AV_LOG_FATAL, "Invalid sync file index: %d.\n", sync_file_idx);
			exit_program(octx, 1);
		}
		if (*sync)
			sync++;
		for (i = 0; i < octx->input_files[sync_file_idx]->nb_streams; i++)
			if (check_stream_specifier(octx->input_files[sync_file_idx]->ctx,
				octx->input_files[sync_file_idx]->ctx->streams[i], sync) == 1) {
				sync_stream_idx = i;
				break;
			}
		if (i == octx->input_files[sync_file_idx]->nb_streams) {
			av_log(NULL, AV_LOG_FATAL, "Sync stream specification in map %s does not "
				"match any streams.\n", arg);
			exit_program(octx, 1);
		}
	}


	if (map[0] == '[') {
		const char *c = map + 1;
		o->stream_maps = grow_array(o->stream_maps, sizeof(*o->stream_maps), &o->nb_stream_maps, o->nb_stream_maps + 1);
		m = &o->stream_maps[o->nb_stream_maps - 1];
		m->linklabel = av_get_token(&c, "]");
		if (!m->linklabel) {
			av_log(NULL, AV_LOG_ERROR, "Invalid output link label: %s.\n", map);
			exit_program(octx, 1);
		}
	}
	else {
		if (allow_unused = strchr(map, '?'))
			*allow_unused = 0;
		file_idx = strtol(map, &p, 0);
		if (file_idx >= octx->nb_input_files || file_idx < 0) {
			av_log(NULL, AV_LOG_FATAL, "Invalid input file index: %d.\n", file_idx);
			exit_program(octx, 1);
		}
		if (negative)
			for (i = 0; i < o->nb_stream_maps; i++) {
				m = &o->stream_maps[i];
				if (file_idx == m->file_index &&
					check_stream_specifier(octx->input_files[m->file_index]->ctx,
						octx->input_files[m->file_index]->ctx->streams[m->stream_index],
						*p == ':' ? p + 1 : p) > 0)
					m->disabled = 1;
			}
		else
			for (i = 0; i < octx->input_files[file_idx]->nb_streams; i++) {

				if (check_stream_specifier(octx->input_files[file_idx]->ctx, octx->input_files[file_idx]->ctx->streams[i],
					*p == ':' ? p + 1 : p) <= 0)
					continue;

				o->stream_maps = grow_array(o->stream_maps, sizeof(*o->stream_maps), &o->nb_stream_maps, o->nb_stream_maps + 1);
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
			exit_program(octx, 1);
		}
	}

	av_freep(&map);
	return 0;
}

static int opt_attach(WXCtx *octx, void *optctx, const char *opt, const char *arg) {
	OptionsContext *o = optctx;
	o->attachments = grow_array(o->attachments, sizeof(*o->attachments), &o->nb_attachments, o->nb_attachments + 1);
	o->attachments[o->nb_attachments - 1] = arg;
	return 0;
}

static int opt_map_channel(WXCtx *octx, void *optctx, const char *opt, const char *arg) {
	OptionsContext *o = optctx;
	int n;
	AVStream *st;
	AudioChannelMap *m;
	char *allow_unused = 0;
	char *mapchan;
	mapchan = av_strdup(arg);
	if (!mapchan)
		return AVERROR(ENOMEM);

	o->audio_channel_maps = grow_array(o->audio_channel_maps, sizeof(*o->audio_channel_maps), &o->nb_audio_channel_maps, o->nb_audio_channel_maps + 1);
	m = &o->audio_channel_maps[o->nb_audio_channel_maps - 1];

	n = sscanf(arg, "%d:%d.%d", &m->channel_idx, &m->ofile_idx, &m->ostream_idx);
	if ((n == 1 || n == 3) && m->channel_idx == -1) {
		m->file_idx = m->stream_idx = -1;
		if (n == 1)
			m->ofile_idx = m->ostream_idx = -1;
		av_free(mapchan);
		return 0;
	}

	n = sscanf(arg, "%d.%d.%d:%d.%d",
		&m->file_idx, &m->stream_idx, &m->channel_idx,
		&m->ofile_idx, &m->ostream_idx);

	if (n != 3 && n != 5) {
		exit_program(octx, 1);
	}

	if (n != 5)
		m->ofile_idx = m->ostream_idx = -1;

	if (m->file_idx < 0 || m->file_idx >= octx->nb_input_files) {
		exit_program(octx, 1);
	}

	if (m->stream_idx < 0 ||
		m->stream_idx >= octx->input_files[m->file_idx]->nb_streams) {
		exit_program(octx, 1);
	}

	st = octx->input_files[m->file_idx]->ctx->streams[m->stream_idx];
	if (st->codecpar->codec_type != AVMEDIA_TYPE_AUDIO) {
		exit_program(octx, 1);
	}

	if (allow_unused = strchr(mapchan, '?'))
		*allow_unused = 0;

	if (m->channel_idx < 0 || m->channel_idx >= st->codecpar->channels) {
		if (!allow_unused) {
			exit_program(octx, 1);
		}
	}

	av_free(mapchan);
	return 0;
}

static int opt_init_hw_device(WXCtx *octx, void *optctx, const char *opt, const char *arg) {
	OptionsContext *o = optctx;
	if (!strcmp(arg, "list")) {
		enum AVHWDeviceType type = AV_HWDEVICE_TYPE_NONE;
		//WXLogWriteNew("Supported hardware device types:\n");
		while ((type = av_hwdevice_iterate_types(type)) !=
			AV_HWDEVICE_TYPE_NONE)
			//WXLogWriteNew("%s\n", av_hwdevice_get_type_name(type));
		//WXLogWriteNew("\n");
		exit_program(octx, 0);
	}
	else {
		return hw_device_init_from_string(octx, arg, NULL);
	}
	return 0;
}

static int opt_filter_hw_device(WXCtx *octx, void *optctx, const char *opt, const char *arg) {
	OptionsContext *o = optctx;
	if (octx->filter_hw_device) {
		av_log(NULL, AV_LOG_ERROR, "Only one filter device can be used.\n");
		return AVERROR(EINVAL);
	}
	octx->filter_hw_device = hw_device_get_by_name(octx, arg);
	if (!octx->filter_hw_device) {
		av_log(NULL, AV_LOG_ERROR, "Invalid filter device %s.\n", arg);
		return AVERROR(EINVAL);
	}
	return 0;
}

static void parse_meta_type(WXCtx *octx, char *arg, char *type, int *index, const char **stream_spec) {
	if (*arg) {
		*type = *arg;
		switch (*arg) {
		case 'g':
			break;
		case 's':
			if (*(++arg) && *arg != ':') {
				av_log(NULL, AV_LOG_FATAL, "Invalid metadata specifier %s.\n", arg);
				exit_program(octx, 1);
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
			exit_program(octx, 1);
		}
	}
	else
		*type = 'g';
}

static int copy_metadata(WXCtx *octx, char *outspec, char *inspec, AVFormatContext *oc, AVFormatContext *ic, OptionsContext *o) {
	AVDictionary **meta_in = NULL;
	AVDictionary **meta_out = NULL;
	int i, ret = 0;
	char type_in, type_out;
	const char *istream_spec = NULL, *ostream_spec = NULL;
	int idx_in = 0, idx_out = 0;

	parse_meta_type(octx, inspec, &type_in, &idx_in, &istream_spec);
	parse_meta_type(octx, outspec, &type_out, &idx_out, &ostream_spec);

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


	if (!ic)
		return 0;

	switch (type_in) { case 'g': meta_in = &ic->metadata; break; case 'c': if ((idx_in) < 0 || (idx_in) >= (ic->nb_chapters)) { av_log(NULL, AV_LOG_FATAL, "Invalid %s index %d while processing metadata maps.\n", ("chapter"), (idx_in)); exit_program(octx, 1); } meta_in = &ic->chapters[idx_in]->metadata; break; case 'p': if ((idx_in) < 0 || (idx_in) >= (ic->nb_programs)) { av_log(NULL, AV_LOG_FATAL, "Invalid %s index %d while processing metadata maps.\n", ("program"), (idx_in)); exit_program(octx, 1); } meta_in = &ic->programs[idx_in]->metadata; break; case 's': break; default: av_assert0(0); };
	switch (type_out) { case 'g': meta_out = &oc->metadata; break; case 'c': if ((idx_out) < 0 || (idx_out) >= (oc->nb_chapters)) { av_log(NULL, AV_LOG_FATAL, "Invalid %s index %d while processing metadata maps.\n", ("chapter"), (idx_out)); exit_program(octx, 1); } meta_out = &oc->chapters[idx_out]->metadata; break; case 'p': if ((idx_out) < 0 || (idx_out) >= (oc->nb_programs)) { av_log(NULL, AV_LOG_FATAL, "Invalid %s index %d while processing metadata maps.\n", ("program"), (idx_out)); exit_program(octx, 1); } meta_out = &oc->programs[idx_out]->metadata; break; case 's': break; default: av_assert0(0); };


	if (type_in == 's') {
		for (i = 0; i < ic->nb_streams; i++) {
			if ((ret = check_stream_specifier(ic, ic->streams[i], istream_spec)) > 0) {
				meta_in = &ic->streams[i]->metadata;
				break;
			}
			else if (ret < 0)
				exit_program(octx, 1);
		}
		if (!meta_in) {
			av_log(NULL, AV_LOG_FATAL, "Stream specifier %s does not match  any streams.\n", istream_spec);
			exit_program(octx, 1);
		}
	}

	if (type_out == 's') {
		for (i = 0; i < oc->nb_streams; i++) {
			if ((ret = check_stream_specifier(oc, oc->streams[i], ostream_spec)) > 0) {
				meta_out = &oc->streams[i]->metadata;
				av_dict_copy(meta_out, *meta_in, AV_DICT_DONT_OVERWRITE);
			}
			else if (ret < 0)
				exit_program(octx, 1);
		}
	}
	else
		av_dict_copy(meta_out, *meta_in, AV_DICT_DONT_OVERWRITE);

	return 0;
}

static int opt_recording_timestamp(WXCtx *octx, void *optctx, const char *opt, const char *arg) {
	OptionsContext *o = optctx;
	char buf[128];
	int64_t recording_timestamp = parse_time_or_die(arg, 0) / 1E6;
	struct tm time = *gmtime((time_t*)&recording_timestamp);
	if (!strftime(buf, sizeof(buf), "creation_time=%Y-%m-%dT%H:%M:%S%z", &time))
		return -1;
	parse_option(octx, o, "metadata", buf, ffmpeg_options);
	return 0;
}

static AVCodec *find_codec_or_die(WXCtx *octx, const char *name, enum AVMediaType type, int encoder) {
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
		exit_program(octx, 1);
	}
	if (codec->type != type) {
		av_log(NULL, AV_LOG_FATAL, "Invalid %s type '%s'\n", codec_string, name);
		exit_program(octx, 1);
	}
	return codec;
}

static AVCodec *choose_decoder(OptionsContext *o, AVFormatContext *s, AVStream *st) {
	char *codec_name = NULL;

	{ int i, ret; for (i = 0; i < o->nb_codec_names; i++) {
		char *spec = o->codec_names[i].specifier;
		if ((ret = check_stream_specifier(s, st, spec)) > 0)
			codec_name = o->codec_names[i].u.str;
		else if (ret < 0)
			exit_program(o->octx, 1);
	}};
	if (codec_name) {
		AVCodec *codec = find_codec_or_die(o->octx, codec_name, st->codecpar->codec_type, 0);
		st->codecpar->codec_id = codec->id;
		return codec;
	}
	else
		return avcodec_find_decoder(st->codecpar->codec_id);
}

static void add_input_streams(OptionsContext *o, AVFormatContext *ic) {
	WXCtx *octx = o->octx;
	int i, ret;

	for (i = 0; i < ic->nb_streams; i++) {
		AVStream *st = ic->streams[i];
		AVCodecParameters *par = st->codecpar;
		InputStream *ist = av_mallocz(sizeof(InputStream));
		ist->octx = octx;
		char *framerate = NULL, *hwaccel_device = NULL;
		const char *hwaccel = NULL;
		char *hwaccel_output_format = NULL;
		char *codec_tag = NULL;
		char *next;
		char *discard_str = NULL;
		const AVClass *cc = avcodec_get_class();
		const AVOption *discard_opt = av_opt_find(&cc, "skip_frame", NULL, 0, 0);

		octx->input_streams = grow_array(octx->input_streams, sizeof(*octx->input_streams), &octx->nb_input_streams, octx->nb_input_streams + 1);
		octx->input_streams[octx->nb_input_streams - 1] = ist;

		ist->st = st;
		ist->file_index = octx->nb_input_files;
		ist->discard = 1;
		st->discard = AVDISCARD_ALL;
		ist->nb_samples = 0;
		ist->min_pts = INT64_MAX;
		ist->max_pts = INT64_MIN;

		ist->ts_scale = 1.0;
		{ int i, ret;
		for (i = 0; i < o->nb_ts_scale; i++) {
			char *spec = o->ts_scale[i].specifier;
			if ((ret = check_stream_specifier(ic, st, spec)) > 0)
				ist->ts_scale = o->ts_scale[i].u.dbl;
			else if (ret < 0)
				exit_program(octx, 1);
		}
		};

		ist->autorotate = 1;
		{ int i, ret;
		for (i = 0; i < o->nb_autorotate; i++) {
			char *spec = o->autorotate[i].specifier;
			if ((ret = check_stream_specifier(ic, st, spec)) > 0)
				ist->autorotate = o->autorotate[i].u.i;
			else if (ret < 0) exit_program(octx, 1);
		}
		};

		{ int i, ret;
		for (i = 0; i < o->nb_codec_tags; i++) {
			char *spec = o->codec_tags[i].specifier;
			if ((ret = check_stream_specifier(ic, st, spec)) > 0)
				codec_tag = o->codec_tags[i].u.str;
			else if (ret < 0) exit_program(octx, 1);
		}
		};

		if (codec_tag) {
			uint32_t tag = strtol(codec_tag, &next, 0);
			if (*next)
				tag = AV_RL32(codec_tag);
			st->codecpar->codec_tag = tag;
		}

		ist->dec = choose_decoder(o, ic, st);
		ist->decoder_opts = filter_codec_opts(octx, o->g->codec_opts, ist->st->codecpar->codec_id, ic, st, ist->dec);

		ist->reinit_filters = -1;
		{
			int i, ret;
			for (i = 0; i < o->nb_reinit_filters; i++) {
				char *spec = o->reinit_filters[i].specifier;
				if ((ret = check_stream_specifier(ic, st, spec)) > 0)
					ist->reinit_filters = o->reinit_filters[i].u.i;
				else if (ret < 0)
					exit_program(octx, 1);
			}
		};

		{
			int i, ret;
			for (i = 0; i < o->nb_discard; i++) {
				char *spec = o->discard[i].specifier;
				if ((ret = check_stream_specifier(ic, st, spec)) > 0)
					discard_str = o->discard[i].u.str;
				else if (ret < 0)
					exit_program(octx, 1);
			}
		};

		ist->user_set_discard = AVDISCARD_NONE;
		if (discard_str && av_opt_eval_int(&cc, discard_opt, discard_str, &ist->user_set_discard) < 0) {
			av_log(NULL, AV_LOG_ERROR, "Error parsing discard %s.\n",
				discard_str);
			exit_program(octx, 1);
		}

		ist->filter_in_rescale_delta_last = AV_NOPTS_VALUE;

		ist->dec_ctx = avcodec_alloc_context3(ist->dec);
		if (!ist->dec_ctx) {
			av_log(NULL, AV_LOG_ERROR, "Error allocating the decoder context.\n");
			exit_program(octx, 1);
		}

		ret = avcodec_parameters_to_context(ist->dec_ctx, par);
		if (ret < 0) {
			av_log(NULL, AV_LOG_ERROR, "Error initializing the decoder context.\n");
			exit_program(octx, 1);
		}

		if (o->bitexact)
			ist->dec_ctx->flags |= AV_CODEC_FLAG_BITEXACT;

		switch (par->codec_type) {
		case AVMEDIA_TYPE_VIDEO:
			if (!ist->dec)
				ist->dec = avcodec_find_decoder(par->codec_id);

			ist->dec_ctx->framerate = st->avg_frame_rate;

			{ int i, ret; for (i = 0; i < o->nb_frame_rates; i++) { char *spec = o->frame_rates[i].specifier; if ((ret = check_stream_specifier(ic, st, spec)) > 0) framerate = o->frame_rates[i].u.str; else if (ret < 0) exit_program(octx, 1); }};
			if (framerate && av_parse_video_rate(&ist->framerate,
				framerate) < 0) {
				av_log(NULL, AV_LOG_ERROR, "Error parsing framerate %s.\n",
					framerate);
				exit_program(octx, 1);
			}

			ist->top_field_first = -1;
			{ int i, ret; for (i = 0; i < o->nb_top_field_first; i++) { char *spec = o->top_field_first[i].specifier; if ((ret = check_stream_specifier(ic, st, spec)) > 0) ist->top_field_first = o->top_field_first[i].u.i; else if (ret < 0) exit_program(octx, 1); }};

			{ int i, ret; for (i = 0; i < o->nb_hwaccels; i++) { char *spec = o->hwaccels[i].specifier; if ((ret = check_stream_specifier(ic, st, spec)) > 0) hwaccel = o->hwaccels[i].u.str; else if (ret < 0) exit_program(octx, 1); }};
			if (hwaccel) {

				if (!strcmp(hwaccel, "nvdec"))
					hwaccel = "cuda";

				if (!strcmp(hwaccel, "none"))
					ist->hwaccel_id = HWACCEL_NONE;
				else if (!strcmp(hwaccel, "auto"))
					ist->hwaccel_id = HWACCEL_AUTO;
				else {
					enum AVHWDeviceType type;
					int i;
					for (i = 0; hwaccels[i].name; i++) {
						if (!strcmp(hwaccels[i].name, hwaccel)) {
							ist->hwaccel_id = hwaccels[i].id;
							break;
						}
					}

					if (!ist->hwaccel_id) {
						type = av_hwdevice_find_type_by_name(hwaccel);
						if (type != AV_HWDEVICE_TYPE_NONE) {
							ist->hwaccel_id = HWACCEL_GENERIC;
							ist->hwaccel_device_type = type;
						}
					}

					if (!ist->hwaccel_id) {
						av_log(NULL, AV_LOG_FATAL, "Unrecognized hwaccel: %s.\n",
							hwaccel);
						av_log(NULL, AV_LOG_FATAL, "Supported hwaccels: ");
						type = AV_HWDEVICE_TYPE_NONE;
						while ((type = av_hwdevice_iterate_types(type)) !=
							AV_HWDEVICE_TYPE_NONE)
							av_log(NULL, AV_LOG_FATAL, "%s ",
								av_hwdevice_get_type_name(type));
						for (i = 0; hwaccels[i].name; i++)
							av_log(NULL, AV_LOG_FATAL, "%s ", hwaccels[i].name);
						av_log(NULL, AV_LOG_FATAL, "\n");
						exit_program(octx, 1);
					}
				}
			}

			{ int i, ret; for (i = 0; i < o->nb_hwaccel_devices; i++) { char *spec = o->hwaccel_devices[i].specifier; if ((ret = check_stream_specifier(ic, st, spec)) > 0) hwaccel_device = o->hwaccel_devices[i].u.str; else if (ret < 0) exit_program(octx, 1); }};
			if (hwaccel_device) {
				ist->hwaccel_device = av_strdup(hwaccel_device);
				if (!ist->hwaccel_device)
					exit_program(octx, 1);
			}

			{ int i, ret; for (i = 0; i < o->nb_hwaccel_output_formats; i++) { char *spec = o->hwaccel_output_formats[i].specifier; if ((ret = check_stream_specifier(ic, st, spec)) > 0) hwaccel_output_format = o->hwaccel_output_formats[i].u.str; else if (ret < 0) exit_program(octx, 1); }}
			;
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
			{ int i, ret; for (i = 0; i < o->nb_guess_layout_max; i++) { char *spec = o->guess_layout_max[i].specifier; if ((ret = check_stream_specifier(ic, st, spec)) > 0) ist->guess_layout_max = o->guess_layout_max[i].u.i; else if (ret < 0) exit_program(octx, 1); }};
			guess_input_channel_layout(ist);
			break;
		case AVMEDIA_TYPE_DATA:
		case AVMEDIA_TYPE_SUBTITLE: {
			char *canvas_size = NULL;
			if (!ist->dec)
				ist->dec = avcodec_find_decoder(par->codec_id);
			{ int i, ret; for (i = 0; i < o->nb_fix_sub_duration; i++) { char *spec = o->fix_sub_duration[i].specifier; if ((ret = check_stream_specifier(ic, st, spec)) > 0) ist->fix_sub_duration = o->fix_sub_duration[i].u.i; else if (ret < 0) exit_program(octx, 1); }};
			{ int i, ret; for (i = 0; i < o->nb_canvas_sizes; i++) { char *spec = o->canvas_sizes[i].specifier; if ((ret = check_stream_specifier(ic, st, spec)) > 0) canvas_size = o->canvas_sizes[i].u.str; else if (ret < 0) exit_program(octx, 1); }};
			if (canvas_size &&
				av_parse_video_size(&ist->dec_ctx->width, &ist->dec_ctx->height, canvas_size) < 0) {
				av_log(NULL, AV_LOG_FATAL, "Invalid canvas size: %s.\n", canvas_size);
				exit_program(octx, 1);
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
			exit_program(octx, 1);
		}
	}
}

static void dump_attachment(WXCtx *octx, AVStream *st, const char *filename)
{
	int ret;
	AVIOContext *out = NULL;
	AVDictionaryEntry *e;

	if (!st->codecpar->extradata_size) {
		return;
	}
	if (!*filename && (e = av_dict_get(st->metadata, "filename", NULL, 0)))
		filename = e->value;
	if (!*filename) {
		exit_program(octx, 1);
	}

	if ((ret = avio_open2(&out, filename, AVIO_FLAG_WRITE, &int_cb, NULL)) < 0) {
		exit_program(octx, 1);
	}

	avio_write(out, st->codecpar->extradata, st->codecpar->extradata_size);
	avio_flush(out);
	avio_close(out);
}

static int open_input_file(WXCtx *octx, OptionsContext *o, const char *filename) {
	InputFile *f;
	AVFormatContext *ic;
	AVInputFormat *file_iformat = NULL;
	int err, i, ret;
	int64_t timestamp;
	AVDictionary *unused_opts = NULL;
	AVDictionaryEntry *e = NULL;
	char * video_codec_name = NULL;
	char * audio_codec_name = NULL;
	char *subtitle_codec_name = NULL;
	char * data_codec_name = NULL;
	int scan_all_pmts_set = 0;

	if (o->stop_time != INT64_MAX && o->recording_time != INT64_MAX) {
		o->stop_time = INT64_MAX;
		av_log(NULL, AV_LOG_WARNING, "-t and -to cannot be used together; using -t.\n");
	}

	if (o->stop_time != INT64_MAX && o->recording_time == INT64_MAX) {
		int64_t start_time = o->start_time == AV_NOPTS_VALUE ? 0 : o->start_time;
		if (o->stop_time <= start_time) {
			av_log(NULL, AV_LOG_ERROR, "-to value smaller than -ss; aborting.\n");
			exit_program(octx, 1);
		}
		else {
			o->recording_time = o->stop_time - start_time;
		}
	}

	if (o->format) {
		if (!(file_iformat = av_find_input_format(o->format))) {
			av_log(NULL, AV_LOG_FATAL, "Unknown input format: '%s'\n", o->format);
			exit_program(octx, 1);
		}
	}

	if (!strcmp(filename, "-"))
		filename = "pipe:";

	ic = avformat_alloc_context();
	if (!ic) {
		exit_program(octx, 1);
	}
	if (o->nb_audio_sample_rate) {
		av_dict_set_int(&o->g->format_opts, "sample_rate", o->audio_sample_rate[o->nb_audio_sample_rate - 1].u.i, 0);
	}
	if (o->nb_audio_channels) {
		if (file_iformat && file_iformat->priv_class &&
			av_opt_find(&file_iformat->priv_class, "channels", NULL, 0,
				AV_OPT_SEARCH_FAKE_OBJ)) {
			av_dict_set_int(&o->g->format_opts, "channels", o->audio_channels[o->nb_audio_channels - 1].u.i, 0);
		}
	}
	if (o->nb_frame_rates) {
		if (file_iformat && file_iformat->priv_class &&
			av_opt_find(&file_iformat->priv_class, "framerate", NULL, 0,
				AV_OPT_SEARCH_FAKE_OBJ)) {
			av_dict_set(&o->g->format_opts, "framerate",
				o->frame_rates[o->nb_frame_rates - 1].u.str, 0);
		}
	}
	if (o->nb_frame_sizes) {
		av_dict_set(&o->g->format_opts, "video_size", o->frame_sizes[o->nb_frame_sizes - 1].u.str, 0);
	}
	if (o->nb_frame_pix_fmts)
		av_dict_set(&o->g->format_opts, "pixel_format", o->frame_pix_fmts[o->nb_frame_pix_fmts - 1].u.str, 0);

	{ int i; for (i = 0; i < o->nb_codec_names; i++) { char *spec = o->codec_names[i].specifier; if (!strcmp(spec, "v")) video_codec_name = o->codec_names[i].u.str; }};
	{ int i; for (i = 0; i < o->nb_codec_names; i++) { char *spec = o->codec_names[i].specifier; if (!strcmp(spec, "a")) audio_codec_name = o->codec_names[i].u.str; }};
	{ int i; for (i = 0; i < o->nb_codec_names; i++) { char *spec = o->codec_names[i].specifier; if (!strcmp(spec, "s")) subtitle_codec_name = o->codec_names[i].u.str; }};
	{ int i; for (i = 0; i < o->nb_codec_names; i++) { char *spec = o->codec_names[i].specifier; if (!strcmp(spec, "d")) data_codec_name = o->codec_names[i].u.str; }};

	if (video_codec_name)
		ic->video_codec = find_codec_or_die(octx, video_codec_name, AVMEDIA_TYPE_VIDEO, 0);
	if (audio_codec_name)
		ic->audio_codec = find_codec_or_die(octx, audio_codec_name, AVMEDIA_TYPE_AUDIO, 0);
	if (subtitle_codec_name)
		ic->subtitle_codec = find_codec_or_die(octx, subtitle_codec_name, AVMEDIA_TYPE_SUBTITLE, 0);
	if (data_codec_name)
		ic->data_codec = find_codec_or_die(octx, data_codec_name, AVMEDIA_TYPE_DATA, 0);

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

	err = avformat_open_input(&ic, filename, file_iformat, &o->g->format_opts);
	if (err < 0) {
		if (err == AVERROR_PROTOCOL_NOT_FOUND)
			av_log(NULL, AV_LOG_ERROR, "Did you mean file:%s?\n", filename);
		exit_program(octx, 1);
	}
	if (scan_all_pmts_set)
		av_dict_set(&o->g->format_opts, "scan_all_pmts", NULL, AV_DICT_MATCH_CASE);
	remove_avoptions(&o->g->format_opts, o->g->codec_opts);
	assert_avoptions(octx, o->g->format_opts);


	for (i = 0; i < ic->nb_streams; i++)
		choose_decoder(o, ic, ic->streams[i]);

	if (1) {
		AVDictionary **opts = setup_find_stream_info_opts(octx, ic, o->g->codec_opts);
		int orig_nb_streams = ic->nb_streams;
		ret = avformat_find_stream_info(ic, opts);
		for (i = 0; i < orig_nb_streams; i++)
			av_dict_free(&opts[i]);
		av_freep(&opts);

		if (ret < 0) {
			av_log(NULL, AV_LOG_FATAL, "%s: could not find codec parameters\n", filename);
			if (ic->nb_streams == 0) {
				avformat_close_input(&ic);
				avformat_free_context(ic);
				exit_program(octx, 1);
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

	if (!o->seek_timestamp && ic->start_time != AV_NOPTS_VALUE)
		timestamp += ic->start_time;


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

	add_input_streams(o, ic);
	av_dump_format(ic, octx->nb_input_files, filename, 0);
	octx->input_files = grow_array(octx->input_files, sizeof(*octx->input_files), &octx->nb_input_files, octx->nb_input_files + 1);

	f = av_mallocz(sizeof(InputFile));
	f->octx = octx;
	octx->input_files[octx->nb_input_files - 1] = f;
	f->ctx = ic;
	f->ist_index = octx->nb_input_streams - ic->nb_streams;
	f->start_time = o->start_time;
	f->recording_time = o->recording_time;
	f->input_ts_offset = o->input_ts_offset;
	f->ts_offset = o->input_ts_offset - (0 ? (0 && ic->start_time != AV_NOPTS_VALUE ? ic->start_time : 0) : timestamp);
	f->nb_streams = ic->nb_streams;
	f->rate_emu = o->rate_emu;
	f->accurate_seek = o->accurate_seek;
	f->loop = o->loop;
	f->duration = 0;
	f->time_base = (AVRational) { 1, 1 };

	unused_opts = strip_specifiers(o->g->codec_opts);
	for (i = f->ist_index; i < octx->nb_input_streams; i++) {
		e = NULL;
		while ((e = av_dict_get(octx->input_streams[i]->decoder_opts, "", e,
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
			exit_program(octx, 1);
		}

	}
	av_dict_free(&unused_opts);

	for (i = 0; i < o->nb_dump_attachment; i++) {
		int j;

		for (j = 0; j < ic->nb_streams; j++) {
			AVStream *st = ic->streams[j];

			if (check_stream_specifier(ic, st, o->dump_attachment[i].specifier) == 1)
				dump_attachment(octx, st, o->dump_attachment[i].u.str);
		}
	}

	octx->input_stream_potentially_available = 1;

	return 0;
}

static uint8_t *get_line(WXCtx *octx, AVIOContext *s)
{
	AVIOContext *line;
	uint8_t *buf;
	char c;

	if (avio_open_dyn_buf(&line) < 0) {
		av_log(NULL, AV_LOG_FATAL, "Could not alloc buffer for reading preset.\n");
		exit_program(octx, 1);
	}

	while ((c = avio_r8(s)) && c != '\n')
		avio_w8(line, c);
	avio_w8(line, 0);
	avio_close_dyn_buf(line, &buf);

	return buf;
}

static int get_preset_file_2(const char *preset_name, const char *codec_name, AVIOContext **s)
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

static int choose_encoder(OptionsContext *o, AVFormatContext *s, OutputStream *ost)
{
	enum AVMediaType type = ost->st->codecpar->codec_type;
	char *codec_name = NULL;

	if (type == AVMEDIA_TYPE_VIDEO || type == AVMEDIA_TYPE_AUDIO || type == AVMEDIA_TYPE_SUBTITLE) {
		{ int i, ret;
		for (i = 0; i < o->nb_codec_names; i++) {
			char *spec = o->codec_names[i].specifier;
			if ((ret = check_stream_specifier(s, ost->st, spec)) > 0)
				codec_name = o->codec_names[i].u.str;
			else if (ret < 0)
				exit_program(o->octx, 1);
		}};
		if (!codec_name) {
			ost->st->codecpar->codec_id = av_guess_codec(s->oformat, NULL, s->url,
				NULL, ost->st->codecpar->codec_type);
			ost->enc = avcodec_find_encoder(ost->st->codecpar->codec_id);
			if (!ost->enc) {
				return AVERROR_ENCODER_NOT_FOUND;
			}
		}
		else if (!strcmp(codec_name, "copy"))
			ost->stream_copy = 1;
		else {
			ost->enc = find_codec_or_die(o->octx, codec_name, ost->st->codecpar->codec_type, 1);
			ost->st->codecpar->codec_id = ost->enc->id;
		}
		ost->encoding_needed = !ost->stream_copy;
	}
	else {

		ost->stream_copy = 1;
		ost->encoding_needed = 0;
	}

	return 0;
}

static OutputStream *new_output_stream(OptionsContext *o, AVFormatContext *oc, enum AVMediaType type, int source_index)
{
	WXCtx *octx = o->octx;
	OutputStream *ost;
	AVStream *st = avformat_new_stream(oc, NULL);
	int idx = oc->nb_streams - 1, ret = 0;
	const char *bsfs = NULL, *time_base = NULL;
	char *next, *codec_tag = NULL;
	double qscale = -1;
	int i;

	if (oc->nb_streams - 1 < o->nb_streamid_map)
		st->id = o->streamid_map[oc->nb_streams - 1];

	octx->output_streams = grow_array(octx->output_streams, sizeof(*octx->output_streams), &octx->nb_output_streams, octx->nb_output_streams + 1);

	ost = av_mallocz(sizeof(*ost));
	ost->octx = octx;

	octx->output_streams[octx->nb_output_streams - 1] = ost;

	ost->file_index = octx->nb_output_files - 1;
	ost->index = idx;
	ost->st = st;
	st->codecpar->codec_type = type;

	ret = choose_encoder(o, oc, ost);
	if (ret < 0) {
		exit_program(octx, 1);
	}

	ost->enc_ctx = avcodec_alloc_context3(ost->enc);
	ost->enc_ctx->codec_type = type;

	ost->ref_par = avcodec_parameters_alloc();

	if (ost->enc) {
		AVIOContext *s = NULL;
		char *buf = NULL, *arg = NULL, *preset = NULL;

		ost->encoder_opts = filter_codec_opts(octx, o->g->codec_opts, ost->enc->id, oc, st, ost->enc);

		{ int i, ret;
		for (i = 0; i < o->nb_presets; i++) {
			char *spec = o->presets[i].specifier;
			if ((ret = check_stream_specifier(oc, st, spec)) > 0)
				preset = o->presets[i].u.str;
			else if (ret < 0)
				exit_program(octx, 1);
		}
		};

		if (preset && (!(ret = get_preset_file_2(preset, ost->enc->name, &s)))) {
			do {
				buf = get_line(o->octx, s);
				if (!buf[0] || buf[0] == '#') {
					av_free(buf);
					continue;
				}
				if (!(arg = strchr(buf, '='))) {
					av_log(NULL, AV_LOG_FATAL, "Invalid line found in the preset file.\n");
					exit_program(octx, 1);
				}
				*arg++ = 0;
				av_dict_set(&ost->encoder_opts, buf, arg, AV_DICT_DONT_OVERWRITE);
				av_free(buf);
			} while (!s->eof_reached);
			avio_closep(&s);
		}
		if (ret) {
			exit_program(octx, 1);
		}
	}
	else {
		ost->encoder_opts = filter_codec_opts(octx, o->g->codec_opts, AV_CODEC_ID_NONE, oc, st, NULL);
	}


	if (o->bitexact)
		ost->enc_ctx->flags |= AV_CODEC_FLAG_BITEXACT;

	{
		int i, ret;
		for (i = 0; i < o->nb_time_bases; i++) {
			char *spec = o->time_bases[i].specifier;
			if ((ret = check_stream_specifier(oc, st, spec)) > 0)
				time_base = o->time_bases[i].u.str;
			else if (ret < 0)
				exit_program(octx, 1);
		}
	};

	if (time_base) {
		AVRational q;
		if (av_parse_ratio(&q, time_base, INT_MAX, 0, NULL) < 0 ||
			q.num <= 0 || q.den <= 0) {
			av_log(NULL, AV_LOG_FATAL, "Invalid time base: %s\n", time_base);
			exit_program(octx, 1);
		}
		st->time_base = q;
	}

	{
		int i, ret;
		for (i = 0; i < o->nb_enc_time_bases; i++) {
			char *spec = o->enc_time_bases[i].specifier;
			if ((ret = check_stream_specifier(oc, st, spec)) > 0)
				time_base = o->enc_time_bases[i].u.str;
			else if (ret < 0)
				exit_program(octx, 1);
		}
	};

	if (time_base) {
		AVRational q;
		if (av_parse_ratio(&q, time_base, INT_MAX, 0, NULL) < 0 ||
			q.den <= 0) {
			av_log(NULL, AV_LOG_FATAL, "Invalid time base: %s\n", time_base);
			exit_program(octx, 1);
		}
		ost->enc_timebase = q;
	}

	ost->max_frames = INT64_MAX;

	{
		int i, ret;
		for (i = 0; i < o->nb_max_frames; i++) {
			char *spec = o->max_frames[i].specifier;
			if ((ret = check_stream_specifier(oc, st, spec)) > 0)
				ost->max_frames = o->max_frames[i].u.i64;
			else if (ret < 0) exit_program(octx, 1);
		}
	};

	for (i = 0; i<o->nb_max_frames; i++) {
		char *p = o->max_frames[i].specifier;
		if (!*p && type != AVMEDIA_TYPE_VIDEO) {
			av_log(NULL, AV_LOG_WARNING, "Applying unspecific -frames to non video streams, maybe you meant -vframes ?\n");
			break;
		}
	}

	ost->copy_prior_start = -1;
	{
		int i, ret;
		for (i = 0; i < o->nb_copy_prior_start; i++) {
			char *spec = o->copy_prior_start[i].specifier;
			if ((ret = check_stream_specifier(oc, st, spec)) > 0)
				ost->copy_prior_start = o->copy_prior_start[i].u.i;
			else if (ret < 0)
				exit_program(octx, 1);
		}
	};

	{
		int i, ret;
		for (i = 0; i < o->nb_bitstream_filters; i++) {
			char *spec = o->bitstream_filters[i].specifier;
			if ((ret = check_stream_specifier(oc, st, spec)) > 0)
				bsfs = o->bitstream_filters[i].u.str;
			else if (ret < 0)
				exit_program(octx, 1);
		}
	};

	while (bsfs && *bsfs) {
		const AVBitStreamFilter *filter;
		char *bsf, *bsf_options_str, *bsf_name;

		bsf = av_get_token(&bsfs, ",");
		if (!bsf)
			exit_program(octx, 1);
		bsf_name = av_strtok(bsf, "=", &bsf_options_str);
		if (!bsf_name)
			exit_program(octx, 1);

		filter = av_bsf_get_by_name(bsf_name);
		if (!filter) {
			av_log(NULL, AV_LOG_FATAL, "Unknown bitstream filter %s\n", bsf_name);
			exit_program(octx, 1);
		}

		ost->bsf_ctx = av_realloc_array(ost->bsf_ctx,
			ost->nb_bitstream_filters + 1,
			sizeof(*ost->bsf_ctx));

		ret = av_bsf_alloc(filter, &ost->bsf_ctx[ost->nb_bitstream_filters]);

		ost->nb_bitstream_filters++;

		if (bsf_options_str && filter->priv_class) {
			const AVOption *opt = av_opt_next(ost->bsf_ctx[ost->nb_bitstream_filters - 1]->priv_data, NULL);
			const char * shorthand[2] = { NULL };

			if (opt)
				shorthand[0] = opt->name;

			ret = av_opt_set_from_string(ost->bsf_ctx[ost->nb_bitstream_filters - 1]->priv_data, bsf_options_str, shorthand, "=", ":");
			if (ret < 0) {
				av_log(NULL, AV_LOG_ERROR, "Error parsing options for bitstream filter %s\n", bsf_name);
				exit_program(octx, 1);
			}
		}
		av_freep(&bsf);

		if (*bsfs)
			bsfs++;
	}

	{
		int i, ret; for (i = 0; i < o->nb_codec_tags; i++) {
			char *spec = o->codec_tags[i].specifier;
			if ((ret = check_stream_specifier(oc, st, spec)) > 0)
				codec_tag = o->codec_tags[i].u.str;
			else if (ret < 0)
				exit_program(octx, 1);
		}
	};
	if (codec_tag) {
		uint32_t tag = strtol(codec_tag, &next, 0);
		if (*next)
			tag = AV_RL32(codec_tag);
		ost->st->codecpar->codec_tag =
			ost->enc_ctx->codec_tag = tag;
	}

	{ int i, ret;
	for (i = 0; i < o->nb_qscale; i++) {
		char *spec = o->qscale[i].specifier;
		if ((ret = check_stream_specifier(oc, st, spec)) > 0)
			qscale = o->qscale[i].u.dbl; else if (ret < 0)
			exit_program(octx, 1);
	}};

	if (qscale >= 0) {
		ost->enc_ctx->flags |= AV_CODEC_FLAG_QSCALE;
		ost->enc_ctx->global_quality = FF_QP2LAMBDA * qscale;
	}

	{ int i, ret; for (i = 0; i < o->nb_disposition; i++) {
		char *spec = o->disposition[i].specifier;
		if ((ret = check_stream_specifier(oc, st, spec)) > 0)
			ost->disposition = o->disposition[i].u.str;
		else if (ret < 0)
			exit_program(octx, 1);
	}};

	ost->disposition = av_strdup(ost->disposition);

	ost->max_muxing_queue_size = 128;

	{ int i, ret;
	for (i = 0; i < o->nb_max_muxing_queue_size; i++) {
		char *spec = o->max_muxing_queue_size[i].specifier;
		if ((ret = check_stream_specifier(oc, st, spec)) > 0)
			ost->max_muxing_queue_size = o->max_muxing_queue_size[i].u.i;
		else if (ret < 0)
			exit_program(octx, 1);
	}};

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
		ost->sync_ist = octx->input_streams[source_index];
		octx->input_streams[source_index]->discard = 0;
		octx->input_streams[source_index]->st->discard = octx->input_streams[source_index]->user_set_discard;
	}
	ost->last_mux_dts = AV_NOPTS_VALUE;
	ost->muxing_queue = av_fifo_alloc(8 * sizeof(AVPacket));
	return ost;
}

static void parse_matrix_coeffs(WXCtx *octx, uint16_t *dest, const char *str)
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
			exit_program(octx, 1);
		}
		p++;
	}
}

static uint8_t *read_file(const char *filename)
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

static char *get_ost_filters(OptionsContext *o, AVFormatContext *oc, OutputStream *ost) {
	AVStream *st = ost->st;
	if (ost->filters_script && ost->filters) {
		exit_program(o->octx, 1);
	}

	if (ost->filters_script)
		return (char *)read_file(ost->filters_script);
	else if (ost->filters)
		return av_strdup(ost->filters);

	return av_strdup(st->codecpar->codec_type == AVMEDIA_TYPE_VIDEO ?
		"null" : "anull");
}

static void check_streamcopy_filters(OptionsContext *o, AVFormatContext *oc, const OutputStream *ost, enum AVMediaType type) {
	if (ost->filters_script || ost->filters) {
		exit_program(o->octx, 1);
	}
}

static OutputStream *new_video_stream(OptionsContext *o, AVFormatContext *oc, int source_index) {
	AVStream *st;
	OutputStream *ost;
	AVCodecContext *video_enc;
	char *frame_rate = NULL, *frame_aspect_ratio = NULL;

	ost = new_output_stream(o, oc, AVMEDIA_TYPE_VIDEO, source_index);
	st = ost->st;
	video_enc = ost->enc_ctx;

	{ int i, ret; for (i = 0; i < o->nb_frame_rates; i++) {
		char *spec = o->frame_rates[i].specifier;
		if ((ret = check_stream_specifier(oc, st, spec)) > 0)
			frame_rate = o->frame_rates[i].u.str; else if (ret < 0) exit_program(o->octx, 1);
	}};
	if (frame_rate && av_parse_video_rate(&ost->frame_rate, frame_rate) < 0) {
		av_log(NULL, AV_LOG_FATAL, "Invalid framerate value: %s\n", frame_rate);
		exit_program(o->octx, 1);
	}

	{ int i, ret;
	for (i = 0; i < o->nb_frame_aspect_ratios; i++) {
		char *spec = o->frame_aspect_ratios[i].specifier;
		if ((ret = check_stream_specifier(oc, st, spec)) > 0)
			frame_aspect_ratio = o->frame_aspect_ratios[i].u.str;
		else if (ret < 0)
			exit_program(o->octx, 1);
	}};
	if (frame_aspect_ratio) {
		AVRational q;
		if (av_parse_ratio(&q, frame_aspect_ratio, 255, 0, NULL) < 0 ||
			q.num <= 0 || q.den <= 0) {
			av_log(NULL, AV_LOG_FATAL, "Invalid aspect ratio: %s\n", frame_aspect_ratio);
			exit_program(o->octx, 1);
		}
		ost->frame_aspect_ratio = q;
	}

	{ int i, ret;
	for (i = 0; i < o->nb_filter_scripts; i++) {
		char *spec = o->filter_scripts[i].specifier;
		if ((ret = check_stream_specifier(oc, st, spec)) > 0)
			ost->filters_script = o->filter_scripts[i].u.str;
		else if (ret < 0)
			exit_program(o->octx, 1);
	}
	};

	{ int i, ret;
	for (i = 0; i < o->nb_filters; i++) {
		char *spec = o->filters[i].specifier;
		if ((ret = check_stream_specifier(oc, st, spec)) > 0)
			ost->filters = o->filters[i].u.str;
		else if (ret < 0)
			exit_program(o->octx, 1);
	}
	};

	if (!ost->stream_copy) {
		const char *p = NULL;
		char *frame_size = NULL;
		char *frame_pix_fmt = NULL;
		char *intra_matrix = NULL, *inter_matrix = NULL;
		char *chroma_intra_matrix = NULL;
		int do_pass = 0;
		int i;

		{ int i, ret;
		for (i = 0; i < o->nb_frame_sizes; i++) {
			char *spec = o->frame_sizes[i].specifier;
			if ((ret = check_stream_specifier(oc, st, spec)) > 0)
				frame_size = o->frame_sizes[i].u.str;
			else if (ret < 0)
				exit_program(o->octx, 1);
		}
		};
		if (frame_size && av_parse_video_size(&video_enc->width, &video_enc->height, frame_size) < 0) {
			av_log(NULL, AV_LOG_FATAL, "Invalid frame size: %s.\n", frame_size);
			exit_program(o->octx, 1);
		}

		video_enc->bits_per_raw_sample = 0;
		{
			int i, ret;
			for (i = 0; i < o->nb_frame_pix_fmts; i++) {
				char *spec = o->frame_pix_fmts[i].specifier;
				if ((ret = check_stream_specifier(oc, st, spec)) > 0)
					frame_pix_fmt = o->frame_pix_fmts[i].u.str;
				else if (ret < 0)
					exit_program(o->octx, 1);
			}
		};

		if (frame_pix_fmt && *frame_pix_fmt == '+') {
			ost->keep_pix_fmt = 1;
			if (!*++frame_pix_fmt)
				frame_pix_fmt = NULL;
		}
		if (frame_pix_fmt && (video_enc->pix_fmt = av_get_pix_fmt(frame_pix_fmt)) == AV_PIX_FMT_NONE) {
			av_log(NULL, AV_LOG_FATAL, "Unknown pixel format requested: %s.\n", frame_pix_fmt);
			exit_program(o->octx, 1);
		}
		st->sample_aspect_ratio = video_enc->sample_aspect_ratio;

		{ int i, ret; for (i = 0; i < o->nb_intra_matrices; i++) {
			char *spec = o->intra_matrices[i].specifier;
			if ((ret = check_stream_specifier(oc, st, spec)) > 0)
				intra_matrix = o->intra_matrices[i].u.str;
			else if (ret < 0) exit_program(o->octx, 1);
		}};
		if (intra_matrix) {
			video_enc->intra_matrix = av_mallocz(sizeof(*video_enc->intra_matrix) * 64);
			parse_matrix_coeffs(o->octx, video_enc->intra_matrix, intra_matrix);
		}
		{ int i, ret; for (i = 0; i < o->nb_chroma_intra_matrices; i++) {
			char *spec = o->chroma_intra_matrices[i].specifier;
			if ((ret = check_stream_specifier(oc, st, spec)) > 0)
				chroma_intra_matrix = o->chroma_intra_matrices[i].u.str;
			else if (ret < 0)
				exit_program(o->octx, 1);
		}};
		if (chroma_intra_matrix) {
			uint16_t *p = av_mallocz(sizeof(*video_enc->chroma_intra_matrix) * 64);
			video_enc->chroma_intra_matrix = p;
			parse_matrix_coeffs(o->octx, p, chroma_intra_matrix);
		}
		{ int i, ret;
		for (i = 0; i < o->nb_inter_matrices; i++) {
			char *spec = o->inter_matrices[i].specifier;
			if ((ret = check_stream_specifier(oc, st, spec)) > 0)
				inter_matrix = o->inter_matrices[i].u.str;
			else if (ret < 0) exit_program(o->octx, 1);
		}};
		if (inter_matrix) {
			video_enc->inter_matrix = av_mallocz(sizeof(*video_enc->inter_matrix) * 64);
			parse_matrix_coeffs(o->octx, video_enc->inter_matrix, inter_matrix);
		}

		{ int i, ret; for (i = 0; i < o->nb_rc_overrides; i++) {
			char *spec = o->rc_overrides[i].specifier;
			if ((ret = check_stream_specifier(oc, st, spec)) > 0)
				p = o->rc_overrides[i].u.str;
			else if (ret < 0) exit_program(o->octx, 1);
		}};
		for (i = 0; p; i++) {
			int start, end, q;
			int e = sscanf(p, "%d,%d,%d", &start, &end, &q);
			if (e != 3) {
				av_log(NULL, AV_LOG_FATAL, "error parsing rc_override\n");
				exit_program(o->octx, 1);
			}
			video_enc->rc_override = av_realloc_array(video_enc->rc_override, i + 1, sizeof(RcOverride));
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

		{ int i, ret; for (i = 0; i < o->nb_pass; i++) {
			char *spec = o->pass[i].specifier;
			if ((ret = check_stream_specifier(oc, st, spec)) > 0)
				do_pass = o->pass[i].u.i; else if (ret < 0) exit_program(o->octx, 1);
		}};
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

		{ int i, ret; for (i = 0; i < o->nb_passlogfiles; i++) {
			char *spec = o->passlogfiles[i].specifier;
			if ((ret = check_stream_specifier(oc, st, spec)) > 0)
				ost->logfile_prefix = o->passlogfiles[i].u.str;
			else if (ret < 0) exit_program(o->octx, 1);
		}};
		if (ost->logfile_prefix &&
			!(ost->logfile_prefix = av_strdup(ost->logfile_prefix)))
			exit_program(o->octx, 1);

		{ int i, ret; for (i = 0; i < o->nb_forced_key_frames; i++) {
			char *spec = o->forced_key_frames[i].specifier;
			if ((ret = check_stream_specifier(oc, st, spec)) > 0)
				ost->forced_keyframes = o->forced_key_frames[i].u.str;
			else if (ret < 0) exit_program(o->octx, 1);
		}};
		if (ost->forced_keyframes)
			ost->forced_keyframes = av_strdup(ost->forced_keyframes);

		{ int i, ret; for (i = 0; i < o->nb_force_fps; i++) {
			char *spec = o->force_fps[i].specifier;
			if ((ret = check_stream_specifier(oc, st, spec)) > 0)
				ost->force_fps = o->force_fps[i].u.i;
			else if (ret < 0) exit_program(o->octx, 1);
		}};

		ost->top_field_first = -1;
		{ int i, ret; for (i = 0; i < o->nb_top_field_first; i++) {
			char *spec = o->top_field_first[i].specifier;
			if ((ret = check_stream_specifier(oc, st, spec)) > 0)
				ost->top_field_first = o->top_field_first[i].u.i;
			else if (ret < 0)
				exit_program(o->octx, 1);
		}};


		ost->avfilter = get_ost_filters(o, oc, ost);
		if (!ost->avfilter)
			exit_program(o->octx, 1);
	}
	else {
		{ int i, ret;
		for (i = 0; i < o->nb_copy_initial_nonkeyframes; i++) {
			char *spec = o->copy_initial_nonkeyframes[i].specifier;
			if ((ret = check_stream_specifier(oc, st, spec)) > 0)
				ost->copy_initial_nonkeyframes = o->copy_initial_nonkeyframes[i].u.i;
			else if (ret < 0)
				exit_program(o->octx, 1);
		}};
	}

	if (ost->stream_copy)
		check_streamcopy_filters(o, oc, ost, AVMEDIA_TYPE_VIDEO);

	return ost;
}

static OutputStream *new_audio_stream(OptionsContext *o, AVFormatContext *oc, int source_index)
{
	WXCtx *octx = o->octx;
	int n;
	AVStream *st;
	OutputStream *ost;
	AVCodecContext *audio_enc;

	ost = new_output_stream(o, oc, AVMEDIA_TYPE_AUDIO, source_index);
	st = ost->st;

	audio_enc = ost->enc_ctx;
	audio_enc->codec_type = AVMEDIA_TYPE_AUDIO;

	{ int i, ret; for (i = 0; i < o->nb_filter_scripts; i++) { char *spec = o->filter_scripts[i].specifier; if ((ret = check_stream_specifier(oc, st, spec)) > 0) ost->filters_script = o->filter_scripts[i].u.str; else if (ret < 0) exit_program(octx, 1); }};
	{ int i, ret; for (i = 0; i < o->nb_filters; i++) { char *spec = o->filters[i].specifier; if ((ret = check_stream_specifier(oc, st, spec)) > 0) ost->filters = o->filters[i].u.str; else if (ret < 0) exit_program(octx, 1); }};

	if (!ost->stream_copy) {
		char *sample_fmt = NULL;

		{ int i, ret; for (i = 0; i < o->nb_audio_channels; i++) { char *spec = o->audio_channels[i].specifier; if ((ret = check_stream_specifier(oc, st, spec)) > 0) audio_enc->channels = o->audio_channels[i].u.i; else if (ret < 0) exit_program(octx, 1); }};

		{ int i, ret; for (i = 0; i < o->nb_sample_fmts; i++) { char *spec = o->sample_fmts[i].specifier; if ((ret = check_stream_specifier(oc, st, spec)) > 0) sample_fmt = o->sample_fmts[i].u.str; else if (ret < 0) exit_program(octx, 1); }};
		if (sample_fmt &&
			(audio_enc->sample_fmt = av_get_sample_fmt(sample_fmt)) == AV_SAMPLE_FMT_NONE) {
			av_log(NULL, AV_LOG_FATAL, "Invalid sample format '%s'\n", sample_fmt);
			exit_program(octx, 1);
		}

		{ int i, ret; for (i = 0; i < o->nb_audio_sample_rate; i++) { char *spec = o->audio_sample_rate[i].specifier; if ((ret = check_stream_specifier(oc, st, spec)) > 0) audio_enc->sample_rate = o->audio_sample_rate[i].u.i; else if (ret < 0) exit_program(octx, 1); }};

		{ int i, ret; for (i = 0; i < o->nb_apad; i++) { char *spec = o->apad[i].specifier; if ((ret = check_stream_specifier(oc, st, spec)) > 0) ost->apad = o->apad[i].u.str; else if (ret < 0) exit_program(octx, 1); }};
		ost->apad = av_strdup(ost->apad);

		ost->avfilter = get_ost_filters(o, oc, ost);
		if (!ost->avfilter)
			exit_program(octx, 1);


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
					ist = octx->input_streams[ost->source_index];
				}

				if (!ist || (ist->file_index == map->file_idx && ist->st->index == map->stream_idx)) {
					av_reallocp_array(&ost->audio_channels_map, ost->audio_channels_mapped + 1, sizeof(*ost->audio_channels_map));
					ost->audio_channels_map[ost->audio_channels_mapped++] = map->channel_idx;
				}
			}
		}
	}

	if (ost->stream_copy)
		check_streamcopy_filters(o, oc, ost, AVMEDIA_TYPE_AUDIO);

	return ost;
}

static OutputStream *new_data_stream(OptionsContext *o, AVFormatContext *oc, int source_index)
{
	WXCtx *octx = o->octx;
	OutputStream *ost;

	ost = new_output_stream(o, oc, AVMEDIA_TYPE_DATA, source_index);
	if (!ost->stream_copy) {
		av_log(NULL, AV_LOG_FATAL, "Data stream encoding not supported yet (only streamcopy)\n");
		exit_program(octx, 1);
	}

	return ost;
}

static OutputStream *new_unknown_stream(OptionsContext *o, AVFormatContext *oc, int source_index)
{
	WXCtx *octx = o->octx;
	OutputStream *ost;

	ost = new_output_stream(o, oc, AVMEDIA_TYPE_UNKNOWN, source_index);
	if (!ost->stream_copy) {
		av_log(NULL, AV_LOG_FATAL, "Unknown stream encoding not supported yet (only streamcopy)\n");
		exit_program(octx, 1);
	}

	return ost;
}

static OutputStream *new_attachment_stream(OptionsContext *o, AVFormatContext *oc, int source_index)
{
	WXCtx *octx = o->octx;
	OutputStream *ost = new_output_stream(o, oc, AVMEDIA_TYPE_ATTACHMENT, source_index);
	ost->stream_copy = 1;
	ost->finished = 1;
	return ost;
}

static OutputStream *new_subtitle_stream(OptionsContext *o, AVFormatContext *oc, int source_index) {
	WXCtx *octx = o->octx;
	AVStream *st;
	OutputStream *ost;
	AVCodecContext *subtitle_enc;

	ost = new_output_stream(o, oc, AVMEDIA_TYPE_SUBTITLE, source_index);
	st = ost->st;
	subtitle_enc = ost->enc_ctx;

	subtitle_enc->codec_type = AVMEDIA_TYPE_SUBTITLE;

	{ int i, ret; for (i = 0; i < o->nb_copy_initial_nonkeyframes; i++) { char *spec = o->copy_initial_nonkeyframes[i].specifier; if ((ret = check_stream_specifier(oc, st, spec)) > 0) ost->copy_initial_nonkeyframes = o->copy_initial_nonkeyframes[i].u.i; else if (ret < 0) exit_program(octx, 1); }};

	if (!ost->stream_copy) {
		char *frame_size = NULL;

		{ int i, ret; for (i = 0; i < o->nb_frame_sizes; i++) { char *spec = o->frame_sizes[i].specifier; if ((ret = check_stream_specifier(oc, st, spec)) > 0) frame_size = o->frame_sizes[i].u.str; else if (ret < 0) exit_program(octx, 1); }};
		if (frame_size && av_parse_video_size(&subtitle_enc->width, &subtitle_enc->height, frame_size) < 0) {
			av_log(NULL, AV_LOG_FATAL, "Invalid frame size: %s.\n", frame_size);
			exit_program(octx, 1);
		}
	}

	return ost;
}

static int opt_streamid(WXCtx *octx, void *optctx, const char *opt, const char *arg)
{
	OptionsContext *o = optctx;
	int idx;
	char *p;
	char idx_str[16];

	av_strlcpy(idx_str, arg, sizeof(idx_str));
	p = strchr(idx_str, ':');
	if (!p) {
		av_log(NULL, AV_LOG_FATAL,
			"Invalid value '%s' for option '%s', required syntax is 'index:value'\n",
			arg, opt);
		exit_program(octx, 1);
	}
	*p++ = '\0';
	idx = parse_number_or_die(idx_str, OPT_INT, 0, MAX_STREAMS - 1);
	o->streamid_map = grow_array(o->streamid_map, sizeof(*o->streamid_map), &o->nb_streamid_map, idx + 1);
	o->streamid_map[idx] = parse_number_or_die(p, OPT_INT, 0, INT_MAX);
	return 0;
}

static int copy_chapters(InputFile *ifile, OutputFile *ofile, int copy_metadata)
{
	AVFormatContext *is = ifile->ctx;
	AVFormatContext *os = ofile->ctx;
	AVChapter **tmp;
	int i;

	tmp = av_realloc_f(os->chapters, is->nb_chapters + os->nb_chapters, sizeof(*os->chapters));
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
		out_ch = av_mallocz(sizeof(AVChapter));
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

static void init_output_filter(OutputFilter *ofilter, OptionsContext *o,
	AVFormatContext *oc)
{
	OutputStream *ost;

	switch (ofilter->type) {
	case AVMEDIA_TYPE_VIDEO: ost = new_video_stream(o, oc, -1); break;
	case AVMEDIA_TYPE_AUDIO: ost = new_audio_stream(o, oc, -1); break;
	default:
		av_log(NULL, AV_LOG_FATAL, "Only video and audio filters are supported "
			"currently.\n");
		exit_program(o->octx, 1);
	}

	ost->source_index = -1;
	ost->filter = ofilter;

	ofilter->ost = ost;
	ofilter->format = -1;

	if (ost->stream_copy) {
		exit_program(o->octx, 1);
	}

	if (ost->avfilter && (ost->filters || ost->filters_script)) {
		const char *opt = ost->filters ? "-vf/-af/-filter" : "-filter_script";
		exit_program(o->octx, 1);
	}

	avfilter_inout_free(&ofilter->out_tmp);
}

static int init_complex_filters(WXCtx *octx) {
	int i;
	for (i = 0; i < octx->nb_filtergraphs; i++) {
		int ret = init_complex_filtergraph(octx->filtergraphs[i]);
		if (ret < 0)
			return ret;
	}
	return 0;
}

static int open_output_file(WXCtx *octx, OptionsContext *o, const char *filename) {
	AVFormatContext *oc;
	int i, j, err;
	OutputFile *of;
	OutputStream *ost;
	InputStream *ist;
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
			exit_program(octx, 1);
		}
		else {
			o->recording_time = o->stop_time - start_time;
		}
	}

	octx->output_files = grow_array(octx->output_files, sizeof(*octx->output_files), &octx->nb_output_files, octx->nb_output_files + 1);
	of = av_mallocz(sizeof(*of));
	of->octx = octx;
	octx->output_files[octx->nb_output_files - 1] = of;
	of->ost_index = octx->nb_output_streams;
	of->recording_time = o->recording_time;
	of->start_time = o->start_time;
	of->limit_filesize = o->limit_filesize;
	of->shortest = o->shortest;
	av_dict_copy(&of->opts, o->g->format_opts, 0);

	if (!strcmp(filename, "-"))
		filename = "pipe:";

	err = avformat_alloc_output_context2(&oc, NULL, o->format, filename);
	if (!oc) {
		exit_program(octx, 1);//打开文件失败
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


	for (i = 0; i < octx->nb_filtergraphs; i++) {
		FilterGraph *fg = octx->filtergraphs[i];
		for (j = 0; j < fg->nb_outputs; j++) {
			OutputFilter *ofilter = fg->outputs[j];

			if (!ofilter->out_tmp || ofilter->out_tmp->name)
				continue;

			switch (ofilter->type) {
			case AVMEDIA_TYPE_VIDEO: o->video_disable = 1; break;
			case AVMEDIA_TYPE_AUDIO: o->audio_disable = 1; break;
			case AVMEDIA_TYPE_SUBTITLE: o->subtitle_disable = 1; break;
			}
			init_output_filter(ofilter, o, oc);
		}
	}

	if (!o->nb_stream_maps) {
		char *subtitle_codec_name = NULL;



		if (!o->video_disable && av_guess_codec(oc->oformat, NULL, filename, NULL, AVMEDIA_TYPE_VIDEO) != AV_CODEC_ID_NONE) {
			int area = 0, idx = -1;
			int qcr = avformat_query_codec(oc->oformat, oc->oformat->video_codec, 0);
			for (i = 0; i < octx->nb_input_streams; i++) {
				int new_area;
				ist = octx->input_streams[i];
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
				new_video_stream(o, oc, idx);
		}


		if (!o->audio_disable && av_guess_codec(oc->oformat, NULL, filename, NULL, AVMEDIA_TYPE_AUDIO) != AV_CODEC_ID_NONE) {
			int best_score = 0, idx = -1;
			for (i = 0; i < octx->nb_input_streams; i++) {
				int score;
				ist = octx->input_streams[i];
				score = ist->st->codecpar->channels + 100000000 * !!ist->st->codec_info_nb_frames;
				if (ist->st->codecpar->codec_type == AVMEDIA_TYPE_AUDIO &&
					score > best_score) {
					best_score = score;
					idx = i;
				}
			}
			if (idx >= 0)
				new_audio_stream(o, oc, idx);
		}


		{ int i; for (i = 0; i < o->nb_codec_names; i++) { char *spec = o->codec_names[i].specifier; if (!strcmp(spec, "s")) subtitle_codec_name = o->codec_names[i].u.str; }};
		if (!o->subtitle_disable && (avcodec_find_encoder(oc->oformat->subtitle_codec) || subtitle_codec_name)) {
			for (i = 0; i <octx->nb_input_streams; i++)
				if (octx->input_streams[i]->st->codecpar->codec_type == AVMEDIA_TYPE_SUBTITLE) {
					AVCodecDescriptor const *input_descriptor =
						avcodec_descriptor_get(octx->input_streams[i]->st->codecpar->codec_id);
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

						input_descriptor && output_descriptor &&
						(!input_descriptor->props ||
							!output_descriptor->props)) {
						new_subtitle_stream(o, oc, i);
						break;
					}
				}
		}

		if (!o->data_disable) {
			enum AVCodecID codec_id = av_guess_codec(oc->oformat, NULL, filename, NULL, AVMEDIA_TYPE_DATA);
			for (i = 0; codec_id != AV_CODEC_ID_NONE && i < octx->nb_input_streams; i++) {
				if (octx->input_streams[i]->st->codecpar->codec_type == AVMEDIA_TYPE_DATA
					&& octx->input_streams[i]->st->codecpar->codec_id == codec_id)
					new_data_stream(o, oc, i);
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

				for (j = 0; j < octx->nb_filtergraphs; j++) {
					fg = octx->filtergraphs[j];
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
					exit_program(octx, 1);
				}
				init_output_filter(ofilter, o, oc);
			}
			else {
				int src_idx = octx->input_files[map->file_index]->ist_index + map->stream_index;

				ist = octx->input_streams[octx->input_files[map->file_index]->ist_index + map->stream_index];
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
				case AVMEDIA_TYPE_VIDEO: ost = new_video_stream(o, oc, src_idx); break;
				case AVMEDIA_TYPE_AUDIO: ost = new_audio_stream(o, oc, src_idx); break;
				case AVMEDIA_TYPE_SUBTITLE: ost = new_subtitle_stream(o, oc, src_idx); break;
				case AVMEDIA_TYPE_DATA: ost = new_data_stream(o, oc, src_idx); break;
				case AVMEDIA_TYPE_ATTACHMENT: ost = new_attachment_stream(o, oc, src_idx); break;
				case AVMEDIA_TYPE_UNKNOWN:
				default:;
					exit_program(octx, 1);
				}
				if (ost)
					ost->sync_ist = octx->input_streams[octx->input_files[map->sync_file_index]->ist_index
					+ map->sync_stream_index];
			}
		}
	}


	for (i = 0; i < o->nb_attachments; i++) {
		AVIOContext *pb;
		uint8_t *attachment;
		const char *p;
		int64_t len;

		if ((err = avio_open2(&pb, o->attachments[i], AVIO_FLAG_READ, &int_cb, NULL)) < 0) {
			av_log(NULL, AV_LOG_FATAL, "Could not open attachment file %s.\n",
				o->attachments[i]);
			exit_program(octx, 1);
		}
		if ((len = avio_size(pb)) <= 0) {
			av_log(NULL, AV_LOG_FATAL, "Could not get size of the attachment %s.\n",
				o->attachments[i]);
			exit_program(octx, 1);
		}
		attachment = av_malloc(len);
		avio_read(pb, attachment, len);

		ost = new_attachment_stream(o, oc, -1);
		ost->stream_copy = 0;
		ost->attachment_filename = o->attachments[i];
		ost->st->codecpar->extradata = attachment;
		ost->st->codecpar->extradata_size = len;

		p = strrchr(o->attachments[i], '/');
		av_dict_set(&ost->st->metadata, "filename", (p && *p) ? p + 1 : o->attachments[i], AV_DICT_DONT_OVERWRITE);
		avio_closep(&pb);
	}

	if (!oc->nb_streams && !(oc->oformat->flags & AVFMT_NOSTREAMS)) {
		av_dump_format(oc, octx->nb_output_files - 1, oc->url, 1);
		exit_program(octx, 1);
	}

	unused_opts = strip_specifiers(o->g->codec_opts);
	for (i = of->ost_index; i < octx->nb_output_streams; i++) {
		e = NULL;
		while ((e = av_dict_get(octx->output_streams[i]->encoder_opts, "", e,
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
			exit_program(octx, 1);
		}

		if (!strcmp(e->key, "gop_timecode"))
			continue;
	}
	av_dict_free(&unused_opts);


	for (i = of->ost_index; i < octx->nb_output_streams; i++) {
		OutputStream *ost = octx->output_streams[i];

		if (ost->encoding_needed && ost->source_index >= 0) {
			InputStream *ist = octx->input_streams[ost->source_index];
			ist->decoding_needed |= DECODING_FOR_OST;

			if (ost->st->codecpar->codec_type == AVMEDIA_TYPE_VIDEO ||
				ost->st->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
				err = init_simple_filtergraph(ist, ost);
				if (err < 0) {
					exit_program(octx, 1);
				}
			}
		}


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
					f->formats = av_mallocz_array(count + 1, sizeof(*f->formats));
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
					f->formats = av_mallocz_array(count + 1, sizeof(*f->formats));
					memcpy(f->formats, ost->enc->sample_fmts, (count + 1) * sizeof(*f->formats));
				}
				if (ost->enc_ctx->sample_rate) {
					f->sample_rate = ost->enc_ctx->sample_rate;
				}
				else if (ost->enc->supported_samplerates) {
					count = 0;
					while (ost->enc->supported_samplerates[count])
						count++;
					f->sample_rates = av_mallocz_array(count + 1, sizeof(*f->sample_rates));
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
					f->channel_layouts = av_mallocz_array(count + 1, sizeof(*f->channel_layouts));
					memcpy(f->channel_layouts, ost->enc->channel_layouts,
						(count + 1) * sizeof(*f->channel_layouts));
				}
				break;
			}
		}
	}


	if (oc->oformat->flags & AVFMT_NEEDNUMBER) {
		if (!av_filename_number_test(oc->url)) {
			exit_program(octx, 1);
		}
	}

	if (!(oc->oformat->flags & AVFMT_NOSTREAMS) && !octx->input_stream_potentially_available) {
		exit_program(octx, 1);
	}

	if (!(oc->oformat->flags & AVFMT_NOFILE)) {
		if ((err = avio_open2(&oc->pb, filename, AVIO_FLAG_WRITE,
			&oc->interrupt_callback,
			&of->opts)) < 0) {
			exit_program(octx, 1);
		}
	}

	if (o->mux_preload) {
		av_dict_set_int(&of->opts, "preload", o->mux_preload*AV_TIME_BASE, 0);
	}
	oc->max_delay = (int)(o->mux_max_delay * AV_TIME_BASE);


	for (i = 0; i < o->nb_metadata_map; i++) {
		char *p;
		int in_file_index = strtol(o->metadata_map[i].u.str, &p, 0);

		if (in_file_index >= octx->nb_input_files) {
			exit_program(octx, 1);
		}
		copy_metadata(octx, o->metadata_map[i].specifier, *p ? p + 1 : p, oc,
			in_file_index >= 0 ?
			octx->input_files[in_file_index]->ctx : NULL, o);
	}


	if (o->chapters_input_file >= octx->nb_input_files) {
		if (o->chapters_input_file == INT_MAX) {

			o->chapters_input_file = -1;
			for (i = 0; i < octx->nb_input_files; i++)
				if (octx->input_files[i]->ctx->nb_chapters) {
					o->chapters_input_file = i;
					break;
				}
		}
		else {
			av_log(NULL, AV_LOG_FATAL, "Invalid input file index %d in chapter mapping.\n",
				o->chapters_input_file);
			exit_program(octx, 1);
		}
	}
	if (o->chapters_input_file >= 0)
		copy_chapters(octx->input_files[o->chapters_input_file], of,
			!o->metadata_chapters_manual);


	if (!o->metadata_global_manual && octx->nb_input_files) {
		av_dict_copy(&oc->metadata, octx->input_files[0]->ctx->metadata,
			AV_DICT_DONT_OVERWRITE);
		if (o->recording_time != INT64_MAX)
			av_dict_set(&oc->metadata, "duration", NULL, 0);
		av_dict_set(&oc->metadata, "creation_time", NULL, 0);
	}
	if (!o->metadata_streams_manual)
		for (i = of->ost_index; i < octx->nb_output_streams; i++) {
			InputStream *ist;
			if (octx->output_streams[i]->source_index < 0)
				continue;
			ist = octx->input_streams[octx->output_streams[i]->source_index];
			av_dict_copy(&octx->output_streams[i]->st->metadata, ist->st->metadata, AV_DICT_DONT_OVERWRITE);
			if (!octx->output_streams[i]->stream_copy) {
				av_dict_set(&octx->output_streams[i]->st->metadata, "encoder", NULL, 0);
			}
		}


	for (i = 0; i < o->nb_program; i++) {
		const char *p = o->program[i].u.str;
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

		p = o->program[i].u.str;
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
				exit_program(octx, 1);
			}
			if (!*p2)
				exit_program(octx, 1);
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
				exit_program(octx, 1);
			}
			av_freep(&to_dealloc);
			av_freep(&key);
		}
	}


	for (i = 0; i < o->nb_metadata; i++) {
		AVDictionary **m;
		char type, *val;
		const char *stream_spec;
		int index = 0, j, ret = 0;

		val = strchr(o->metadata[i].u.str, '=');
		if (!val) {
			av_log(NULL, AV_LOG_FATAL, "No '=' character in metadata string %s.\n",
				o->metadata[i].u.str);
			exit_program(octx, 1);
		}
		*val++ = 0;

		parse_meta_type(octx, o->metadata[i].specifier, &type, &index, &stream_spec);
		if (type == 's') {
			for (j = 0; j < oc->nb_streams; j++) {
				ost = octx->output_streams[octx->nb_output_streams - oc->nb_streams + j];
				if ((ret = check_stream_specifier(oc, oc->streams[j], stream_spec)) > 0) {
					if (!strcmp(o->metadata[i].u.str, "rotate")) {
						char *tail;
						double theta = av_strtod(val, &tail);
						if (!*tail) {
							ost->rotate_overridden = 1;
							ost->rotate_override_value = theta;
						}
					}
					else {
						av_dict_set(&oc->streams[j]->metadata, o->metadata[i].u.str, *val ? val : NULL, 0);
					}
				}
				else if (ret < 0)
					exit_program(octx, 1);
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
					exit_program(octx, 1);
				}
				m = &oc->chapters[index]->metadata;
				break;
			case 'p':
				if (index < 0 || index >= oc->nb_programs) {
					av_log(NULL, AV_LOG_FATAL, "Invalid program index %d in metadata specifier.\n", index);
					exit_program(octx, 1);
				}
				m = &oc->programs[index]->metadata;
				break;
			default:
				av_log(NULL, AV_LOG_FATAL, "Invalid metadata specifier %s.\n", o->metadata[i].specifier);
				exit_program(octx, 1);
			}
			av_dict_set(m, o->metadata[i].u.str, *val ? val : NULL, 0);
		}
	}

	return 0;
}

static int opt_target(WXCtx *octx, void *optctx, const char *opt, const char *arg)
{
	OptionsContext *o = optctx;
	enum { PAL, NTSC, FILM, UNKNOWN } norm = UNKNOWN;

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

		if (octx->nb_input_files) {
			int i, j, fr;
			for (j = 0; j < octx->nb_input_files; j++) {
				for (i = 0; i < octx->input_files[j]->nb_streams; i++) {
					AVStream *st = octx->input_files[j]->ctx->streams[i];
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
		exit_program(octx, 1);
	}

	if (!strcmp(arg, "vcd")) {
		opt_video_codec(octx, o, "c:v", "mpeg1video");
		opt_audio_codec(octx, o, "c:a", "mp2");
		parse_option(octx, o, "f", "vcd", ffmpeg_options);

		parse_option(octx, o, "s", norm == PAL ? "352x288" : "352x240", ffmpeg_options);
		parse_option(octx, o, "r", frame_rates[norm], ffmpeg_options);
		opt_default(octx, o, "g", norm == PAL ? "15" : "18");

		opt_default(octx, o, "b:v", "1150000");
		opt_default(octx, o, "maxrate:v", "1150000");
		opt_default(octx, o, "minrate:v", "1150000");
		opt_default(octx, o, "bufsize:v", "327680");

		opt_default(octx, o, "b:a", "224000");
		parse_option(octx, o, "ar", "44100", ffmpeg_options);
		parse_option(octx, o, "ac", "2", ffmpeg_options);

		opt_default(octx, o, "packetsize", "2324");
		opt_default(octx, o, "muxrate", "1411200");
		o->mux_preload = (36000 + 3 * 1200) / 90000.0f;
	}
	else if (!strcmp(arg, "svcd")) {

		opt_video_codec(octx, o, "c:v", "mpeg2video");
		opt_audio_codec(octx, o, "c:a", "mp2");
		parse_option(octx, o, "f", "svcd", ffmpeg_options);

		parse_option(octx, o, "s", norm == PAL ? "480x576" : "480x480", ffmpeg_options);
		parse_option(octx, o, "r", frame_rates[norm], ffmpeg_options);
		parse_option(octx, o, "pix_fmt", "yuv420p", ffmpeg_options);
		opt_default(octx, o, "g", norm == PAL ? "15" : "18");

		opt_default(octx, o, "b:v", "2040000");
		opt_default(octx, o, "maxrate:v", "2516000");
		opt_default(octx, o, "minrate:v", "0");
		opt_default(octx, o, "bufsize:v", "1835008");
		opt_default(octx, o, "scan_offset", "1");

		opt_default(octx, o, "b:a", "224000");
		parse_option(octx, o, "ar", "44100", ffmpeg_options);

		opt_default(octx, o, "packetsize", "2324");

	}
	else if (!strcmp(arg, "dvd")) {

		opt_video_codec(octx, o, "c:v", "mpeg2video");
		opt_audio_codec(octx, o, "c:a", "ac3");
		parse_option(octx, o, "f", "dvd", ffmpeg_options);

		parse_option(octx, o, "s", norm == PAL ? "720x576" : "720x480", ffmpeg_options);
		parse_option(octx, o, "r", frame_rates[norm], ffmpeg_options);
		parse_option(octx, o, "pix_fmt", "yuv420p", ffmpeg_options);
		opt_default(octx, o, "g", norm == PAL ? "15" : "18");

		opt_default(octx, o, "b:v", "6000000");
		opt_default(octx, o, "maxrate:v", "9000000");
		opt_default(octx, o, "minrate:v", "0");
		opt_default(octx, o, "bufsize:v", "1835008");

		opt_default(octx, o, "packetsize", "2048");
		opt_default(octx, o, "muxrate", "10080000");

		opt_default(octx, o, "b:a", "448000");
		parse_option(octx, o, "ar", "48000", ffmpeg_options);

	}
	else if (!strncmp(arg, "dv", 2)) {

		parse_option(octx, o, "f", "dv", ffmpeg_options);

		parse_option(octx, o, "s", norm == PAL ? "720x576" : "720x480", ffmpeg_options);
		parse_option(octx, o, "pix_fmt", !strncmp(arg, "dv50", 4) ? "yuv422p" :
			norm == PAL ? "yuv420p" : "yuv411p", ffmpeg_options);
		parse_option(octx, o, "r", frame_rates[norm], ffmpeg_options);

		parse_option(octx, o, "ar", "48000", ffmpeg_options);
		parse_option(octx, o, "ac", "2", ffmpeg_options);

	}
	else {
		av_log(NULL, AV_LOG_ERROR, "Unknown target: %s\n", arg);
		return AVERROR(EINVAL);
	}

	av_dict_copy(&o->g->codec_opts, octx->codec_opts, AV_DICT_DONT_OVERWRITE);
	av_dict_copy(&o->g->format_opts, octx->format_opts, AV_DICT_DONT_OVERWRITE);

	return 0;
}

static int opt_video_frames(WXCtx *octx, void *optctx, const char *opt, const char *arg)
{
	OptionsContext *o = optctx;
	return parse_option(octx, o, "frames:v", arg, ffmpeg_options);
}

static int opt_audio_frames(WXCtx *octx, void *optctx, const char *opt, const char *arg)
{
	OptionsContext *o = optctx;
	return parse_option(octx, o, "frames:a", arg, ffmpeg_options);
}

static int opt_data_frames(WXCtx *octx, void *optctx, const char *opt, const char *arg)
{
	OptionsContext *o = optctx;
	return parse_option(octx, o, "frames:d", arg, ffmpeg_options);
}

static int opt_default_new(WXCtx *octx, OptionsContext *o, const char *opt, const char *arg)
{
	int ret;
	AVDictionary *cbak = octx->codec_opts;
	AVDictionary *fbak = octx->format_opts;
	octx->codec_opts = NULL;
	octx->format_opts = NULL;

	ret = opt_default(octx, o, opt, arg);

	av_dict_copy(&o->g->codec_opts, octx->codec_opts, 0);
	av_dict_copy(&o->g->format_opts, octx->format_opts, 0);
	av_dict_free(&octx->codec_opts);
	av_dict_free(&octx->format_opts);
	octx->codec_opts = cbak;
	octx->format_opts = fbak;

	return ret;
}

static int opt_preset(WXCtx *octx, void *optctx, const char *opt, const char *arg)
{
	OptionsContext *o = optctx;
	FILE *f = NULL;
	char filename[1000], line[1000], tmp_line[1000];
	const char *codec_name = NULL;

	tmp_line[0] = *opt;
	tmp_line[1] = 0;
	{ int i; for (i = 0; i < o->nb_codec_names; i++) { char *spec = o->codec_names[i].specifier; if (!strcmp(spec, tmp_line)) codec_name = o->codec_names[i].u.str; }};

	if (!(f = get_preset_file(filename, sizeof(filename), arg, *opt == 'f', codec_name))) {
		if (!strncmp(arg, "libx264-lossless", strlen("libx264-lossless"))) {
			av_log(NULL, AV_LOG_FATAL, "Please use -preset <speed> -qp 0\n");
		}
		else
			av_log(NULL, AV_LOG_FATAL, "File for preset '%s' not found\n", arg);
		exit_program(octx, 1);
	}

	while (fgets(line, sizeof(line), f)) {
		char *key = tmp_line, *value, *endptr;

		if (strcspn(line, "#\n\r") == 0)
			continue;
		av_strlcpy(tmp_line, line, sizeof(tmp_line));
		if (!av_strtok(key, "=", &value) ||
			!av_strtok(value, "\r\n", &endptr)) {
			av_log(NULL, AV_LOG_FATAL, "%s: Invalid syntax: '%s'\n", filename, line);
			exit_program(octx, 1);
		}
		av_log(NULL, AV_LOG_DEBUG, "ffpreset[%s]: set '%s' = '%s'\n", filename, key, value);

		if (!strcmp(key, "acodec")) opt_audio_codec(octx, o, key, value);
		else if (!strcmp(key, "vcodec")) opt_video_codec(octx, o, key, value);
		else if (!strcmp(key, "scodec")) opt_subtitle_codec(octx, o, key, value);
		else if (!strcmp(key, "dcodec")) opt_data_codec(octx, o, key, value);
		else if (opt_default_new(octx, o, key, value) < 0) {
			av_log(NULL, AV_LOG_FATAL, "%s: Invalid option or argument: '%s', parsed as '%s' = '%s'\n",
				filename, line, key, value);
			exit_program(octx, 1);
		}
	}

	fclose(f);

	return 0;
}

static int opt_old2new(WXCtx *octx, void *optctx, const char *opt, const char *arg)
{
	OptionsContext *o = optctx;
	char *s = av_asprintf("%s:%c", opt + 1, *opt);
	int ret = parse_option(octx, o, s, arg, ffmpeg_options);
	av_free(s);
	return ret;
}

static int opt_bitrate(WXCtx *octx, void *optctx, const char *opt, const char *arg)
{
	OptionsContext *o = optctx;
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

static int opt_qscale(WXCtx *octx, void *optctx, const char *opt, const char *arg)
{
	OptionsContext *o = optctx;
	char *s;
	int ret;
	if (!strcmp(opt, "qscale")) {
		av_log(NULL, AV_LOG_WARNING, "Please use -q:a or -q:v, -qscale is ambiguous\n");
		return parse_option(octx, o, "q:v", arg, ffmpeg_options);
	}
	s = av_asprintf("q%s", opt + 6);
	ret = parse_option(octx, o, s, arg, ffmpeg_options);
	av_free(s);
	return ret;
}

static int opt_profile(WXCtx *octx, void *optctx, const char *opt, const char *arg)
{
	OptionsContext *o = optctx;
	if (!strcmp(opt, "profile")) {
		av_log(NULL, AV_LOG_WARNING, "Please use -profile:a or -profile:v, -profile is ambiguous\n");
		av_dict_set(&o->g->codec_opts, "profile:v", arg, 0);
		return 0;
	}
	av_dict_set(&o->g->codec_opts, opt, arg, 0);
	return 0;
}

static int opt_video_filters(WXCtx *octx, void *optctx, const char *opt, const char *arg)
{
	OptionsContext *o = optctx;
	return parse_option(octx, o, "filter:v", arg, ffmpeg_options);
}

static int opt_audio_filters(WXCtx *octx, void *optctx, const char *opt, const char *arg)
{
	OptionsContext *o = optctx;
	return parse_option(octx, o, "filter:a", arg, ffmpeg_options);
}

static int opt_timecode(WXCtx *octx, void *optctx, const char *opt, const char *arg)
{
	OptionsContext *o = optctx;
	char *tcr = av_asprintf("timecode=%s", arg);
	int ret = parse_option(octx, o, "metadata:g", tcr, ffmpeg_options);
	if (ret >= 0)
		ret = av_dict_set(&o->g->codec_opts, "gop_timecode", arg, 0);
	av_free(tcr);
	return ret;
}

static int opt_channel_layout(WXCtx *octx, void *optctx, const char *opt, const char *arg)
{
	OptionsContext *o = optctx;
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
	snprintf(layout_str, sizeof(layout_str), "%"PRIu64, layout);
	ret = opt_default_new(octx, o, opt, layout_str);
	if (ret < 0)
		return ret;

	channels = av_get_channel_layout_nb_channels(layout);
	snprintf(layout_str, sizeof(layout_str), "%d", channels);
	stream_str = strchr(opt, ':');
	ac_str_size = 3 + (stream_str ? strlen(stream_str) : 0);
	ac_str = av_mallocz(ac_str_size);
	av_strlcpy(ac_str, "ac", 3);
	if (stream_str)
		av_strlcat(ac_str, stream_str, ac_str_size);
	ret = parse_option(octx, o, ac_str, layout_str, ffmpeg_options);
	av_free(ac_str);
	return ret;
}

static int opt_audio_qscale(WXCtx *octx, void *optctx, const char *opt, const char *arg) {
	OptionsContext *o = optctx;
	return parse_option(octx, o, "q:a", arg, ffmpeg_options);
}

static int opt_filter_complex(WXCtx *octx, void *optctx, const char *opt, const char *arg) {
	OptionsContext *o = optctx;
	octx->filtergraphs = grow_array(octx->filtergraphs, sizeof(*octx->filtergraphs), &octx->nb_filtergraphs, octx->nb_filtergraphs + 1);
	octx->filtergraphs[octx->nb_filtergraphs - 1] = av_mallocz(sizeof(*octx->filtergraphs[0]));
	octx->filtergraphs[octx->nb_filtergraphs - 1]->octx = octx;
	octx->filtergraphs[octx->nb_filtergraphs - 1]->index = octx->nb_filtergraphs - 1;
	octx->filtergraphs[octx->nb_filtergraphs - 1]->graph_desc = av_strdup(arg);
	octx->input_stream_potentially_available = 1;
	return 0;
}

static int opt_filter_complex_script(WXCtx *octx, void *optctx, const char *opt, const char *arg) {
	OptionsContext *o = optctx;
	uint8_t *graph_desc = read_file(arg);
	if (!graph_desc)
		return AVERROR(EINVAL);
	octx->filtergraphs = grow_array(octx->filtergraphs, sizeof(*octx->filtergraphs), &octx->nb_filtergraphs, octx->nb_filtergraphs + 1);
	octx->filtergraphs[octx->nb_filtergraphs - 1] = av_mallocz(sizeof(*octx->filtergraphs[0]));
	octx->filtergraphs[octx->nb_filtergraphs - 1]->index = octx->nb_filtergraphs - 1;
	octx->filtergraphs[octx->nb_filtergraphs - 1]->graph_desc = graph_desc;
	octx->input_stream_potentially_available = 1;
	return 0;
}

static int open_files(WXCtx *octx, OptionGroupList *l, const char *inout, int(*open_file)(WXCtx *, OptionsContext*, const char*)) {
	int i, ret;
	for (i = 0; i < l->nb_groups; i++) {
		OptionGroup *g = &l->groups[i];
		OptionsContext o;
		init_options(&o);
		o.g = g;
		o.octx = octx;
		ret = parse_optgroup(octx, &o, g);
		if (ret < 0) {
			return ret;
		}
		ret = open_file(octx, &o, g->arg);
		uninit_options(&o);
		if (ret < 0) {
			return ret;
		}
	}
	return 0;
}

int ffmpeg_parse_options(WXCtx *octx, int argc, char **argv)
{
	int ret = split_commandline(octx, argc, argv, ffmpeg_options, groups, FF_ARRAY_ELEMS(groups));

	if (ret < 0) {
		goto fail;
	}

	ret = parse_optgroup(octx, NULL, &octx->global_opts);
	if (ret < 0) {
		goto fail;
	}

	ret = open_files(octx, &octx->groups[GROUP_INFILE], "input", open_input_file);//打开输入文件
	if (ret < 0) {
		goto fail;
	}

	ret = init_complex_filters(octx);//配置滤镜
	if (ret < 0) {
		goto fail;
	}

	ret = open_files(octx, &octx->groups[GROUP_OUTFILE], "output", open_output_file);//打开输出文件
	if (ret < 0) {
		goto fail;
	}

	check_filter_outputs(octx);//检查输入输出
	return 0;
fail:
	uninit_parse_context(octx);
	return -1;
}

const OptionDef ffmpeg_options[177] = {

	{ "L", OPT_EXIT,{ .func_arg = show_help }, "show license" },
	{ "h", OPT_EXIT,{ .func_arg = show_help }, "show help", "topic" },
	{ "?", OPT_EXIT,{ .func_arg = show_help }, "show help", "topic" },
	{ "help", OPT_EXIT,{ .func_arg = show_help }, "show help", "topic" },
	{ "-help", OPT_EXIT,{ .func_arg = show_help }, "show help", "topic" },
	{ "version", OPT_EXIT,{ .func_arg = show_help }, "show version" },
	{ "buildconf", OPT_EXIT,{ .func_arg = show_help }, "show build configuration" },
	{ "formats", OPT_EXIT,{ .func_arg = show_help }, "show available formats" },
	{ "muxers", OPT_EXIT,{ .func_arg = show_help }, "show available muxers" },
	{ "demuxers", OPT_EXIT,{ .func_arg = show_help }, "show available demuxers" },
	{ "devices", OPT_EXIT,{ .func_arg = show_help }, "show available devices" },
	{ "codecs", OPT_EXIT,{ .func_arg = show_help }, "show available codecs" },
	{ "decoders", OPT_EXIT,{ .func_arg = show_help }, "show available decoders" },
	{ "encoders", OPT_EXIT,{ .func_arg = show_help }, "show available encoders" },
	{ "bsfs", OPT_EXIT,{ .func_arg = show_help }, "show available bit stream filters" },
	{ "protocols", OPT_EXIT,{ .func_arg = show_help }, "show available protocols" },
	{ "filters", OPT_EXIT,{ .func_arg = show_help }, "show available filters" },
	{ "pix_fmts", OPT_EXIT,{ .func_arg = show_help }, "show available pixel formats" },
	{ "layouts", OPT_EXIT,{ .func_arg = show_help }, "show standard channel layouts" },
	{ "sample_fmts", OPT_EXIT,{ .func_arg = show_help }, "show available audio sample formats" },
	{ "colors", OPT_EXIT,{ .func_arg = show_help }, "show available color names" },
	{ "loglevel", HAS_ARG,{ .func_arg = opt_loglevel }, "set logging level", "loglevel" },
	{ "v", HAS_ARG,{ .func_arg = opt_loglevel }, "set logging level", "loglevel" },
	{ "report", 0,{ (void*)opt_report }, "generate a report" },
	{ "max_alloc", HAS_ARG,{ .func_arg = opt_max_alloc }, "set maximum size of a single allocated block", "bytes" },
	{ "cpuflags", HAS_ARG | OPT_EXPERT,{ .func_arg = opt_cpuflags }, "force specific cpu flags", "flags" },
	{ "hide_banner", OPT_BOOL | OPT_EXPERT,{ &dummy }, "do not show program banner", "hide_banner" },
	{ "sources" , OPT_EXIT | HAS_ARG,{ .func_arg = show_help },
	"list sources of the input device", "device" },
	{ "sinks" , OPT_EXIT | HAS_ARG,{ .func_arg = show_help },
	"list sinks of the output device", "device" },

	{ "f", HAS_ARG | OPT_STRING | OPT_OFFSET |
	OPT_INPUT | OPT_OUTPUT,{ .off = offsetof(OptionsContext, format) },
	"force format", "fmt" },
	{ "y", OPT_BOOL,{ &dummy },
	"overwrite output files" },
	{ "n", OPT_BOOL,{ &dummy },
	"never overwrite output files" },
	{ "ignore_unknown", OPT_BOOL,{ &dummy },
	"Ignore unknown stream types" },
	{ "copy_unknown", OPT_BOOL | OPT_EXPERT,{ &dummy },
	"Copy unknown stream types" },
	{ "c", HAS_ARG | OPT_STRING | OPT_SPEC |
	OPT_INPUT | OPT_OUTPUT,{ .off = offsetof(OptionsContext, codec_names) },
	"codec name", "codec" },
	{ "codec", HAS_ARG | OPT_STRING | OPT_SPEC |
	OPT_INPUT | OPT_OUTPUT,{ .off = offsetof(OptionsContext, codec_names) },
	"codec name", "codec" },
	{ "pre", HAS_ARG | OPT_STRING | OPT_SPEC |
	OPT_OUTPUT,{ .off = offsetof(OptionsContext, presets) },
	"preset name", "preset" },
	{ "map", HAS_ARG | OPT_EXPERT | OPT_PERFILE |
	OPT_OUTPUT,{ .func_arg = opt_map },
	"set input stream mapping",
	"[-]input_file_id[:stream_specifier][,sync_file_id[:stream_specifier]]" },
	{ "map_channel", HAS_ARG | OPT_EXPERT | OPT_PERFILE | OPT_OUTPUT,{ .func_arg = opt_map_channel },
	"map an audio channel from one stream to another", "file.stream.channel[:syncfile.syncstream]" },
	{ "map_metadata", HAS_ARG | OPT_STRING | OPT_SPEC |
	OPT_OUTPUT,{ .off = offsetof(OptionsContext, metadata_map) },
	"set metadata information of outfile from infile",
	"outfile[,metadata]:infile[,metadata]" },
	{ "map_chapters", HAS_ARG | OPT_INT | OPT_EXPERT | OPT_OFFSET |
	OPT_OUTPUT,{ .off = offsetof(OptionsContext, chapters_input_file) },
	"set chapters mapping", "input_file_index" },
	{ "t", HAS_ARG | OPT_TIME | OPT_OFFSET |
	OPT_INPUT | OPT_OUTPUT,{ .off = offsetof(OptionsContext, recording_time) },
	"record or transcode \"duration\" seconds of audio/video",
	"duration" },
	{ "to", HAS_ARG | OPT_TIME | OPT_OFFSET | OPT_INPUT | OPT_OUTPUT,{ .off = offsetof(OptionsContext, stop_time) },
	"record or transcode stop time", "time_stop" },
	{ "fs", HAS_ARG | OPT_INT64 | OPT_OFFSET | OPT_OUTPUT,{ .off = offsetof(OptionsContext, limit_filesize) },
	"set the limit file size in bytes", "limit_size" },
	{ "ss", HAS_ARG | OPT_TIME | OPT_OFFSET |
	OPT_INPUT | OPT_OUTPUT,{ .off = offsetof(OptionsContext, start_time) },
	"set the start time offset", "time_off" },
	{ "sseof", HAS_ARG | OPT_TIME | OPT_OFFSET |
	OPT_INPUT | OPT_OUTPUT,{ .off = offsetof(OptionsContext, start_time_eof) },
	"set the start time offset relative to EOF", "time_off" },
	{ "seek_timestamp", HAS_ARG | OPT_INT | OPT_OFFSET |
	OPT_INPUT,{ .off = offsetof(OptionsContext, seek_timestamp) },
	"enable/disable seeking by timestamp with -ss" },
	{ "accurate_seek", OPT_BOOL | OPT_OFFSET | OPT_EXPERT |
	OPT_INPUT,{ .off = offsetof(OptionsContext, accurate_seek) },
	"enable/disable accurate seeking with -ss" },
	{ "itsoffset", HAS_ARG | OPT_TIME | OPT_OFFSET |
	OPT_EXPERT | OPT_INPUT,{ .off = offsetof(OptionsContext, input_ts_offset) },
	"set the input ts offset", "time_off" },
	{ "itsscale", HAS_ARG | OPT_DOUBLE | OPT_SPEC |
	OPT_EXPERT | OPT_INPUT,{ .off = offsetof(OptionsContext, ts_scale) },
	"set the input ts scale", "scale" },
	{ "timestamp", HAS_ARG | OPT_PERFILE | OPT_OUTPUT,{ .func_arg = opt_recording_timestamp },
	"set the recording timestamp ('now' to set the current time)", "time" },
	{ "metadata", HAS_ARG | OPT_STRING | OPT_SPEC | OPT_OUTPUT,{ .off = offsetof(OptionsContext, metadata) },
	"add metadata", "string=string" },
	{ "program", HAS_ARG | OPT_STRING | OPT_SPEC | OPT_OUTPUT,{ .off = offsetof(OptionsContext, program) },
	"add program with specified streams", "title=string:st=number..." },
	{ "dframes", HAS_ARG | OPT_PERFILE | OPT_EXPERT |
	OPT_OUTPUT,{ .func_arg = opt_data_frames },
	"set the number of data frames to output", "number" },
	{ "benchmark", OPT_BOOL | OPT_EXPERT,{ &dummy },
	"add timings for benchmarking" },
	{ "benchmark_all", OPT_BOOL | OPT_EXPERT,{ &dummy },
	"add timings for each task" },
	{ "progress", HAS_ARG | OPT_EXPERT,{ .func_arg = opt_progress },
	"write program-readable progress information", "url" },
	{ "stdin", OPT_BOOL | OPT_EXPERT,{ &dummy },
	"enable or disable interaction on standard input" },
	{ "timelimit", HAS_ARG | OPT_EXPERT,{ .func_arg = opt_timelimit },
	"set max runtime in seconds", "limit" },
	{ "dump", OPT_BOOL | OPT_EXPERT,{ &dummy },
	"dump each input packet" },
	{ "hex", OPT_BOOL | OPT_EXPERT,{ &dummy },
	"when dumping packets, also dump the payload" },
	{ "re", OPT_BOOL | OPT_EXPERT | OPT_OFFSET |
	OPT_INPUT,{ .off = offsetof(OptionsContext, rate_emu) },
	"read input at native frame rate", "" },
	{ "target", HAS_ARG | OPT_PERFILE | OPT_OUTPUT,{ .func_arg = opt_target },
	"specify target file type (\"vcd\", \"svcd\", \"dvd\", \"dv\" or \"dv50\" "
	"with optional prefixes \"pal-\", \"ntsc-\" or \"film-\")", "type" },
	{ "vsync", HAS_ARG | OPT_EXPERT,{ &dummy },
	"video sync method", "" },
	{ "frame_drop_threshold", HAS_ARG | OPT_FLOAT | OPT_EXPERT,{ &dummy },
	"frame drop threshold", "" },
	{ "async", HAS_ARG | OPT_INT | OPT_EXPERT,{ &dummy },
	"audio sync method", "" },
	{ "adrift_threshold", HAS_ARG | OPT_FLOAT | OPT_EXPERT,{ &dummy },
	"audio drift threshold", "threshold" },
	{ "copyts", OPT_BOOL | OPT_EXPERT,{ &dummy },
	"copy timestamps" },
	{ "start_at_zero", OPT_BOOL | OPT_EXPERT,{ &dummy },
	"shift input timestamps to start at 0 when using copyts" },
	{ "copytb", HAS_ARG | OPT_INT | OPT_EXPERT,{ &dummy },
	"copy input stream time base when stream copying", "mode" },
	{ "shortest", OPT_BOOL | OPT_EXPERT | OPT_OFFSET |
	OPT_OUTPUT,{ .off = offsetof(OptionsContext, shortest) },
	"finish encoding within shortest input" },
	{ "bitexact", OPT_BOOL | OPT_EXPERT | OPT_OFFSET |
	OPT_OUTPUT | OPT_INPUT,{ .off = offsetof(OptionsContext, bitexact) },
	"bitexact mode" },
	{ "apad", OPT_STRING | HAS_ARG | OPT_SPEC |
	OPT_OUTPUT,{ .off = offsetof(OptionsContext, apad) },
	"audio pad", "" },
	{ "dts_delta_threshold", HAS_ARG | OPT_FLOAT | OPT_EXPERT,{ &dummy },
	"timestamp discontinuity delta threshold", "threshold" },
	{ "dts_error_threshold", HAS_ARG | OPT_FLOAT | OPT_EXPERT,{ &dummy },
	"timestamp error delta threshold", "threshold" },
	{ "xerror", OPT_BOOL | OPT_EXPERT,{ &dummy },
	"exit on error", "error" },
	{ "abort_on", HAS_ARG | OPT_EXPERT,{ .func_arg = opt_abort_on },
	"abort on the specified condition flags", "flags" },
	{ "copyinkf", OPT_BOOL | OPT_EXPERT | OPT_SPEC |
	OPT_OUTPUT,{ .off = offsetof(OptionsContext, copy_initial_nonkeyframes) },
	"copy initial non-keyframes" },
	{ "copypriorss", OPT_INT | HAS_ARG | OPT_EXPERT | OPT_SPEC | OPT_OUTPUT,{ .off = offsetof(OptionsContext, copy_prior_start) },
	"copy or discard frames before start time" },
	{ "frames", OPT_INT64 | HAS_ARG | OPT_SPEC | OPT_OUTPUT,{ .off = offsetof(OptionsContext, max_frames) },
	"set the number of frames to output", "number" },
	{ "tag", OPT_STRING | HAS_ARG | OPT_SPEC |
	OPT_EXPERT | OPT_OUTPUT | OPT_INPUT,{ .off = offsetof(OptionsContext, codec_tags) },
	"force codec tag/fourcc", "fourcc/tag" },
	{ "q", HAS_ARG | OPT_EXPERT | OPT_DOUBLE |
	OPT_SPEC | OPT_OUTPUT,{ .off = offsetof(OptionsContext, qscale) },
	"use fixed quality scale (VBR)", "q" },
	{ "qscale", HAS_ARG | OPT_EXPERT | OPT_PERFILE |
	OPT_OUTPUT,{ .func_arg = opt_qscale },
	"use fixed quality scale (VBR)", "q" },
	{ "profile", HAS_ARG | OPT_EXPERT | OPT_PERFILE | OPT_OUTPUT,{ .func_arg = opt_profile },
	"set profile", "profile" },
	{ "filter", HAS_ARG | OPT_STRING | OPT_SPEC | OPT_OUTPUT,{ .off = offsetof(OptionsContext, filters) },
	"set stream filtergraph", "filter_graph" },
	{ "filter_threads", HAS_ARG | OPT_INT,{ &dummy },
	"number of non-complex filter threads" },
	{ "filter_script", HAS_ARG | OPT_STRING | OPT_SPEC | OPT_OUTPUT,{ .off = offsetof(OptionsContext, filter_scripts) },
	"read stream filtergraph description from a file", "filename" },
	{ "reinit_filter", HAS_ARG | OPT_INT | OPT_SPEC | OPT_INPUT,{ .off = offsetof(OptionsContext, reinit_filters) },
	"reinit filtergraph on input parameter changes", "" },
	{ "filter_complex", HAS_ARG | OPT_EXPERT,{ .func_arg = opt_filter_complex },
	"create a complex filtergraph", "graph_description" },
	{ "filter_complex_threads", HAS_ARG | OPT_INT,{ &dummy },
	"number of threads for -filter_complex" },
	{ "lavfi", HAS_ARG | OPT_EXPERT,{ .func_arg = opt_filter_complex },
	"create a complex filtergraph", "graph_description" },
	{ "filter_complex_script", HAS_ARG | OPT_EXPERT,{ .func_arg = opt_filter_complex_script },
	"read complex filtergraph description from a file", "filename" },
	{ "stats", OPT_BOOL,{ &dummy },
	"print progress report during encoding", },
	{ "attach", HAS_ARG | OPT_PERFILE | OPT_EXPERT |
	OPT_OUTPUT,{ .func_arg = opt_attach },
	"add an attachment to the output file", "filename" },
	{ "dump_attachment", HAS_ARG | OPT_STRING | OPT_SPEC |
	OPT_EXPERT | OPT_INPUT,{ .off = offsetof(OptionsContext, dump_attachment) },
	"extract an attachment into a file", "filename" },
	{ "stream_loop", OPT_INT | HAS_ARG | OPT_EXPERT | OPT_INPUT |
	OPT_OFFSET,{ .off = offsetof(OptionsContext, loop) }, "set number of times input stream shall be looped", "loop count" },
	{ "debug_ts", OPT_BOOL | OPT_EXPERT,{ &dummy },
	"print timestamp debugging info" },
	{ "max_error_rate", HAS_ARG | OPT_FLOAT,{ &dummy },
	"maximum error rate", "ratio of errors (0.0: no errors, 1.0: 100% errors) above which ffmpeg returns an error instead of success." },
	{ "discard", OPT_STRING | HAS_ARG | OPT_SPEC |
	OPT_INPUT,{ .off = offsetof(OptionsContext, discard) },
	"discard", "" },
	{ "disposition", OPT_STRING | HAS_ARG | OPT_SPEC |
	OPT_OUTPUT,{ .off = offsetof(OptionsContext, disposition) },
	"disposition", "" },
	{ "thread_queue_size", HAS_ARG | OPT_INT | OPT_OFFSET | OPT_EXPERT | OPT_INPUT,
	{ .off = offsetof(OptionsContext, thread_queue_size) },
	"set the maximum number of queued packets from the demuxer" },
	{ "find_stream_info", OPT_BOOL | OPT_PERFILE | OPT_INPUT | OPT_EXPERT,{ &dummy },
	"read and decode the streams to fill missing information with heuristics" },


	{ "vframes", OPT_VIDEO | HAS_ARG | OPT_PERFILE | OPT_OUTPUT,{ .func_arg = opt_video_frames },
	"set the number of video frames to output", "number" },
	{ "r", OPT_VIDEO | HAS_ARG | OPT_STRING | OPT_SPEC |
	OPT_INPUT | OPT_OUTPUT,{ .off = offsetof(OptionsContext, frame_rates) },
	"set frame rate (Hz value, fraction or abbreviation)", "rate" },
	{ "s", OPT_VIDEO | HAS_ARG | OPT_SUBTITLE | OPT_STRING | OPT_SPEC |
	OPT_INPUT | OPT_OUTPUT,{ .off = offsetof(OptionsContext, frame_sizes) },
	"set frame size (WxH or abbreviation)", "size" },
	{ "aspect", OPT_VIDEO | HAS_ARG | OPT_STRING | OPT_SPEC |
	OPT_OUTPUT,{ .off = offsetof(OptionsContext, frame_aspect_ratios) },
	"set aspect ratio (4:3, 16:9 or 1.3333, 1.7777)", "aspect" },
	{ "pix_fmt", OPT_VIDEO | HAS_ARG | OPT_EXPERT | OPT_STRING | OPT_SPEC |
	OPT_INPUT | OPT_OUTPUT,{ .off = offsetof(OptionsContext, frame_pix_fmts) },
	"set pixel format", "format" },
	{ "bits_per_raw_sample", OPT_VIDEO | OPT_INT | HAS_ARG,{ &dummy },
	"set the number of bits per raw sample", "number" },
	{ "intra", OPT_VIDEO | OPT_BOOL | OPT_EXPERT,{ &dummy },
	"deprecated use -g 1" },
	{ "vn", OPT_VIDEO | OPT_BOOL | OPT_OFFSET | OPT_INPUT | OPT_OUTPUT,{ .off = offsetof(OptionsContext, video_disable) },
	"disable video" },
	{ "rc_override", OPT_VIDEO | HAS_ARG | OPT_EXPERT | OPT_STRING | OPT_SPEC |
	OPT_OUTPUT,{ .off = offsetof(OptionsContext, rc_overrides) },
	"rate control override for specific intervals", "override" },
	{ "vcodec", OPT_VIDEO | HAS_ARG | OPT_PERFILE | OPT_INPUT |
	OPT_OUTPUT,{ .func_arg = opt_video_codec },
	"force video codec ('copy' to copy stream)", "codec" },
	{ "sameq", OPT_VIDEO | OPT_EXPERT ,{ .func_arg = opt_sameq },
	"Removed" },
	{ "same_quant", OPT_VIDEO | OPT_EXPERT ,{ .func_arg = opt_sameq },
	"Removed" },
	{ "timecode", OPT_VIDEO | HAS_ARG | OPT_PERFILE | OPT_OUTPUT,{ .func_arg = opt_timecode },
	"set initial TimeCode value.", "hh:mm:ss[:;.]ff" },
	{ "pass", OPT_VIDEO | HAS_ARG | OPT_SPEC | OPT_INT | OPT_OUTPUT,{ .off = offsetof(OptionsContext, pass) },
	"select the pass number (1 to 3)", "n" },
	{ "passlogfile", OPT_VIDEO | HAS_ARG | OPT_STRING | OPT_EXPERT | OPT_SPEC |
	OPT_OUTPUT,{ .off = offsetof(OptionsContext, passlogfiles) },
	"select two pass log file name prefix", "prefix" },
	{ "deinterlace", OPT_VIDEO | OPT_BOOL | OPT_EXPERT,{ &dummy },
	"this option is deprecated, use the yadif filter instead" },
	{ "psnr", OPT_VIDEO | OPT_BOOL | OPT_EXPERT,{ &dummy },
	"calculate PSNR of compressed frames" },
	{ "vstats", OPT_VIDEO | OPT_EXPERT ,{ .func_arg = opt_vstats },
	"dump video coding statistics to file" },
	{ "vstats_file", OPT_VIDEO | HAS_ARG | OPT_EXPERT ,{ .func_arg = opt_vstats_file },
	"dump video coding statistics to file", "file" },
	{ "vstats_version", OPT_VIDEO | OPT_INT | HAS_ARG | OPT_EXPERT ,{ &dummy },
	"Version of the vstats format to use." },
	{ "vf", OPT_VIDEO | HAS_ARG | OPT_PERFILE | OPT_OUTPUT,{ .func_arg = opt_video_filters },
	"set video filters", "filter_graph" },
	{ "intra_matrix", OPT_VIDEO | HAS_ARG | OPT_EXPERT | OPT_STRING | OPT_SPEC |
	OPT_OUTPUT,{ .off = offsetof(OptionsContext, intra_matrices) },
	"specify intra matrix coeffs", "matrix" },
	{ "inter_matrix", OPT_VIDEO | HAS_ARG | OPT_EXPERT | OPT_STRING | OPT_SPEC |
	OPT_OUTPUT,{ .off = offsetof(OptionsContext, inter_matrices) },
	"specify inter matrix coeffs", "matrix" },
	{ "chroma_intra_matrix", OPT_VIDEO | HAS_ARG | OPT_EXPERT | OPT_STRING | OPT_SPEC |
	OPT_OUTPUT,{ .off = offsetof(OptionsContext, chroma_intra_matrices) },
	"specify intra matrix coeffs", "matrix" },
	{ "top", OPT_VIDEO | HAS_ARG | OPT_EXPERT | OPT_INT | OPT_SPEC |
	OPT_INPUT | OPT_OUTPUT,{ .off = offsetof(OptionsContext, top_field_first) },
	"top=1/bottom=0/auto=-1 field first", "" },
	{ "vtag", OPT_VIDEO | HAS_ARG | OPT_EXPERT | OPT_PERFILE |
	OPT_INPUT | OPT_OUTPUT,{ .func_arg = opt_old2new },
	"force video tag/fourcc", "fourcc/tag" },
	{ "qphist", OPT_VIDEO | OPT_BOOL | OPT_EXPERT ,{ &dummy },
	"show QP histogram" },
	{ "force_fps", OPT_VIDEO | OPT_BOOL | OPT_EXPERT | OPT_SPEC |
	OPT_OUTPUT,{ .off = offsetof(OptionsContext, force_fps) },
	"force the selected framerate, disable the best supported framerate selection" },
	{ "streamid", OPT_VIDEO | HAS_ARG | OPT_EXPERT | OPT_PERFILE |
	OPT_OUTPUT,{ .func_arg = opt_streamid },
	"set the value of an outfile streamid", "streamIndex:value" },
	{ "force_key_frames", OPT_VIDEO | OPT_STRING | HAS_ARG | OPT_EXPERT |
	OPT_SPEC | OPT_OUTPUT,{ .off = offsetof(OptionsContext, forced_key_frames) },
	"force key frames at specified timestamps", "timestamps" },
	{ "ab", OPT_VIDEO | HAS_ARG | OPT_PERFILE | OPT_OUTPUT,{ .func_arg = opt_bitrate },
	"audio bitrate (please use -b:a)", "bitrate" },
	{ "b", OPT_VIDEO | HAS_ARG | OPT_PERFILE | OPT_OUTPUT,{ .func_arg = opt_bitrate },
	"video bitrate (please use -b:v)", "bitrate" },
	{ "hwaccel", OPT_VIDEO | OPT_STRING | HAS_ARG | OPT_EXPERT |
	OPT_SPEC | OPT_INPUT,{ .off = offsetof(OptionsContext, hwaccels) },
	"use HW accelerated decoding", "hwaccel name" },
	{ "hwaccel_device", OPT_VIDEO | OPT_STRING | HAS_ARG | OPT_EXPERT |
	OPT_SPEC | OPT_INPUT,{ .off = offsetof(OptionsContext, hwaccel_devices) },
	"select a device for HW acceleration", "devicename" },
	{ "hwaccel_output_format", OPT_VIDEO | OPT_STRING | HAS_ARG | OPT_EXPERT |
	OPT_SPEC | OPT_INPUT,{ .off = offsetof(OptionsContext, hwaccel_output_formats) },
	"select output format used with HW accelerated decoding", "format" },

	{ "hwaccels", OPT_EXIT,{ .func_arg = show_help },
	"show available HW acceleration methods" },
	{ "autorotate", HAS_ARG | OPT_BOOL | OPT_SPEC |
	OPT_EXPERT | OPT_INPUT,{ .off = offsetof(OptionsContext, autorotate) },
	"automatically insert correct rotate filters" },


	{ "aframes", OPT_AUDIO | HAS_ARG | OPT_PERFILE | OPT_OUTPUT,{ .func_arg = opt_audio_frames },
	"set the number of audio frames to output", "number" },
	{ "aq", OPT_AUDIO | HAS_ARG | OPT_PERFILE | OPT_OUTPUT,{ .func_arg = opt_audio_qscale },
	"set audio quality (codec-specific)", "quality", },
	{ "ar", OPT_AUDIO | HAS_ARG | OPT_INT | OPT_SPEC |
	OPT_INPUT | OPT_OUTPUT,{ .off = offsetof(OptionsContext, audio_sample_rate) },
	"set audio sampling rate (in Hz)", "rate" },
	{ "ac", OPT_AUDIO | HAS_ARG | OPT_INT | OPT_SPEC |
	OPT_INPUT | OPT_OUTPUT,{ .off = offsetof(OptionsContext, audio_channels) },
	"set number of audio channels", "channels" },
	{ "an", OPT_AUDIO | OPT_BOOL | OPT_OFFSET | OPT_INPUT | OPT_OUTPUT,{ .off = offsetof(OptionsContext, audio_disable) },
	"disable audio" },
	{ "acodec", OPT_AUDIO | HAS_ARG | OPT_PERFILE |
	OPT_INPUT | OPT_OUTPUT,{ .func_arg = opt_audio_codec },
	"force audio codec ('copy' to copy stream)", "codec" },
	{ "atag", OPT_AUDIO | HAS_ARG | OPT_EXPERT | OPT_PERFILE |
	OPT_OUTPUT,{ .func_arg = opt_old2new },
	"force audio tag/fourcc", "fourcc/tag" },
	{ "vol", OPT_AUDIO | HAS_ARG | OPT_INT,{ &dummy },//audio_volmue
	"change audio volume (256=normal)" , "volume" },
	{ "sample_fmt", OPT_AUDIO | HAS_ARG | OPT_EXPERT | OPT_SPEC |
	OPT_STRING | OPT_INPUT | OPT_OUTPUT,{ .off = offsetof(OptionsContext, sample_fmts) },
	"set sample format", "format" },
	{ "channel_layout", OPT_AUDIO | HAS_ARG | OPT_EXPERT | OPT_PERFILE |
	OPT_INPUT | OPT_OUTPUT,{ .func_arg = opt_channel_layout },
	"set channel layout", "layout" },
	{ "af", OPT_AUDIO | HAS_ARG | OPT_PERFILE | OPT_OUTPUT,{ .func_arg = opt_audio_filters },
	"set audio filters", "filter_graph" },
	{ "guess_layout_max", OPT_AUDIO | HAS_ARG | OPT_INT | OPT_SPEC | OPT_EXPERT | OPT_INPUT,{ .off = offsetof(OptionsContext, guess_layout_max) },
	"set the maximum number of channels to try to guess the channel layout" },


	{ "sn", OPT_SUBTITLE | OPT_BOOL | OPT_OFFSET | OPT_INPUT | OPT_OUTPUT,{ .off = offsetof(OptionsContext, subtitle_disable) },
	"disable subtitle" },
	{ "scodec", OPT_SUBTITLE | HAS_ARG | OPT_PERFILE | OPT_INPUT | OPT_OUTPUT,{ .func_arg = opt_subtitle_codec },
	"force subtitle codec ('copy' to copy stream)", "codec" },
	{ "stag", OPT_SUBTITLE | HAS_ARG | OPT_EXPERT | OPT_PERFILE | OPT_OUTPUT,{ .func_arg = opt_old2new }
	, "force subtitle tag/fourcc", "fourcc/tag" },
	{ "fix_sub_duration", OPT_BOOL | OPT_EXPERT | OPT_SUBTITLE | OPT_SPEC | OPT_INPUT,{ .off = offsetof(OptionsContext, fix_sub_duration) },
	"fix subtitles duration" },
	{ "canvas_size", OPT_SUBTITLE | HAS_ARG | OPT_STRING | OPT_SPEC | OPT_INPUT,{ .off = offsetof(OptionsContext, canvas_sizes) },
	"set canvas size (WxH or abbreviation)", "size" },


	{ "vc", HAS_ARG | OPT_EXPERT | OPT_VIDEO,{ .func_arg = opt_video_channel },
	"deprecated, use -channel", "channel" },
	{ "tvstd", HAS_ARG | OPT_EXPERT | OPT_VIDEO,{ .func_arg = opt_video_standard },
	"deprecated, use -standard", "standard" },
	{ "isync", OPT_BOOL | OPT_EXPERT,{ &dummy }, "this option is deprecated and does nothing", "" },


	{ "muxdelay", OPT_FLOAT | HAS_ARG | OPT_EXPERT | OPT_OFFSET | OPT_OUTPUT,{ .off = offsetof(OptionsContext, mux_max_delay) },
	"set the maximum demux-decode delay", "seconds" },
	{ "muxpreload", OPT_FLOAT | HAS_ARG | OPT_EXPERT | OPT_OFFSET | OPT_OUTPUT,{ .off = offsetof(OptionsContext, mux_preload) },
	"set the initial demux-decode delay", "seconds" },
	{ "sdp_file", HAS_ARG | OPT_EXPERT | OPT_OUTPUT,{ .func_arg = opt_sdp_file },
	"specify a file in which to print sdp information", "file" },

	{ "time_base", HAS_ARG | OPT_STRING | OPT_EXPERT | OPT_SPEC | OPT_OUTPUT,{ .off = offsetof(OptionsContext, time_bases) },
	"set the desired time base hint for output stream (1:24, 1:48000 or 0.04166, 2.0833e-5)", "ratio" },
	{ "enc_time_base", HAS_ARG | OPT_STRING | OPT_EXPERT | OPT_SPEC | OPT_OUTPUT,{ .off = offsetof(OptionsContext, enc_time_bases) },
	"set the desired time base for the encoder (1:24, 1:48000 or 0.04166, 2.0833e-5). "
	"two special values are defined - "
	"0 = use frame rate (video) or sample rate (audio),"
	"-1 = match source time base", "ratio" },

	{ "bsf", HAS_ARG | OPT_STRING | OPT_SPEC | OPT_EXPERT | OPT_OUTPUT,{ .off = offsetof(OptionsContext, bitstream_filters) },
	"A comma-separated list of bitstream filters", "bitstream_filters" },
	{ "absf", HAS_ARG | OPT_AUDIO | OPT_EXPERT | OPT_PERFILE | OPT_OUTPUT,{ .func_arg = opt_old2new },
	"deprecated", "audio bitstream_filters" },
	{ "vbsf", OPT_VIDEO | HAS_ARG | OPT_EXPERT | OPT_PERFILE | OPT_OUTPUT,{ .func_arg = opt_old2new },
	"deprecated", "video bitstream_filters" },

	{ "apre", HAS_ARG | OPT_AUDIO | OPT_EXPERT | OPT_PERFILE | OPT_OUTPUT,{ .func_arg = opt_preset },
	"set the audio options to the indicated preset", "preset" },
	{ "vpre", OPT_VIDEO | HAS_ARG | OPT_EXPERT | OPT_PERFILE | OPT_OUTPUT,{ .func_arg = opt_preset },
	"set the video options to the indicated preset", "preset" },
	{ "spre", HAS_ARG | OPT_SUBTITLE | OPT_EXPERT | OPT_PERFILE | OPT_OUTPUT,{ .func_arg = opt_preset },
	"set the subtitle options to the indicated preset", "preset" },
	{ "fpre", HAS_ARG | OPT_EXPERT | OPT_PERFILE | OPT_OUTPUT,{ .func_arg = opt_preset },
	"set options from indicated preset file", "filename" },

	{ "max_muxing_queue_size", HAS_ARG | OPT_INT | OPT_SPEC | OPT_EXPERT | OPT_OUTPUT,{ .off = offsetof(OptionsContext, max_muxing_queue_size) },
	"maximum number of packets that can be buffered while waiting for all streams to initialize", "packets" },


	{ "dcodec", HAS_ARG | OPT_DATA | OPT_PERFILE | OPT_EXPERT | OPT_INPUT | OPT_OUTPUT,{ .func_arg = opt_data_codec },
	"force data codec ('copy' to copy stream)", "codec" },
	{ "dn", OPT_BOOL | OPT_VIDEO | OPT_OFFSET | OPT_INPUT | OPT_OUTPUT,{ .off = offsetof(OptionsContext, data_disable) },
	"disable data" },

	{ "init_hw_device", HAS_ARG | OPT_EXPERT,{ .func_arg = opt_init_hw_device },
	"initialise hardware device", "args" },
	{ "filter_hw_device", HAS_ARG | OPT_EXPERT,{ .func_arg = opt_filter_hw_device },
	"set hardware device used when filtering", "device" },

	{ NULL, },
};

static const enum AVPixelFormat *get_compliance_unofficial_pix_fmts(enum AVCodecID codec_id, const enum AVPixelFormat default_formats[]){
	if (codec_id == AV_CODEC_ID_MJPEG) {
		return mjpeg_formats;
	}else if (codec_id == AV_CODEC_ID_LJPEG) {
		return ljpeg_formats;
	}else {
		return default_formats;
	}
}

enum AVPixelFormat choose_pixel_fmt(AVStream *st, AVCodecContext *enc_ctx, AVCodec *codec, enum AVPixelFormat target){
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
			if (target != AV_PIX_FMT_NONE)
				av_log(NULL, AV_LOG_WARNING,
					"Incompatible pixel format '%s' for codec '%s', auto-selecting format '%s'\n",
					av_get_pix_fmt_name(target),
					codec->name,
					av_get_pix_fmt_name(best));
			return best;
		}
	}
	return target;
}

void choose_sample_fmt(AVStream *st, AVCodec *codec)
{
	if (codec && codec->sample_fmts) {
		const enum AVSampleFormat *p = codec->sample_fmts;
		for (; *p != -1; p++) {
			if (*p == st->codecpar->format)
				break;
		}
		if (*p == -1) {
			if ((codec->capabilities & AV_CODEC_CAP_LOSSLESS) && av_get_sample_fmt_name(st->codecpar->format) > av_get_sample_fmt_name(codec->sample_fmts[0]))
				av_log(NULL, AV_LOG_ERROR, "Conversion will not be lossless.\n");
			if (av_get_sample_fmt_name(st->codecpar->format))
				av_log(NULL, AV_LOG_WARNING,
					"Incompatible sample format '%s' for codec '%s', auto-selecting format '%s'\n",
					av_get_sample_fmt_name(st->codecpar->format),
					codec->name,
					av_get_sample_fmt_name(codec->sample_fmts[0]));
			st->codecpar->format = codec->sample_fmts[0];
		}
	}
}

static char *choose_pix_fmts(WXCtx *octx, OutputFilter *ofilter)
{
	OutputStream *ost = ofilter->ost;
	AVDictionaryEntry *strict_dict = av_dict_get(ost->encoder_opts, "strict", NULL, 0);
	if (strict_dict)

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
			exit_program(octx, 1);

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
		return ret;
	}
	else
		return NULL;
}

static char *choose_sample_fmts(WXCtx *octx, OutputFilter *ofilter) {
	if (ofilter->format != AV_SAMPLE_FMT_NONE) {
		const char *name = av_get_sample_fmt_name(ofilter->format);
		return av_strdup(name);
	}
	else if (ofilter->formats) {
		const enum AVSampleFormat *p;
		AVIOContext *s = NULL;
		uint8_t *ret;
		int len;
		if (avio_open_dyn_buf(&s) < 0)
			exit_program(octx, 1);
		for (p = (const enum AVSampleFormat *)ofilter->formats; *p != AV_SAMPLE_FMT_NONE; p++) {
			const char *name = av_get_sample_fmt_name(*p);
			avio_printf(s, "%s|", name);
		}
		len = avio_close_dyn_buf(s, &ret);
		ret[len - 1] = 0; return ret;
	}
	else
		return NULL;
}


static char *choose_sample_rates(WXCtx *octx, OutputFilter *ofilter) {
	if (ofilter->sample_rate != 0) {
		char name[16]; snprintf(name, sizeof(name), "%d", ofilter->sample_rate);;
		return av_strdup(name);
	}
	else if (ofilter->sample_rates) {
		const int *p;
		AVIOContext *s = NULL;
		uint8_t *ret;
		int len;
		if (avio_open_dyn_buf(&s) < 0)
			exit_program(octx, 1);
		for (p = ofilter->sample_rates; *p != 0; p++) {
			char name[16]; snprintf(name, sizeof(name), "%d", *p);;
			avio_printf(s, "%s|", name);
		}
		len = avio_close_dyn_buf(s, &ret);
		ret[len - 1] = 0;
		return ret;
	}
	else return NULL;
}


static char *choose_channel_layouts(WXCtx *octx, OutputFilter *ofilter) {
	if (ofilter->channel_layout != 0) {
		char name[16]; snprintf(name, sizeof(name), "0x%"PRIx64, ofilter->channel_layout);;
		return av_strdup(name);
	}
	else if (ofilter->channel_layouts) {
		const uint64_t *p;
		AVIOContext *s = NULL;
		uint8_t *ret;
		int len;
		if (avio_open_dyn_buf(&s) < 0)
			exit_program(octx, 1);
		for (p = ofilter->channel_layouts; *p != 0; p++) {
			char name[16]; snprintf(name, sizeof(name), "0x%"PRIx64, *p);;
			avio_printf(s, "%s|", name);
		}
		len = avio_close_dyn_buf(s, &ret);
		ret[len - 1] = 0;
		return ret;
	}
	else
		return NULL;
}


int init_simple_filtergraph(InputStream *ist, OutputStream *ost) {
	WXCtx *octx = ist->octx;
	FilterGraph *fg = av_mallocz(sizeof(*fg));
	fg->octx = octx;
	fg->index = octx->nb_filtergraphs;
	fg->outputs = grow_array(fg->outputs, sizeof(*fg->outputs), &fg->nb_outputs, fg->nb_outputs + 1);
	fg->outputs[0] = av_mallocz(sizeof(*fg->outputs[0]));

	fg->outputs[0]->octx = octx;
	fg->outputs[0]->ost = ost;
	fg->outputs[0]->graph = fg;
	fg->outputs[0]->format = -1;

	ost->filter = fg->outputs[0];
	fg->inputs = grow_array(fg->inputs, sizeof(*fg->inputs), &fg->nb_inputs, fg->nb_inputs + 1);
	fg->inputs[0] = av_mallocz(sizeof(*fg->inputs[0]));
	fg->inputs[0]->octx = octx;
	fg->inputs[0]->ist = ist;
	fg->inputs[0]->graph = fg;
	fg->inputs[0]->format = -1;
	fg->inputs[0]->frame_queue = av_fifo_alloc(8 * sizeof(AVFrame*));
	ist->filters = grow_array(ist->filters, sizeof(*ist->filters), &ist->nb_filters, ist->nb_filters + 1);
	ist->filters[ist->nb_filters - 1] = fg->inputs[0];
	octx->filtergraphs = grow_array(octx->filtergraphs, sizeof(*octx->filtergraphs), &octx->nb_filtergraphs, octx->nb_filtergraphs + 1);
	octx->filtergraphs[octx->nb_filtergraphs - 1] = fg;
	return 0;
}

static char *describe_filter_link(FilterGraph *fg, AVFilterInOut *inout, int in) {
	AVFilterContext *ctx = inout->filter_ctx;
	AVFilterPad *pads = in ? ctx->input_pads : ctx->output_pads;
	int nb_pads = in ? ctx->nb_inputs : ctx->nb_outputs;
	AVIOContext *pb;
	uint8_t *res = NULL;

	if (avio_open_dyn_buf(&pb) < 0)
		exit_program(fg->octx, 1);

	avio_printf(pb, "%s", ctx->filter->name);
	if (nb_pads > 1)
		avio_printf(pb, ":%s", avfilter_pad_get_name(pads, inout->pad_idx));
	avio_w8(pb, 0);
	avio_close_dyn_buf(pb, &res);
	return res;
}

static void init_input_filter(FilterGraph *fg, AVFilterInOut *in)
{
	WXCtx *octx = fg->octx;
	InputStream *ist = NULL;
	enum AVMediaType type = avfilter_pad_get_type(in->filter_ctx->input_pads, in->pad_idx);
	int i;
	if (type != AVMEDIA_TYPE_VIDEO && type != AVMEDIA_TYPE_AUDIO) {
		av_log(NULL, AV_LOG_FATAL, "Only video and audio filters supported "
			"currently.\n");
		exit_program(octx, 1);
	}
	if (in->name) {
		AVFormatContext *s;
		AVStream *st = NULL;
		char *p;
		int file_idx = strtol(in->name, &p, 0);

		if (file_idx < 0 || file_idx >= octx->nb_input_files) {
			exit_program(octx, 1);
		}
		s = octx->input_files[file_idx]->ctx;

		for (i = 0; i < s->nb_streams; i++) {
			enum AVMediaType stream_type = s->streams[i]->codecpar->codec_type;
			if (stream_type != type &&
				!(stream_type == AVMEDIA_TYPE_SUBTITLE &&
					type == AVMEDIA_TYPE_VIDEO))
				continue;
			if (check_stream_specifier(s, s->streams[i], *p == ':' ? p + 1 : p) == 1) {
				st = s->streams[i];
				break;
			}
		}
		if (!st) {
			exit_program(octx, 1);
		}
		ist = octx->input_streams[octx->input_files[file_idx]->ist_index + st->index];
	}
	else {

		for (i = 0; i < octx->nb_input_streams; i++) {
			ist = octx->input_streams[i];
			if (ist->dec_ctx->codec_type == type && ist->discard)
				break;
		}
		if (i == octx->nb_input_streams) {
			exit_program(octx, 1);
		}
	}
	av_assert0(ist);

	ist->discard = 0;
	ist->decoding_needed |= DECODING_FOR_FILTER;
	ist->st->discard = AVDISCARD_NONE;
	fg->inputs = grow_array(fg->inputs, sizeof(*fg->inputs), &fg->nb_inputs, fg->nb_inputs + 1);
	fg->inputs[fg->nb_inputs - 1] = av_mallocz(sizeof(*fg->inputs[0]));
	fg->inputs[fg->nb_inputs - 1]->octx = octx;
	fg->inputs[fg->nb_inputs - 1]->ist = ist;
	fg->inputs[fg->nb_inputs - 1]->graph = fg;
	fg->inputs[fg->nb_inputs - 1]->format = -1;
	fg->inputs[fg->nb_inputs - 1]->type = ist->st->codecpar->codec_type;
	fg->inputs[fg->nb_inputs - 1]->name = describe_filter_link(fg, in, 1);
	fg->inputs[fg->nb_inputs - 1]->frame_queue = av_fifo_alloc(8 * sizeof(AVFrame*));
	ist->filters = grow_array(ist->filters, sizeof(*ist->filters), &ist->nb_filters, ist->nb_filters + 1);
	ist->filters[ist->nb_filters - 1] = fg->inputs[fg->nb_inputs - 1];
}

int init_complex_filtergraph(FilterGraph *fg) {
	WXCtx *octx = fg->octx;
	AVFilterInOut *inputs, *outputs, *cur;
	AVFilterGraph *graph;
	int ret = 0;
	graph = avfilter_graph_alloc();
	graph->nb_threads = 1;

	ret = avfilter_graph_parse2(graph, fg->graph_desc, &inputs, &outputs);
	if (ret < 0)
		goto fail;

	for (cur = inputs; cur; cur = cur->next)
		init_input_filter(fg, cur);

	for (cur = outputs; cur;) {
		fg->outputs = grow_array(fg->outputs, sizeof(*fg->outputs), &fg->nb_outputs, fg->nb_outputs + 1);
		fg->outputs[fg->nb_outputs - 1] = av_mallocz(sizeof(*fg->outputs[0]));
		fg->outputs[fg->nb_outputs - 1]->graph = fg;
		fg->outputs[fg->nb_outputs - 1]->octx = octx;
		fg->outputs[fg->nb_outputs - 1]->out_tmp = cur;
		fg->outputs[fg->nb_outputs - 1]->type = avfilter_pad_get_type(cur->filter_ctx->output_pads,
			cur->pad_idx);
		fg->outputs[fg->nb_outputs - 1]->name = describe_filter_link(fg, cur, 0);
		cur = cur->next;
		fg->outputs[fg->nb_outputs - 1]->out_tmp->next = NULL;
	}

fail:
	avfilter_inout_free(&inputs);
	avfilter_graph_free(&graph);
	return ret;
}

static int insert_trim(int64_t start_time, int64_t duration,
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

static int insert_filter(AVFilterContext **last_filter, int *pad_idx,
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

static int configure_output_video_filter(FilterGraph *fg, OutputFilter *ofilter, AVFilterInOut *out)
{
	WXCtx *octx = fg->octx;
	char *pix_fmts;
	OutputStream *ost = ofilter->ost;
	OutputFile *of = octx->output_files[ost->file_index];
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

	if ((pix_fmts = choose_pix_fmts(octx, ofilter))) {
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

static int configure_output_audio_filter(FilterGraph *fg, OutputFilter *ofilter, AVFilterInOut *out)
{
	WXCtx *octx = fg->octx;
	OutputStream *ost = ofilter->ost;
	OutputFile *of = octx->output_files[ost->file_index];
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
	if (ost->audio_channels_mapped) {
		int i;
		AVBPrint pan_buf;
		av_bprint_init(&pan_buf, 256, 8192);
		av_bprintf(&pan_buf, "0x%"PRIx64,
			av_get_default_channel_layout(ost->audio_channels_mapped));
		for (i = 0; i < ost->audio_channels_mapped; i++)
			if (ost->audio_channels_map[i] != -1)
				av_bprintf(&pan_buf, "|c%d=c%d", i, ost->audio_channels_map[i]);

		do { AVFilterContext *filt_ctx; av_log(NULL, AV_LOG_INFO, "-map_channel" " is forwarded to lavfi " "similarly to -af " "pan" "=%s.\n", pan_buf.str); ret = avfilter_graph_create_filter(&filt_ctx, avfilter_get_by_name("pan"), "pan", pan_buf.str, NULL, fg->graph); if (ret < 0) return ret; ret = avfilter_link(last_filter, pad_idx, filt_ctx, 0); if (ret < 0) return ret; last_filter = filt_ctx; pad_idx = 0; } while (0);
		av_bprint_finalize(&pan_buf, NULL);
	}

	if (codec->channels && !codec->channel_layout)
		codec->channel_layout = av_get_default_channel_layout(codec->channels);

	sample_fmts = choose_sample_fmts(octx, ofilter);
	sample_rates = choose_sample_rates(octx, ofilter);
	channel_layouts = choose_channel_layouts(octx, ofilter);
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

	if (octx->audio_volume != 256 && 0) {
		char args[256];
		snprintf(args, sizeof(args), "%f", octx->audio_volume / 256.);
		do { AVFilterContext *filt_ctx; av_log(NULL, AV_LOG_INFO, "-vol" " is forwarded to lavfi " "similarly to -af " "volume" "=%s.\n", args); ret = avfilter_graph_create_filter(&filt_ctx, avfilter_get_by_name("volume"), "volume", args, NULL, fg->graph); if (ret < 0) return ret; ret = avfilter_link(last_filter, pad_idx, filt_ctx, 0); if (ret < 0) return ret; last_filter = filt_ctx; pad_idx = 0; } while (0);
	}

	if (ost->apad && of->shortest) {
		char args[256];
		int i;

		for (i = 0; i<of->ctx->nb_streams; i++)
			if (of->ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
				break;

		if (i<of->ctx->nb_streams) {
			snprintf(args, sizeof(args), "%s", ost->apad);
			do { AVFilterContext *filt_ctx; av_log(NULL, AV_LOG_INFO, "-apad" " is forwarded to lavfi " "similarly to -af " "apad" "=%s.\n", args); ret = avfilter_graph_create_filter(&filt_ctx, avfilter_get_by_name("apad"), "apad", args, NULL, fg->graph); if (ret < 0) return ret; ret = avfilter_link(last_filter, pad_idx, filt_ctx, 0); if (ret < 0) return ret; last_filter = filt_ctx; pad_idx = 0; } while (0);
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

int configure_output_filter(FilterGraph *fg, OutputFilter *ofilter, AVFilterInOut *out)
{
	WXCtx *octx = fg->octx;
	if (!ofilter->ost) {
		av_log(NULL, AV_LOG_FATAL, "Filter %s has an unconnected output\n", ofilter->name);
		exit_program(octx, 1);
	}

	switch (avfilter_pad_get_type(out->filter_ctx->output_pads, out->pad_idx)) {
	case AVMEDIA_TYPE_VIDEO: return configure_output_video_filter(fg, ofilter, out);
	case AVMEDIA_TYPE_AUDIO: return configure_output_audio_filter(fg, ofilter, out);
	default: av_assert0(0);
	}
}

void check_filter_outputs(WXCtx *octx)
{
	int i;
	for (i = 0; i < octx->nb_filtergraphs; i++) {
		int n;
		for (n = 0; n < octx->filtergraphs[i]->nb_outputs; n++) {
			OutputFilter *output = octx->filtergraphs[i]->outputs[n];
			if (!output->ost) {
				exit_program(octx, 1);
			}
		}
	}
}

static int sub2video_prepare(InputStream *ist, InputFilter *ifilter)
{
	WXCtx *octx = ist->octx;
	AVFormatContext *avf = octx->input_files[ist->file_index]->ctx;
	int i, w, h;
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
	ifilter->format = AV_PIX_FMT_RGB32;
	ist->sub2video.frame = av_frame_alloc();
	ist->sub2video.last_pts = INT64_MIN;
	return 0;
}

static int configure_input_video_filter(FilterGraph *fg, InputFilter *ifilter,
	AVFilterInOut *in)
{
	WXCtx *octx = fg->octx;
	AVFilterContext *last_filter;
	const AVFilter *buffer_filt = avfilter_get_by_name("buffer");
	InputStream *ist = ifilter->ist;
	InputFile *f = octx->input_files[ist->file_index];
	AVRational tb = ist->framerate.num ? av_inv_q(ist->framerate) :
		ist->st->time_base;
	AVRational fr = ist->framerate;
	AVRational sar;
	AVBPrint args;
	char name[255];
	int ret, pad_idx = 0;
	int64_t tsoffset = 0;
	AVBufferSrcParameters *par = av_buffersrc_parameters_alloc();
	memset(par, 0, sizeof(*par));
	par->format = AV_PIX_FMT_NONE;

	if (ist->dec_ctx->codec_type == AVMEDIA_TYPE_AUDIO) {
		av_log(NULL, AV_LOG_ERROR, "Cannot connect video filter to audio input\n");
		ret = AVERROR(EINVAL);
		goto fail;
	}

	if (!fr.num)
		fr = av_guess_frame_rate(octx->input_files[ist->file_index]->ctx, ist->st, NULL);

	if (ist->dec_ctx->codec_type == AVMEDIA_TYPE_SUBTITLE) {
		ret = sub2video_prepare(ist, ifilter);
		if (ret < 0)
			goto fail;
	}

	sar = ifilter->sample_aspect_ratio;
	if (!sar.den)
		sar = (AVRational) { 0, 1 };
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


	snprintf(name, sizeof(name), "trim_in_%d_%d",
		ist->file_index, ist->st->index);

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

static int configure_input_audio_filter(FilterGraph *fg, InputFilter *ifilter,
	AVFilterInOut *in)
{
	WXCtx *octx = fg->octx;
	AVFilterContext *last_filter;
	const AVFilter *abuffer_filt = avfilter_get_by_name("abuffer");
	InputStream *ist = ifilter->ist;
	InputFile *f = octx->input_files[ist->file_index];
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
		av_get_sample_fmt_name(ifilter->format));
	if (ifilter->channel_layout)
		av_bprintf(&args, ":channel_layout=0x%"PRIx64,
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

	if (octx->audio_volume != 256) {
		char args[256];
		snprintf(args, sizeof(args), "%f", octx->audio_volume / 256.);
		do {
			AVFilterContext *filt_ctx;
			snprintf(name, sizeof(name), "graph_%d_%s_in_%d_%d", fg->index, "volume", ist->file_index, ist->st->index);
			ret = avfilter_graph_create_filter(&filt_ctx, avfilter_get_by_name("volume"), name, args, NULL, fg->graph);
			if (ret < 0)
				return ret;
			ret = avfilter_link(last_filter, 0, filt_ctx, 0);
			if (ret < 0)
				return ret;
			last_filter = filt_ctx;
		} while (0);
	}

	snprintf(name, sizeof(name), "trim for input stream %d:%d",
		ist->file_index, ist->st->index);

	ret = insert_trim(((f->start_time == AV_NOPTS_VALUE) || !f->accurate_seek) ?
		AV_NOPTS_VALUE : tsoffset, f->recording_time,
		&last_filter, &pad_idx, name);
	if (ret < 0)
		return ret;

	if ((ret = avfilter_link(last_filter, 0, in->filter_ctx, in->pad_idx)) < 0)
		return ret;

	return 0;
}

static int configure_input_filter(FilterGraph *fg, InputFilter *ifilter,
	AVFilterInOut *in)
{
	WXCtx *octx = fg->octx;
	if (!ifilter->ist->dec) {
		return AVERROR_DECODER_NOT_FOUND;
	}
	switch (avfilter_pad_get_type(in->filter_ctx->input_pads, in->pad_idx)) {
	case AVMEDIA_TYPE_VIDEO: return configure_input_video_filter(fg, ifilter, in);
	case AVMEDIA_TYPE_AUDIO: return configure_input_audio_filter(fg, ifilter, in);
	default: av_assert0(0);
	}
}

static void cleanup_filtergraph(FilterGraph *fg)
{
	WXCtx *octx = fg->octx;
	int i;
	for (i = 0; i < fg->nb_outputs; i++)
		fg->outputs[i]->filter = (AVFilterContext *)NULL;
	for (i = 0; i < fg->nb_inputs; i++)
		fg->inputs[i]->filter = (AVFilterContext *)NULL;
	avfilter_graph_free(&fg->graph);
}

int configure_filtergraph(FilterGraph *fg)
{
	WXCtx *octx = fg->octx;
	AVFilterInOut *inputs, *outputs, *cur;
	int ret, i, simple = filtergraph_is_simple(fg);
	const char *graph_desc = simple ? fg->outputs[0]->ost->avfilter :
		fg->graph_desc;

	cleanup_filtergraph(fg);
	fg->graph = avfilter_graph_alloc();

	if (simple) {
		OutputStream *ost = fg->outputs[0]->ost;
		char args[512];
		AVDictionaryEntry *e = NULL;

		fg->graph->nb_threads = 0;

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
		fg->graph->nb_threads = 0;
	}

	if ((ret = avfilter_graph_parse2(fg->graph, graph_desc, &inputs, &outputs)) < 0)
		goto fail;

	if (octx->filter_hw_device || octx->hw_device_ctx) {
		AVBufferRef *device = octx->filter_hw_device ? octx->filter_hw_device->device_ref
			: octx->hw_device_ctx;
		for (i = 0; i < fg->graph->nb_filters; i++) {
			fg->graph->filters[i]->hw_device_ctx = av_buffer_ref(device);
			if (!fg->graph->filters[i]->hw_device_ctx) {
				ret = AVERROR(ENOMEM);
				goto fail;
			}
		}
	}

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

	for (i = 0; i < fg->nb_inputs; i++) {
		if (fg->inputs[i]->eof) {
			ret = av_buffersrc_add_frame(fg->inputs[i]->filter, NULL);
			if (ret < 0)
				goto fail;
		}
	}

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

int ifilter_parameters_from_frame(InputFilter *ifilter, const AVFrame *frame){
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

int ist_in_filtergraph(FilterGraph *fg, InputStream *ist)
{
	int i;
	for (i = 0; i < fg->nb_inputs; i++)
		if (fg->inputs[i]->ist == ist)
			return 1;
	return 0;
}

int filtergraph_is_simple(FilterGraph *fg) {
	return !fg->graph_desc;
}


void init_opts(WXCtx *octx) {
	av_dict_set(&octx->sws_dict, "flags", "bicubic", 0);
}

void uninit_opts(WXCtx *octx)
{
	av_dict_free(&octx->swr_opts);
	av_dict_free(&octx->sws_dict);
	av_dict_free(&octx->format_opts);
	av_dict_free(&octx->codec_opts);
	av_dict_free(&octx->resample_opts);
}

void exit_program(WXCtx *octx, int ret){
	octx->avffmpeg_state = ret;
	octx->avffmpeg_pts_curr = 0;
	octx->avffmpeg_pts_total = 0;
	avffmpeg_OnError(octx, ret);
    ffmpeg_cleanup(octx, ret);
	longjmp(octx->avffmpeg_jmpbuf, ret);
}

static const OptionDef *find_option(const OptionDef *po, const char *name){
	const char *p = strchr(name, ':');
	int len = p ? p - name : strlen(name);
	while (po->name) {
		if (!strncmp(name, po->name, len) && strlen(po->name) == len)
			break;
		po++;
	}
	return po;
}

static int write_option(WXCtx *octx, void *optctx, const OptionDef *po, const char *opt, const char *arg){
	void *dst = po->flags & (OPT_OFFSET | OPT_SPEC) ?
		(uint8_t *)optctx + po->u.off : po->u.dst_ptr;
	int *dstcount;

	if (po->flags & OPT_SPEC) {
		SpecifierOpt **so = dst;
		char *p = strchr(opt, ':');
		char *str;

		dstcount = (int *)(so + 1);
		*so = grow_array(*so, sizeof(**so), dstcount, *dstcount + 1);
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
		*(int *)dst = parse_number_or_die(arg, OPT_INT64, INT_MIN, INT_MAX);
	}
	else if (po->flags & OPT_INT64) {
		*(int64_t *)dst = parse_number_or_die(arg, OPT_INT64, (double)INT64_MIN, (double)INT64_MAX);
	}
	else if (po->flags & OPT_TIME) {
		*(int64_t *)dst = parse_time_or_die(arg, 1);
	}
	else if (po->flags & OPT_FLOAT) {
		*(float *)dst = parse_number_or_die(arg, OPT_FLOAT, -INFINITY, INFINITY);
	}
	else if (po->flags & OPT_DOUBLE) {
		*(double *)dst = parse_number_or_die(arg, OPT_DOUBLE, -INFINITY, INFINITY);
	}
	else if (po->u.func_arg) {
		int ret = po->u.func_arg(octx, optctx, opt, arg);
		if (ret < 0) {
			return ret;
		}
	}
	if (po->flags & OPT_EXIT)
		exit_program(octx, 0);

	return 0;
}

int parse_option(WXCtx *octx, void *optctx, const char *opt, const char *arg,
	const OptionDef *options)
{
	OptionsContext *o = optctx;
	const OptionDef *po;
	int ret;

	po = find_option(options, opt);
	if (!po->name && opt[0] == 'n' && opt[1] == 'o') {

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

	ret = write_option(octx, optctx, po, opt, arg);
	if (ret < 0)
		return ret;

	return !!(po->flags & HAS_ARG);
}

void parse_options(WXCtx *octx, void *optctx, int argc, char **argv, const OptionDef *options,
	void(*parse_arg_function)(void *, const char*))
{
	OptionsContext *ok = (OptionsContext*)optctx;

	const char *opt;
	int optindex, handleoptions = 1, ret;


	optindex = 1;
	while (optindex < argc) {
		opt = argv[optindex++];

		if (handleoptions && opt[0] == '-' && opt[1] != '\0') {
			if (opt[1] == '-' && opt[2] == '\0') {
				handleoptions = 0;
				continue;
			}
			opt++;

			if ((ret = parse_option(octx, optctx, opt, argv[optindex], options)) < 0)
				exit_program(octx, 1);
			optindex += ret;
		}
		else {
			if (parse_arg_function)
				parse_arg_function(optctx, opt);
		}
	}
}

int parse_optgroup(WXCtx *octx, void *optctx, OptionGroup *g)
{
	int i, ret;

	av_log(NULL, AV_LOG_DEBUG, "Parsing a group of options: %s %s.\n",
		g->group_def->name, g->arg);

	for (i = 0; i < g->nb_opts; i++) {
		Option *o = &g->opts[i];
		if (g->group_def->flags &&
			!(g->group_def->flags & o->opt->flags)) {
			return AVERROR(EINVAL);
		}
		ret = write_option(octx, optctx, o->opt, o->key, o->val);
		if (ret < 0)
			return ret;
	}
	return 0;
}

static void check_options(const OptionDef *po) {
	while (po->name) {
		if (po->flags & OPT_PERFILE)
			av_assert0(po->flags & (OPT_INPUT | OPT_OUTPUT));
		po++;
	}
}


static const AVOption *opt_find(void *obj, const char *name, const char *unit,
	int opt_flags, int search_flags)
{
	const AVOption *o = av_opt_find(obj, name, unit, opt_flags, search_flags);
	if (o && !o->flags)
		return NULL;
	return o;
}


int opt_default(WXCtx *octx, void *optctx, const char *opt, const char *arg)
{
	OptionsContext *oc = optctx;

	const AVOption *o;
	int consumed = 0;
	char opt_stripped[128];
	const char *p;
	const AVClass *cc = avcodec_get_class(), *fc = avformat_get_class();

	if (!strcmp(opt, "debug") || !strcmp(opt, "fdebug"))
		av_log_set_level(AV_LOG_DEBUG);

	if (!(p = strchr(opt, ':')))
		p = opt + strlen(opt);
	av_strlcpy(opt_stripped, opt, FFMIN(sizeof(opt_stripped), p - opt + 1));

	if ((o = opt_find(&cc, opt_stripped, NULL, 0,
		AV_OPT_SEARCH_CHILDREN | AV_OPT_SEARCH_FAKE_OBJ)) ||
		((opt[0] == 'v' || opt[0] == 'a' || opt[0] == 's') &&
		(o = opt_find(&cc, opt + 1, NULL, 0, AV_OPT_SEARCH_FAKE_OBJ)))) {
		av_dict_set(&octx->codec_opts, opt, arg, (o->type == AV_OPT_TYPE_FLAGS && (arg[0] == '-' || arg[0] == '+')) ? AV_DICT_APPEND : 0);
		consumed = 1;
	}
	if ((o = opt_find(&fc, opt, NULL, 0,
		AV_OPT_SEARCH_CHILDREN | AV_OPT_SEARCH_FAKE_OBJ))) {
		av_dict_set(&octx->format_opts, opt, arg, (o->type == AV_OPT_TYPE_FLAGS && (arg[0] == '-' || arg[0] == '+')) ? AV_DICT_APPEND : 0);
		if (consumed)
			av_log(NULL, AV_LOG_VERBOSE, "Routing option %s to both codec and muxer layer\n", opt);
		consumed = 1;
	}


	if (!consumed && !strcmp(opt, "sws_flags")) {
		av_log(NULL, AV_LOG_WARNING, "Ignoring %s %s, due to disabled swscale\n", opt, arg);
		consumed = 1;
	}

	if (consumed)
		return 0;
	return AVERROR_OPTION_NOT_FOUND;
}

static int match_group_separator(const OptionGroupDef *groups, int nb_groups,
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

static void finish_group(WXCtx *octx, int group_idx, const char *arg) {
	OptionGroupList *l = &octx->groups[group_idx];
	OptionGroup *g;
	l->groups = grow_array(l->groups, sizeof(*l->groups), &l->nb_groups, l->nb_groups + 1);
	g = &l->groups[l->nb_groups - 1];
	*g = octx->cur_group;
	g->arg = arg;
	g->group_def = l->group_def;
	g->sws_dict = octx->sws_dict;
	g->swr_opts = octx->swr_opts;
	g->codec_opts = octx->codec_opts;
	g->format_opts = octx->format_opts;
	g->resample_opts = octx->resample_opts;
	octx->codec_opts = NULL;
	octx->format_opts = NULL;
	octx->resample_opts = NULL;
	octx->sws_dict = NULL;
	octx->swr_opts = NULL;
	init_opts(octx);
	memset(&octx->cur_group, 0, sizeof(octx->cur_group));
}

static void add_opt(WXCtx *octx, const OptionDef *opt,
	const char *key, const char *val)
{
	int global = !(opt->flags & (OPT_PERFILE | OPT_SPEC | OPT_OFFSET));
	OptionGroup *g = global ? &octx->global_opts : &octx->cur_group;

	g->opts = grow_array(g->opts, sizeof(*g->opts), &g->nb_opts, g->nb_opts + 1);
	g->opts[g->nb_opts - 1].opt = opt;
	g->opts[g->nb_opts - 1].key = key;
	g->opts[g->nb_opts - 1].val = val;
}

static void init_parse_context(WXCtx *octx,
	const OptionGroupDef *groups, int nb_groups)
{
	static const OptionGroupDef global_group = { "global" };
	int i;
	octx->nb_groups = nb_groups;
	octx->groups = av_mallocz_array(octx->nb_groups, sizeof(*octx->groups));
	for (i = 0; i < octx->nb_groups; i++)
		octx->groups[i].group_def = &groups[i];

	octx->global_opts.group_def = &global_group;
	octx->global_opts.arg = "";

	init_opts(octx);
}

void uninit_parse_context(WXCtx *octx)
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

	uninit_opts(octx);
}

int split_commandline(WXCtx *octx, int argc, char *argv[],
	const OptionDef *options,
	const OptionGroupDef *groups,
	int nb_groups)
{
	int optindex = 1;
	int dashdash = -2;
	init_parse_context(octx, groups, nb_groups);

	while (optindex < argc) {
		const char *opt = argv[optindex++], *arg;
		const OptionDef *po;
		int ret;

		if (opt[0] == '-' && opt[1] == '-' && !opt[2]) {
			dashdash = optindex;
			continue;
		}

		if (opt[0] != '-' || !opt[1] || dashdash + 1 == optindex) {
			finish_group(octx, 0, opt);
			continue;
		}
		opt++;

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


		po = find_option(options, opt);
		if (po->name) {
			if (po->flags & OPT_EXIT) {

				arg = argv[optindex++];
			}
			else if (po->flags & HAS_ARG) {
				do {
					arg = argv[optindex++];
					if (!arg) {
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


		if (argv[optindex]) {
			ret = opt_default(octx, NULL, opt, argv[optindex]);
			if (ret >= 0) {
				optindex++;
				continue;
			}
			else if (ret != AVERROR_OPTION_NOT_FOUND) {
				return ret;
			}
		}

		if (opt[0] == 'n' && opt[1] == 'o' &&
			(po = find_option(options, opt + 2)) &&
			po->name && po->flags & OPT_BOOL) {
			add_opt(octx, po, opt, "0");
			continue;
		}
		return AVERROR_OPTION_NOT_FOUND;
	}
	return 0;
}


static unsigned get_codecs_sorted(WXCtx *octx, const AVCodecDescriptor ***rcodecs) {
	const AVCodecDescriptor *desc = NULL;
	const AVCodecDescriptor **codecs;
	unsigned nb_codecs = 0, i = 0;

	while ((desc = avcodec_descriptor_next(desc)))
		nb_codecs++;
	if (!(codecs = av_calloc(nb_codecs, sizeof(*codecs)))) {
		av_log(NULL, AV_LOG_ERROR, "Out of memory\n");
		exit_program(octx, 1);
	}
	desc = NULL;
	while ((desc = avcodec_descriptor_next(desc)))
		codecs[i++] = desc;
	av_assert0(i == nb_codecs);
	qsort(codecs, nb_codecs, sizeof(*codecs), compare_codec_desc);
	*rcodecs = codecs;
	return nb_codecs;
}

FILE *get_preset_file(char *filename, size_t filename_size,
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

		char datadir[MAX_PATH], *ls;
		base[2] = NULL;

#ifdef _WIN32
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

int check_stream_specifier(AVFormatContext *s, AVStream *st, const char *spec)
{
	int ret = avformat_match_stream_specifier(s, st, spec);
	if (ret < 0)
		av_log(s, AV_LOG_ERROR, "Invalid stream specifier: %s.\n", spec);
	return ret;
}

AVDictionary *filter_codec_opts(WXCtx *octx, AVDictionary *opts, enum AVCodecID codec_id,
	AVFormatContext *s, AVStream *st, AVCodec *codec)
{
	AVDictionary *ret = NULL;
	AVDictionaryEntry *t = NULL;
	int flags = s->oformat ? AV_OPT_FLAG_ENCODING_PARAM
		: AV_OPT_FLAG_DECODING_PARAM;
	char prefix = 0;
	const AVClass *cc = avcodec_get_class();

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


		if (p)
			switch (check_stream_specifier(s, st, p + 1)) {
			case 1: *p = 0; break;
			case 0: continue;
			default: exit_program(octx, 1);
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

AVDictionary **setup_find_stream_info_opts(WXCtx *octx, AVFormatContext *s,
	AVDictionary *codec_opts)
{
	int i;
	AVDictionary **opts;

	if (!s->nb_streams)
		return NULL;
	opts = av_mallocz_array(s->nb_streams, sizeof(*opts));
	for (i = 0; i < s->nb_streams; i++)
		opts[i] = filter_codec_opts(octx, codec_opts, s->streams[i]->codecpar->codec_id,
			s, s->streams[i], NULL);
	return opts;
}

static int sub2video_get_blank_frame(InputStream *ist)
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

static void sub2video_copy_rect(uint8_t *dst, int dst_linesize, int w, int h,
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

static void sub2video_push_ref(InputStream *ist, int64_t pts)
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
		if (ret != AVERROR_EOF && ret < 0)
			av_log(NULL, AV_LOG_WARNING, "Error while add the frame to buffer source(%s).\n",
				av_err2str(ret));
	}
}

void sub2video_update(InputStream *ist, AVSubtitle *sub)
{
	AVFrame *frame = ist->sub2video.frame;
	int8_t *dst;
	int dst_linesize;
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
	dst = frame->data[0];
	dst_linesize = frame->linesize[0];
	for (i = 0; i < num_rects; i++)
		sub2video_copy_rect(dst, dst_linesize, frame->width, frame->height, sub->rects[i]);
	sub2video_push_ref(ist, pts);
	ist->sub2video.end_pts = end_pts;
}

static void sub2video_heartbeat(InputStream *ist, int64_t pts)
{
	WXCtx *octx = ist->octx;
	InputFile *infile = octx->input_files[ist->file_index];
	int i, j, nb_reqs;
	int64_t pts2;

	for (i = 0; i < infile->nb_streams; i++) {
		InputStream *ist2 = octx->input_streams[infile->ist_index + i];
		if (!ist2->sub2video.frame)
			continue;


		pts2 = av_rescale_q(pts, ist->st->time_base, ist2->st->time_base) - 1;

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

static void sub2video_flush(InputStream *ist)
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

void ffmpeg_cleanup(WXCtx *octx, int ret) {
	int i, j;
	octx->avffmpeg_pts_curr = 0;

	for (i = 0; i < octx->nb_filtergraphs; i++) {
		FilterGraph *fg = octx->filtergraphs[i];
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

		av_freep(&octx->filtergraphs[i]);
	}
	octx->nb_filtergraphs = 0;
	av_freep(&octx->filtergraphs);

	av_freep(&octx->subtitle_out);


	for (i = 0; i < octx->nb_output_files; i++) {
		OutputFile *of = octx->output_files[i];
		AVFormatContext *s;
		if (!of)
			continue;
		s = of->ctx;
		if (s && s->oformat && !(s->oformat->flags & AVFMT_NOFILE))
			avio_closep(&s->pb);
		avformat_free_context(s);
		av_dict_free(&of->opts);

		av_freep(&octx->output_files[i]);
	}
	octx->nb_output_files = 0;


	for (i = 0; i < octx->nb_output_streams; i++) {
		OutputStream *ost = octx->output_streams[i];

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

		av_freep(&octx->output_streams[i]);
	}
	octx->nb_output_streams = 0;

	for (i = 0; i < octx->nb_input_files; i++) {
		avformat_close_input(&octx->input_files[i]->ctx);
		avformat_free_context(octx->input_files[i]->ctx);
		av_freep(&octx->input_files[i]);
	}
	octx->nb_input_files = 0;


	for (i = 0; i < octx->nb_input_streams; i++) {
		InputStream *ist = octx->input_streams[i];

		av_frame_free(&ist->decoded_frame);
		av_frame_free(&ist->filter_frame);
		av_dict_free(&ist->decoder_opts);
		avsubtitle_free(&ist->prev_sub.subtitle);
		av_frame_free(&ist->sub2video.frame);
		av_freep(&ist->filters);
		av_freep(&ist->hwaccel_device);
		av_freep(&ist->dts_buffer);

		avcodec_free_context(&ist->dec_ctx);

		av_freep(&octx->input_streams[i]);
	}
	octx->nb_input_streams = 0;

	av_freep(&octx->input_streams);
	av_freep(&octx->input_files);
	av_freep(&octx->output_streams);
	av_freep(&octx->output_files);

	uninit_opts(octx);
}

void remove_avoptions(AVDictionary **a, AVDictionary *b)
{
	AVDictionaryEntry *t = NULL;

	while ((t = av_dict_get(b, "", t, AV_DICT_IGNORE_SUFFIX))) {
		av_dict_set(a, t->key, NULL, AV_DICT_MATCH_CASE);
	}
}

void assert_avoptions(WXCtx *octx, AVDictionary *m)
{
	AVDictionaryEntry *t;
	if ((t = av_dict_get(m, "", NULL, AV_DICT_IGNORE_SUFFIX))) {
		av_log(NULL, AV_LOG_FATAL, "Option %s not found.\n", t->key);
		exit_program(octx, 1);
	}
}


static void close_all_output_streams(OutputStream *ost, OSTFinished this_stream, OSTFinished others)
{
	WXCtx *octx = ost->octx;
	int i;
	for (i = 0; i < octx->nb_output_streams; i++) {
		OutputStream *ost2 = octx->output_streams[i];
		ost2->finished |= ost == ost2 ? this_stream : others;
	}
}

static void write_packet(OutputFile *of, AVPacket *pkt, OutputStream *ost, int unqueue)
{
	AVFormatContext *s = of->ctx;
	AVStream *st = ost->st;
	int ret;

	if (!(st->codecpar->codec_type == AVMEDIA_TYPE_VIDEO && ost->encoding_needed) && !unqueue) {
		if (ost->frame_number >= ost->max_frames) {
			av_packet_unref(pkt);
			return;
		}
		ost->frame_number++;
	}

	if (!of->header_written) {
		AVPacket tmp_pkt = { 0 };

		if (!av_fifo_space(ost->muxing_queue)) {
			int new_size = FFMIN(2 * av_fifo_size(ost->muxing_queue),
				ost->max_muxing_queue_size);
			if (new_size <= av_fifo_size(ost->muxing_queue)) {
				exit_program(of->octx, 1);
			}
			ret = av_fifo_realloc2(ost->muxing_queue, new_size);
		}
		ret = av_packet_ref(&tmp_pkt, pkt);
		if (ret < 0)
			exit_program(of->octx, 1);
		av_fifo_generic_write(ost->muxing_queue, &tmp_pkt, sizeof(tmp_pkt), NULL);
		av_packet_unref(pkt);
		return;
	}

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
			av_log(s, AV_LOG_WARNING, "Invalid DTS: %"PRId64" PTS: %"PRId64" in output stream %d:%d, replacing by guess\n",
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

static void close_output_stream(OutputStream *ost)
{
	WXCtx *octx = ost->octx;
	OutputFile *of = octx->output_files[ost->file_index];

	ost->finished |= ENCODER_FINISHED;
	if (of->shortest) {
		int64_t end = av_rescale_q(ost->sync_opts - ost->first_pts, ost->enc_ctx->time_base, AV_TIME_BASE_Q);
		of->recording_time = FFMIN(of->recording_time, end);
	}
}

static void output_packet(OutputFile *of, AVPacket *pkt, OutputStream *ost, int eof) {
	int ret = 0;

	if (ost->nb_bitstream_filters) {
		int idx;

		ret = av_bsf_send_packet(ost->bsf_ctx[0], eof ? NULL : pkt);
		if (ret < 0)
			goto finish;

		eof = 0;
		idx = 1;
		while (idx) {

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
	if (ret < 0 && ret != AVERROR_EOF) {
		av_log(NULL, AV_LOG_ERROR, "Error applying bitstream filters to an output "
			"packet for stream #%d:%d.\n", ost->file_index, ost->index);
	}
}

static int check_recording_time(OutputStream *ost)
{
	WXCtx *octx = ost->octx;
	OutputFile *of = octx->output_files[ost->file_index];

	if (of->recording_time != INT64_MAX &&
		av_compare_ts(ost->sync_opts - ost->first_pts, ost->enc_ctx->time_base, of->recording_time,
			AV_TIME_BASE_Q) >= 0) {
		close_output_stream(ost);
		return 0;
	}
	return 1;
}

static void do_audio_out(OutputFile *of, OutputStream *ost,
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

	if (frame->pts == AV_NOPTS_VALUE)
		frame->pts = ost->sync_opts;
	ost->sync_opts = frame->pts + frame->nb_samples;
	ost->samples_encoded += frame->nb_samples;
	ost->frames_encoded++;

	av_assert0(pkt.size || !pkt.data);

	ret = avcodec_send_frame(enc, frame);
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
	exit_program(of->octx, 1);
}

static void do_subtitle_out(OutputFile *of,
	OutputStream *ost,
	AVSubtitle *sub)
{
	WXCtx *octx = ost->octx;
	int subtitle_out_max_size = 1024 * 1024;
	int subtitle_out_size, nb, i;
	AVCodecContext *enc;
	AVPacket pkt;
	int64_t pts;

	if (sub->pts == AV_NOPTS_VALUE) {
		av_log(NULL, AV_LOG_ERROR, "Subtitle packets must have a pts\n");
		return;
	}

	enc = ost->enc_ctx;

	if (!octx->subtitle_out) {
		octx->subtitle_out = av_malloc(subtitle_out_max_size);
	}

	if (enc->codec_id == AV_CODEC_ID_DVB_SUBTITLE)
		nb = 2;
	else
		nb = 1;


	pts = sub->pts;
	if (octx->output_files[ost->file_index]->start_time != AV_NOPTS_VALUE)
		pts -= octx->output_files[ost->file_index]->start_time;
	for (i = 0; i < nb; i++) {
		unsigned save_num_rects = sub->num_rects;

		ost->sync_opts = av_rescale_q(pts, AV_TIME_BASE_Q, enc->time_base);
		if (!check_recording_time(ost))
			return;

		sub->pts = pts;

		sub->pts += av_rescale_q(sub->start_display_time, (AVRational) { 1, 1000 }, AV_TIME_BASE_Q);
		sub->end_display_time -= sub->start_display_time;
		sub->start_display_time = 0;
		if (i == 1)
			sub->num_rects = 0;

		ost->frames_encoded++;

		subtitle_out_size = avcodec_encode_subtitle(enc, octx->subtitle_out,
			subtitle_out_max_size, sub);
		if (i == 1)
			sub->num_rects = save_num_rects;
		if (subtitle_out_size < 0) {
			av_log(NULL, AV_LOG_FATAL, "Subtitle encoding failed\n");
			exit_program(octx, 1);
		}

		av_init_packet(&pkt);
		pkt.data = octx->subtitle_out;
		pkt.size = subtitle_out_size;
		pkt.pts = av_rescale_q(sub->pts, AV_TIME_BASE_Q, ost->mux_timebase);
		pkt.duration = av_rescale_q(sub->end_display_time, (AVRational) { 1, 1000 }, ost->mux_timebase);
		if (enc->codec_id == AV_CODEC_ID_DVB_SUBTITLE) {
			if (i == 0)
				pkt.pts += av_rescale_q(sub->start_display_time, (AVRational) { 1, 1000 }, ost->mux_timebase);
			else
				pkt.pts += av_rescale_q(sub->end_display_time, (AVRational) { 1, 1000 }, ost->mux_timebase);
		}
		pkt.dts = pkt.pts;
		output_packet(of, &pkt, ost, 0);
	}
}

static void do_video_out(OutputFile *of,
	OutputStream *ost,
	AVFrame *next_picture,
	double sync_ipts)
{
	WXCtx *octx = ost->octx;
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
		ist = octx->input_streams[ost->source_index];

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
		nb0_frames = nb_frames = mid_pred(ost->last_nb0_frames[0],
			ost->last_nb0_frames[1],
			ost->last_nb0_frames[2]);
	}
	else {
		delta0 = sync_ipts - ost->sync_opts;
		delta = delta0 + duration;
		nb0_frames = 0;
		nb_frames = 1;
		format_video_sync = VSYNC_AUTO;
		if (format_video_sync == VSYNC_AUTO) {
			if (!strcmp(of->ctx->oformat->name, "avi")) {
				format_video_sync = VSYNC_VFR;
			}
			else
				format_video_sync = (of->ctx->oformat->flags & AVFMT_VARIABLE_FPS) ? ((of->ctx->oformat->flags & AVFMT_NOTIMESTAMPS) ? VSYNC_PASSTHROUGH : VSYNC_VFR) : VSYNC_CFR;
			if (ist
				&& format_video_sync == VSYNC_CFR
				&& octx->input_files[ist->file_index]->ctx->nb_streams == 1
				&& octx->input_files[ist->file_index]->input_ts_offset == 0) {
				format_video_sync = VSYNC_VSCFR;
			}
			if (format_video_sync == VSYNC_CFR && 0) {
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

			if (delta < -1.1)
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

		if (!check_recording_time(ost))
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
			in_picture->pict_type = 0;

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

			if (octx->m_cbVideo) { //编码前的数据回调
				//AVFrame *new_frame = av_frame_clone(in_picture);
				AVFrame *new_frame = av_frame_alloc();
				new_frame->format = in_picture->format;
				new_frame->width = in_picture->width;
				new_frame->height = in_picture->height;
				av_frame_get_buffer(new_frame, 32);
				av_frame_copy_props(new_frame, in_picture);
				av_frame_copy(new_frame, in_picture);

				int64_t ptsVideo = new_frame->pts;
				ptsVideo = ptsVideo * 1000 * enc->time_base.num / enc->time_base.den;
				octx->m_cbVideo(octx->avffmpeg_owner, 
					new_frame->width, new_frame->height,
					new_frame->data, new_frame->linesize, ptsVideo);
				ret = avcodec_send_frame(enc, new_frame);

				av_frame_free(&new_frame);//如果直接对in_picture
			}else {
				ret = avcodec_send_frame(enc, in_picture);
			}


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


				if (ost->logfile && enc->stats_out) {
					fprintf(ost->logfile, "%s", enc->stats_out);
				}
			}
		}
		ost->sync_opts++;
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
	exit_program(octx, 1);
}


static int init_output_stream(OutputStream *ost, char *error, int error_len);

static void finish_output_stream(OutputStream *ost)
{
	WXCtx *octx = ost->octx;
	OutputFile *of = octx->output_files[ost->file_index];
	int i;

	ost->finished = ENCODER_FINISHED | MUXER_FINISHED;

	if (of->shortest) {
		for (i = 0; i < of->ctx->nb_streams; i++)
			octx->output_streams[of->ost_index + i]->finished = ENCODER_FINISHED | MUXER_FINISHED;
	}
}


static int reap_filters(WXCtx *octx, int flush)
{
	AVFrame *filtered_frame = NULL;
	int i;


	for (i = 0; i < octx->nb_output_streams; i++) {
		OutputStream *ost = octx->output_streams[i];
		OutputFile *of = octx->output_files[ost->file_index];
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
				exit_program(octx, 1);
			}
		}

		if (!ost->filtered_frame && !(ost->filtered_frame = av_frame_alloc())) {
			return AVERROR(ENOMEM);
		}
		filtered_frame = ost->filtered_frame;

		while (1) {
			double float_pts = AV_NOPTS_VALUE;
			ret = av_buffersink_get_frame_flags(filter, filtered_frame,
				AV_BUFFERSINK_FLAG_NO_REQUEST);
			if (ret < 0) {
				if (ret != AVERROR(EAGAIN) && ret != AVERROR_EOF) {
					av_log(NULL, AV_LOG_WARNING,
						"Error in av_buffersink_get_frame_flags(): %s\n", av_err2str(ret));
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

				av_assert0(0);
			}

			av_frame_unref(filtered_frame);
		}
	}

	return 0;
}

static void flush_encoders(WXCtx *octx)
{
	int i, ret;

	for (i = 0; i < octx->nb_output_streams; i++) {
		OutputStream *ost = octx->output_streams[i];
		AVCodecContext *enc = ost->enc_ctx;
		OutputFile *of = octx->output_files[ost->file_index];

		if (!ost->encoding_needed)
			continue;

		if (!ost->initialized) {
			FilterGraph *fg = ost->filter->graph;
			char error[1024] = "";

			if (ost->filter && !fg->graph) {
				int x;
				for (x = 0; x < fg->nb_inputs; x++) {
					InputFilter *ifilter = fg->inputs[x];
					if (ifilter->format < 0) {
						AVCodecParameters *par = ifilter->ist->st->codecpar;


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
					exit_program(octx, 1);
				}

				finish_output_stream(ost);
			}

			ret = init_output_stream(ost, error, sizeof(error));
			if (ret < 0) {
				av_log(NULL, AV_LOG_ERROR, "Error initializing output stream %d:%d -- %s\n",
					ost->file_index, ost->index, error);
				exit_program(octx, 1);
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
					av_log(NULL, AV_LOG_FATAL, "%s encoding failed: %s\n",
						desc,
						av_err2str(ret));
					exit_program(octx, 1);
				}
			}

			if (ret < 0 && ret != AVERROR_EOF) {
				av_log(NULL, AV_LOG_FATAL, "%s encoding failed: %s\n",
					desc,
					av_err2str(ret));
				exit_program(octx, 1);
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


static int check_output_constraints(InputStream *ist, OutputStream *ost)
{
	WXCtx *octx = ist->octx;
	OutputFile *of = octx->output_files[ost->file_index];
	int ist_index = octx->input_files[ist->file_index]->ist_index + ist->st->index;

	if (ost->source_index != ist_index)
		return 0;

	if (ost->finished)
		return 0;

	if (of->start_time != AV_NOPTS_VALUE && ist->pts < of->start_time)
		return 0;

	return 1;
}

static void do_streamcopy(InputStream *ist, OutputStream *ost, const AVPacket *pkt)
{
	WXCtx *octx = ist->octx;
	OutputFile *of = octx->output_files[ost->file_index];
	InputFile *f = octx->input_files[ist->file_index];
	int64_t start_time = (of->start_time == AV_NOPTS_VALUE) ? 0 : of->start_time;
	int64_t ost_tb_start_time = av_rescale_q(start_time, AV_TIME_BASE_Q, ost->mux_timebase);
	AVPacket opkt = { 0 };

	av_init_packet(&opkt);

	if (!pkt) {
		output_packet(of, &opkt, ost, 1);
		return;
	}

	if ((!ost->frame_number && !(pkt->flags & AV_PKT_FLAG_KEY)) &&
		!ost->copy_initial_nonkeyframes)
		return;

	if (!ost->frame_number && !ost->copy_prior_start) {
		int64_t comp_start = start_time;
		if (0 && f->start_time != AV_NOPTS_VALUE)
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
		if (f->start_time != AV_NOPTS_VALUE && 0)
			start_time += f->start_time;
		if (ist->pts >= f->recording_time + start_time) {
			close_output_stream(ost);
			return;
		}
	}

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
		opkt.dts = opkt.pts = av_rescale_delta(ist->st->time_base, pkt->dts,
			(AVRational) {
			1, ist->dec_ctx->sample_rate
		}, duration, &ist->filter_in_rescale_delta_last,
			ost->mux_timebase) - ost_tb_start_time;
	}

	opkt.duration = av_rescale_q(pkt->duration, ist->st->time_base, ost->mux_timebase);

	opkt.flags = pkt->flags;

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
			exit_program(octx, 1);
		}
		if (ret) {
			opkt.buf = av_buffer_create(opkt.data, opkt.size, av_buffer_default_free, NULL, 0);
			if (!opkt.buf)
				exit_program(octx, 1);
		}
	}
	else {
		opkt.data = pkt->data;
		opkt.size = pkt->size;
	}
	av_copy_packet_side_data(&opkt, pkt);

	output_packet(of, &opkt, ost, 0);
}

int guess_input_channel_layout(InputStream *ist)
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



static int ifilter_has_all_input_formats(FilterGraph *fg)
{
	int i;
	for (i = 0; i < fg->nb_inputs; i++) {
		if (fg->inputs[i]->format < 0 && (fg->inputs[i]->type == AVMEDIA_TYPE_AUDIO ||
			fg->inputs[i]->type == AVMEDIA_TYPE_VIDEO))
			return 0;
	}
	return 1;
}

static int ifilter_send_frame(InputFilter *ifilter, AVFrame *frame)
{

	FilterGraph *fg = ifilter->graph;
	WXCtx *octx = fg->octx;

	int need_reinit, ret, i;

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


	if (need_reinit || !fg->graph) {
		for (i = 0; i < fg->nb_inputs; i++) {
			if (!ifilter_has_all_input_formats(fg)) {
				AVFrame *tmp = av_frame_clone(frame);
				if (!tmp)return AVERROR(ENOMEM);
				av_frame_unref(frame);
				if (!av_fifo_space(ifilter->frame_queue)) {
					ret = av_fifo_realloc2(ifilter->frame_queue, 2 * av_fifo_size(ifilter->frame_queue));
				}
				av_fifo_generic_write(ifilter->frame_queue, &tmp, sizeof(tmp), NULL);
				return 0;
			}
		}

		ret = reap_filters(octx, 1);
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
		if (ret != AVERROR_EOF)
			av_log(NULL, AV_LOG_ERROR, "Error while filtering: %s\n", av_err2str(ret));
		return ret;
	}

	return 0;
}

static int ifilter_send_eof(InputFilter *ifilter, int64_t pts)
{
	int i, j, ret;

	ifilter->eof = 1;

	if (ifilter->filter) {
		ret = av_buffersrc_close(ifilter->filter, pts, AV_BUFFERSRC_FLAG_PUSH);
		if (ret < 0)
			return ret;
	}
	else {

		FilterGraph *fg = ifilter->graph;
		for (i = 0; i < fg->nb_inputs; i++)
			if (!fg->inputs[i]->eof)
				break;
		if (i == fg->nb_inputs) {



			for (j = 0; j < fg->nb_outputs; j++)
				finish_output_stream(fg->outputs[j]->ost);
		}
	}

	return 0;
}


static int DecodeFrame(AVCodecContext *avctx, AVFrame *frame, int *got_frame, AVPacket *pkt) {
	int ret;
	*got_frame = 0;
	if (pkt) {
		ret = avcodec_send_packet(avctx, pkt);
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

static int send_frame_to_filters(InputStream *ist, AVFrame *decoded_frame)
{
	int i, ret;
	AVFrame *f;

	av_assert1(ist->nb_filters > 0);
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
			ret = 0;
		if (ret < 0) {
			av_log(NULL, AV_LOG_ERROR,
				"Failed to inject frame into filter network: %s\n", av_err2str(ret));
			break;
		}
	}
	return ret;
}

static int decode_audio(InputStream *ist, AVPacket *pkt, int *got_output,
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

	ret = DecodeFrame(avctx, decoded_frame, got_output, pkt);
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




	ist->next_pts += ((int64_t)AV_TIME_BASE * decoded_frame->nb_samples) /
		avctx->sample_rate;
	ist->next_dts += ((int64_t)AV_TIME_BASE * decoded_frame->nb_samples) /
		avctx->sample_rate;


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
	if (decoded_frame->pts != AV_NOPTS_VALUE)
		decoded_frame->pts = av_rescale_delta(decoded_frame_tb, decoded_frame->pts,
		(AVRational) {
		1, avctx->sample_rate
	}, decoded_frame->nb_samples, &ist->filter_in_rescale_delta_last,
			(AVRational) {
		1, avctx->sample_rate
	});
	ist->nb_samples = decoded_frame->nb_samples;
	err = send_frame_to_filters(ist, decoded_frame);

	av_frame_unref(ist->filter_frame);
	av_frame_unref(decoded_frame);
	return err < 0 ? err : ret;
}

static int decode_video(InputStream *ist, AVPacket *pkt, int *got_output, int64_t *duration_pts, int eof,
	int *decode_failed)
{
	AVFrame *decoded_frame = NULL;
	int i, ret = 0, err = 0;
	int64_t best_effort_timestamp;
	int64_t dts = AV_NOPTS_VALUE;
	AVPacket avpkt;

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
		avpkt.dts = dts;
	}


	if (eof) {
		void *new = av_realloc_array(ist->dts_buffer, ist->nb_dts_buffer + 1, sizeof(ist->dts_buffer[0]));
		ist->dts_buffer = new;
		ist->dts_buffer[ist->nb_dts_buffer++] = dts;
	}

	ret = DecodeFrame(ist->dec_ctx, decoded_frame, got_output, pkt ? &avpkt : NULL);
	if (ret < 0)
		*decode_failed = 1;

	if (ist->st->codecpar->video_delay < ist->dec_ctx->has_b_frames) {
		if (ist->dec_ctx->codec_id == AV_CODEC_ID_H264) {
			ist->st->codecpar->video_delay = ist->dec_ctx->has_b_frames;
		}
		else
			av_log(ist->dec_ctx, AV_LOG_WARNING,
				"video_delay is larger in decoder than demuxer %d > %d.\n"
				"If you want to help, upload a sample "
				"of this file to ftp://upload.ffmpeg.org/incoming/ "
				"and contact the ffmpeg-devel mailing list. (ffmpeg-devel@ffmpeg.org)\n",
				ist->dec_ctx->has_b_frames,
				ist->st->codecpar->video_delay);
	}

	if (*got_output && ret >= 0) {
		if (ist->dec_ctx->width != decoded_frame->width ||
			ist->dec_ctx->height != decoded_frame->height ||
			ist->dec_ctx->pix_fmt != decoded_frame->format) {
			av_log(NULL, AV_LOG_DEBUG, "Frame parameters mismatch context %d,%d,%d != %d,%d,%d\n",
				decoded_frame->width,
				decoded_frame->height,
				decoded_frame->format,
				ist->dec_ctx->width,
				ist->dec_ctx->height,
				ist->dec_ctx->pix_fmt);
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
	ist->hwaccel_retrieved_pix_fmt = decoded_frame->format;

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

static int transcode_subtitles(InputStream *ist, AVPacket *pkt, int *got_output,
	int *decode_failed)
{
	WXCtx *octx = ist->octx;

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
				av_log(ist->dec_ctx, AV_LOG_DEBUG,
					"Subtitle duration reduced from %"PRId32" to %d%s\n",
					ist->prev_sub.subtitle.end_display_time, end,
					end <= 0 ? ", dropping it" : "");
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
			exit_program(octx, 1);
		if (!av_fifo_space(ist->sub2video.sub_queue)) {
			ret = av_fifo_realloc2(ist->sub2video.sub_queue, 2 * av_fifo_size(ist->sub2video.sub_queue));
		}
		av_fifo_generic_write(ist->sub2video.sub_queue, &subtitle, sizeof(subtitle), NULL);
		free_sub = 0;
	}

	if (!subtitle.num_rects)
		goto out;

	ist->frames_decoded++;

	for (i = 0; i < octx->nb_output_streams; i++) {
		OutputStream *ost = octx->output_streams[i];

		if (!check_output_constraints(ist, ost) || !ost->encoding_needed
			|| ost->enc->type != AVMEDIA_TYPE_SUBTITLE)
			continue;

		do_subtitle_out(octx->output_files[ost->file_index], ost, &subtitle);
	}

out:
	if (free_sub)
		avsubtitle_free(&subtitle);
	return ret;
}

static int send_filter_eof(InputStream *ist)
{
	int i, ret;

	int64_t pts = av_rescale_q_rnd(ist->pts, AV_TIME_BASE_Q, ist->st->time_base,
		AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX);

	for (i = 0; i < ist->nb_filters; i++) {
		ret = ifilter_send_eof(ist->filters[i], pts);
		if (ret < 0)
			return ret;
	}
	return 0;
}


static int process_input_packet(InputStream *ist, const AVPacket *pkt, int no_eof)
{
	WXCtx *octx = ist->octx;
	int ret = 0, i;
	int repeating = 0;
	int eof_reached = 0;

	AVPacket avpkt;
	if (!ist->saw_first_ts) {
		ist->dts = ist->st->avg_frame_rate.num ? -ist->dec_ctx->has_b_frames * AV_TIME_BASE / av_q2d(ist->st->avg_frame_rate) : 0;
		ist->pts = 0;
		if (pkt && pkt->pts != AV_NOPTS_VALUE && !ist->decoding_needed) {
			ist->dts += av_rescale_q(pkt->pts, ist->st->time_base, AV_TIME_BASE_Q);
			ist->pts = ist->dts;
		}
		ist->saw_first_ts = 1;
	}

	if (ist->next_dts == AV_NOPTS_VALUE)
		ist->next_dts = ist->dts;
	if (ist->next_pts == AV_NOPTS_VALUE)
		ist->next_pts = ist->pts;

	if (!pkt) {

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
			if (decode_failed) {
				av_log(NULL, AV_LOG_ERROR, "Error while decoding stream #%d:%d: %s\n",
					ist->file_index, ist->st->index, av_err2str(ret));
			}
			else {
				av_log(NULL, AV_LOG_FATAL, "Error while processing the decoded "
					"data for stream #%d:%d\n", ist->file_index, ist->st->index);
			}
			if (!decode_failed)
				exit_program(octx, 1);
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



	if (!pkt && ist->decoding_needed && eof_reached && !no_eof) {
		int ret = send_filter_eof(ist);
		if (ret < 0) {
			av_log(NULL, AV_LOG_FATAL, "Error marking filters as finished\n");
			exit_program(octx, 1);
		}
	}


	if (!ist->decoding_needed && pkt) {
		ist->dts = ist->next_dts;
		switch (ist->dec_ctx->codec_type) {
		case AVMEDIA_TYPE_AUDIO:
			ist->next_dts += ((int64_t)AV_TIME_BASE * ist->dec_ctx->frame_size) /
				ist->dec_ctx->sample_rate;
			break;
		case AVMEDIA_TYPE_VIDEO:
			if (ist->framerate.num) {

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
	for (i = 0; i < octx->nb_output_streams; i++) {
		OutputStream *ost = octx->output_streams[i];

		if (!check_output_constraints(ist, ost) || ost->encoding_needed)
			continue;

		do_streamcopy(ist, ost, pkt);
	}

	return !eof_reached;
}

static enum AVPixelFormat get_format(AVCodecContext *s, const enum AVPixelFormat *pix_fmts)
{
	InputStream *ist = s->opaque;
	const enum AVPixelFormat *p;
	int ret;

	for (p = pix_fmts; *p != AV_PIX_FMT_NONE; p++) {
		const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(*p);
		const AVCodecHWConfig *config = NULL;
		int i;

		if (!(desc->flags & AV_PIX_FMT_FLAG_HWACCEL))
			break;

		if (ist->hwaccel_id == HWACCEL_GENERIC ||
			ist->hwaccel_id == HWACCEL_AUTO) {
			for (i = 0;; i++) {
				config = avcodec_get_hw_config(s->codec, i);
				if (!config)
					break;
				if (!(config->methods &
					AV_CODEC_HW_CONFIG_METHOD_HW_DEVICE_CTX))
					continue;
				if (config->pix_fmt == *p)
					break;
			}
		}
		if (config) {
			if (config->device_type != ist->hwaccel_device_type) {

				continue;
			}

			ret = hwaccel_decode_init(s);
			if (ret < 0) {
				if (ist->hwaccel_id == HWACCEL_GENERIC) {
					av_log(NULL, AV_LOG_FATAL,
						"%s hwaccel requested for input stream #%d:%d, "
						"but cannot be initialized.\n",
						av_hwdevice_get_type_name(config->device_type),
						ist->file_index, ist->st->index);
					return AV_PIX_FMT_NONE;
				}
				continue;
			}
		}
		else {
			const HWAccel *hwaccel = NULL;
			int i;
			for (i = 0; hwaccels[i].name; i++) {
				if (hwaccels[i].pix_fmt == *p) {
					hwaccel = &hwaccels[i];
					break;
				}
			}
			if (!hwaccel) {

				continue;
			}
			if (hwaccel->id != ist->hwaccel_id) {

				continue;
			}

			ret = hwaccel->init(s);
			if (ret < 0) {
				av_log(NULL, AV_LOG_FATAL,
					"%s hwaccel requested for input stream #%d:%d, "
					"but cannot be initialized.\n", hwaccel->name,
					ist->file_index, ist->st->index);
				return AV_PIX_FMT_NONE;
			}
		}

		if (ist->hw_frames_ctx) {
			s->hw_frames_ctx = av_buffer_ref(ist->hw_frames_ctx);
			if (!s->hw_frames_ctx)
				return AV_PIX_FMT_NONE;
		}

		ist->hwaccel_pix_fmt = *p;
		break;
	}

	return *p;
}

static int get_buffer(AVCodecContext *s, AVFrame *frame, int flags)
{
	InputStream *ist = s->opaque;

	if (ist->hwaccel_get_buffer && frame->format == ist->hwaccel_pix_fmt)
		return ist->hwaccel_get_buffer(s, frame, flags);

	return avcodec_default_get_buffer2(s, frame, flags);
}

static int init_input_stream(WXCtx *octx, int ist_index, char *error, int error_len)
{
	int ret;
	InputStream *ist = octx->input_streams[ist_index];

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



		ist->dec_ctx->pkt_timebase = ist->st->time_base;

		if (!av_dict_get(ist->decoder_opts, "threads", NULL, 0))
			av_dict_set(&ist->decoder_opts, "threads", "auto", 0);

		if (ist->st->disposition & AV_DISPOSITION_ATTACHED_PIC)
			av_dict_set(&ist->decoder_opts, "threads", "1", 0);

		ret = hw_device_setup_for_decode(ist);
		if (ret < 0) {
			snprintf(error, error_len, "Device setup failed for "
				"decoder on input stream #%d:%d : %s",
				ist->file_index, ist->st->index, av_err2str(ret));
			return ret;
		}

		if ((ret = avcodec_open2(ist->dec_ctx, codec, &ist->decoder_opts)) < 0) {
			if (ret == AVERROR_EXPERIMENTAL)
				exit_program(octx, 1);

			snprintf(error, error_len,
				"Error while opening decoder for input stream "
				"#%d:%d : %s",
				ist->file_index, ist->st->index, av_err2str(ret));
			return ret;
		}
		assert_avoptions(octx, ist->decoder_opts);
	}

	ist->next_pts = AV_NOPTS_VALUE;
	ist->next_dts = AV_NOPTS_VALUE;

	return 0;
}

static InputStream *get_input_stream(OutputStream *ost)
{
	WXCtx *octx = ost->octx;
	if (ost->source_index >= 0)
		return octx->input_streams[ost->source_index];
	return NULL;
}

static int compare_int64(const void *a, const void *b)
{
	return FFDIFFSIGN(*(const int64_t *)a, *(const int64_t *)b);
}


static int check_init_output_file(OutputFile *of, int file_index)
{
	WXCtx *octx = of->octx;

	int ret, i;

	for (i = 0; i < of->ctx->nb_streams; i++) {
		OutputStream *ost = octx->output_streams[of->ost_index + i];
		if (!ost->initialized)
			return 0;
	}

	of->ctx->interrupt_callback = int_cb;

	ret = avformat_write_header(of->ctx, &of->opts);
	if (ret < 0) {
		return ret;
	}

	of->header_written = 1;

	av_dump_format(of->ctx, file_index, of->ctx->url, 1);

	for (i = 0; i < of->ctx->nb_streams; i++) {
		OutputStream *ost = octx->output_streams[of->ost_index + i];

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

static int init_output_bsfs(OutputStream *ost)
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

static int init_output_stream_streamcopy(OutputStream *ost)
{
	WXCtx *octx = ost->octx;
	OutputFile *of = octx->output_files[ost->file_index];
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

	ret = avformat_transfer_internal_stream_timing_info(of->ctx->oformat, ost->st, ist->st, -1);
	if (ret < 0)
		return ret;


	if (ost->st->time_base.num <= 0 || ost->st->time_base.den <= 0)
		ost->st->time_base = av_add_q(av_stream_get_codec_timebase(ost->st), (AVRational) { 0, 1 });


	if (ost->st->duration <= 0 && ist->st->duration > 0)
		ost->st->duration = av_rescale_q(ist->st->duration, ist->st->time_base, ost->st->time_base);


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

	switch (par_dst->codec_type) {
	case AVMEDIA_TYPE_AUDIO:
		if (octx->audio_volume != 256) {
			av_log(NULL, AV_LOG_FATAL, "-acodec copy and -vol are incompatible (frames are not decoded)\n");
			exit_program(octx, 1);
		}
		if ((par_dst->block_align == 1 || par_dst->block_align == 1152 || par_dst->block_align == 576) && par_dst->codec_id == AV_CODEC_ID_MP3)
			par_dst->block_align = 0;
		if (par_dst->codec_id == AV_CODEC_ID_AC3)
			par_dst->block_align = 0;
		break;
	case AVMEDIA_TYPE_VIDEO:
		if (ost->frame_aspect_ratio.num) {
			sar =
				av_mul_q(ost->frame_aspect_ratio,
				(AVRational) {
				par_dst->height, par_dst->width
			});
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

static void set_encoder_id(OutputFile *of, OutputStream *ost)
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
	encoder_string = av_mallocz(encoder_string_len);
	if (!(format_flags & AVFMT_FLAG_BITEXACT) && !(codec_flags & AV_CODEC_FLAG_BITEXACT))
		av_strlcpy(encoder_string, LIBAVCODEC_IDENT " ", encoder_string_len);
	else
		av_strlcpy(encoder_string, "Lavc ", encoder_string_len);
	av_strlcat(encoder_string, ost->enc->name, encoder_string_len);
	av_dict_set(&ost->st->metadata, "encoder", encoder_string,
		AV_DICT_DONT_STRDUP_VAL | AV_DICT_DONT_OVERWRITE);
}

static void parse_forced_key_frames(char *kf, OutputStream *ost,
	AVCodecContext *avctx)
{
	WXCtx *octx = ost->octx;
	char *p;
	int n = 1, i, size, index = 0;
	int64_t t, *pts;

	for (p = kf; *p; p++)
		if (*p == ',')
			n++;
	size = n;
	pts = av_malloc_array(size, sizeof(*pts));
	p = kf;
	for (i = 0; i < n; i++) {
		char *next = strchr(p, ',');

		if (next)
			*next++ = 0;

		if (!memcmp(p, "chapters", 8)) {

			AVFormatContext *avf = octx->output_files[ost->file_index]->ctx;
			int j;

			if (avf->nb_chapters > INT_MAX - size) {
				pts = av_realloc_f(pts, size += avf->nb_chapters - 1, sizeof(*pts));
			}
			t = p[8] ? parse_time_or_die(p + 8, 1) : 0;
			t = av_rescale_q(t, AV_TIME_BASE_Q, avctx->time_base);

			for (j = 0; j < avf->nb_chapters; j++) {
				AVChapter *c = avf->chapters[j];
				av_assert1(index < size);
				pts[index++] = av_rescale_q(c->start, c->time_base,
					avctx->time_base) + t;
			}
		}
		else {
			t = parse_time_or_die(p, 1);
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

static void init_encoder_time_base(OutputStream *ost, AVRational default_time_base)
{
	WXCtx *octx = ost->octx;
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
		oc = octx->output_files[ost->file_index]->ctx;
	}

	enc_ctx->time_base = default_time_base;
}

static int init_output_stream_encode(OutputStream *ost) {
	WXCtx *octx = ost->octx;

	InputStream *ist = get_input_stream(ost);
	AVCodecContext *enc_ctx = ost->enc_ctx;
	AVCodecContext *dec_ctx = NULL;
	AVFormatContext *oc = octx->output_files[ost->file_index]->ctx;
	int j, ret;

	set_encoder_id(octx->output_files[ost->file_index], ost);
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
			ost->frame_rate = (AVRational) { 25, 1 };
		}

		if (ost->enc->supported_framerates && !ost->force_fps) {
			int idx = av_find_nearest_q_idx(ost->frame_rate, ost->enc->supported_framerates);
			ost->frame_rate = ost->enc->supported_framerates[idx];
		}

		if (enc_ctx->codec_id == AV_CODEC_ID_MPEG4) {
			av_reduce(&ost->frame_rate.num, &ost->frame_rate.den,
				ost->frame_rate.num, ost->frame_rate.den, 65535);
		}
	}

	switch (enc_ctx->codec_type) {
	case AVMEDIA_TYPE_AUDIO:
		enc_ctx->sample_fmt = av_buffersink_get_format(ost->filter->filter);
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
		if (av_q2d(enc_ctx->time_base) < 0.001 && VSYNC_AUTO != VSYNC_PASSTHROUGH
			&& (VSYNC_AUTO == VSYNC_CFR || VSYNC_AUTO == VSYNC_VSCFR || (VSYNC_AUTO == VSYNC_AUTO && !(oc->oformat->flags & AVFMT_VARIABLE_FPS)))) {
			av_log(oc, AV_LOG_WARNING, "Frame rate very high for a muxer not efficiently supporting it.\n"
				"Please consider specifying a lower framerate, a different muxer or -vsync 2\n");
		}
		for (j = 0; j < ost->forced_kf_count; j++)
			ost->forced_kf_pts[j] = av_rescale_q(ost->forced_kf_pts[j],
				AV_TIME_BASE_Q,
				enc_ctx->time_base);

		enc_ctx->width = av_buffersink_get_w(ost->filter->filter);
		enc_ctx->height = av_buffersink_get_h(ost->filter->filter);
		enc_ctx->sample_aspect_ratio = ost->st->sample_aspect_ratio =
			ost->frame_aspect_ratio.num ?
			av_mul_q(ost->frame_aspect_ratio, (AVRational) { enc_ctx->height, enc_ctx->width }) :
			av_buffersink_get_sample_aspect_ratio(ost->filter->filter);

		enc_ctx->pix_fmt = av_buffersink_get_format(ost->filter->filter);
		if (dec_ctx)
			enc_ctx->bits_per_raw_sample = FFMIN(dec_ctx->bits_per_raw_sample,
				av_pix_fmt_desc_get(enc_ctx->pix_fmt)->comp[0].depth);

		enc_ctx->framerate = ost->frame_rate;

		ost->st->avg_frame_rate = ost->frame_rate;

		if (!dec_ctx ||
			enc_ctx->width != dec_ctx->width ||
			enc_ctx->height != dec_ctx->height ||
			enc_ctx->pix_fmt != dec_ctx->pix_fmt) {
			enc_ctx->bits_per_raw_sample = 0;
		}

		if (ost->forced_keyframes) {
			if (!strncmp(ost->forced_keyframes, "expr:", 5)) {
				ret = av_expr_parse(&ost->forced_keyframes_pexpr, ost->forced_keyframes + 5,
					forced_keyframes_const_names, NULL, NULL, NULL, NULL, 0, NULL);
				if (ret < 0) {
					return ret;
				}
				ost->forced_keyframes_expr_const_values[FKF_N] = 0;
				ost->forced_keyframes_expr_const_values[FKF_N_FORCED] = 0;
				ost->forced_keyframes_expr_const_values[FKF_PREV_FORCED_N] = NAN;
				ost->forced_keyframes_expr_const_values[FKF_PREV_FORCED_T] = NAN;

			}
			else if (strncmp(ost->forced_keyframes, "source", 6)) {
				parse_forced_key_frames(ost->forced_keyframes, ost, ost->enc_ctx);
			}
		}
		break;
	case AVMEDIA_TYPE_SUBTITLE:
		enc_ctx->time_base = AV_TIME_BASE_Q;
		if (!enc_ctx->width) {
			enc_ctx->width = octx->input_streams[ost->source_index]->st->codecpar->width;
			enc_ctx->height = octx->input_streams[ost->source_index]->st->codecpar->height;
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

static int init_output_stream(OutputStream *ost, char *error, int error_len)
{
	WXCtx *octx = ost->octx;
	int ret = 0;

	if (ost->encoding_needed) {
		AVCodec *codec = ost->enc;
		AVCodecContext *dec = NULL;
		InputStream *ist;

		ret = init_output_stream_encode(ost);
		if (ret < 0)
			return ret;

		if ((ist = get_input_stream(ost)))
			dec = ist->dec_ctx;
		if (dec && dec->subtitle_header) {
			ost->enc_ctx->subtitle_header = av_mallocz(dec->subtitle_header_size + 1);
			memcpy(ost->enc_ctx->subtitle_header, dec->subtitle_header, dec->subtitle_header_size);
			ost->enc_ctx->subtitle_header_size = dec->subtitle_header_size;
		}

		if (ost->enc->type == AVMEDIA_TYPE_AUDIO &&
			!codec->defaults &&
			!av_dict_get(ost->encoder_opts, "b", NULL, 0) &&
			!av_dict_get(ost->encoder_opts, "ab", NULL, 0))
			av_dict_set(&ost->encoder_opts, "b", "128000", 0);

		if (ost->filter && av_buffersink_get_hw_frames_ctx(ost->filter->filter) &&
			((AVHWFramesContext*)av_buffersink_get_hw_frames_ctx(ost->filter->filter)->data)->format ==
			av_buffersink_get_format(ost->filter->filter)) {
			ost->enc_ctx->hw_frames_ctx = av_buffer_ref(av_buffersink_get_hw_frames_ctx(ost->filter->filter));
			if (!ost->enc_ctx->hw_frames_ctx)
				return AVERROR(ENOMEM);
		}
		else {
			ret = hw_device_setup_for_encode(ost);
			if (ret < 0) {
				snprintf(error, error_len, "Device setup failed for "
					"encoder on output stream #%d:%d : %s",
					ost->file_index, ost->index, av_err2str(ret));
				return ret;
			}
		}

		////X264 编码器
		////优化编码速度
		if (stricmp(codec->name, "libx264") == 0) {

		}

		
		if ((ret = avcodec_open2(ost->enc_ctx, codec, &ost->encoder_opts)) < 0) {
			if (ret == AVERROR_EXPERIMENTAL)
				exit_program(octx, 1);
			snprintf(error, error_len,
				"Error while opening encoder for output stream #%d:%d - "
				"maybe incorrect parameters such as bit_rate, rate, width or height",
				ost->file_index, ost->index);
			return ret;
		}
		if (ost->enc->type == AVMEDIA_TYPE_AUDIO &&
			!(ost->enc->capabilities & AV_CODEC_CAP_VARIABLE_FRAME_SIZE))
			av_buffersink_set_frame_size(ost->filter->filter,
				ost->enc_ctx->frame_size);
		assert_avoptions(octx, ost->encoder_opts);
		if (ost->enc_ctx->bit_rate && ost->enc_ctx->bit_rate < 1000)
			av_log(NULL, AV_LOG_WARNING, "The bitrate parameter is set too low."
				" It takes bits/s as argument, not kbits/s\n");

		ret = avcodec_parameters_from_context(ost->st->codecpar, ost->enc_ctx);
		if (ret < 0) {
			av_log(NULL, AV_LOG_FATAL,
				"Error initializing the output stream codec context.\n");
			exit_program(octx, 1);
		}

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

		if (ist) {
			int i;
			for (i = 0; i < ist->st->nb_side_data; i++) {
				AVPacketSideData *sd = &ist->st->side_data[i];
				uint8_t *dst = av_stream_new_side_data(ost->st, sd->type, sd->size);
				if (!dst)
					return AVERROR(ENOMEM);
				memcpy(dst, sd->data, sd->size);
				if (ist->autorotate && sd->type == AV_PKT_DATA_DISPLAYMATRIX)
					av_display_rotation_set((uint32_t *)dst, 0);
			}
		}


		if (ost->st->time_base.num <= 0 || ost->st->time_base.den <= 0)
			ost->st->time_base = av_add_q(ost->enc_ctx->time_base, (AVRational) { 0, 1 });


		if (ost->st->duration <= 0 && ist && ist->st->duration > 0)
			ost->st->duration = av_rescale_q(ist->st->duration, ist->st->time_base, ost->st->time_base);

		ost->st->codec->codec = ost->enc_ctx->codec;
	}
	else if (ost->stream_copy) {
		ret = init_output_stream_streamcopy(ost);
		if (ret < 0)
			return ret;
		ret = avcodec_parameters_to_context(ost->parser_avctx, ost->st->codecpar);
		if (ret < 0)
			return ret;
	}

	if (ost->disposition) {
		static const AVOption opts[] = {
			{ "disposition" , NULL, 0, AV_OPT_TYPE_FLAGS,{ .i64 = 0 }, (double)INT64_MIN, (double)INT64_MAX,.unit = "flags" },
			{ "default" , NULL, 0, AV_OPT_TYPE_CONST,{ .i64 = AV_DISPOSITION_DEFAULT },.unit = "flags" },
			{ "dub" , NULL, 0, AV_OPT_TYPE_CONST,{ .i64 = AV_DISPOSITION_DUB },.unit = "flags" },
			{ "original" , NULL, 0, AV_OPT_TYPE_CONST,{ .i64 = AV_DISPOSITION_ORIGINAL },.unit = "flags" },
			{ "comment" , NULL, 0, AV_OPT_TYPE_CONST,{ .i64 = AV_DISPOSITION_COMMENT },.unit = "flags" },
			{ "lyrics" , NULL, 0, AV_OPT_TYPE_CONST,{ .i64 = AV_DISPOSITION_LYRICS },.unit = "flags" },
			{ "karaoke" , NULL, 0, AV_OPT_TYPE_CONST,{ .i64 = AV_DISPOSITION_KARAOKE },.unit = "flags" },
			{ "forced" , NULL, 0, AV_OPT_TYPE_CONST,{ .i64 = AV_DISPOSITION_FORCED },.unit = "flags" },
			{ "hearing_impaired" , NULL, 0, AV_OPT_TYPE_CONST,{ .i64 = AV_DISPOSITION_HEARING_IMPAIRED },.unit = "flags" },
			{ "visual_impaired" , NULL, 0, AV_OPT_TYPE_CONST,{ .i64 = AV_DISPOSITION_VISUAL_IMPAIRED },.unit = "flags" },
			{ "clean_effects" , NULL, 0, AV_OPT_TYPE_CONST,{ .i64 = AV_DISPOSITION_CLEAN_EFFECTS },.unit = "flags" },
			{ "captions" , NULL, 0, AV_OPT_TYPE_CONST,{ .i64 = AV_DISPOSITION_CAPTIONS },.unit = "flags" },
			{ "descriptions" , NULL, 0, AV_OPT_TYPE_CONST,{ .i64 = AV_DISPOSITION_DESCRIPTIONS },.unit = "flags" },
			{ "metadata" , NULL, 0, AV_OPT_TYPE_CONST,{ .i64 = AV_DISPOSITION_METADATA },.unit = "flags" },
			{ NULL },
		};
		static const AVClass _class = {
			.class_name = "",
			.item_name = av_default_item_name,
			.option = opts,
			.version = LIBAVUTIL_VERSION_INT,
		};
		const AVClass *pclass = &_class;

		ret = av_opt_eval_flags(&pclass, &opts[0], ost->disposition, &ost->st->disposition);
		if (ret < 0)
			return ret;
	}

	ret = init_output_bsfs(ost);
	if (ret < 0)
		return ret;

	ost->initialized = 1;

	ret = check_init_output_file(octx->output_files[ost->file_index], ost->file_index);
	if (ret < 0)
		return ret;

	return ret;
}

static void report_new_stream(WXCtx *octx, int input_index, AVPacket *pkt)
{
	InputFile *file = octx->input_files[input_index];
	AVStream *st = file->ctx->streams[pkt->stream_index];

	if (pkt->stream_index < file->nb_streams_warn)
		return;
	file->nb_streams_warn = pkt->stream_index + 1;
}

static int transcode_init(WXCtx *octx)
{
	int ret = 0, i, j, k;
	AVFormatContext *oc;
	OutputStream *ost;
	InputStream *ist;
	char error[1024] = { 0 };

	for (i = 0; i < octx->nb_filtergraphs; i++) {
		FilterGraph *fg = octx->filtergraphs[i];
		for (j = 0; j < fg->nb_outputs; j++) {
			OutputFilter *ofilter = fg->outputs[j];
			if (!ofilter->ost || ofilter->ost->source_index >= 0)
				continue;
			if (fg->nb_inputs != 1)
				continue;
			for (k = octx->nb_input_streams - 1; k >= 0; k--)
				if (fg->inputs[0]->ist == octx->input_streams[k])
					break;
			ofilter->ost->source_index = k;
		}
	}

	for (i = 0; i < octx->nb_input_files; i++) {
		InputFile *ifile = octx->input_files[i];
		if (ifile->rate_emu)
			for (j = 0; j < ifile->nb_streams; j++)
				octx->input_streams[j + ifile->ist_index]->start = av_gettime_relative();
	}

	for (i = 0; i < octx->nb_input_streams; i++)
		if ((ret = init_input_stream(octx, i, error, sizeof(error))) < 0) {
			for (i = 0; i < octx->nb_output_streams; i++) {
				ost = octx->output_streams[i];
				avcodec_close(ost->enc_ctx);
			}
			goto dump_format;
		}

	for (i = 0; i < octx->nb_output_streams; i++) {
		if (octx->output_streams[i]->filter)
			continue;
		ret = init_output_stream(octx->output_streams[i], error, sizeof(error));
		if (ret < 0)
			goto dump_format;
	}

	for (i = 0; i < octx->nb_input_files; i++) {
		InputFile *ifile = octx->input_files[i];
		for (j = 0; j < ifile->ctx->nb_programs; j++) {
			AVProgram *p = ifile->ctx->programs[j];
			int discard = AVDISCARD_ALL;

			for (k = 0; k < p->nb_stream_indexes; k++)
				if (!octx->input_streams[ifile->ist_index + p->stream_index[k]]->discard) {
					discard = AVDISCARD_DEFAULT;
					break;
				}
			p->discard = discard;
		}
	}

	for (i = 0; i < octx->nb_output_files; i++) {
		oc = octx->output_files[i]->ctx;
		if (oc->oformat->flags & AVFMT_NOSTREAMS && oc->nb_streams == 0) {
			ret = check_init_output_file(octx->output_files[i], i);
			if (ret < 0)
				goto dump_format;
		}
	}

dump_format:
	av_log(NULL, AV_LOG_INFO, "Stream mapping:\n");
	for (i = 0; i < octx->nb_input_streams; i++) {
		ist = octx->input_streams[i];

		for (j = 0; j < ist->nb_filters; j++) {
			if (!filtergraph_is_simple(ist->filters[j]->graph)) {
				av_log(NULL, AV_LOG_INFO, "  Stream #%d:%d (%s) -> %s",
					ist->file_index, ist->st->index, ist->dec ? ist->dec->name : "?",
					ist->filters[j]->name);
				if (octx->nb_filtergraphs > 1)
					av_log(NULL, AV_LOG_INFO, " (graph %d)", ist->filters[j]->graph->index);
				av_log(NULL, AV_LOG_INFO, "\n");
			}
		}
	}

	for (i = 0; i < octx->nb_output_streams; i++) {
		ost = octx->output_streams[i];

		if (ost->attachment_filename) {

			av_log(NULL, AV_LOG_INFO, "  File %s -> Stream #%d:%d\n",
				ost->attachment_filename, ost->file_index, ost->index);
			continue;
		}

		if (ost->filter && !filtergraph_is_simple(ost->filter->graph)) {

			av_log(NULL, AV_LOG_INFO, "  %s", ost->filter->name);
			if (octx->nb_filtergraphs > 1)
				av_log(NULL, AV_LOG_INFO, " (graph %d)", ost->filter->graph->index);

			av_log(NULL, AV_LOG_INFO, " -> Stream #%d:%d (%s)\n", ost->file_index,
				ost->index, ost->enc ? ost->enc->name : "?");
			continue;
		}

		av_log(NULL, AV_LOG_INFO, "  Stream #%d:%d -> #%d:%d",
			octx->input_streams[ost->source_index]->file_index,
			octx->input_streams[ost->source_index]->st->index,
			ost->file_index,
			ost->index);
		if (ost->sync_ist != octx->input_streams[ost->source_index])
			av_log(NULL, AV_LOG_INFO, " [sync #%d:%d]",
				ost->sync_ist->file_index,
				ost->sync_ist->st->index);
		if (ost->stream_copy)
			av_log(NULL, AV_LOG_INFO, " (copy)");
		else {
			const AVCodec *in_codec = octx->input_streams[ost->source_index]->dec;
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


static int need_output(WXCtx *octx) {
	int i;
	for (i = 0; i < octx->nb_output_streams; i++) {
		OutputStream *ost = octx->output_streams[i];
		OutputFile *of = octx->output_files[ost->file_index];
		AVFormatContext *os = octx->output_files[ost->file_index]->ctx;

		if (ost->finished ||
			(os->pb && avio_tell(os->pb) >= of->limit_filesize))
			continue;
		if (ost->frame_number >= ost->max_frames) {
			int j;
			for (j = 0; j < of->ctx->nb_streams; j++)
				close_output_stream(octx->output_streams[of->ost_index + j]);
			continue;
		}
		return 1;
	}
	return 0;
}

static OutputStream *choose_output(WXCtx *octx)
{
	int i;
	int64_t opts_min = INT64_MAX;
	OutputStream *ost_min = NULL;

	for (i = 0; i < octx->nb_output_streams; i++) {
		OutputStream *ost = octx->output_streams[i];
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

static int get_input_packet(InputFile *f, AVPacket *pkt)
{
	WXCtx *octx = f->octx;
	if (f->rate_emu) {
		int i;
		for (i = 0; i < f->nb_streams; i++) {
			InputStream *ist = octx->input_streams[f->ist_index + i];
			int64_t pts = av_rescale(ist->dts, 1000000, AV_TIME_BASE);
			int64_t now = av_gettime_relative() - ist->start;
			if (pts > now)
				return AVERROR(EAGAIN);
		}
	}
	return av_read_frame(f->ctx, pkt);
}

static int got_eagain(WXCtx *octx) {
	int i;
	for (i = 0; i < octx->nb_output_streams; i++)
		if (octx->output_streams[i]->unavailable)
			return 1;
	return 0;
}

static void reset_eagain(WXCtx *octx) {
	int i;
	for (i = 0; i < octx->nb_input_files; i++)
		octx->input_files[i]->eagain = 0;
	for (i = 0; i < octx->nb_output_streams; i++)
		octx->output_streams[i]->unavailable = 0;
}


static int seek_to_start(InputFile *ifile, AVFormatContext *is) {
	WXCtx *octx = ifile->octx;
	InputStream *ist;
	AVCodecContext *avctx;
	int i, ret, has_audio = 0;
	int64_t duration = 0;

	ret = av_seek_frame(is, -1, is->start_time, 0);
	if (ret < 0)
		return ret;

	for (i = 0; i < ifile->nb_streams; i++) {
		ist = octx->input_streams[ifile->ist_index + i];
		avctx = ist->dec_ctx;
		if (ist->decoding_needed) {
			process_input_packet(ist, NULL, 1);
			avcodec_flush_buffers(avctx);
		}
		if (avctx->codec_type == AVMEDIA_TYPE_AUDIO && ist->nb_samples)
			has_audio = 1;
	}

	for (i = 0; i < ifile->nb_streams; i++) {
		ist = octx->input_streams[ifile->ist_index + i];
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
		duration += ist->max_pts - ist->min_pts;
		ifile->time_base = duration_max(duration, &ifile->duration, ist->st->time_base,
			ifile->time_base);
	}
	if (ifile->loop > 0)
		ifile->loop--;
	return ret;
}

static int process_input(WXCtx *octx, int file_index) {
	InputFile *ifile = octx->input_files[file_index];
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
			ist = octx->input_streams[ifile->ist_index + i];
			if (ist->decoding_needed) {
				ret = process_input_packet(ist, NULL, 0);
				if (ret>0)
					return 0;
			}

			for (j = 0; j < octx->nb_output_streams; j++) {
				OutputStream *ost = octx->output_streams[j];

				if (ost->source_index == ifile->ist_index + i &&
					(ost->stream_copy || ost->enc->type == AVMEDIA_TYPE_SUBTITLE))
					finish_output_stream(ost);
			}
		}

		ifile->eof_reached = 1;
		return AVERROR(EAGAIN);
	}

	reset_eagain(octx);

	if (pkt.stream_index >= ifile->nb_streams) {
		report_new_stream(octx, file_index, &pkt);
		goto discard_packet;
	}

	ist = octx->input_streams[ifile->ist_index + pkt.stream_index];

	ist->data_size += pkt.size;
	ist->nb_packets++;

	if (ist->discard)
		goto discard_packet;

	if (!ist->wrap_correction_done && is->start_time != AV_NOPTS_VALUE && ist->st->pts_wrap_bits < 64) {
		int64_t stime, stime2;
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
				av_log(is, AV_LOG_VERBOSE, "Correcting start time by %"PRId64"\n", new_start_time - is->start_time);
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
				exit_program(octx, 1);

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

	pkt_dts = av_rescale_q_rnd(pkt.dts, ist->st->time_base, AV_TIME_BASE_Q, AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX);
	if ((ist->dec_ctx->codec_type == AVMEDIA_TYPE_VIDEO ||
		ist->dec_ctx->codec_type == AVMEDIA_TYPE_AUDIO) &&
		pkt_dts != AV_NOPTS_VALUE && ist->next_dts == AV_NOPTS_VALUE
		&& (is->iformat->flags & AVFMT_TS_DISCONT) && ifile->last_ts != AV_NOPTS_VALUE) {
		int64_t delta = pkt_dts - ifile->last_ts;
		if (delta < -1LL * 10 * AV_TIME_BASE ||
			delta > 1LL * 10 * AV_TIME_BASE) {
			ifile->ts_offset -= delta;
			av_log(NULL, AV_LOG_DEBUG,
				"Inter stream timestamp discontinuity %"PRId64", new offset= %"PRId64"\n",
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

	pkt_dts = av_rescale_q_rnd(pkt.dts, ist->st->time_base, AV_TIME_BASE_Q, AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX);
	if ((ist->dec_ctx->codec_type == AVMEDIA_TYPE_VIDEO ||
		ist->dec_ctx->codec_type == AVMEDIA_TYPE_AUDIO) &&
		pkt_dts != AV_NOPTS_VALUE && ist->next_dts != AV_NOPTS_VALUE) {
		int64_t delta = pkt_dts - ist->next_dts;
		if (is->iformat->flags & AVFMT_TS_DISCONT) {
			if (delta < -1LL * dummy *AV_TIME_BASE ||
				delta > 1LL * dummy *AV_TIME_BASE ||
				pkt_dts + AV_TIME_BASE / 10 < FFMAX(ist->pts, ist->dts)) {
				ifile->ts_offset -= delta;
				av_log(NULL, AV_LOG_DEBUG,
					"timestamp discontinuity %"PRId64", new offset= %"PRId64"\n",
					delta, ifile->ts_offset);
				pkt.dts -= av_rescale_q(delta, AV_TIME_BASE_Q, ist->st->time_base);
				if (pkt.pts != AV_NOPTS_VALUE)
					pkt.pts -= av_rescale_q(delta, AV_TIME_BASE_Q, ist->st->time_base);
			}
		}
		else {
			if (delta < -1LL * 3600 * 30 * AV_TIME_BASE ||
				delta > 1LL * 3600 * 30 * AV_TIME_BASE) {
				av_log(NULL, AV_LOG_WARNING, "DTS %"PRId64", next:%"PRId64" st:%d invalid dropping\n", pkt.dts, ist->next_dts, pkt.stream_index);
				pkt.dts = AV_NOPTS_VALUE;
			}
			if (pkt.pts != AV_NOPTS_VALUE) {
				int64_t pkt_pts = av_rescale_q(pkt.pts, ist->st->time_base, AV_TIME_BASE_Q);
				delta = pkt_pts - ist->next_dts;
				if (delta < -1LL * 3600 * 30 * AV_TIME_BASE ||
					delta > 1LL * 3600 * 30 * AV_TIME_BASE) {
					av_log(NULL, AV_LOG_WARNING, "PTS %"PRId64", next:%"PRId64" invalid dropping st:%d\n", pkt.pts, ist->next_dts, pkt.stream_index);
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

static int transcode_from_filter(FilterGraph *graph, InputStream **best_ist)
{
	WXCtx *octx = graph->octx;
	int i, ret;
	int nb_requests, nb_requests_max = 0;
	InputFilter *ifilter;
	InputStream *ist;

	*best_ist = NULL;
	ret = avfilter_graph_request_oldest(graph->graph);
	if (ret >= 0)
		return reap_filters(octx, 0);

	if (ret == AVERROR_EOF) {
		ret = reap_filters(octx, 1);
		for (i = 0; i < graph->nb_outputs; i++)
			close_output_stream(graph->outputs[i]->ost);
		return ret;
	}
	if (ret != AVERROR(EAGAIN))
		return ret;

	for (i = 0; i < graph->nb_inputs; i++) {
		ifilter = graph->inputs[i];
		ist = ifilter->ist;
		if (octx->input_files[ist->file_index]->eagain ||
			octx->input_files[ist->file_index]->eof_reached)
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

static int transcode_step(WXCtx *octx)
{
	OutputStream *ost;
	InputStream *ist = NULL;
	int ret;

	ost = choose_output(octx);
	if (!ost) {
		if (got_eagain(octx)) {
			reset_eagain(octx);
			return 0;
		}
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
				exit_program(octx, 1);
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
			if (!ifilter->ist->got_output && !octx->input_files[ifilter->ist->file_index]->eof_reached) {
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
		ist = octx->input_streams[ost->source_index];
	}

	ret = process_input(octx, ist->file_index);
	if (ret == AVERROR(EAGAIN)) {
		if (octx->input_files[ist->file_index]->eagain)
			ost->unavailable = 1;
		return 0;
	}

	if (ret < 0)
		return ret == AVERROR_EOF ? 0 : ret;

	return reap_filters(octx, 0);
}

static int transcode(WXCtx *octx)
{
	int ret, i;
	AVFormatContext *os;
	OutputStream *ost;
	InputStream *ist;

	int64_t total_packets_written = 0;

	ret = transcode_init(octx);
	if (ret < 0)
		goto fail;

	while (!octx->received_sigterm) {
		if (!need_output(octx)) {
			av_log(NULL, AV_LOG_VERBOSE, "No more output streams to write to, finishing.\n");
			break;
		}
		ret = transcode_step(octx);
		if (ret < 0 && ret != AVERROR_EOF) {
			char errbuf[128];
			av_strerror(ret, errbuf, sizeof(errbuf));
			av_log(NULL, AV_LOG_ERROR, "Error while filtering: %s\n", errbuf);
			break;
		}

		int i;
		/* dump report by using the output first video and audio streams */
		for (i = 0; i <octx->nb_output_streams; i++) {
			OutputStream *ost = octx->output_streams[i];
			if (av_stream_get_end_pts(ost->st) != AV_NOPTS_VALUE) {
				int64_t pts = INT64_MIN + 1;
				pts = FFMAX(pts, av_rescale_q(av_stream_get_end_pts(ost->st), ost->st->time_base, AV_TIME_BASE_Q));
				octx->avffmpeg_pts_curr = pts / 1000;//Curr Time 
			}
		}
	}

	for (i = 0; i < octx->nb_input_streams; i++) {
		ist = octx->input_streams[i];
		if (!octx->input_files[ist->file_index]->eof_reached) {
			process_input_packet(ist, NULL, 0);
		}
	}
	flush_encoders(octx);

	for (i = 0; i < octx->nb_output_files; i++) {
		os = octx->output_files[i]->ctx;
		if (!octx->output_files[i]->header_written) {
			continue;
		}
		if ((ret = av_write_trailer(os)) < 0) {
			av_log(NULL, AV_LOG_ERROR, "Error writing trailer of %s: %s\n", os->url, av_err2str(ret));
		}
	}

	for (i = 0; i < octx->nb_output_streams; i++) {
		ost = octx->output_streams[i];
		if (ost->encoding_needed) {
			av_freep(&ost->enc_ctx->stats_in);
		}
		total_packets_written += ost->packets_written;
	}


	for (i = 0; i < octx->nb_input_streams; i++) {
		ist = octx->input_streams[i];
		if (ist->decoding_needed) {
			avcodec_close(ist->dec_ctx);
			if (ist->hwaccel_uninit)
				ist->hwaccel_uninit(ist->dec_ctx);
		}
	}
	av_buffer_unref(&octx->hw_device_ctx);
	hw_device_free_all(octx);
	ret = 0;

fail:
	if (octx->output_streams) {
		for (i = 0; i < octx->nb_output_streams; i++) {
			ost = octx->output_streams[i];
			if (ost) {
				if (ost->logfile) {
					if (fclose(ost->logfile))
						av_log(NULL, AV_LOG_ERROR,
							"Error closing logfile, loss of information possible: %s\n",
							av_err2str(AVERROR(errno)));
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

WXDELOGO_CAPI int    avffmpeg_convert(WXCtx *octx, int argc, char **argv){

	octx->avffmpeg_state = FFMPEG_ERROR_PROCESS;//正在处理的状态
	if (setjmp(octx->avffmpeg_jmpbuf) != 0) {
		ffmpeg_cleanup(octx, 0);
        return  FFMPEG_ERROR_EXIT_ON_ERROR;
	}
	int ret = ffmpeg_parse_options(octx, argc, argv); //参数解析失败

	if (ret < 0)
		exit_program(octx, 1);

	if ((octx->nb_output_files <= 0 && octx->nb_input_files == 0) || octx->nb_output_files <= 0) {
		exit_program(octx, 1); //输入或者输出为0
	}
	int i;
	for (i = 0; i < octx->nb_input_files; i++) {
		if (octx->input_files[i]->ctx->duration > 0)
			octx->avffmpeg_pts_total = octx->input_files[0]->ctx->duration / 1000;//第一个文件，Merger时候使用叠加处理
	}

	if (transcode(octx) < 0) //转换失败
		exit_program(octx, 1);

	octx->avffmpeg_state = FFMPEG_ERROR_OK;//OK
	ffmpeg_cleanup(octx, 0);
	return 0;
}

// ---------------- hard ware  -----------------------
static HWDevice *hw_device_get_by_type(WXCtx *octx, enum AVHWDeviceType type) {
	HWDevice *found = NULL;
	int i;
	for (i = 0; i < octx->nb_hw_devices; i++) {
		if (octx->hw_devices[i]->type == type) {
			if (found)
				return NULL;
			found = octx->hw_devices[i];
		}
	}
	return found;
}

HWDevice *hw_device_get_by_name(WXCtx *octx, const char *name) {
	int i;
	for (i = 0; i < octx->nb_hw_devices; i++) {
		if (!strcmp(octx->hw_devices[i]->name, name))
			return octx->hw_devices[i];
	}
	return NULL;
}

static HWDevice *hw_device_add(WXCtx *octx) {
	int err = av_reallocp_array(&octx->hw_devices, octx->nb_hw_devices + 1, sizeof(*octx->hw_devices));
	octx->hw_devices[octx->nb_hw_devices] = av_mallocz(sizeof(HWDevice));
	return octx->hw_devices[octx->nb_hw_devices++];
}

static char *hw_device_default_name(WXCtx *octx, enum AVHWDeviceType type) {
	const char *type_name = av_hwdevice_get_type_name(type);
	char *name;
	size_t index_pos;
	int index, index_limit = 1000;
	index_pos = strlen(type_name);
	name = av_malloc(index_pos + 4);
	for (index = 0; index < index_limit; index++) {
		snprintf(name, index_pos + 4, "%s%d", type_name, index);
		if (!hw_device_get_by_name(octx, name))
			break;
	}
	if (index >= index_limit) {
		av_freep(&name);
		return NULL;
	}
	return name;
}

int hw_device_init_from_string(WXCtx *octx, const char *arg, HWDevice **dev_out) {
	AVDictionary *options = NULL;
	char *type_name = NULL, *name = NULL, *device = NULL;
	enum AVHWDeviceType type;
	HWDevice *dev, *src;
	AVBufferRef *device_ref = NULL;
	int err;
	const char *errmsg, *p, *q;
	size_t k;

	k = strcspn(arg, ":=@");
	p = arg + k;

	type_name = av_strndup(arg, k);
	if (!type_name) {
		err = AVERROR(ENOMEM);
		goto fail;
	}
	type = av_hwdevice_find_type_by_name(type_name);
	if (type == AV_HWDEVICE_TYPE_NONE) {
		errmsg = "unknown device type";
		goto invalid;
	}

	if (*p == '=') {
		k = strcspn(p + 1, ":@");

		name = av_strndup(p + 1, k);
		if (!name) {
			err = AVERROR(ENOMEM);
			goto fail;
		}
		if (hw_device_get_by_name(octx, name)) {
			errmsg = "named device already exists";
			goto invalid;
		}
		p += 1 + k;
	}
	else {
		name = hw_device_default_name(octx, type);
		if (!name) {
			err = AVERROR(ENOMEM);
			goto fail;
		}
	}

	if (!*p) {
		err = av_hwdevice_ctx_create(&device_ref, type, NULL, NULL, 0);
		if (err < 0)
			goto fail;
	}
	else if (*p == ':') {

		++p;
		q = strchr(p, ',');
		if (q) {
			device = av_strndup(p, q - p);
			if (!device) {
				err = AVERROR(ENOMEM);
				goto fail;
			}
			err = av_dict_parse_string(&options, q + 1, "=", ",", 0);
			if (err < 0) {
				errmsg = "failed to parse options";
				goto invalid;
			}
		}

		err = av_hwdevice_ctx_create(&device_ref, type,
			device ? device : p, options, 0);
		if (err < 0)
			goto fail;

	}
	else if (*p == '@') {
		src = hw_device_get_by_name(octx, p + 1);
		if (!src) {
			errmsg = "invalid source device name";
			goto invalid;
		}
		err = av_hwdevice_ctx_create_derived(&device_ref, type, src->device_ref, 0);
		if (err < 0)
			goto fail;
	}
	else {
		errmsg = "parse error";
		goto invalid;
	}

	dev = hw_device_add(octx);
	if (!dev) {
		err = AVERROR(ENOMEM);
		goto fail;
	}

	dev->name = name;
	dev->type = type;
	dev->device_ref = device_ref;

	if (dev_out)
		*dev_out = dev;

	name = NULL;
	err = 0;
done:
	av_freep(&type_name);
	av_freep(&name);
	av_freep(&device);
	av_dict_free(&options);
	return err;
invalid:
	av_log(NULL, AV_LOG_ERROR,
		"Invalid device specification \"%s\": %s\n", arg, errmsg);
	err = AVERROR(EINVAL);
	goto done;
fail:
	av_log(NULL, AV_LOG_ERROR,
		"Device creation failed: %d.\n", err);
	av_buffer_unref(&device_ref);
	goto done;
}

static int hw_device_init_from_type(WXCtx *octx, enum AVHWDeviceType type,
	const char *device,
	HWDevice **dev_out)
{
	AVBufferRef *device_ref = NULL;
	HWDevice *dev;
	char *name;
	int err;

	name = hw_device_default_name(octx, type);
	if (!name) {
		err = AVERROR(ENOMEM);
		goto fail;
	}

	err = av_hwdevice_ctx_create(&device_ref, type, device, NULL, 0);
	if (err < 0) {
		goto fail;
	}

	dev = hw_device_add(octx);
	if (!dev) {
		err = AVERROR(ENOMEM);
		goto fail;
	}

	dev->name = name;
	dev->type = type;
	dev->device_ref = device_ref;

	if (dev_out)
		*dev_out = dev;

	return 0;

fail:
	av_freep(&name);
	av_buffer_unref(&device_ref);
	return err;
}

void hw_device_free_all(WXCtx *octx)
{
	int i;
	for (i = 0; i < octx->nb_hw_devices; i++) {
		av_freep(&octx->hw_devices[i]->name);
		av_buffer_unref(&octx->hw_devices[i]->device_ref);
		av_freep(&octx->hw_devices[i]);
	}
	av_freep(&octx->hw_devices);
	octx->nb_hw_devices = 0;
}

static HWDevice *hw_device_match_by_codec(WXCtx *octx, const AVCodec *codec)
{
	const AVCodecHWConfig *config;
	HWDevice *dev;
	int i;
	for (i = 0;; i++) {
		config = avcodec_get_hw_config(codec, i);
		if (!config)
			return NULL;
		if (!(config->methods & AV_CODEC_HW_CONFIG_METHOD_HW_DEVICE_CTX))
			continue;
		dev = hw_device_get_by_type(octx, config->device_type);
		if (dev)
			return dev;
	}
}

int hw_device_setup_for_decode(InputStream *ist)
{
	const AVCodecHWConfig *config;
	enum AVHWDeviceType type;
	HWDevice *dev = NULL;
	int err, auto_device = 0;

	if (ist->hwaccel_device) {
		dev = hw_device_get_by_name(ist->octx, ist->hwaccel_device);
		if (!dev) {
			if (ist->hwaccel_id == HWACCEL_AUTO) {
				auto_device = 1;
			}
			else if (ist->hwaccel_id == HWACCEL_GENERIC) {
				type = ist->hwaccel_device_type;
				err = hw_device_init_from_type(ist->octx, type, ist->hwaccel_device,
					&dev);
			}
			else {


				return 0;
			}
		}
		else {
			if (ist->hwaccel_id == HWACCEL_AUTO) {
				ist->hwaccel_device_type = dev->type;
			}
			else if (ist->hwaccel_device_type != dev->type) {
				return AVERROR(EINVAL);
			}
		}
	}
	else {
		if (ist->hwaccel_id == HWACCEL_AUTO) {
			auto_device = 1;
		}
		else if (ist->hwaccel_id == HWACCEL_GENERIC) {
			type = ist->hwaccel_device_type;
			dev = hw_device_get_by_type(ist->octx, type);
			if (!dev)
				err = hw_device_init_from_type(ist->octx, type, NULL, &dev);
		}
		else {
			dev = hw_device_match_by_codec(ist->octx, ist->dec);
			if (!dev) {
				return 0;
			}
		}
	}

	if (auto_device) {
		int i;
		if (!avcodec_get_hw_config(ist->dec, 0)) {

			return 0;
		}
		for (i = 0; !dev; i++) {
			config = avcodec_get_hw_config(ist->dec, i);
			if (!config)
				break;
			type = config->device_type;
			dev = hw_device_get_by_type(ist->octx, type);
			if (dev) {
				av_log(ist->dec_ctx, AV_LOG_INFO, "Using auto "
					"hwaccel type %s with existing device %s.\n",
					av_hwdevice_get_type_name(type), dev->name);
			}
		}
		for (i = 0; !dev; i++) {
			config = avcodec_get_hw_config(ist->dec, i);
			if (!config)
				break;
			type = config->device_type;
			err = hw_device_init_from_type(ist->octx, type, ist->hwaccel_device, &dev);
			if (err < 0) {
				continue;
			}
		}
		if (dev) {
			ist->hwaccel_device_type = type;
		}
		else {
			return 0;
		}
	}

	if (!dev) {
		return err;
	}

	ist->dec_ctx->hw_device_ctx = av_buffer_ref(dev->device_ref);
	if (!ist->dec_ctx->hw_device_ctx)
		return AVERROR(ENOMEM);

	return 0;
}

int hw_device_setup_for_encode(OutputStream *ost)
{
	HWDevice *dev;

	dev = hw_device_match_by_codec(ost->octx, ost->enc);
	if (dev) {
		ost->enc_ctx->hw_device_ctx = av_buffer_ref(dev->device_ref);
		if (!ost->enc_ctx->hw_device_ctx)
			return AVERROR(ENOMEM);
		return 0;
	}
	else {

		return 0;
	}
}

//------------------------------------------------------------------------
static int hwaccel_retrieve_data(AVCodecContext *avctx, AVFrame *input)
{
	InputStream *ist = avctx->opaque;
	AVFrame *output = NULL;
	enum AVPixelFormat output_format = ist->hwaccel_output_format;
	int err;

	if (input->format == output_format) {
		return 0;
	}

	output = av_frame_alloc();
	output->format = output_format;

	err = av_hwframe_transfer_data(output, input, 0);
	if (err < 0) {
		goto fail;
	}

	err = av_frame_copy_props(output, input);
	if (err < 0) {
		av_frame_unref(output);
		goto fail;
	}
	av_frame_unref(input);
	av_frame_move_ref(input, output);
	av_frame_free(&output);
	return 0;
fail:
	av_frame_free(&output);
	return err;
}

int hwaccel_decode_init(AVCodecContext *avctx) {
	InputStream *ist = avctx->opaque;
	ist->hwaccel_retrieve_data = &hwaccel_retrieve_data;
	return 0;
}
