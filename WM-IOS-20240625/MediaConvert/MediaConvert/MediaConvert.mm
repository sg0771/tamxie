//#include "stdafx.h"

#include <stdint.h>
#include <string.h>
#include <thread>
#include <mutex>
#include "MediaConvert.h"

#ifdef __ANDROID__
#include <jni.h>
#include <android/log.h>  
#define   LOG_TAG  "WXMediaConvert"
#define   LOGE(...)   __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)  
#else
#define   LOGE(...)
#endif

/*
基于ffmpeg.exe 的C++封装
*/

#ifdef _WIN32
#include "ffmpeg-config.h"
#else
#include "config.h"
#endif

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#include <process.h>
#else
#define strnicmp strncasecmp
#define stricmp strcasecmp
#endif

#include <stdint.h>


extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavfilter/avfilter.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
#include <libavutil/fifo.h>
#include <libavutil/opt.h>
#include <libavfilter/buffersrc.h>
#include <libavfilter/buffersink.h>
#include <libavutil/display.h>
#include <libavutil/eval.h>
#include <libavutil/bprint.h>
#include <libavutil/pixdesc.h>
#include <libavutil/parseutils.h>
#include <libavutil/avassert.h>
#include <libavutil/intreadwrite.h>
#include <libavutil/time.h>
#include <libavutil/ffversion.h>
#include <libavutil/timestamp.h>
#include <libavutil/hwcontext.h>
}

extern "C" void ms_log_callback(void* handle, int level, const char* fmt, va_list vl) {
	//printf("AV Log\n");
}

#define WXLocker    std::recursive_mutex
#define WXAutoLock  std::lock_guard<WXLocker>

class FfmpegExe {
public:
	char    m_szMsg[4096];
	void Log(const char* format, ...) {
		memset(m_szMsg, 0, 4096);
#ifdef _WIN32
		va_list marker = nullptr;
#else
		va_list marker;
#endif
		va_start(marker, format);
		vsprintf(m_szMsg, format, marker);
		va_end(marker);
        //printf(m_szMsg);
		//ms_log_callback(NULL, AV_LOG_ERROR, "-- %s -- ", m_szMsg);
	}

	int MidPred(int a, int b, int c) {
		if (a > b) {
			if (c > b) {
				if (c > a) b = a;
				else    b = c;
			}
		}
		else {
			if (b > c) {
				if (c > a) b = c;
				else    b = a;
			}
		}
		return b;
	}
public:
	const char* DEFAULT_PASS_LOGFILENAME_PREFIX = "ffmpeg2pass";
	class  OptionsContext;
	struct InputStream;
	struct FilterGraph;
	struct OutputFilter;
	struct OutputStream;
	enum {
		DECODING_FOR_OST = 1,
		DECODING_FOR_FILTER = 2,
		ABORT_ON_FLAG_EMPTY_OUTPUT = (1 << 0),
		ENCODER_FINISHED = 1,
		MUXER_FINISHED = 2,
		GROUP_OUTFILE = 0,
		GROUP_INFILE = 1,
		VSYNC_AUTO = -1,
		VSYNC_PASSTHROUGH = 0,
		VSYNC_CFR = 1,
		VSYNC_VFR = 2,
		VSYNC_VSCFR = 0xfe,
		VSYNC_DROP = 0xff,
		MAX_STREAMS = 1024,    /* arbitrary sanity check value */
		HAS_ARG = 0x0001,
		OPT_BOOL = 0x0002,
		OPT_EXPERT = 0x0004,
		OPT_STRING = 0x0008,
		OPT_VIDEO = 0x0010,
		OPT_AUDIO = 0x0020,
		OPT_INT = 0x0080,
		OPT_FLOAT = 0x0100,
		OPT_SUBTITLE = 0x0200,
		OPT_INT64 = 0x0400,
		OPT_EXIT = 0x0800,
		OPT_DATA = 0x1000,
		OPT_PERFILE = 0x2000,
		OPT_OFFSET = 0x4000,
		OPT_SPEC = 0x8000,
		OPT_TIME = 0x10000,
		OPT_DOUBLE = 0x20000,
		OPT_INPUT = 0x40000,
		OPT_OUTPUT = 0x80000,
	};
	enum VideoTarget {
		PAL,
		NTSC,
		FILM,
		UNKNOWN
	};
	enum forced_keyframes_const {
		FKF_N,
		FKF_N_FORCED,
		FKF_PREV_FORCED_N,
		FKF_PREV_FORCED_T,
		FKF_T,
		FKF_NB
	};
	enum HWAccelID {
		HWACCEL_NONE = 0,
		HWACCEL_AUTO,
		HWACCEL_GENERIC,
		HWACCEL_VIDEOTOOLBOX,
		HWACCEL_QSV,
		HWACCEL_CUVID,
	};
	struct HWAccel {
		const char* name;
		int(*init)(AVCodecContext* s);
		enum HWAccelID id;
		enum AVPixelFormat pix_fmt;
	};
	const HWAccel hwaccels[3] = {
		{ 0 },
	};
	struct HWDevice {
		char* name = NULL;
		enum AVHWDeviceType type = AV_HWDEVICE_TYPE_NONE;
		AVBufferRef* device_ref = NULL;
	};
	struct StreamMap {
		int disabled = 0;
		int file_index = 0;
		int stream_index = 0;
		int sync_file_index = 0;
		int sync_stream_index = 0;
		char* linklabel = NULL;
	};
	struct AudioChannelMap {
		int file_idx = 0;
		int stream_idx = 0;
		int  channel_idx = 0;
		int ofile_idx = 0;
		int ostream_idx = 0;
	};
	struct SpecifierOpt {
		char* specifier = NULL;
		union {
			uint8_t* str = NULL;
			int i;
			int64_t i64;
			uint64_t ui64;
			float f;
			double dbl;
		} u;
	};
	typedef int(*FuncArg)(FfmpegExe*, OptionsContext*, const char*, const char*);
	struct OptionDef {
		const char* name;
		int flags;
		union {
			void* dst_ptr;
			FuncArg func_arg;
			size_t off;
		} u;
		const char* help;
		const char* argname;
	};
	struct Option {
		const OptionDef* opt = NULL;
		const char* key = NULL;
		const char* val = NULL;
	};
	struct OptionGroupDef {
		const char* name;
		const char* sep;
		int flags;
	};
	const OptionGroupDef global_group = { "global" };
	const OptionGroupDef OptGroup_groups[2] = {
		{ "output url", NULL, OPT_OUTPUT },
		{ "input url", "i", OPT_INPUT },
	};
	struct OptionGroup {
		const OptionGroupDef* group_def = NULL;
		const char* arg = NULL;
		Option* opts = NULL;
		int nb_opts = 0;
		AVDictionary* codec_opts = NULL;
		AVDictionary* format_opts = NULL;
		AVDictionary* resample_opts = NULL;
		AVDictionary* sws_dict = NULL;
		AVDictionary* swr_opts = NULL;
	};
	struct OptionGroupList {
		const OptionGroupDef* group_def = NULL;
		OptionGroup* groups = NULL;
		int nb_groups = 0;
	};
	struct OptionParseContext {
		OptionGroup global_opts;
		OptionGroupList* groups = NULL;
		int nb_groups = 0;
		OptionGroup cur_group;
	};
	struct InputFilter {
		AVFilterContext* filter = NULL;
		InputStream* ist = NULL;
		FilterGraph* graph = NULL;
		uint8_t* name = NULL;
		enum AVMediaType type = AVMEDIA_TYPE_UNKNOWN;
		AVFifoBuffer* frame_queue = NULL;
		int format = 0;
		int width = 0;
		int height = 0;
		AVRational sample_aspect_ratio;
		int sample_rate = 0;
		int channels = 0;
		uint64_t channel_layout = 0;
		AVBufferRef* hw_frames_ctx = NULL;
		int eof = 0;
	};
	struct FilterGraph {
		int index = 0;
		const char* graph_desc = NULL;
		AVFilterGraph* graph = NULL;
		int reconfiguration = 0;
		InputFilter** inputs = NULL;
		int nb_inputs = 0;
		OutputFilter** outputs = NULL;
		int nb_outputs = 0;
	};
	struct InputStream {
		FfmpegExe* owner = NULL;
		int file_index = 0;
		AVStream* st = NULL;
		int discard = 0;
		int user_set_discard = 0;
		int decoding_needed = 0;
		AVCodecContext* dec_ctx = NULL;
		AVCodec* dec = NULL;
		AVFrame* decoded_frame = NULL;
		AVFrame* filter_frame = NULL;
		int64_t start = 0;
		int64_t next_dts = 0;
		int64_t dts = 0;
		int64_t next_pts = 0;
		int64_t pts = 0;
		int wrap_correction_done = 0;
		int64_t filter_in_rescale_delta_last = 0;
		int64_t min_pts = 0;
		int64_t max_pts = 0;
		int64_t cfr_next_pts = 0;
		int64_t nb_samples = 0;
		double ts_scale = 0;
		int saw_first_ts = 0;
		AVDictionary* decoder_opts = NULL;
		AVRational framerate;
		int top_field_first = 0;
		int guess_layout_max = 0;
		int autorotate = 0;
		int fix_sub_duration = 0;
		struct {
			int got_output = 0;
			int ret = 0;
			AVSubtitle subtitle;
		} prev_sub;
		struct sub2video {
			int64_t last_pts = 0;
			int64_t end_pts = 0;
			AVFifoBuffer* sub_queue = NULL;
			AVFrame* frame = NULL;
			int w = 0;
			int h = 0;
		} sub2video;
		int dr1 = 0;
		InputFilter** filters = NULL;
		int nb_filters = 0;
		int reinit_filters = 0;
		enum HWAccelID hwaccel_id = HWACCEL_NONE;
		enum AVHWDeviceType hwaccel_device_type = AV_HWDEVICE_TYPE_NONE;
		char* hwaccel_device = NULL;
		enum AVPixelFormat hwaccel_output_format = AV_PIX_FMT_NONE;
		void* hwaccel_ctx = NULL;
		void(*hwaccel_uninit)(AVCodecContext* s) = NULL;
		int(*hwaccel_get_buffer)(AVCodecContext* s, AVFrame* frame, int flags) = NULL;
		int(*hwaccel_retrieve_data)(AVCodecContext* s, AVFrame* frame) = NULL;
		enum AVPixelFormat hwaccel_pix_fmt;
		enum AVPixelFormat hwaccel_retrieved_pix_fmt;
		AVBufferRef* hw_frames_ctx = NULL;
		uint64_t data_size = 0;
		uint64_t nb_packets = 0;
		uint64_t frames_decoded = 0;
		uint64_t samples_decoded = 0;
		int64_t* dts_buffer = NULL;
		int nb_dts_buffer = 0;
		int got_output = 0;
	};
	struct InputFile {
		AVFormatContext* ctx = NULL;
		int eof_reached = 0;
		int eagain = 0;
		int ist_index = 0;
		int loop = 0;
		int64_t duration = 0;
		AVRational time_base;
		int64_t input_ts_offset = 0;
		int64_t ts_offset = 0;
		int64_t last_ts = 0;
		int64_t start_time = AV_NOPTS_VALUE;
		int seek_timestamp = 0;
		int64_t recording_time = INT64_MAX;
		int nb_streams = 0;
		int nb_streams_warn = 0;
		int rate_emu = 0;
		int accurate_seek = 1;
	};
	struct OutputFilter {
		AVFilterContext* filter = NULL;
		OutputStream* ost = NULL;
		FilterGraph* graph = NULL;
		uint8_t* name = NULL;
		AVFilterInOut* out_tmp = NULL;
		enum AVMediaType type;
		int width = 0;
		int height = 0;
		AVRational frame_rate;
		int format = 0;
		int sample_rate = 0;
		uint64_t channel_layout = 0;
		int* formats = NULL;
		uint64_t* channel_layouts = NULL;
		int* sample_rates = NULL;
	};
	struct OutputStream {
		int file_index = 0;
		int index = 0;
		int source_index = 0;
		AVStream* st = NULL;
		int encoding_needed = 0;
		int frame_number = 0;
		InputStream* sync_ist = NULL;
		int64_t sync_opts = 0;
		int64_t first_pts = 0;
		int64_t last_mux_dts = 0;
		AVRational mux_timebase;
		AVRational enc_timebase;
		int nb_bitstream_filters = 0;
		AVBSFContext** bsf_ctx = NULL;
		AVCodecContext* enc_ctx = NULL;
		AVCodecParameters* ref_par = NULL;
		AVCodec* enc = NULL;
		int64_t max_frames = 0;
		AVFrame* filtered_frame = NULL;
		AVFrame* last_frame = NULL;
		int last_dropped = 0;
		int last_nb0_frames[3] = { 0,0,0 };
		void* hwaccel_ctx = NULL;
		AVRational frame_rate;
		int is_cfr = 0;
		int force_fps = 0;
		int top_field_first = 0;
		int rotate_overridden = 0;
		double rotate_override_value = 0;
		AVRational frame_aspect_ratio;
		int64_t forced_kf_ref_pts = 0;
		int64_t* forced_kf_pts;
		int forced_kf_count = 0;
		int forced_kf_index = 0;
		char* forced_keyframes = NULL;
		AVExpr* forced_keyframes_pexpr = NULL;
		double forced_keyframes_expr_const_values[FKF_NB] = { 0 };
		int* audio_channels_map = NULL;
		int audio_channels_mapped = 0;
		char* logfile_prefix = NULL;
		FILE* logfile = NULL;
		OutputFilter* filter = NULL;
		char* avfilter = NULL;
		char* filters = NULL;
		char* filters_script = NULL;
		AVDictionary* encoder_opts = NULL;
		AVDictionary* sws_dict = NULL;
		AVDictionary* swr_opts = NULL;
		AVDictionary* resample_opts = NULL;
		char* apad = NULL;
		int finished = 0;
		int unavailable = 0;
		int stream_copy = 0;
		int initialized = 0;
		int inputs_done = 0;
		const char* attachment_filename = NULL;
		int copy_initial_nonkeyframes = 0;
		int copy_prior_start = 0;
		char* disposition = NULL;
		int keep_pix_fmt = 0;
		uint64_t data_size = 0;
		uint64_t packets_written = 0;
		uint64_t frames_encoded = 0;
		uint64_t samples_encoded = 0;
		int quality = 0;
		int max_muxing_queue_size = 0;
		AVFifoBuffer* muxing_queue = NULL;
		int pict_type = 0;
		int64_t error[4] = { 0 };
	};

	struct OutputFile {
		AVFormatContext* ctx = NULL;
		AVDictionary* opts = NULL;
		int ost_index = 0;
		int64_t recording_time = INT64_MAX;
		int64_t start_time = AV_NOPTS_VALUE;
		uint64_t limit_filesize = UINT64_MAX;
		int shortest = 0;
		int header_written = 0;
	};

	class OptionsContext {
	public:
		FfmpegExe* owner = NULL;
		OptionGroup* g = NULL;
		int64_t start_time = AV_NOPTS_VALUE;
		int64_t start_time_eof = AV_NOPTS_VALUE;
		int seek_timestamp = 0;
		const char* format = NULL;
		SpecifierOpt* codec_names = NULL;
		int nb_codec_names = 0;
		SpecifierOpt* audio_channels = NULL;
		int nb_audio_channels = 0;
		SpecifierOpt* audio_sample_rate = NULL;
		int nb_audio_sample_rate = 0;
		SpecifierOpt* frame_rates = NULL;
		int nb_frame_rates = 0;
		SpecifierOpt* frame_sizes = NULL;
		int nb_frame_sizes = 0;
		SpecifierOpt* frame_pix_fmts = NULL;
		int nb_frame_pix_fmts = 0;
		int64_t input_ts_offset = 0;
		int loop = 0;
		int rate_emu = 0;
		int accurate_seek = 1;
		int thread_queue_size = 0;
		SpecifierOpt* ts_scale = NULL;
		int nb_ts_scale = 0;
		SpecifierOpt* dump_attachment = NULL;
		int nb_dump_attachment = 0;
		SpecifierOpt* hwaccels = NULL;
		int nb_hwaccels = 0;
		SpecifierOpt* hwaccel_devices = NULL;
		int nb_hwaccel_devices = 0;
		SpecifierOpt* hwaccel_output_formats = NULL;
		int nb_hwaccel_output_formats = 0;
		SpecifierOpt* autorotate = NULL;
		int nb_autorotate = 0;
		StreamMap* stream_maps = NULL;
		int nb_stream_maps = 0;
		AudioChannelMap* audio_channel_maps = NULL;
		int nb_audio_channel_maps = 0;
		int metadata_global_manual = 0;
		int metadata_streams_manual = 0;
		int metadata_chapters_manual = 0;
		const char** attachments = NULL;
		int nb_attachments = 0;
		int chapters_input_file = INT_MAX;
		int64_t recording_time = INT64_MAX;
		int64_t stop_time = INT64_MAX;
		uint64_t limit_filesize = UINT64_MAX;
		float mux_preload = 0;
		float mux_max_delay = 0.7;
		int shortest = 0;
		int bitexact = 0;
		int video_disable = 0;
		int audio_disable = 0;
		int subtitle_disable = 0;
		int data_disable = 0;
		int* streamid_map = NULL;
		int nb_streamid_map = 0;
		SpecifierOpt* metadata = NULL;
		int nb_metadata = 0;
		SpecifierOpt* max_frames = NULL;
		int nb_max_frames = 0;
		SpecifierOpt* bitstream_filters = NULL;
		int nb_bitstream_filters = 0;
		SpecifierOpt* codec_tags = NULL;
		int nb_codec_tags = 0;
		SpecifierOpt* sample_fmts = NULL;
		int nb_sample_fmts = 0;
		SpecifierOpt* qscale = NULL;
		int nb_qscale = 0;
		SpecifierOpt* forced_key_frames = NULL;
		int nb_forced_key_frames = 0;
		SpecifierOpt* force_fps = NULL;
		int nb_force_fps = 0;
		SpecifierOpt* frame_aspect_ratios = NULL;
		int nb_frame_aspect_ratios = 0;
		SpecifierOpt* rc_overrides = NULL;
		int nb_rc_overrides = 0;
		SpecifierOpt* intra_matrices = NULL;
		int nb_intra_matrices = 0;
		SpecifierOpt* inter_matrices = NULL;
		int nb_inter_matrices = 0;
		SpecifierOpt* chroma_intra_matrices = NULL;
		int nb_chroma_intra_matrices = 0;
		SpecifierOpt* top_field_first = NULL;
		int nb_top_field_first = 0;
		SpecifierOpt* metadata_map = NULL;
		int nb_metadata_map = 0;
		SpecifierOpt* presets = NULL;
		int nb_presets = 0;
		SpecifierOpt* copy_initial_nonkeyframes = NULL;
		int nb_copy_initial_nonkeyframes = 0;
		SpecifierOpt* copy_prior_start = NULL;
		int nb_copy_prior_start = 0;
		SpecifierOpt* filters = NULL;
		int nb_filters = 0;
		SpecifierOpt* filter_scripts = NULL;
		int nb_filter_scripts = 0;
		SpecifierOpt* reinit_filters = NULL;
		int nb_reinit_filters = 0;
		SpecifierOpt* fix_sub_duration = NULL;
		int nb_fix_sub_duration = 0;
		SpecifierOpt* canvas_sizes = NULL;
		int nb_canvas_sizes = 0;
		SpecifierOpt* pass = NULL;
		int nb_pass = 0;
		SpecifierOpt* passlogfiles = NULL;
		int nb_passlogfiles = 0;
		SpecifierOpt* max_muxing_queue_size = NULL;
		int nb_max_muxing_queue_size = 0;
		SpecifierOpt* guess_layout_max = NULL;
		int nb_guess_layout_max = 0;
		SpecifierOpt* apad = NULL;
		int nb_apad = 0;
		SpecifierOpt* discard = NULL;
		int nb_discard = 0;
		SpecifierOpt* disposition = NULL;
		int nb_disposition = 0;
		SpecifierOpt* program = NULL;
		int nb_program = 0;
		SpecifierOpt* time_bases = NULL;
		int nb_time_bases = 0;
		SpecifierOpt* enc_time_bases = NULL;
		int nb_enc_time_bases = 0;
	};
	const AVOption opt_abort_on_opts[3] = {
		{ "abort_on" , NULL, 0, AV_OPT_TYPE_FLAGS,{ 0 }, (double)INT64_MIN, (double)INT64_MAX,0, "flags" },
		{ "empty_output" , NULL, 0, AV_OPT_TYPE_CONST,{ ABORT_ON_FLAG_EMPTY_OUTPUT },0,0,0, "flags" },
		{ NULL },
	};
	const AVClass opt_abort_on_class = {
		"",
		av_default_item_name,
		opt_abort_on_opts,
		LIBAVUTIL_VERSION_INT,
	};
	const AVOption init_output_stream_opts[16] = {
		{ "default" , NULL, 0, AV_OPT_TYPE_CONST,{ AV_DISPOSITION_DEFAULT },0,0,0 , "flags" },
		{ "dub" , NULL, 0, AV_OPT_TYPE_CONST,{ AV_DISPOSITION_DUB },0,0,0 , "flags" },
		{ "original" , NULL, 0, AV_OPT_TYPE_CONST,{ AV_DISPOSITION_ORIGINAL },0,0,0 , "flags" },
		{ "comment" , NULL, 0, AV_OPT_TYPE_CONST,{ AV_DISPOSITION_COMMENT },0,0,0 , "flags" },
		{ "lyrics" , NULL, 0, AV_OPT_TYPE_CONST,{ AV_DISPOSITION_LYRICS },0,0,0 , "flags" },
		{ "karaoke" , NULL, 0, AV_OPT_TYPE_CONST,{ AV_DISPOSITION_KARAOKE },0,0,0 , "flags" },
		{ "forced" , NULL, 0, AV_OPT_TYPE_CONST,{ AV_DISPOSITION_FORCED },0,0,0 , "flags" },
		{ "hearing_impaired" , NULL, 0, AV_OPT_TYPE_CONST,{ AV_DISPOSITION_HEARING_IMPAIRED },0,0,0 , "flags" },
		{ "visual_impaired" , NULL, 0, AV_OPT_TYPE_CONST,{ AV_DISPOSITION_VISUAL_IMPAIRED },0,0,0 , "flags" },
		{ "clean_effects" , NULL, 0, AV_OPT_TYPE_CONST,{ AV_DISPOSITION_CLEAN_EFFECTS },0,0,0 , "flags" },
		{ "attached_pic" , NULL, 0, AV_OPT_TYPE_CONST,{ AV_DISPOSITION_ATTACHED_PIC },0,0,0 , "flags" },
		{ "captions" , NULL, 0, AV_OPT_TYPE_CONST,{ AV_DISPOSITION_CAPTIONS },0,0,0 , "flags" },
		{ "descriptions" , NULL, 0, AV_OPT_TYPE_CONST,{ AV_DISPOSITION_DESCRIPTIONS },0,0,0 , "flags" },
		{ "dependent" , NULL, 0, AV_OPT_TYPE_CONST,{ AV_DISPOSITION_DEPENDENT },0,0,0 , "flags" },
		{ "metadata" , NULL, 0, AV_OPT_TYPE_CONST,{ AV_DISPOSITION_METADATA },0,0,0 , "flags" },
		{ NULL },
	};
	const AVClass init_output_class = {
		"",
		av_default_item_name,
		init_output_stream_opts,
		LIBAVUTIL_VERSION_INT,
	};
	const enum AVPixelFormat mjpeg_formats[7] = {
		AV_PIX_FMT_YUVJ420P,
		AV_PIX_FMT_YUVJ422P,
		AV_PIX_FMT_YUVJ444P,
		AV_PIX_FMT_YUV420P,
		AV_PIX_FMT_YUV422P,
		AV_PIX_FMT_YUV444P,
		AV_PIX_FMT_NONE };
	const enum AVPixelFormat ljpeg_formats[10] = {
		AV_PIX_FMT_BGR24 ,
		AV_PIX_FMT_BGRA ,
		AV_PIX_FMT_BGR0,
		AV_PIX_FMT_YUVJ420P,
		AV_PIX_FMT_YUVJ444P,
		AV_PIX_FMT_YUVJ422P,
		AV_PIX_FMT_YUV420P ,
		AV_PIX_FMT_YUV444P ,
		AV_PIX_FMT_YUV422P,
		AV_PIX_FMT_NONE };
	const AVRational _AV_TIME_BASE_Q{ 1, AV_TIME_BASE };
	const AVRational TimeBase_1_1{ 1, 1 };
	const AVRational TimeBase_0_1{ 0, 1 };
	const AVRational TimeBase_25_1{ 25, 1 };
	const AVRational TimeBase_1_1000{ 1, 1000 };
	const char* const forced_keyframes_const_names[6] = {
		"n",
		"n_forced",
		"prev_forced_n",
		"prev_forced_t",
		"t",
		NULL
	};
	const char* const frame_rates[3] = {
		"25",
		"30000/1001",
		"24000/1001"
	};
public:
	static int compare_codec_desc(const void* a, const void* b) {
		const AVCodecDescriptor* const* da = (const AVCodecDescriptor* const*)a;
		const AVCodecDescriptor* const* db = (const AVCodecDescriptor* const*)b;
		return (*da)->type != (*db)->type ? FFDIFFSIGN((*da)->type, (*db)->type) :
			strcmp((*da)->name, (*db)->name);
	}
	static const AVOption* opt_find(void* obj, const char* name, const char* unit, int opt_flags, int search_flags) {
		const AVOption* o = av_opt_find(obj, name, unit, opt_flags, search_flags);
		if (o && !o->flags)
			return NULL;
		return o;
	}
	int m_bExit = 0;
	char s_avErr[AV_ERROR_MAX_STRING_SIZE];
	char* _av_err2str(int errnum) {
		memset(s_avErr, 0, AV_ERROR_MAX_STRING_SIZE);
		av_make_error_string(s_avErr, AV_ERROR_MAX_STRING_SIZE, errnum);
		return s_avErr;
	}
	char s_avTimeErr[AV_ERROR_MAX_STRING_SIZE];
	char* _av_ts2timestr(int64_t ts, AVRational* tb) {
		memset(s_avTimeErr, 0, AV_TS_MAX_STRING_SIZE);
		av_ts_make_time_string(s_avTimeErr, ts, tb);
		return s_avTimeErr;
	}
	int nb_hw_devices = 0;
	HWDevice** hw_devices = NULL;
	int want_sdp = 1;
	AVIOContext* progress_avio = NULL;
	uint8_t* subtitle_out = NULL;
	InputStream** input_streams = NULL;
	int nb_input_streams = 0;
	InputFile** input_files = NULL;
	int nb_input_files = 0;
	OutputStream** output_streams = NULL;
	int nb_output_streams = 0;
	OutputFile** output_files = NULL;
	int nb_output_files = 0;
	FilterGraph** filtergraphs = NULL;
	int nb_filtergraphs = 0;
	AVBufferRef* hw_device_ctx = NULL;
	HWDevice* filter_hw_device = NULL;
	char* sdp_filename = NULL;
	float audio_drift_threshold = 0.1;
	float dts_delta_threshold = 10;
	float dts_error_threshold = 3600 * 30;
	int dummy = 0;
	int audio_volume = 256;
	int audio_sync_method = 0;
	int video_sync_method = VSYNC_AUTO;
	float frame_drop_threshold = 0;
	int do_deinterlace = 0;
	int do_benchmark = 0;
	int do_benchmark_all = 0;
	int do_hex_dump = 0;
	int do_pkt_dump = 0;
	int copy_ts = 0;
	int start_at_zero = 0;
	int copy_tb = -1;
	int debug_ts = 0;
	int exit_on_error = 0;
	int abort_on_flags = 0;
	int qp_hist = 0;
	int print_stats = -1;
	int stdin_interaction = 1;
	int frame_bits_per_raw_sample = 0;
	float max_error_rate = 2.0 / 3;
	int filter_nbthreads = 0;
	int filter_complex_nbthreads = 0;
	int vstats_version = 2;
	int intra_only = 0;
	int file_overwrite = 0;
	int no_file_overwrite = 0;
	int do_psnr = 0;
	int input_sync = 0;
	int input_stream_potentially_available = 0;
	int ignore_unknown_streams = 0;
	int copy_unknown_streams = 0;
	int find_stream_info = 1;
	AVDictionary* sws_dict = NULL;
	AVDictionary* swr_opts = NULL;
	AVDictionary* format_opts = NULL;
	AVDictionary* codec_opts = NULL;
	AVDictionary* resample_opts = NULL;
	int hide_banner = 0;
	static int show_help(FfmpegExe* owner, OptionsContext* optctx, const char* opt, const char* arg) { return 0; }
	void init_opts(void)
	{
		av_dict_set(&sws_dict, "flags", "bicubic", 0);
	}
	void uninit_opts(void)
	{
		av_dict_free(&swr_opts);
		av_dict_free(&sws_dict);
		av_dict_free(&format_opts);
		av_dict_free(&codec_opts);
		av_dict_free(&resample_opts);
	}
	void Stop() {
		Log("%s", __FUNCTION__);
		ffmpeg_cleanup();
		throw - 2;
	}
	double parse_number_or_die(const char* context, const char* numstr, int type, double min, double max)
	{
		char* tail;
		const char* error;
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
		Log(error, context, numstr, min, max);
		Stop();
		return 0;
	}
	int64_t parse_time_or_die(const char* context, const char* timestr,
		int is_duration)
	{
		int64_t us;
		if (av_parse_time(&us, timestr, is_duration) < 0) {
			Log("Invalid %s specification for %s: %s\n",
				is_duration ? "duration" : "date", context, timestr);
			Stop();
		}
		return us;
	}
	const OptionDef* find_option(const OptionDef* po, const char* name)
	{
		const char* p = strchr(name, ':');
		int len = p ? p - name : strlen(name);
		while (po->name) {
			if (!strncmp(name, po->name, len) && strlen(po->name) == len)
				break;
			po++;
		}
		return po;
	}
	int write_option(OptionsContext* optctx, const OptionDef* po, const char* opt, const char* arg) {
		void* dst = po->flags & (OPT_OFFSET | OPT_SPEC) ?
			(uint8_t*)optctx + po->u.off : po->u.dst_ptr;
		int* dstcount;
		if (po->flags & OPT_SPEC) {
			SpecifierOpt** so = (SpecifierOpt**)dst;
			char* p = (char*)strchr(opt, ':');
			char* str;
			dstcount = (int*)(so + 1);
			*so = (SpecifierOpt*)grow_array(*so, sizeof(**so), dstcount, *dstcount + 1);
			str = av_strdup(p ? p + 1 : "");
			if (!str) { Log("%s %d Memory error", __FUNCTION__, __LINE__); Stop(); return -1; }

			(*so)[*dstcount - 1].specifier = str;
			dst = &(*so)[*dstcount - 1].u;
		}
		if (po->flags & OPT_STRING) {
			char* str;
			str = av_strdup(arg);
			av_freep(dst);
			if (!str)
			{
				Log("%s %d Memory error", __FUNCTION__, __LINE__); Stop(); return -1;
			}
			*(char**)dst = str;
		}
		else if (po->flags & OPT_BOOL || po->flags & OPT_INT) {
			*(int*)dst = parse_number_or_die(opt, arg, OPT_INT64, INT_MIN, INT_MAX);
		}
		else if (po->flags & OPT_INT64) {
			*(int64_t*)dst = parse_number_or_die(opt, arg, OPT_INT64, INT64_MIN, INT64_MAX);
		}
		else if (po->flags & OPT_TIME) {
			*(int64_t*)dst = parse_time_or_die(opt, arg, 1);
		}
		else if (po->flags & OPT_FLOAT) {
			*(float*)dst = parse_number_or_die(opt, arg, OPT_FLOAT, -INFINITY, INFINITY);
		}
		else if (po->flags & OPT_DOUBLE) {
			*(double*)dst = parse_number_or_die(opt, arg, OPT_DOUBLE, -INFINITY, INFINITY);
		}
		else if (po->u.func_arg) {
			int ret = po->u.func_arg(this, optctx, opt, arg);
			if (ret < 0) {
				Log("Failed to set value '%s' for option '%s': %s\n",
					arg, opt, _av_err2str(ret));
				return ret;
			}
		}
		if (po->flags & OPT_EXIT)
			Stop();
		return 0;
	}
	int parse_option(OptionsContext* optctx, const char* opt, const char* arg,
		const OptionDef* options)
	{
		const OptionDef* po;
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
			Log("Unrecognized option '%s'\n", opt);
			return AVERROR(EINVAL);
		}
		if (po->flags & HAS_ARG && !arg) {
			Log("Missing argument for option '%s'\n", opt);
			return AVERROR(EINVAL);
		}
		ret = write_option(optctx, po, opt, arg);
		if (ret < 0)
			return ret;
		return !!(po->flags & HAS_ARG);
	}
	void parse_options(OptionsContext* optctx, int argc, char** argv, const OptionDef* options,
		void(*parse_arg_function)(void*, const char*))
	{
		const char* opt;
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
				if ((ret = parse_option(optctx, opt, argv[optindex], options)) < 0)
					Stop();
				optindex += ret;
			}
			else {
				if (parse_arg_function)
					parse_arg_function(optctx, opt);
			}
		}
	}
	int parse_optgroup(OptionsContext* optctx, OptionGroup* g)
	{
		int i, ret;
		Log("Parsing a group of options: %s %s.\n",
			g->group_def->name, g->arg);
		for (i = 0; i < g->nb_opts; i++) {
			Option* o = &g->opts[i];
			if (g->group_def->flags &&
				!(g->group_def->flags & o->opt->flags)) {
				Log("Option %s (%s) cannot be applied to "
					"%s %s -- you are trying to apply an input option to an "
					"output file or vice versa. Move this option before the "
					"file it belongs to.\n", o->key, o->opt->help,
					g->group_def->name, g->arg);
				return AVERROR(EINVAL);
			}
			Log("Applying option %s (%s) with argument %s.\n",
				o->key, o->opt->help, o->val);
			ret = write_option(optctx, o->opt, o->key, o->val);
			if (ret < 0)
				return ret;
		}
		Log("Successfully parsed a group of options.\n");
		return 0;
	}
	void check_options(const OptionDef* po)
	{
		while (po->name) {
			if (po->flags & OPT_PERFILE)
				av_assert0(po->flags & (OPT_INPUT | OPT_OUTPUT));
			po++;
		}
	}
	static int opt_default(FfmpegExe* owner, OptionsContext* optctx, const char* opt, const char* arg)
	{
		const AVOption* o;
		int consumed = 0;
		char opt_stripped[128];
		const char* p;
		const AVClass* cc = avcodec_get_class(), * fc = avformat_get_class();
#if CONFIG_SWSCALE
		const AVClass* sc = sws_get_class();
#endif
#if CONFIG_SWRESAMPLE
		const AVClass* swr_class = swr_get_class();
#endif
		if (!strcmp(opt, "debug") || !strcmp(opt, "fdebug"))
			av_log_set_level(AV_LOG_DEBUG);
		if (!(p = strchr(opt, ':')))
			p = opt + strlen(opt);
		av_strlcpy(opt_stripped, opt, FFMIN(sizeof(opt_stripped), p - opt + 1));
		if ((o = opt_find(&cc, opt_stripped, NULL, 0,
			AV_OPT_SEARCH_CHILDREN | AV_OPT_SEARCH_FAKE_OBJ)) ||
			((opt[0] == 'v' || opt[0] == 'a' || opt[0] == 's') &&
				(o = opt_find(&cc, opt + 1, NULL, 0, AV_OPT_SEARCH_FAKE_OBJ)))) {
			av_dict_set(&owner->codec_opts, opt, arg, (o->type == AV_OPT_TYPE_FLAGS && (arg[0] == '-' || arg[0] == '+')) ? AV_DICT_APPEND : 0);
			consumed = 1;
		}
		if ((o = opt_find(&fc, opt, NULL, 0,
			AV_OPT_SEARCH_CHILDREN | AV_OPT_SEARCH_FAKE_OBJ))) {
			av_dict_set(&owner->format_opts, opt, arg, (o->type == AV_OPT_TYPE_FLAGS && (arg[0] == '-' || arg[0] == '+')) ? AV_DICT_APPEND : 0);
			if (consumed)
				owner->Log("Routing option %s to both codec and muxer layer\n", opt);
			consumed = 1;
		}
#if CONFIG_SWSCALE
		if (!consumed && (o = opt_find(&sc, opt, NULL, 0,
			AV_OPT_SEARCH_CHILDREN | AV_OPT_SEARCH_FAKE_OBJ))) {
			struct SwsContext* sws = sws_alloc_context();
			int ret = av_opt_set(sws, opt, arg, 0);
			sws_freeContext(sws);
			if (!strcmp(opt, "srcw") || !strcmp(opt, "srch") ||
				!strcmp(opt, "dstw") || !strcmp(opt, "dsth") ||
				!strcmp(opt, "src_format") || !strcmp(opt, "dst_format")) {
				owner->Log("Directly using swscale dimensions/format options is not supported, please use the -s or -pix_fmt options\n");
				return AVERROR(EINVAL);
			}
			if (ret < 0) {
				owner->Log("Error setting option %s.\n", opt);
				return ret;
			}
			av_dict_set(&owner->sws_dict, opt, arg, (o->type == AV_OPT_TYPE_FLAGS && (arg[0] == '-' || arg[0] == '+')) ? AV_DICT_APPEND : 0);
			consumed = 1;
		}
#else 
		if (!consumed && !strcmp(opt, "sws_flags")) {
			Log("Ignoring %s %s, due to disabled swscale\n", opt, arg);
			consumed = 1;
		}
#endif
#if CONFIG_SWRESAMPLE
		if (!consumed && (o = opt_find(&swr_class, opt, NULL, 0,
			AV_OPT_SEARCH_CHILDREN | AV_OPT_SEARCH_FAKE_OBJ))) {
			struct SwrContext* swr = swr_alloc();
			int ret = av_opt_set(swr, opt, arg, 0);
			swr_free(&swr);
			if (ret < 0) {
				owner->Log("Error setting option %s.\n", opt);
				return ret;
			}
			av_dict_set(&owner->swr_opts, opt, arg, (o->type == AV_OPT_TYPE_FLAGS && (arg[0] == '-' || arg[0] == '+')) ? AV_DICT_APPEND : 0);
			consumed = 1;
		}
#endif
		if (consumed)
			return 0;
		return AVERROR_OPTION_NOT_FOUND;
	}
	int match_group_separator(const OptionGroupDef* groups, int nb_groups,
		const char* opt)
	{
		int i;
		for (i = 0; i < nb_groups; i++) {
			const OptionGroupDef* p = &groups[i];
			if (p->sep && !strcmp(p->sep, opt))
				return i;
		}
		return -1;
	}
	void finish_group(OptionParseContext* octx, int group_idx,
		const char* arg)
	{
		OptionGroupList* l = &octx->groups[group_idx];
		OptionGroup* g;
		l->groups = (OptionGroup*)grow_array(l->groups, sizeof(*l->groups), &l->nb_groups, l->nb_groups + 1);
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
		init_opts();
		memset(&octx->cur_group, 0, sizeof(octx->cur_group));
	}
	void add_opt(OptionParseContext* octx, const OptionDef* opt,
		const char* key, const char* val)
	{
		int global = !(opt->flags & (OPT_PERFILE | OPT_SPEC | OPT_OFFSET));
		OptionGroup* g = global ? &octx->global_opts : &octx->cur_group;
		g->opts = (Option*)grow_array(g->opts, sizeof(*g->opts), &g->nb_opts, g->nb_opts + 1);
		g->opts[g->nb_opts - 1].opt = opt;
		g->opts[g->nb_opts - 1].key = key;
		g->opts[g->nb_opts - 1].val = val;
	}
	void init_parse_context(OptionParseContext* octx, const OptionGroupDef* groups, int nb_groups) {
		int i;
		memset(octx, 0, sizeof(*octx));
		octx->nb_groups = nb_groups;
		octx->groups = (OptionGroupList*)av_mallocz_array(octx->nb_groups, sizeof(*octx->groups));
		if (!octx->groups)
			Stop();
		for (i = 0; i < octx->nb_groups; i++)
			octx->groups[i].group_def = &groups[i];
		octx->global_opts.group_def = &global_group;
		octx->global_opts.arg = "";
		init_opts();
	}
	void uninit_parse_context(OptionParseContext* octx)
	{
		int i, j;
		for (i = 0; i < octx->nb_groups; i++) {
			OptionGroupList* l = &octx->groups[i];
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
		uninit_opts();
	}

	int split_commandline(OptionParseContext* octx, int argc, char* argv[], const OptionDef* options, const OptionGroupDef* groups, int nb_groups) {
		int optindex = 1;
		int dashdash = -2;
		init_parse_context(octx, groups, nb_groups);
		Log("Splitting the commandline.\n");
		while (optindex < argc) {
			const char* opt = argv[optindex++];
			Log("Reading option [%d] '%s' ...\n", optindex - 1, opt);
			if (opt == NULL || strlen(opt) == 0)
				break;
			const OptionDef* po;
			int ret;
			if (opt[0] == '-' && opt[1] == '-' && !opt[2]) {
				dashdash = optindex;
				continue;
			}
			if (opt[0] != '-' || !opt[1] || dashdash + 1 == optindex) {
				finish_group(octx, 0, opt);
				Log(" matched as %s.\n", groups[0].name);
				continue;
			}
			opt++;
			const char* arg = NULL;
			if ((ret = match_group_separator(groups, nb_groups, opt)) >= 0) {
				do {
					arg = argv[optindex++];
					if (!arg) {
						Log("Missing argument for option '%s'.\n", opt);
						return AVERROR(EINVAL);
					}
				} while (0);
				finish_group(octx, ret, arg);
				Log(" matched as %s with argument '%s'.\n",
					groups[ret].name, arg);
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
							Log("Missing argument for option '%s'.\n", opt);
							return AVERROR(EINVAL);
						}
					} while (0);
				}
				else {
					arg = "1";
				}
				add_opt(octx, po, opt, arg);
				Log(" matched as option '%s' (%s) with "
					"argument '%s'.\n", po->name, po->help, arg);
				continue;
			}
			if (argv[optindex]) {
				ret = opt_default(this, NULL, opt, argv[optindex]);
				if (ret >= 0) {
					Log(" matched as AVOption '%s' with "
						"argument '%s'.\n", opt, argv[optindex]);
					optindex++;
					continue;
				}
				else if (ret != AVERROR_OPTION_NOT_FOUND) {
					Log("Error parsing option '%s' "
						"with argument '%s'.\n", opt, argv[optindex]);
					return ret;
				}
			}
			if (opt[0] == 'n' && opt[1] == 'o' &&
				(po = find_option(options, opt + 2)) &&
				po->name && po->flags & OPT_BOOL) {
				add_opt(octx, po, opt, "0");
				Log(" matched as option '%s' (%s) with "
					"argument 0.\n", po->name, po->help);
				continue;
			}
			Log("Unrecognized option '%s'.\n", opt);
			return AVERROR_OPTION_NOT_FOUND;
		}
		if (octx->cur_group.nb_opts || codec_opts || format_opts || resample_opts)
			Log("Trailing options were found on the "
				"commandline.\n");
		Log("Finished splitting the commandline.\n");
		return 0;
	}
	static int opt_cpuflags(FfmpegExe* owner, OptionsContext* optctx, const char* opt, const char* arg)
	{
		int ret;
		unsigned flags = av_get_cpu_flags();
		if ((ret = av_parse_cpu_caps(&flags, arg)) < 0)
			return ret;
		av_force_cpu_flags(flags);
		return 0;
	}
	static int opt_loglevel(FfmpegExe* owner, OptionsContext* optctx, const char* opt, const char* arg)
	{
		const struct {
			const char* name; int level;
		}log_levels[] = {
			{ "quiet" , AV_LOG_QUIET },
			{ "panic" , AV_LOG_PANIC },
			{ "fatal" , AV_LOG_FATAL },
			{ "error" , AV_LOG_ERROR },
			{ "warning", AV_LOG_WARNING },
			{ "info" , AV_LOG_INFO },
			{ "verbose", AV_LOG_VERBOSE },
			{ "debug" , AV_LOG_DEBUG },
			{ "trace" , AV_LOG_TRACE },
		};
		const char* token;
		char* tail;
		int flags = av_log_get_flags();
		int level = av_log_get_level();
		int cmd, i = 0;
		av_assert0(arg);
		while (*arg) {
			token = arg;
			if (*token == '+' || *token == '-') {
				cmd = *token++;
			}
			else {
				cmd = 0;
			}
			if (!i && !cmd) {
				flags = 0;
			}
			if (!strncmp(token, "repeat", 6)) {
				if (cmd == '-') {
					flags |= AV_LOG_SKIP_REPEATED;
				}
				else {
					flags &= ~AV_LOG_SKIP_REPEATED;
				}
				arg = token + 6;
			}
			else if (!strncmp(token, "level", 5)) {
				if (cmd == '-') {
					flags &= ~AV_LOG_PRINT_LEVEL;
				}
				else {
					flags |= AV_LOG_PRINT_LEVEL;
				}
				arg = token + 5;
			}
			else {
				break;
			}
			i++;
		}
		if (!*arg) {
			goto end;
		}
		else if (*arg == '+') {
			arg++;
		}
		else if (!i) {
			flags = av_log_get_flags();
		}
		for (i = 0; i < FF_ARRAY_ELEMS(log_levels); i++) {
			if (!strcmp(log_levels[i].name, arg)) {
				level = log_levels[i].level;
				goto end;
			}
		}
		level = strtol(arg, &tail, 10);
		if (*tail) {
			owner->Log("Invalid loglevel \"%s\". "
				"Possible levels are numbers or:\n", arg);
			for (i = 0; i < FF_ARRAY_ELEMS(log_levels); i++)
				owner->Log("\"%s\"\n", log_levels[i].name);
			owner->Stop();
		}
	end:
		av_log_set_flags(flags);
		av_log_set_level(level);
		return 0;
	}
	static int opt_report(const char* opt)
	{
		return 0;
	}
	static int opt_max_alloc(FfmpegExe* owner, OptionsContext* optctx, const char* opt, const char* arg)
	{
		char* tail;
		size_t max;
		max = strtol(arg, &tail, 10);
		if (*tail) {
			owner->Log("Invalid max_alloc \"%s\".\n", arg);
			owner->Stop();
		}
		av_max_alloc(max);
		return 0;
	}
	static int opt_timelimit(FfmpegExe* owner, OptionsContext* optctx, const char* opt, const char* arg)
	{
		owner->Log("-%s not implemented on this OS\n", opt);
		return 0;
	}
	char get_media_type_char(enum AVMediaType type)
	{
		switch (type) {
		case AVMEDIA_TYPE_VIDEO: return 'V';
		case AVMEDIA_TYPE_AUDIO: return 'A';
		case AVMEDIA_TYPE_DATA: return 'D';
		case AVMEDIA_TYPE_SUBTITLE: return 'S';
		case AVMEDIA_TYPE_ATTACHMENT:return 'T';
		default: return '?';
		}
	}
	const AVCodec* next_codec_for_id(enum AVCodecID id, const AVCodec* prev,
		int encoder)
	{
		while ((prev = av_codec_next(prev))) {
			if (prev->id == id &&
				(encoder ? av_codec_is_encoder(prev) : av_codec_is_decoder(prev)))
				return prev;
		}
		return NULL;
	}
	unsigned get_codecs_sorted(const AVCodecDescriptor*** rcodecs)
	{
		const AVCodecDescriptor* desc = NULL;
		const AVCodecDescriptor** codecs;
		unsigned nb_codecs = 0, i = 0;
		while ((desc = avcodec_descriptor_next(desc)))
			nb_codecs++;
		if (!(codecs = (const AVCodecDescriptor**)av_calloc(nb_codecs, sizeof(*codecs)))) {
			Log("Out of memory\n");
			Stop();
		}
		desc = NULL;
		while ((desc = avcodec_descriptor_next(desc)))
			codecs[i++] = desc;
		av_assert0(i == nb_codecs);
		qsort(codecs, nb_codecs, sizeof(*codecs), compare_codec_desc);
		*rcodecs = codecs;
		return nb_codecs;
	}
	FILE* get_preset_file(char* filename, size_t filename_size,
		const char* preset_name, int is_path,
		const char* codec_name)
	{
		FILE* f = NULL;
		int i;
		const char* base[3] = { getenv("FFMPEG_DATADIR"),
			getenv("HOME"),
			FFMPEG_DATADIR, };
		if (is_path) {
			av_strlcpy(filename, preset_name, filename_size);
			f = fopen(filename, "r");
		}
		else {
#ifdef _WIN32
			char datadir[MAX_PATH], * ls;
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
	int check_stream_specifier(AVFormatContext* s, AVStream* st, const char* spec)
	{
		int ret = avformat_match_stream_specifier(s, st, spec);
		if (ret < 0)
			Log("Invalid stream specifier: %s.\n", spec);
		return ret;
	}
	AVDictionary* filter_codec_opts(AVDictionary* opts, enum AVCodecID codec_id,
		AVFormatContext* s, AVStream* st, AVCodec* codec)
	{
		AVDictionary* ret = NULL;
		AVDictionaryEntry* t = NULL;
		int flags = s->oformat ? AV_OPT_FLAG_ENCODING_PARAM
			: AV_OPT_FLAG_DECODING_PARAM;
		char prefix = 0;
		const AVClass* cc = avcodec_get_class();
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
			char* p = strchr(t->key, ':');
			if (p)
				switch (check_stream_specifier(s, st, p + 1)) {
				case 1: *p = 0; break;
				case 0: continue;
				default: Stop();
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
	AVDictionary** setup_find_stream_info_opts(AVFormatContext* s, AVDictionary* codec_opts) {
		int i;
		AVDictionary** opts;
		if (!s->nb_streams)
			return NULL;
		opts = (AVDictionary**)av_mallocz_array(s->nb_streams, sizeof(*opts));
		if (!opts) {
			Log("Could not alloc memory for stream options.\n");
			return NULL;
		}
		for (i = 0; i < s->nb_streams; i++)
			opts[i] = filter_codec_opts(codec_opts, s->streams[i]->codecpar->codec_id,
				s, s->streams[i], NULL);
		return opts;
	}
	void* grow_array(void* array, int elem_size, int* size, int new_size) {
		if (new_size >= INT_MAX / elem_size) {
			Log("Array too big.\n");
			Stop();
		}
		if (*size < new_size) {
			uint8_t* tmp = (uint8_t*)av_realloc_array(array, new_size, elem_size);
			if (!tmp) {
				Log("Could not alloc buffer.\n");
				Stop();
			}
			memset(tmp + *size * elem_size, 0, (new_size - *size) * elem_size);
			*size = new_size;
			return tmp;
		}
		return array;
	}
	double get_rotation(AVStream* st) {
		uint8_t* displaymatrix = av_stream_get_side_data(st,
			AV_PKT_DATA_DISPLAYMATRIX, NULL);
		double theta = 0;
		if (displaymatrix)
			theta = -av_display_rotation_get((int32_t*)displaymatrix);
		theta -= 360 * floor(theta / 360 + 0.9 / 360);
		return theta;
	}
	void uninit_options(OptionsContext* o) {
		const OptionDef* po = ffmpeg_options;
		while (po->name) {
			void* dst = (uint8_t*)o + po->u.off;
			if (po->flags & OPT_SPEC) {
				SpecifierOpt** so = (SpecifierOpt**)dst;
				int* count = (int*)(so + 1);
				for (int i = 0; i < *count; i++) {
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
		for (int i = 0; i < o->nb_stream_maps; i++)
			av_freep(&o->stream_maps[i].linklabel);
		av_freep(&o->stream_maps);
		av_freep(&o->audio_channel_maps);
		av_freep(&o->streamid_map);
		av_freep(&o->attachments);
	}
	AVDictionary* strip_specifiers(AVDictionary* dict) {
		AVDictionaryEntry* e = NULL;
		AVDictionary* ret = NULL;
		while ((e = av_dict_get(dict, "", e, AV_DICT_IGNORE_SUFFIX))) {
			char* p = strchr(e->key, ':');
			if (p)
				*p = 0;
			av_dict_set(&ret, e->key, e->value, 0);
			if (p)
				*p = ':';
		}
		return ret;
	}
	static int opt_abort_on(FfmpegExe* owner, OptionsContext* optctx, const char* opt, const char* arg) {
		const AVClass* pclass = &owner->opt_abort_on_class;
		return av_opt_eval_flags(&pclass, &owner->opt_abort_on_opts[0], arg, &owner->abort_on_flags);
	}
	static int opt_sameq(FfmpegExe* owner, OptionsContext* optctx, const char* opt, const char* arg) {
		owner->Log("Option '%s' was removed. "
			"If you are looking for an option to preserve the quality (which is not "
			"what -%s was for), use -qscale 0 or an equivalent quality factor option.\n",
			opt, opt);
		return AVERROR(EINVAL);
	}
	static int opt_video_channel(FfmpegExe* owner, OptionsContext* optctx, const char* opt, const char* arg) {
		owner->Log("This option is deprecated, use -channel.\n");
		return opt_default(owner, optctx, "channel", arg);
	}
	static int opt_video_standard(FfmpegExe* owner, OptionsContext* optctx, const char* opt, const char* arg) {
		owner->Log("This option is deprecated, use -standard.\n");
		return opt_default(owner, optctx, "standard", arg);
	}
	static int opt_audio_codec(FfmpegExe* owner, OptionsContext* optctx, const char* opt, const char* arg) {
		return owner->parse_option(optctx, "codec:a", arg, owner->ffmpeg_options);
	}
	static int opt_video_codec(FfmpegExe* owner, OptionsContext* optctx, const char* opt, const char* arg) {
		return owner->parse_option(optctx, "codec:v", arg, owner->ffmpeg_options);
	}
	static int opt_subtitle_codec(FfmpegExe* owner, OptionsContext* optctx, const char* opt, const char* arg) {
		return owner->parse_option(optctx, "codec:s", arg, owner->ffmpeg_options);
	}
	static int opt_data_codec(FfmpegExe* owner, OptionsContext* optctx, const char* opt, const char* arg) {
		return owner->parse_option(optctx, "codec:d", arg, owner->ffmpeg_options);
	}
	static int opt_map(FfmpegExe* owner, OptionsContext* optctx, const char* opt, const char* arg) {
		StreamMap* m = NULL;
		int i, negative = 0, file_idx, disabled = 0;
		int sync_file_idx = -1, sync_stream_idx = 0;
		char* p, * sync;
		char* map;
		char* allow_unused;
		if (*arg == '-') {
			negative = 1;
			arg++;
		}
		map = av_strdup(arg);
		if (!map) {
			owner->Log("%s %d Memory error", __FUNCTION__, __LINE__);
			owner->Stop();
			return -1;
		}
		if (sync = strchr(map, ',')) {
			*sync = 0;
			sync_file_idx = strtol(sync + 1, &sync, 0);
			if (sync_file_idx >= owner->nb_input_files || sync_file_idx < 0) {
				owner->Log("Invalid sync file index: %d.\n", sync_file_idx);
				owner->Stop();
			}
			if (*sync)
				sync++;
			for (i = 0; i < owner->input_files[sync_file_idx]->nb_streams; i++)
				if (owner->check_stream_specifier(owner->input_files[sync_file_idx]->ctx,
					owner->input_files[sync_file_idx]->ctx->streams[i], sync) == 1) {
					sync_stream_idx = i;
					break;
				}
			if (i == owner->input_files[sync_file_idx]->nb_streams) {
				owner->Log("Sync stream specification in map %s does not "
					"match any streams.\n", arg);
				owner->Stop();
			}
			if (owner->input_streams[owner->input_files[sync_file_idx]->ist_index + sync_stream_idx]->user_set_discard == AVDISCARD_ALL) {
				owner->Log("Sync stream specification in map %s matches a disabled input "
					"stream.\n", arg);
				owner->Stop();
			}
		}
		if (map[0] == '[') {
			const char* c = map + 1;
			optctx->stream_maps = (StreamMap*)owner->grow_array(optctx->stream_maps, sizeof(*optctx->stream_maps), &optctx->nb_stream_maps, optctx->nb_stream_maps + 1);
			m = &optctx->stream_maps[optctx->nb_stream_maps - 1];
			m->linklabel = av_get_token(&c, "]");
			if (!m->linklabel) {
				owner->Log("Invalid output link label: %s.\n", map);
				owner->Stop();
			}
		}
		else {
			if (allow_unused = strchr(map, '?'))
				*allow_unused = 0;
			file_idx = strtol(map, &p, 0);
			if (file_idx >= owner->nb_input_files || file_idx < 0) {
				owner->Log("Invalid input file index: %d.\n", file_idx);
				owner->Stop();
			}
			if (negative)
				for (i = 0; i < optctx->nb_stream_maps; i++) {
					m = &optctx->stream_maps[i];
					if (file_idx == m->file_index &&
						owner->check_stream_specifier(owner->input_files[m->file_index]->ctx,
							owner->input_files[m->file_index]->ctx->streams[m->stream_index],
							*p == ':' ? p + 1 : p) > 0)
						m->disabled = 1;
				}
			else
				for (i = 0; i < owner->input_files[file_idx]->nb_streams; i++) {
					if (owner->check_stream_specifier(owner->input_files[file_idx]->ctx, owner->input_files[file_idx]->ctx->streams[i],
						*p == ':' ? p + 1 : p) <= 0)
						continue;
					if (owner->input_streams[owner->input_files[file_idx]->ist_index + i]->user_set_discard == AVDISCARD_ALL) {
						disabled = 1;
						continue;
					}
					optctx->stream_maps = (StreamMap*)owner->grow_array(optctx->stream_maps, sizeof(*optctx->stream_maps), &optctx->nb_stream_maps, optctx->nb_stream_maps + 1);
					m = &optctx->stream_maps[optctx->nb_stream_maps - 1];
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
				owner->Log("Stream map '%s' matches no streams; ignoring.\n", arg);
			}
			else if (disabled) {
				owner->Log("Stream map '%s' matches disabled streams.\n"
					"To ignore this, add a trailing '?' to the map.\n", arg);
				owner->Stop();
			}
			else {
				owner->Log("Stream map '%s' matches no streams.\n"
					"To ignore this, add a trailing '?' to the map.\n", arg);
				owner->Stop();
			}
		}
		av_freep(&map);
		return 0;
	}
	static int opt_attach(FfmpegExe* owner, OptionsContext* optctx, const char* opt, const char* arg) {
		optctx->attachments = (const char**)owner->grow_array(optctx->attachments, sizeof(*optctx->attachments), &optctx->nb_attachments, optctx->nb_attachments + 1);
		optctx->attachments[optctx->nb_attachments - 1] = arg;
		return 0;
	}
	static int opt_map_channel(FfmpegExe* owner, OptionsContext* optctx, const char* opt, const char* arg) {
		int n;
		AVStream* st;
		AudioChannelMap* m;
		char* allow_unused;
		char* mapchan;
		mapchan = av_strdup(arg);
		if (!mapchan) {
			owner->Log("%s %d Memory error", __FUNCTION__, __LINE__);
			owner->Stop();
			return -1;
		}
		optctx->audio_channel_maps = (AudioChannelMap*)owner->grow_array(optctx->audio_channel_maps, sizeof(*optctx->audio_channel_maps), &optctx->nb_audio_channel_maps, optctx->nb_audio_channel_maps + 1);
		m = &optctx->audio_channel_maps[optctx->nb_audio_channel_maps - 1];
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
			owner->Log("Syntax error, mapchan usage: "
				"[file.stream.channel|-1][:syncfile:syncstream]\n");
			owner->Stop();
		}
		if (n != 5)
			m->ofile_idx = m->ostream_idx = -1;
		if (m->file_idx < 0 || m->file_idx >= owner->nb_input_files) {
			owner->Log("mapchan: invalid input file index: %d\n",
				m->file_idx);
			owner->Stop();
		}
		if (m->stream_idx < 0 ||
			m->stream_idx >= owner->input_files[m->file_idx]->nb_streams) {
			owner->Log("mapchan: invalid input file stream index #%d.%d\n",
				m->file_idx, m->stream_idx);
			owner->Stop();
		}
		st = owner->input_files[m->file_idx]->ctx->streams[m->stream_idx];
		if (st->codecpar->codec_type != AVMEDIA_TYPE_AUDIO) {
			owner->Log("mapchan: stream #%d.%d is not an audio stream.\n",
				m->file_idx, m->stream_idx);
			owner->Stop();
		}
		if (allow_unused = strchr(mapchan, '?'))
			*allow_unused = 0;
		if (m->channel_idx < 0 || m->channel_idx >= st->codecpar->channels ||
			owner->input_streams[owner->input_files[m->file_idx]->ist_index + m->stream_idx]->user_set_discard == AVDISCARD_ALL) {
			if (allow_unused) {
				owner->Log("mapchan: invalid audio channel #%d.%d.%d\n",
					m->file_idx, m->stream_idx, m->channel_idx);
			}
			else {
				owner->Log("mapchan: invalid audio channel #%d.%d.%d\n"
					"To ignore this, add a trailing '?' to the map_channel.\n",
					m->file_idx, m->stream_idx, m->channel_idx);
				owner->Stop();
			}
		}
		av_free(mapchan);
		return 0;
	}
	static int opt_sdp_file(FfmpegExe* owner, OptionsContext* optctx, const char* opt, const char* arg) {
		av_free(owner->sdp_filename);
		owner->sdp_filename = av_strdup(arg);
		return 0;
	}

	static int opt_init_hw_device(FfmpegExe* owner, OptionsContext* optctx, const char* opt, const char* arg) {
		if (!strcmp(arg, "list")) {
			enum AVHWDeviceType type = AV_HWDEVICE_TYPE_NONE;
			printf("Supported hardware device types:\n");
			while ((type = av_hwdevice_iterate_types(type)) !=
				AV_HWDEVICE_TYPE_NONE)
				printf("%s\n", av_hwdevice_get_type_name(type));
			printf("\n");
			owner->Stop();
			return -1;
		}
		return owner->hw_device_init_from_string(arg, NULL);
	}
	static int opt_filter_hw_device(FfmpegExe* owner, OptionsContext* optctx, const char* opt, const char* arg) {
		if (owner->filter_hw_device) {
			owner->Log("Only one filter device can be used.\n");
			return AVERROR(EINVAL);
		}
		owner->filter_hw_device = owner->hw_device_get_by_name(arg);
		if (!owner->filter_hw_device) {
			owner->Log("Invalid filter device %s.\n", arg);
			return AVERROR(EINVAL);
		}
		return 0;
	}
	void parse_meta_type(char* arg, char* type, int* index, const char** stream_spec) {
		if (*arg) {
			*type = *arg;
			switch (*arg) {
			case 'g':
				break;
			case 's':
				if (*(++arg) && *arg != ':') {
					Log("Invalid metadata specifier %s.\n", arg);
					Stop();
				}
				*stream_spec = *arg == ':' ? arg + 1 : "";
				break;
			case 'c':
			case 'p':
				if (*(++arg) == ':')
					*index = strtol(++arg, NULL, 0);
				break;
			default:
				Log("Invalid metadata type %c.\n", *arg);
				Stop();
			}
		}
		else
			*type = 'g';
	}
	int copy_metadata(char* outspec, char* inspec, AVFormatContext* oc, AVFormatContext* ic, OptionsContext* o) {
		AVDictionary** meta_in = NULL;
		AVDictionary** meta_out = NULL;
		int i, ret = 0;
		char type_in, type_out;
		const char* istream_spec = NULL, * ostream_spec = NULL;
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
		if (!ic)
			return 0;
		switch (type_in) {
		case 'g': meta_in = &ic->metadata;
			break;
		case 'c':
			if ((idx_in) < 0 || (idx_in) >= (ic->nb_chapters)) {
				Log("Invalid %s index %d while processing metadata maps.\n", ("chapter"),
					(idx_in)); Stop();
			}
			meta_in = &ic->chapters[idx_in]->metadata;
			break;
		case 'p':
			if ((idx_in) < 0 || (idx_in) >= (ic->nb_programs)) {
				Log("Invalid %s index %d while processing metadata maps.\n", ("program"), (idx_in));
				Stop();
			}
			meta_in = &ic->programs[idx_in]->metadata;
			break;
		case 's':
			break;
		default: av_assert0(0);
		};
		switch (type_out) {
		case 'g':
			meta_out = &oc->metadata;
			break;
		case 'c':
			if ((idx_out) < 0 || (idx_out) >= (oc->nb_chapters)) {
				Log("Invalid %s index %d while processing metadata maps.\n", ("chapter"), (idx_out));
				Stop();
			}
			meta_out = &oc->chapters[idx_out]->metadata;
			break;
		case 'p':
			if ((idx_out) < 0 || (idx_out) >= (oc->nb_programs)) {
				Log("Invalid %s index %d while processing metadata maps.\n", ("program"), (idx_out));
				Stop();
			}
			meta_out = &oc->programs[idx_out]->metadata;
			break;
		case 's':
			break;
		default:
			av_assert0(0);
		};
		if (type_in == 's') {
			for (i = 0; i < ic->nb_streams; i++) {
				if ((ret = check_stream_specifier(ic, ic->streams[i], istream_spec)) > 0) {
					meta_in = &ic->streams[i]->metadata;
					break;
				}
				else if (ret < 0)
					Stop();
			}
			if (!meta_in) {
				Log("Stream specifier %s does not match  any streams.\n", istream_spec);
				Stop();
			}
		}
		if (type_out == 's') {
			for (i = 0; i < oc->nb_streams; i++) {
				if ((ret = check_stream_specifier(oc, oc->streams[i], ostream_spec)) > 0) {
					meta_out = &oc->streams[i]->metadata;
					av_dict_copy(meta_out, *meta_in, AV_DICT_DONT_OVERWRITE);
				}
				else if (ret < 0)
					Stop();
			}
		}
		else
			av_dict_copy(meta_out, *meta_in, AV_DICT_DONT_OVERWRITE);
		return 0;
	}
	static int opt_recording_timestamp(FfmpegExe* owner, OptionsContext* optctx, const char* opt, const char* arg) {
		char buf[128];
		int64_t recording_timestamp = owner->parse_time_or_die(opt, arg, 0) / 1E6;
		struct tm time = *gmtime((time_t*)&recording_timestamp);
		if (!strftime(buf, sizeof(buf), "creation_time=%Y-%m-%dT%H:%M:%S%z", &time))
			return -1;
		owner->parse_option(optctx, "metadata", buf, owner->ffmpeg_options);
		owner->Log("%s is deprecated, set the 'creation_time' metadata "
			"tag instead.\n", opt);
		return 0;
	}
	AVCodec* find_codec_or_die(const char* name, enum AVMediaType type, int encoder) {
		const AVCodecDescriptor* desc;
		const char* codec_string = encoder ? "encoder" : "decoder";
		AVCodec* codec;
		codec = encoder ?
			avcodec_find_encoder_by_name(name) :
			avcodec_find_decoder_by_name(name);
		if (!codec && (desc = avcodec_descriptor_get_by_name(name))) {
			codec = encoder ? avcodec_find_encoder(desc->id) :
				avcodec_find_decoder(desc->id);
			if (codec)
				Log("Matched %s '%s' for codec '%s'.\n",
					codec_string, codec->name, desc->name);
		}
		if (!codec) {
			Log("Unknown %s '%s'\n", codec_string, name);
			Stop();
		}
		if (codec->type != type) {
			Log("Invalid %s type '%s'\n", codec_string, name);
			Stop();
		}
		return codec;
	}
	AVCodec* choose_decoder(OptionsContext* o, AVFormatContext* s, AVStream* st) {
		char* codec_name = NULL;
		{
			int i, ret;
			for (i = 0; i < o->nb_codec_names; i++) {
				char* spec = (char*)o->codec_names[i].specifier;
				if ((ret = check_stream_specifier(s, st, (const char*)spec)) > 0)
					codec_name = (char*)o->codec_names[i].u.str;
				else if (ret < 0)
					Stop();
			}
		};
		if (codec_name) {
			AVCodec* codec = find_codec_or_die(codec_name, st->codecpar->codec_type, 0);
			st->codecpar->codec_id = codec->id;
			return codec;
		}
		else
			return avcodec_find_decoder(st->codecpar->codec_id);
	}
	void add_input_streams(OptionsContext* o, AVFormatContext* ic) {
		int i, ret;
		for (i = 0; i < ic->nb_streams; i++) {
			AVStream* st = ic->streams[i];
			AVCodecParameters* par = st->codecpar;
			InputStream* ist = (InputStream*)(InputStream*)av_mallocz(sizeof(*ist));
			ist->owner = this;
			char* framerate = NULL, * hwaccel_device = NULL;
			const char* hwaccel = NULL;
			char* hwaccel_output_format = NULL;
			char* codec_tag = NULL;
			char* next;
			char* discard_str = NULL;
			const AVClass* cc = avcodec_get_class();
			const AVOption* discard_opt = av_opt_find(&cc, "skip_frame", NULL, 0, 0);
			if (!ist)
				Stop();
			input_streams = (InputStream**)grow_array(input_streams, sizeof(*input_streams), &nb_input_streams, nb_input_streams + 1);
			input_streams[nb_input_streams - 1] = ist;
			ist->st = st;
			ist->file_index = nb_input_files;
			ist->discard = 1;
			st->discard = AVDISCARD_ALL;
			ist->nb_samples = 0;
			ist->min_pts = INT64_MAX;
			ist->max_pts = INT64_MIN;
			ist->ts_scale = 1.0;
			{
				int i, ret;
				for (i = 0; i < o->nb_ts_scale; i++) {
					char* spec = (char*)o->ts_scale[i].specifier;
					if ((ret = check_stream_specifier(ic, st, (const char*)spec)) > 0)
						ist->ts_scale = (double)o->ts_scale[i].u.dbl;
					else if (ret < 0)
						Stop();
				}
			};
			ist->autorotate = 1;
			{
				int i, ret;
				for (i = 0; i < o->nb_autorotate; i++) {
					char* spec = (char*)o->autorotate[i].specifier;
					if ((ret = check_stream_specifier(ic, st, (const char*)spec)) > 0)
						ist->autorotate = (int)o->autorotate[i].u.i;
					else if (ret < 0)
						Stop();
				}
			};
			{
				int i, ret;
				for (i = 0; i < o->nb_codec_tags; i++) {
					char* spec = (char*)o->codec_tags[i].specifier;
					if ((ret = check_stream_specifier(ic, st, (const char*)spec)) > 0)
						codec_tag = (char*)o->codec_tags[i].u.str;
					else if (ret < 0) Stop();
				}
			};
			if (codec_tag) {
				uint32_t tag = strtol(codec_tag, &next, 0);
				if (*next)
					tag = AV_RL32(codec_tag);
				st->codecpar->codec_tag = tag;
			}
			ist->dec = choose_decoder(o, ic, st);
			ist->decoder_opts = filter_codec_opts(o->g->codec_opts, ist->st->codecpar->codec_id, ic, st, ist->dec);
			ist->reinit_filters = -1;
			{
				int i, ret;
				for (i = 0; i < o->nb_reinit_filters; i++) {
					char* spec = (char*)o->reinit_filters[i].specifier;
					if ((ret = check_stream_specifier(ic, st, (const char*)spec)) > 0)
						ist->reinit_filters = (int)o->reinit_filters[i].u.i;
					else if (ret < 0) Stop();
				}
			};
			{
				int i, ret;
				for (i = 0; i < o->nb_discard; i++) {
					char* spec = (char*)o->discard[i].specifier;
					if ((ret = check_stream_specifier(ic, st, (const char*)spec)) > 0)
						discard_str = (char*)o->discard[i].u.str;
					else if (ret < 0)
						Stop();
				}
			};
			ist->user_set_discard = AVDISCARD_NONE;
			if ((o->video_disable && ist->st->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) ||
				(o->audio_disable && ist->st->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) ||
				(o->subtitle_disable && ist->st->codecpar->codec_type == AVMEDIA_TYPE_SUBTITLE) ||
				(o->data_disable && ist->st->codecpar->codec_type == AVMEDIA_TYPE_DATA))
				ist->user_set_discard = AVDISCARD_ALL;
			if (discard_str && av_opt_eval_int(&cc, discard_opt, discard_str, &ist->user_set_discard) < 0) {
				Log("Error parsing discard %s.\n",
					discard_str);
				Stop();
			}
			ist->filter_in_rescale_delta_last = AV_NOPTS_VALUE;
			ist->dec_ctx = avcodec_alloc_context3(ist->dec);
			if (!ist->dec_ctx) {
				Log("Error allocating the decoder context.\n");
				Stop();
			}
			ret = avcodec_parameters_to_context(ist->dec_ctx, par);
			if (ret < 0) {
				Log("Error initializing the decoder context.\n");
				Stop();
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
				ist->dec_ctx->framerate = st->avg_frame_rate;
				{
					int i, ret;
					for (i = 0; i < o->nb_frame_rates; i++) {
						char* spec = (char*)o->frame_rates[i].specifier;
						if ((ret = check_stream_specifier(ic, st, (const char*)spec)) > 0)
							framerate = (char*)o->frame_rates[i].u.str;
						else if (ret < 0)
							Stop();
					}
				};
				if (framerate && av_parse_video_rate(&ist->framerate,
					framerate) < 0) {
					Log("Error parsing framerate %s.\n",
						framerate);
					Stop();
				}
				ist->top_field_first = -1;
				{
					int i, ret;
					for (i = 0; i < o->nb_top_field_first; i++) {
						char* spec = (char*)o->top_field_first[i].specifier;
						if ((ret = check_stream_specifier(ic, st, (const char*)spec)) > 0)
							ist->top_field_first = (int)o->top_field_first[i].u.i;
						else if (ret < 0)
							Stop();
					}
				};
				{
					int i, ret;
					for (i = 0; i < o->nb_hwaccels; i++) {
						char* spec = (char*)o->hwaccels[i].specifier;
						if ((ret = check_stream_specifier(ic, st, (const char*)spec)) > 0)
							hwaccel = (const char*)o->hwaccels[i].u.str; else if (ret < 0)
							Stop();
					}
				};
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
							Log("Unrecognized hwaccel: %s.\n",
								hwaccel);
							Log("Supported hwaccels: ");
							type = AV_HWDEVICE_TYPE_NONE;
							while ((type = av_hwdevice_iterate_types(type)) !=
								AV_HWDEVICE_TYPE_NONE)
								Log("%s ",
									av_hwdevice_get_type_name(type));
							for (i = 0; hwaccels[i].name; i++)
								Log("%s ", hwaccels[i].name);
							Log("\n");
							Stop();
						}
					}
				}
				{
					int i, ret; for (i = 0; i < o->nb_hwaccel_devices; i++) {
						char* spec = (char*)o->hwaccel_devices[i].specifier;
						if ((ret = check_stream_specifier(ic, st, (const char*)spec)) > 0)
							hwaccel_device = (char*)o->hwaccel_devices[i].u.str;
						else if (ret < 0) Stop();
					}
				};
				if (hwaccel_device) {
					ist->hwaccel_device = av_strdup(hwaccel_device);
					if (!ist->hwaccel_device)
						Stop();
				}
				{
					int i, ret; for (i = 0; i < o->nb_hwaccel_output_formats; i++) {
						char* spec = (char*)o->hwaccel_output_formats[i].specifier;
						if ((ret = check_stream_specifier(ic, st, (const char*)spec)) > 0)
							hwaccel_output_format = (char*)o->hwaccel_output_formats[i].u.str;
						else if (ret < 0) Stop();
					}
				};
				if (hwaccel_output_format) {
					ist->hwaccel_output_format = av_get_pix_fmt(hwaccel_output_format);
					if (ist->hwaccel_output_format == AV_PIX_FMT_NONE) {
						Log("Unrecognised hwaccel output "
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
				{
					int i, ret;
					for (i = 0; i < o->nb_guess_layout_max; i++) {
						char* spec = (char*)o->guess_layout_max[i].specifier;
						if ((ret = check_stream_specifier(ic, st, (const char*)spec)) > 0)
							ist->guess_layout_max = (int)o->guess_layout_max[i].u.i;
						else if (ret < 0) Stop();
					}
				};
				guess_input_channel_layout(ist);
				break;
			case AVMEDIA_TYPE_DATA:
			case AVMEDIA_TYPE_SUBTITLE: {
				char* canvas_size = NULL;
				if (!ist->dec)
					ist->dec = avcodec_find_decoder(par->codec_id);
				{
					int i, ret;
					for (i = 0; i < o->nb_fix_sub_duration; i++) {
						char* spec = (char*)o->fix_sub_duration[i].specifier;
						if ((ret = check_stream_specifier(ic, st, (const char*)spec)) > 0)
							ist->fix_sub_duration = (int)o->fix_sub_duration[i].u.i;
						else if (ret < 0)
							Stop();
					}
				};
				{
					int i, ret;
					for (i = 0; i < o->nb_canvas_sizes; i++) {
						char* spec = (char*)o->canvas_sizes[i].specifier;
						if ((ret = check_stream_specifier(ic, st, (const char*)spec)) > 0)
							canvas_size = (char*)o->canvas_sizes[i].u.str;
						else if (ret < 0)
							Stop();
					}
				};
				if (canvas_size &&
					av_parse_video_size(&ist->dec_ctx->width, &ist->dec_ctx->height, canvas_size) < 0) {
					Log("Invalid canvas size: %s.\n", canvas_size);
					Stop();
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
				Log("Error initializing the decoder context.\n");
				Stop();
			}
		}
	}
	void dump_attachment(AVStream* st, const char* filename) {
		int ret;
		AVIOContext* out = NULL;
		AVDictionaryEntry* e;
		if (!st->codecpar->extradata_size) {
			Log("No extradata to dump in stream #%d:%d.\n",
				nb_input_files - 1, st->index);
			return;
		}
		if (!*filename && (e = av_dict_get(st->metadata, "filename", NULL, 0)))
			filename = e->value;
		if (!*filename) {
			Log("No filename specified and no 'filename' tag"
				"in stream #%d:%d.\n", nb_input_files - 1, st->index);
			Stop();
		}
		if ((ret = avio_open2(&out, filename, AVIO_FLAG_WRITE, NULL, NULL)) < 0) {
			Log("Could not open file %s for writing.\n",
				filename);
			Stop();
		}
		avio_write(out, st->codecpar->extradata, st->codecpar->extradata_size);
		avio_flush(out);
		avio_close(out);
	}
	static int open_input_file(FfmpegExe* owner, OptionsContext* o, const char* filename) {
		InputFile* f = NULL;
		AVFormatContext* ic = NULL;
		AVInputFormat* file_iformat = NULL;
		int err, i, ret;
		int64_t timestamp;
		AVDictionary* unused_opts = NULL;
		AVDictionaryEntry* e = NULL;
		char* video_codec_name = NULL;
		char* audio_codec_name = NULL;
		char* subtitle_codec_name = NULL;
		char* data_codec_name = NULL;
		int scan_all_pmts_set = 0;
		if (o->stop_time != INT64_MAX && o->recording_time != INT64_MAX) {
			o->stop_time = INT64_MAX;
			owner->Log("-t and -to cannot be used together; using -t.\n");
		}
		if (o->stop_time != INT64_MAX && o->recording_time == INT64_MAX) {
			int64_t start_time = o->start_time == AV_NOPTS_VALUE ? 0 : o->start_time;
			if (o->stop_time <= start_time) {
				owner->Log("-to value smaller than -ss; aborting.\n");
				owner->Stop();
			}
			else {
				o->recording_time = o->stop_time - start_time;
			}
		}
		if (o->format) {
			if (!(file_iformat = av_find_input_format(o->format))) {
				owner->Log("Unknown input format: '%s'\n", o->format);
				owner->Stop();
			}
		}
		if (!strcmp(filename, "-"))
			filename = "pipe:";
		ic = avformat_alloc_context();
		if (!ic) {
			owner->Stop();
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
					(const char*)o->frame_rates[o->nb_frame_rates - 1].u.str, 0);
			}
		}
		if (o->nb_frame_sizes) {
			av_dict_set(&o->g->format_opts, "video_size", (const char*)o->frame_sizes[o->nb_frame_sizes - 1].u.str, 0);
		}
		if (o->nb_frame_pix_fmts)
			av_dict_set(&o->g->format_opts, "pixel_format", (const char*)o->frame_pix_fmts[o->nb_frame_pix_fmts - 1].u.str, 0);
		for (int i = 0; i < o->nb_codec_names; i++) {
			const char* spec = (const char*)o->codec_names[i].specifier;
			if (!strcmp(spec, (const char*)"v"))
				video_codec_name = (char*)o->codec_names[i].u.str;
		}
		for (int i = 0; i < o->nb_codec_names; i++) {
			const char* spec = (const char*)o->codec_names[i].specifier;
			if (!strcmp(spec, (const char*)"a"))
				audio_codec_name = (char*)o->codec_names[i].u.str;
		}
		for (int i = 0; i < o->nb_codec_names; i++) {
			const char* spec = (const char*)o->codec_names[i].specifier;
			if (!strcmp(spec, (const char*)"s"))
				subtitle_codec_name = (char*)o->codec_names[i].u.str;
		}
		for (int i = 0; i < o->nb_codec_names; i++) {
			const char* spec = (const char*)o->codec_names[i].specifier;
			if (!strcmp(spec, (const char*)"d"))
				data_codec_name = (char*)o->codec_names[i].u.str;
		}
		if (video_codec_name)
			ic->video_codec = owner->find_codec_or_die(video_codec_name, AVMEDIA_TYPE_VIDEO, 0);
		if (audio_codec_name)
			ic->audio_codec = owner->find_codec_or_die(audio_codec_name, AVMEDIA_TYPE_AUDIO, 0);
		if (subtitle_codec_name)
			ic->subtitle_codec = owner->find_codec_or_die(subtitle_codec_name, AVMEDIA_TYPE_SUBTITLE, 0);
		if (data_codec_name)
			ic->data_codec = owner->find_codec_or_die(data_codec_name, AVMEDIA_TYPE_DATA, 0);
		ic->video_codec_id = video_codec_name ? ic->video_codec->id : AV_CODEC_ID_NONE;
		ic->audio_codec_id = audio_codec_name ? ic->audio_codec->id : AV_CODEC_ID_NONE;
		ic->subtitle_codec_id = subtitle_codec_name ? ic->subtitle_codec->id : AV_CODEC_ID_NONE;
		ic->data_codec_id = data_codec_name ? ic->data_codec->id : AV_CODEC_ID_NONE;
		ic->flags |= AVFMT_FLAG_NONBLOCK;
		if (o->bitexact)
			ic->flags |= AVFMT_FLAG_BITEXACT;
		if (!av_dict_get(o->g->format_opts, "scan_all_pmts", NULL, AV_DICT_MATCH_CASE)) {
			av_dict_set(&o->g->format_opts, "scan_all_pmts", "1", AV_DICT_DONT_OVERWRITE);
			scan_all_pmts_set = 1;
		}
		err = avformat_open_input(&ic, filename, file_iformat, &o->g->format_opts);
		if (err < 0) {
			if (err == AVERROR_PROTOCOL_NOT_FOUND)
				owner->Log("Did you mean file:%s?\n", filename);
			owner->Stop();
		}
		if (scan_all_pmts_set)
			av_dict_set(&o->g->format_opts, "scan_all_pmts", NULL, AV_DICT_MATCH_CASE);
		owner->remove_avoptions(&o->g->format_opts, o->g->codec_opts);
		owner->assert_avoptions(o->g->format_opts);
		for (i = 0; i < ic->nb_streams; i++)
			owner->choose_decoder(o, ic, ic->streams[i]);
		if (owner->find_stream_info) {
			AVDictionary** opts = owner->setup_find_stream_info_opts(ic, o->g->codec_opts);
			int orig_nb_streams = ic->nb_streams;
			ret = avformat_find_stream_info(ic, opts);
			for (i = 0; i < orig_nb_streams; i++)
				av_dict_free(&opts[i]);
			av_freep(&opts);
			if (ret < 0) {
				owner->Log("%s: could not find codec parameters\n", filename);
				if (ic->nb_streams == 0) {
					avformat_close_input(&ic);
					owner->Stop();
				}
			}
		}
		if (o->start_time != AV_NOPTS_VALUE && o->start_time_eof != AV_NOPTS_VALUE) {
			owner->Log("Cannot use -ss and -sseof both, using -ss for %s\n", filename);
			o->start_time_eof = AV_NOPTS_VALUE;
		}
		if (o->start_time_eof != AV_NOPTS_VALUE) {
			if (o->start_time_eof >= 0) {
				owner->Log("-sseof value must be negative; aborting\n");
				owner->Stop();
			}
			if (ic->duration > 0) {
				o->start_time = o->start_time_eof + ic->duration;
				if (o->start_time < 0) {
					owner->Log("-sseof value seeks to before start of file %s; ignored\n", filename);
					o->start_time = AV_NOPTS_VALUE;
				}
			}
			else
				owner->Log("Cannot use -sseof, duration of %s not known\n", filename);
		}
		timestamp = (o->start_time == AV_NOPTS_VALUE) ? 0 : o->start_time;
		if (!o->seek_timestamp && ic->start_time != AV_NOPTS_VALUE)
			timestamp += ic->start_time;
		if (o->start_time != AV_NOPTS_VALUE) {
			int64_t seek_timestamp = timestamp;
			if (!(ic->iformat->flags & AVFMT_SEEK_TO_PTS)) {
				int dts_heuristic = 0;
				for (i = 0; i < ic->nb_streams; i++) {
					const AVCodecParameters* par = ic->streams[i]->codecpar;
					if (par->video_delay) {
						dts_heuristic = 1;
						break;
					}
				}
				if (dts_heuristic) {
					seek_timestamp -= 3 * AV_TIME_BASE / 23;
				}
			}
			ret = avformat_seek_file(ic, -1, INT64_MIN, seek_timestamp, seek_timestamp, 0);
			if (ret < 0) {
				owner->Log("%s: could not seek to position %0.3f\n",
					filename, (double)timestamp / AV_TIME_BASE);
			}
		}
		owner->add_input_streams(o, ic);
		av_dump_format(ic, owner->nb_input_files, filename, 0);
		owner->input_files = (InputFile**)owner->grow_array(owner->input_files, sizeof(*owner->input_files), &owner->nb_input_files, owner->nb_input_files + 1);
		f = (InputFile*)av_mallocz(sizeof(*f));
		if (!f)
			owner->Stop();
		owner->input_files[owner->nb_input_files - 1] = f;
		f->ctx = ic;
		f->ist_index = owner->nb_input_streams - ic->nb_streams;
		f->start_time = o->start_time;
		f->recording_time = o->recording_time;
		f->input_ts_offset = o->input_ts_offset;
		f->ts_offset = o->input_ts_offset - (owner->copy_ts ? (owner->start_at_zero && ic->start_time != AV_NOPTS_VALUE ? ic->start_time : 0) : timestamp);
		f->nb_streams = ic->nb_streams;
		f->rate_emu = o->rate_emu;
		f->accurate_seek = o->accurate_seek;
		f->loop = o->loop;
		f->duration = 0;
		f->time_base = owner->TimeBase_1_1;

		unused_opts = owner->strip_specifiers(o->g->codec_opts);
		for (i = f->ist_index; i < owner->nb_input_streams; i++) {
			e = NULL;
			while ((e = av_dict_get(owner->input_streams[i]->decoder_opts, "", e,
				AV_DICT_IGNORE_SUFFIX)))
				av_dict_set(&unused_opts, e->key, NULL, 0);
		}
		e = NULL;
		while ((e = av_dict_get(unused_opts, "", e, AV_DICT_IGNORE_SUFFIX))) {
			const AVClass* _class = avcodec_get_class();
			const AVOption* option = av_opt_find(&_class, e->key, NULL, 0,
				AV_OPT_SEARCH_CHILDREN | AV_OPT_SEARCH_FAKE_OBJ);
			const AVClass* fclass = avformat_get_class();
			const AVOption* foption = av_opt_find(&fclass, e->key, NULL, 0,
				AV_OPT_SEARCH_CHILDREN | AV_OPT_SEARCH_FAKE_OBJ);
			if (!option || foption)
				continue;
			if (!(option->flags & AV_OPT_FLAG_DECODING_PARAM)) {
				owner->Log("Codec AVOption %s (%s) specified for "
					"input file #%d (%s) is not a decoding option.\n", e->key,
					option->help ? option->help : "", owner->nb_input_files - 1,
					filename);
				owner->Stop();
			}
		}
		av_dict_free(&unused_opts);
		for (i = 0; i < o->nb_dump_attachment; i++) {
			for (int j = 0; j < ic->nb_streams; j++) {
				AVStream* st = ic->streams[j];
				if (owner->check_stream_specifier(ic, st, o->dump_attachment[i].specifier) == 1)
					owner->dump_attachment(st, (const char*)o->dump_attachment[i].u.str);
			}
		}
		owner->input_stream_potentially_available = 1;
		return 0;
	}
	uint8_t* get_line(AVIOContext* s) {
		AVIOContext* line;
		uint8_t* buf;
		char c;
		if (avio_open_dyn_buf(&line) < 0) {
			Log("Could not alloc buffer for reading preset.\n");
			Stop();
		}
		while ((c = avio_r8(s)) && c != '\n')
			avio_w8(line, c);
		avio_w8(line, 0);
		avio_close_dyn_buf(line, &buf);
		return buf;
	}
	int get_preset_file_2(const char* preset_name, const char* codec_name, AVIOContext** s) {
		int i, ret = -1;
		char filename[1000];
		const char* base[3] = { getenv("AVCONV_DATADIR"),
			getenv("HOME"),
			AVCONV_DATADIR,
		};
		for (i = 0; i < FF_ARRAY_ELEMS(base) && ret < 0; i++) {
			if (!base[i])
				continue;
			if (codec_name) {
				snprintf(filename, sizeof(filename), "%s%s/%s-%s.avpreset", base[i],
					i != 1 ? "" : "/.avconv", codec_name, preset_name);
				ret = avio_open2(s, filename, AVIO_FLAG_READ, NULL, NULL);
			}
			if (ret < 0) {
				snprintf(filename, sizeof(filename), "%s%s/%s.avpreset", base[i],
					i != 1 ? "" : "/.avconv", preset_name);
				ret = avio_open2(s, filename, AVIO_FLAG_READ, NULL, NULL);
			}
		}
		return ret;
	}
	int choose_encoder(OptionsContext* o, AVFormatContext* s, OutputStream* ost) {
		enum AVMediaType type = ost->st->codecpar->codec_type;
		char* codec_name = NULL;
		if (type == AVMEDIA_TYPE_VIDEO || type == AVMEDIA_TYPE_AUDIO || type == AVMEDIA_TYPE_SUBTITLE) {
			{
				int i, ret;
				for (i = 0; i < o->nb_codec_names; i++) {
					char* spec = (char*)o->codec_names[i].specifier;
					if ((ret = check_stream_specifier(s, ost->st, (const char*)spec)) > 0)
						codec_name = (char*)o->codec_names[i].u.str; else if (ret < 0)
						Stop();
				}
			};
			if (!codec_name) {
				ost->st->codecpar->codec_id = av_guess_codec(s->oformat, NULL, s->url,
					NULL, ost->st->codecpar->codec_type);
				ost->enc = avcodec_find_encoder(ost->st->codecpar->codec_id);
				if (!ost->enc) {
					Log("Automatic encoder selection failed for "
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
			ost->stream_copy = 1;
			ost->encoding_needed = 0;
		}
		return 0;
	}
	OutputStream* new_output_stream(OptionsContext* o, AVFormatContext* oc, enum AVMediaType type, int source_index) {
		OutputStream* ost;
		AVStream* st = avformat_new_stream(oc, NULL);
		int idx = oc->nb_streams - 1, ret = 0;
		const char* bsfs = NULL, * time_base = NULL;
		char* next, * codec_tag = NULL;
		double qscale = -1;
		int i;
		if (!st) {
			Log("Could not alloc stream.\n");
			Stop();
		}
		if (oc->nb_streams - 1 < o->nb_streamid_map)
			st->id = o->streamid_map[oc->nb_streams - 1];
		output_streams = (OutputStream**)grow_array(output_streams, sizeof(*output_streams), &nb_output_streams, nb_output_streams + 1);
		if (!(ost = (OutputStream*)av_mallocz(sizeof(*ost))))
			Stop();
		output_streams[nb_output_streams - 1] = ost;
		ost->file_index = nb_output_files - 1;
		ost->index = idx;
		ost->st = st;
		ost->forced_kf_ref_pts = AV_NOPTS_VALUE;
		st->codecpar->codec_type = type;
		ret = choose_encoder(o, oc, ost);
		if (ret < 0) {
			Log("Error selecting an encoder for stream "
				"%d:%d\n", ost->file_index, ost->index);
			Stop();
		}
		ost->enc_ctx = avcodec_alloc_context3(ost->enc);
		if (!ost->enc_ctx) {
			Log("Error allocating the encoding context.\n");
			Stop();
		}
		ost->enc_ctx->codec_type = type;
		ost->ref_par = avcodec_parameters_alloc();
		if (!ost->ref_par) {
			Log("Error allocating the encoding parameters.\n");
			Stop();
		}
		if (ost->enc) {
			AVIOContext* s = NULL;
			char* buf = NULL, * arg = NULL, * preset = NULL;
			ost->encoder_opts = filter_codec_opts(o->g->codec_opts, ost->enc->id, oc, st, ost->enc);
			{
				int i, ret;
				for (i = 0; i < o->nb_presets; i++) {
					char* spec = (char*)o->presets[i].specifier;
					if ((ret = check_stream_specifier(oc, st, (const char*)spec)) > 0)
						preset = (char*)o->presets[i].u.str;
					else if (ret < 0) Stop();
				}
			};
			if (preset && (!(ret = get_preset_file_2(preset, ost->enc->name, &s)))) {
				do {
					buf = (char*)get_line(s);
					if (!buf[0] || buf[0] == '#') {
						av_free(buf);
						continue;
					}
					if (!(arg = strchr(buf, '='))) {
						Log("Invalid line found in the preset file.\n");
						Stop();
					}
					*arg++ = 0;
					av_dict_set(&ost->encoder_opts, buf, arg, AV_DICT_DONT_OVERWRITE);
					av_free(buf);
				} while (!s->eof_reached);
				avio_closep(&s);
			}
			if (ret) {
				Log("Preset %s specified for stream %d:%d, but could not be opened.\n",
					preset, ost->file_index, ost->index);
				Stop();
			}
		}
		else {
			ost->encoder_opts = filter_codec_opts(o->g->codec_opts, AV_CODEC_ID_NONE, oc, st, NULL);
		}
		if (o->bitexact)
			ost->enc_ctx->flags |= AV_CODEC_FLAG_BITEXACT;
		{
			int i, ret;
			for (i = 0; i < o->nb_time_bases; i++) {
				char* spec = (char*)o->time_bases[i].specifier;
				if ((ret = check_stream_specifier(oc, st, (const char*)spec)) > 0)
					time_base = (const char*)o->time_bases[i].u.str;
				else if (ret < 0)
					Stop();
			}
		};
		if (time_base) {
			AVRational q;
			if (av_parse_ratio(&q, time_base, INT_MAX, 0, NULL) < 0 ||
				q.num <= 0 || q.den <= 0) {
				Log("Invalid time base: %s\n", time_base);
				Stop();
			}
			st->time_base = q;
		}
		{
			int i, ret;
			for (i = 0; i < o->nb_enc_time_bases; i++) {
				char* spec = (char*)o->enc_time_bases[i].specifier;
				if ((ret = check_stream_specifier(oc, st, (const char*)spec)) > 0)
					time_base = (const char*)o->enc_time_bases[i].u.str;
				else if (ret < 0)
					Stop();
			}
		};
		if (time_base) {
			AVRational q;
			if (av_parse_ratio(&q, time_base, INT_MAX, 0, NULL) < 0 ||
				q.den <= 0) {
				Log("Invalid time base: %s\n", time_base);
				Stop();
			}
			ost->enc_timebase = q;
		}
		ost->max_frames = INT64_MAX;
		{
			int i, ret; for (i = 0; i < o->nb_max_frames; i++) {
				char* spec = (char*)o->max_frames[i].specifier;
				if ((ret = check_stream_specifier(oc, st, (const char*)spec)) > 0)
					ost->max_frames = (int64_t)o->max_frames[i].u.i64;
				else if (ret < 0) Stop();
			}
		};
		for (i = 0; i < o->nb_max_frames; i++) {
			char* p = o->max_frames[i].specifier;
			if (!*p && type != AVMEDIA_TYPE_VIDEO) {
				Log("Applying unspecific -frames to non video streams, maybe you meant -vframes ?\n");
				break;
			}
		}
		ost->copy_prior_start = -1;
		{
			int i, ret;
			for (i = 0; i < o->nb_copy_prior_start; i++) {
				char* spec = (char*)o->copy_prior_start[i].specifier;
				if ((ret = check_stream_specifier(oc, st, (const char*)spec)) > 0)
					ost->copy_prior_start = (int)o->copy_prior_start[i].u.i;
				else if (ret < 0)
					Stop();
			}
		};
		{
			int i, ret;
			for (i = 0; i < o->nb_bitstream_filters; i++) {
				char* spec = (char*)o->bitstream_filters[i].specifier;
				if ((ret = check_stream_specifier(oc, st, (const char*)spec)) > 0)
					bsfs = (const char*)o->bitstream_filters[i].u.str;
				else if (ret < 0)
					Stop();
			}
		};
		while (bsfs && *bsfs) {
			const AVBitStreamFilter* filter;
			char* bsf, * bsf_options_str, * bsf_name;
			bsf = av_get_token(&bsfs, ",");
			if (!bsf)
				Stop();
			bsf_name = av_strtok(bsf, "=", &bsf_options_str);
			if (!bsf_name)
				Stop();
			filter = av_bsf_get_by_name(bsf_name);
			if (!filter) {
				Log("Unknown bitstream filter %s\n", bsf_name);
				Stop();
			}
			ost->bsf_ctx = (AVBSFContext**)av_realloc_array(ost->bsf_ctx,
				ost->nb_bitstream_filters + 1,
				sizeof(*ost->bsf_ctx));
			if (!ost->bsf_ctx)
				Stop();
			ret = av_bsf_alloc(filter, &ost->bsf_ctx[ost->nb_bitstream_filters]);
			if (ret < 0) {
				Log("Error allocating a bitstream filter context\n");
				Stop();
			}
			ost->nb_bitstream_filters++;
			if (bsf_options_str && filter->priv_class) {
				const AVOption* opt = av_opt_next(ost->bsf_ctx[ost->nb_bitstream_filters - 1]->priv_data, NULL);
				const char* shorthand[2] = { NULL };
				if (opt)
					shorthand[0] = opt->name;
				ret = av_opt_set_from_string(ost->bsf_ctx[ost->nb_bitstream_filters - 1]->priv_data, bsf_options_str, shorthand, "=", ":");
				if (ret < 0) {
					Log("Error parsing options for bitstream filter %s\n", bsf_name);
					Stop();
				}
			}
			av_freep(&bsf);
			if (*bsfs)
				bsfs++;
		}
		{
			int i, ret;
			for (i = 0; i < o->nb_codec_tags; i++) {
				char* spec = (char*)o->codec_tags[i].specifier;
				if ((ret = check_stream_specifier(oc, st, (const char*)spec)) > 0)
					codec_tag = (char*)o->codec_tags[i].u.str;
				else if (ret < 0)
					Stop();
			}
		};
		if (codec_tag) {
			uint32_t tag = strtol(codec_tag, &next, 0);
			if (*next)
				tag = AV_RL32(codec_tag);
			ost->st->codecpar->codec_tag =
				ost->enc_ctx->codec_tag = tag;
		}
		{
			int i, ret;
			for (i = 0; i < o->nb_qscale; i++) {
				char* spec = (char*)o->qscale[i].specifier;
				if ((ret = check_stream_specifier(oc, st, (const char*)spec)) > 0)
					qscale = (double)o->qscale[i].u.dbl;
				else if (ret < 0)
					Stop();
			}
		};
		if (qscale >= 0) {
			ost->enc_ctx->flags |= AV_CODEC_FLAG_QSCALE;
			ost->enc_ctx->global_quality = FF_QP2LAMBDA * qscale;
		}
		{
			int i, ret;
			for (i = 0; i < o->nb_disposition; i++) {
				char* spec = (char*)o->disposition[i].specifier;
				if ((ret = check_stream_specifier(oc, st, (const char*)spec)) > 0)
					ost->disposition = (char*)o->disposition[i].u.str;
				else if (ret < 0)
					Stop();
			}
		};
		ost->disposition = av_strdup(ost->disposition);
		ost->max_muxing_queue_size = 128;
		{
			int i, ret;
			for (i = 0; i < o->nb_max_muxing_queue_size; i++) {
				char* spec = (char*)o->max_muxing_queue_size[i].specifier;
				if ((ret = check_stream_specifier(oc, st, (const char*)spec)) > 0)
					ost->max_muxing_queue_size = (int)o->max_muxing_queue_size[i].u.i;
				else if (ret < 0)
					Stop();
			}
		};
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
			input_streams[source_index]->st->discard = (enum AVDiscard)input_streams[source_index]->user_set_discard;
		}
		ost->last_mux_dts = AV_NOPTS_VALUE;
		ost->muxing_queue = av_fifo_alloc(8 * sizeof(AVPacket));
		if (!ost->muxing_queue)
			Stop();
		return ost;
	}
	void parse_matrix_coeffs(uint16_t* dest, const char* str) {
		int i;
		const char* p = str;
		for (i = 0;; i++) {
			dest[i] = atoi(p);
			if (i == 63)
				break;
			p = strchr(p, ',');
			if (!p) {
				Log("Syntax error in matrix \"%s\" at coeff %d\n", str, i);
				Stop();
			}
			p++;
		}
	}
	uint8_t* read_file(const char* filename) {
		AVIOContext* pb = NULL;
		AVIOContext* dyn_buf = NULL;
		int ret = avio_open(&pb, filename, AVIO_FLAG_READ);
		uint8_t buf[1024], * str;
		if (ret < 0) {
			Log("Error opening file %s.\n", filename);
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
	char* get_ost_filters(OptionsContext* o, AVFormatContext* oc, OutputStream* ost) {
		AVStream* st = ost->st;
		if (ost->filters_script && ost->filters) {
			Log("Both -filter and -filter_script set for "
				"output stream #%d:%d.\n", nb_output_files, st->index);
			Stop();
		}
		if (ost->filters_script)
			return (char*)read_file(ost->filters_script);
		else if (ost->filters)
			return av_strdup(ost->filters);
		return av_strdup(st->codecpar->codec_type == AVMEDIA_TYPE_VIDEO ?
			"null" : "anull");
	}
	void check_streamcopy_filters(OptionsContext* o, AVFormatContext* oc, const OutputStream* ost, enum AVMediaType type) {
		if (ost->filters_script || ost->filters) {
			Log(
				"%s '%s' was defined for %s output stream %d:%d but codec copy was selected.\n"
				"Filtering and streamcopy cannot be used together.\n",
				ost->filters ? "Filtergraph" : "Filtergraph script",
				ost->filters ? ost->filters : ost->filters_script,
				av_get_media_type_string(type), ost->file_index, ost->index);
			Stop();
		}
	}
	OutputStream* new_video_stream(OptionsContext* o, AVFormatContext* oc, int source_index)
	{
		AVStream* st;
		OutputStream* ost;
		AVCodecContext* video_enc;
		char* frame_rate = NULL, * frame_aspect_ratio = NULL;
		ost = new_output_stream(o, oc, AVMEDIA_TYPE_VIDEO, source_index);
		st = ost->st;
		video_enc = ost->enc_ctx;
		{
			int i, ret;
			for (i = 0; i < o->nb_frame_rates; i++) {
				char* spec = (char*)o->frame_rates[i].specifier;
				if ((ret = check_stream_specifier(oc, st, (const char*)spec)) > 0)
					frame_rate = (char*)o->frame_rates[i].u.str;
				else if (ret < 0)
					Stop();
			}
		};
		if (frame_rate && av_parse_video_rate(&ost->frame_rate, frame_rate) < 0) {
			Log("Invalid framerate value: %s\n", frame_rate);
			Stop();
		}
		if (frame_rate && video_sync_method == VSYNC_PASSTHROUGH)
			Log("Using -vsync 0 and -r can produce invalid output files\n");
		{
			int i, ret;
			for (i = 0; i < o->nb_frame_aspect_ratios; i++) {
				char* spec = (char*)o->frame_aspect_ratios[i].specifier;
				if ((ret = check_stream_specifier(oc, st, (const char*)spec)) > 0)
					frame_aspect_ratio = (char*)o->frame_aspect_ratios[i].u.str;
				else if (ret < 0) Stop();
			}
		};
		if (frame_aspect_ratio) {
			AVRational q;
			if (av_parse_ratio(&q, frame_aspect_ratio, 255, 0, NULL) < 0 ||
				q.num <= 0 || q.den <= 0) {
				Log("Invalid aspect ratio: %s\n", frame_aspect_ratio);
				Stop();
			}
			ost->frame_aspect_ratio = q;
		}
		{
			int i, ret;
			for (i = 0; i < o->nb_filter_scripts; i++) {
				char* spec = (char*)o->filter_scripts[i].specifier;
				if ((ret = check_stream_specifier(oc, st, (const char*)spec)) > 0)
					ost->filters_script = (char*)o->filter_scripts[i].u.str;
				else if (ret < 0) Stop();
			}
		};
		{
			int i, ret;
			for (i = 0; i < o->nb_filters; i++) {
				char* spec = (char*)o->filters[i].specifier;
				if ((ret = check_stream_specifier(oc, st, (const char*)spec)) > 0)
					ost->filters = (char*)o->filters[i].u.str; else if (ret < 0) Stop();
			}
		};
		if (o->nb_filters > 1)
			Log("Only '-vf %s' read, ignoring remaining -vf options: Use ',' to separate filters\n", ost->filters);
		if (!ost->stream_copy) {
			const char* p = NULL;
			char* frame_size = NULL;
			char* frame_pix_fmt = NULL;
			char* intra_matrix = NULL, * inter_matrix = NULL;
			char* chroma_intra_matrix = NULL;
			int do_pass = 0;
			int i;
			{
				int i, ret; for (i = 0; i < o->nb_frame_sizes; i++) {
					char* spec = (char*)o->frame_sizes[i].specifier;
					if ((ret = check_stream_specifier(oc, st, (const char*)spec)) > 0)
						frame_size = (char*)o->frame_sizes[i].u.str;
					else if (ret < 0) Stop();
				}
			};
			if (frame_size && av_parse_video_size(&video_enc->width, &video_enc->height, frame_size) < 0) {
				Log("Invalid frame size: %s.\n", frame_size);
				Stop();
			}
			video_enc->bits_per_raw_sample = frame_bits_per_raw_sample;
			{
				int i, ret; for (i = 0; i < o->nb_frame_pix_fmts; i++) {
					char* spec = (char*)o->frame_pix_fmts[i].specifier;
					if ((ret = check_stream_specifier(oc, st, (const char*)spec)) > 0)
						frame_pix_fmt = (char*)o->frame_pix_fmts[i].u.str;
					else if (ret < 0)
						Stop();
				}
			};
			if (frame_pix_fmt && *frame_pix_fmt == '+') {
				ost->keep_pix_fmt = 1;
				if (!*++frame_pix_fmt)
					frame_pix_fmt = NULL;
			}
			if (frame_pix_fmt && (video_enc->pix_fmt = av_get_pix_fmt(frame_pix_fmt)) == AV_PIX_FMT_NONE) {
				Log("Unknown pixel format requested: %s.\n", frame_pix_fmt);
				Stop();
			}
			st->sample_aspect_ratio = video_enc->sample_aspect_ratio;
			if (intra_only)
				video_enc->gop_size = 0;
			{
				int i, ret;
				for (i = 0; i < o->nb_intra_matrices; i++) {
					char* spec = (char*)o->intra_matrices[i].specifier;
					if ((ret = check_stream_specifier(oc, st, (const char*)spec)) > 0)
						intra_matrix = (char*)o->intra_matrices[i].u.str;
					else if (ret < 0)
						Stop();
				}
			};
			if (intra_matrix) {
				if (!(video_enc->intra_matrix = (uint16_t*)av_mallocz(sizeof(*video_enc->intra_matrix) * 64))) {
					Log("Could not allocate memory for intra matrix.\n");
					Stop();
				}
				parse_matrix_coeffs(video_enc->intra_matrix, intra_matrix);
			}
			{
				int i, ret;
				for (i = 0; i < o->nb_chroma_intra_matrices; i++) {
					char* spec = (char*)o->chroma_intra_matrices[i].specifier;
					if ((ret = check_stream_specifier(oc, st, (const char*)spec)) > 0)
						chroma_intra_matrix = (char*)o->chroma_intra_matrices[i].u.str;
					else if (ret < 0) Stop();
				}
			};
			if (chroma_intra_matrix) {
				uint16_t* p = (uint16_t*)av_mallocz(sizeof(*video_enc->chroma_intra_matrix) * 64);
				if (!p) {
					Log("Could not allocate memory for intra matrix.\n");
					Stop();
				}
				video_enc->chroma_intra_matrix = p;
				parse_matrix_coeffs(p, chroma_intra_matrix);
			}
			{
				int i, ret;
				for (i = 0; i < o->nb_inter_matrices; i++) {
					char* spec = (char*)o->inter_matrices[i].specifier;
					if ((ret = check_stream_specifier(oc, st, (const char*)spec)) > 0)
						inter_matrix = (char*)o->inter_matrices[i].u.str;
					else if (ret < 0) Stop();
				}
			};
			if (inter_matrix) {
				if (!(video_enc->inter_matrix = (uint16_t*)av_mallocz(sizeof(*video_enc->inter_matrix) * 64))) {
					Log("Could not allocate memory for inter matrix.\n");
					Stop();
				}
				parse_matrix_coeffs(video_enc->inter_matrix, inter_matrix);
			}
			{
				int i, ret;
				for (i = 0; i < o->nb_rc_overrides; i++) {
					char* spec = (char*)o->rc_overrides[i].specifier;
					if ((ret = check_stream_specifier(oc, st, (const char*)spec)) > 0)
						p = (const char*)o->rc_overrides[i].u.str;
					else if (ret < 0)
						Stop();
				}
			};
			for (i = 0; p; i++) {
				int start, end, q;
				int e = sscanf(p, "%d,%d,%d", &start, &end, &q);
				if (e != 3) {
					Log("error parsing rc_override\n");
					Stop();
				}
				video_enc->rc_override = (RcOverride*)av_realloc_array(video_enc->rc_override, i + 1, sizeof(RcOverride));
				if (!video_enc->rc_override) {
					Log("Could not (re)allocate memory for rc_override.\n");
					Stop();
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
			{
				int i, ret;
				for (i = 0; i < o->nb_pass; i++) {
					char* spec = (char*)o->pass[i].specifier;
					if ((ret = check_stream_specifier(oc, st, (const char*)spec)) > 0)
						do_pass = (int)o->pass[i].u.i;
					else if (ret < 0)
						Stop();
				}
			};
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
			{
				int i, ret;
				for (i = 0; i < o->nb_passlogfiles; i++) {
					char* spec = (char*)o->passlogfiles[i].specifier;
					if ((ret = check_stream_specifier(oc, st, (const char*)spec)) > 0)
						ost->logfile_prefix = (char*)o->passlogfiles[i].u.str;
					else if (ret < 0) Stop();
				}
			};
			if (ost->logfile_prefix &&
				!(ost->logfile_prefix = av_strdup(ost->logfile_prefix)))
				Stop();
			if (do_pass) {
				char logfilename[1024];
				FILE* f;
				snprintf(logfilename, sizeof(logfilename), "%s-%d.log",
					ost->logfile_prefix ? ost->logfile_prefix :
					DEFAULT_PASS_LOGFILENAME_PREFIX,
					i);
				if (!strcmp(ost->enc->name, "libx264")) {
					av_dict_set(&ost->encoder_opts, "stats", logfilename, AV_DICT_DONT_OVERWRITE);
				}
				else {
					if (video_enc->flags & AV_CODEC_FLAG_PASS2) {
						char* logbuffer = (char*)read_file(logfilename);
						if (!logbuffer) {
							Log("Error reading log file '%s' for pass-2 encoding\n",
								logfilename);
							Stop();
						}
						video_enc->stats_in = logbuffer;
					}
					if (video_enc->flags & AV_CODEC_FLAG_PASS1) {
						f = av_fopen_utf8(logfilename, "wb");
						if (!f) {
							Log(
								"Cannot write log file '%s' for pass-1 encoding: %s\n",
								logfilename, strerror(errno));
							Stop();
						}
						ost->logfile = f;
					}
				}
			}
			{
				int i, ret;
				for (i = 0; i < o->nb_forced_key_frames; i++) {
					char* spec = (char*)o->forced_key_frames[i].specifier;
					if ((ret = check_stream_specifier(oc, st, (const char*)spec)) > 0)
						ost->forced_keyframes = (char*)o->forced_key_frames[i].u.str;
					else if (ret < 0) Stop();
				}
			};
			if (ost->forced_keyframes)
				ost->forced_keyframes = av_strdup(ost->forced_keyframes);
			{
				int i, ret;
				for (i = 0; i < o->nb_force_fps; i++) {
					char* spec = (char*)o->force_fps[i].specifier;
					if ((ret = check_stream_specifier(oc, st, (const char*)spec)) > 0)
						ost->force_fps = (int)o->force_fps[i].u.i;
					else if (ret < 0) Stop();
				}
			};
			ost->top_field_first = -1;
			{
				int i, ret;
				for (i = 0; i < o->nb_top_field_first; i++) {
					char* spec = (char*)o->top_field_first[i].specifier;
					if ((ret = check_stream_specifier(oc, st, (const char*)spec)) > 0)
						ost->top_field_first = (int)o->top_field_first[i].u.i; else if (ret < 0)
						Stop();
				}
			};
			ost->avfilter = get_ost_filters(o, oc, ost);
			if (!ost->avfilter)
				Stop();
		}
		else {
			{
				int i, ret;
				for (i = 0; i < o->nb_copy_initial_nonkeyframes; i++) {
					char* spec = (char*)o->copy_initial_nonkeyframes[i].specifier;
					if ((ret = check_stream_specifier(oc, st, (const char*)spec)) > 0)
						ost->copy_initial_nonkeyframes = (int)o->copy_initial_nonkeyframes[i].u.i;
					else if (ret < 0)
						Stop();
				}
			};
		}
		if (ost->stream_copy)
			check_streamcopy_filters(o, oc, ost, AVMEDIA_TYPE_VIDEO);
		return ost;
	}
	OutputStream* new_audio_stream(OptionsContext* o, AVFormatContext* oc, int source_index)
	{
		int n;
		AVStream* st;
		OutputStream* ost;
		AVCodecContext* audio_enc;
		ost = new_output_stream(o, oc, AVMEDIA_TYPE_AUDIO, source_index);
		st = ost->st;
		audio_enc = ost->enc_ctx;
		audio_enc->codec_type = AVMEDIA_TYPE_AUDIO;
		{
			int i, ret;
			for (i = 0; i < o->nb_filter_scripts; i++) {
				char* spec = (char*)o->filter_scripts[i].specifier;
				if ((ret = check_stream_specifier(oc, st, (const char*)spec)) > 0)
					ost->filters_script = (char*)o->filter_scripts[i].u.str;
				else if (ret < 0) Stop();
			}
		};
		{
			int i, ret;
			for (i = 0; i < o->nb_filters; i++) {
				char* spec = (char*)o->filters[i].specifier;
				if ((ret = check_stream_specifier(oc, st, (const char*)spec)) > 0)
					ost->filters = (char*)o->filters[i].u.str;
				else if (ret < 0) Stop();
			}
		};
		if (o->nb_filters > 1)
			Log("Only '-af %s' read, ignoring remaining -af options: Use ',' to separate filters\n", ost->filters);
		if (!ost->stream_copy) {
			char* sample_fmt = NULL;
			{
				int i, ret;
				for (i = 0; i < o->nb_audio_channels; i++) {
					char* spec = (char*)o->audio_channels[i].specifier;
					if ((ret = check_stream_specifier(oc, st, (const char*)spec)) > 0)
						audio_enc->channels = (int)o->audio_channels[i].u.i;
					else if (ret < 0) Stop();
				}
			};
			{
				int i, ret;
				for (i = 0; i < o->nb_sample_fmts; i++) {
					char* spec = (char*)o->sample_fmts[i].specifier;
					if ((ret = check_stream_specifier(oc, st, (const char*)spec)) > 0)
						sample_fmt = (char*)o->sample_fmts[i].u.str;
					else if (ret < 0) Stop();
				}
			};
			if (sample_fmt &&
				(audio_enc->sample_fmt = av_get_sample_fmt(sample_fmt)) == AV_SAMPLE_FMT_NONE) {
				Log("Invalid sample format '%s'\n", sample_fmt);
				Stop();
			}
			{
				int i, ret;
				for (i = 0; i < o->nb_audio_sample_rate; i++) {
					char* spec = (char*)o->audio_sample_rate[i].specifier;
					if ((ret = check_stream_specifier(oc, st, (const char*)spec)) > 0)
						audio_enc->sample_rate = (int)o->audio_sample_rate[i].u.i;
					else if (ret < 0)
						Stop();
				}
			};
			{
				int i, ret;
				for (i = 0; i < o->nb_apad; i++) {
					char* spec = (char*)o->apad[i].specifier;
					if ((ret = check_stream_specifier(oc, st, (const char*)spec)) > 0)
						ost->apad = (char*)o->apad[i].u.str;
					else if (ret < 0)
						Stop();
				}
			};
			ost->apad = av_strdup(ost->apad);
			ost->avfilter = get_ost_filters(o, oc, ost);
			if (!ost->avfilter)
				Stop();
			for (n = 0; n < o->nb_audio_channel_maps; n++) {
				AudioChannelMap* map = &o->audio_channel_maps[n];
				if ((map->ofile_idx == -1 || ost->file_index == map->ofile_idx) &&
					(map->ostream_idx == -1 || ost->st->index == map->ostream_idx)) {
					InputStream* ist;
					if (map->channel_idx == -1) {
						ist = NULL;
					}
					else if (ost->source_index < 0) {
						Log("Cannot determine input stream for channel mapping %d.%d\n",
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
							Stop();
						ost->audio_channels_map[ost->audio_channels_mapped++] = map->channel_idx;
					}
				}
			}
		}
		if (ost->stream_copy)
			check_streamcopy_filters(o, oc, ost, AVMEDIA_TYPE_AUDIO);
		return ost;
	}
	OutputStream* new_data_stream(OptionsContext* o, AVFormatContext* oc, int source_index) {
		OutputStream* ost = new_output_stream(o, oc, AVMEDIA_TYPE_DATA, source_index);
		if (!ost->stream_copy) {
			Log("Data stream encoding not supported yet (only streamcopy)\n");
			Stop();
		}
		return ost;
	}
	OutputStream* new_unknown_stream(OptionsContext* o, AVFormatContext* oc, int source_index) {
		OutputStream* ost = new_output_stream(o, oc, AVMEDIA_TYPE_UNKNOWN, source_index);
		if (!ost->stream_copy) {
			Log("Unknown stream encoding not supported yet (only streamcopy)\n");
			Stop();
		}
		return ost;
	}
	OutputStream* new_attachment_stream(OptionsContext* o, AVFormatContext* oc, int source_index) {
		OutputStream* ost = new_output_stream(o, oc, AVMEDIA_TYPE_ATTACHMENT, source_index);
		ost->stream_copy = 1;
		ost->finished = 1;
		return ost;
	}
	OutputStream* new_subtitle_stream(OptionsContext* o, AVFormatContext* oc, int source_index)
	{
		AVStream* st;
		OutputStream* ost;
		AVCodecContext* subtitle_enc;
		ost = new_output_stream(o, oc, AVMEDIA_TYPE_SUBTITLE, source_index);
		st = ost->st;
		subtitle_enc = ost->enc_ctx;
		subtitle_enc->codec_type = AVMEDIA_TYPE_SUBTITLE;
		{
			int i, ret;
			for (i = 0; i < o->nb_copy_initial_nonkeyframes; i++) {
				char* spec = (char*)o->copy_initial_nonkeyframes[i].specifier;
				if ((ret = check_stream_specifier(oc, st, (const char*)spec)) > 0)
					ost->copy_initial_nonkeyframes = (int)o->copy_initial_nonkeyframes[i].u.i;
				else if (ret < 0)
					Stop();
			}
		};
		if (!ost->stream_copy) {
			char* frame_size = NULL;
			{
				int i, ret;
				for (i = 0; i < o->nb_frame_sizes; i++) {
					char* spec = (char*)o->frame_sizes[i].specifier;
					if ((ret = check_stream_specifier(oc, st, (const char*)spec)) > 0)
						frame_size = (char*)o->frame_sizes[i].u.str; else if (ret < 0)
						Stop();
				}
			};
			if (frame_size && av_parse_video_size(&subtitle_enc->width, &subtitle_enc->height, frame_size) < 0) {
				Log("Invalid frame size: %s.\n", frame_size);
				Stop();
			}
		}
		return ost;
	}
	static int opt_streamid(FfmpegExe* owner, OptionsContext* optctx, const char* opt, const char* arg)
	{

		int idx;
		char* p;
		char idx_str[16];
		av_strlcpy(idx_str, arg, sizeof(idx_str));
		p = strchr(idx_str, ':');
		if (!p) {
			owner->Log("Invalid value '%s' for option '%s', required syntax is 'index:value'\n",
				arg, opt);
			owner->Stop();
		}
		*p++ = '\0';
		idx = owner->parse_number_or_die(opt, idx_str, OPT_INT, 0, MAX_STREAMS - 1);
		optctx->streamid_map = (int*)owner->grow_array(optctx->streamid_map, sizeof(*optctx->streamid_map), &optctx->nb_streamid_map, idx + 1);
		optctx->streamid_map[idx] = owner->parse_number_or_die(opt, p, OPT_INT, 0, INT_MAX);
		return 0;
	}
	int copy_chapters(InputFile* ifile, OutputFile* ofile, int copy_metadata)
	{
		AVFormatContext* is = ifile->ctx;
		AVFormatContext* os = ofile->ctx;
		AVChapter** tmp;
		int i;
		tmp = (AVChapter**)av_realloc_f(os->chapters, is->nb_chapters + os->nb_chapters, sizeof(*os->chapters));
		if (!tmp)
		{
			Log("%s %d Memory error", __FUNCTION__, __LINE__); Stop(); return -1;
		}
		os->chapters = tmp;
		for (i = 0; i < is->nb_chapters; i++) {
			AVChapter* in_ch = is->chapters[i], * out_ch;
			int64_t start_time = (ofile->start_time == AV_NOPTS_VALUE) ? 0 : ofile->start_time;
			int64_t ts_off = av_rescale_q(start_time - ifile->ts_offset,
				_AV_TIME_BASE_Q, in_ch->time_base);
			int64_t rt = (ofile->recording_time == INT64_MAX) ? INT64_MAX :
				av_rescale_q(ofile->recording_time, _AV_TIME_BASE_Q, in_ch->time_base);
			if (in_ch->end < ts_off)
				continue;
			if (rt != INT64_MAX && in_ch->start > rt + ts_off)
				break;
			out_ch = (AVChapter*)av_mallocz(sizeof(AVChapter));
			if (!out_ch)
			{
				Log("%s %d Memory error", __FUNCTION__, __LINE__); Stop(); return -1;
			}
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
	void init_output_filter(OutputFilter* ofilter, OptionsContext* o,
		AVFormatContext* oc)
	{
		OutputStream* ost;
		switch (ofilter->type) {
		case AVMEDIA_TYPE_VIDEO: ost = new_video_stream(o, oc, -1); break;
		case AVMEDIA_TYPE_AUDIO: ost = new_audio_stream(o, oc, -1); break;
		default:
			Log("Only video and audio filters are supported "
				"currently.\n");
			Stop();
		}
		ost->source_index = -1;
		ost->filter = ofilter;
		ofilter->ost = ost;
		ofilter->format = -1;
		if (ost->stream_copy) {
			Log("Streamcopy requested for output stream %d:%d, "
				"which is fed from a complex filtergraph. Filtering and streamcopy "
				"cannot be used together.\n", ost->file_index, ost->index);
			Stop();
		}
		if (ost->avfilter && (ost->filters || ost->filters_script)) {
			const char* opt = ost->filters ? "-vf/-af/-filter" : "-filter_script";
			Log("%s '%s' was specified through the %s option "
				"for output stream %d:%d, which is fed from a complex filtergraph.\n"
				"%s and -filter_complex cannot be used together for the same stream.\n",
				ost->filters ? "Filtergraph" : "Filtergraph script",
				ost->filters ? ost->filters : ost->filters_script,
				opt, ost->file_index, ost->index, opt);
			Stop();
		}
		avfilter_inout_free(&ofilter->out_tmp);
	}
	int init_complex_filters(void)
	{
		int i, ret = 0;
		for (i = 0; i < nb_filtergraphs; i++) {
			ret = init_complex_filtergraph(filtergraphs[i]);
			if (ret < 0)
				return ret;
		}
		return 0;
	}
	static int open_output_file(FfmpegExe* owner, OptionsContext* o, const char* filename)
	{
		AVFormatContext* oc;
		int i, j, err;
		OutputFile* of;
		OutputStream* ost;
		InputStream* ist;
		AVDictionary* unused_opts = NULL;
		AVDictionaryEntry* e = NULL;
		int format_flags = 0;
		if (o->stop_time != INT64_MAX && o->recording_time != INT64_MAX) {
			o->stop_time = INT64_MAX;
			owner->Log("-t and -to cannot be used together; using -t.\n");
		}
		if (o->stop_time != INT64_MAX && o->recording_time == INT64_MAX) {
			int64_t start_time = o->start_time == AV_NOPTS_VALUE ? 0 : o->start_time;
			if (o->stop_time <= start_time) {
				owner->Log("-to value smaller than -ss; aborting.\n");
				owner->Stop();
			}
			else {
				o->recording_time = o->stop_time - start_time;
			}
		}
		owner->output_files = (OutputFile**)owner->grow_array(owner->output_files, sizeof(*owner->output_files), &owner->nb_output_files, owner->nb_output_files + 1);
		of = (OutputFile*)av_mallocz(sizeof(*of));
		if (!of)
			owner->Stop();
		owner->output_files[owner->nb_output_files - 1] = of;
		of->ost_index = owner->nb_output_streams;
		of->recording_time = o->recording_time;
		of->start_time = o->start_time;
		of->limit_filesize = o->limit_filesize;
		of->shortest = o->shortest;
		av_dict_copy(&of->opts, o->g->format_opts, 0);
		if (!strcmp(filename, "-"))
			filename = "pipe:";
		err = avformat_alloc_output_context2(&oc, NULL, o->format, filename);
		if (!oc) {
			owner->Stop();
		}
		of->ctx = oc;
		if (o->recording_time != INT64_MAX)
			oc->duration = o->recording_time;
		e = av_dict_get(o->g->format_opts, "fflags", NULL, 0);
		if (e) {
			const AVOption* o = av_opt_find(oc, "fflags", NULL, 0, 0);
			av_opt_eval_flags(oc, o, e->value, &format_flags);
		}
		if (o->bitexact) {
			format_flags |= AVFMT_FLAG_BITEXACT;
			oc->flags |= AVFMT_FLAG_BITEXACT;
		}
		for (i = 0; i < owner->nb_filtergraphs; i++) {
			FilterGraph* fg = owner->filtergraphs[i];
			for (j = 0; j < fg->nb_outputs; j++) {
				OutputFilter* ofilter = fg->outputs[j];
				if (!ofilter->out_tmp || ofilter->out_tmp->name)
					continue;
				switch (ofilter->type) {
				case AVMEDIA_TYPE_VIDEO: o->video_disable = 1; break;
				case AVMEDIA_TYPE_AUDIO: o->audio_disable = 1; break;
				case AVMEDIA_TYPE_SUBTITLE: o->subtitle_disable = 1; break;
				}
				owner->init_output_filter(ofilter, o, oc);
			}
		}
		if (!o->nb_stream_maps) {
			char* subtitle_codec_name = NULL;
			if (!o->video_disable && av_guess_codec(oc->oformat, NULL, filename, NULL, AVMEDIA_TYPE_VIDEO) != AV_CODEC_ID_NONE) {
				int area = 0, idx = -1;
				int qcr = avformat_query_codec(oc->oformat, oc->oformat->video_codec, 0);
				for (i = 0; i < owner->nb_input_streams; i++) {
					int new_area;
					ist = owner->input_streams[i];
					new_area = ist->st->codecpar->width * ist->st->codecpar->height + 100000000 * !!ist->st->codec_info_nb_frames;
					if (ist->user_set_discard == AVDISCARD_ALL)
						continue;
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
					owner->new_video_stream(o, oc, idx);
			}
			if (!o->audio_disable && av_guess_codec(oc->oformat, NULL, filename, NULL, AVMEDIA_TYPE_AUDIO) != AV_CODEC_ID_NONE) {
				int best_score = 0, idx = -1;
				for (i = 0; i < owner->nb_input_streams; i++) {
					int score;
					ist = owner->input_streams[i];
					score = ist->st->codecpar->channels + 100000000 * !!ist->st->codec_info_nb_frames;
					if (ist->user_set_discard == AVDISCARD_ALL)
						continue;
					if (ist->st->codecpar->codec_type == AVMEDIA_TYPE_AUDIO &&
						score > best_score) {
						best_score = score;
						idx = i;
					}
				}
				if (idx >= 0)
					owner->new_audio_stream(o, oc, idx);
			}
			for (int i = 0; i < o->nb_codec_names; i++) {
				const char* spec = (const char*)o->codec_names[i].specifier;
				if (!strcmp(spec, (const char*)"s"))
					subtitle_codec_name = (char*)o->codec_names[i].u.str;
			}
			if (!o->subtitle_disable && (avcodec_find_encoder(oc->oformat->subtitle_codec) || subtitle_codec_name)) {
				for (i = 0; i < owner->nb_input_streams; i++)
					if (owner->input_streams[i]->st->codecpar->codec_type == AVMEDIA_TYPE_SUBTITLE) {
						AVCodecDescriptor const* input_descriptor =
							avcodec_descriptor_get(owner->input_streams[i]->st->codecpar->codec_id);
						AVCodecDescriptor const* output_descriptor = NULL;
						AVCodec const* output_codec =
							avcodec_find_encoder(oc->oformat->subtitle_codec);
						int input_props = 0, output_props = 0;
						if (owner->input_streams[i]->user_set_discard == AVDISCARD_ALL)
							continue;
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
							owner->new_subtitle_stream(o, oc, i);
							break;
						}
					}
			}
			if (!o->data_disable) {
				enum AVCodecID codec_id = av_guess_codec(oc->oformat, NULL, filename, NULL, AVMEDIA_TYPE_DATA);
				for (i = 0; codec_id != AV_CODEC_ID_NONE && i < owner->nb_input_streams; i++) {
					if (owner->input_streams[i]->user_set_discard == AVDISCARD_ALL)
						continue;
					if (owner->input_streams[i]->st->codecpar->codec_type == AVMEDIA_TYPE_DATA
						&& owner->input_streams[i]->st->codecpar->codec_id == codec_id)
						owner->new_data_stream(o, oc, i);
				}
			}
		}
		else {
			for (i = 0; i < o->nb_stream_maps; i++) {
				StreamMap* map = &o->stream_maps[i];
				if (map->disabled)
					continue;
				if (map->linklabel) {
					FilterGraph* fg;
					OutputFilter* ofilter = NULL;
					int j, k;
					for (j = 0; j < owner->nb_filtergraphs; j++) {
						fg = owner->filtergraphs[j];
						for (k = 0; k < fg->nb_outputs; k++) {
							AVFilterInOut* out = fg->outputs[k]->out_tmp;
							if (out && !strcmp(out->name, map->linklabel)) {
								ofilter = fg->outputs[k];
								goto loop_end;
							}
						}
					}
				loop_end:
					if (!ofilter) {
						owner->Log("Output with label '%s' does not exist "
							"in any defined filter graph, or was already used elsewhere.\n", map->linklabel);
						owner->Stop();
					}
					owner->init_output_filter(ofilter, o, oc);
				}
				else {
					int src_idx = owner->input_files[map->file_index]->ist_index + map->stream_index;
					ist = owner->input_streams[owner->input_files[map->file_index]->ist_index + map->stream_index];
					if (ist->user_set_discard == AVDISCARD_ALL) {
						owner->Log("Stream #%d:%d is disabled and cannot be mapped.\n",
							map->file_index, map->stream_index);
						owner->Stop();
					}
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
					case AVMEDIA_TYPE_VIDEO: ost = owner->new_video_stream(o, oc, src_idx); break;
					case AVMEDIA_TYPE_AUDIO: ost = owner->new_audio_stream(o, oc, src_idx); break;
					case AVMEDIA_TYPE_SUBTITLE: ost = owner->new_subtitle_stream(o, oc, src_idx); break;
					case AVMEDIA_TYPE_DATA: ost = owner->new_data_stream(o, oc, src_idx); break;
					case AVMEDIA_TYPE_ATTACHMENT: ost = owner->new_attachment_stream(o, oc, src_idx); break;
					case AVMEDIA_TYPE_UNKNOWN:
						if (owner->copy_unknown_streams) {
							ost = owner->new_unknown_stream(o, oc, src_idx);
							break;
						}
					default:
						owner->Log("Cannot map stream #%d:%d - unsupported type.\n",
							map->file_index, map->stream_index);
						if (!owner->ignore_unknown_streams) {
							owner->Log("If you want unsupported types ignored instead "
								"of failing, please use the -ignore_unknown option\n"
								"If you want them copied, please use -copy_unknown\n");
							owner->Stop();
						}
					}
					if (ost)
						ost->sync_ist = owner->input_streams[owner->input_files[map->sync_file_index]->ist_index
						+ map->sync_stream_index];
				}
			}
		}
		for (i = 0; i < o->nb_attachments; i++) {
			AVIOContext* pb;
			uint8_t* attachment;
			const char* p;
			int64_t len;
			if ((err = avio_open2(&pb, o->attachments[i], AVIO_FLAG_READ, NULL, NULL)) < 0) {
				owner->Log("Could not open attachment file %s.\n",
					o->attachments[i]);
				owner->Stop();
			}
			if ((len = avio_size(pb)) <= 0) {
				owner->Log("Could not get size of the attachment %s.\n",
					o->attachments[i]);
				owner->Stop();
			}
			if (!(attachment = (uint8_t*)av_malloc(len))) {
				owner->Log("Attachment %s too large to fit into memory.\n",
					o->attachments[i]);
				owner->Stop();
			}
			avio_read(pb, attachment, len);
			ost = owner->new_attachment_stream(o, oc, -1);
			ost->stream_copy = 0;
			ost->attachment_filename = o->attachments[i];
			ost->st->codecpar->extradata = attachment;
			ost->st->codecpar->extradata_size = len;
			p = strrchr(o->attachments[i], '/');
			av_dict_set(&ost->st->metadata, "filename", (p && *p) ? p + 1 : o->attachments[i], AV_DICT_DONT_OVERWRITE);
			avio_closep(&pb);
		}
#if FF_API_LAVF_AVCTX
		for (i = owner->nb_output_streams - oc->nb_streams; i < owner->nb_output_streams; i++) {
			AVDictionaryEntry* e;
			ost = owner->output_streams[i];
			if ((ost->stream_copy || ost->attachment_filename)
				&& (e = av_dict_get(o->g->codec_opts, "flags", NULL, AV_DICT_IGNORE_SUFFIX))
				&& (!e->key[5] || owner->check_stream_specifier(oc, ost->st, e->key + 6)))
				if (av_opt_set(ost->st->codec, "flags", e->value, 0) < 0)
					owner->Stop();
		}
#endif
		if (!oc->nb_streams && !(oc->oformat->flags & AVFMT_NOSTREAMS)) {
			av_dump_format(oc, owner->nb_output_files - 1, oc->url, 1);
			owner->Log("Output file #%d does not contain any stream\n", owner->nb_output_files - 1);
			owner->Stop();
		}
		unused_opts = owner->strip_specifiers(o->g->codec_opts);
		for (i = of->ost_index; i < owner->nb_output_streams; i++) {
			e = NULL;
			while ((e = av_dict_get(owner->output_streams[i]->encoder_opts, "", e,
				AV_DICT_IGNORE_SUFFIX)))
				av_dict_set(&unused_opts, e->key, NULL, 0);
		}
		e = NULL;
		while ((e = av_dict_get(unused_opts, "", e, AV_DICT_IGNORE_SUFFIX))) {
			const AVClass* _class = avcodec_get_class();
			const AVOption* option = av_opt_find(&_class, e->key, NULL, 0,
				AV_OPT_SEARCH_CHILDREN | AV_OPT_SEARCH_FAKE_OBJ);
			const AVClass* fclass = avformat_get_class();
			const AVOption* foption = av_opt_find(&fclass, e->key, NULL, 0,
				AV_OPT_SEARCH_CHILDREN | AV_OPT_SEARCH_FAKE_OBJ);
			if (!option || foption)
				continue;
			if (!(option->flags & AV_OPT_FLAG_ENCODING_PARAM)) {
				owner->Log("Codec AVOption %s (%s) specified for "
					"output file #%d (%s) is not an encoding option.\n", e->key,
					option->help ? option->help : "", owner->nb_output_files - 1,
					filename);
				owner->Stop();
			}
			if (!strcmp(e->key, "gop_timecode"))
				continue;
			owner->Log("Codec AVOption %s (%s) specified for "
				"output file #%d (%s) has not been used for any stream. The most "
				"likely reason is either wrong type (e.g. a video option with "
				"no video streams) or that it is a private option of some encoder "
				"which was not actually used for any stream.\n", e->key,
				option->help ? option->help : "", owner->nb_output_files - 1, filename);
		}
		av_dict_free(&unused_opts);
		for (i = of->ost_index; i < owner->nb_output_streams; i++) {
			OutputStream* ost = owner->output_streams[i];
			if (ost->encoding_needed && ost->source_index >= 0) {
				InputStream* ist = (InputStream*)owner->input_streams[ost->source_index];
				ist->decoding_needed |= DECODING_FOR_OST;
				if (ost->st->codecpar->codec_type == AVMEDIA_TYPE_VIDEO ||
					ost->st->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
					err = owner->init_simple_filtergraph(ist, ost);
					if (err < 0) {
						owner->Log(
							"Error initializing a simple filtergraph between streams "
							"%d:%d->%d:%d\n", ist->file_index, ost->source_index,
							owner->nb_output_files - 1, ost->st->index);
						owner->Stop();
					}
				}
			}
			if (ost->filter) {
				OutputFilter* f = ost->filter;
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
						if (!f->formats)
							owner->Stop();
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
						if (!f->formats)
							owner->Stop();
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
						if (!f->sample_rates)
							owner->Stop();
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
						if (!f->channel_layouts)
							owner->Stop();
						memcpy(f->channel_layouts, ost->enc->channel_layouts,
							(count + 1) * sizeof(*f->channel_layouts));
					}
					break;
				}
			}
		}
		if (oc->oformat->flags & AVFMT_NEEDNUMBER) {
			if (!av_filename_number_test(oc->url)) {
				owner->Stop();
			}
		}
		if (!(oc->oformat->flags & AVFMT_NOSTREAMS) && !owner->input_stream_potentially_available) {
			owner->Log("No input streams but output needs an input stream\n");
			owner->Stop();
		}
		if (!(oc->oformat->flags & AVFMT_NOFILE)) {
			if ((err = avio_open2(&oc->pb, filename, AVIO_FLAG_WRITE,
				NULL/*&oc->interrupt_callback*/,
				&of->opts)) < 0) {
				owner->Stop();
			}
		}
		if (o->mux_preload) {
			av_dict_set_int(&of->opts, "preload", o->mux_preload * AV_TIME_BASE, 0);
		}
		oc->max_delay = (int)(o->mux_max_delay * AV_TIME_BASE);
		for (i = 0; i < o->nb_metadata_map; i++) {
			char* p;
			int in_file_index = strtol((const char*)o->metadata_map[i].u.str, &p, 0);
			if (in_file_index >= owner->nb_input_files) {
				owner->Log("Invalid input file index %d while processing metadata maps\n", in_file_index);
				owner->Stop();
			}
			owner->copy_metadata(o->metadata_map[i].specifier, *p ? p + 1 : p, oc,
				in_file_index >= 0 ?
				owner->input_files[in_file_index]->ctx : NULL, o);
		}
		if (o->chapters_input_file >= owner->nb_input_files) {
			if (o->chapters_input_file == INT_MAX) {
				o->chapters_input_file = -1;
				for (i = 0; i < owner->nb_input_files; i++)
					if (owner->input_files[i]->ctx->nb_chapters) {
						o->chapters_input_file = i;
						break;
					}
			}
			else {
				owner->Log("Invalid input file index %d in chapter mapping.\n",
					o->chapters_input_file);
				owner->Stop();
			}
		}
		if (o->chapters_input_file >= 0)
			owner->copy_chapters(owner->input_files[o->chapters_input_file], of,
				!o->metadata_chapters_manual);
		if (!o->metadata_global_manual && owner->nb_input_files) {
			av_dict_copy(&oc->metadata, owner->input_files[0]->ctx->metadata,
				AV_DICT_DONT_OVERWRITE);
			if (o->recording_time != INT64_MAX)
				av_dict_set(&oc->metadata, "duration", NULL, 0);
			av_dict_set(&oc->metadata, "creation_time", NULL, 0);
		}
		if (!o->metadata_streams_manual)
			for (i = of->ost_index; i < owner->nb_output_streams; i++) {
				InputStream* ist;
				if (owner->output_streams[i]->source_index < 0)
					continue;
				ist = owner->input_streams[owner->output_streams[i]->source_index];
				av_dict_copy(&owner->output_streams[i]->st->metadata, ist->st->metadata, AV_DICT_DONT_OVERWRITE);
				if (!owner->output_streams[i]->stream_copy) {
					av_dict_set(&owner->output_streams[i]->st->metadata, "encoder", NULL, 0);
				}
			}
		for (i = 0; i < o->nb_program; i++) {
			const char* p = (const char*)o->program[i].u.str;
			int progid = i + 1;
			AVProgram* program;
			while (*p) {
				const char* p2 = av_get_token(&p, ":");
				const char* to_dealloc = p2;
				char* key;
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
			p = (const char*)o->program[i].u.str;
			while (*p) {
				const char* p2 = av_get_token(&p, ":");
				const char* to_dealloc = p2;
				char* key;
				if (!p2)
					break;
				if (*p) p++;
				key = av_get_token(&p2, "=");
				if (!key) {
					owner->Log("No '=' character in program string %s.\n",
						p2);
					owner->Stop();
				}
				if (!*p2)
					owner->Stop();
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
					owner->Log("Unknown program key %s.\n", key);
					owner->Stop();
				}
				av_freep(&to_dealloc);
				av_freep(&key);
			}
		}
		for (i = 0; i < o->nb_metadata; i++) {
			AVDictionary** m;
			char type, * val;
			const char* stream_spec;
			int index = 0, j, ret = 0;
			val = (char*)strchr((const char*)o->metadata[i].u.str, '=');
			if (!val) {
				owner->Log("No '=' character in metadata string %s.\n",
					o->metadata[i].u.str);
				owner->Stop();
			}
			*val++ = 0;
			owner->parse_meta_type(o->metadata[i].specifier, &type, &index, &stream_spec);
			if (type == 's') {
				for (j = 0; j < oc->nb_streams; j++) {
					ost = owner->output_streams[owner->nb_output_streams - oc->nb_streams + j];
					if ((ret = owner->check_stream_specifier(oc, oc->streams[j], stream_spec)) > 0) {
						if (!strcmp((const char*)o->metadata[i].u.str, "rotate")) {
							char* tail;
							double theta = av_strtod(val, &tail);
							if (!*tail) {
								ost->rotate_overridden = 1;
								ost->rotate_override_value = theta;
							}
						}
						else {
							av_dict_set(&oc->streams[j]->metadata, (const char*)o->metadata[i].u.str, *val ? val : NULL, 0);
						}
					}
					else if (ret < 0)
						owner->Stop();
				}
			}
			else {
				switch (type) {
				case 'g':
					m = &oc->metadata;
					break;
				case 'c':
					if (index < 0 || index >= oc->nb_chapters) {
						owner->Log("Invalid chapter index %d in metadata specifier.\n", index);
						owner->Stop();
					}
					m = &oc->chapters[index]->metadata;
					break;
				case 'p':
					if (index < 0 || index >= oc->nb_programs) {
						owner->Log("Invalid program index %d in metadata specifier.\n", index);
						owner->Stop();
					}
					m = &oc->programs[index]->metadata;
					break;
				default:
					owner->Log("Invalid metadata specifier %s.\n", o->metadata[i].specifier);
					owner->Stop();
				}
				av_dict_set(m, (const char*)o->metadata[i].u.str, *val ? val : NULL, 0);
			}
		}
		return 0;
	}
	static int opt_target(FfmpegExe* owner, OptionsContext* optctx, const char* opt, const char* arg)
	{

		enum VideoTarget norm = UNKNOWN;
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
			if (owner->nb_input_files) {
				int i, j, fr;
				for (j = 0; j < owner->nb_input_files; j++) {
					for (i = 0; i < owner->input_files[j]->nb_streams; i++) {
						AVStream* st = owner->input_files[j]->ctx->streams[i];
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
				owner->Log("Assuming %s for target.\n", norm == PAL ? "PAL" : "NTSC");
		}
		if (norm == UNKNOWN) {
			owner->Log("Could not determine norm (PAL/NTSC/NTSC-Film) for target.\n");
			owner->Log("Please prefix target with \"pal-\", \"ntsc-\" or \"film-\",\n");
			owner->Log("or set a framerate with \"-r xxx\".\n");
			owner->Stop();
		}
		if (!strcmp(arg, "vcd")) {
			opt_video_codec(owner, optctx, "c:v", "mpeg1video");
			opt_audio_codec(owner, optctx, "c:a", "mp2");
			owner->parse_option(optctx, "f", "vcd", owner->ffmpeg_options);
			owner->parse_option(optctx, "s", norm == PAL ? "352x288" : "352x240", owner->ffmpeg_options);
			owner->parse_option(optctx, "r", owner->frame_rates[norm], owner->ffmpeg_options);
			opt_default(owner, NULL, "g", norm == PAL ? "15" : "18");
			opt_default(owner, NULL, "b:v", "1150000");
			opt_default(owner, NULL, "maxrate:v", "1150000");
			opt_default(owner, NULL, "minrate:v", "1150000");
			opt_default(owner, NULL, "bufsize:v", "327680");
			opt_default(owner, NULL, "b:a", "224000");
			owner->parse_option(optctx, "ar", "44100", owner->ffmpeg_options);
			owner->parse_option(optctx, "ac", "2", owner->ffmpeg_options);
			opt_default(owner, NULL, "packetsize", "2324");
			opt_default(owner, NULL, "muxrate", "1411200");
			optctx->mux_preload = (36000 + 3 * 1200) / 90000.0;
		}
		else if (!strcmp(arg, "svcd")) {
			opt_video_codec(owner, optctx, "c:v", "mpeg2video");
			opt_audio_codec(owner, optctx, "c:a", "mp2");
			owner->parse_option(optctx, "f", "svcd", owner->ffmpeg_options);
			owner->parse_option(optctx, "s", norm == PAL ? "480x576" : "480x480", owner->ffmpeg_options);
			owner->parse_option(optctx, "r", owner->frame_rates[norm], owner->ffmpeg_options);
			owner->parse_option(optctx, "pix_fmt", "yuv420p", owner->ffmpeg_options);
			opt_default(owner, NULL, "g", norm == PAL ? "15" : "18");
			opt_default(owner, NULL, "b:v", "2040000");
			opt_default(owner, NULL, "maxrate:v", "2516000");
			opt_default(owner, NULL, "minrate:v", "0");
			opt_default(owner, NULL, "bufsize:v", "1835008");
			opt_default(owner, NULL, "scan_offset", "1");
			opt_default(owner, NULL, "b:a", "224000");
			owner->parse_option(optctx, "ar", "44100", owner->ffmpeg_options);
			opt_default(owner, NULL, "packetsize", "2324");
		}
		else if (!strcmp(arg, "dvd")) {
			opt_video_codec(owner, optctx, "c:v", "mpeg2video");
			opt_audio_codec(owner, optctx, "c:a", "ac3");
			owner->parse_option(optctx, "f", "dvd", owner->ffmpeg_options);
			owner->parse_option(optctx, "s", norm == PAL ? "720x576" : "720x480", owner->ffmpeg_options);
			owner->parse_option(optctx, "r", owner->frame_rates[norm], owner->ffmpeg_options);
			owner->parse_option(optctx, "pix_fmt", "yuv420p", owner->ffmpeg_options);
			opt_default(owner, NULL, "g", norm == PAL ? "15" : "18");
			opt_default(owner, NULL, "b:v", "6000000");
			opt_default(owner, NULL, "maxrate:v", "9000000");
			opt_default(owner, NULL, "minrate:v", "0");
			opt_default(owner, NULL, "bufsize:v", "1835008");
			opt_default(owner, NULL, "packetsize", "2048");
			opt_default(owner, NULL, "muxrate", "10080000");
			opt_default(owner, NULL, "b:a", "448000");
			owner->parse_option(optctx, "ar", "48000", owner->ffmpeg_options);
		}
		else if (!strncmp(arg, "dv", 2)) {
			owner->parse_option(optctx, "f", "dv", owner->ffmpeg_options);
			owner->parse_option(optctx, "s", norm == PAL ? "720x576" : "720x480", owner->ffmpeg_options);
			owner->parse_option(optctx, "pix_fmt", !strncmp(arg, "dv50", 4) ? "yuv422p" :
				norm == PAL ? "yuv420p" : "yuv411p", owner->ffmpeg_options);
			owner->parse_option(optctx, "r", owner->frame_rates[norm], owner->ffmpeg_options);
			owner->parse_option(optctx, "ar", "48000", owner->ffmpeg_options);
			owner->parse_option(optctx, "ac", "2", owner->ffmpeg_options);
		}
		else {
			owner->Log("Unknown target: %s\n", arg);
			return AVERROR(EINVAL);
		}
		av_dict_copy(&optctx->g->codec_opts, owner->codec_opts, AV_DICT_DONT_OVERWRITE);
		av_dict_copy(&optctx->g->format_opts, owner->format_opts, AV_DICT_DONT_OVERWRITE);
		return 0;
	}
	static int opt_vstats_file(FfmpegExe* owner, OptionsContext* optctx, const char* opt, const char* arg)
	{
		return 0;
	}
	static int opt_vstats(FfmpegExe* owner, OptionsContext* optctx, const char* opt, const char* arg)
	{
		return 0;
	}
	static int opt_video_frames(FfmpegExe* owner, OptionsContext* optctx, const char* opt, const char* arg)
	{

		return owner->parse_option(optctx, "frames:v", arg, owner->ffmpeg_options);
	}
	static int opt_audio_frames(FfmpegExe* owner, OptionsContext* optctx, const char* opt, const char* arg)
	{

		return owner->parse_option(optctx, "frames:a", arg, owner->ffmpeg_options);
	}
	static int opt_data_frames(FfmpegExe* owner, OptionsContext* optctx, const char* opt, const char* arg)
	{

		return owner->parse_option(optctx, "frames:d", arg, owner->ffmpeg_options);
	}
	static int opt_default_new(FfmpegExe* owner, OptionsContext* o, const char* opt, const char* arg)
	{
		int ret;
		AVDictionary* cbak = owner->codec_opts;
		AVDictionary* fbak = owner->format_opts;
		owner->codec_opts = NULL;
		owner->format_opts = NULL;
		ret = opt_default(owner, NULL, opt, arg);
		av_dict_copy(&o->g->codec_opts, owner->codec_opts, 0);
		av_dict_copy(&o->g->format_opts, owner->format_opts, 0);
		av_dict_free(&owner->codec_opts);
		av_dict_free(&owner->format_opts);
		owner->codec_opts = cbak;
		owner->format_opts = fbak;
		return ret;
	}
	static int opt_preset(FfmpegExe* owner, OptionsContext* optctx, const char* opt, const char* arg)
	{

		FILE* f = NULL;
		char filename[1000], line[1000], tmp_line[1000];
		const char* codec_name = NULL;
		tmp_line[0] = *opt;
		tmp_line[1] = 0;
		for (int i = 0; i < optctx->nb_codec_names; i++) {
			const char* spec = (const char*)optctx->codec_names[i].specifier;
			if (!strcmp(spec, (const char*)tmp_line))
				codec_name = (const char*)optctx->codec_names[i].u.str;
		}
		if (!(f = owner->get_preset_file(filename, sizeof(filename), arg, *opt == 'f', codec_name))) {
			if (!strncmp(arg, "libx264-lossless", strlen("libx264-lossless"))) {
				owner->Log("Please use -preset <speed> -qp 0\n");
			}
			else
				owner->Log("File for preset '%s' not found\n", arg);
			owner->Stop();
		}
		while (fgets(line, sizeof(line), f)) {
			char* key = tmp_line, * value, * endptr;
			if (strcspn(line, "#\n\r") == 0)
				continue;
			av_strlcpy(tmp_line, line, sizeof(tmp_line));
			if (!av_strtok(key, "=", &value) ||
				!av_strtok(value, "\r\n", &endptr)) {
				owner->Log("%s: Invalid syntax: '%s'\n", filename, line);
				owner->Stop();
			}
			owner->Log("ffpreset[%s]: set '%s' = '%s'\n", filename, key, value);
			if (!strcmp(key, "acodec")) opt_audio_codec(owner, optctx, key, value);
			else if (!strcmp(key, "vcodec")) opt_video_codec(owner, optctx, key, value);
			else if (!strcmp(key, "scodec")) opt_subtitle_codec(owner, optctx, key, value);
			else if (!strcmp(key, "dcodec")) opt_data_codec(owner, optctx, key, value);
			else if (opt_default_new(owner, optctx, key, value) < 0) {
				owner->Log("%s: Invalid option or argument: '%s', parsed as '%s' = '%s'\n",
					filename, line, key, value);
				owner->Stop();
			}
		}
		fclose(f);
		return 0;
	}
	static int opt_old2new(FfmpegExe* owner, OptionsContext* optctx, const char* opt, const char* arg) {
		char* s = av_asprintf("%s:%c", opt + 1, *opt);
		int ret = owner->parse_option(optctx, s, arg, owner->ffmpeg_options);
		av_free(s);
		return ret;
	}
	static int opt_bitrate(FfmpegExe* owner, OptionsContext* optctx, const char* opt, const char* arg)
	{
		if (!strcmp(opt, "ab")) {
			av_dict_set(&optctx->g->codec_opts, "b:a", arg, 0);
			return 0;
		}
		else if (!strcmp(opt, "b")) {
			owner->Log("Please use -b:a or -b:v, -b is ambiguous\n");
			av_dict_set(&optctx->g->codec_opts, "b:v", arg, 0);
			return 0;
		}
		av_dict_set(&optctx->g->codec_opts, opt, arg, 0);
		return 0;
	}
	static int opt_qscale(FfmpegExe* owner, OptionsContext* optctx, const char* opt, const char* arg) {
		char* s;
		int ret;
		if (!strcmp(opt, "qscale")) {
			owner->Log("Please use -q:a or -q:v, -qscale is ambiguous\n");
			return owner->parse_option(optctx, "q:v", arg, owner->ffmpeg_options);
		}
		s = av_asprintf("q%s", opt + 6);
		ret = owner->parse_option(optctx, s, arg, owner->ffmpeg_options);
		av_free(s);
		return ret;
	}
	static int opt_profile(FfmpegExe* owner, OptionsContext* optctx, const char* opt, const char* arg) {
		if (!strcmp(opt, "profile")) {
			owner->Log("Please use -profile:a or -profile:v, -profile is ambiguous\n");
			av_dict_set(&optctx->g->codec_opts, "profile:v", arg, 0);
			return 0;
		}
		av_dict_set(&optctx->g->codec_opts, opt, arg, 0);
		return 0;
	}
	static int opt_video_filters(FfmpegExe* owner, OptionsContext* optctx, const char* opt, const char* arg) {
		return owner->parse_option(optctx, "filter:v", arg, owner->ffmpeg_options);
	}
	static int opt_audio_filters(FfmpegExe* owner, OptionsContext* optctx, const char* opt, const char* arg) {
		return owner->parse_option(optctx, "filter:a", arg, owner->ffmpeg_options);
	}
	static int opt_vsync(FfmpegExe* owner, OptionsContext* optctx, const char* opt, const char* arg) {
		if (!av_strcasecmp(arg, "cfr")) owner->video_sync_method = VSYNC_CFR;
		else if (!av_strcasecmp(arg, "vfr"))owner->video_sync_method = VSYNC_VFR;
		else if (!av_strcasecmp(arg, "passthrough")) owner->video_sync_method = VSYNC_PASSTHROUGH;
		else if (!av_strcasecmp(arg, "drop")) owner->video_sync_method = VSYNC_DROP;
		if (owner->video_sync_method == VSYNC_AUTO)
			owner->video_sync_method = owner->parse_number_or_die("vsync", arg, OPT_INT, VSYNC_AUTO, VSYNC_VFR);
		return 0;
	}
	static int opt_timecode(FfmpegExe* owner, OptionsContext* optctx, const char* opt, const char* arg) {
		char* tcr = av_asprintf("timecode=%s", arg);
		int ret = owner->parse_option(optctx, "metadata:g", tcr, owner->ffmpeg_options);
		if (ret >= 0)
			ret = av_dict_set(&optctx->g->codec_opts, "gop_timecode", arg, 0);
		av_free(tcr);
		return ret;
	}
	static int opt_channel_layout(FfmpegExe* owner, OptionsContext* optctx, const char* opt, const char* arg) {
		char layout_str[32];
		char* stream_str;
		char* ac_str;
		int ret, channels, ac_str_size;
		uint64_t layout;
		layout = av_get_channel_layout(arg);
		if (!layout) {
			owner->Log("Unknown channel layout: %s\n", arg);
			return AVERROR(EINVAL);
		}
		snprintf(layout_str, sizeof(layout_str), "%" PRIu64, layout);
		ret = opt_default_new(owner, optctx, opt, layout_str);
		if (ret < 0)
			return ret;
		channels = av_get_channel_layout_nb_channels(layout);
		snprintf(layout_str, sizeof(layout_str), "%d", channels);
		stream_str = (char*)strchr(opt, ':');
		ac_str_size = 3 + (stream_str ? strlen(stream_str) : 0);
		ac_str = (char*)av_mallocz(ac_str_size);
		if (!ac_str) {
			owner->Log("%s %d Memory error", __FUNCTION__, __LINE__);
			owner->Stop();
			return -1;
		}
		av_strlcpy(ac_str, "ac", 3);
		if (stream_str)
			av_strlcat(ac_str, stream_str, ac_str_size);
		ret = owner->parse_option(optctx, ac_str, layout_str, owner->ffmpeg_options);
		av_free(ac_str);
		return ret;
	}
	static int opt_audio_qscale(FfmpegExe* owner, OptionsContext* optctx, const char* opt, const char* arg) {
		return owner->parse_option(optctx, "q:a", arg, owner->ffmpeg_options);
	}
	static int opt_filter_complex(FfmpegExe* owner, OptionsContext* optctx, const char* opt, const char* arg) {
		owner->filtergraphs = (FilterGraph**)owner->grow_array(owner->filtergraphs, sizeof(*owner->filtergraphs), &owner->nb_filtergraphs, owner->nb_filtergraphs + 1);
		if (!(owner->filtergraphs[owner->nb_filtergraphs - 1] = (FilterGraph*)av_mallocz(sizeof(*filtergraphs[0])))) {
			owner->Log("%s %d Memory error", __FUNCTION__, __LINE__);
			owner->Stop();
			return -1;
		}
		owner->filtergraphs[owner->nb_filtergraphs - 1]->index = owner->nb_filtergraphs - 1;
		owner->filtergraphs[owner->nb_filtergraphs - 1]->graph_desc = av_strdup(arg);
		if (!owner->filtergraphs[owner->nb_filtergraphs - 1]->graph_desc) {
			owner->Log("%s %d Memory error", __FUNCTION__, __LINE__);
			owner->Stop();
			return -1;
		}
		owner->input_stream_potentially_available = 1;
		return 0;
	}

	static int opt_filter_complex_script(FfmpegExe* owner, OptionsContext* optctx, const char* opt, const char* arg)
	{
		uint8_t* graph_desc = owner->read_file(arg);
		if (!graph_desc)
			return AVERROR(EINVAL);
		owner->filtergraphs = (FilterGraph**)owner->grow_array(owner->filtergraphs, sizeof(*owner->filtergraphs), &owner->nb_filtergraphs, owner->nb_filtergraphs + 1);
		if (!(owner->filtergraphs[owner->nb_filtergraphs - 1] = (FilterGraph*)av_mallocz(sizeof(*filtergraphs[0])))) {
			owner->Log("%s %d Memory error", __FUNCTION__, __LINE__);
			owner->Stop();
			return -1;
		}
		owner->filtergraphs[owner->nb_filtergraphs - 1]->index = owner->nb_filtergraphs - 1;
		owner->filtergraphs[owner->nb_filtergraphs - 1]->graph_desc = (const char*)graph_desc;
		owner->input_stream_potentially_available = 1;
		return 0;
	}

	int open_files(OptionGroupList* l, const char* inout, int(*open_file)(FfmpegExe*, OptionsContext*, const char*)) {
		int i, ret;
		for (i = 0; i < l->nb_groups; i++) {
			OptionGroup* g = &l->groups[i];
			OptionsContext o;
			o.owner = this;
			o.g = g;
			ret = parse_optgroup(&o, g);
			if (ret < 0) {
				Log("Error parsing options for %s file %s.\n", inout, g->arg);
				return ret;
			}
			Log("Opening an %s file: %s.\n", inout, g->arg);
			ret = open_file(this, &o, g->arg);
			uninit_options(&o);
			if (ret < 0) {
				Log("Error opening %s file %s.\n", inout, g->arg);
				return ret;
			}
			Log("Successfully opened the file.\n");
		}
		return 0;
	}

	int ffmpeg_parse_options(int argc, char** argv) {
		OptionParseContext octx;
		uint8_t error[128];
		int ret;
		memset(&octx, 0, sizeof(octx));
		ret = split_commandline(&octx, argc, argv, ffmpeg_options, OptGroup_groups, FF_ARRAY_ELEMS(OptGroup_groups));
		if (ret < 0) {
			Log("Error splitting the argument list: ");
			goto fail;
		}
		ret = parse_optgroup(NULL, &octx.global_opts);
		if (ret < 0) {
			Log("Error parsing global options: ");
			goto fail;
		}
		ret = open_files(&octx.groups[GROUP_INFILE], "input", open_input_file);
		if (ret < 0) {
			Log("Error opening input files: ");
			goto fail;
		}
		ret = init_complex_filters();
		if (ret < 0) {
			Log("Error initializing complex filters.\n");
			goto fail;
		}
		ret = open_files(&octx.groups[GROUP_OUTFILE], "output", open_output_file);
		if (ret < 0) {
			Log("Error opening output files: ");
			goto fail;
		}
		check_filter_outputs();
	fail:
		uninit_parse_context(&octx);
		if (ret < 0) {
			av_strerror(ret, (char*)error, sizeof(error));
			Log("%s\n", error);
		}
		return ret;
	}
	static int opt_progress(FfmpegExe* owner, OptionsContext* optctx, const char* opt, const char* arg)
	{
		AVIOContext* avio = NULL;
		int ret;
		if (!strcmp(arg, "-"))
			arg = "pipe:";
		ret = avio_open2(&avio, arg, AVIO_FLAG_WRITE, NULL, NULL);
		if (ret < 0) {
			owner->Log("Failed to open progress URL \"%s\": %s\n", arg, owner->_av_err2str(ret));
			return ret;
		}
		owner->progress_avio = avio;
		return 0;
	}

	const enum AVPixelFormat* get_compliance_unofficial_pix_fmts(enum AVCodecID codec_id, const enum AVPixelFormat default_formats[]) {
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

	enum AVPixelFormat choose_pixel_fmt(AVStream* st, AVCodecContext* enc_ctx, AVCodec* codec, enum AVPixelFormat target) {
		if (codec && codec->pix_fmts) {
			const enum AVPixelFormat* p = codec->pix_fmts;
			const AVPixFmtDescriptor* desc = av_pix_fmt_desc_get(target);
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
					Log("Incompatible pixel format '%s' for codec '%s', auto-selecting format '%s'\n",
						av_get_pix_fmt_name(target),
						codec->name,
						av_get_pix_fmt_name(best));
				return best;
			}
		}
		return target;
	}

	void choose_sample_fmt(AVStream* st, AVCodec* codec) {
		if (codec && codec->sample_fmts) {
			const enum AVSampleFormat* p = codec->sample_fmts;
			for (; *p != -1; p++) {
				if (*p == st->codecpar->format)
					break;
			}
			if (*p == -1) {
				if ((codec->capabilities & AV_CODEC_CAP_LOSSLESS) && av_get_sample_fmt_name((enum AVSampleFormat)st->codecpar->format) > av_get_sample_fmt_name(codec->sample_fmts[0]))
					Log("Conversion will not be lossless.\n");
				if (av_get_sample_fmt_name((enum AVSampleFormat)st->codecpar->format))
					Log("Incompatible sample format '%s' for codec '%s', auto-selecting format '%s'\n",
						av_get_sample_fmt_name((enum AVSampleFormat)st->codecpar->format),
						codec->name,
						av_get_sample_fmt_name(codec->sample_fmts[0]));
				st->codecpar->format = codec->sample_fmts[0];
			}
		}
	}

	char* choose_pix_fmts(OutputFilter* ofilter) {
		OutputStream* ost = ofilter->ost;
		AVDictionaryEntry* strict_dict = av_dict_get(ost->encoder_opts, "strict", NULL, 0);
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
			const enum AVPixelFormat* p;
			AVIOContext* s = NULL;
			uint8_t* ret;
			int len;
			if (avio_open_dyn_buf(&s) < 0)
				Stop();
			p = ost->enc->pix_fmts;
			if (ost->enc_ctx->strict_std_compliance <= FF_COMPLIANCE_UNOFFICIAL) {
				p = get_compliance_unofficial_pix_fmts(ost->enc_ctx->codec_id, p);
			}
			for (; *p != AV_PIX_FMT_NONE; p++) {
				const char* name = av_get_pix_fmt_name(*p);
				avio_printf(s, "%s|", name);
			}
			len = avio_close_dyn_buf(s, &ret);
			ret[len - 1] = 0;
			return (char*)ret;
		}
		else
			return NULL;
	}
	char* choose_sample_fmts(OutputFilter* ofilter) {
		if (ofilter->format != AV_SAMPLE_FMT_NONE) {
			const char* name = (const char*)av_get_sample_fmt_name((enum AVSampleFormat)ofilter->format);
			return av_strdup(name);
		}
		else if (ofilter->formats) {
			const enum AVSampleFormat* p;
			AVIOContext* s = NULL;
			uint8_t* ret;
			int len;
			if (avio_open_dyn_buf(&s) < 0)
				Stop();
			for (p = (enum AVSampleFormat*)ofilter->formats; *p != AV_SAMPLE_FMT_NONE; p++) {
				const char* name = (const char*)av_get_sample_fmt_name((enum AVSampleFormat)*p);
				avio_printf(s, "%s|", name);
			}
			len = avio_close_dyn_buf(s, &ret);
			ret[len - 1] = 0;
			return (char*)ret;
		}
		else return NULL;
	}
	char* choose_sample_rates(OutputFilter* ofilter) {
		if (ofilter->sample_rate != 0) {
			char name[16];
			snprintf(name, sizeof(name), "%d", ofilter->sample_rate);
			return av_strdup(name);
		}
		else if (ofilter->sample_rates) {
			const int* p;
			AVIOContext* s = NULL;
			uint8_t* ret;
			int len;
			if (avio_open_dyn_buf(&s) < 0)
				Stop();
			for (p = (int*)ofilter->sample_rates; *p != 0; p++) {
				char name[16];
				snprintf(name, sizeof(name), "%d", *p);
				avio_printf(s, "%s|", name);
			}
			len = avio_close_dyn_buf(s, &ret);
			ret[len - 1] = 0;
			return (char*)ret;
		}
		else return NULL;
	}

	char* choose_channel_layouts(OutputFilter* ofilter) {
		if (ofilter->channel_layout != 0) {
			char name[16];
			snprintf(name, sizeof(name), "0x%" PRIx64, ofilter->channel_layout);
			return av_strdup(name);
		}
		else if (ofilter->channel_layouts) {
			const uint64_t* p;
			AVIOContext* s = NULL;
			uint8_t* ret;
			int len;
			if (avio_open_dyn_buf(&s) < 0)
				Stop();
			for (p = (uint64_t*)ofilter->channel_layouts; *p != 0; p++) {
				char name[16];
				snprintf(name, sizeof(name), "0x%" PRIx64, *p);
				avio_printf(s, "%s|", name);
			}
			len = avio_close_dyn_buf(s, &ret);
			ret[len - 1] = 0;
			return (char*)ret;
		}
		else
			return NULL;
	}

	int init_simple_filtergraph(InputStream* ist, OutputStream* ost) {
		FilterGraph* fg = (FilterGraph*)av_mallocz(sizeof(*fg));
		if (!fg)
			Stop();
		fg->index = nb_filtergraphs;
		fg->outputs = (OutputFilter**)grow_array(fg->outputs, sizeof(*fg->outputs), &fg->nb_outputs, fg->nb_outputs + 1);
		if (!(fg->outputs[0] = (OutputFilter*)av_mallocz(sizeof(*fg->outputs[0]))))
			Stop();
		fg->outputs[0]->ost = ost;
		fg->outputs[0]->graph = fg;
		fg->outputs[0]->format = -1;
		ost->filter = fg->outputs[0];
		fg->inputs = (InputFilter**)grow_array(fg->inputs, sizeof(*fg->inputs), &fg->nb_inputs, fg->nb_inputs + 1);
		if (!(fg->inputs[0] = (InputFilter*)av_mallocz(sizeof(*fg->inputs[0]))))
			Stop();
		fg->inputs[0]->ist = ist;
		fg->inputs[0]->graph = fg;
		fg->inputs[0]->format = -1;
		fg->inputs[0]->frame_queue = av_fifo_alloc(8 * sizeof(AVFrame*));
		if (!fg->inputs[0]->frame_queue)
			Stop();
		ist->filters = (InputFilter**)grow_array(ist->filters, sizeof(*ist->filters), &ist->nb_filters, ist->nb_filters + 1);
		ist->filters[ist->nb_filters - 1] = fg->inputs[0];
		filtergraphs = (FilterGraph**)grow_array(filtergraphs, sizeof(*filtergraphs), &nb_filtergraphs, nb_filtergraphs + 1);
		filtergraphs[nb_filtergraphs - 1] = fg;
		return 0;
	}

	char* describe_filter_link(FilterGraph* fg, AVFilterInOut* inout, int in) {
		AVFilterContext* ctx = inout->filter_ctx;
		AVFilterPad* pads = in ? ctx->input_pads : ctx->output_pads;
		int nb_pads = in ? ctx->nb_inputs : ctx->nb_outputs;
		AVIOContext* pb;
		uint8_t* res = NULL;
		if (avio_open_dyn_buf(&pb) < 0)
			Stop();
		avio_printf(pb, "%s", ctx->filter->name);
		if (nb_pads > 1)
			avio_printf(pb, ":%s", avfilter_pad_get_name(pads, inout->pad_idx));
		avio_w8(pb, 0);
		avio_close_dyn_buf(pb, &res);
		return (char*)res;
	}

	void init_input_filter(FilterGraph* fg, AVFilterInOut* in) {
		InputStream* ist = (InputStream*)NULL;
		enum AVMediaType type = avfilter_pad_get_type(in->filter_ctx->input_pads, in->pad_idx);
		int i;
		if (type != AVMEDIA_TYPE_VIDEO && type != AVMEDIA_TYPE_AUDIO) {
			Log("Only video and audio filters supported "
				"currently.\n");
			Stop();
		}
		if (in->name) {
			AVFormatContext* s;
			AVStream* st = NULL;
			char* p;
			int file_idx = strtol(in->name, &p, 0);
			if (file_idx < 0 || file_idx >= nb_input_files) {
				Log("Invalid file index %d in filtergraph description %s.\n",
					file_idx, fg->graph_desc);
				Stop();
			}
			s = input_files[file_idx]->ctx;
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
				Log("Stream specifier '%s' in filtergraph description %s "
					"matches no streams.\n", p, fg->graph_desc);
				Stop();
			}
			ist = input_streams[input_files[file_idx]->ist_index + st->index];
			if (ist->user_set_discard == AVDISCARD_ALL) {
				Log("Stream specifier '%s' in filtergraph description %s "
					"matches a disabled input stream.\n", p, fg->graph_desc);
				Stop();
			}
		}
		else {
			for (i = 0; i < nb_input_streams; i++) {
				ist = input_streams[i];
				if (ist->user_set_discard == AVDISCARD_ALL)
					continue;
				if (ist->dec_ctx->codec_type == type && ist->discard)
					break;
			}
			if (i == nb_input_streams) {
				Log("Cannot find a matching stream for "
					"unlabeled input pad %d on filter %s\n", in->pad_idx,
					in->filter_ctx->name);
				Stop();
			}
		}
		av_assert0(ist);
		ist->discard = 0;
		ist->decoding_needed |= DECODING_FOR_FILTER;
		ist->st->discard = AVDISCARD_NONE;
		fg->inputs = (InputFilter**)grow_array(fg->inputs, sizeof(*fg->inputs), &fg->nb_inputs, fg->nb_inputs + 1);
		if (!(fg->inputs[fg->nb_inputs - 1] = (InputFilter*)av_mallocz(sizeof(*fg->inputs[0]))))
			Stop();
		fg->inputs[fg->nb_inputs - 1]->ist = ist;
		fg->inputs[fg->nb_inputs - 1]->graph = fg;
		fg->inputs[fg->nb_inputs - 1]->format = -1;
		fg->inputs[fg->nb_inputs - 1]->type = ist->st->codecpar->codec_type;
		fg->inputs[fg->nb_inputs - 1]->name = (uint8_t*)describe_filter_link(fg, in, 1);
		fg->inputs[fg->nb_inputs - 1]->frame_queue = av_fifo_alloc(8 * sizeof(AVFrame*));
		if (!fg->inputs[fg->nb_inputs - 1]->frame_queue)
			Stop();
		ist->filters = (InputFilter**)grow_array(ist->filters, sizeof(*ist->filters), &ist->nb_filters, ist->nb_filters + 1);
		ist->filters[ist->nb_filters - 1] = fg->inputs[fg->nb_inputs - 1];
	}

	int init_complex_filtergraph(FilterGraph* fg) {
		AVFilterInOut* inputs, * outputs, * cur;
		AVFilterGraph* graph;
		int ret = 0;
		graph = avfilter_graph_alloc();
		if (!graph)
		{
			Log("%s %d Memory error", __FUNCTION__, __LINE__); Stop(); return -1;
		}
		graph->nb_threads = 1;
		ret = avfilter_graph_parse2(graph, fg->graph_desc, &inputs, &outputs);
		if (ret < 0)
			goto fail;
		for (cur = inputs; cur; cur = cur->next)
			init_input_filter(fg, cur);
		for (cur = outputs; cur;) {
			fg->outputs = (OutputFilter**)grow_array(fg->outputs, sizeof(*fg->outputs), &fg->nb_outputs, fg->nb_outputs + 1);
			fg->outputs[fg->nb_outputs - 1] = (OutputFilter*)av_mallocz(sizeof(*fg->outputs[0]));
			if (!fg->outputs[fg->nb_outputs - 1])
				Stop();
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

	int insert_trim(int64_t start_time, int64_t duration, AVFilterContext** last_filter, int* pad_idx, const char* filter_name) {
		AVFilterGraph* graph = (*last_filter)->graph;
		AVFilterContext* ctx;
		const AVFilter* trim;
		enum AVMediaType type = avfilter_pad_get_type((*last_filter)->output_pads, *pad_idx);
		const char* name = (type == AVMEDIA_TYPE_VIDEO) ? "trim" : "atrim";
		int ret = 0;
		if (duration == INT64_MAX && start_time == AV_NOPTS_VALUE)
			return 0;
		trim = avfilter_get_by_name(name);
		if (!trim) {
			Log("%s filter not present, cannot limit "
				"recording time.\n", name);
			return AVERROR_FILTER_NOT_FOUND;
		}
		ctx = avfilter_graph_alloc_filter(graph, trim, filter_name);
		if (!ctx)
		{
			Log("%s %d Memory error", __FUNCTION__, __LINE__); Stop(); return -1;
		}
		if (duration != INT64_MAX) {
			ret = av_opt_set_int(ctx, "durationi", duration,
				AV_OPT_SEARCH_CHILDREN);
		}
		if (ret >= 0 && start_time != AV_NOPTS_VALUE) {
			ret = av_opt_set_int(ctx, "starti", start_time,
				AV_OPT_SEARCH_CHILDREN);
		}
		if (ret < 0) {
			Log("Error configuring the %s filter", name);
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
	int insert_filter(AVFilterContext** last_filter, int* pad_idx, const char* filter_name, const char* args) {
		AVFilterGraph* graph = (*last_filter)->graph;
		AVFilterContext* ctx;
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
	int configure_output_video_filter(FilterGraph* fg, OutputFilter* ofilter, AVFilterInOut* out) {
		char* pix_fmts;
		OutputStream* ost = ofilter->ost;
		OutputFile* of = output_files[ost->file_index];
		AVFilterContext* last_filter = out->filter_ctx;
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
			AVFilterContext* filter;
			AVDictionaryEntry* e = NULL;
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
			AVFilterContext* filter;
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
			AVFilterContext* fps;
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
	int configure_output_audio_filter(FilterGraph* fg, OutputFilter* ofilter, AVFilterInOut* out) {
		OutputStream* ost = ofilter->ost;
		OutputFile* of = output_files[ost->file_index];
		AVCodecContext* codec = ost->enc_ctx;
		AVFilterContext* last_filter = out->filter_ctx;
		int pad_idx = out->pad_idx;
		char* sample_fmts, * sample_rates, * channel_layouts;
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
			av_bprintf(&pan_buf, "0x%" PRIx64,
				av_get_default_channel_layout(ost->audio_channels_mapped));
			for (i = 0; i < ost->audio_channels_mapped; i++)
				if (ost->audio_channels_map[i] != -1)
					av_bprintf(&pan_buf, "|c%d=c%d", i, ost->audio_channels_map[i]);
			do {
				AVFilterContext* filt_ctx = NULL;
				Log("-map_channel" " is forwarded to lavfi " "similarly to -af " "pan" "=%s.\n",
					pan_buf.str);
				ret = avfilter_graph_create_filter(&filt_ctx, avfilter_get_by_name("pan"), "pan", pan_buf.str, NULL, fg->graph);
				if (ret < 0)
					return ret;
				ret = avfilter_link(last_filter, pad_idx, filt_ctx, 0);
				if (ret < 0)
					return ret;
				last_filter = filt_ctx; pad_idx = 0;
			} while (0);
			av_bprint_finalize(&pan_buf, NULL);
		}
		if (codec->channels && !codec->channel_layout)
			codec->channel_layout = av_get_default_channel_layout(codec->channels);
		sample_fmts = choose_sample_fmts(ofilter);
		sample_rates = choose_sample_rates(ofilter);
		channel_layouts = choose_channel_layouts(ofilter);
		if (sample_fmts || sample_rates || channel_layouts) {
			AVFilterContext* format;
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
			do {
				AVFilterContext* filt_ctx = NULL;
				Log("-vol" " is forwarded to lavfi " "similarly to -af " "volume" "=%s.\n", args);
				ret = avfilter_graph_create_filter(&filt_ctx, avfilter_get_by_name("volume"), "volume",
					args, NULL, fg->graph);
				if (ret < 0)
					return ret;
				ret = avfilter_link(last_filter, pad_idx, filt_ctx, 0);
				if (ret < 0)
					return ret; last_filter = filt_ctx; pad_idx = 0;
			} while (0);
		}
		if (ost->apad && of->shortest) {
			char args[256];
			int i;
			for (i = 0; i < of->ctx->nb_streams; i++)
				if (of->ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
					break;
			if (i < of->ctx->nb_streams) {
				snprintf(args, sizeof(args), "%s", ost->apad);
				do {
					AVFilterContext* filt_ctx = NULL;
					Log("-apad" " is forwarded to lavfi " "similarly to -af " "apad" "=%s.\n", args);
					ret = avfilter_graph_create_filter(&filt_ctx, avfilter_get_by_name("apad"), "apad", args, NULL, fg->graph);
					if (ret < 0)
						return ret;
					ret = avfilter_link(last_filter, pad_idx, filt_ctx, 0);
					if (ret < 0)
						return ret;
					last_filter = filt_ctx;
					pad_idx = 0;
				} while (0);
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
	int configure_output_filter(FilterGraph* fg, OutputFilter* ofilter, AVFilterInOut* out)
	{
		if (!ofilter->ost) {
			Log("Filter %s has an unconnected output\n", ofilter->name);
			Stop();
		}
		switch (avfilter_pad_get_type(out->filter_ctx->output_pads, out->pad_idx)) {
		case AVMEDIA_TYPE_VIDEO: return configure_output_video_filter(fg, ofilter, out);
		case AVMEDIA_TYPE_AUDIO: return configure_output_audio_filter(fg, ofilter, out);
		default: av_assert0(0);
		}
	}
	void check_filter_outputs(void) {
		for (int i = 0; i < nb_filtergraphs; i++) {
			for (int n = 0; n < filtergraphs[i]->nb_outputs; n++) {
				OutputFilter* output = filtergraphs[i]->outputs[n];
				if (!output->ost) {
					Log("Filter %s has an unconnected output\n", output->name);
					Stop();
				}
			}
		}
	}
	int sub2video_prepare(InputStream* ist, InputFilter* ifilter) {
		AVFormatContext* avf = input_files[ist->file_index]->ctx;
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
			Log("sub2video: using %dx%d canvas\n", w, h);
		}
		ist->sub2video.w = ifilter->width = w;
		ist->sub2video.h = ifilter->height = h;
		ifilter->width = ist->dec_ctx->width ? ist->dec_ctx->width : ist->sub2video.w;
		ifilter->height = ist->dec_ctx->height ? ist->dec_ctx->height : ist->sub2video.h;
		ifilter->format = AV_PIX_FMT_RGB32;
		ist->sub2video.frame = av_frame_alloc();
		if (!ist->sub2video.frame)
		{
			Log("%s %d Memory error", __FUNCTION__, __LINE__); Stop(); return -1;
		}
		ist->sub2video.last_pts = INT64_MIN;
		ist->sub2video.end_pts = INT64_MIN;
		return 0;
	}
	int configure_input_video_filter(FilterGraph* fg, InputFilter* ifilter, AVFilterInOut* in) {
		AVFilterContext* last_filter;
		const AVFilter* buffer_filt = avfilter_get_by_name("buffer");
		InputStream* ist = (InputStream*)ifilter->ist;
		InputFile* f = input_files[ist->file_index];
		AVRational tb = ist->framerate.num ? av_inv_q(ist->framerate) :
			ist->st->time_base;
		AVRational fr = ist->framerate;
		AVRational sar;
		AVBPrint args;
		char name[255];
		int ret, pad_idx = 0;
		int64_t tsoffset = 0;
		AVBufferSrcParameters* par = av_buffersrc_parameters_alloc();
		if (!par)
		{
			Log("%s %d Memory error", __FUNCTION__, __LINE__); Stop(); return -1;
		}
		memset(par, 0, sizeof(*par));
		par->format = AV_PIX_FMT_NONE;
		if (ist->dec_ctx->codec_type == AVMEDIA_TYPE_AUDIO) {
			Log("Cannot connect video filter to audio input\n");
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
			sar = TimeBase_0_1;
		av_bprint_init(&args, 0, AV_BPRINT_SIZE_AUTOMATIC);
		av_bprintf(&args,
			"video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:"
			"pixel_aspect=%d/%d:sws_param=flags=%d",
			ifilter->width, ifilter->height, ifilter->format,
			tb.num, tb.den, sar.num, sar.den,
			SWS_BILINEAR + ((ist->dec_ctx->flags & AV_CODEC_FLAG_BITEXACT) ? SWS_BITEXACT : 0));
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
			AVFilterContext* yadif;
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
	int configure_input_audio_filter(FilterGraph* fg, InputFilter* ifilter, AVFilterInOut* in) {
		AVFilterContext* last_filter;
		const AVFilter* abuffer_filt = avfilter_get_by_name("abuffer");
		InputStream* ist = (InputStream*)ifilter->ist;
		InputFile* f = input_files[ist->file_index];
		AVBPrint args;
		char name[255];
		int ret, pad_idx = 0;
		int64_t tsoffset = 0;
		if (ist->dec_ctx->codec_type != AVMEDIA_TYPE_AUDIO) {
			Log("Cannot connect audio filter to non audio input\n");
			return AVERROR(EINVAL);
		}
		av_bprint_init(&args, 0, AV_BPRINT_SIZE_AUTOMATIC);
		av_bprintf(&args, "time_base=%d/%d:sample_rate=%d:sample_fmt=%s",
			1, ifilter->sample_rate,
			ifilter->sample_rate,
			av_get_sample_fmt_name((enum AVSampleFormat)ifilter->format));
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
		if (audio_sync_method > 0) {
			char args[256] = { 0 };
			av_strlcatf(args, sizeof(args), "async=%d", audio_sync_method);
			if (audio_drift_threshold != 0.1)
				av_strlcatf(args, sizeof(args), ":min_hard_comp=%f", audio_drift_threshold);
			if (!fg->reconfiguration)
				av_strlcatf(args, sizeof(args), ":first_pts=0");
			do {
				AVFilterContext* filt_ctx = NULL;
				Log("-async" " is forwarded to lavfi " "similarly to -af " "aresample" "=%s.\n", args);
				snprintf(name, sizeof(name), "graph_%d_%s_in_%d_%d", fg->index, "aresample", ist->file_index, ist->st->index);
				ret = avfilter_graph_create_filter(&filt_ctx, avfilter_get_by_name("aresample"), name, args, NULL, fg->graph);
				if (ret < 0)
					return ret;
				ret = avfilter_link(last_filter, 0, filt_ctx, 0);
				if (ret < 0)
					return ret;
				last_filter = filt_ctx;
			} while (0);
		}
		if (audio_volume != 256) {
			char args[256];
			Log("-vol has been deprecated. Use the volume  audio filter instead.\n");
			snprintf(args, sizeof(args), "%f", audio_volume / 256.);
			do {
				AVFilterContext* filt_ctx = NULL;
				Log("-vol" " is forwarded to lavfi " "similarly to -af " "volume" "=%s.\n", args);
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
	int configure_input_filter(FilterGraph* fg, InputFilter* ifilter, AVFilterInOut* in) {
		if (!ifilter->ist->dec) {
			Log("No decoder for stream #%d:%d, filtering impossible\n",
				ifilter->ist->file_index, ifilter->ist->st->index);
			return AVERROR_DECODER_NOT_FOUND;
		}
		switch (avfilter_pad_get_type(in->filter_ctx->input_pads, in->pad_idx)) {
		case AVMEDIA_TYPE_VIDEO: return configure_input_video_filter(fg, ifilter, in);
		case AVMEDIA_TYPE_AUDIO: return configure_input_audio_filter(fg, ifilter, in);
		default: av_assert0(0);
		}
	}
	void cleanup_filtergraph(FilterGraph* fg) {
		for (int i = 0; i < fg->nb_outputs; i++)
			fg->outputs[i]->filter = (AVFilterContext*)NULL;
		for (int i = 0; i < fg->nb_inputs; i++)
			fg->inputs[i]->filter = (AVFilterContext*)NULL;
		avfilter_graph_free(&fg->graph);
	}
	int configure_filtergraph(FilterGraph* fg) {
		AVFilterInOut* inputs, * outputs, * cur;
		int ret, i, simple = filtergraph_is_simple(fg);
		const char* graph_desc = simple ? fg->outputs[0]->ost->avfilter :
			fg->graph_desc;
		cleanup_filtergraph(fg);
		if (!(fg->graph = avfilter_graph_alloc()))
		{
			Log("%s %d Memory error", __FUNCTION__, __LINE__); Stop(); return -1;
		}
		if (simple) {
			OutputStream* ost = fg->outputs[0]->ost;
			char args[512];
			AVDictionaryEntry* e = NULL;
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
		if (filter_hw_device || hw_device_ctx) {
			AVBufferRef* device = filter_hw_device ? filter_hw_device->device_ref
				: hw_device_ctx;
			for (i = 0; i < fg->graph->nb_filters; i++) {
				fg->graph->filters[i]->hw_device_ctx = av_buffer_ref(device);
				if (!fg->graph->filters[i]->hw_device_ctx) {
					ret = AVERROR(ENOMEM);
					goto fail;
				}
			}
		}
		if (simple && (!inputs || inputs->next || !outputs || outputs->next)) {
			const char* num_inputs;
			const char* num_outputs;
			if (!outputs) {
				num_outputs = "0";
			}
			else if (outputs->next) {
				num_outputs = ">1";
			}
			else {
				num_outputs = "1";
			}if (!inputs) {
				num_inputs = "0";
			}
			else if (inputs->next) {
				num_inputs = ">1";
			}
			else {
				num_inputs = "1";
			}
			Log("Simple filtergraph '%s' was expected "
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
		for (i = 0; i < fg->nb_outputs; i++) {
			OutputFilter* ofilter = fg->outputs[i];
			AVFilterContext* sink = ofilter->filter;
			ofilter->format = av_buffersink_get_format(sink);
			ofilter->width = av_buffersink_get_w(sink);
			ofilter->height = av_buffersink_get_h(sink);
			ofilter->sample_rate = av_buffersink_get_sample_rate(sink);
			ofilter->channel_layout = av_buffersink_get_channel_layout(sink);
		}
		fg->reconfiguration = 1;
		for (i = 0; i < fg->nb_outputs; i++) {
			OutputStream* ost = fg->outputs[i]->ost;
			if (!ost->enc) {
				Log("Encoder (codec %s) not found for output stream #%d:%d\n",
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
				AVFrame* tmp;
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
			InputStream* ist = (InputStream*)fg->inputs[i]->ist;
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
	int ifilter_parameters_from_frame(InputFilter* ifilter, const AVFrame* frame) {
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
			{
				Log("%s %d Memory error", __FUNCTION__, __LINE__); Stop(); return -1;
			}
		}
		return 0;
	}
	int ist_in_filtergraph(FilterGraph* fg, InputStream* ist) {
		for (int i = 0; i < fg->nb_inputs; i++)
			if (fg->inputs[i]->ist == ist)
				return 1;
		return 0;
	}
	int filtergraph_is_simple(FilterGraph* fg) {
		return !fg->graph_desc;
	}
	HWDevice* hw_device_get_by_type(enum AVHWDeviceType type)
	{
		HWDevice* found = NULL;
		int i;
		for (i = 0; i < nb_hw_devices; i++) {
			if (hw_devices[i]->type == type) {
				if (found)
					return NULL;
				found = hw_devices[i];
			}
		}
		return found;
	}
	HWDevice* hw_device_get_by_name(const char* name)
	{
		int i;
		for (i = 0; i < nb_hw_devices; i++) {
			if (!strcmp(hw_devices[i]->name, name))
				return hw_devices[i];
		}
		return NULL;
	}
	HWDevice* hw_device_add(void)
	{
		int err;
		err = av_reallocp_array(&hw_devices, nb_hw_devices + 1,
			sizeof(*hw_devices));
		if (err) {
			nb_hw_devices = 0;
			return NULL;
		}
		hw_devices[nb_hw_devices] = (HWDevice*)av_mallocz(sizeof(HWDevice));
		if (!hw_devices[nb_hw_devices])
			return NULL;
		return hw_devices[nb_hw_devices++];
	}
	char* hw_device_default_name(enum AVHWDeviceType type)
	{
		const char* type_name = av_hwdevice_get_type_name(type);
		char* name;
		size_t index_pos;
		int index, index_limit = 1000;
		index_pos = strlen(type_name);
		name = (char*)av_malloc(index_pos + 4);
		if (!name)
			return NULL;
		for (index = 0; index < index_limit; index++) {
			snprintf(name, index_pos + 4, "%s%d", type_name, index);
			if (!hw_device_get_by_name(name))
				break;
		}
		if (index >= index_limit) {
			av_freep(&name);
			return NULL;
		}
		return name;
	}
	int hw_device_init_from_string(const char* arg, HWDevice** dev_out)
	{
		AVDictionary* options = NULL;
		char* type_name = NULL, * name = NULL, * device = NULL;
		enum AVHWDeviceType type;
		HWDevice* dev, * src;
		AVBufferRef* device_ref = NULL;
		int err;
		const char* errmsg, * p, * q;
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
			if (hw_device_get_by_name(name)) {
				errmsg = "named device already exists";
				goto invalid;
			}
			p += 1 + k;
		}
		else {
			name = hw_device_default_name(type);
			if (!name) {
				err = AVERROR(ENOMEM);
				goto fail;
			}
		}
		if (!*p) {
			err = av_hwdevice_ctx_create(&device_ref, type,
				NULL, NULL, 0);
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
			src = hw_device_get_by_name(p + 1);
			if (!src) {
				errmsg = "invalid source device name";
				goto invalid;
			}
			err = av_hwdevice_ctx_create_derived(&device_ref, type,
				src->device_ref, 0);
			if (err < 0)
				goto fail;
		}
		else {
			errmsg = "parse error";
			goto invalid;
		}
		dev = hw_device_add();
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
		Log("Invalid device specification \"%s\": %s\n", arg, errmsg);
		err = AVERROR(EINVAL);
		goto done;
	fail:
		Log("Device creation failed: %d.\n", err);
		av_buffer_unref(&device_ref);
		goto done;
	}
	int hw_device_init_from_type(enum AVHWDeviceType type, const char* device, HWDevice** dev_out) {
		AVBufferRef* device_ref = NULL;
		HWDevice* dev;
		char* name;
		int err;
		name = hw_device_default_name(type);
		if (!name) {
			err = AVERROR(ENOMEM);
			goto fail;
		}
		err = av_hwdevice_ctx_create(&device_ref, type, device, NULL, 0);
		if (err < 0) {
			Log("Device creation failed: %d.\n", err);
			goto fail;
		}
		dev = hw_device_add();
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
	void hw_device_free_all(void) {
		int i;
		for (i = 0; i < nb_hw_devices; i++) {
			av_freep(&hw_devices[i]->name);
			av_buffer_unref(&hw_devices[i]->device_ref);
			av_freep(&hw_devices[i]);
		}
		av_freep(&hw_devices);
		nb_hw_devices = 0;
	}
	HWDevice* hw_device_match_by_codec(const AVCodec* codec) {
		const AVCodecHWConfig* config;
		HWDevice* dev;
		int i;
		for (i = 0;; i++) {
			config = avcodec_get_hw_config(codec, i);
			if (!config)
				return NULL;
			if (!(config->methods & AV_CODEC_HW_CONFIG_METHOD_HW_DEVICE_CTX))
				continue;
			dev = hw_device_get_by_type(config->device_type);
			if (dev)
				return dev;
		}
	}
	int hw_device_setup_for_decode(InputStream* ist) {
		const AVCodecHWConfig* config;
		enum AVHWDeviceType type;
		HWDevice* dev = NULL;
		int err, auto_device = 0;
		if (ist->hwaccel_device) {
			dev = hw_device_get_by_name(ist->hwaccel_device);
			if (!dev) {
				if (ist->hwaccel_id == HWACCEL_AUTO) {
					auto_device = 1;
				}
				else if (ist->hwaccel_id == HWACCEL_GENERIC) {
					type = ist->hwaccel_device_type;
					err = hw_device_init_from_type(type, ist->hwaccel_device,
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
					Log("Invalid hwaccel device "
						"specified for decoder: device %s of type %s is not "
						"usable with hwaccel %s.\n", dev->name,
						av_hwdevice_get_type_name(dev->type),
						av_hwdevice_get_type_name(ist->hwaccel_device_type));
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
				dev = hw_device_get_by_type(type);
				if (!dev)
					err = hw_device_init_from_type(type, NULL, &dev);
			}
			else {
				dev = hw_device_match_by_codec(ist->dec);
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
				dev = hw_device_get_by_type(type);
				if (dev) {
					Log("Using auto "
						"hwaccel type %s with existing device %s.\n",
						av_hwdevice_get_type_name(type), dev->name);
				}
			}
			for (i = 0; !dev; i++) {
				config = avcodec_get_hw_config(ist->dec, i);
				if (!config)
					break;
				type = config->device_type;
				err = hw_device_init_from_type(type, ist->hwaccel_device,
					&dev);
				if (err < 0) {
					continue;
				}
				if (ist->hwaccel_device) {
					Log("Using auto "
						"hwaccel type %s with new device created "
						"from %s.\n", av_hwdevice_get_type_name(type),
						ist->hwaccel_device);
				}
				else {
					Log("Using auto "
						"hwaccel type %s with new default device.\n",
						av_hwdevice_get_type_name(type));
				}
			}
			if (dev) {
				ist->hwaccel_device_type = type;
			}
			else {
				Log("Auto hwaccel "
					"disabled: no device found.\n");
				ist->hwaccel_id = HWACCEL_NONE;
				return 0;
			}
		}
		if (!dev) {
			Log("No device available "
				"for decoder: device type %s needed for codec %s.\n",
				av_hwdevice_get_type_name(type), ist->dec->name);
			return err;
		}
		ist->dec_ctx->hw_device_ctx = av_buffer_ref(dev->device_ref);
		if (!ist->dec_ctx->hw_device_ctx)
		{
			Log("%s %d Memory error", __FUNCTION__, __LINE__); Stop(); return -1;
		}
		return 0;
	}
	int hw_device_setup_for_encode(OutputStream* ost) {
		HWDevice* dev;
		dev = hw_device_match_by_codec(ost->enc);
		if (dev) {
			ost->enc_ctx->hw_device_ctx = av_buffer_ref(dev->device_ref);
			if (!ost->enc_ctx->hw_device_ctx)
			{
				Log("%s %d Memory error", __FUNCTION__, __LINE__); Stop(); return -1;
			}
			return 0;
		}
		else {
			return 0;
		}
	}
	static int hwaccel_retrieve_data(AVCodecContext* avctx, AVFrame* input)
	{
		InputStream* ist = (InputStream*)(InputStream*)avctx->opaque;
		AVFrame* output = NULL;
		enum AVPixelFormat output_format = ist->hwaccel_output_format;
		int err;
		if (input->format == output_format) {
			return 0;
		}
		output = av_frame_alloc();
		if (!output) {
			ist->owner->Log("%s %d Memory error", __FUNCTION__, __LINE__);
			ist->owner->Stop();
			return -1;
		}
		output->format = output_format;
		err = av_hwframe_transfer_data(output, input, 0);
		if (err < 0) {
			ist->owner->Log("Failed to transfer data to "
				"output frame: %d.\n", err);
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
	static int hwaccel_decode_init(AVCodecContext* avctx) {
		InputStream* ist = (InputStream*)avctx->opaque;
		ist->hwaccel_retrieve_data = &ist->owner->hwaccel_retrieve_data;
		return 0;
	}

	int sub2video_get_blank_frame(InputStream* ist) {
		int ret;
		AVFrame* frame = ist->sub2video.frame;
		av_frame_unref(frame);
		ist->sub2video.frame->width = ist->dec_ctx->width ? ist->dec_ctx->width : ist->sub2video.w;
		ist->sub2video.frame->height = ist->dec_ctx->height ? ist->dec_ctx->height : ist->sub2video.h;
		ist->sub2video.frame->format = AV_PIX_FMT_RGB32;
		if ((ret = av_frame_get_buffer(frame, 32)) < 0)
			return ret;
		memset(frame->data[0], 0, frame->height * frame->linesize[0]);
		return 0;
	}
	void sub2video_copy_rect(uint8_t* dst, int dst_linesize, int w, int h, AVSubtitleRect* r) {
		uint32_t* pal, * dst2;
		uint8_t* src, * src2;
		int x, y;
		if (r->type != SUBTITLE_BITMAP) {
			Log("sub2video: non-bitmap subtitle\n");
			return;
		}
		if (r->x < 0 || r->x + r->w > w || r->y < 0 || r->y + r->h > h) {
			Log("sub2video: rectangle (%d %d %d %d) overflowing %d %d\n",
				r->x, r->y, r->w, r->h, w, h
			);
			return;
		}
		dst += r->y * dst_linesize + r->x * 4;
		src = r->data[0];
		pal = (uint32_t*)r->data[1];
		for (y = 0; y < r->h; y++) {
			dst2 = (uint32_t*)dst;
			src2 = src;
			for (x = 0; x < r->w; x++)
				*(dst2++) = pal[*(src2++)];
			dst += dst_linesize;
			src += r->linesize[0];
		}
	}
	void sub2video_push_ref(InputStream* ist, int64_t pts)
	{
		AVFrame* frame = ist->sub2video.frame;
		int i;
		int ret;
		av_assert1(frame->data[0]);
		ist->sub2video.last_pts = frame->pts = pts;
		for (i = 0; i < ist->nb_filters; i++) {
			ret = av_buffersrc_add_frame_flags(ist->filters[i]->filter, frame,
				AV_BUFFERSRC_FLAG_KEEP_REF |
				AV_BUFFERSRC_FLAG_PUSH);
			if (ret != AVERROR_EOF && ret < 0)
				Log("Error while add the frame to buffer source(%s).\n",
					_av_err2str(ret));
		}
	}
	void sub2video_update(InputStream* ist, AVSubtitle* sub)
	{
		AVFrame* frame = ist->sub2video.frame;
		int8_t* dst;
		int dst_linesize;
		int num_rects, i;
		int64_t pts, end_pts;
		if (!frame)
			return;
		if (sub) {
			pts = av_rescale_q(sub->pts + sub->start_display_time * 1000LL,
				_AV_TIME_BASE_Q, ist->st->time_base);
			end_pts = av_rescale_q(sub->pts + sub->end_display_time * 1000LL,
				_AV_TIME_BASE_Q, ist->st->time_base);
			num_rects = sub->num_rects;
		}
		else {
			pts = ist->sub2video.end_pts;
			end_pts = INT64_MAX;
			num_rects = 0;
		}
		if (sub2video_get_blank_frame(ist) < 0) {
			Log("Impossible to get a blank canvas.\n");
			return;
		}
		dst = (int8_t*)frame->data[0];
		dst_linesize = frame->linesize[0];
		for (i = 0; i < num_rects; i++)
			sub2video_copy_rect((uint8_t*)dst, dst_linesize, frame->width, frame->height, sub->rects[i]);
		sub2video_push_ref(ist, pts);
		ist->sub2video.end_pts = end_pts;
	}
	void sub2video_heartbeat(InputStream* ist, int64_t pts)
	{
		InputFile* infile = input_files[ist->file_index];
		int i, j, nb_reqs;
		int64_t pts2;
		for (i = 0; i < infile->nb_streams; i++) {
			InputStream* ist2 = input_streams[infile->ist_index + i];
			if (!ist2->sub2video.frame)
				continue;
			pts2 = av_rescale_q(pts, ist->st->time_base, ist2->st->time_base) - 1;
			if (pts2 <= ist2->sub2video.last_pts)
				continue;
			if (pts2 >= ist2->sub2video.end_pts ||
				(!ist2->sub2video.frame->data[0] && ist2->sub2video.end_pts < INT64_MAX))
				sub2video_update(ist2, NULL);
			for (j = 0, nb_reqs = 0; j < ist2->nb_filters; j++)
				nb_reqs += av_buffersrc_get_nb_failed_requests(ist2->filters[j]->filter);
			if (nb_reqs)
				sub2video_push_ref(ist2, pts2);
		}
	}
	void sub2video_flush(InputStream* ist)
	{
		int i;
		int ret;
		if (ist->sub2video.end_pts < INT64_MAX)
			sub2video_update(ist, NULL);
		for (i = 0; i < ist->nb_filters; i++) {
			ret = av_buffersrc_add_frame(ist->filters[i]->filter, NULL);
			if (ret != AVERROR_EOF && ret < 0)
				Log("Flush the frame error.\n");
		}
	}
	void ffmpeg_cleanup() {
		int i, j;
		if (filtergraphs) {
			for (i = 0; i < nb_filtergraphs; i++) {
				FilterGraph* fg = filtergraphs[i];
				if (fg) {
					avfilter_graph_free(&fg->graph);
					for (j = 0; j < fg->nb_inputs; j++) {
						while (av_fifo_size(fg->inputs[j]->frame_queue)) {
							AVFrame* frame;
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

			}
			av_freep(&filtergraphs);
		}

		av_freep(&subtitle_out);
		if (output_files) {
			for (i = 0; i < nb_output_files; i++) {
				OutputFile* of = output_files[i];
				AVFormatContext* s;
				if (!of)
					continue;
				s = of->ctx;
				if (s && s->oformat && !(s->oformat->flags & AVFMT_NOFILE))
					avio_closep(&s->pb);
				avformat_free_context(s);
				av_dict_free(&of->opts);
				av_freep(&output_files[i]);
			}
		}

		if (output_streams) {
			for (i = 0; i < nb_output_streams; i++) {
				OutputStream* ost = output_streams[i];
				if (!ost)
					continue;
				for (j = 0; j < ost->nb_bitstream_filters; j++)
					av_bsf_free(&ost->bsf_ctx[j]);
				av_freep(&ost->bsf_ctx);
				av_frame_free(&ost->filtered_frame);
				av_frame_free(&ost->last_frame);
				av_dict_free(&ost->encoder_opts);
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
		}


		if (input_files) {
			for (i = 0; i < nb_input_files; i++) {
				avformat_close_input(&input_files[i]->ctx);
				av_freep(&input_files[i]);
			}
		}

		if (input_streams) {
			for (i = 0; i < nb_input_streams; i++) {
				InputStream* ist = (InputStream*)input_streams[i];
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
		}

		av_freep(&input_files);
		av_freep(&output_streams);
		av_freep(&output_files);
		uninit_opts();
	}
	void remove_avoptions(AVDictionary** a, AVDictionary* b) {
		AVDictionaryEntry* t = NULL;
		while ((t = av_dict_get(b, "", t, AV_DICT_IGNORE_SUFFIX))) {
			av_dict_set(a, t->key, NULL, AV_DICT_MATCH_CASE);
		}
	}
	void assert_avoptions(AVDictionary* m) {
		AVDictionaryEntry* t;
		if ((t = av_dict_get(m, "", NULL, AV_DICT_IGNORE_SUFFIX))) {
			Log("Option %s not found.\n", t->key);
			Stop();
		}
	}
	void abort_codec_experimental(AVCodec* c, int encoder) {
		Stop();
	}
	void close_all_output_streams(OutputStream* ost, int this_stream, int others) {
		int i;
		for (i = 0; i < nb_output_streams; i++) {
			OutputStream* ost2 = output_streams[i];
			ost2->finished |= ost == ost2 ? this_stream : others;
		}
	}
	void write_packet(OutputFile* of, AVPacket* pkt, OutputStream* ost, int unqueue) {
		AVFormatContext* s = of->ctx;
		AVStream* st = ost->st;
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
				int new_size = FFMAX(2 * av_fifo_size(ost->muxing_queue),
					ost->max_muxing_queue_size);
				if (new_size <= av_fifo_size(ost->muxing_queue)) {
					Log("Too many packets buffered for output stream %d:%d.\n", ost->file_index, ost->st->index);
					Stop();
				}
				ret = av_fifo_realloc2(ost->muxing_queue, new_size);
				if (ret < 0)
					Stop();
			}
			ret = av_packet_make_refcounted(pkt);
			if (ret < 0)
				Stop();
			av_packet_move_ref(&tmp_pkt, pkt);
			av_fifo_generic_write(ost->muxing_queue, &tmp_pkt, sizeof(tmp_pkt), NULL);
			return;
		}
		if ((st->codecpar->codec_type == AVMEDIA_TYPE_VIDEO && video_sync_method == VSYNC_DROP) ||
			(st->codecpar->codec_type == AVMEDIA_TYPE_AUDIO && audio_sync_method < 0))
			pkt->pts = pkt->dts = AV_NOPTS_VALUE;
		if (st->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
			int i;
			uint8_t* sd = av_packet_get_side_data(pkt, AV_PKT_DATA_QUALITY_STATS,
				NULL);
			ost->quality = sd ? AV_RL32(sd) : -1;
			ost->pict_type = sd ? sd[4] : AV_PICTURE_TYPE_NONE;
			for (i = 0; i < FF_ARRAY_ELEMS(ost->error); i++) {
				if (sd && i < sd[5])
					ost->error[i] = AV_RL64(sd + 8 + 8 * i);
				else
					ost->error[i] = -1;
			}
			if (ost->frame_rate.num && ost->is_cfr) {
				//if (pkt->duration > 0)
				//	Log( "Overriding packet duration by frame rate, this should not happen\n");
				pkt->duration = av_rescale_q(1, av_inv_q(ost->frame_rate),
					ost->mux_timebase);
			}
		}
		av_packet_rescale_ts(pkt, ost->mux_timebase, ost->st->time_base);
		if (!(s->oformat->flags & AVFMT_NOTIMESTAMPS)) {
			if (pkt->dts != AV_NOPTS_VALUE &&
				pkt->pts != AV_NOPTS_VALUE &&
				pkt->dts > pkt->pts) {
				//Log( "Invalid DTS: %" PRId64" PTS: %" PRId64" in output stream %d:%d, replacing by guess\n",
				//	pkt->dts, pkt->pts,
				//	ost->file_index, ost->st->index);
				pkt->pts = pkt->dts = pkt->pts + pkt->dts + ost->last_mux_dts + 1
					- FFMIN3(pkt->pts, pkt->dts, ost->last_mux_dts + 1)
					- FFMAX3(pkt->pts, pkt->dts, ost->last_mux_dts + 1);
			}
			if ((st->codecpar->codec_type == AVMEDIA_TYPE_AUDIO || st->codecpar->codec_type == AVMEDIA_TYPE_VIDEO || st->codecpar->codec_type == AVMEDIA_TYPE_SUBTITLE) &&
				pkt->dts != AV_NOPTS_VALUE &&
				!(st->codecpar->codec_id == AV_CODEC_ID_VP9 && ost->stream_copy) &&
				ost->last_mux_dts != AV_NOPTS_VALUE) {
				int64_t max = ost->last_mux_dts + !(s->oformat->flags & AVFMT_TS_NONSTRICT);
				if (pkt->dts < max) {
					//Log( "Non-monotonous DTS in output stream "
					//	"%d:%d; previous: %" PRId64", current: %" PRId64"; ",
					//	ost->file_index, ost->st->index, ost->last_mux_dts, pkt->dts);
					if (exit_on_error) {
						Log("aborting.\n");
						Stop();
					}
					////Log( "changing to %" PRId64". This may result "
					////	"in incorrect timestamps in the output file.\n",
					////	max);
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

		//计算时间戳,单位为秒
		//m_currTime = FFMAX(m_currTime, pkt->pts * ost->mux_timebase.num / ost->mux_timebase.den);//Curr Time

		ret = av_interleaved_write_frame(s, pkt);//输出文件
		if (ret < 0) {
			close_all_output_streams(ost, MUXER_FINISHED | ENCODER_FINISHED, ENCODER_FINISHED);
		}
		av_packet_unref(pkt);
	}
	void close_output_stream(OutputStream* ost) {
		OutputFile* of = output_files[ost->file_index];
		ost->finished |= ENCODER_FINISHED;
		if (of->shortest) {
			int64_t end = av_rescale_q(ost->sync_opts - ost->first_pts, ost->enc_ctx->time_base, _AV_TIME_BASE_Q);
			of->recording_time = FFMIN(of->recording_time, end);
		}
	}
	void output_packet(OutputFile* of, AVPacket* pkt, OutputStream* ost, int eof) {
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
			Log("Error applying bitstream filters to an output "
				"packet for stream #%d:%d.\n", ost->file_index, ost->index);
			if (exit_on_error)
				Stop();
		}
	}
	int check_recording_time(OutputStream* ost)
	{
		OutputFile* of = output_files[ost->file_index];
		if (of->recording_time != INT64_MAX &&
			av_compare_ts(ost->sync_opts - ost->first_pts, ost->enc_ctx->time_base, of->recording_time,
				_AV_TIME_BASE_Q) >= 0) {
			close_output_stream(ost);
			return 0;
		}
		return 1;
	}
	void do_audio_out(OutputFile* of, OutputStream* ost, AVFrame* frame) {
		AVCodecContext* enc = ost->enc_ctx;
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
		Log("Audio encoding failed\n");
		Stop();
	}
	void do_subtitle_out(OutputFile* of, OutputStream* ost, AVSubtitle* sub) {
		int subtitle_out_max_size = 1024 * 1024;
		int subtitle_out_size, nb, i;
		AVCodecContext* enc;
		AVPacket pkt;
		int64_t pts;
		if (sub->pts == AV_NOPTS_VALUE) {
			Log("Subtitle packets must have a pts\n");
			if (exit_on_error)
				Stop();
			return;
		}
		enc = ost->enc_ctx;
		if (!subtitle_out) {
			subtitle_out = (uint8_t*)av_malloc(subtitle_out_max_size);
			if (!subtitle_out) {
				Log("Failed to allocate subtitle_out\n");
				Stop();
			}
		}
		if (enc->codec_id == AV_CODEC_ID_DVB_SUBTITLE)
			nb = 2;
		else
			nb = 1;
		pts = sub->pts;
		if (output_files[ost->file_index]->start_time != AV_NOPTS_VALUE)
			pts -= output_files[ost->file_index]->start_time;
		for (i = 0; i < nb; i++) {
			unsigned save_num_rects = sub->num_rects;
			ost->sync_opts = av_rescale_q(pts, _AV_TIME_BASE_Q, enc->time_base);
			if (!check_recording_time(ost))
				return;
			sub->pts = pts;
			sub->pts += av_rescale_q(sub->start_display_time, TimeBase_1_1000, _AV_TIME_BASE_Q);
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
				Log("Subtitle encoding failed\n");
				Stop();
			}
			av_init_packet(&pkt);
			pkt.data = subtitle_out;
			pkt.size = subtitle_out_size;
			pkt.pts = av_rescale_q(sub->pts, _AV_TIME_BASE_Q, ost->mux_timebase);
			pkt.duration = av_rescale_q(sub->end_display_time, TimeBase_1_1000, ost->mux_timebase);
			if (enc->codec_id == AV_CODEC_ID_DVB_SUBTITLE) {
				if (i == 0)
					pkt.pts += av_rescale_q(sub->start_display_time, TimeBase_1_1000, ost->mux_timebase);
				else
					pkt.pts += av_rescale_q(sub->end_display_time, TimeBase_1_1000, ost->mux_timebase);
			}
			pkt.dts = pkt.pts;
			output_packet(of, &pkt, ost, 0);
		}
	}

	void do_video_out(OutputFile* of, OutputStream* ost, AVFrame* next_picture, double sync_ipts) {
		int ret, format_video_sync;
		AVPacket pkt;
		AVCodecContext* enc = ost->enc_ctx;
		AVCodecParameters* mux_par = ost->st->codecpar;
		AVRational frame_rate;
		int nb_frames, nb0_frames, i;
		double delta, delta0;
		double duration = 0;
		int frame_size = 0;
		InputStream* ist = (InputStream*)NULL;
		AVFilterContext* filter = ost->filter->filter;
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
			nb0_frames = nb_frames = MidPred(ost->last_nb0_frames[0],
				ost->last_nb0_frames[1],
				ost->last_nb0_frames[2]);
		}
		else {
			delta0 = sync_ipts - ost->sync_opts;
			delta = delta0 + duration;
			nb0_frames = 0;
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
				//if (delta0 < -0.6) {
				//	Log("Past duration %f too large\n", -delta0);
				//}
				//else
				//	Log("Clipping frame in rate conversion by %f\n", -delta0);
				sync_ipts = ost->sync_opts;
				duration += delta0;
				delta0 = 0;
			}
			switch (format_video_sync) {
			case VSYNC_VSCFR:
				if (ost->frame_number == 0 && delta0 >= 0.5) {
					//Log("Not duplicating %d initial frames\n", (int)lrintf(delta0));
					delta = duration;
					delta0 = 0;
					ost->sync_opts = lrint(sync_ipts);
				}
			case VSYNC_CFR:
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
		for (i = 0; i < nb_frames; i++) {
			AVFrame* in_picture;
			int forced_keyframe = 0;
			double pts_time;
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
			in_picture->pict_type = (enum AVPictureType)0;
			if (ost->forced_kf_ref_pts == AV_NOPTS_VALUE &&
				in_picture->pts != AV_NOPTS_VALUE)
				ost->forced_kf_ref_pts = in_picture->pts;
			pts_time = in_picture->pts != AV_NOPTS_VALUE ?
				(in_picture->pts - ost->forced_kf_ref_pts) * av_q2d(enc->time_base) : NAN;
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
				//Log("force_key_frame: n:%f n_forced:%f prev_forced_n:%f t:%f prev_forced_t:%f -> res:%f\n",
				//	ost->forced_keyframes_expr_const_values[FKF_N],
				//	ost->forced_keyframes_expr_const_values[FKF_N_FORCED],
				//	ost->forced_keyframes_expr_const_values[FKF_PREV_FORCED_N],
				//	ost->forced_keyframes_expr_const_values[FKF_T],
				//	ost->forced_keyframes_expr_const_values[FKF_PREV_FORCED_T],
				//	res);
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
				//Log("Forced keyframe at time %f\n", pts_time);
			}
			ost->frames_encoded++;
			ret = avcodec_send_frame(enc, in_picture);
			if (ret < 0)
				goto error;
			av_frame_remove_side_data(in_picture, AV_FRAME_DATA_A53_CC);
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
		Log("Video encoding failed\n");
		Stop();
	}
	void finish_output_stream(OutputStream* ost)
	{
		OutputFile* of = output_files[ost->file_index];
		int i;
		ost->finished = ENCODER_FINISHED | MUXER_FINISHED;
		if (of->shortest) {
			for (i = 0; i < of->ctx->nb_streams; i++)
				output_streams[of->ost_index + i]->finished = ENCODER_FINISHED | MUXER_FINISHED;
		}
	}
	int reap_filters(int flush)
	{
		AVFrame* filtered_frame = NULL;
		int i;
		for (i = 0; i < nb_output_streams; i++) {
			OutputStream* ost = output_streams[i];
			OutputFile* of = output_files[ost->file_index];
			AVFilterContext* filter;
			AVCodecContext* enc = ost->enc_ctx;
			int ret = 0;
			if (!ost->filter || !ost->filter->graph->graph)
				continue;
			filter = ost->filter->filter;
			if (!ost->initialized) {
				char error[1024] = "";
				ret = init_output_stream(ost, error, sizeof(error));
				if (ret < 0) {
					Log("Error initializing output stream %d:%d -- %s\n",
						ost->file_index, ost->index, error);
					Stop();
				}
			}
			if (!ost->filtered_frame && !(ost->filtered_frame = av_frame_alloc())) {
				Log("%s %d Memory error", __FUNCTION__, __LINE__);
				Stop();
				return -1;
			}
			filtered_frame = ost->filtered_frame;
			while (1) {
				double float_pts = AV_NOPTS_VALUE;
				ret = av_buffersink_get_frame_flags(filter, filtered_frame,
					AV_BUFFERSINK_FLAG_NO_REQUEST);
				if (ret < 0) {
					if (ret != AVERROR(EAGAIN) && ret != AVERROR_EOF) {
						Log("Error in av_buffersink_get_frame_flags(): %s\n", _av_err2str(ret));
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
						av_rescale_q(start_time, _AV_TIME_BASE_Q, tb);
					float_pts /= 1 << extra_bits;
					float_pts += FFSIGN(float_pts) * 1.0 / (1 << 17);
					filtered_frame->pts =
						av_rescale_q(filtered_frame->pts, filter_tb, enc->time_base) -
						av_rescale_q(start_time, _AV_TIME_BASE_Q, enc->time_base);
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
						Log("Audio filter graph output is not normalized and encoder does not support parameter changes\n");
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
	void print_final_stats(int64_t total_size)
	{
		uint64_t video_size = 0, audio_size = 0, extra_size = 0, other_size = 0;
		uint64_t subtitle_size = 0;
		uint64_t data_size = 0;
		float percent = -1.0;
		int i, j;
		int pass1_used = 1;
		for (i = 0; i < nb_output_streams; i++) {
			OutputStream* ost = output_streams[i];
			switch (ost->enc_ctx->codec_type) {
			case AVMEDIA_TYPE_VIDEO: video_size += ost->data_size; break;
			case AVMEDIA_TYPE_AUDIO: audio_size += ost->data_size; break;
			case AVMEDIA_TYPE_SUBTITLE: subtitle_size += ost->data_size; break;
			default: other_size += ost->data_size; break;
			}
			extra_size += ost->enc_ctx->extradata_size;
			data_size += ost->data_size;
			if ((ost->enc_ctx->flags & (AV_CODEC_FLAG_PASS1 | AV_CODEC_FLAG_PASS2))
				!= AV_CODEC_FLAG_PASS1)
				pass1_used = 0;
		}
		if (data_size && total_size > 0 && total_size >= data_size)
			percent = 100.0 * (total_size - data_size) / data_size;
		Log("video:%1.0fkB audio:%1.0fkB subtitle:%1.0fkB other streams:%1.0fkB global headers:%1.0fkB muxing overhead: ",
			video_size / 1024.0,
			audio_size / 1024.0,
			subtitle_size / 1024.0,
			other_size / 1024.0,
			extra_size / 1024.0);
		if (percent >= 0.0)
			Log("%f%%", percent);
		else
			Log("unknown");
		Log("\n");
		for (i = 0; i < nb_input_files; i++) {
			InputFile* f = input_files[i];
			uint64_t total_packets = 0, total_size = 0;
			Log("Input file #%d (%s):\n",
				i, f->ctx->url);
			for (j = 0; j < f->nb_streams; j++) {
				InputStream* ist = (InputStream*)input_streams[f->ist_index + j];
				enum AVMediaType type = ist->dec_ctx->codec_type;
				total_size += ist->data_size;
				total_packets += ist->nb_packets;
				Log("  Input stream #%d:%d (%s): ",
					i, j, av_get_media_type_string(type));
				Log("%" PRIu64" packets read (%" PRIu64" bytes); ",
					ist->nb_packets, ist->data_size);
				if (ist->decoding_needed) {
					Log("%" PRIu64" frames decoded",
						ist->frames_decoded);
					if (type == AVMEDIA_TYPE_AUDIO)
						Log(" (%" PRIu64" samples)", ist->samples_decoded);
					Log("; ");
				}
				Log("\n");
			}
			Log("  Total: %" PRIu64" packets (%" PRIu64" bytes) demuxed\n",
				total_packets, total_size);
		}
		for (i = 0; i < nb_output_files; i++) {
			OutputFile* of = output_files[i];
			uint64_t total_packets = 0, total_size = 0;
			Log("Output file #%d (%s):\n",
				i, of->ctx->url);
			for (j = 0; j < of->ctx->nb_streams; j++) {
				OutputStream* ost = output_streams[of->ost_index + j];
				enum AVMediaType type = ost->enc_ctx->codec_type;
				total_size += ost->data_size;
				total_packets += ost->packets_written;
				Log("  Output stream #%d:%d (%s): ",
					i, j, av_get_media_type_string(type));
				if (ost->encoding_needed) {
					Log("%" PRIu64" frames encoded",
						ost->frames_encoded);
					if (type == AVMEDIA_TYPE_AUDIO)
						Log(" (%" PRIu64" samples)", ost->samples_encoded);
					Log("; ");
				}
				Log("%" PRIu64" packets muxed (%" PRIu64" bytes); ",
					ost->packets_written, ost->data_size);
				Log("\n");
			}
			Log("  Total: %" PRIu64" packets (%" PRIu64" bytes) muxed\n",
				total_packets, total_size);
		}
		if (video_size + data_size + audio_size + subtitle_size + extra_size == 0) {
			Log("Output file is empty, nothing was encoded ");
			if (pass1_used) {
				Log("\n");
			}
			else {
				Log("(check -ss / -t / -frames parameters if used)\n");
			}
		}
	}
	void ifilter_parameters_from_codecpar(InputFilter* ifilter, AVCodecParameters* par) {
		ifilter->format = par->format;
		ifilter->sample_rate = par->sample_rate;
		ifilter->channels = par->channels;
		ifilter->channel_layout = par->channel_layout;
		ifilter->width = par->width;
		ifilter->height = par->height;
		ifilter->sample_aspect_ratio = par->sample_aspect_ratio;
	}
	void flush_encoders(void)
	{
		int i, ret;
		for (i = 0; i < nb_output_streams; i++) {
			OutputStream* ost = output_streams[i];
			AVCodecContext* enc = ost->enc_ctx;
			OutputFile* of = output_files[ost->file_index];
			if (!ost->encoding_needed)
				continue;
			if (!ost->initialized) {
				FilterGraph* fg = ost->filter->graph;
				char error[1024] = "";
				Log("Finishing stream %d:%d without any data written to it.\n",
					ost->file_index, ost->st->index);
				if (ost->filter && !fg->graph) {
					int x;
					for (x = 0; x < fg->nb_inputs; x++) {
						InputFilter* ifilter = fg->inputs[x];
						if (ifilter->format < 0)
							ifilter_parameters_from_codecpar(ifilter, ifilter->ist->st->codecpar);
					}
					if (!ifilter_has_all_input_formats(fg))
						continue;
					ret = configure_filtergraph(fg);
					if (ret < 0) {
						Log("Error configuring filter graph\n");
						Stop();
					}
					finish_output_stream(ost);
				}
				ret = init_output_stream(ost, error, sizeof(error));
				if (ret < 0) {
					Log("Error initializing output stream %d:%d -- %s\n",
						ost->file_index, ost->index, error);
					Stop();
				}
			}
			if (enc->codec_type == AVMEDIA_TYPE_AUDIO && enc->frame_size <= 1)
				continue;
			if (enc->codec_type != AVMEDIA_TYPE_VIDEO && enc->codec_type != AVMEDIA_TYPE_AUDIO)
				continue;
			for (;;) {
				const char* desc = NULL;
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
						Log("%s encoding failed: %s\n",
							desc,
							_av_err2str(ret));
						Stop();
					}
				}
				if (ret < 0 && ret != AVERROR_EOF) {
					Log("%s encoding failed: %s\n",
						desc,
						_av_err2str(ret));
					Stop();
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
	int check_output_constraints(InputStream* ist, OutputStream* ost)
	{
		OutputFile* of = output_files[ost->file_index];
		int ist_index = input_files[ist->file_index]->ist_index + ist->st->index;
		if (ost->source_index != ist_index)
			return 0;
		if (ost->finished)
			return 0;
		if (of->start_time != AV_NOPTS_VALUE && ist->pts < of->start_time)
			return 0;
		return 1;
	}
	void do_streamcopy(InputStream* ist, OutputStream* ost, const AVPacket* pkt)
	{
		OutputFile* of = output_files[ost->file_index];
		InputFile* f = input_files[ist->file_index];
		int64_t start_time = (of->start_time == AV_NOPTS_VALUE) ? 0 : of->start_time;
		int64_t ost_tb_start_time = av_rescale_q(start_time, _AV_TIME_BASE_Q, ost->mux_timebase);
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
			if (copy_ts && f->start_time != AV_NOPTS_VALUE)
				comp_start = FFMAX(start_time, f->start_time + f->ts_offset);
			if (pkt->pts == AV_NOPTS_VALUE ?
				ist->pts < comp_start :
				pkt->pts < av_rescale_q(comp_start, _AV_TIME_BASE_Q, ist->st->time_base))
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
		if (ost->enc_ctx->codec_type == AVMEDIA_TYPE_VIDEO)
			ost->sync_opts++;
		if (pkt->pts != AV_NOPTS_VALUE)
			opkt.pts = av_rescale_q(pkt->pts, ist->st->time_base, ost->mux_timebase) - ost_tb_start_time;
		else
			opkt.pts = AV_NOPTS_VALUE;
		if (pkt->dts == AV_NOPTS_VALUE)
			opkt.dts = av_rescale_q(ist->dts, _AV_TIME_BASE_Q, ost->mux_timebase);
		else
			opkt.dts = av_rescale_q(pkt->dts, ist->st->time_base, ost->mux_timebase);
		opkt.dts -= ost_tb_start_time;
		if (ost->st->codecpar->codec_type == AVMEDIA_TYPE_AUDIO && pkt->dts != AV_NOPTS_VALUE) {
			int duration = av_get_audio_frame_duration(ist->dec_ctx, pkt->size);
			if (!duration)
				duration = ist->dec_ctx->frame_size;
			AVRational tbAudio;
			tbAudio.num = 1;
			tbAudio.den = ist->dec_ctx->sample_rate;
			opkt.dts = opkt.pts = av_rescale_delta(ist->st->time_base, pkt->dts,
				tbAudio, duration, &ist->filter_in_rescale_delta_last,
				ost->mux_timebase) - ost_tb_start_time;
		}
		opkt.duration = av_rescale_q(pkt->duration, ist->st->time_base, ost->mux_timebase);
		opkt.flags = pkt->flags;
		if (pkt->buf) {
			opkt.buf = av_buffer_ref(pkt->buf);
			if (!opkt.buf)
				Stop();
		}
		opkt.data = pkt->data;
		opkt.size = pkt->size;
		av_copy_packet_side_data(&opkt, pkt);
		output_packet(of, &opkt, ost, 0);
	}
	int guess_input_channel_layout(InputStream* ist)
	{
		AVCodecContext* dec = ist->dec_ctx;
		if (!dec->channel_layout) {
			char layout_name[256];
			if (dec->channels > ist->guess_layout_max)
				return 0;
			dec->channel_layout = av_get_default_channel_layout(dec->channels);
			if (!dec->channel_layout)
				return 0;
			av_get_channel_layout_string(layout_name, sizeof(layout_name),
				dec->channels, dec->channel_layout);
			Log("Guessed Channel Layout for Input Stream "
				"#%d.%d : %s\n", ist->file_index, ist->st->index, layout_name);
		}
		return 1;
	}
	void check_decode_result(InputStream* ist, int* got_output, int ret)
	{
		if (ret < 0 && exit_on_error)
			Stop();
		if (*got_output && ist) {
			if (ist->decoded_frame->decode_error_flags || (ist->decoded_frame->flags & AV_FRAME_FLAG_CORRUPT)) {
				Log("%s: corrupt decoded frame in stream %d\n", input_files[ist->file_index]->ctx->url, ist->st->index);
				if (exit_on_error)
					Stop();
			}
		}
	}
	int ifilter_has_all_input_formats(FilterGraph* fg)
	{
		int i;
		for (i = 0; i < fg->nb_inputs; i++) {
			if (fg->inputs[i]->format < 0 && (fg->inputs[i]->type == AVMEDIA_TYPE_AUDIO ||
				fg->inputs[i]->type == AVMEDIA_TYPE_VIDEO))
				return 0;
		}
		return 1;
	}
	int ifilter_send_frame(InputFilter* ifilter, AVFrame* frame)
	{
		FilterGraph* fg = ifilter->graph;
		int need_reinit, ret, i;
		need_reinit = ifilter->format != frame->format;
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
		if (!ifilter->ist->reinit_filters && fg->graph)
			need_reinit = 0;
		if (!!ifilter->hw_frames_ctx != !!frame->hw_frames_ctx ||
			(ifilter->hw_frames_ctx && ifilter->hw_frames_ctx->data != frame->hw_frames_ctx->data))
			need_reinit = 1;
		if (need_reinit) {
			ret = ifilter_parameters_from_frame(ifilter, frame);
			if (ret < 0)
				return ret;
		}
		if (need_reinit || !fg->graph) {
			for (i = 0; i < fg->nb_inputs; i++) {
				if (!ifilter_has_all_input_formats(fg)) {
					AVFrame* tmp = av_frame_clone(frame);
					if (!tmp)
					{
						Log("%s %d Memory error", __FUNCTION__, __LINE__); Stop(); return -1;
					}
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
				Log("Error while filtering: %s\n", _av_err2str(ret));
				return ret;
			}
			ret = configure_filtergraph(fg);
			if (ret < 0) {
				Log("Error reinitializing filters!\n");
				return ret;
			}
		}
		ret = av_buffersrc_add_frame_flags(ifilter->filter, frame, AV_BUFFERSRC_FLAG_PUSH);
		if (ret < 0) {
			if (ret != AVERROR_EOF)
				Log("Error while filtering: %s\n", _av_err2str(ret));
			return ret;
		}
		return 0;
	}
	int ifilter_send_eof(InputFilter* ifilter, int64_t pts)
	{
		int ret;
		ifilter->eof = 1;
		if (ifilter->filter) {
			ret = av_buffersrc_close(ifilter->filter, pts, AV_BUFFERSRC_FLAG_PUSH);
			if (ret < 0)
				return ret;
		}
		else {
			if (ifilter->format < 0)
				ifilter_parameters_from_codecpar(ifilter, ifilter->ist->st->codecpar);
			if (ifilter->format < 0 && (ifilter->type == AVMEDIA_TYPE_AUDIO || ifilter->type == AVMEDIA_TYPE_VIDEO)) {
				Log("Cannot determine format of input stream %d:%d after EOF\n", ifilter->ist->file_index, ifilter->ist->st->index);
				return AVERROR_INVALIDDATA;
			}
		}
		return 0;
	}
	int decode(AVCodecContext* avctx, AVFrame* frame, int* got_frame, AVPacket* pkt)
	{
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
	int send_frame_to_filters(InputStream* ist, AVFrame* decoded_frame)
	{
		int i, ret;
		AVFrame* f;
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
				Log("Failed to inject frame into filter network: %s\n", _av_err2str(ret));
				break;
			}
		}
		return ret;
	}
	int decode_audio(InputStream* ist, AVPacket* pkt, int* got_output,
		int* decode_failed)
	{
		AVFrame* decoded_frame;
		AVCodecContext* avctx = ist->dec_ctx;
		int ret, err = 0;
		AVRational decoded_frame_tb;
		if (!ist->decoded_frame && !(ist->decoded_frame = av_frame_alloc()))
		{
			Log("%s %d Memory error", __FUNCTION__, __LINE__); Stop(); return -1;
		}
		if (!ist->filter_frame && !(ist->filter_frame = av_frame_alloc()))
		{
			Log("%s %d Memory error", __FUNCTION__, __LINE__); Stop(); return -1;
		}
		decoded_frame = ist->decoded_frame;
		ret = decode(avctx, decoded_frame, got_output, pkt);
		if (ret < 0)
			*decode_failed = 1;
		if (ret >= 0 && avctx->sample_rate <= 0) {
			Log("Sample rate %d invalid\n", avctx->sample_rate);
			ret = AVERROR_INVALIDDATA;
		}
		if (ret != AVERROR_EOF)
			check_decode_result(ist, got_output, ret);
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
			decoded_frame_tb = _AV_TIME_BASE_Q;
		}
		AVRational tbAudio;
		tbAudio.num = 1;
		tbAudio.den = ist->dec_ctx->sample_rate;
		if (decoded_frame->pts != AV_NOPTS_VALUE)
			decoded_frame->pts = av_rescale_delta(decoded_frame_tb, decoded_frame->pts,
				tbAudio, decoded_frame->nb_samples, &ist->filter_in_rescale_delta_last,
				tbAudio);
		ist->nb_samples = decoded_frame->nb_samples;
		err = send_frame_to_filters(ist, decoded_frame);
		av_frame_unref(ist->filter_frame);
		av_frame_unref(decoded_frame);
		return err < 0 ? err : ret;
	}
	int decode_video(InputStream* ist, AVPacket* pkt, int* got_output, int64_t* duration_pts, int eof,
		int* decode_failed)
	{
		AVFrame* decoded_frame;
		int i, ret = 0, err = 0;
		int64_t best_effort_timestamp;
		int64_t dts = AV_NOPTS_VALUE;
		AVPacket avpkt;
		if (!eof && pkt && pkt->size == 0)
			return 0;
		if (!ist->decoded_frame && !(ist->decoded_frame = av_frame_alloc()))
		{
			Log("%s %d Memory error", __FUNCTION__, __LINE__); Stop(); return -1;
		}
		if (!ist->filter_frame && !(ist->filter_frame = av_frame_alloc()))
		{
			Log("%s %d Memory error", __FUNCTION__, __LINE__); Stop(); return -1;
		}
		decoded_frame = ist->decoded_frame;
		if (ist->dts != AV_NOPTS_VALUE)
			dts = av_rescale_q(ist->dts, _AV_TIME_BASE_Q, ist->st->time_base);
		if (pkt) {
			avpkt = *pkt;
			avpkt.dts = dts;
		}
		if (eof) {
			void* _new = av_realloc_array(ist->dts_buffer, ist->nb_dts_buffer + 1, sizeof(ist->dts_buffer[0]));
			if (!_new)
			{
				Log("%s %d Memory error", __FUNCTION__, __LINE__); Stop(); return -1;
			}
			ist->dts_buffer = (int64_t*)_new;
			ist->dts_buffer[ist->nb_dts_buffer++] = dts;
		}
		ret = decode(ist->dec_ctx, decoded_frame, got_output, pkt ? &avpkt : NULL);
		if (ret < 0)
			*decode_failed = 1;
		if (ist->st->codecpar->video_delay < ist->dec_ctx->has_b_frames) {
			if (ist->dec_ctx->codec_id == AV_CODEC_ID_H264) {
				ist->st->codecpar->video_delay = ist->dec_ctx->has_b_frames;
			}
			else
				Log("video_delay is larger in decoder than demuxer %d > %d.\n"
					"If you want to help, upload a sample "
					"of this file to ftp://upload.ffmpeg.org/incoming/ "
					"and contact the ffmpeg-devel mailing list. (ffmpeg-devel@ffmpeg.org)\n",
					ist->dec_ctx->has_b_frames,
					ist->st->codecpar->video_delay);
		}
		if (ret != AVERROR_EOF)
			check_decode_result(ist, got_output, ret);
		if (*got_output && ret >= 0) {
			if (ist->dec_ctx->width != decoded_frame->width ||
				ist->dec_ctx->height != decoded_frame->height ||
				ist->dec_ctx->pix_fmt != decoded_frame->format) {
				Log("Frame parameters mismatch context %d,%d,%d != %d,%d,%d\n",
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
		ist->hwaccel_retrieved_pix_fmt = (enum AVPixelFormat)decoded_frame->format;
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
			int64_t ts = av_rescale_q(decoded_frame->pts = best_effort_timestamp, ist->st->time_base, _AV_TIME_BASE_Q);
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
	int transcode_subtitles(InputStream* ist, AVPacket* pkt, int* got_output,
		int* decode_failed)
	{
		AVSubtitle subtitle;
		int free_sub = 1;
		int i, ret = avcodec_decode_subtitle2(ist->dec_ctx,
			&subtitle, got_output, pkt);
		check_decode_result(NULL, got_output, ret);
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
					Log("Subtitle duration reduced from %" PRId32" to %d%s\n",
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
				Stop();
			if (!av_fifo_space(ist->sub2video.sub_queue)) {
				ret = av_fifo_realloc2(ist->sub2video.sub_queue, 2 * av_fifo_size(ist->sub2video.sub_queue));
				if (ret < 0)
					Stop();
			}
			av_fifo_generic_write(ist->sub2video.sub_queue, &subtitle, sizeof(subtitle), NULL);
			free_sub = 0;
		}
		if (!subtitle.num_rects)
			goto out;
		ist->frames_decoded++;
		for (i = 0; i < nb_output_streams; i++) {
			OutputStream* ost = output_streams[i];
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
	int send_filter_eof(InputStream* ist)
	{
		int i, ret;
		int64_t pts = av_rescale_q_rnd(ist->pts, _AV_TIME_BASE_Q, ist->st->time_base,
			(AVRounding)((int)AV_ROUND_NEAR_INF | (int)AV_ROUND_PASS_MINMAX));
		for (i = 0; i < ist->nb_filters; i++) {
			ret = ifilter_send_eof(ist->filters[i], pts);
			if (ret < 0)
				return ret;
		}
		return 0;
	}
	int process_input_packet(InputStream* ist, const AVPacket* pkt, int no_eof)
	{
		int ret = 0, i;
		int repeating = 0;
		int eof_reached = 0;
		AVPacket avpkt;
		if (!ist->saw_first_ts) {
			ist->dts = ist->st->avg_frame_rate.num ? -ist->dec_ctx->has_b_frames * AV_TIME_BASE / av_q2d(ist->st->avg_frame_rate) : 0;
			ist->pts = 0;
			if (pkt && pkt->pts != AV_NOPTS_VALUE && !ist->decoding_needed) {
				ist->dts += av_rescale_q(pkt->pts, ist->st->time_base, _AV_TIME_BASE_Q);
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
			ist->next_dts = ist->dts = av_rescale_q(pkt->dts, ist->st->time_base, _AV_TIME_BASE_Q);
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
						duration_dts = av_rescale_q(pkt->duration, ist->st->time_base, _AV_TIME_BASE_Q);
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
						ist->next_pts += av_rescale_q(duration_pts, ist->st->time_base, _AV_TIME_BASE_Q);
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
					Log("Error while decoding stream #%d:%d: %s\n",
						ist->file_index, ist->st->index, _av_err2str(ret));
				}
				else {
					Log("Error while processing the decoded "
						"data for stream #%d:%d\n", ist->file_index, ist->st->index);
				}
				if (!decode_failed || exit_on_error)
					Stop();
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
				Log("Error marking filters as finished\n");
				Stop();
			}
		}
		if (!ist->decoding_needed && pkt) {
			ist->dts = ist->next_dts;
			switch (ist->dec_ctx->codec_type) {
			case AVMEDIA_TYPE_AUDIO:
				av_assert1(pkt->duration >= 0);
				if (ist->dec_ctx->sample_rate) {
					ist->next_dts += ((int64_t)AV_TIME_BASE * ist->dec_ctx->frame_size) /
						ist->dec_ctx->sample_rate;
				}
				else {
					ist->next_dts += av_rescale_q(pkt->duration, ist->st->time_base, _AV_TIME_BASE_Q);
				}
				break;
			case AVMEDIA_TYPE_VIDEO:
				if (ist->framerate.num) {
					AVRational time_base_q = _AV_TIME_BASE_Q;
					int64_t next_dts = av_rescale_q(ist->next_dts, time_base_q, av_inv_q(ist->framerate));
					ist->next_dts = av_rescale_q(next_dts + 1, av_inv_q(ist->framerate), time_base_q);
				}
				else if (pkt->duration) {
					ist->next_dts += av_rescale_q(pkt->duration, ist->st->time_base, _AV_TIME_BASE_Q);
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
			OutputStream* ost = output_streams[i];
			if (!check_output_constraints(ist, ost) || ost->encoding_needed)
				continue;
			do_streamcopy(ist, ost, pkt);
		}
		return !eof_reached;
	}
	void print_sdp(void)
	{
		char sdp[16384];
		int i;
		int j;
		AVIOContext* sdp_pb;
		AVFormatContext** avc;
		for (i = 0; i < nb_output_files; i++) {
			if (!output_files[i]->header_written)
				return;
		}
		avc = (AVFormatContext**)av_malloc_array(nb_output_files, sizeof(*avc));
		if (!avc)
			Stop();
		for (i = 0, j = 0; i < nb_output_files; i++) {
			if (!strcmp(output_files[i]->ctx->oformat->name, "rtp")) {
				avc[j] = output_files[i]->ctx;
				j++;
			}
		}
		if (!j)
			goto fail;
		av_sdp_create(avc, j, sdp, sizeof(sdp));
		if (!sdp_filename) {
			printf("SDP:\n%s\n", sdp);
			fflush(stdout);
		}
		else {
			if (avio_open2(&sdp_pb, sdp_filename, AVIO_FLAG_WRITE, NULL, NULL) < 0) {
				Log("Failed to open sdp file '%s'\n", sdp_filename);
			}
			else {
				avio_printf(sdp_pb, "SDP:\n%s", sdp);
				avio_closep(&sdp_pb);
				av_freep(&sdp_filename);
			}
		}
	fail:
		av_freep(&avc);
	}
	static enum AVPixelFormat get_format(AVCodecContext* s, const enum AVPixelFormat* pix_fmts)
	{
		InputStream* ist = (InputStream*)(InputStream*)s->opaque;
		const enum AVPixelFormat* p;
		int ret;
		for (p = pix_fmts; *p != AV_PIX_FMT_NONE; p++) {
			const AVPixFmtDescriptor* desc = av_pix_fmt_desc_get(*p);
			const AVCodecHWConfig* config = NULL;
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
						ist->owner->Log("%s hwaccel requested for input stream #%d:%d, "
							"but cannot be initialized.\n",
							av_hwdevice_get_type_name(config->device_type),
							ist->file_index, ist->st->index);
						return AV_PIX_FMT_NONE;
					}
					continue;
				}
			}
			else {
				const HWAccel* hwaccel = NULL;
				int i;
				for (i = 0; ist->owner->hwaccels[i].name; i++) {
					if (ist->owner->hwaccels[i].pix_fmt == *p) {
						hwaccel = &ist->owner->hwaccels[i];
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
					ist->owner->Log("%s hwaccel requested for input stream #%d:%d, "
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
	static int get_buffer(AVCodecContext* s, AVFrame* frame, int flags) {
		InputStream* ist = (InputStream*)s->opaque;
		if (ist->hwaccel_get_buffer && frame->format == ist->hwaccel_pix_fmt)
			return ist->hwaccel_get_buffer(s, frame, flags);
		return avcodec_default_get_buffer2(s, frame, flags);
	}
	int init_input_stream(int ist_index, char* error, int error_len) {
		int ret;
		InputStream* ist = (InputStream*)input_streams[ist_index];
		if (ist->decoding_needed) {
			AVCodec* codec = ist->dec;
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
					Log("Warning using DVB subtitles for filtering and output at the same time is not fully supported, also see -compute_edt [0|1]\n");
			}
			av_dict_set(&ist->decoder_opts, "sub_text_format", "ass", AV_DICT_DONT_OVERWRITE);
			ist->dec_ctx->pkt_timebase = ist->st->time_base;
			if (!av_dict_get(ist->decoder_opts, "threads", NULL, 0))
				av_dict_set(&ist->decoder_opts, "threads", "1", 0);//多线程CPU太高
			if (ist->st->disposition & AV_DISPOSITION_ATTACHED_PIC)
				av_dict_set(&ist->decoder_opts, "threads", "1", 0);
			ret = hw_device_setup_for_decode(ist);
			if (ret < 0) {
				snprintf(error, error_len, "Device setup failed for "
					"decoder on input stream #%d:%d : %s",
					ist->file_index, ist->st->index, _av_err2str(ret));
				return ret;
			}
           // printf("Decoder name=%s\r\n",ist->dec_ctx->name);
			if ((ret = avcodec_open2(ist->dec_ctx, codec, &ist->decoder_opts)) < 0) {
				if (ret == AVERROR_EXPERIMENTAL)
					abort_codec_experimental(codec, 0);
				snprintf(error, error_len,
					"Error while opening decoder for input stream "
					"#%d:%d : %s",
					ist->file_index, ist->st->index, _av_err2str(ret));
				return ret;
			}
			assert_avoptions(ist->decoder_opts);
		}
		ist->next_pts = AV_NOPTS_VALUE;
		ist->next_dts = AV_NOPTS_VALUE;
		return 0;
	}
	InputStream* get_input_stream(OutputStream* ost) {
		if (ost->source_index >= 0)
			return input_streams[ost->source_index];
		return NULL;
	}
	static int compare_int64(const void* a, const void* b) {
		return FFDIFFSIGN(*(const int64_t*)a, *(const int64_t*)b);
	}
	int check_init_output_file(OutputFile* of, int file_index) {
		int ret, i;
		for (i = 0; i < of->ctx->nb_streams; i++) {
			OutputStream* ost = output_streams[of->ost_index + i];
			if (!ost->initialized)
				return 0;
		}
		ret = avformat_write_header(of->ctx, &of->opts);
		if (ret < 0) {
			Log("Could not write header for output file #%d "
				"(incorrect codec parameters ?): %s\n",
				file_index, _av_err2str(ret));
			return ret;
		}
		of->header_written = 1;
		av_dump_format(of->ctx, file_index, of->ctx->url, 1);
		if (sdp_filename || want_sdp)
			print_sdp();
		for (i = 0; i < of->ctx->nb_streams; i++) {
			OutputStream* ost = output_streams[of->ost_index + i];
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
	int init_output_bsfs(OutputStream* ost) {
		AVBSFContext* ctx;
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
				Log("Error initializing bitstream filter: %s\n",
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
	int init_output_stream_streamcopy(OutputStream* ost)
	{
		OutputFile* of = output_files[ost->file_index];
		InputStream* ist = (InputStream*)get_input_stream(ost);
		AVCodecParameters* par_dst = ost->st->codecpar;
		AVCodecParameters* par_src = ost->ref_par;
		AVRational sar;
		int i, ret;
		uint32_t codec_tag = par_dst->codec_tag;
		av_assert0(ist && !ost->filter);
		ret = avcodec_parameters_to_context(ost->enc_ctx, ist->st->codecpar);
		if (ret >= 0)
			ret = av_opt_set_dict(ost->enc_ctx, &ost->encoder_opts);
		if (ret < 0) {
			Log("Error setting up codec context options.\n");
			return ret;
		}
		ret = avcodec_parameters_from_context(par_src, ost->enc_ctx);
		if (ret < 0) {
			Log("Error getting reference codec parameters.\n");
			return ret;
		}
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
		if (ost->st->time_base.num <= 0 || ost->st->time_base.den <= 0)
			ost->st->time_base = av_add_q(av_stream_get_codec_timebase(ost->st), TimeBase_0_1);
		if (ost->st->duration <= 0 && ist->st->duration > 0)
			ost->st->duration = av_rescale_q(ist->st->duration, ist->st->time_base, ost->st->time_base);
		ost->st->disposition = ist->st->disposition;
		if (ist->st->nb_side_data) {
			for (i = 0; i < ist->st->nb_side_data; i++) {
				const AVPacketSideData* sd_src = &ist->st->side_data[i];
				uint8_t* dst_data;
				dst_data = av_stream_new_side_data(ost->st, sd_src->type, sd_src->size);
				if (!dst_data)
				{
					Log("%s %d Memory error", __FUNCTION__, __LINE__); Stop(); return -1;
				}
				memcpy(dst_data, sd_src->data, sd_src->size);
			}
		}
		if (ost->rotate_overridden) {
			uint8_t* sd = av_stream_new_side_data(ost->st, AV_PKT_DATA_DISPLAYMATRIX,
				sizeof(int32_t) * 9);
			if (sd)
				av_display_rotation_set((int32_t*)sd, -ost->rotate_override_value);
		}
		AVRational tbPar;
		tbPar.num = par_dst->height;
		tbPar.den = par_dst->width;
		switch (par_dst->codec_type) {
		case AVMEDIA_TYPE_AUDIO:
			if (audio_volume != 256) {
				Log("-acodec copy and -vol are incompatible (frames are not decoded)\n");
				Stop();
			}
			if ((par_dst->block_align == 1 || par_dst->block_align == 1152 || par_dst->block_align == 576) && par_dst->codec_id == AV_CODEC_ID_MP3)
				par_dst->block_align = 0;
			if (par_dst->codec_id == AV_CODEC_ID_AC3)
				par_dst->block_align = 0;
			break;
		case AVMEDIA_TYPE_VIDEO:
			if (ost->frame_aspect_ratio.num) {
				sar =
					av_mul_q(ost->frame_aspect_ratio, tbPar);
				Log("Overriding aspect ratio "
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
	void set_encoder_id(OutputFile* of, OutputStream* ost)
	{
		AVDictionaryEntry* e;
		uint8_t* encoder_string;
		int encoder_string_len;
		int format_flags = 0;
		int codec_flags = ost->enc_ctx->flags;
		if (av_dict_get(ost->st->metadata, "encoder", NULL, 0))
			return;
		e = av_dict_get(of->opts, "fflags", NULL, 0);
		if (e) {
			const AVOption* o = av_opt_find(of->ctx, "fflags", NULL, 0, 0);
			if (!o)
				return;
			av_opt_eval_flags(of->ctx, o, e->value, &format_flags);
		}
		e = av_dict_get(ost->encoder_opts, "flags", NULL, 0);
		if (e) {
			const AVOption* o = av_opt_find(ost->enc_ctx, "flags", NULL, 0, 0);
			if (!o)
				return;
			av_opt_eval_flags(ost->enc_ctx, o, e->value, &codec_flags);
		}
		encoder_string_len = sizeof(LIBAVCODEC_IDENT) + strlen(ost->enc->name) + 2;
		encoder_string = (uint8_t*)av_mallocz(encoder_string_len);
		if (!encoder_string)
			Stop();
		if (!(format_flags & AVFMT_FLAG_BITEXACT) && !(codec_flags & AV_CODEC_FLAG_BITEXACT))
			av_strlcpy((char*)encoder_string, LIBAVCODEC_IDENT " ", encoder_string_len);
		else
			av_strlcpy((char*)encoder_string, "Lavc ", encoder_string_len);
		av_strlcat((char*)encoder_string, ost->enc->name, encoder_string_len);
		av_dict_set(&ost->st->metadata, "encoder", (const char*)encoder_string,
			AV_DICT_DONT_STRDUP_VAL | AV_DICT_DONT_OVERWRITE);
	}
	void parse_forced_key_frames(char* kf, OutputStream* ost, AVCodecContext* avctx) {
		char* p;
		int n = 1, i, size, index = 0;
		int64_t t, * pts;
		for (p = kf; *p; p++)
			if (*p == ',')
				n++;
		size = n;
		pts = (int64_t*)av_malloc_array(size, sizeof(*pts));
		if (!pts) {
			Log("Could not allocate forced key frames array.\n");
			Stop();
		}
		p = kf;
		for (i = 0; i < n; i++) {
			char* next = strchr(p, ',');
			if (next)
				*next++ = 0;
			if (!memcmp(p, "chapters", 8)) {
				AVFormatContext* avf = output_files[ost->file_index]->ctx;
				int j;
				if (avf->nb_chapters > INT_MAX - size ||
					!(pts = (int64_t*)av_realloc_f(pts, size += avf->nb_chapters - 1,
						sizeof(*pts)))) {
					Log("Could not allocate forced key frames array.\n");
					Stop();
				}
				t = p[8] ? parse_time_or_die("force_key_frames", p + 8, 1) : 0;
				t = av_rescale_q(t, _AV_TIME_BASE_Q, avctx->time_base);
				for (j = 0; j < avf->nb_chapters; j++) {
					AVChapter* c = avf->chapters[j];
					av_assert1(index < size);
					pts[index++] = av_rescale_q(c->start, c->time_base,
						avctx->time_base) + t;
				}
			}
			else {
				t = parse_time_or_die("force_key_frames", p, 1);
				av_assert1(index < size);
				pts[index++] = av_rescale_q(t, _AV_TIME_BASE_Q, avctx->time_base);
			}
			p = next;
		}
		av_assert0(index == size);
		qsort(pts, size, sizeof(*pts), compare_int64);
		ost->forced_kf_count = size;
		ost->forced_kf_pts = pts;
	}
	void init_encoder_time_base(OutputStream* ost, AVRational default_time_base) {
		InputStream* ist = (InputStream*)get_input_stream(ost);
		AVCodecContext* enc_ctx = ost->enc_ctx;
		AVFormatContext* oc;
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
			Log("Input stream data not available, using default time base\n");
		}
		enc_ctx->time_base = default_time_base;
	}
	int init_output_stream_encode(OutputStream* ost) {
		InputStream* ist = (InputStream*)get_input_stream(ost);
		AVCodecContext* enc_ctx = ost->enc_ctx;
		AVCodecContext* dec_ctx = NULL;
		AVFormatContext* oc = output_files[ost->file_index]->ctx;
		int j, ret;
		set_encoder_id(output_files[ost->file_index], ost);
		av_dict_set(&ost->st->metadata, "rotate", NULL, 0);
		if (ist) {
			ost->st->disposition = ist->st->disposition;
			dec_ctx = ist->dec_ctx;
			enc_ctx->chroma_sample_location = dec_ctx->chroma_sample_location;
		}
		else {
			for (j = 0; j < oc->nb_streams; j++) {
				AVStream* st = oc->streams[j];
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
				ost->frame_rate = TimeBase_25_1;
				Log("No information "
					"about the input framerate is available. Falling "
					"back to a default value of 25fps for output stream #%d:%d. Use the -r option "
					"if you want a different framerate.\n",
					ost->file_index, ost->index);
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
		AVRational tbEncCtx;
		tbEncCtx.num = enc_ctx->height;
		tbEncCtx.den = enc_ctx->width;
		switch (enc_ctx->codec_type) {
		case AVMEDIA_TYPE_AUDIO:
			enc_ctx->sample_fmt = (enum AVSampleFormat)av_buffersink_get_format(ost->filter->filter);
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
				Log("Frame rate very high for a muxer not efficiently supporting it.\n"
					"Please consider specifying a lower framerate, a different muxer or -vsync 2\n");
			}
			for (j = 0; j < ost->forced_kf_count; j++)
				ost->forced_kf_pts[j] = av_rescale_q(ost->forced_kf_pts[j],
					_AV_TIME_BASE_Q,
					enc_ctx->time_base);
			enc_ctx->width = av_buffersink_get_w(ost->filter->filter);
			enc_ctx->height = av_buffersink_get_h(ost->filter->filter);
			enc_ctx->sample_aspect_ratio = ost->st->sample_aspect_ratio =
				ost->frame_aspect_ratio.num ?
				av_mul_q(ost->frame_aspect_ratio, tbEncCtx) :
				av_buffersink_get_sample_aspect_ratio(ost->filter->filter);
			enc_ctx->pix_fmt = (enum AVPixelFormat)av_buffersink_get_format(ost->filter->filter);
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
			if (ost->top_field_first == 0) {
				enc_ctx->field_order = AV_FIELD_BB;
			}
			else if (ost->top_field_first == 1) {
				enc_ctx->field_order = AV_FIELD_TT;
			}
			if (ost->forced_keyframes) {
				if (!strncmp(ost->forced_keyframes, "expr:", 5)) {
					ret = av_expr_parse(&ost->forced_keyframes_pexpr, ost->forced_keyframes + 5,
						forced_keyframes_const_names, NULL, NULL, NULL, NULL, 0, NULL);
					if (ret < 0) {
						Log("Invalid force_key_frames expression '%s'\n", ost->forced_keyframes + 5);
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
			enc_ctx->time_base = _AV_TIME_BASE_Q;
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
	int init_output_stream(OutputStream* ost, char* error, int error_len) {
		int ret = 0;
		if (ost->encoding_needed) {
			AVCodec* codec = ost->enc;
			AVCodecContext* dec = NULL;
			InputStream* ist;
			ret = init_output_stream_encode(ost);
			if (ret < 0)
				return ret;
			if ((ist = get_input_stream(ost)))
				dec = ist->dec_ctx;
			if (dec && dec->subtitle_header) {
				ost->enc_ctx->subtitle_header = (uint8_t*)av_mallocz(dec->subtitle_header_size + 1);
				if (!ost->enc_ctx->subtitle_header)
				{
					Log("%s %d Memory error", __FUNCTION__, __LINE__); Stop(); return -1;
				}
				memcpy(ost->enc_ctx->subtitle_header, dec->subtitle_header, dec->subtitle_header_size);
				ost->enc_ctx->subtitle_header_size = dec->subtitle_header_size;
			}
			if (!av_dict_get(ost->encoder_opts, "threads", NULL, 0))
				av_dict_set(&ost->encoder_opts, "threads", "auto", 0);  //多线程CPU太高
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
				{
					Log("%s %d Memory error", __FUNCTION__, __LINE__); Stop(); return -1;
				}
			}
			else {
				ret = hw_device_setup_for_encode(ost);
				if (ret < 0) {
					snprintf(error, error_len, "Device setup failed for "
						"encoder on output stream #%d:%d : %s",
						ost->file_index, ost->index, _av_err2str(ret));
					return ret;
				}
			}
			if (ist && ist->dec->type == AVMEDIA_TYPE_SUBTITLE && ost->enc->type == AVMEDIA_TYPE_SUBTITLE) {
				int input_props = 0, output_props = 0;
				AVCodecDescriptor const* input_descriptor =
					avcodec_descriptor_get(dec->codec_id);
				AVCodecDescriptor const* output_descriptor =
					avcodec_descriptor_get(ost->enc_ctx->codec_id);
				if (input_descriptor)
					input_props = input_descriptor->props & (AV_CODEC_PROP_TEXT_SUB | AV_CODEC_PROP_BITMAP_SUB);
				if (output_descriptor)
					output_props = output_descriptor->props & (AV_CODEC_PROP_TEXT_SUB | AV_CODEC_PROP_BITMAP_SUB);
				if (input_props && output_props && input_props != output_props) {
					snprintf(error, error_len,
						"Subtitle encoding currently only possible from text to text "
						"or bitmap to bitmap");
					return AVERROR_INVALIDDATA;
				}
			}
            printf("Encoder name=%s\n",ost->enc_ctx->codec->name);
			if ((ret = avcodec_open2(ost->enc_ctx, codec, &ost->encoder_opts)) < 0) {
				if (ret == AVERROR_EXPERIMENTAL)
					abort_codec_experimental(codec, 1);
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
			assert_avoptions(ost->encoder_opts);
			if (ost->enc_ctx->bit_rate && ost->enc_ctx->bit_rate < 1000 &&
				ost->enc_ctx->codec_id != AV_CODEC_ID_CODEC2)
				Log("The bitrate parameter is set too low."
					" It takes bits/s as argument, not kbits/s\n");
			ret = avcodec_parameters_from_context(ost->st->codecpar, ost->enc_ctx);
			if (ret < 0) {
				Log("Error initializing the output stream codec context.\n");
				Stop();
			}
			ret = avcodec_copy_context(ost->st->codec, ost->enc_ctx);
			if (ret < 0)
				return ret;
			if (ost->enc_ctx->nb_coded_side_data) {
				int i;
				for (i = 0; i < ost->enc_ctx->nb_coded_side_data; i++) {
					const AVPacketSideData* sd_src = &ost->enc_ctx->coded_side_data[i];
					uint8_t* dst_data;
					dst_data = av_stream_new_side_data(ost->st, sd_src->type, sd_src->size);
					if (!dst_data)
					{
						Log("%s %d Memory error", __FUNCTION__, __LINE__); Stop(); return -1;
					}
					memcpy(dst_data, sd_src->data, sd_src->size);
				}
			}
			if (ist) {
				int i;
				for (i = 0; i < ist->st->nb_side_data; i++) {
					AVPacketSideData* sd = &ist->st->side_data[i];
					uint8_t* dst = av_stream_new_side_data(ost->st, sd->type, sd->size);
					if (!dst)
					{
						Log("%s %d Memory error", __FUNCTION__, __LINE__); Stop(); return -1;
					}
					memcpy(dst, sd->data, sd->size);
					if (ist->autorotate && sd->type == AV_PKT_DATA_DISPLAYMATRIX)
						av_display_rotation_set((int32_t*)dst, 0);
				}
			}
			if (ost->st->time_base.num <= 0 || ost->st->time_base.den <= 0)
				ost->st->time_base = av_add_q(ost->enc_ctx->time_base, TimeBase_0_1);
			if (ost->st->duration <= 0 && ist && ist->st->duration > 0)
				ost->st->duration = av_rescale_q(ist->st->duration, ist->st->time_base, ost->st->time_base);
			ost->st->codec->codec = ost->enc_ctx->codec;
		}
		else if (ost->stream_copy) {
			ret = init_output_stream_streamcopy(ost);
			if (ret < 0)
				return ret;
		}
		if (ost->disposition) {
			const AVClass* pclass = &init_output_class;
			ret = av_opt_eval_flags(&pclass, &init_output_stream_opts[0], ost->disposition, &ost->st->disposition);
			if (ret < 0)
				return ret;
		}
		ret = init_output_bsfs(ost);
		if (ret < 0)
			return ret;
		ost->initialized = 1;
		ret = check_init_output_file(output_files[ost->file_index], ost->file_index);
		if (ret < 0)
			return ret;
		return ret;
	}
	void report_new_stream(int input_index, AVPacket* pkt) {
		InputFile* file = input_files[input_index];
		AVStream* st = file->ctx->streams[pkt->stream_index];
		if (pkt->stream_index < file->nb_streams_warn)
			return;
		Log("New %s stream %d:%d at pos:%" PRId64" and DTS:%ss\n",
			av_get_media_type_string(st->codecpar->codec_type),
			input_index, pkt->stream_index,
			pkt->pos, _av_ts2timestr(pkt->dts, &st->time_base));
		file->nb_streams_warn = pkt->stream_index + 1;
	}
	int transcode_init(void) {
		int ret = 0, i, j, k;
		AVFormatContext* oc;
		OutputStream* ost;
		InputStream* ist;
		char error[1024] = { 0 };
		for (i = 0; i < nb_filtergraphs; i++) {
			FilterGraph* fg = filtergraphs[i];
			for (j = 0; j < fg->nb_outputs; j++) {
				OutputFilter* ofilter = fg->outputs[j];
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
		for (i = 0; i < nb_input_files; i++) {
			InputFile* ifile = input_files[i];
			if (ifile->rate_emu)
				for (j = 0; j < ifile->nb_streams; j++)
					input_streams[j + ifile->ist_index]->start = av_gettime_relative();
		}
		for (i = 0; i < nb_input_streams; i++)
			if ((ret = init_input_stream(i, error, sizeof(error))) < 0) {
				for (i = 0; i < nb_output_streams; i++) {
					ost = output_streams[i];
					avcodec_close(ost->enc_ctx);
				}
				goto dump_format;
			}
		for (i = 0; i < nb_output_streams; i++) {
			if (output_streams[i]->filter)
				continue;
			ret = init_output_stream(output_streams[i], error, sizeof(error));
			if (ret < 0)
				goto dump_format;
		}
		for (i = 0; i < nb_input_files; i++) {
			InputFile* ifile = input_files[i];
			for (j = 0; j < ifile->ctx->nb_programs; j++) {
				AVProgram* p = ifile->ctx->programs[j];
				int discard = AVDISCARD_ALL;
				for (k = 0; k < p->nb_stream_indexes; k++)
					if (!input_streams[ifile->ist_index + p->stream_index[k]]->discard) {
						discard = AVDISCARD_DEFAULT;
						break;
					}
				p->discard = (enum AVDiscard)discard;
			}
		}
		for (i = 0; i < nb_output_files; i++) {
			oc = output_files[i]->ctx;
			if (oc->oformat->flags & AVFMT_NOSTREAMS && oc->nb_streams == 0) {
				ret = check_init_output_file(output_files[i], i);
				if (ret < 0)
					goto dump_format;
			}
		}
	dump_format:
		Log("Stream mapping:\n");
		for (i = 0; i < nb_input_streams; i++) {
			ist = input_streams[i];
			for (j = 0; j < ist->nb_filters; j++) {
				if (!filtergraph_is_simple(ist->filters[j]->graph)) {
					Log("  Stream #%d:%d (%s) -> %s",
						ist->file_index, ist->st->index, ist->dec ? ist->dec->name : "?",
						ist->filters[j]->name);
					if (nb_filtergraphs > 1)
						Log(" (graph %d)", ist->filters[j]->graph->index);
					Log("\n");
				}
			}
		}
		for (i = 0; i < nb_output_streams; i++) {
			ost = output_streams[i];
			if (ost->attachment_filename) {
				Log("  File %s -> Stream #%d:%d\n",
					ost->attachment_filename, ost->file_index, ost->index);
				continue;
			}
			if (ost->filter && !filtergraph_is_simple(ost->filter->graph)) {
				Log("  %s", ost->filter->name);
				if (nb_filtergraphs > 1)
					Log(" (graph %d)", ost->filter->graph->index);
				Log(" -> Stream #%d:%d (%s)\n", ost->file_index,
					ost->index, ost->enc ? ost->enc->name : "?");
				continue;
			}
			Log("  Stream #%d:%d -> #%d:%d",
				input_streams[ost->source_index]->file_index,
				input_streams[ost->source_index]->st->index,
				ost->file_index,
				ost->index);
			if (ost->sync_ist != input_streams[ost->source_index])
				Log(" [sync #%d:%d]",
					ost->sync_ist->file_index,
					ost->sync_ist->st->index);
			if (ost->stream_copy)
				Log(" (copy)");
			else {
				const AVCodec* in_codec = input_streams[ost->source_index]->dec;
				const AVCodec* out_codec = ost->enc;
				const char* decoder_name = "?";
				const char* in_codec_name = "?";
				const char* encoder_name = "?";
				const char* out_codec_name = "?";
				const AVCodecDescriptor* desc;
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
				Log(" (%s (%s) -> %s (%s))",
					in_codec_name, decoder_name,
					out_codec_name, encoder_name);
			}
			Log("\n");
		}
		if (ret) {
			Log("%s\n", error);
			return ret;
		}
		return 0;
	}
	int need_output(void) {
		int i;
		for (i = 0; i < nb_output_streams; i++) {
			OutputStream* ost = output_streams[i];
			OutputFile* of = output_files[ost->file_index];
			AVFormatContext* os = output_files[ost->file_index]->ctx;
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
	OutputStream* choose_output(void) {
		int i;
		int64_t opts_min = INT64_MAX;
		OutputStream* ost_min = NULL;
		for (i = 0; i < nb_output_streams; i++) {
			OutputStream* ost = output_streams[i];
			int64_t opts = ost->st->cur_dts == AV_NOPTS_VALUE ? INT64_MIN :
				av_rescale_q(ost->st->cur_dts, ost->st->time_base,
					_AV_TIME_BASE_Q);
			//if (ost->st->cur_dts == AV_NOPTS_VALUE)
			//	Log( "cur_dts is invalid (this is harmless if it occurs once at the start per stream)\n");
			if (!ost->initialized && !ost->inputs_done)
				return ost;
			if (!ost->finished && opts < opts_min) {
				opts_min = opts;
				ost_min = ost->unavailable ? NULL : ost;
			}
		}
		return ost_min;
	}

	int get_input_packet(InputFile* f, AVPacket* pkt)
	{
		if (f->rate_emu) {
			int i;
			for (i = 0; i < f->nb_streams; i++) {
				InputStream* ist = (InputStream*)input_streams[f->ist_index + i];
				int64_t pts = av_rescale(ist->dts, 1000000, AV_TIME_BASE);
				int64_t now = av_gettime_relative() - ist->start;
				if (pts > now)
					return AVERROR(EAGAIN);
			}
		}

		return av_read_frame(f->ctx, pkt);
	}
	int got_eagain(void) {
		for (int i = 0; i < nb_output_streams; i++)
			if (output_streams[i]->unavailable)
				return 1;
		return 0;
	}
	void reset_eagain(void) {
		for (int i = 0; i < nb_input_files; i++)
			input_files[i]->eagain = 0;
		for (int i = 0; i < nb_output_streams; i++)
			output_streams[i]->unavailable = 0;
	}
	AVRational duration_max(int64_t tmp, int64_t* duration, AVRational tmp_time_base, AVRational time_base) {
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
	int seek_to_start(InputFile* ifile, AVFormatContext* is) {
		InputStream* ist;
		AVCodecContext* avctx;
		int i, ret, has_audio = 0;
		int64_t duration = 0;
		ret = av_seek_frame(is, -1, is->start_time, 0);
		if (ret < 0)
			return ret;
		for (i = 0; i < ifile->nb_streams; i++) {
			ist = input_streams[ifile->ist_index + i];
			avctx = ist->dec_ctx;
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
			duration += ist->max_pts - ist->min_pts;
			ifile->time_base = duration_max(duration, &ifile->duration, ist->st->time_base,
				ifile->time_base);
		}
		if (ifile->loop > 0)
			ifile->loop--;
		return ret;
	}
	int process_input(int file_index) {
		InputFile* ifile = input_files[file_index];
		AVFormatContext* is;
		InputStream* ist;
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
			AVCodecContext* avctx;
			for (i = 0; i < ifile->nb_streams; i++) {
				ist = input_streams[ifile->ist_index + i];
				avctx = ist->dec_ctx;
				if (ist->decoding_needed) {
					ret = process_input_packet(ist, NULL, 1);
					if (ret > 0)
						return 0;
					avcodec_flush_buffers(avctx);
				}
			}

			ret = seek_to_start(ifile, is);

			if (ret < 0)
				Log("Seek to start failed.\n");
			else
				ret = get_input_packet(ifile, &pkt);
			if (ret == AVERROR(EAGAIN)) {
				ifile->eagain = 1;
				return ret;
			}
		}
		if (ret < 0) {
			if (ret != AVERROR_EOF) {
				if (exit_on_error)
					Stop();
			}
			for (i = 0; i < ifile->nb_streams; i++) {
				ist = input_streams[ifile->ist_index + i];
				if (ist->decoding_needed) {
					ret = process_input_packet(ist, NULL, 0);
					if (ret > 0)
						return 0;
				}
				for (j = 0; j < nb_output_streams; j++) {
					OutputStream* ost = output_streams[j];
					if (ost->source_index == ifile->ist_index + i &&
						(ost->stream_copy || ost->enc->type == AVMEDIA_TYPE_SUBTITLE))
						finish_output_stream(ost);
				}
			}
			ifile->eof_reached = 1;
			return AVERROR(EAGAIN);
		}
		reset_eagain();
		if (pkt.stream_index >= ifile->nb_streams) {
			report_new_stream(file_index, &pkt);
			goto discard_packet;
		}
		ist = input_streams[ifile->ist_index + pkt.stream_index];
		ist->data_size += pkt.size;
		ist->nb_packets++;
		if (ist->discard)
			goto discard_packet;
		if (pkt.flags & AV_PKT_FLAG_CORRUPT) {
			Log("%s: corrupt input packet in stream %d\n", is->url, pkt.stream_index);
			if (exit_on_error)
				Stop();
		}
		if (!ist->wrap_correction_done && is->start_time != AV_NOPTS_VALUE && ist->st->pts_wrap_bits < 64) {
			int64_t stime, stime2;
			if (ist->next_dts == AV_NOPTS_VALUE
				&& ifile->ts_offset == -is->start_time
				&& (is->iformat->flags & AVFMT_TS_DISCONT)) {
				int64_t new_start_time = INT64_MAX;
				for (i = 0; i < is->nb_streams; i++) {
					AVStream* st = is->streams[i];
					if (st->discard == AVDISCARD_ALL || st->start_time == AV_NOPTS_VALUE)
						continue;
					new_start_time = FFMIN(new_start_time, av_rescale_q(st->start_time, st->time_base, _AV_TIME_BASE_Q));
				}
				if (new_start_time > is->start_time) {
					Log("Correcting start time by %" PRId64"\n", new_start_time - is->start_time);
					ifile->ts_offset = -new_start_time;
				}
			}
			stime = av_rescale_q(is->start_time, _AV_TIME_BASE_Q, ist->st->time_base);
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
				AVPacketSideData* src_sd = &ist->st->side_data[i];
				uint8_t* dst_data;
				if (src_sd->type == AV_PKT_DATA_DISPLAYMATRIX)
					continue;
				if (av_packet_get_side_data(&pkt, src_sd->type, NULL))
					continue;
				dst_data = av_packet_new_side_data(&pkt, src_sd->type, src_sd->size);
				if (!dst_data)
					Stop();
				memcpy(dst_data, src_sd->data, src_sd->size);
			}
		}
		if (pkt.dts != AV_NOPTS_VALUE)
			pkt.dts += av_rescale_q(ifile->ts_offset, _AV_TIME_BASE_Q, ist->st->time_base);
		if (pkt.pts != AV_NOPTS_VALUE)
			pkt.pts += av_rescale_q(ifile->ts_offset, _AV_TIME_BASE_Q, ist->st->time_base);
		if (pkt.pts != AV_NOPTS_VALUE)
			pkt.pts *= ist->ts_scale;
		if (pkt.dts != AV_NOPTS_VALUE)
			pkt.dts *= ist->ts_scale;
		pkt_dts = av_rescale_q_rnd(pkt.dts, ist->st->time_base, _AV_TIME_BASE_Q,
			(enum AVRounding)((int)AV_ROUND_NEAR_INF | (int)AV_ROUND_PASS_MINMAX));
		if ((ist->dec_ctx->codec_type == AVMEDIA_TYPE_VIDEO ||
			ist->dec_ctx->codec_type == AVMEDIA_TYPE_AUDIO) &&
			pkt_dts != AV_NOPTS_VALUE && ist->next_dts == AV_NOPTS_VALUE && !copy_ts
			&& (is->iformat->flags & AVFMT_TS_DISCONT) && ifile->last_ts != AV_NOPTS_VALUE) {
			int64_t delta = pkt_dts - ifile->last_ts;
			if (delta < -1LL * dts_delta_threshold * AV_TIME_BASE ||
				delta > 1LL * dts_delta_threshold * AV_TIME_BASE) {
				ifile->ts_offset -= delta;
				Log("Inter stream timestamp discontinuity %" PRId64", new offset= %" PRId64"\n",
					delta, ifile->ts_offset);
				pkt.dts -= av_rescale_q(delta, _AV_TIME_BASE_Q, ist->st->time_base);
				if (pkt.pts != AV_NOPTS_VALUE)
					pkt.pts -= av_rescale_q(delta, _AV_TIME_BASE_Q, ist->st->time_base);
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
		pkt_dts = av_rescale_q_rnd(pkt.dts, ist->st->time_base, _AV_TIME_BASE_Q,
			(enum AVRounding)((int)AV_ROUND_NEAR_INF | (int)AV_ROUND_PASS_MINMAX));
		if ((ist->dec_ctx->codec_type == AVMEDIA_TYPE_VIDEO ||
			ist->dec_ctx->codec_type == AVMEDIA_TYPE_AUDIO) &&
			pkt_dts != AV_NOPTS_VALUE && ist->next_dts != AV_NOPTS_VALUE &&
			!copy_ts) {
			int64_t delta = pkt_dts - ist->next_dts;
			if (is->iformat->flags & AVFMT_TS_DISCONT) {
				if (delta < -1LL * dts_delta_threshold * AV_TIME_BASE ||
					delta > 1LL * dts_delta_threshold * AV_TIME_BASE ||
					pkt_dts + AV_TIME_BASE / 10 < FFMAX(ist->pts, ist->dts)) {
					ifile->ts_offset -= delta;
					Log("timestamp discontinuity for stream #%d:%d "
						"(id=%d, type=%s): %" PRId64", new offset= %" PRId64"\n",
						ist->file_index, ist->st->index, ist->st->id,
						av_get_media_type_string(ist->dec_ctx->codec_type),
						delta, ifile->ts_offset);
					pkt.dts -= av_rescale_q(delta, _AV_TIME_BASE_Q, ist->st->time_base);
					if (pkt.pts != AV_NOPTS_VALUE)
						pkt.pts -= av_rescale_q(delta, _AV_TIME_BASE_Q, ist->st->time_base);
				}
			}
			else {
				if (delta < -1LL * dts_error_threshold * AV_TIME_BASE ||
					delta > 1LL * dts_error_threshold * AV_TIME_BASE) {
					Log("DTS %" PRId64", next:%" PRId64" st:%d invalid dropping\n",
						pkt.dts, ist->next_dts, pkt.stream_index);
					pkt.dts = AV_NOPTS_VALUE;
				}
				if (pkt.pts != AV_NOPTS_VALUE) {
					int64_t pkt_pts = av_rescale_q(pkt.pts, ist->st->time_base, _AV_TIME_BASE_Q);
					delta = pkt_pts - ist->next_dts;
					if (delta < -1LL * dts_error_threshold * AV_TIME_BASE ||
						delta > 1LL * dts_error_threshold * AV_TIME_BASE) {
						Log("PTS %" PRId64", next:%" PRId64" invalid dropping st:%d\n",
							pkt.pts, ist->next_dts, pkt.stream_index);
						pkt.pts = AV_NOPTS_VALUE;
					}
				}
			}
		}
		if (pkt.dts != AV_NOPTS_VALUE)
			ifile->last_ts = av_rescale_q(pkt.dts, ist->st->time_base, _AV_TIME_BASE_Q);
		sub2video_heartbeat(ist, pkt.pts);
		process_input_packet(ist, &pkt, 0);
	discard_packet:
		av_packet_unref(&pkt);
		return 0;
	}
	int transcode_from_filter(FilterGraph* graph, InputStream** best_ist) {
		int i, ret;
		int nb_requests, nb_requests_max = 0;
		InputFilter* ifilter;
		InputStream* ist;
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
	int transcode_step(void)
	{
		OutputStream* ost;
		InputStream* ist = NULL;
		int ret;
		ost = choose_output();
		if (!ost) {
			if (got_eagain()) {
				reset_eagain();
				av_usleep(10000);
				return 0;
			}
			Log("No more inputs to read from, finishing.\n");
			return AVERROR_EOF;
		}
		if (ost->filter && !ost->filter->graph->graph) {
			if (ifilter_has_all_input_formats(ost->filter->graph)) {
				ret = configure_filtergraph(ost->filter->graph);
				if (ret < 0) {
					Log("Error reinitializing filters!\n");
					return ret;
				}
			}
		}
		if (ost->filter && ost->filter->graph->graph) {
			if (!ost->initialized) {
				char error[1024] = { 0 };
				ret = init_output_stream(ost, error, sizeof(error));
				if (ret < 0) {
					Log("Error initializing output stream %d:%d -- %s\n",
						ost->file_index, ost->index, error);
					Stop();
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
				InputFilter* ifilter = ost->filter->graph->inputs[i];
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
	//转换函数
	int transcode(void) {
		int ret, i;
		AVFormatContext* os;
		OutputStream* ost;
		InputStream* ist;
		int64_t timer_start;
		int64_t total_packets_written = 0;
		ret = transcode_init();
		if (ret < 0)
			goto fail;
		timer_start = av_gettime_relative();

		while (!m_bExit) { //转换过程
			if (!need_output()) {
				Log("No more output streams to write to, finishing.\n");
				break;
			}
			ret = transcode_step();
			if (ret < 0 && ret != AVERROR_EOF) {
				Log("Error while filtering: %s\n", _av_err2str(ret));
				break;
			}
			/* dump report by using the output first video and audio streams */
			for (int i = 0; i < nb_output_streams; i++) {
				OutputStream* ost = output_streams[i];
				if (av_stream_get_end_pts(ost->st) != AV_NOPTS_VALUE) {
					int64_t pts = INT64_MIN + 1;
					pts = FFMAX(pts, av_rescale_q(av_stream_get_end_pts(ost->st), ost->st->time_base, TimeBase_1_1));
					m_currTime = FFMAX(m_currTime, pts);//Curr Time 
				}
			}
		}  //转换过程

		for (i = 0; i < nb_input_streams; i++) {
			ist = input_streams[i];
			if (!input_files[ist->file_index]->eof_reached) {
				process_input_packet(ist, NULL, 0);
			}
		}
		flush_encoders();
		for (i = 0; i < nb_output_files; i++) {
			os = output_files[i]->ctx;
			if (!output_files[i]->header_written) {
				Log("Nothing was written into output file %d (%s), because "
					"at least one of its streams received no packets.\n",
					i, os->url);
				continue;
			}
			if ((ret = av_write_trailer(os)) < 0) {
				Log("Error writing trailer of %s: %s\n", os->url, _av_err2str(ret));
				if (exit_on_error)
					Stop();
			}
		}
		for (i = 0; i < nb_output_streams; i++) {
			ost = output_streams[i];
			if (ost->encoding_needed) {
				av_freep(&ost->enc_ctx->stats_in);
			}
			total_packets_written += ost->packets_written;
		}
		if (!total_packets_written && (abort_on_flags & ABORT_ON_FLAG_EMPTY_OUTPUT)) {
			Log("Empty output\n");
			Stop();
		}
		for (i = 0; i < nb_input_streams; i++) {
			ist = input_streams[i];
			if (ist->decoding_needed) {
				avcodec_close(ist->dec_ctx);
				if (ist->hwaccel_uninit)
					ist->hwaccel_uninit(ist->dec_ctx);
			}
		}
		av_buffer_unref(&hw_device_ctx);
		hw_device_free_all();
		ret = 0;
	fail:

		if (output_streams) {
			for (i = 0; i < nb_output_streams; i++) {
				ost = output_streams[i];
				if (ost) {
					if (ost->logfile) {
						if (fclose(ost->logfile))
							Log("Error closing logfile, loss of information possible: %s\n",
								_av_err2str(AVERROR(errno)));
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
	const OptionDef ffmpeg_options[180] = {
		{ "L", OPT_EXIT,{ (void*)show_help }, "show license" },
		{ "h", OPT_EXIT,{ (void*)show_help }, "show help", "topic" },
		{ "?", OPT_EXIT,{ (void*)show_help }, "show help", "topic" },
		{ "help", OPT_EXIT,{ (void*)show_help }, "show help", "topic" },
		{ "-help", OPT_EXIT,{ (void*)show_help }, "show help", "topic" },
		{ "version", OPT_EXIT,{ (void*)show_help }, "show version" },
		{ "buildconf", OPT_EXIT,{ (void*)show_help }, "show build configuration" },
		{ "formats", OPT_EXIT,{ (void*)show_help }, "show available formats" },
		{ "muxers", OPT_EXIT,{ (void*)show_help }, "show available muxers" },
		{ "demuxers", OPT_EXIT,{ (void*)show_help }, "show available demuxers" },
		{ "devices", OPT_EXIT,{ (void*)show_help }, "show available devices" },
		{ "codecs", OPT_EXIT,{ (void*)show_help }, "show available codecs" },
		{ "decoders", OPT_EXIT,{ (void*)show_help }, "show available decoders" },
		{ "encoders", OPT_EXIT,{ (void*)show_help }, "show available encoders" },
		{ "bsfs", OPT_EXIT,{ (void*)show_help }, "show available bit stream filters" },
		{ "protocols", OPT_EXIT,{ (void*)show_help }, "show available protocols" },
		{ "filters", OPT_EXIT,{ (void*)show_help }, "show available filters" },
		{ "pix_fmts", OPT_EXIT,{ (void*)show_help }, "show available pixel formats" },
		{ "layouts", OPT_EXIT,{ (void*)show_help }, "show standard channel layouts" },
		{ "sample_fmts", OPT_EXIT,{ (void*)show_help }, "show available audio sample formats" },
		{ "colors", OPT_EXIT,{ (void*)show_help }, "show available color names" },
		{ "loglevel", HAS_ARG,{ (void*)opt_loglevel }, "set logging level", "loglevel" },
		{ "v", HAS_ARG,{ (void*)opt_loglevel }, "set logging level", "loglevel" },
		{ "report", 0,{ (void*)opt_report }, "generate a report" },
		{ "max_alloc", HAS_ARG,{ (void*)opt_max_alloc }, "set maximum size of a single allocated block", "bytes" },
		{ "cpuflags", HAS_ARG | OPT_EXPERT,{ (void*)opt_cpuflags }, "force specific cpu flags", "flags" },
		{ "hide_banner", OPT_BOOL | OPT_EXPERT,{ &hide_banner }, "do not show program banner", "hide_banner" },
		{ "f", HAS_ARG | OPT_STRING | OPT_OFFSET | OPT_INPUT | OPT_OUTPUT,{ (void*)offsetof(OptionsContext, format) }, "force format", "fmt" },
		{ "y", OPT_BOOL,{ &file_overwrite }, "overwrite output files" },
		{ "n", OPT_BOOL,{ &no_file_overwrite }, "never overwrite output files" },
		{ "ignore_unknown", OPT_BOOL,{ &ignore_unknown_streams }, "Ignore unknown stream types" },
		{ "copy_unknown", OPT_BOOL | OPT_EXPERT,{ &copy_unknown_streams },"Copy unknown stream types" },
		{ "c", HAS_ARG | OPT_STRING | OPT_SPEC | OPT_INPUT | OPT_OUTPUT,{ (void*)offsetof(OptionsContext, codec_names) }, "codec name (VideoCodec c:v AudioCodec c:a)", "codec" },
		{ "codec", HAS_ARG | OPT_STRING | OPT_SPEC | OPT_INPUT | OPT_OUTPUT,{ (void*)offsetof(OptionsContext, codec_names) }, "codec name", "codec" },
		{ "pre", HAS_ARG | OPT_STRING | OPT_SPEC | OPT_OUTPUT,{ (void*)offsetof(OptionsContext, presets) },"preset name", "preset" },
		{ "map", HAS_ARG | OPT_EXPERT | OPT_PERFILE | OPT_OUTPUT,{ (void*)opt_map },"set input stream mapping","[-]input_file_id[:stream_specifier][,sync_file_id[:stream_specifier]]" },
		{ "map_channel", HAS_ARG | OPT_EXPERT | OPT_PERFILE | OPT_OUTPUT,{ (void*)opt_map_channel },"map an audio channel from one stream to another", "file.stream.channel[:syncfile.syncstream]" },
		{ "map_metadata", HAS_ARG | OPT_STRING | OPT_SPEC | OPT_OUTPUT,{ (void*)offsetof(OptionsContext, metadata_map) },"set metadata information of outfile from infile","outfile[,metadata]:infile[,metadata]" },
		{ "map_chapters", HAS_ARG | OPT_INT | OPT_EXPERT | OPT_OFFSET | OPT_OUTPUT,{ (void*)offsetof(OptionsContext, chapters_input_file) },"set chapters mapping", "input_file_index" },
		{ "t", HAS_ARG | OPT_TIME | OPT_OFFSET | OPT_INPUT | OPT_OUTPUT,{ (void*)offsetof(OptionsContext, recording_time) },"record or transcode \"duration\" seconds of audio/video","duration" },
		{ "to", HAS_ARG | OPT_TIME | OPT_OFFSET | OPT_INPUT | OPT_OUTPUT,{ (void*)offsetof(OptionsContext, stop_time) },"record or transcode stop time", "time_stop" },
		{ "fs", HAS_ARG | OPT_INT64 | OPT_OFFSET | OPT_OUTPUT,{ (void*)offsetof(OptionsContext, limit_filesize) },"set the limit file size in bytes", "limit_size" },
		{ "ss", HAS_ARG | OPT_TIME | OPT_OFFSET | OPT_INPUT | OPT_OUTPUT,{ (void*)offsetof(OptionsContext, start_time) },"set the start time offset", "time_off" },
		{ "sseof", HAS_ARG | OPT_TIME | OPT_OFFSET | OPT_INPUT,{ (void*)offsetof(OptionsContext, start_time_eof) },"set the start time offset relative to EOF", "time_off" },
		{ "seek_timestamp", HAS_ARG | OPT_INT | OPT_OFFSET | OPT_INPUT,{ (void*)offsetof(OptionsContext, seek_timestamp) },"enable/disable seeking by timestamp with -ss" },
		{ "accurate_seek", OPT_BOOL | OPT_OFFSET | OPT_EXPERT | OPT_INPUT,{ (void*)offsetof(OptionsContext, accurate_seek) },"enable/disable accurate seeking with -ss" },
		{ "itsoffset", HAS_ARG | OPT_TIME | OPT_OFFSET | OPT_EXPERT | OPT_INPUT,{ (void*)offsetof(OptionsContext, input_ts_offset) },"set the input ts offset", "time_off" },
		{ "itsscale", HAS_ARG | OPT_DOUBLE | OPT_SPEC | OPT_EXPERT | OPT_INPUT,{ (void*)offsetof(OptionsContext, ts_scale) },"set the input ts scale", "scale" },
		{ "timestamp", HAS_ARG | OPT_PERFILE | OPT_OUTPUT,{ (void*)opt_recording_timestamp },"set the recording timestamp ('now' to set the current time)", "time" },
		{ "metadata", HAS_ARG | OPT_STRING | OPT_SPEC | OPT_OUTPUT,{ (void*)offsetof(OptionsContext, metadata) },"add metadata", "string=string" },
		{ "program", HAS_ARG | OPT_STRING | OPT_SPEC | OPT_OUTPUT,{ (void*)offsetof(OptionsContext, program) },"add program with specified streams", "title=string:st=number..." },
		{ "dframes", HAS_ARG | OPT_PERFILE | OPT_EXPERT | OPT_OUTPUT,{ (void*)opt_data_frames },"set the number of data frames to output", "number" },
		{ "benchmark", OPT_BOOL | OPT_EXPERT,{ &do_benchmark },"add timings for benchmarking" },
		{ "benchmark_all", OPT_BOOL | OPT_EXPERT,{ &do_benchmark_all },"add timings for each task" },
		{ "progress", HAS_ARG | OPT_EXPERT,{ (void*)opt_progress },"write program-readable progress information", "url" },
		{ "stdin", OPT_BOOL | OPT_EXPERT,{ &stdin_interaction },"enable or disable interaction on standard input" },
		{ "timelimit", HAS_ARG | OPT_EXPERT,{ (void*)opt_timelimit },"set max runtime in seconds", "limit" },
		{ "dump", OPT_BOOL | OPT_EXPERT,{ &do_pkt_dump },"dump each input packet" },
		{ "hex", OPT_BOOL | OPT_EXPERT,{ &do_hex_dump },"when dumping packets, also dump the payload" },
		{ "re", OPT_BOOL | OPT_EXPERT | OPT_OFFSET | OPT_INPUT,{ (void*)offsetof(OptionsContext, rate_emu) },"read input at native frame rate", "" },
		{ "target", HAS_ARG | OPT_PERFILE | OPT_OUTPUT,{ (void*)opt_target },"specify target file type (\"vcd\", \"svcd\", \"dvd\", \"dv\" or \"dv50\" ""with optional prefixes \"pal-\", \"ntsc-\" or \"film-\")", "type" },
		{ "vsync", HAS_ARG | OPT_EXPERT,{ (void*)opt_vsync },"video sync method", "" },
		{ "frame_drop_threshold", HAS_ARG | OPT_FLOAT | OPT_EXPERT,{ &frame_drop_threshold },"frame drop threshold", "" },
		{ "async", HAS_ARG | OPT_INT | OPT_EXPERT,{ &audio_sync_method },"audio sync method", "" },
		{ "adrift_threshold", HAS_ARG | OPT_FLOAT | OPT_EXPERT,{ &audio_drift_threshold },"audio drift threshold", "threshold" },
		{ "copyts", OPT_BOOL | OPT_EXPERT,{ &copy_ts },"copy timestamps" },
		{ "start_at_zero", OPT_BOOL | OPT_EXPERT,{ &start_at_zero },"shift input timestamps to start at 0 when using copyts" },
		{ "copytb", HAS_ARG | OPT_INT | OPT_EXPERT,{ &copy_tb },"copy input stream time base when stream copying", "mode" },
		{ "shortest", OPT_BOOL | OPT_EXPERT | OPT_OFFSET | OPT_OUTPUT,{ (void*)offsetof(OptionsContext, shortest) },"finish encoding within shortest input" },
		{ "bitexact", OPT_BOOL | OPT_EXPERT | OPT_OFFSET | OPT_OUTPUT | OPT_INPUT,{ (void*)offsetof(OptionsContext, bitexact) },"bitexact mode" },
		{ "apad", OPT_STRING | HAS_ARG | OPT_SPEC | OPT_OUTPUT,{ (void*)offsetof(OptionsContext, apad) },"audio pad", "" },
		{ "dts_delta_threshold", HAS_ARG | OPT_FLOAT | OPT_EXPERT,{ &dts_delta_threshold },"timestamp discontinuity delta threshold", "threshold" },
		{ "dts_error_threshold", HAS_ARG | OPT_FLOAT | OPT_EXPERT,{ &dts_error_threshold },"timestamp error delta threshold", "threshold" },
		{ "xerror", OPT_BOOL | OPT_EXPERT,{ &exit_on_error },"exit on error", "error" },
		{ "abort_on", HAS_ARG | OPT_EXPERT,{ (void*)opt_abort_on },"abort on the specified condition flags", "flags" },
		{ "copyinkf", OPT_BOOL | OPT_EXPERT | OPT_SPEC | OPT_OUTPUT,{ (void*)offsetof(OptionsContext, copy_initial_nonkeyframes) },"copy initial non-keyframes" },
		{ "copypriorss", OPT_INT | HAS_ARG | OPT_EXPERT | OPT_SPEC | OPT_OUTPUT,{ (void*)offsetof(OptionsContext, copy_prior_start) },"copy or discard frames before start time" },
		{ "frames", OPT_INT64 | HAS_ARG | OPT_SPEC | OPT_OUTPUT,{ (void*)offsetof(OptionsContext, max_frames) },"set the number of frames to output", "number" },
		{ "tag", OPT_STRING | HAS_ARG | OPT_SPEC | OPT_EXPERT | OPT_OUTPUT | OPT_INPUT,{ (void*)offsetof(OptionsContext, codec_tags) },"force codec tag/fourcc", "fourcc/tag" },
		{ "q", HAS_ARG | OPT_EXPERT | OPT_DOUBLE | OPT_SPEC | OPT_OUTPUT,{ (void*)offsetof(OptionsContext, qscale) },"use fixed quality scale (VBR)", "q" },
		{ "qscale", HAS_ARG | OPT_EXPERT | OPT_PERFILE | OPT_OUTPUT,{ (void*)opt_qscale },"use fixed quality scale (VBR)", "q" },
		{ "profile", HAS_ARG | OPT_EXPERT | OPT_PERFILE | OPT_OUTPUT,{ (void*)opt_profile },"set profile", "profile" },
		{ "filter", HAS_ARG | OPT_STRING | OPT_SPEC | OPT_OUTPUT,{ (void*)offsetof(OptionsContext, filters) },"set stream filtergraph", "filter_graph" },
		{ "filter_threads", HAS_ARG | OPT_INT,{ &filter_nbthreads },"number of non-complex filter threads" },
		{ "filter_script", HAS_ARG | OPT_STRING | OPT_SPEC | OPT_OUTPUT,{ (void*)offsetof(OptionsContext, filter_scripts) },"read stream filtergraph description from a file", "filename" },
		{ "reinit_filter", HAS_ARG | OPT_INT | OPT_SPEC | OPT_INPUT,{ (void*)offsetof(OptionsContext, reinit_filters) },"reinit filtergraph on input parameter changes", "" },
		{ "filter_complex", HAS_ARG | OPT_EXPERT,{(void*)opt_filter_complex },"create a complex filtergraph", "graph_description" },
		{ "filter_complex_threads", HAS_ARG | OPT_INT,{ &filter_complex_nbthreads },"number of threads for -filter_complex" },
		{ "lavfi", HAS_ARG | OPT_EXPERT,{ (void*)opt_filter_complex },"create a complex filtergraph", "graph_description" },
		{ "filter_complex_script", HAS_ARG | OPT_EXPERT,{ (void*)opt_filter_complex_script },"read complex filtergraph description from a file", "filename" },
		{ "stats", OPT_BOOL,{ &print_stats },"print progress report during encoding", },
		{ "attach", HAS_ARG | OPT_PERFILE | OPT_EXPERT | OPT_OUTPUT,{ (void*)opt_attach },"add an attachment to the output file", "filename" },
		{ "dump_attachment", HAS_ARG | OPT_STRING | OPT_SPEC | OPT_EXPERT | OPT_INPUT,{ (void*)offsetof(OptionsContext, dump_attachment) },"extract an attachment into a file", "filename" },
		{ "stream_loop", OPT_INT | HAS_ARG | OPT_EXPERT | OPT_INPUT | OPT_OFFSET,{ (void*)offsetof(OptionsContext, loop) }, "set number of times input stream shall be looped", "loop count" },
		{ "debug_ts", OPT_BOOL | OPT_EXPERT,{ &debug_ts },"print timestamp debugging info" },
		{ "max_error_rate", HAS_ARG | OPT_FLOAT,{ &max_error_rate },"ratio of errors (0.0: no errors, 1.0: 100% errors) above which ffmpeg returns an error instead of success.", "maximum error rate" },
		{ "discard", OPT_STRING | HAS_ARG | OPT_SPEC | OPT_INPUT,{ (void*)offsetof(OptionsContext, discard) },"discard", "" },
		{ "disposition", OPT_STRING | HAS_ARG | OPT_SPEC | OPT_OUTPUT,{ (void*)offsetof(OptionsContext, disposition) },"disposition", "" },
		{ "thread_queue_size", HAS_ARG | OPT_INT | OPT_OFFSET | OPT_EXPERT | OPT_INPUT,{ (void*)offsetof(OptionsContext, thread_queue_size) },"set the maximum number of queued packets from the demuxer" },
		{ "find_stream_info", OPT_BOOL | OPT_PERFILE | OPT_INPUT | OPT_EXPERT,{ &find_stream_info },"read and decode the streams to fill missing information with heuristics" },
		{ "vframes", OPT_VIDEO | HAS_ARG | OPT_PERFILE | OPT_OUTPUT,{ (void*)opt_video_frames },"set the number of video frames to output", "number" },
		{ "r", OPT_VIDEO | HAS_ARG | OPT_STRING | OPT_SPEC | OPT_INPUT | OPT_OUTPUT,{ (void*)offsetof(OptionsContext, frame_rates) },"set frame rate (Hz value, fraction or abbreviation)", "rate" },
		{ "s", OPT_VIDEO | HAS_ARG | OPT_SUBTITLE | OPT_STRING | OPT_SPEC | OPT_INPUT | OPT_OUTPUT,{ (void*)offsetof(OptionsContext, frame_sizes) },"set frame size (WxH or abbreviation)", "size" },
		{ "aspect", OPT_VIDEO | HAS_ARG | OPT_STRING | OPT_SPEC | OPT_OUTPUT,{ (void*)offsetof(OptionsContext, frame_aspect_ratios) },"set aspect ratio (4:3, 16:9 or 1.3333, 1.7777)", "aspect" },
		{ "pix_fmt", OPT_VIDEO | HAS_ARG | OPT_EXPERT | OPT_STRING | OPT_SPEC | OPT_INPUT | OPT_OUTPUT,{ (void*)offsetof(OptionsContext, frame_pix_fmts) },"set pixel format", "format" },
		{ "bits_per_raw_sample", OPT_VIDEO | OPT_INT | HAS_ARG,{ &frame_bits_per_raw_sample },"set the number of bits per raw sample", "number" },
		{ "intra", OPT_VIDEO | OPT_BOOL | OPT_EXPERT,{ &intra_only },"deprecated use -g 1" },
		{ "vn", OPT_VIDEO | OPT_BOOL | OPT_OFFSET | OPT_INPUT | OPT_OUTPUT,{ (void*)offsetof(OptionsContext, video_disable) },"disable video" },
		{ "rc_override", OPT_VIDEO | HAS_ARG | OPT_EXPERT | OPT_STRING | OPT_SPEC | OPT_OUTPUT,{ (void*)offsetof(OptionsContext, rc_overrides) },"rate control override for specific intervals", "override" },
		{ "vcodec", OPT_VIDEO | HAS_ARG | OPT_PERFILE | OPT_INPUT | OPT_OUTPUT,{ (void*)opt_video_codec },"force video codec ('copy' to copy stream)", "codec" },
		{ "sameq", OPT_VIDEO | OPT_EXPERT ,{ (void*)opt_sameq },"Removed" },
		{ "same_quant", OPT_VIDEO | OPT_EXPERT ,{ (void*)opt_sameq },"Removed" },
		{ "timecode", OPT_VIDEO | HAS_ARG | OPT_PERFILE | OPT_OUTPUT,{ (void*)opt_timecode },"set initial TimeCode value.", "hh:mm:ss[:;.]ff" },
		{ "pass", OPT_VIDEO | HAS_ARG | OPT_SPEC | OPT_INT | OPT_OUTPUT,{ (void*)offsetof(OptionsContext, pass) },"select the pass number (1 to 3)", "n" },
		{ "passlogfile", OPT_VIDEO | HAS_ARG | OPT_STRING | OPT_EXPERT | OPT_SPEC | OPT_OUTPUT,{ (void*)offsetof(OptionsContext, passlogfiles) },"select two pass log file name prefix", "prefix" },
		{ "deinterlace", OPT_VIDEO | OPT_BOOL | OPT_EXPERT,{ &do_deinterlace },"this option is deprecated, use the yadif filter instead" },
		{ "psnr", OPT_VIDEO | OPT_BOOL | OPT_EXPERT,{ &do_psnr },"calculate PSNR of compressed frames" },
		{ "vstats", OPT_VIDEO | OPT_EXPERT ,{ (void*)opt_vstats },"dump video coding statistics to file" },
		{ "vstats_file", OPT_VIDEO | HAS_ARG | OPT_EXPERT ,{(void*)opt_vstats_file },"dump video coding statistics to file", "file" },
		{ "vstats_version", OPT_VIDEO | OPT_INT | HAS_ARG | OPT_EXPERT ,{ &vstats_version },"Version of the vstats format to use." },
		{ "vf", OPT_VIDEO | HAS_ARG | OPT_PERFILE | OPT_OUTPUT,{ (void*)opt_video_filters },"set video filters", "filter_graph" },
		{ "intra_matrix", OPT_VIDEO | HAS_ARG | OPT_EXPERT | OPT_STRING | OPT_SPEC | OPT_OUTPUT,{ (void*)offsetof(OptionsContext, intra_matrices) },"specify intra matrix coeffs", "matrix" },
		{ "inter_matrix", OPT_VIDEO | HAS_ARG | OPT_EXPERT | OPT_STRING | OPT_SPEC | OPT_OUTPUT,{ (void*)offsetof(OptionsContext, inter_matrices) },"specify inter matrix coeffs", "matrix" },
		{ "chroma_intra_matrix", OPT_VIDEO | HAS_ARG | OPT_EXPERT | OPT_STRING | OPT_SPEC | OPT_OUTPUT,{ (void*)offsetof(OptionsContext, chroma_intra_matrices) },"specify intra matrix coeffs", "matrix" },
		{ "top", OPT_VIDEO | HAS_ARG | OPT_EXPERT | OPT_INT | OPT_SPEC | OPT_INPUT | OPT_OUTPUT,{ (void*)offsetof(OptionsContext, top_field_first) },"top=1/bottom=0/auto=-1 field first", "" },
		{ "vtag", OPT_VIDEO | HAS_ARG | OPT_EXPERT | OPT_PERFILE | OPT_INPUT | OPT_OUTPUT,{ (void*)opt_old2new },"force video tag/fourcc", "fourcc/tag" },
		{ "qphist", OPT_VIDEO | OPT_BOOL | OPT_EXPERT ,{ &qp_hist },"show QP histogram" },
		{ "force_fps", OPT_VIDEO | OPT_BOOL | OPT_EXPERT | OPT_SPEC | OPT_OUTPUT,{ (void*)offsetof(OptionsContext, force_fps) },"force the selected framerate, disable the best supported framerate selection" },
		{ "streamid", OPT_VIDEO | HAS_ARG | OPT_EXPERT | OPT_PERFILE | OPT_OUTPUT,{ (void*)opt_streamid },"set the value of an outfile streamid", "streamIndex:value" },
		{ "force_key_frames", OPT_VIDEO | OPT_STRING | HAS_ARG | OPT_EXPERT | OPT_SPEC | OPT_OUTPUT,{ (void*)offsetof(OptionsContext, forced_key_frames) },"force key frames at specified timestamps", "timestamps" },
		{ "ab", OPT_VIDEO | HAS_ARG | OPT_PERFILE | OPT_OUTPUT,{ (void*)opt_bitrate }, "audio bitrate (please use -b:a)", "bitrate" },
		{ "b",  OPT_VIDEO | HAS_ARG | OPT_PERFILE | OPT_OUTPUT,{ (void*)opt_bitrate },  "video bitrate (please use -b:v)/audio bitrate (please use -b:a)", "bitrate" },
		{ "hwaccel", OPT_VIDEO | OPT_STRING | HAS_ARG | OPT_EXPERT | OPT_SPEC | OPT_INPUT,{ (void*)offsetof(OptionsContext, hwaccels) },"use HW accelerated decoding", "hwaccel name" },
		{ "hwaccel_device", OPT_VIDEO | OPT_STRING | HAS_ARG | OPT_EXPERT | OPT_SPEC | OPT_INPUT,{ (void*)offsetof(OptionsContext, hwaccel_devices) },"select a device for HW acceleration", "devicename" },
		{ "hwaccel_output_format", OPT_VIDEO | OPT_STRING | HAS_ARG | OPT_EXPERT | OPT_SPEC | OPT_INPUT,{ (void*)offsetof(OptionsContext, hwaccel_output_formats) },"select output format used with HW accelerated decoding", "format" },
		{ "hwaccels", OPT_EXIT,{ (void*)show_help },"show available HW acceleration methods" },
		{ "autorotate", HAS_ARG | OPT_BOOL | OPT_SPEC | OPT_EXPERT | OPT_INPUT,{ (void*)offsetof(OptionsContext, autorotate) },"automatically insert correct rotate filters" },
		{ "aframes", OPT_AUDIO | HAS_ARG | OPT_PERFILE | OPT_OUTPUT,{ (void*)opt_audio_frames },"set the number of audio frames to output", "number" },
		{ "aq", OPT_AUDIO | HAS_ARG | OPT_PERFILE | OPT_OUTPUT,{ (void*)opt_audio_qscale },"set audio quality (codec-specific)", "quality", },
		{ "ar", OPT_AUDIO | HAS_ARG | OPT_INT | OPT_SPEC | OPT_INPUT | OPT_OUTPUT,{ (void*)offsetof(OptionsContext, audio_sample_rate) },"set audio sampling rate (in Hz)", "rate" },
		{ "ac", OPT_AUDIO | HAS_ARG | OPT_INT | OPT_SPEC | OPT_INPUT | OPT_OUTPUT,{ (void*)offsetof(OptionsContext, audio_channels) },"set number of audio channels", "channels" },
		{ "an", OPT_AUDIO | OPT_BOOL | OPT_OFFSET | OPT_INPUT | OPT_OUTPUT,{ (void*)offsetof(OptionsContext, audio_disable) },"disable audio" },
		{ "acodec", OPT_AUDIO | HAS_ARG | OPT_PERFILE | OPT_INPUT | OPT_OUTPUT,{ (void*)opt_audio_codec },"force audio codec ('copy' to copy stream)", "codec" },
		{ "atag", OPT_AUDIO | HAS_ARG | OPT_EXPERT | OPT_PERFILE | OPT_OUTPUT,{(void*)opt_old2new },"force audio tag/fourcc", "fourcc/tag" },
		{ "vol", OPT_AUDIO | HAS_ARG | OPT_INT,{ &audio_volume },"change audio volume (256=normal)" , "volume" },
		{ "sample_fmt", OPT_AUDIO | HAS_ARG | OPT_EXPERT | OPT_SPEC | OPT_STRING | OPT_INPUT | OPT_OUTPUT,{ (void*)offsetof(OptionsContext, sample_fmts) },"set sample format", "format" },
		{ "channel_layout", OPT_AUDIO | HAS_ARG | OPT_EXPERT | OPT_PERFILE | OPT_INPUT | OPT_OUTPUT,{ (void*)opt_channel_layout },"set channel layout", "layout" },
		{ "af", OPT_AUDIO | HAS_ARG | OPT_PERFILE | OPT_OUTPUT,{ (void*)opt_audio_filters },"set audio filters", "filter_graph" },
		{ "guess_layout_max", OPT_AUDIO | HAS_ARG | OPT_INT | OPT_SPEC | OPT_EXPERT | OPT_INPUT,{ (void*)offsetof(OptionsContext, guess_layout_max) },"set the maximum number of channels to try to guess the channel layout" },
		{ "sn", OPT_SUBTITLE | OPT_BOOL | OPT_OFFSET | OPT_INPUT | OPT_OUTPUT,{ (void*)offsetof(OptionsContext, subtitle_disable) },"disable subtitle" },
		{ "scodec", OPT_SUBTITLE | HAS_ARG | OPT_PERFILE | OPT_INPUT | OPT_OUTPUT,{ (void*)opt_subtitle_codec },"force subtitle codec ('copy' to copy stream)", "codec" },
		{ "stag", OPT_SUBTITLE | HAS_ARG | OPT_EXPERT | OPT_PERFILE | OPT_OUTPUT,{ (void*)opt_old2new }, "force subtitle tag/fourcc", "fourcc/tag" },
		{ "fix_sub_duration", OPT_BOOL | OPT_EXPERT | OPT_SUBTITLE | OPT_SPEC | OPT_INPUT,{ (void*)offsetof(OptionsContext, fix_sub_duration) },"fix subtitles duration" },
		{ "canvas_size", OPT_SUBTITLE | HAS_ARG | OPT_STRING | OPT_SPEC | OPT_INPUT,{ (void*)offsetof(OptionsContext, canvas_sizes) },"set canvas size (WxH or abbreviation)", "size" },
		{ "vc", HAS_ARG | OPT_EXPERT | OPT_VIDEO,{ (void*)opt_video_channel },"deprecated, use -channel", "channel" },
		{ "tvstd", HAS_ARG | OPT_EXPERT | OPT_VIDEO,{ (void*)opt_video_standard },"deprecated, use -standard", "standard" },
		{ "isync", OPT_BOOL | OPT_EXPERT,{ &input_sync }, "this option is deprecated and does nothing", "" },
		{ "muxdelay", OPT_FLOAT | HAS_ARG | OPT_EXPERT | OPT_OFFSET | OPT_OUTPUT,{ (void*)offsetof(OptionsContext, mux_max_delay) },"set the maximum demux-decode delay", "seconds" },
		{ "muxpreload", OPT_FLOAT | HAS_ARG | OPT_EXPERT | OPT_OFFSET | OPT_OUTPUT,{ (void*)offsetof(OptionsContext, mux_preload) },"set the initial demux-decode delay", "seconds" },
		{ "sdp_file", HAS_ARG | OPT_EXPERT | OPT_OUTPUT,{ (void*)opt_sdp_file },"specify a file in which to print sdp information", "file" },
		{ "time_base", HAS_ARG | OPT_STRING | OPT_EXPERT | OPT_SPEC | OPT_OUTPUT,{ (void*)offsetof(OptionsContext, time_bases) },"set the desired time base hint for output stream (1:24, 1:48000 or 0.04166, 2.0833e-5)", "ratio" },
		{ "enc_time_base", HAS_ARG | OPT_STRING | OPT_EXPERT | OPT_SPEC | OPT_OUTPUT,{ (void*)offsetof(OptionsContext, enc_time_bases) },
		"set the desired time base for the encoder (1:24, 1:48000 or 0.04166, 2.0833e-5). "
		"two special values are defined - "
		"0 = use frame rate (video) or sample rate (audio),"
		"-1 = match source time base", "ratio" },
		{ "bsf", HAS_ARG | OPT_STRING | OPT_SPEC | OPT_EXPERT | OPT_OUTPUT,{ (void*)offsetof(OptionsContext, bitstream_filters) },"A comma-separated list of bitstream filters", "bitstream_filters" },
		{ "absf", HAS_ARG | OPT_AUDIO | OPT_EXPERT | OPT_PERFILE | OPT_OUTPUT,{ (void*)opt_old2new },"deprecated", "audio bitstream_filters" },
		{ "vbsf", OPT_VIDEO | HAS_ARG | OPT_EXPERT | OPT_PERFILE | OPT_OUTPUT,{ (void*)opt_old2new },"deprecated", "video bitstream_filters" },
		{ "apre", HAS_ARG | OPT_AUDIO | OPT_EXPERT | OPT_PERFILE | OPT_OUTPUT,{ (void*)opt_preset },"set the audio options to the indicated preset", "preset" },
		{ "vpre", OPT_VIDEO | HAS_ARG | OPT_EXPERT | OPT_PERFILE | OPT_OUTPUT,{ (void*)opt_preset },"set the video options to the indicated preset", "preset" },
		{ "spre", HAS_ARG | OPT_SUBTITLE | OPT_EXPERT | OPT_PERFILE | OPT_OUTPUT,{ (void*)opt_preset },"set the subtitle options to the indicated preset", "preset" },
		{ "fpre", HAS_ARG | OPT_EXPERT | OPT_PERFILE | OPT_OUTPUT,{ (void*)opt_preset },"set options from indicated preset file", "filename" },
		{ "max_muxing_queue_size", HAS_ARG | OPT_INT | OPT_SPEC | OPT_EXPERT | OPT_OUTPUT,{ (void*)offsetof(OptionsContext, max_muxing_queue_size) },"maximum number of packets that can be buffered while waiting for all streams to initialize", "packets" },
		{ "dcodec", HAS_ARG | OPT_DATA | OPT_PERFILE | OPT_EXPERT | OPT_INPUT | OPT_OUTPUT,{ (void*)opt_data_codec },"force data codec ('copy' to copy stream)", "codec" },
		{ "dn", OPT_BOOL | OPT_VIDEO | OPT_OFFSET | OPT_INPUT | OPT_OUTPUT,{ (void*)offsetof(OptionsContext, data_disable) },"disable data" },

		{ "init_hw_device", HAS_ARG | OPT_EXPERT,{ (void*)opt_init_hw_device },"initialise hardware device", "args" },
		{ "filter_hw_device", HAS_ARG | OPT_EXPERT,{ (void*)opt_filter_hw_device },"set hardware device used when filtering", "device" },
		{ NULL, },
	};
	//需要UTF8字符串
	FfmpegExe() {
		av_log_set_callback(ms_log_callback);
		setvbuf(stderr, NULL, _IONBF, 0);
		avformat_network_init();
	}

	virtual ~FfmpegExe() {
		avformat_network_deinit();
	}

	void Exit() { //退出
		m_bExit = 1;
	}

	int64_t GetTotalTime() {
		return m_totalTime;
	}

	int64_t GetCurrTime() {
		return m_currTime;
	}


	int64_t m_totalTime = 0;//总时间长度，单位s
	int64_t m_currTime = 0;//当前处理的时间，单位s

	int m_bStart = 0;//是否开始
	int m_bRunning = 0;//是否正在运行
	int m_bProcessOK = 0;//运行是否成功


	int GetState() {
		if (!m_bStart) { //未開始
			return -2;
		}
		else if (m_bRunning) { //正在运行
			if (m_totalTime > 0) {
				int nRate = 100 * m_currTime / m_totalTime; //可以正在运行的时候返回100，会让界面误解处理完毕
                if(nRate >= 100)nRate = 99;
                return nRate;
			}else {
				return 0;
			}
		}
		else { // 执行完毕
			if (m_bProcessOK) //成功
				return 100;
			else
				return -1; //失败
		}
		return -2;
	}

	int Excute(int argc, char** argv) {
		m_bStart = 1;//开始转换
		m_bRunning = 1;//正在运行
		try {
			if (ffmpeg_parse_options(argc, argv) < 0) {
				Log("ffmpeg_parse_options error \n");
				Stop();
			}

			if (nb_output_files <= 0 && nb_input_files == 0) { //没有输入和输出
				Log("Use -h to get full help or, even better, run 'man ffmpeg'\n");
				Stop();
			}

			if (nb_output_files <= 0) { //没有指定输出
				Log("At least one output file must be specified\n");
				Stop();
			}

			for (int i = 0; i < nb_output_files; i++) {
				if (strcmp(output_files[i]->ctx->oformat->name, "rtp"))
					want_sdp = 0;
			}

			for (int i = 0; i < nb_input_files; i++) { //计算输出总时间
				if (input_files[i]->ctx->duration > 0) {
					m_totalTime += input_files[i]->ctx->duration / AV_TIME_BASE;
				}
			}


			if (transcode() < 0) {
				Stop();
			}

			ffmpeg_cleanup();
			m_bRunning = 0;
			m_bProcessOK = 1;
            Log(" TotalTime = %lld\r\n", m_totalTime);
			printf("------ MediaConver  OK!!!\r\n");
			return 1;
		}
		catch (...) {
			//异常后 Stop() 跳转到这里！！
			ffmpeg_cleanup(); //释放内存
			m_bRunning = 0;
			m_bProcessOK = 0;
            printf("------ MediaConver ERROR !!!!! \r\n");
			return 0;
		}
		return 0;
	}
};

static int  s_bInit = 0;//是否初始化成功
static char s_strError[4096];//错误信息

class MediaConvert {
    int m_nRotate = 0;//旋转角度
	int ParserFast(const char* strFileName) {
		AVFormatContext* ic = nullptr;
		int err = avformat_open_input(&ic, strFileName, nullptr, nullptr);//打开文件
		if (err < 0) {
			sprintf(s_strError, "[%s] avformat_open_input error", strFileName);
			return 0;
		}
		err = avformat_find_stream_info(ic, nullptr);//获取文件信息
		if (err < 0) {
			sprintf(s_strError, "[%s] avformat_find_stream_info error", strFileName);
			avformat_close_input(&ic);
			return 0;
		}

		if (ic->nb_streams) { //正常获取
			m_nFileTime = ic->duration / AV_TIME_BASE;

			for (int i = 0; i < ic->nb_streams; i++) {
				if (ic->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO && !(ic->streams[i]->disposition & AV_DISPOSITION_ATTACHED_PIC)) {
					//m_hasVideo = true;
					int64_t  nFrames = ic->streams[i]->nb_frames;//帧的总数量
					m_nWidth = ic->streams[i]->codec->width;
					m_nHeight = ic->streams[i]->codec->height;
                    printf("Media Size [%dx%d]\r\n",m_nWidth,m_nHeight);
					m_nFps = 24;
					if (ic->streams[i]->r_frame_rate.den) {
						m_nFps = (int)((double)ic->streams[i]->r_frame_rate.num / (double)ic->streams[i]->r_frame_rate.den + 0.5);
					}else if (nFrames > 0 && m_nFileTime > 0) { //计算平均帧率
						m_nFps = (double)nFrames / (double)m_nFileTime + 0.5;
					}
                    printf("Media FPS = %d\r\n",m_nFps);
                    AVDictionaryEntry *tag = av_dict_get(ic->streams[i]->metadata, "rotate", NULL, 0);
                    if (tag != NULL){
                        m_nRotate = atoi(tag->value);
                        m_nRotate %= 360;
                    }
                    printf("Media Rotate = %d\r\n",m_nRotate);
                    if(m_nRotate == 90 || m_nRotate == 270){
                        int temp = m_nWidth;
                        m_nWidth = m_nHeight;
                        m_nHeight = temp;
                        printf("nRotate=%d Swap size [%d,%d]\r\n",m_nRotate,m_nWidth,m_nHeight);
                    }
				}
				else if (ic->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
					//m_hasAudio = true;
				}
			}
			avformat_close_input(&ic);
			strcpy(s_strError, "NoError");
			return 1;//成功
		}
		else {
			sprintf(s_strError, "[%s] no Media data", strFileName);
			avformat_close_input(&ic);
			return 0;
		}
		return 0;
	}
public:
	int64_t  m_nFileTime = 0;//文件总长度， 单位为秒
	int      m_nFps = 0;//视频帧率
	int      m_nWidth = 0;//视频分辨率，宽度
	int      m_nHeight = 0;//视频分辨率，高度

	int m_bInit = 0;
	char m_strInput[4096]; //输入文件
	char m_strOutput[4096];//输出文件

	int m_nTargetWidth = 0;       //预设输出宽度
	int m_nTargetHeight = 0;      //预设输出高度
	int m_nTargetFps = 0;         //预设输出帧率

	int m_nTargetVideoBitrate = 0;//预设视频码率
	int m_nTargetAudioBitrate = 0;//预设音频码率

	int m_nTargetSize = 0;//预设文件大小 ，和m_nTargetVideoBitrate m_nTargetAudioBitrate 冲突，两者必有一个为0

	std::thread* m_thread = nullptr;//转换线程

	WXLocker m_mutex;
	FfmpegExe* m_obj = nullptr;

public:
	//计算预估文件大小
	int GetLength(int width, int height, int fps, int mode) {
		double ff = 1.0;
		if (width == 0) {
			ff *= m_nWidth;
		}
		else {
			ff *= width;
		}
		if (height == 0) {
			ff *= m_nHeight;
		}
		else {
			ff *= height;
		}
		if (fps == 0) {
			ff *= m_nFps;
		}
		else {
			ff *= fps;
		}

		ff *= 12.0;
		ff /= 1024.0;//每秒原始数据kbps

		double target_vb = 2000;
		double target_ab = 64;

		if (mode == MODE_LOW) {
			target_vb = ff / 200.0f;
		}	if (mode == MODE_NORMAL) {
			target_vb = ff / 300.0f;
		}	if (mode == MODE_HIGH) {
			target_vb = ff / 450.0f;
		}

		if (target_vb > 2500)
			target_ab = 128;
		else if (target_vb > 1000)
			target_ab = 96;
		else
			target_ab = 64;

		return  (m_nFileTime * (target_vb + target_ab)) / 8.0;
	}

	int Excute(int width, int height, int fps, int target_size, const char* strOutput) {
		WXAutoLock lck(m_mutex);
		if (m_obj == nullptr) {
			strcpy(m_strOutput, strOutput);
			m_nTargetWidth = width;       //预设输出宽度
			m_nTargetHeight = height;      //预设输出高度
			m_nTargetFps = fps;         //预设输出帧率
			m_nTargetVideoBitrate = 0;//预设视频码率
			m_nTargetAudioBitrate = 0;//预设音频码率
			m_nTargetSize = target_size;//预设文件大小
			m_thread = new std::thread(&MediaConvert::ExcuteImpl, this);
		}
		else {
			return 0;
		}
		return 1;
	}

	int GetLengthExt(int video_bitrate, int audio_bitrate) {
		return  (m_nFileTime * (video_bitrate + audio_bitrate)) / 8.0;
	}

	int ExcuteExt(int width, int height, int fps, int video_bitrate, int audio_bitrate, const char* strOutput) {
		WXAutoLock lck(m_mutex);

		if (m_obj == nullptr) {
			strcpy(m_strOutput, strOutput);
			m_nTargetWidth = width;       //预设输出宽度
			m_nTargetHeight = height;      //预设输出高度
			m_nTargetFps = fps;         //预设输出帧率
			m_nTargetVideoBitrate = video_bitrate;//预设视频码率
			m_nTargetAudioBitrate = audio_bitrate;//预设音频码率
			m_nTargetSize = 0;
			m_thread = new std::thread(&MediaConvert::ExcuteImpl, this);
		}
		else {
			return 0;
		}
		return 1;
	}

public:
	MediaConvert() {}
	virtual ~MediaConvert() {
		Interrupt();
	}
	//设置输入文件，解析文件成功返回1，失败返回0
	int ParserFile(const char* strInput) {
		WXAutoLock lck(m_mutex);
		strcpy(m_strInput, strInput);
		m_bInit = ParserFast(m_strInput);
		return m_bInit;
	}

	int  GetThumb(const char* strThumb) {
		WXAutoLock lck(m_mutex);
		char* strArgv[100];
		int nArgc = 0;
		strArgv[nArgc++] = (char*)"ffmpeg";
		strArgv[nArgc++] = (char*)"-ss";
		if (m_nFileTime > 100)
			strArgv[nArgc++] = (char*)"3.0";
		else
			strArgv[nArgc++] = (char*)"0.8";
		strArgv[nArgc++] = (char*)"-i";
		strArgv[nArgc++] = (char*)m_strInput;//输入文件
		strArgv[nArgc++] = (char*)"-r";
		strArgv[nArgc++] = (char*)"1";
		strArgv[nArgc++] = (char*)"-vframes";
		strArgv[nArgc++] = (char*)"1";
		strArgv[nArgc++] = (char*)"-an";
		strArgv[nArgc++] = (char*)"-f";
		strArgv[nArgc++] = (char*)"image2";
		strArgv[nArgc++] = (char*)strThumb;//输出文件

		FfmpegExe* obj = new FfmpegExe;
		int ret = obj->Excute(nArgc, strArgv);
		if (ret == 0) {
			//截图失败
			strcpy(s_strError, obj->m_szMsg);
		}
		delete obj;
		return ret;
	}

	int  GetState() { //获取进度
		if (m_obj) {
			return m_obj->GetState();
		}
		return m_nState;
	}

	void Interrupt() { //强制中断

		{
			WXAutoLock lck(m_mutex);
			if (m_obj) {
				m_obj->Exit();
			}
		}

		if (m_thread) {
			m_thread->join();
			delete m_thread;
			m_thread = nullptr;
            printf("MediaConvrt Thread Stop!!!\r\n");
		}
	}

	int m_nState = -2;//状态！
	void ExcuteImpl() { //异步执行执行ffmpeg转换处理

		{
			WXAutoLock lck(m_mutex);
			m_obj = new FfmpegExe;
		}
		m_nState = -2;

		char* strArgv[100];
		int nArgc = 0;
		strArgv[nArgc++] = (char*)"ffmpeg";
		strArgv[nArgc++] = (char*)"-i";
		strArgv[nArgc++] = (char*)m_strInput;//输入文件

		int len = (int)strlen(m_strOutput);
		const char* Last4 = m_strOutput + (len - 4);
		if (strnicmp(Last4, "webm", 4) != 0) {
			//非webm
			strArgv[nArgc++] = (char*)"-c:v";
			strArgv[nArgc++] = (char*)"h264";

			strArgv[nArgc++] = (char*)"-tune";
			strArgv[nArgc++] = (char*)"zerolatency";

			strArgv[nArgc++] = (char*)"-preset";
			strArgv[nArgc++] = (char*)"veryfast";

			strArgv[nArgc++] = (char*)"-c:a";
			strArgv[nArgc++] = (char*)"aac";
		}

		//指定分辨率
        char szSize[10];
		if (m_nTargetWidth > 0 && m_nTargetHeight > 0) {
            strArgv[nArgc++] = (char*)"-s";
			sprintf(szSize, "%dx%d", m_nTargetWidth, m_nTargetHeight);
			strArgv[nArgc++] = (char*)szSize;//输入文件
		}
    
		//指定帧率
        char szFps[10];
		if (m_nTargetFps > 0) {
			strArgv[nArgc++] = (char*)"-r";
			sprintf(szFps, "%d", m_nTargetFps);
			strArgv[nArgc++] = (char*)szFps;//输入文件
		}

		if (m_nTargetSize != 0) { //自己根据TargetSize来计算音视频的码率
			double kbps = (m_nTargetSize * 8.0) / m_nFileTime;//计算每秒数据量 kbps
			if (kbps > 2500 + 128) {
				m_nTargetAudioBitrate = 128;
				m_nTargetVideoBitrate = kbps - 128;
			}
			else if (kbps > 1000 + 96) {
				m_nTargetAudioBitrate = 96;
				m_nTargetVideoBitrate = kbps - 96;
			}
			else {
				m_nTargetAudioBitrate = 64;
				m_nTargetVideoBitrate = kbps - 64;
			}
		}

		strArgv[nArgc++] = (char*)"-b:v";
		char szBV[10];
		sprintf(szBV, "%dk", m_nTargetVideoBitrate);
		strArgv[nArgc++] = (char*)szBV;//输入文件

		strArgv[nArgc++] = (char*)"-b:a";
		char szAV[10];
		sprintf(szAV, "%dk", m_nTargetAudioBitrate);
		strArgv[nArgc++] = (char*)szAV;//输入文件
		strArgv[nArgc++] = (char*)m_strOutput;//输出文件

       // strArgv[nArgc++] = (char*)"-autorotate";//Auto Rotate
		char msg[4096];
		memset(msg, 0, 4096);
		for (int i = 0; i < nArgc; i++)
		{
			strcat(msg, " ");
			strcat(msg, strArgv[i]);
		}
		printf("%s\r\n",msg);
		//NSLog(@"FFMPEG --- %s", msg);
		m_nState = 0;
		int ret = m_obj->Excute(nArgc, strArgv);
		if (ret == 0) {  //耗时操作，等待结束
			strcpy(s_strError, m_obj->m_szMsg);
			m_nState = -1;//转换失败
		}
		else {
			m_nState = 100;//转换成功
		}

		{
			WXAutoLock lck(m_mutex);
			delete m_obj;
			m_obj = nullptr;
		}
	}

};

#ifdef __ANDROID__

//功能:获取执行错误信息
//参数:无
//返回值:返回执行错误信息
//说明:库全局操作，在其它方法调用失败时可以通过该方法获取错误信息
MC_API JNIEXPORT jstring Java_com_apowersoft_WXMedia_MediaConvert_GetLastError(JNIEnv* env, jobject obj) {
	LOGE("%s", __FUNCTION__);
	return env->NewStringUTF(s_strError);
}

static jclass contextClass;
static jclass signatureClass;
static jclass packageNameClass;
static jclass packageInfoClass;

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
	JNIEnv* env = NULL;
	jint result = -1;
	if (vm->GetEnv((void**)&env, JNI_VERSION_1_4) != JNI_OK)
		return result;
	contextClass = (jclass)env->NewGlobalRef((env)->FindClass("android/content/Context"));
	signatureClass = (jclass)env->NewGlobalRef((env)->FindClass("android/content/pm/Signature"));
	packageNameClass = (jclass)env->NewGlobalRef((env)->FindClass("android/content/pm/PackageManager"));
	packageInfoClass = (jclass)env->NewGlobalRef((env)->FindClass("android/content/pm/PackageInfo"));
	return JNI_VERSION_1_4;
}


//功能 : 视频转换压缩SDK 库初始化
//参数bundleID：应用包名
//返回值 : 成功为1 失败为0
MC_API JNIEXPORT jint Java_com_apowersoft_WXMedia_MediaConvert_LibraryInit(JNIEnv* env, jobject _obj, jobject contextObject) {
	LOGE("%s", __FUNCTION__);
	jmethodID getPackageManagerId = (env)->GetMethodID(contextClass, "getPackageManager", "()Landroid/content/pm/PackageManager;");
	jmethodID getPackageNameId = (env)->GetMethodID(contextClass, "getPackageName", "()Ljava/lang/String;");
	jmethodID signToStringId = (env)->GetMethodID(signatureClass, "toCharsString", "()Ljava/lang/String;");
	jmethodID getPackageInfoId = (env)->GetMethodID(packageNameClass, "getPackageInfo", "(Ljava/lang/String;I)Landroid/content/pm/PackageInfo;");
	jobject packageManagerObject = (env)->CallObjectMethod(contextObject, getPackageManagerId);
	jstring packNameString = (jstring)(env)->CallObjectMethod(contextObject, getPackageNameId);

	const char* strName = (char*)env->GetStringUTFChars(packNameString, 0);

	if (stricmp(strName, "cn.wps.moffice_eng") == 0) {
		LOGE("%s PackName is %s  Init OK", __FUNCTION__, strName);
		s_bInit = 1;
	}
	else if (stricmp(strName, "com.WXMedia.Test") == 0) {
		LOGE("%s PackName is %s  Init OK", __FUNCTION__, strName);
		s_bInit = 1;
	}
	else {
		LOGE("%s PackName is %s  Init Error", __FUNCTION__, strName);
		s_bInit = 0;
	}
	env->ReleaseStringUTFChars(packNameString, strName);
	return s_bInit;
}


//功能: 创建一个操作句柄
//参数 strInput : 表示要转换的视频源文件名
//返回值 : 文件解析成功返回非0值，解析失败返回0
MC_API JNIEXPORT jlong Java_com_apowersoft_WXMedia_MediaConvert_Create(JNIEnv* env, jobject _obj, jstring jstrInput) {
	LOGE("%s", __FUNCTION__);
	if (!s_bInit) {
		strcpy(s_strError, "MediaConvert Library is not Inited");
		return 0;
	}
	MediaConvert* obj = new MediaConvert;
	const char* strInput = (char*)env->GetStringUTFChars(jstrInput, 0);
	int ret = obj->Parser(strInput);
	env->ReleaseStringUTFChars(jstrInput, strInput);
	if (ret) {
		LOGE("%s  OK", __FUNCTION__);
		return (jlong)obj;
	}
	else {
		LOGE("%s ERROR", __FUNCTION__);
	}
	delete obj;
	return 0;
}

//功能: 获取缩略图，一般是JPG图像
//参数handle : 操作句柄，Create方法的返回值
//参数strThumb : 缩略图文件名
//返回值 : 成功返回1，失败返回0
MC_API JNIEXPORT jint Java_com_apowersoft_WXMedia_MediaConvert_GetThumb(JNIEnv* env, jobject _obj, jlong handle, jstring jstrThumb) {
	LOGE("%s", __FUNCTION__);
	if (handle != 0) {
		MediaConvert* obj = (MediaConvert*)handle;
		const char* strThumb = (char*)env->GetStringUTFChars(jstrThumb, 0);
		int ret = obj->GetThumb(strThumb);
		if (ret) {
			LOGE("%s  OK", __FUNCTION__);
		}
		else {
			LOGE("%s ERROR", __FUNCTION__);
		}
		env->ReleaseStringUTFChars(jstrThumb, strThumb);
		return ret;
	}
	else {
		strcpy(s_strError, "MediaConvert Handle is NULL");
	}
	return 0;
}

//功能: 获取视频源文件分辨率宽度
//参数handle : 操作句柄，Create方法的返回值
//返回值 : 返回视频源文件分辨率宽度
MC_API JNIEXPORT jint Java_com_apowersoft_WXMedia_MediaConvert_GetWidth(JNIEnv* env, jobject _obj, jlong handle) {
	LOGE("%s", __FUNCTION__);
	if (handle != 0) {
		MediaConvert* obj = (MediaConvert*)handle;
		return obj->m_nWidth;
	}
	else {
		strcpy(s_strError, "MediaConvert Handle is NULL");
	}
	return -1;
}

//功能: 获取视频源文件分辨率高度
//参数handle : 操作句柄，Create方法的返回值
//返回值 : 返回视频源文件分辨率高度
MC_API JNIEXPORT jint Java_com_apowersoft_WXMedia_MediaConvert_GetHeight(JNIEnv* env, jobject _obj, jlong handle) {
	LOGE("%s", __FUNCTION__);
	if (handle != 0) {
		MediaConvert* obj = (MediaConvert*)handle;
		return obj->m_nHeight;
	}
	else {
		strcpy(s_strError, "MediaConvert Handle is NULL");
	}
	return -1;
}

//功能: 获取视频源文件帧率
//参数handle : 操作句柄，Create方法的返回值
//返回值 : 返回视频源文件帧率
MC_API JNIEXPORT jint Java_com_apowersoft_WXMedia_MediaConvert_GetFps(JNIEnv* env, jobject _obj, jlong handle) {
	LOGE("%s", __FUNCTION__);
	if (handle != 0) {
		MediaConvert* obj = (MediaConvert*)handle;
		return obj->m_nFps;
	}
	else {
		strcpy(s_strError, "MediaConvert Handle is NULL");
	}
	return -1;
}

//功能             : 返回指定参数的视频压缩的输出文件预计大小
//参数 [handle]    : 操作句柄，Create的返回值
//参数 [width]     : 视频分辨率宽度,0表示原始宽度
//参数 [height]    : 视频分辨率高度,0表示原始高度
//参数 [fps]       : 视频分辨率高度,0表示原始帧率
//参数 [mode]      : 视频压缩模式
//返回值           : 输出文件预计大小，单位KB
//说明             : 用于计算指定分辨率对应几种模式的压缩文件的预估大小
//                 : 其中MODE_LOW    对应最大值
//                 : 其中MODE_NORMAL 对应默认值
//                 : 其中MODE_HIGH   对应最小值
//                 : 返回值可以用于Excute方法来指定视频压缩文件的预估大小
MC_API MC_API JNIEXPORT jint Java_com_apowersoft_WXMedia_MediaConvert_GetLength(JNIEnv* env, jobject _obj, jlong handle,
	jint width, jint height, jint fps, jint mode) {
	LOGE("%s", __FUNCTION__);
	if (handle != 0) {
		MediaConvert* obj = (MediaConvert*)handle;
		obj->GetLength(width, height, fps, mode);
		return 1;
	}
	else {
		strcpy(s_strError, "MediaConvert Handle is NULL");
	}
	return 0;
}

//功能             : 设置视频压缩的输出文件预计大小
//参数 [handle]    : 操作句柄，MediaConvertCreate的返回值
//参数 [width]     : 视频分辨率宽度,0表示原始宽度
//参数 [height]    : 视频分辨率高度,0表示原始高度
//参数 [fps]       : 视频分辨率高度,0表示原始帧率
//参数 [target_size]: 预设的输出文件大小，单位KB
//返回值           : 成功返回1，失败返回0
//说明             : 当预估大小超出GetLength最大最小时可能会转换失败
MC_API JNIEXPORT jint Java_com_apowersoft_WXMedia_MediaConvert_Excute(JNIEnv* env, jobject _obj, jlong handle,
	jint width, jint height, jint fps, jint target_size, jstring jstrOutput) {
	LOGE("%s", __FUNCTION__);
	if (handle != 0) {
		MediaConvert* obj = (MediaConvert*)handle;
		char* strOutput = (char*)env->GetStringUTFChars(jstrOutput, 0);
		int ret = obj->Excute(width, height, fps, target_size, strOutput);
		env->ReleaseStringUTFChars(jstrOutput, strOutput);
		return ret;
	}
	else {
		strcpy(s_strError, "MediaConvert Handle is NULL");
	}
	return -1;
}

//功能                : 返回指定参数的视频压缩的输出文件预计大小
//参数 [handle]       : 操作句柄，MediaConvertCreate的返回值
//参数 [video_bitrate]: 视频码率,单位kbps，建议不小于300，不大于8000
//参数 [audio_bitrate]: 视频码率,单位kbps，建议在64-128之间
//返回值              : 输出文件预计大小，单位KB
//说明                : 用于计算指定分辨率对应几种模式的压缩文件的预估大小
//                    : 返回值可以用于显示ExcuteExt方法的预估大小
MC_API JNIEXPORT jint Java_com_apowersoft_WXMedia_MediaConvert_GetLengthExt(JNIEnv* env, jobject _obj, jlong handle,
	jint video_bitrate, jint audio_bitrate) {
	if (handle != 0) {
		MediaConvert* obj = (MediaConvert*)handle;
		return obj->GetLengthExt(video_bitrate, audio_bitrate);
	}
	else {
		strcpy(s_strError, "MediaConvert Handle is NULL");
	}
	return 0;
}

//功能             : 返回指定分辨率和压缩模式的视频压缩的输出文件预计大小
//参数 [ptr]       : 操作句柄，MediaConvertCreate的返回值
//参数 [width]     : 视频分辨率宽度,0表示原始宽度
//参数 [height]    : 视频分辨率高度,0表示原始高度
//参数 [fps]       : 视频分辨率高度,0表示原始帧率
//参数 [video_bitrate]: 视频码率,单位kbps，建议不小于300，不大于8000
//参数 [audio_bitrate]: 视频码率,单位kbps，建议在64-128之间
//返回值           : 成功返回1，失败返回0
//说明             : 过大或者过小的音视频码率的设置可能会导致专门失败，请尝试使用Excute重新转码
MC_API JNIEXPORT jint Java_com_apowersoft_WXMedia_MediaConvert_ExcuteExt(JNIEnv* env, jobject _obj, jlong handle,
	jint width, jint height, jint fps, jint video_bitrate, jint audio_bitrate, jstring jstrOutput) {
	if (handle != 0) {
		MediaConvert* obj = (MediaConvert*)handle;
		char* strOutput = (char*)env->GetStringUTFChars(jstrOutput, 0);
		int ret = obj->ExcuteExt(width, height, fps, video_bitrate, audio_bitrate, strOutput);
		env->ReleaseStringUTFChars(jstrOutput, strOutput);
		return ret;
	}
	else {
		strcpy(s_strError, "MediaConvert Handle is NULL");
	}
	return 0;
}

//功能: 强制中断转换进程
//参数handle : 操作句柄，Create方法的返回值
//返回值 : 成功返回1，失败返回0
//说明 : 执行后也需要不断调用MediaConvertGetState获取执行进度， 获得返回值为 - 1 或者 100 才可以调用MediaConvertDestroy销毁句柄
MC_API JNIEXPORT jint Java_com_apowersoft_WXMedia_MediaConvert_Interrupt(JNIEnv* env, jobject _obj, jlong handle) {
	if (handle != 0) {
		MediaConvert* obj = (MediaConvert*)handle;
		obj->Interrupt();
		return 1;
	}
	else {
		strcpy(s_strError, "MediaConvert Handle is NULL");
	}
	return 0;
}

//功能: 获取转换进度
//参数handle : 操作句柄，Create方法的返回值
//返回值 : -1 表示转换失败
//100表示转换成功
//0 - 99 表示转换进度百分比
//说明 : 获得返回值为 - 1 或者 100 才可以调用MediaConvertDestroy销毁句柄
MC_API JNIEXPORT jint Java_com_apowersoft_WXMedia_MediaConvert_GetState(JNIEnv* env, jobject _obj, jlong handle) {
	if (handle != 0) {
		MediaConvert* obj = (MediaConvert*)handle;
		return obj->GetState();
	}
	else {
		strcpy(s_strError, "MediaConvert Handle is NULL");
	}
	return -1;
}

//功能: 销毁转换句柄
//参数handle : 操作句柄，Create方法的返回值
//返回值 :成功返回1，失败返回0
//说明 : 可以等到GetState 返回 -1 或者100再销毁，建议先调用Interrupt再销毁
MC_API JNIEXPORT jint Java_com_apowersoft_WXMedia_MediaConvert_Destroy(JNIEnv* env, jobject _obj, jlong handle) {
	LOGE("%s", __FUNCTION__);
	if (handle != 0) {
		MediaConvert* obj = (MediaConvert*)handle;
		delete obj;
		return 1;
	}
	else {
		strcpy(s_strError, "MediaConvert Handle is NULL");
	}
	return 0;
}

#else

//功能:获取执行错误信息
//参数:无
//返回值:返回执行错误信息
//说明:库全局操作，在其它方法调用失败时可以通过该方法获取错误信息
MC_API const char* MediaConvertGetLastError() {
	return s_strError;
}

//功能 : 视频转换压缩SDK 库初始化
//参数[bundleID]：应用包名
//返回值 : 成功为1 失败为0
#import <Foundation/Foundation.h>
static char s_strBundleID[4096] = "com.kingsoft.www.office.wpsoffice";//包名
MC_API int MediaConvertLibraryInit() {
#ifdef _WIN32
	s_bInit = 1;
#else  //APPLE
	NSString* ID = [[NSBundle mainBundle]bundleIdentifier];
	NSLog(@"App ID is %@", ID);
	const char* strID = [ID UTF8String];
	if (stricmp(strID, s_strBundleID) == 0) {
		s_bInit = 1;
		NSLog(@"%s OK", __FUNCTION__);
	}
	else if (stricmp(strID, "com.xktspace.MCTest") == 0) {
		s_bInit = 1;
		NSLog(@"%s OK", __FUNCTION__);
	}
	else {
		s_bInit = 0;
		NSLog(@"%s ERROR", __FUNCTION__);
	}
#endif
	return s_bInit;
}

//功能: 创建一个操作句柄
//参数 [strInput] : 表示要转换的视频源文件名
//返回值 : 文件解析成功返回非0指针，解析失败返回NULL
MC_API void* MediaConvertCreate(const char* strInput) {
	if (!s_bInit) {
		strcpy(s_strError, "MediaConvert Library is not Inited");
		return NULL;
	}
	MediaConvert* obj = new MediaConvert;
	int ret = obj->ParserFile(strInput);
	if (ret) {
		return (void*)obj;
	}
	delete obj;
	return NULL;
}


//功能: 获取缩略图，一般是JPG图像
//参数 [handle] : 操作句柄，MediaConvertCreate的返回值
//参数 [strThumb] : 缩略图文件名
//返回值 : 成功返回1，失败返回0
MC_API int MediaConvertGetThumb(void* handle, const char* strThumb) {
	if (handle) {
		MediaConvert* obj = (MediaConvert*)handle;
		return obj->GetThumb(strThumb);
	}
	else {
		strcpy(s_strError, "MediaConvert Handle is NULL");
	}
	return -1;
}


//功能: 获取视频源文件分辨率宽度
//参数 [handle] : 操作句柄，MediaConvertCreate的返回值
//返回值 : 返回视频源文件分辨率宽度
MC_API int MediaConvertGetWidth(void* handle) {
	if (handle) {
		MediaConvert* obj = (MediaConvert*)handle;
		return obj->m_nWidth;
	}
	else {
		strcpy(s_strError, "MediaConvert Handle is NULL");
	}
	return -1;
}

//功能: 获取视频源文件分辨率高度
//参数 [handle] : 操作句柄，MediaConvertCreate的返回值
//返回值 : 返回视频源文件分辨率高度
MC_API int MediaConvertGetHeight(void* handle) {
	if (handle) {
		MediaConvert* obj = (MediaConvert*)handle;
		return obj->m_nHeight;
	}
	else {
		strcpy(s_strError, "MediaConvert Handle is NULL");
	}
	return -1;
}

//功能: 获取视频源文件帧率
//参数 [ptr] : 操作句柄，MediaConvertCreate的返回值
//返回值 : 返回视频源文件帧率
MC_API int MediaConvertGetFps(void* ptr) {
	if (ptr) {
		MediaConvert* obj = (MediaConvert*)ptr;
		return obj->m_nFps;
	}
	else {
		strcpy(s_strError, "MediaConvert Handle is NULL");
	}
	return -1;
}

//功能             : 返回指定参数的视频压缩的输出文件预计大小
//参数 [handle]    : 操作句柄，MediaConvertCreate的返回值
//参数 [width]     : 视频分辨率宽度,0表示原始宽度
//参数 [height]    : 视频分辨率高度,0表示原始高度
//参数 [fps]       : 视频分辨率高度,0表示原始帧率
//参数 [mode]      : 视频压缩模式
//返回值           : 输出文件预计大小，单位兆(MB)
//说明             : 用于计算指定分辨率对应几种模式的压缩文件的预估大小
//                 : 其中MODE_LOW    对应最大值
//                 : 其中MODE_NORMAL 对应默认值
//                 : 其中MODE_HIGH   对应最小值
//                 : 返回值可以用于MediaConvertSetTargetSize方法来指定视频压缩文件的预估大小
MC_API int MediaConvertGetLength(void* handle, int width, int height, int fps, int mode) {
	if (handle) {
		MediaConvert* obj = (MediaConvert*)handle;
		return obj->GetLength(width, height, fps, mode);
	}
	else {
		strcpy(s_strError, "MediaConvert Handle is NULL");
	}
	return 0;
}


//功能             : 设置视频压缩的输出文件预计大小
//参数 [handle]    : 操作句柄，MediaConvertCreate的返回值
//参数 [width]     : 视频分辨率宽度,0表示原始宽度
//参数 [height]    : 视频分辨率高度,0表示原始高度
//参数 [fps]       : 视频分辨率高度,0表示原始帧率
//参数 [target_size]: 预设的输出文件大小，单位兆(MB)
//参数 [strOutput] : 表示要转换的输出视频文件名
//返回值           : 成功返回1，失败返回0
//说明             : 当预估大小超出MediaConvertGetLength最大最小时可能会转换失败
MC_API int MediaConvertExcute(void* handle, int width, int height, int fps, int target_size, const char* strOutput) {
	if (handle) {
		MediaConvert* obj = (MediaConvert*)handle;
		return obj->Excute(width, height, fps, target_size, strOutput);
	}
	else {
		strcpy(s_strError, "MediaConvert Handle is NULL");
	}
	return 0;
}

//功能                : 返回指定参数的视频压缩的输出文件预计大小
//参数 [ptr]          : 操作句柄，MediaConvertCreate的返回值
//参数 [video_bitrate]: 视频码率,单位kbps，建议不小于300，不大于8000
//参数 [audio_bitrate]: 视频码率,单位kbps，建议在64-128之间
//返回值              : 输出文件预计大小，单位兆(MB)
//说明                : 用于计算指定分辨率对应几种模式的压缩文件的预估大小
//                    : 返回值可以用于MediaConvertInitExt方法的预估大小
MC_API int MediaConvertGetLengthExt(void* ptr, int video_bitrate, int audio_bitrate) {
	if (ptr) {
		MediaConvert* obj = (MediaConvert*)ptr;
		return obj->GetLengthExt(video_bitrate, audio_bitrate);
	}
	else {
		strcpy(s_strError, "MediaConvert Handle is NULL");
	}
	return 0;
}


//功能             : 返回指定分辨率和压缩模式的视频压缩的输出文件预计大小
//参数 [handle]    : 操作句柄，MediaConvertCreate的返回值
//参数 [width]     : 视频分辨率宽度,0表示原始宽度
//参数 [height]    : 视频分辨率高度,0表示原始高度
//参数 [fps]       : 视频分辨率高度,0表示原始帧率
//参数 [video_bitrate]: 视频码率,单位kbps，建议不小于300，不大于8000
//参数 [audio_bitrate]: 视频码率,单位kbps，建议在64-128之间
//参数 [strOutput] : 表示要转换的输出视频文件名
//返回值           : 成功返回1，失败返回0
//说明             : 过大或者过小的音视频码率的设置可能会导致专门失败，请尝试使用MediaConvertInit重新转码
MC_API int MediaConvertExcuteExt(void* handle, int width, int height, int fps, int video_bitrate, int audio_bitrate, const char* strOutput) {
	if (handle) {
		MediaConvert* obj = (MediaConvert*)handle;
		return obj->ExcuteExt(width, height, fps, video_bitrate, audio_bitrate, strOutput);
	}
	else {
		strcpy(s_strError, "MediaConvert Handle is NULL");
	}
	return 0;
}


//功能: 强制中断转换进程
//参数 [handle] : 操作句柄，MediaConvertCreate的返回值
//返回值 : 成功返回1，失败返回0
//说明 : 执行后也需要不断调用MediaConvertGetState获取执行进度， 获得返回值为 - 1 或者 100 才可以调用MediaConvertDestroy销毁句柄
MC_API int MediaConvertInterrupt(void* handle) {
	if (handle) {
		MediaConvert* obj = (MediaConvert*)handle;
		obj->Interrupt();
		return 1;
	}
	else {
		strcpy(s_strError, "MediaConvert Handle is NULL");
	}
	return 0;
}

//功能: 获取转换进度
//参数 [handle] : 操作句柄，MediaConvertCreate的返回值
//返回值 : -1 表示转换失败
//100表示转换成功
//0 - 99 表示转换进度百分比
//说明 : 获得返回值为 - 1 或者 100 才可以调用MediaConvertDestroy销毁句柄
MC_API int MediaConvertGetState(void* handle) {
	if (handle) {
		MediaConvert* obj = (MediaConvert*)handle;
		return obj->GetState();
	}
	else {
		strcpy(s_strError, "MediaConvert Handle is NULL");
	}
	return -1;
}

//功能: 销毁转换句柄
//参数 [handle] : 操作句柄，MediaConvertCreate的返回值
//返回值 : 成功返回1，失败返回0
//说明 : 调用前先用MediaConvertGetState方法获得返回值为 - 1 或者 100 才能那个继续执行
MC_API int MediaConvertDestroy(void* handle) {
	if (handle) {
		MediaConvert* obj = (MediaConvert*)handle;
        obj->Interrupt();
		delete obj;
		return 1;
	}
	else {
		strcpy(s_strError, "MediaConvert Handle is NULL");
	}
	return 0;
}

#endif
