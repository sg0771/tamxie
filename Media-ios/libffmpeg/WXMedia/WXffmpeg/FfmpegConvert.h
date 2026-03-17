#ifndef WX_FFMPEGCONVERT_H
#define WX_FFMPEGCONVERT_H

#include <WXBase.h>
#ifdef _WIN32
#pragma comment(lib,"libffmpeg.lib")
#endif 

#ifdef _WIN32
#include "ffmpeg-config-win32.h"
#endif


#ifdef __APPLE__
#include "ffmpeg-config-mac.h"
#endif


#include <stdint.h>
#include <stdio.h>
#include <signal.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
//#include <ff_stdatomic.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
//#include <mfx/mfxvideo.h>

#if HAVE_IO_H
#ifdef _WIN32
	#include <io.h>
#else
	#include <sys/io.h>
#endif // _WIN32

#endif

#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#if HAVE_SYS_RESOURCE_H
#include <sys/time.h>
#include <sys/types.h>
#include <sys/resource.h>
#elif HAVE_GETPROCESSTIMES
#ifdef  _WIN32
#include <windows.h>
#endif //  _WIN32


#endif

#if HAVE_GETPROCESSMEMORYINFO
#ifdef _WIN32
	#include <psapi.h>
#endif // _WIN32


#endif

#if HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif

#if HAVE_TERMIOS_H
#include <fcntl.h>
#include <sys/ioctl.h>
#include <termios.h>
#elif HAVE_KBHIT
#ifdef _WIN32
	#include <conio.h>
#else
	#include <atomic>
#endif // _WIN32


#endif

#if HAVE_SYS_RESOURCE_H
#include <sys/resource.h>
#endif


#ifdef __cplusplus
extern "C" {
#endif
#include "libavformat/avformat.h"
#include "libavfilter/avfilter.h"
	//#include "libavdevice/avdevice.h"
	//#include "libavresample/avresample.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
	//#include "libpostproc/postprocess.h"
#include "libavutil/attributes.h"
#include "libavutil/avassert.h"
#include "libavutil/avstring.h"
#include "libavutil/bprint.h"
#include "libavutil/display.h"
#include "libavutil/mathematics.h"
#include "libavutil/imgutils.h"
#include "libavutil/parseutils.h"
#include "libavutil/pixdesc.h"
#include "libavutil/eval.h"
#include "libavutil/dict.h"
#include "libavutil/opt.h"
#include "libavutil/cpu.h"
#include "libavutil/ffversion.h"
#include "libavutil/version.h"
#include "libavfilter/avfilter.h"
#include "libavfilter/buffersink.h"
#include "libavfilter/buffersrc.h"
	//#include "libavresample/avresample.h"
#include "libavutil/avassert.h"
#include "libavutil/avstring.h"
#include "libavutil/bprint.h"
#include "libavutil/channel_layout.h"
#include "libavutil/display.h"
#include "libavutil/opt.h"
#include "libavutil/pixdesc.h"
#include "libavutil/pixfmt.h"
#include "libavutil/imgutils.h"
#include "libavutil/samplefmt.h"
#include "libavutil/avstring.h"
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libavfilter/avfilter.h"
#include "libavutil/avassert.h"
#include "libavutil/avstring.h"
#include "libavutil/avutil.h"
#include "libavutil/channel_layout.h"
#include "libavutil/intreadwrite.h"
#include "libavutil/fifo.h"
#include "libavutil/mathematics.h"
#include "libavutil/opt.h"
#include "libavutil/parseutils.h"
#include "libavutil/pixdesc.h"
#include "libavutil/pixfmt.h"
#include "libavutil/dict.h"
#include "libavutil/hwcontext.h"
	//#include "libavutil/hwcontext_qsv.h"
#include "libavutil/mem.h"
#include "libavutil/opt.h"
	//#include "libavcodec/qsv.h"
#include "libavutil/hwcontext.h"
#include "libavutil/pixdesc.h"
#include "libavcodec/avcodec.h"
#include "libavfilter/avfilter.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavformat/avformat.h"
	//#include "libavdevice/avdevice.h"
#include "libswresample/swresample.h"
#include "libavutil/opt.h"
#include "libavutil/channel_layout.h"
#include "libavutil/parseutils.h"
#include "libavutil/samplefmt.h"
#include "libavutil/fifo.h"
#include "libavutil/hwcontext.h"
#include "libavutil/intreadwrite.h"
#include "libavutil/dict.h"
#include "libavutil/display.h"
#include "libavutil/mathematics.h"
#include "libavutil/pixdesc.h"
#include "libavutil/avstring.h"
#include "libavutil/imgutils.h"
#include "libavutil/timestamp.h"
#include "libavutil/bprint.h"
#include "libavutil/libavutil_time.h"
#include "libavutil/threadmessage.h"
//#include "libavcodec/mathops.h"
#include "libavfilter/avfilter.h"
#include "libavfilter/buffersrc.h"
#include "libavfilter/buffersink.h"
#include "libavformat/avformat.h"
#include "libavformat/avio.h"
#include "libavcodec/avcodec.h"
#include "libavfilter/avfilter.h"
#include "libavutil/avutil.h"
#include "libavutil/dict.h"
#include "libavutil/eval.h"
#include "libavutil/fifo.h"
#include "libavutil/hwcontext.h"
#include "libavutil/pixfmt.h"
#include "libavutil/rational.h"
#include "libavutil/threadmessage.h"
#include "libswresample/swresample.h"
#ifdef __cplusplus
}
#endif

#define HAS_ARG    0x0001
#define OPT_BOOL   0x0002
#define OPT_EXPERT 0x0004
#define OPT_STRING 0x0008
#define OPT_VIDEO  0x0010
#define OPT_AUDIO  0x0020
#define OPT_INT    0x0080
#define OPT_FLOAT  0x0100
#define OPT_SUBTITLE 0x0200
#define OPT_INT64    0x0400
#define OPT_EXIT     0x0800
#define OPT_DATA     0x1000
#define OPT_PERFILE  0x2000 
#define OPT_OFFSET   0x4000  
#define OPT_SPEC     0x8000 
#define OPT_TIME     0x10000
#define OPT_DOUBLE   0x20000
#define OPT_INPUT    0x40000
#define OPT_OUTPUT   0x80000

#define VSYNC_AUTO       -1
#define VSYNC_PASSTHROUGH 0
#define VSYNC_CFR         1
#define VSYNC_VFR         2
#define VSYNC_VSCFR       0xfe
#define VSYNC_DROP        0xff
#define MAX_STREAMS 1024    /* arbitrary sanity check value */

#define GROUP_OUTFILE  0
#define GROUP_INFILE   1


#define DEFAULT_PASS_LOGFILENAME_PREFIX "ffmpeg2pass"

#define MATCH_PER_STREAM_OPT(name, type, outvar, fmtctx, st, output_type)\
{\
    int i, ret;\
    for (i = 0; i < o->nb_ ## name; i++) {\
        char *spec = (char *)o->name[i].specifier;\
        if ((ret = check_stream_specifier(fmtctx, st, (const char*)spec)) > 0)\
            outvar = (output_type)o->name[i].u.type;\
        else if (ret < 0)\
            exit_program(ret);\
    }\
}

#define MATCH_PER_TYPE_OPT(name, type, outvar, fmtctx, mediatype, output_type)\
{\
    int i;\
    for (i = 0; i < o->nb_ ## name; i++) {\
        char *spec = (char*)o->name[i].specifier;\
        if (!strcmp(spec, mediatype))\
            outvar = (output_type)o->name[i].u.type;\
    }\
}

enum forced_keyframes_const {
	FKF_N,
	FKF_N_FORCED,
	FKF_PREV_FORCED_N,
	FKF_PREV_FORCED_T,
	FKF_T,
	FKF_NB
};


#define DECODING_FOR_OST    1
#define DECODING_FOR_FILTER 2
#define OSTFinished      int
#define ENCODER_FINISHED 1
#define MUXER_FINISHED   2

#define GROW_ARRAY(array, nb_elems, array_type)  \
array = (array_type)grow_array(array, sizeof(*array), &nb_elems, nb_elems + 1)

#define GROW_ARRAY_STATIC(array, nb_elems, array_type)  \
array = (array_type)ctx->grow_array(array, sizeof(*array), &nb_elems, nb_elems + 1)

class FfmpegConvert;
struct OptionsContext;

enum HWAccelID {
	HWACCEL_NONE = 0,
	HWACCEL_AUTO,
	HWACCEL_GENERIC,
	HWACCEL_VIDEOTOOLBOX,
	HWACCEL_QSV,
	HWACCEL_CUVID,
};

typedef struct OptionGroupDef {
	const char *name;
	const char *sep;
	int flags;
} OptionGroupDef;


#undef  AV_TIME_BASE_Q
static AVRational AV_TIME_BASE_Q{ 1, AV_TIME_BASE };

static AVRational AV_TIME_BASE_1_1000{ 1, 1000 };
static AVRational AV_TIME_BASE_0_1{ 0, 1 };
static AVRational AV_TIME_BASE_1_1{ 1, 1 };
static AVRational AV_TIME_BASE_25_1{ 25,1 };

typedef struct SpecifierOpt {
	char *specifier;    /**< stream/chapter/program/... specifier */
	union {
		uint8_t *str;
		int        i;
		int64_t  i64;
		uint64_t ui64;
		float      f;
		double   dbl;
	} u;
} SpecifierOpt;

typedef struct OptionDef {
	const char *name;
	int flags;
	void *dst_ptr;
	int(*func_arg)(FfmpegConvert*, void *, const char *, const char *);
	size_t off;
	const char *help;
	const char *argname;
} OptionDef;

typedef struct Option {
	const OptionDef  *opt;
	const char       *key;
	const char       *val;
} Option;

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

typedef struct OptionParseContext {
	OptionGroup global_opts;
	OptionGroupList *groups;
	int           nb_groups;
	OptionGroup cur_group;
} OptionParseContext;


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


	FfmpegConvert *m_owner;
} OptionsContext;

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
} InputFilter;

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
} OutputFilter;

typedef struct FilterGraph {
	int            index;
	const char    *graph_desc;

	AVFilterGraph *graph;
	int reconfiguration;

	InputFilter   **inputs;
	int          nb_inputs;
	OutputFilter **outputs;
	int         nb_outputs;
} FilterGraph;

typedef struct InputStream {
	int file_index;
	AVStream *st;
	int discard;             /* true if stream data should be discarded */
	int user_set_discard;
	int decoding_needed;     /* non zero if the packets must be decoded in 'raw_fifo', see DECODING_FOR_* */
	AVCodecContext *dec_ctx;
	AVCodec *dec;
	AVFrame *decoded_frame;
	AVFrame *filter_frame;
	int64_t       start;
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
} InputStream;

typedef struct InputFile {
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

} InputFile;



typedef struct OutputStream {
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
} OutputStream;

typedef struct OutputFile {
	AVFormatContext *ctx;
	AVDictionary *opts;
	int ost_index;       /* index of the first stream in output_streams */
	int64_t recording_time;  ///< desired length of the resulting file in microseconds == AV_TIME_BASE units
	int64_t start_time;      ///< start time in microseconds == AV_TIME_BASE units
	uint64_t limit_filesize; /* filesize limit expressed in bytes */

	int shortest;

	int header_written;
} OutputFile;


const char *const forced_keyframes_const_names[] = {
	"n",
	"n_forced",
	"prev_forced_n",
	"prev_forced_t",
	"t",
	NULL
};


static const OptionGroupDef url_groups[2] = {
	{ "output url",  NULL, OPT_OUTPUT },
	{ "input url",   "i",  OPT_INPUT },
};

static int compare_codec_desc(const void *a, const void *b)
{
	const AVCodecDescriptor * const *da = (const AVCodecDescriptor * const *)a;
	const AVCodecDescriptor * const *db = (const AVCodecDescriptor * const *)b;

	return (*da)->type != (*db)->type ? FFDIFFSIGN((*da)->type, (*db)->type) :
		strcmp((*da)->name, (*db)->name);
}

static enum AVPixelFormat get_format(AVCodecContext *s, const enum AVPixelFormat *pix_fmts)
{
	InputStream *ist = (InputStream *)s->opaque;
	const enum AVPixelFormat *p;
	int ret;

	for (p = pix_fmts; *p != AV_PIX_FMT_NONE; p++) {
		const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(*p);
		const AVCodecHWConfig  *config = NULL;
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



static int compare_int64(const void *a, const void *b)
{
	return FFDIFFSIGN(*(const int64_t *)a, *(const int64_t *)b);
}

static int get_buffer(AVCodecContext *s, AVFrame *frame, int flags)
{
	InputStream *ist = (InputStream *)s->opaque;

	if (ist->hwaccel_get_buffer && frame->format == ist->hwaccel_pix_fmt)
		return ist->hwaccel_get_buffer(s, frame, flags);

	return avcodec_default_get_buffer2(s, frame, flags);
}

static  int decode_interrupt_cb(void *ctx)
{
	return 0;
}

static const AVIOInterruptCB int_cb = { decode_interrupt_cb, NULL };

static void log_callback_null(void *ptr, int level, const char *fmt, va_list vl)
{
}

static int dummy = 0;
static int show_help(FfmpegConvert* ctx, void *optctx, const char *opt, const char *arg)
{
	return 0;
}


static int opt_dummy(FfmpegConvert* ctx, void *optctx, const char *opt, const char *arg)
{
	return 0;
}


static const OptionGroupDef global_group = { "global" };




#endif