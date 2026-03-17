/*
ffmpeg 函数
*/
#include "FfmpegIncludes.h"

#include "WXBase.h"

#ifdef _WIN32
#include <timeapi.h>
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"libffmpeg.lib")
#endif
AVPacket flush_pkt;


extern "C" void log_callback(void *ptr, int level, const char *fmt, va_list vl){
    //printf("AV Log\n");
}

#pragma mark ----------------- ffmpeg init/deinit -------------------
WXDELOGO_CAPI void WXFfmpegInit() {
#ifdef _WIN32
	SetDllDirectory(_T(""));
#endif
	//avcodec_register_all();
	avfilter_register_all();
	//av_register_all();
	avformat_network_init();
	//SDL_Init(SDL_INIT_AUDIO | SDL_INIT_TIMER);
	av_init_packet(&flush_pkt);
    av_log_set_callback(log_callback);
}

WXDELOGO_CAPI void WXFfmpegDeinit() {
	//WX_SilenceAudioClose();
	//SDL_Quit();
}

WXDELOGO_CAPI void    WXFfmpegSetLogFile(WXCTSTR szFileName) {
	//WXSetLogFile(szFileName);
}


WXDELOGO_CAPI WXTSTR WXStrcpy(WXTSTR str1, WXCTSTR str2) {
	return WXUtils::Strcpy(str1, str2);
}

WXDELOGO_CAPI int WXStrcmp(WXCTSTR str1, WXCTSTR str2) {
	return WXUtils::Strcmp(str1, str2);
}

WXDELOGO_CAPI int WXStrlen(WXCTSTR str) {
	return WXUtils::Strlen(str);
}

WXDELOGO_CAPI WXCTSTR  WXStrdup(WXCTSTR str) {
	return WXUtils::Strdup(str);
}

static WXString s_wxstr = _T("OK");
WXDELOGO_CAPI WXCTSTR WXFfmpegGetError(int code) {
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

