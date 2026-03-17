/*
ffmpeg 函数
*/
#include "FfmpegIncludes.h"

#include <WXBase.h>

#ifndef _WIN32
#include <sys/select.h>
#include <sys/time.h>
#endif

AVPacket flush_pkt;

//获取多媒体文件音频抽样值
WXFFMPEG_CAPI int  WXGetAudioVolumeData(WXCTSTR filename, int *pData, int Num, int bFullWave) {
	if (Num <= 0)return 0;
	memset(pData, 0, Num * sizeof(int));
	
	int error = 0;
	int iAudio = 0;
	int64_t time = 0;//ts

	void *MediaInfo = WXMediaInfoCreate(filename, &error);
	if (MediaInfo) {
		iAudio = WXMediaInfoGetAudioChannelNumber(MediaInfo);
		time = WXMediaInfoGetFileDuration(MediaInfo);
		if (iAudio == 0 || time == 0) {
			WXMediaInfoDestroy(MediaInfo);
			WXLogWriteNew("没有音频流或者文件长度为0");
			return 0;
		}
		WXMediaInfoDestroy(MediaInfo);
		//有音频通道
		//解码数据转为Momo, 然后将数据分割为Num个部分，找出每一个部分的第一个
		
		WXString str = filename;
		AVFormatContext *ic = avformat_alloc_context();
		int err = avformat_open_input(&ic, str.c_str(), nullptr, nullptr);//打开文件
		avformat_find_stream_info(ic , NULL);
		int audio_index = -1;
		for (int  i = 0; i < ic->nb_streams; i++){
			if (ic->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
				audio_index = i;
				WXLogWriteNew("audio_index = %d , Time = %lldms",audio_index, time);
				break;
			}
		}

		AVCodecContext *avctx = avcodec_alloc_context3(nullptr);
		avcodec_parameters_to_context(avctx, ic->streams[audio_index]->codecpar);
		AVCodec *codec = avcodec_find_decoder(avctx->codec_id);//解码器

		if (avcodec_open2(avctx, codec, nullptr) >= 0) {
			WXLogWriteNew("音频解码器 =[%s,%s]",codec->name,codec->long_name);
			
			SwrContext *pSwr = nullptr;
			//开始解码
			AVPacket pkt;
			AVFrame *frame = av_frame_alloc();
			AVFrame *dstFrame = nullptr;
			

			int64_t Index = 0;//序号
			int64_t TotalLength = 0;//音频总长度
			int64_t Step = 0;//间隔

			while (1){

				av_init_packet(&pkt);
				int ret = av_read_frame(ic, &pkt);
				if (ret == AVERROR_EOF)break;

				if (pkt.stream_index != audio_index) {
					int xx = 0;
					xx++;
				}
				if (pkt.stream_index == audio_index) { //音频包
					int got_picture = 0;
					avcodec_decode_audio4(avctx, frame, &got_picture, &pkt);

					if (got_picture) {
						if (pSwr == nullptr) {
							pSwr = swr_alloc_set_opts(nullptr,
								frame->channel_layout, (enum AVSampleFormat)frame->format, frame->sample_rate,
								AV_CH_LAYOUT_MONO, (enum AVSampleFormat)AV_SAMPLE_FMT_U8, frame->sample_rate,
								0, nullptr);

							dstFrame = av_frame_alloc();
							dstFrame->format = AV_SAMPLE_FMT_U8;
							dstFrame->channel_layout = AV_CH_LAYOUT_MONO;
							dstFrame->channels = 1;
							dstFrame->sample_rate = frame->sample_rate;
							dstFrame->nb_samples = frame->nb_samples;
							av_frame_get_buffer(dstFrame, 0);

							int64_t xtime = time * frame->sample_rate / 1000;//数据总长度
							Step = xtime / Num;//分段数据
						}
						swr_convert(pSwr, dstFrame->data, dstFrame->nb_samples, (const uint8_t**) frame->data, frame->nb_samples);


						uint8_t *buf = dstFrame->data[0];
						int buf_size = dstFrame->nb_samples;
						if (Index == 0) {
							pData[Index++] = bFullWave ? buf[0] : abs(buf[0] - 128);//第一个数据
						}

						int64_t newLength = TotalLength +  buf_size;	

						if (newLength >= Index * Step) {
							int Num = newLength / Step + 1;
							for (int i = Index; i < Num; i++) {
								int64_t pos = i * Step - TotalLength;
								pData[i] = bFullWave ? buf[pos] : abs(buf[pos] - 128);
								Index++;
							}
						}
						TotalLength = newLength;		
					}
				}
				av_packet_unref(&pkt);
			}

			WXLogWriteNew("文件读取结束");
			avcodec_close(avctx);

			av_frame_free(&frame);
			if(dstFrame)
			av_frame_free(&dstFrame);
			swr_free(&pSwr);

			avcodec_free_context(&avctx);
			avformat_close_input(&ic);
			avformat_free_context(ic);
			return 1;
		}

		avcodec_free_context(&avctx);
		avformat_close_input(&ic);
		avformat_free_context(ic);
	}

	WXLogWriteNew("打开文件失败");
	return 0;
}

#ifdef _WIN32
#define FFMAX(a,b) ((a) > (b) ? (a) : (b))
#define SDL_AUDIO_MIN_BUFFER_SIZE 512
#define SDL_AUDIO_MAX_CALLBACKS_PER_SEC 30
static int g_channel = 0;

static void sdl_audio_callback(void *opaque, Uint8 *stream, int len) {
	memset(stream, 0, len);
}

static void WX_SilenceAudioOpen() {
	SDL_AudioSpec wanted_spec, spec;
	wanted_spec.channels = 2;
	wanted_spec.freq = 44100;
	wanted_spec.format = AUDIO_S16SYS;
	wanted_spec.silence = 0;
	wanted_spec.samples = FFMAX(SDL_AUDIO_MIN_BUFFER_SIZE, 2 << av_log2(wanted_spec.freq / SDL_AUDIO_MAX_CALLBACKS_PER_SEC));
	wanted_spec.callback = sdl_audio_callback;
	wanted_spec.userdata = nullptr;
	g_channel = SDL_OpenAudioDevice(nullptr, 0, &wanted_spec, &spec, SDL_AUDIO_ALLOW_ANY_CHANGE);
	if (g_channel >= 2) {
		SDL_PauseAudioDevice(g_channel, 0);
	}
}
static void WX_SilenceAudioClose() {
	if (g_channel >= 0) {
		SDL_CloseAudioDevice(g_channel);
		g_channel = 0;
	}
}
#else
static void WX_SilenceAudioOpen() {}
static void WX_SilenceAudioClose() {};
#endif

#pragma mark ----------------- ffmpeg init/deinit -------------------

static void log_callback_null(void *ptr, int level, const char *fmt, va_list vl)
{
}

WXFFMPEG_CAPI void WXFfmpegInit() {
#ifdef _WIN32
	SetDllDirectory(_T(""));
#endif
	avcodec_register_all();
	//avdevice_register_all();
	avfilter_register_all();
	av_register_all();
	avformat_network_init();
    
    av_log_set_callback(log_callback_null);
    
	SDL_Init(SDL_INIT_AUDIO | SDL_INIT_TIMER);
	av_init_packet(&flush_pkt);
	WX_SilenceAudioOpen();
}

WXFFMPEG_CAPI void WXFfmpegDeinit() {
	WX_SilenceAudioClose();
	SDL_Quit();
}

WXFFMPEG_CAPI void    WXFfmpegSetLogFile(WXCTSTR szFileName) {
	WXSetLogFile(szFileName);
}

//设置日志文件
WXFFMPEG_CAPI void WXSetLogFile(WXCTSTR wszFileName) {
	WXUtils::SetLogFile(wszFileName);
}

WXFFMPEG_CAPI int64_t  WXGetTimeMs() {
#ifdef _WIN32
	return ::timeGetTime();
#else
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000 + tv.tv_usec / 1000;
#endif
}

WXFFMPEG_CAPI void     WXSleepMs(int ms) {
	if (ms <= 0)return;
#ifdef _WIN32
	::timeBeginPeriod(1);
	::Sleep(ms);
	::timeEndPeriod(1);
#else
	struct timeval delay;
	delay.tv_sec = 0;
	delay.tv_usec = ms * 1000;
	select(0, NULL, NULL, NULL, &delay);
#endif
}



WXFFMPEG_CAPI WXTSTR WXStrcpy(WXTSTR str1, WXCTSTR str2) {
	return WXUtils::Strcpy(str1, str2);
}

WXFFMPEG_CAPI int WXStrcmp(WXCTSTR str1, WXCTSTR str2) {
	return WXUtils::Strcmp(str1, str2);
}

WXFFMPEG_CAPI int WXStrlen(WXCTSTR str) {
	return WXUtils::Strlen(str);
}

WXFFMPEG_CAPI WXCTSTR  WXStrdup(WXCTSTR str) {
	return WXUtils::Strdup(str);
}

static WXString s_wxstr = _T("OK");
WXFFMPEG_CAPI WXCTSTR WXFfmpegGetError(int code) {
	switch (code) {
	case FFMPEG_ERROR_OK:
		s_wxstr = _T("No error ");
		break;
	case FFMPEG_ERROR_NOFILE:
		s_wxstr = _T("file no exist ");
		break;
	case FFMPEG_ERROR_EMPTYFILE:
		s_wxstr = _T("file is empty ");
		break;
	case FFMPEG_ERROR_INIT:
		s_wxstr = _T("error on init func ");
		break;
	case FFMPEG_ERROR_READFILE:
		s_wxstr = _T("error on read file ");
		break;
	case FFMPEG_ERROR_PARSE:
		s_wxstr = _T("error on parse file ");
		break;
	case FFMPEG_ERROR_BREADK:
		s_wxstr = _T("error on user break ");
		break;
	case FFMPEG_ERROR_NO_MEIDADATA:
		s_wxstr = _T("error of no MediaData");
		break;
	case FFMPEG_ERROR_PROCESS:
		s_wxstr = _T("Processing ");
		break;
	case FFMPEG_ERROR_NO_OUTPUT_FILE:
		s_wxstr = _T("No output file ");
		break;
	case FFMPEG_ERROR_TRANSCODE:
		s_wxstr = _T("error on function transcode ");
		break;
	case FFMPEG_ERROR_DECODE_ERROR_STAT:
		s_wxstr = _T("error on function decode ");
		break;
	case FFMPEG_ERROR_ASSERT_AVOPTIONS:
		s_wxstr = _T("error on function assert avopyopns");
		break;
	case FFMPEG_ERROR_ABORT_CODEC_EXPERIMENTAL:
		s_wxstr = _T("error on function abort codec");
		break;
	case FFMPEG_ERROR_DO_AUDIO_OUT:
		s_wxstr = _T("error on do audio out");
		break;
	case FFMPEG_ERROR_DO_SUBTITLE_OUT:
		s_wxstr = _T("error on function subtitle codec ");
		break;
	case FFMPEG_ERROR_DO_VIDEO_OUT:
		s_wxstr = _T("error on function do video out ");
		break;
	case FFMPEG_ERROR_DO_VIDEO_STAT:
		s_wxstr = _T("error on function do video stat ");
		break;
	case FFMPEG_ERROR_READ_FILTERS:
		s_wxstr = _T("error on function read filters ");
		break;
	case FFMPEG_ERROR_FLUSH_ENCODERS:
		s_wxstr = _T("error on function flush encoders ");
		break;
	case FFMPEG_ERROR_LIBAVUTIL:
		s_wxstr = _T("error on Libavutil ");
		break;
	case FFMPEG_ERROR_ON_FILTERS:
		s_wxstr = _T("error on filters ");
		break;
	case FFMPEG_ERROR_ON_OPTS:
		s_wxstr = _T("error on opts ");
		break;
	case FFMPEG_ERROR_EXIT_ON_ERROR:
		s_wxstr = _T("exit");
		break;

	case 	FFPLAY_ERROR_OK_START:
		s_wxstr = _T("ffplay Start");
		break;
	case 	FFPLAY_ERROR_OK_STOP:
		s_wxstr = _T("ffplay Stop");
		break;
	case 	FFPLAY_ERROR_OK_GET_PICTURE:
		s_wxstr = _T("ffplay Get a Picture OK");
		break;
	case 	FFPLAY_ERROR_OK_GET_EOF:
		s_wxstr = _T("ffplay Read File EOF");
		break;
	case 	FFPLAY_ERROR_OK_VIDEO_STOP:
		s_wxstr = _T("ffplay Play Video End");
		break;
	case 	FFPLAY_ERROR_OK_AUDIO_STOP:
		s_wxstr = _T("ffplay Play Audio End");
		break;

	default:
		s_wxstr = _T("OK");
		break;
	}
	return s_wxstr.str();
}

