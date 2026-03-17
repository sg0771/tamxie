/*
ffplay Meida Player
*/

#include "FfmpegIncludes.h"
#include "WXBase.h"

#include <strings.h>
#include <SDL.h>
#include <SDL_thread.h>
#include "libyuv.h"

#include "WXVideoRender.h"

//音频录音或者播放回调函数
typedef void(*OnAudioData)(void *ctx, uint8_t * buf, int buf_size);
//视频数据回调函数
typedef void(*WXFfplayOnAVFrame)(void* ctx, struct AVFrame* frame);//回调AVFrame

#define SDL_AUDIO_MIN_BUFFER_SIZE 512
#define SDL_AUDIO_MAX_CALLBACKS_PER_SEC 30
#define WXMEDIA_API
#define WXLogWriteNew printf

#ifndef SAFE_AV_FREE
#define SAFE_AV_FREE(p)  if(p){av_frame_free(&p);p=nullptr;}
#endif

#ifndef SAFE_SWR_FREE
#define SAFE_SWR_FREE(p)  if(p){swr_free(&p);p=nullptr;}
#endif

#ifndef SAFE_SWS_FREE
#define SAFE_SWS_FREE(p)  if(p){sws_freeContext(p);p=nullptr;}
#endif


#define INSERT_FILT(name, arg) do {                                          \
    AVFilterContext *filt_ctx;                                               \
    ret = avfilter_graph_create_filter(&filt_ctx, avfilter_get_by_name(name), "m_" name, arg, NULL, graph);    \
    if (ret < 0)goto fail;                                                   \
    ret = avfilter_link(filt_ctx, 0, last_filter, 0);                        \
    if (ret < 0) goto fail;                                                  \
    last_filter = filt_ctx;                                                  \
} while (0)

#define FRAME_DURATION_THRESHOLD_MAX 0.1   //100ms,最大帧时长，add by vicky
#define PLAY_FINISH_THRESHOLD 0.5   //500ms,播放完成延迟时间，add by vicky

static unsigned sws_flags = SWS_BICUBIC;

static  int flush_pkt_init = FALSE;
static  AVPacket flush_pkt;

#undef  AV_TIME_BASE_Q
static AVRational AV_TIME_BASE_Q{ 1, AV_TIME_BASE };

//视频转换类
class WXVideoConvert {
public:
    SwsContext *m_pSwsCtx = nullptr;
    int m_nWidth = 0;
    int m_nHeight = 0;
    AVPixelFormat m_srcFmt = AV_PIX_FMT_NONE;
    AVPixelFormat m_dstFmt = AV_PIX_FMT_NONE;

    WXVideoConvert() {}
    virtual ~WXVideoConvert() {
        if (m_pSwsCtx) {
            sws_freeContext(m_pSwsCtx);
            m_pSwsCtx = nullptr;
        }
    }

    void Init(int width, int height, int srcFmt, int dstFmt) {
        if (m_pSwsCtx) {
            sws_freeContext(m_pSwsCtx);
            m_pSwsCtx = nullptr;
        }
        m_nWidth = width;
        m_nHeight = height;
        m_srcFmt = (AVPixelFormat)srcFmt;
        m_dstFmt = (AVPixelFormat)dstFmt;
        m_pSwsCtx = sws_getContext(width, height, (AVPixelFormat)srcFmt,
            width, height, (AVPixelFormat)dstFmt,
            SWS_FAST_BILINEAR, nullptr, nullptr, nullptr);
    }
    void Convert(const AVFrame *srcFrame, AVFrame *dstFrame) {
        if (m_nWidth != srcFrame->width || m_nHeight != srcFrame->height) {
            if (m_pSwsCtx) {
                sws_freeContext(m_pSwsCtx);
                m_pSwsCtx = nullptr;
            }
        }
        if (m_pSwsCtx == nullptr) {
            Init(dstFrame->width, dstFrame->height, srcFrame->format, dstFrame->format);
        }
        if (m_pSwsCtx) {
            av_frame_make_writable(dstFrame);
            sws_scale(m_pSwsCtx, srcFrame->data, srcFrame->linesize,
                0, m_nHeight,
                dstFrame->data, dstFrame->linesize);
        }
    }
};

WXMEDIA_API  AVFrame *WXMediaUtilsAllocVideoFrame(enum AVPixelFormat pix_fmt, int width, int height) {
    AVFrame *picture = av_frame_alloc();
    picture->format = pix_fmt;
    picture->width = width;
    picture->height = height;
    av_frame_get_buffer(picture, 1);
    picture->pts = 0;
    if (pix_fmt == AV_PIX_FMT_YUV420P || pix_fmt == AV_PIX_FMT_YUV420P) {
        memset(picture->data[0], 0, picture->linesize[0] * height);
        memset(picture->data[1], 128, picture->linesize[1] * height / 2);
        memset(picture->data[2], 128, picture->linesize[2] * height / 2);
    }
    if (pix_fmt == AV_PIX_FMT_RGB32) {
        memset(picture->data[0], 0, picture->linesize[0] * height);
    }
    return picture;
}

//视频帧
class WXVideoFrame {
public:
    int m_bUsed = FALSE;//队列标记位
#ifdef _WIN32
    BITMAPINFOHEADER m_bih;
#endif
    AVFrame *m_pFrame = nullptr;
    int m_iWidth = 0;
    int m_iHeight = 0;
    int m_pixfmt = AV_PIX_FMT_NONE;
    WXVideoFrame() {}

    WXVideoFrame(AVFrame*frame) {
        if (frame == NULL)return;
        Copy(frame);
    }
    virtual ~WXVideoFrame() {
        SAFE_AV_FREE(m_pFrame);
    }

    //------------------------------------------
    void Init(int format, int width, int height) {
        SAFE_AV_FREE(m_pFrame);
        m_iWidth = width;
        m_iHeight = height;
        m_pixfmt = format;
        m_pFrame = WXMediaUtilsAllocVideoFrame((AVPixelFormat)m_pixfmt, m_iWidth, m_iHeight);
#ifdef _WIN32
        if (format == AV_PIX_FMT_RGB32) {
            memset(&m_bih, 0, sizeof(BITMAPINFOHEADER));
            m_bih.biSize = sizeof(BITMAPINFOHEADER);
            m_bih.biBitCount = 32;
            m_bih.biWidth = width;
            m_bih.biHeight = -height;
            m_bih.biPlanes = 1;
            m_bih.biCompression = BI_RGB;
            m_bih.biSizeImage = width * height * 4;
        }
        else if (format == AV_PIX_FMT_BGR24) {
            memset(&m_bih, 0, sizeof(BITMAPINFOHEADER));
            m_bih.biSize = sizeof(BITMAPINFOHEADER);
            m_bih.biBitCount = 24;
            m_bih.biWidth = width;
            m_bih.biHeight = -height;
            m_bih.biPlanes = 1;
            m_bih.biCompression = BI_RGB;
            m_bih.biSizeImage = width * height * 3;
        }
#endif
    }

    void Copy(AVFrame*frame) {
        if (frame == NULL)return;
        SAFE_AV_FREE(m_pFrame);
        if (m_iWidth != frame->width || m_iHeight != frame->height || m_pixfmt != frame->format) {
            SAFE_AV_FREE(m_pFrame);
        }
        m_iWidth = frame->width;
        m_iHeight = frame->height;
        m_pixfmt = frame->format;
        if (m_pFrame == nullptr)
            m_pFrame = WXMediaUtilsAllocVideoFrame((AVPixelFormat)m_pixfmt, m_iWidth, m_iHeight);
        av_frame_copy(m_pFrame, frame);
    }
};

//优化声音转换
WXMEDIA_API  SwrContext* WXMediaUtilsAllocSwrCtx(int inSampleRate, int inCh, enum AVSampleFormat inSampleFmt,
    int outSampleRate, int outCh, enum AVSampleFormat outSampleFmt) {
    int64_t out_layout = av_get_default_channel_layout(outCh);
    int64_t in_layout = av_get_default_channel_layout(inCh);
    SwrContext *swr_ctx = swr_alloc_set_opts(NULL,
        out_layout, outSampleFmt, outSampleRate,
        in_layout, inSampleFmt, inSampleRate,
        0, NULL);
    swr_init(swr_ctx);
    return swr_ctx;
}

class AudioResampler{
public:
    WXFifo m_tempFifo;//输入缓存队列

    int m_nOutMax = 0;
    WXFifo m_outputFifo;//输出缓存 输出为S16数据

    int m_bFloat32 = TRUE;
    int m_iInSampleRate = 48000;//输入采样频率
    int m_iInChannel = 2;//输出声道
    int m_iInSampleCount = 480;//10ms数据量
    int m_iInSize = 1920;

    int m_iOutSampleRate = 44100;//输入采样频率
    int m_iOutChannel = 2;//输出声道
    int m_iOutSampleCount = 441;//10ms数据量
    int m_iOutSize = 1764;//

    WXDataBuffer m_srcFrame;
    WXDataBuffer m_dstFrame;
    SwrContext *m_pSwrCtx = nullptr;
public:
    AudioResampler() {}
    void Init(int bFloat32, int inSampleRate, int inChannel, int outSampleRate, int outChannel){
        m_bFloat32 = bFloat32;
        m_iInSampleRate = inSampleRate;
        m_iInChannel = inChannel;
        m_iInSampleCount = m_iInSampleRate / 100; //10ms
        m_iInSize = m_iInSampleCount * m_iInChannel * 2;
        if (bFloat32)
            m_iInSize *= 2;
        m_srcFrame.Init(nullptr, m_iInSize);//输出缓存

        m_iOutSampleRate = outSampleRate;
        m_iOutChannel = outChannel;
        m_iOutSampleCount = m_iOutSampleRate / 100;
        m_iOutSize = m_iOutSampleCount * m_iOutChannel * 2;

        m_nOutMax = m_iOutChannel * m_iOutSampleRate * 2 / 2;//500ms 数据量

        m_dstFrame.Init(nullptr, m_iOutSize);//输出缓存
        
        if (m_iInSampleRate != m_iOutSampleRate || m_iInChannel != m_iOutChannel || m_bFloat32) {
            m_pSwrCtx = WXMediaUtilsAllocSwrCtx(m_iInSampleRate, m_iInChannel,m_bFloat32 ? AV_SAMPLE_FMT_FLT : AV_SAMPLE_FMT_S16,
                m_iOutSampleRate, m_iOutChannel, AV_SAMPLE_FMT_S16);
        }
    }
    virtual ~AudioResampler(){
        SAFE_SWR_FREE(m_pSwrCtx);
    }
public:
    void WriteData(uint8_t *buf, int size){
        
        if (m_pSwrCtx) {
            if (size == m_iInSize) {//录音操作，每次进来都是10ms的数据量
                uint8_t *pOutData[4] = { m_dstFrame.GetBuffer() };
                uint8_t *pInData[4] = { buf };
                swr_convert(m_pSwrCtx, pOutData, m_iOutSampleCount,
                    (const uint8_t **)pInData, m_iInSampleCount);
                if(m_outputFifo.Size() < m_nOutMax)
                    m_outputFifo.Write(m_dstFrame.GetBuffer(), m_iOutSize);
            }else {
                m_tempFifo.Write(buf, size); //声音播放
                while (m_tempFifo.Size() >= m_iInSize) { //每次取出10ms的数据量，转换为10ms 的 S16 数据
                    m_tempFifo.Read(m_srcFrame.GetBuffer(), m_iInSize);

                    uint8_t *pOutData[4] = { m_dstFrame.GetBuffer() };
                    uint8_t *pInData[4] = { m_srcFrame.GetBuffer() };
                    swr_convert(m_pSwrCtx, pOutData, m_iOutSampleCount,
                        (const uint8_t **)pInData, m_iInSampleCount);
                    if (m_outputFifo.Size() < m_nOutMax)
                        m_outputFifo.Write(m_dstFrame.GetBuffer(), m_iOutSize);
                }
            }
        }else {
            if (m_outputFifo.Size() < m_nOutMax)
                m_outputFifo.Write(buf, size); //不转换
        }
    }
};



#ifdef __APPLE__

#include <WXBase.h>
#include <OpenAL/al.h>
#include <OpenAL/alc.h>

class ALRender: public WXThread  {
public:
    enum {
        NUM_SOURCES = 1,
        NUM_BUFFERS = 6
    };
    
    ALCcontext *mContext = nullptr;
    ALCdevice *mDevice = nullptr;
    
    ALuint    mSource = 0;
    ALuint    mBuffers[NUM_BUFFERS] = { 0 };
    
    int m_nSampleRate = 44100;
    int m_nFormat = AL_FORMAT_STEREO16;
    int m_nSample = 441;
    int m_nChannel = 2;
    WXDataBuffer    m_buf;
    AudioResampler m_resample;

    WXDataBuffer    m_buf1;
    void *m_pSink = nullptr;
    OnAudioData m_cbFunc = nullptr;
    int m_bPause = 0;
public:
    virtual  void ThreadPost(){
        //Stop
        if (mSource) {
            alSourceStop(mSource);
            alDeleteSources(1, &mSource);
            mSource = 0;
            alDeleteBuffers(NUM_BUFFERS, mBuffers);
        }
        if (mContext) {
            alcDestroyContext(mContext);
            mContext = nullptr;
        }
        if (mDevice) {
            alcCloseDevice(mDevice);
            mDevice = nullptr;
        }
    }
    virtual  void ThreadProcess(){
        ALint processed = 0;
        alGetSourcei(mSource, AL_BUFFERS_PROCESSED, &processed);
        while (processed){
            ALuint bids = 0;
            alSourceUnqueueBuffers(mSource, 1, &bids);
            if (bids != 0){
                
                if(m_cbFunc){
                    m_cbFunc(m_pSink, m_buf1.GetBuffer(), m_buf1.m_iBufSize);//对外部请求10ms数据量
                    m_resample.WriteData(m_buf1.GetBuffer(), m_buf1.m_iBufSize);//写入缓冲区
                }
                memset(m_buf.GetBuffer(), 0, m_buf.m_iBufSize);
                m_resample.m_outputFifo.Read2(m_buf.GetBuffer(), m_buf.m_iBufSize);//音频设备请求10ms数据量
                if(m_bPause){
                    //静音播放
                    memset(m_buf.GetBuffer(),0, m_buf.m_iBufSize);
                }
                
                //AudioDataPush(m_buf.GetBuffer(), m_buf.m_iBufSize);
                
                alBufferData(bids, m_nFormat, m_buf.GetBuffer(), m_buf.m_iBufSize, m_nSampleRate);
                alSourceQueueBuffers(mSource, 1, &bids);
                processed--;
                if(processed==0){
                    ALenum state;
                    alGetSourcei(mSource, AL_SOURCE_STATE, &state );
                    if(state != AL_PLAYING){
                        alSourcePlay(mSource);
                    }
                    break;
                }
            }
        }
        SLEEPMS(5);
    }
    bool Start(int inSampleRate, int inChannel, void *pSink = nullptr, OnAudioData cbFunc = nullptr){
        mDevice = alcOpenDevice(NULL);
        if (nullptr == mDevice) {
            return false;
        }

        mContext = alcCreateContext(mDevice, NULL);
        alcMakeContextCurrent(mContext);

        alGenBuffers(NUM_BUFFERS, mBuffers);
        alGenSources(NUM_SOURCES, &mSource );
        alSourcei(mSource, AL_SOURCE_RELATIVE, AL_TRUE );
        alSourcei(mSource, AL_ROLLOFF_FACTOR, 0 );
        alSourceRewind(mSource);
        alSourcei( mSource, AL_BUFFER, 0 );

        int iBufsize = 1764;
        m_buf.Init(nullptr, iBufsize);
        m_resample.Init(FALSE, inSampleRate, inChannel, 44100, 2);//S16 转换
      
        for (int idx = 0; idx < NUM_BUFFERS; idx++) {
            alBufferData(mBuffers[idx], m_nFormat, m_buf.GetBuffer(), m_buf.m_iBufSize, m_nSampleRate);
            alSourceQueueBuffers(mSource, 1, &mBuffers[idx]);
        }
        alSourcePlay(mSource);

        if (cbFunc) {
            m_pSink = pSink;
            m_cbFunc = cbFunc;
            int iBufSize = inSampleRate * inChannel * 2 / 100;//10ms 数据量
            m_buf1.Init(nullptr, iBufSize);
        }

        ThreadStart();
        // SetThreadName("ALRender");
        return true;
    }

    void Stop(){
        ThreadStop();
    }
    void WriteData(uint8_t *buf, int buf_size) {
        if(!m_cbFunc){
            m_resample.WriteData(buf, buf_size);
        }
    }
    void Pause(int b){
        m_bPause = b;
    }
    virtual ~ALRender(){
        Stop();
    }
};

//------------------------------------------------------------------------------
//----------------------------------------------------------------------------
//实现类似于SDL_AudioCallBack 的播放
WXMEDIA_API void *WXSoundPlayerCreate2(int inSampleRate, int inChannel, void *pSink, OnAudioData cbFunc) {
    ALRender *player = new ALRender;
    bool bRet = player->Start(inSampleRate, inChannel, pSink, cbFunc);
    if (!bRet) {
        delete player;
        return nullptr;
    }
    return (void*)player;
}

//用于实时流数据的播放，和WXSoundPlayerWriteData配套使用
WXMEDIA_API void *WXSoundPlayerCreate(int inSampleRate, int inChannel) {
    ALRender *player = new ALRender;
    bool bRet = player->Start(inSampleRate, inChannel);
    if (!bRet) {
        delete player;
        return nullptr;
    }
    return (void*)player;
}

WXMEDIA_API void  WXSoundPlayerPause(void* ptr, int b) {
    if (ptr) {
        ALRender *player = (ALRender*)ptr;
        player->Pause(b);
    }
}

WXMEDIA_API void  WXSoundPlayerDestroy(void* ptr) {
    if (ptr) {
        ALRender *player = (ALRender*)ptr;
        delete player;
    }

}

WXMEDIA_API void  WXSoundPlayerWriteData(void* ptr, uint8_t *buf, int buf_size) {
    if (ptr) {
        ALRender *player = (ALRender*)ptr;
        player->WriteData(buf, buf_size);
    }
}

#endif

//网络流的分辨率
int g_nStreamWidth = 0;
int g_nStreamHeight = 0;
static int g_bRecord = 0;

WXMEDIA_API void WXSetStreamRecord(int b) {
	g_bRecord = b;
}
WXMEDIA_API int  WXGetStreamRecord() {
	return g_bRecord;
}

class WXFfplay {

	WXLocker m_mutex;

	WXString  m_strSubtitleFontName = L"";
	int       m_iSubtitleFontSize = 20;
	int       m_iSubtitleFontColor = 0xFFFFFF;
	int       m_iSubtitleFontAlpha = 0;
	int       m_iSubtitlePostion = 0;
	int       m_iAlignment = 2;

	void HandleSubtitle() {
		if (m_strSubtitle.length() != 0) {
			WXString wxstr;
			wxstr.Format("subtitles=%s", m_strSubtitle.c_str());
			uint32_t color = (m_iSubtitleFontAlpha << 24) | m_iSubtitleFontColor;
			WXString wxstrForce_Style;
			if (m_strSubtitleFontName.length() > 0) {
				wxstrForce_Style.Format(":force_style=\'FontName=%s,FontSize=%d,PrimaryColour=&H%08x&,MarginV=%d,Alignment=%d\'",
					m_strSubtitleFontName.c_str(), m_iSubtitleFontSize, color, m_iSubtitlePostion, m_iAlignment);
			}
			else {
				wxstrForce_Style.Format(":force_style=\'FontSize=%d,PrimaryColour=&H%08x&,MarginV=%d,Alignment=%d\'",
					m_iSubtitleFontSize, color, m_iSubtitlePostion, m_iAlignment);
			}
			wxstr += wxstrForce_Style;
			m_strVF.Cat(wxstr, L", ");
		}
	}
public:
	virtual void   SetSubtitleFont(WXCTSTR  wszFontName, int FontSize, int FontColor) {
		WXAutoLock al(m_mutex);
		if (m_strSubtitle.length() == 0)return;
		m_strSubtitleFontName = L"";
#ifdef _WIN32
		if (wszFontName != nullptr && WXStrlen(wszFontName) > 0)
			m_strSubtitleFontName = wszFontName;
#endif
		m_iSubtitleFontSize = FontSize;
		m_iSubtitleFontColor = FontColor;
		SendChangeFilter();
	}

	virtual void   SetSubtitleAlpha(int alpha) {
		WXAutoLock al(m_mutex);
		if (m_strSubtitle.length() == 0)return;
		m_iSubtitleFontAlpha = av_clip_c(alpha, 0, 255);
		SendChangeFilter();
	}

	virtual void   SetSubtitlePostion(int postion) {
		WXAutoLock al(m_mutex);
		if (m_strSubtitle.length() == 0)return;
		m_iSubtitlePostion = postion;
		SendChangeFilter();
	}

	virtual void SetSubtitleAlignment(int Alignment) {
		int Align = av_clip(Alignment, 0, 2);
		if (Align == 0)
			m_iAlignment = 2;
		else if (Align == 1)
			m_iAlignment = 10;
		else
			m_iAlignment = 6;
		SendChangeFilter();
	}
public:
	WXCTSTR GetType() { return _T("FFPLAY"); }

	static int lockmgr(void **mtx, enum AVLockOp op) {
		switch (op) {
		case AV_LOCK_CREATE:
			*mtx = SDL_CreateMutex();
			if (!*mtx) {
				WXLogWriteNew(" AV_LOG_FATAL, SDL_CreateMutex(): %s\n", SDL_GetError());
				return 1;
			}
			return 0;
		case AV_LOCK_OBTAIN:
			return !!SDL_LockMutex((SDL_mutex*)*mtx);
		case AV_LOCK_RELEASE:
			return !!SDL_UnlockMutex((SDL_mutex*)*mtx);
		case AV_LOCK_DESTROY:
			SDL_DestroyMutex((SDL_mutex*)*mtx);
			return 0;
		}
		return 1;
	}
	static AVDictionary *filter_codec_opts(AVDictionary *opts, enum AVCodecID codec_id, AVFormatContext *s, AVStream *st, AVCodec *codec) {
		AVDictionary    *ret = nullptr;
		AVDictionaryEntry *t = nullptr;
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
				switch (avformat_match_stream_specifier(s, st, p + 1)) {
				case  1: *p = 0; break;
				case  0:         continue;
				default:         return nullptr;
				}

			if (av_opt_find(&cc, t->key, nullptr, flags, AV_OPT_SEARCH_FAKE_OBJ) ||
				!codec ||
				(codec->priv_class &&
					av_opt_find(&codec->priv_class, t->key, nullptr, flags,
						AV_OPT_SEARCH_FAKE_OBJ)))
				av_dict_set(&ret, t->key, t->value, 0);
			else if (t->key[0] == prefix &&
				av_opt_find(&cc, t->key + 1, nullptr, flags,
					AV_OPT_SEARCH_FAKE_OBJ))
				av_dict_set(&ret, t->key + 1, t->value, 0);

			if (p)
				*p = ':';
		}
		return ret;
	}
	static AVDictionary **setup_find_stream_info_opts(AVFormatContext *s, AVDictionary *codec_opts) {
		if (!s->nb_streams)return nullptr;
		AVDictionary **opts = (AVDictionary **)av_mallocz_array(s->nb_streams, sizeof(*opts));
		if (!opts) {
			WXLogWriteNew(" AV_LOG_ERROR,Could not alloc memory for stream options.\n");
			return nullptr;
		}
		for (int i = 0; i < s->nb_streams; i++)
			opts[i] = filter_codec_opts(codec_opts, s->streams[i]->codecpar->codec_id, s, s->streams[i], nullptr);
		return opts;
	}
	static double get_rotation(AVStream *st) {
		uint8_t* displaymatrix = av_stream_get_side_data(st, AV_PKT_DATA_DISPLAYMATRIX, nullptr);
		double theta = 0;
		if (displaymatrix)
			theta = -av_display_rotation_get((int32_t*)displaymatrix);

		theta -= 360 * floor(theta / 360 + 0.9 / 360);
		return theta;
	}
	static int cmp_audio_fmts(enum AVSampleFormat fmt1, int64_t channel_count1, enum AVSampleFormat fmt2, int64_t channel_count2) {
		/* If channel count == 1, planar and non-planar formats are the same */
		if (channel_count1 == 1 && channel_count2 == 1)
			return av_get_packed_sample_fmt(fmt1) != av_get_packed_sample_fmt(fmt2);
		else
			return channel_count1 != channel_count2 || fmt1 != fmt2;
	}
	static int64_t get_valid_channel_layout(int64_t channel_layout, int channels) {
		if (channel_layout && av_get_channel_layout_nb_channels(channel_layout) == channels)
			return channel_layout;
		else
			return 0;
	}

	static int configure_filtergraph(AVFilterGraph *graph, const char *filtergraph, AVFilterContext *source_ctx, AVFilterContext *sink_ctx)
	{
		int ret, i;
		int nb_filters = graph->nb_filters;
		AVFilterInOut *outputs = nullptr, *inputs = nullptr;

		if (filtergraph) {
			outputs = avfilter_inout_alloc();
			inputs = avfilter_inout_alloc();
			if (!outputs || !inputs) {
				ret = AVERROR(ENOMEM);
				goto fail;
			}

			outputs->name = av_strdup("in");
			outputs->filter_ctx = source_ctx;
			outputs->pad_idx = 0;
			outputs->next = nullptr;

			inputs->name = av_strdup("out");
			inputs->filter_ctx = sink_ctx;
			inputs->pad_idx = 0;
			inputs->next = nullptr;

			if ((ret = avfilter_graph_parse_ptr(graph, filtergraph, &inputs, &outputs, nullptr)) < 0)
				goto fail;
		}
		else {
			if ((ret = avfilter_link(source_ctx, 0, sink_ctx, 0)) < 0)
				goto fail;
		}

		/* Reorder the filters to ensure that inputs of the custom filters are merged first */
		for (i = 0; i < graph->nb_filters - nb_filters; i++)
			FFSWAP(AVFilterContext*, graph->filters[i], graph->filters[i + nb_filters]);

		ret = avfilter_graph_config(graph, nullptr);
	fail:
		avfilter_inout_free(&outputs);
		avfilter_inout_free(&inputs);
		return ret;
	}

	enum {
		AV_SYNC_AUDIO_MASTER, /* default choice */
		AV_SYNC_VIDEO_MASTER,
		AV_SYNC_EXTERNAL_CLOCK, /* synchronize to an external clock */
	};

	class PacketList {
	public:
		PacketList() { av_init_packet(&pkt); }
		AVPacket pkt;
		PacketList *next = nullptr;
		int serial = 0;
	};

	class PacketQueue {
	public:
		PacketList *first_pkt = nullptr;
		PacketList *last_pkt = nullptr;
		int nb_packets = 0;
		int size = 0;
		int64_t duration = 0;
		int m_bAbortRequest = 0;
		int serial = 0;
		SDL_mutex *mutex = nullptr;
		SDL_cond *cond = nullptr;

		int stream_has_enough_packets(AVStream *st, int stream_id) {
			return stream_id < 0 ||
				m_bAbortRequest ||
				(st->disposition & AV_DISPOSITION_ATTACHED_PIC) ||
				nb_packets > MIN_FRAMES && (!duration || av_q2d(st->time_base) * duration > 1.0);
		}

		int PutPrivate(AVPacket *pkt) {
			if (m_bAbortRequest)
				return -1;
			PacketList *pkt1 = new PacketList;
			pkt1->pkt = *pkt;
			pkt1->next = nullptr;
			if (pkt == &flush_pkt)serial++;
			pkt1->serial = serial;
			if (!last_pkt)
				first_pkt = pkt1;
			else
				last_pkt->next = pkt1;
			last_pkt = pkt1;
			nb_packets++;
			size += pkt1->pkt.size + sizeof(*pkt1);
			duration += pkt1->pkt.duration;
			/* XXX: should duplicate packet data in DV case */
			SDL_CondSignal(cond);
			return 0;
		}

		int Put(AVPacket *pkt) {
			SDL_LockMutex(mutex);
			int ret = PutPrivate(pkt);
			SDL_UnlockMutex(mutex);
			if (pkt != &flush_pkt && ret < 0)
				av_packet_unref(pkt);
			return ret;
		}

		int PutNullpkt(int stream_index) {
			AVPacket pkt1, *pkt = &pkt1;
			av_init_packet(pkt);
			pkt->data = nullptr;
			pkt->size = 0;
			pkt->stream_index = stream_index;
			return Put(pkt);
		}

		/* packet queue handling */
		int Init() {
			mutex = SDL_CreateMutex();
			cond = SDL_CreateCond();
			m_bAbortRequest = 1;
			return 0;
		}

		void Flush() {
			PacketList *pkt, *pkt1;

			SDL_LockMutex(mutex);
			for (pkt = first_pkt; pkt; pkt = pkt1) {
				pkt1 = pkt->next;
				av_packet_unref(&pkt->pkt);
				delete pkt;
			}
			last_pkt = nullptr;
			first_pkt = nullptr;
			nb_packets = 0;
			size = 0;
			duration = 0;
			SDL_UnlockMutex(mutex);
		}

		void Destroy() {
			Flush();
			SDL_DestroyMutex(mutex);
			SDL_DestroyCond(cond);
		}

		void Abort() {
			SDL_LockMutex(mutex);
			m_bAbortRequest = 1;
			SDL_CondSignal(cond);
			SDL_UnlockMutex(mutex);
		}

		void Start() {
			SDL_LockMutex(mutex);
			m_bAbortRequest = 0;
			PutPrivate(&flush_pkt);
			SDL_UnlockMutex(mutex);
		}

		/* return < 0 if aborted, 0 if no packet and > 0 if packet.  */
		int Get(AVPacket *pkt, int block, int *serial) {
			PacketList *pkt1;
			int ret;
			SDL_LockMutex(mutex);
			for (;;) {
				if (m_bAbortRequest) {
					ret = -1;
					break;
				}
				pkt1 = first_pkt;
				if (pkt1) {
					first_pkt = pkt1->next;
					if (!first_pkt)
						last_pkt = nullptr;
					nb_packets--;
					size -= pkt1->pkt.size + sizeof(*pkt1);
					duration -= pkt1->pkt.duration;
					*pkt = pkt1->pkt;
					if (serial)
						*serial = pkt1->serial;
					delete pkt1;
					ret = 1;
					break;
				}
				else if (!block) {
					ret = 0;
					break;
				}
				else {
					SDL_CondWait(cond, mutex);
				}
			}
			SDL_UnlockMutex(mutex);
			return ret;
		}
	};

	class AudioParams {
	public:
		int freq = 44100;
		int channels = 2;
		int64_t channel_layout = 3;
		enum AVSampleFormat fmt = AV_SAMPLE_FMT_S16;
		int frame_size = 0;
		int bytes_per_sec = 0;
	};

	class Clock {
	public:
		double pts = 0;
		double pts_drift = 0;
		double last_updated = 0;
		double speed = 0;
		int serial = 0;
		int paused = 0;
		int *queue_serial = nullptr;

		float m_fClockSpeed = 1.0;

		double Get() {
			if (*queue_serial != serial)
				return NAN;
			if (paused) {
				return pts;
			}
			else {
				double time = av_gettime_relative() / 1000000.0;
				return pts_drift + time - (time - last_updated) * (1.0 - speed);
			}
		}

		void Set(double _pts, int _serial, double _time) {
			pts = _pts;
			last_updated = _time;
			pts_drift = pts - _time;
			serial = _serial;
		}

		void Set2(double _pts, int _serial, float fspeed) {
			double time = av_gettime_relative() / 1000000.0;
			Set(_pts, _serial, time);
			m_fClockSpeed = fspeed;
		}


		void Init(int *_queue_serial) {
			speed = 1.0;
			paused = 0;
			queue_serial = _queue_serial;
			Set2(NAN, -1, 1.0);
		}

		void Sync(Clock *slave) {
			double clock = Get();
			double slave_clock = slave->Get();
			if (!isnan(slave_clock) && (isnan(clock) || fabs(clock - slave_clock) > AV_NOSYNC_THRESHOLD))
				Set2(slave_clock, slave->serial, m_fClockSpeed);
		}
	};

	class Frame {
	public:
		AVFrame *frame = nullptr;
		AVSubtitle sub;
		int serial = 0;
		double pts = 0;
		double duration = 0;
		int64_t pos = 0;
		int width = 0;
		int height = 0;
		int format = 0;
		AVRational sar;
		int uploaded = 0;
		int flip_v = 0;
		Frame() { memset(&sub, 0, sizeof(sub)); }
		void Unref() {
			av_frame_unref(frame);
			avsubtitle_free(&sub);
		}
	};

	class FrameQueue {
	public:
		WXLocker m_mutex;
		Frame queue[FRAME_QUEUE_SIZE];
		int rindex = 0;
		int windex = 0;
		int size = 0;
		int max_size = 0;
		int keep_last = 0;
		int rindex_shown = 0;
		SDL_mutex *mutex = nullptr;
		SDL_cond *cond = nullptr;
		PacketQueue *pktq = nullptr;
		void Signal() {
			SDL_LockMutex(mutex);
			SDL_CondSignal(cond);
			SDL_UnlockMutex(mutex);
		}
		int Init(PacketQueue *_pktq, int _max_size, int _keep_last) {
			WXAutoLock al(m_mutex);
			mutex = SDL_CreateMutex();
			cond = SDL_CreateCond();
			pktq = _pktq;
			max_size = FFMIN(_max_size, FRAME_QUEUE_SIZE);
			keep_last = !!_keep_last;
			for (int i = 0; i < max_size; i++)
				if (!(queue[i].frame = av_frame_alloc()))
					return AVERROR(ENOMEM);
			return 0;
		}

		void Destroy() {
			WXAutoLock al(m_mutex);
			for (int i = 0; i < max_size; i++) {
				Frame *vp = &queue[i];
				vp->Unref();
				av_frame_free(&vp->frame);
			}
			if (mutex) {
				SDL_DestroyMutex(mutex);
				mutex = nullptr;
			}
			if (cond) {
				SDL_DestroyCond(cond);
				cond = nullptr;
			}
		}

		Frame *Peek() {
			return &queue[(rindex + rindex_shown) % max_size];
		}

		Frame *PeekNext() {
			return &queue[(rindex + rindex_shown + 1) % max_size];
		}

		Frame *Last() {
			return &queue[rindex];
		}

		Frame *Writable() {
			/* wait until we have space to put a new frame */
			SDL_LockMutex(mutex);
			while (size >= max_size &&
				!pktq->m_bAbortRequest) {
				SDL_CondWait(cond, mutex);
			}
			SDL_UnlockMutex(mutex);

			if (pktq->m_bAbortRequest)
				return nullptr;

			return &queue[windex];
		}

		Frame *Readable() {
			WXAutoLock al(m_mutex);
			if (mutex == nullptr)return nullptr;
			/* wait until we have a readable a new frame */
			SDL_LockMutex(mutex);//退出时容易崩溃：
			while (size - rindex_shown <= 0 &&
				!pktq->m_bAbortRequest) {
				SDL_CondWait(cond, mutex);
			}
			SDL_UnlockMutex(mutex);

			if (pktq->m_bAbortRequest)
				return nullptr;

			return &queue[(rindex + rindex_shown) % max_size];
		}

		void Push() {
			if (++windex == max_size)
				windex = 0;
			SDL_LockMutex(mutex);
			size++;
			SDL_CondSignal(cond);
			SDL_UnlockMutex(mutex);
		}

		void Next() {
			if (keep_last && !rindex_shown) {
				rindex_shown = 1;
				return;
			}
			queue[rindex].Unref();
			if (++rindex == max_size)
				rindex = 0;
			SDL_LockMutex(mutex);
			size--;
			SDL_CondSignal(cond);
			SDL_UnlockMutex(mutex);
		}

		/* return the number of undisplayed frames in the queue */
		int Remaining() {
			return size - rindex_shown;
		}
	};

	class Decoder {
	public:
		AVPacket m_pkt;
		AVPacket pkt_temp;
		PacketQueue *m_queue = nullptr;
		AVCodecContext *m_avctx = nullptr;
		int pkt_serial = 0;
		int finished = 0;
		int packet_pending = 0;
		SDL_cond *m_empty_queue_cond = nullptr;
		int64_t start_pts = 0;
		AVRational start_pts_tb;
		int64_t next_pts = 0;
		AVRational next_pts_tb;
		SDL_Thread *decoder_tid = nullptr;
		Decoder() {
			av_init_packet(&m_pkt);
			av_init_packet(&pkt_temp);
		}
		void Init(AVCodecContext *avctx, PacketQueue *queue, SDL_cond *empty_queue_cond) {
			m_avctx = avctx;
			m_queue = queue;
			m_empty_queue_cond = empty_queue_cond;
			start_pts = AV_NOPTS_VALUE;
		}

		int DecodeFrame(AVFrame *frame, AVSubtitle *sub) {
			int got_frame = 0;
			do {
				int ret = -1;

				if (m_queue->m_bAbortRequest)
					return -1;

				if (!packet_pending || m_queue->serial != pkt_serial) {
					AVPacket pkt;
					do {
						if (m_queue->nb_packets == 0)
							SDL_CondSignal(m_empty_queue_cond);
						if (m_queue->Get(&pkt, 1, &pkt_serial) < 0)
							return -1;
						if (pkt.data == flush_pkt.data) {
							avcodec_flush_buffers(m_avctx);
							finished = 0;
							next_pts = start_pts;
							next_pts_tb = start_pts_tb;
						}
					} while (pkt.data == flush_pkt.data || m_queue->serial != pkt_serial);
					av_packet_unref(&m_pkt);
					pkt_temp = m_pkt = pkt;
					packet_pending = 1;
				}

				switch (m_avctx->codec_type) {
				case AVMEDIA_TYPE_VIDEO:
					ret = avcodec_decode_video2(m_avctx, frame, &got_frame, &pkt_temp);
					if (got_frame) {
						AVPixelFormat fff = (AVPixelFormat)frame->format;
						frame->width = frame->width / 4 * 4;
						frame->height = frame->height / 4 * 4;
						frame->pts = av_frame_get_best_effort_timestamp(frame);
					}
					break;
				case AVMEDIA_TYPE_AUDIO:
					ret = avcodec_decode_audio4(m_avctx, frame, &got_frame, &pkt_temp);
					if (got_frame) {
						AVRational tb{ 1, frame->sample_rate };
						if (frame->pts != AV_NOPTS_VALUE)
							frame->pts = av_rescale_q(frame->pts, av_codec_get_pkt_timebase(m_avctx), tb);
						else if (next_pts != AV_NOPTS_VALUE)
							frame->pts = av_rescale_q(next_pts, next_pts_tb, tb);
						if (frame->pts != AV_NOPTS_VALUE) {
							next_pts = frame->pts + frame->nb_samples;
							next_pts_tb = tb;
						}
					}
					break;
				case AVMEDIA_TYPE_SUBTITLE:
					ret = avcodec_decode_subtitle2(m_avctx, sub, &got_frame, &pkt_temp);
					break;
				}

				if (ret < 0) {
					packet_pending = 0;
				}
				else {
					pkt_temp.dts =
						pkt_temp.pts = AV_NOPTS_VALUE;
					if (pkt_temp.data) {
						if (m_avctx->codec_type != AVMEDIA_TYPE_AUDIO)
							ret = pkt_temp.size;
						pkt_temp.data += ret;
						pkt_temp.size -= ret;
						if (pkt_temp.size <= 0)
							packet_pending = 0;
					}
					else {
						if (!got_frame) {
							packet_pending = 0;
							finished = pkt_serial;
						}
					}
				}
			} while (!got_frame && !finished);

			return got_frame;
		}

		~Decoder() {
			if (m_avctx) {
				av_packet_unref(&m_pkt);
				avcodec_free_context(&m_avctx);
				m_avctx = nullptr;
			}
		}

		void Abort(FrameQueue *fq) {
			m_queue->Abort();
			fq->Signal();
			SDL_WaitThread(decoder_tid, nullptr);
			decoder_tid = nullptr;
			m_queue->Flush();
		}

		int Start(int(*fn)(void *), void *arg) {
			m_queue->Start();
			decoder_tid = SDL_CreateThread(fn, "decoder", arg);
			if (!decoder_tid) {
				WXLogWriteNew("AV_LOG_ERROR,SDL_CreateThread(): %s\n", SDL_GetError());
				return AVERROR(ENOMEM);
			}
			return 0;
		}
	};
public:
	WXFfplay() {
		if (!flush_pkt_init) {
			flush_pkt_init = TRUE;
			av_init_packet(&flush_pkt);
		}
	}

	~WXFfplay() {
		Destroy();
	}

	int OpenAudio(int64_t wanted_channel_layout, int wanted_nb_channels, int wanted_sample_rate, AudioParams *audio_hw_params) {
		SDL_AudioSpec wanted_spec;
		//const int next_nb_channels[] = { 0, 0, 1, 6, 2, 6, 4, 6 };
		const int next_sample_rates[] = { 0, 44100, 48000, 96000, 192000 };
		int next_sample_rate_idx = FF_ARRAY_ELEMS(next_sample_rates) - 1;


		wanted_nb_channels = 2;
		wanted_channel_layout = av_get_default_channel_layout(wanted_nb_channels);

		if (!wanted_channel_layout || wanted_nb_channels != av_get_channel_layout_nb_channels(wanted_channel_layout)) {
			wanted_channel_layout = av_get_default_channel_layout(wanted_nb_channels);
			wanted_channel_layout &= ~AV_CH_LAYOUT_STEREO_DOWNMIX;
		}
		wanted_nb_channels = av_get_channel_layout_nb_channels(wanted_channel_layout);
		wanted_spec.channels = wanted_nb_channels;
		wanted_spec.freq = wanted_sample_rate;
		if (wanted_spec.freq <= 0 || wanted_spec.channels <= 0) {
			WXLogWriteNew("AV_LOG_ERROR, Invalid sample rate or channel count!\n");
			return -1;
		}
		while (next_sample_rate_idx && next_sample_rates[next_sample_rate_idx] >= wanted_spec.freq)
			next_sample_rate_idx--;
		wanted_spec.format = AUDIO_S16SYS;
		wanted_spec.silence = 0;
		wanted_spec.samples = FFMAX(SDL_AUDIO_MIN_BUFFER_SIZE, 2 << av_log2(wanted_spec.freq / SDL_AUDIO_MAX_CALLBACKS_PER_SEC));
		wanted_spec.callback = AudioCallback;
		wanted_spec.userdata = this;
		m_pSoundPlayer = WXSoundPlayerCreate2(wanted_sample_rate, wanted_nb_channels, this, AudioCallback);

		if (m_pSoundPlayer == NULL) {
			WXLogWriteNew(" 音频设备初始化失败 ");
			return -1;
		}

		audio_hw_params->fmt = AV_SAMPLE_FMT_S16;
		audio_hw_params->freq = wanted_sample_rate;
		audio_hw_params->channel_layout = wanted_channel_layout;
		audio_hw_params->channels = wanted_nb_channels;
		audio_hw_params->frame_size = av_samples_get_buffer_size(nullptr, audio_hw_params->channels, 1, audio_hw_params->fmt, 1);
		audio_hw_params->bytes_per_sec = av_samples_get_buffer_size(nullptr, audio_hw_params->channels, audio_hw_params->freq, audio_hw_params->fmt, 1);
		if (audio_hw_params->bytes_per_sec <= 0 || audio_hw_params->frame_size <= 0) {
			WXLogWriteNew("AV_LOG_ERROR, av_samples_get_buffer_size failed\n");
			return -1;
		}
		return wanted_sample_rate * wanted_nb_channels * 2 / 100;//10ms 数据量
	}

	int OpenStream(int stream_index) { //打开指定通道
		AVFormatContext *ic = m_ic;
		AVCodecContext *avctx = nullptr;
		AVCodec *codec = nullptr;
		AVDictionary *opts = nullptr;
		AVDictionaryEntry *t = nullptr;
		int sample_rate, nb_channels;
		int64_t channel_layout;
		int ret = 0;
		int stream_lowres = 0;

		if (stream_index < 0 || stream_index >= ic->nb_streams)
			return -1;

		avctx = avcodec_alloc_context3(nullptr);
		ret = avcodec_parameters_to_context(avctx, ic->streams[stream_index]->codecpar);

		if (ret < 0)
			goto fail;
		av_codec_set_pkt_timebase(avctx, ic->streams[stream_index]->time_base);

		codec = avcodec_find_decoder(avctx->codec_id);
		if (!codec) {
			goto fail;
		}

		avctx->codec_id = codec->id;
		if (stream_lowres > av_codec_get_max_lowres(codec)) {
			stream_lowres = av_codec_get_max_lowres(codec);
		}
		av_codec_set_lowres(avctx, stream_lowres);

		opts = filter_codec_opts(m_codec_opts, avctx->codec_id, ic, ic->streams[stream_index], codec);
		if (!av_dict_get(opts, "threads", nullptr, 0))
			av_dict_set(&opts, "threads", "auto", 0);
		if (stream_lowres)
			av_dict_set_int(&opts, "lowres", stream_lowres, 0);
		if (avctx->codec_type == AVMEDIA_TYPE_VIDEO || avctx->codec_type == AVMEDIA_TYPE_AUDIO)
			av_dict_set(&opts, "refcounted_frames", "1", 0);
		if ((ret = avcodec_open2(avctx, codec, &opts)) < 0) {
			goto fail;
		}
		if ((t = av_dict_get(opts, "", nullptr, AV_DICT_IGNORE_SUFFIX))) {
			WXLogWriteNew("AV_LOG_ERROR, Option %s not found.\n", t->key);
			ret = AVERROR_OPTION_NOT_FOUND;
			goto fail;
		}

		m_EOF = 0;
		ic->streams[stream_index]->discard = AVDISCARD_DEFAULT;
		switch (avctx->codec_type) {
		case AVMEDIA_TYPE_AUDIO:
			m_nSampleRate = avctx->sample_rate;//频率
			AVFilterContext *sink;
			audio_filter_src.freq = avctx->sample_rate;
			audio_filter_src.channels = avctx->channels;
			audio_filter_src.channel_layout = get_valid_channel_layout(avctx->channel_layout, avctx->channels);
			audio_filter_src.fmt = avctx->sample_fmt;

			if ((ret = configure_audio_filters(m_strAF.length() ? m_strAF.c_str() : nullptr, 0)) < 0)
				goto fail;
			sink = out_audio_filter;
			sample_rate = av_buffersink_get_sample_rate(sink);
			nb_channels = av_buffersink_get_channels(sink);
			channel_layout = av_buffersink_get_channel_layout(sink);
			/* prepare audio output */
			if ((ret = OpenAudio(channel_layout, nb_channels, sample_rate, &audio_tgt)) < 0)
				goto fail;

			audio_hw_buf_size = ret;
			audio_src = audio_tgt;
			audio_buf_size = 0;
			audio_buf_index = 0;

			/* init averaging filter */
			audio_diff_avg_coef = exp(log(0.01) / AUDIO_DIFF_AVG_NB);
			audio_diff_avg_count = 0;
			/* since we do not have a precise anough audio FIFO fullness,
			we correct audio sync only if larger than this threshold */
			audio_diff_threshold = (double)(audio_hw_buf_size) / audio_tgt.bytes_per_sec;
			m_iAudioStream = stream_index;
			m_stAudio = ic->streams[stream_index];
			//m_dDurationAudio = m_stAudio->duration*av_q2d(m_stAudio->time_base);
			m_decoderAudio.Init(avctx, &audioq, continue_read);
			if ((m_ic->iformat->flags & (AVFMT_NOBINSEARCH | AVFMT_NOGENSEARCH | AVFMT_NO_BYTE_SEEK)) && !m_ic->iformat->read_seek) {
				m_decoderAudio.start_pts = m_stAudio->start_time;
				m_decoderAudio.start_pts_tb = m_stAudio->time_base;
			}
			if ((ret = m_decoderAudio.Start(ThreadAudio, this)) < 0)
				goto out;
			break;

		case AVMEDIA_TYPE_VIDEO:
			m_iVideoStream = stream_index;
			m_stVideo = ic->streams[stream_index];
			//m_dDurationVideo = m_stVideo->duration*av_q2d(m_stVideo->time_base);
			m_decoderVideo.Init(avctx, &videoq, continue_read);
			if ((ret = m_decoderVideo.Start(ThreadVideo, this)) < 0)
				goto out;
			m_bQueueQttachmentsReq = 1;
			break;
		case AVMEDIA_TYPE_SUBTITLE:
			m_iSubtitleStream = stream_index;
			subtitle_st = ic->streams[stream_index];
			m_decoderSubtitle.Init(avctx, &subtitleq, continue_read);
			if ((ret = m_decoderSubtitle.Start(ThreadSubtitle, this)) < 0)
				goto out;
			break;
		default:
			break;
		}
		goto out;
	fail:
		avcodec_free_context(&avctx);
	out:
		av_dict_free(&opts);
		return ret;
	}

	int  configure_video_filters(AVFilterGraph *graph, const char *vfilters, AVFrame *frame) {
		const enum AVPixelFormat pix_fmts[] = { AV_PIX_FMT_YUV420P, AV_PIX_FMT_BGRA, AV_PIX_FMT_NONE };
		char sws_flags_str[512] = "";
		char buffersrc_args[256];
		int ret;
		AVFilterContext *filt_src = nullptr, *filt_out = nullptr, *last_filter = nullptr;
		AVCodecParameters *codecpar = m_stVideo->codecpar;
		AVRational fr = av_guess_frame_rate(m_ic, m_stVideo, nullptr);
		AVDictionaryEntry *e = nullptr;

		while ((e = av_dict_get(m_sws_dict, "", e, AV_DICT_IGNORE_SUFFIX))) {
			if (!strcmp(e->key, "sws_flags")) {
				av_strlcatf(sws_flags_str, sizeof(sws_flags_str), "%s=%s:", "flags", e->value);
			}
			else
				av_strlcatf(sws_flags_str, sizeof(sws_flags_str), "%s=%s:", e->key, e->value);
		}
		if (strlen(sws_flags_str))
			sws_flags_str[strlen(sws_flags_str) - 1] = '\0';

		graph->scale_sws_opts = av_strdup(sws_flags_str);

		snprintf(buffersrc_args, sizeof(buffersrc_args),
			"video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
			frame->width, frame->height, frame->format,
			m_stVideo->time_base.num, m_stVideo->time_base.den,
			codecpar->sample_aspect_ratio.num, FFMAX(codecpar->sample_aspect_ratio.den, 1));
		if (fr.num && fr.den)
			av_strlcatf(buffersrc_args, sizeof(buffersrc_args), ":frame_rate=%d/%d", fr.num, fr.den);

		if ((ret = avfilter_graph_create_filter(&filt_src,
			avfilter_get_by_name("buffer"),
			"m_buffer", buffersrc_args, nullptr,
			graph)) < 0)
			goto fail;

		ret = avfilter_graph_create_filter(&filt_out, avfilter_get_by_name("buffersink"), "m_buffersink", nullptr, nullptr, graph);

		if (ret < 0)
			goto fail;

		if ((ret = av_opt_set_int_list(filt_out, "pix_fmts", pix_fmts, AV_PIX_FMT_NONE, AV_OPT_SEARCH_CHILDREN)) < 0)
			goto fail;

		last_filter = filt_out;

		{   //支持旋转,设置旋转角度
			double theta = m_iRotate ? m_iRotate : get_rotation(m_stVideo);
			if (fabs(theta - 90) < 1.0) {
				INSERT_FILT("transpose", "clock");
			}
			else if (fabs(theta - 180) < 1.0) {
				INSERT_FILT("hflip", nullptr);
				INSERT_FILT("vflip", nullptr);
			}
			else if (fabs(theta - 270) < 1.0) {
				INSERT_FILT("transpose", "cclock");
			}
			else if (fabs(theta) > 1.0) {
				char rotate_buf[64];
				snprintf(rotate_buf, sizeof(rotate_buf), "%f*PI/180", theta);
				INSERT_FILT("rotate", rotate_buf);
			}
		}

		if ((ret = configure_filtergraph(graph, vfilters, filt_src, last_filter)) < 0)
			goto fail;

		in_video_filter = filt_src;
		out_video_filter = filt_out;

	fail:
		return ret;
	}

	int  synchronize_audio(int nb_samples) {
		int wanted_nb_samples = nb_samples;

		/* if not master, then we try to remove or add samples to correct the clock */
		if (get_master_sync_type() != AV_SYNC_AUDIO_MASTER) {
			double diff, avg_diff;
			int min_nb_samples, max_nb_samples;

			diff = m_clockAudio.Get() - get_master_clock();

			if (!isnan(diff) && fabs(diff) < AV_NOSYNC_THRESHOLD) {
				audio_diff_cum = diff + audio_diff_avg_coef * audio_diff_cum;
				if (audio_diff_avg_count < AUDIO_DIFF_AVG_NB) {
					/* not enough measures to have a correct estimate */
					audio_diff_avg_count++;
				}
				else {
					/* estimate the A-V difference */
					avg_diff = audio_diff_cum * (1.0 - audio_diff_avg_coef);
					if (fabs(avg_diff) >= audio_diff_threshold) {
						wanted_nb_samples = nb_samples + (int)(diff * audio_src.freq);
						min_nb_samples = ((nb_samples * (100 - SAMPLE_CORRECTION_PERCENT_MAX) / 100));
						max_nb_samples = ((nb_samples * (100 + SAMPLE_CORRECTION_PERCENT_MAX) / 100));
						wanted_nb_samples = av_clip(wanted_nb_samples, min_nb_samples, max_nb_samples);
					}
				}
			}
			else {
				audio_diff_avg_count = 0;
				audio_diff_cum = 0;
			}
		}
		return wanted_nb_samples;
	}

	int  audio_decode_frame() {
		int data_size = 0;
		int resampled_data_size = 0;
		int64_t dec_channel_layout = 0;
		av_unused double audio_clock0 = 0.0;
		int wanted_nb_samples = 0;
		Frame *af = NULL;

		if (paused)
			return -1;

		do {
#if defined(_WIN32)
			while (sampq.Remaining() == 0) {
				int64_t delay1 = (av_gettime_relative() - m_tsAudioCallbackTime);
				int64_t delay2 = 5000;// 1000000LL * (int64_t)audio_hw_buf_size / (int64_t)audio_tgt.bytes_per_sec / 2;
				if (delay1 > delay2)
					return -1;
				av_usleep(1000);
			}
#endif

			if (!(af = sampq.Readable()))
				return -1;
			/*add by vicky 2018.5.23 用于seek
			丢弃seek点之前的音频解码数据，
			如果视频帧的解码速度还未到达，音频解码等待，反之，正常解码
			*/

			/*if (m_bSeek) {
			WXString str;
			str.Format("-----------pts=%02f seek=%02f max_audio=%02f\n\r", af->pts, m_ptsSeekPos / 1000000.0, m_ptsAudioMax);
			OutputDebugString((LPCWSTR)(str.str()));
			}*/
			if (af->serial != audioq.serial)
				sampq.Next();
			else {
				if (!m_bSeek)
					sampq.Next();
				else {//seek
					if (!isnan(af->pts)) {
						double dpts = af->pts*m_fSpeed;
						if (dpts < m_ptsSeekPos / 1000000.0&&dpts < m_ptsAudioMax - FRAME_DURATION_THRESHOLD_MAX) {
							sampq.Next();
							return -1;
						}
						else {
							if (m_bAudioSeek)
								m_bAudioLast = true;
							if (m_iVideoStream >= 0)
								m_bAudioSeek = false;
							else
								sampq.Next();

							if (m_bVideoSeek) {
								audio_clock = af->pts;
								audio_clock_serial = af->serial;
								return -1;
							}
							/*WXString str;
							str.Format("*************pts=%02f seek=%02f max_audio=%02f\n\r", dpts, m_ptsSeekPos / 1000000.0, m_ptsAudioMax);
							OutputDebugString((LPCWSTR)(str.str()));*/
						}
					}
					else
						sampq.Next();
				}
				break;
			}
		} while (true);//while (af->serial != audioq.serial);

		data_size = av_samples_get_buffer_size(nullptr, av_frame_get_channels(af->frame),
			af->frame->nb_samples,
			(enum AVSampleFormat) af->frame->format, 1);

		dec_channel_layout =
			(af->frame->channel_layout && av_frame_get_channels(af->frame) == av_get_channel_layout_nb_channels(af->frame->channel_layout)) ?
			af->frame->channel_layout : av_get_default_channel_layout(av_frame_get_channels(af->frame));
		wanted_nb_samples = synchronize_audio(af->frame->nb_samples);

		if (af->frame->format != audio_src.fmt ||
			dec_channel_layout != audio_src.channel_layout ||
			af->frame->sample_rate != audio_src.freq ||
			(wanted_nb_samples != af->frame->nb_samples && !swr_ctx)) {
			SAFE_SWR_FREE(swr_ctx);
			swr_ctx = swr_alloc_set_opts(nullptr,
				audio_tgt.channel_layout, audio_tgt.fmt, audio_tgt.freq,
				dec_channel_layout, (enum AVSampleFormat)af->frame->format, af->frame->sample_rate,
				0, nullptr);
			if (!swr_ctx || swr_init(swr_ctx) < 0) {
				SAFE_SWR_FREE(swr_ctx);
				return -1;
			}
			audio_src.channel_layout = dec_channel_layout;
			audio_src.channels = av_frame_get_channels(af->frame);
			audio_src.freq = af->frame->sample_rate;
			audio_src.fmt = (enum AVSampleFormat) af->frame->format;
		}

		if (swr_ctx) {
			const uint8_t **in = (const uint8_t **)af->frame->extended_data;
			uint8_t **out = &audio_buf1;
			int out_count = (int64_t)wanted_nb_samples * audio_tgt.freq / af->frame->sample_rate + 256;
			int out_size = av_samples_get_buffer_size(nullptr, audio_tgt.channels, out_count, audio_tgt.fmt, 0);
			int len2;
			if (out_size < 0) {
				return -1;
			}
			if (wanted_nb_samples != af->frame->nb_samples) {
				if (swr_set_compensation(swr_ctx,
					(wanted_nb_samples - af->frame->nb_samples) * audio_tgt.freq / af->frame->sample_rate,
					wanted_nb_samples * audio_tgt.freq / af->frame->sample_rate) < 0) {
					return -1;
				}
			}
			av_fast_malloc(&audio_buf1, &audio_buf1_size, out_size);
			if (!audio_buf1)
				return AVERROR(ENOMEM);

			len2 = swr_convert(swr_ctx, out, out_count, in, af->frame->nb_samples);
			if (len2 < 0) {
				return -1;
			}
			if (len2 == out_count) {
				if (swr_init(swr_ctx) < 0)
					SAFE_SWR_FREE(swr_ctx);
			}
			audio_buf = audio_buf1;
			resampled_data_size = len2 * audio_tgt.channels * av_get_bytes_per_sample(audio_tgt.fmt);
		}
		else {
			audio_buf = af->frame->data[0];
			resampled_data_size = data_size;
		}

		audio_clock0 = audio_clock;
		/* update the audio clock with the pts */
		if (!isnan(af->pts))
			audio_clock = af->pts + (double)af->frame->nb_samples / af->frame->sample_rate;
		else
			audio_clock = NAN;
		audio_clock_serial = af->serial;

		return resampled_data_size;
	}

	int  queue_picture(AVFrame *src_frame, double pts, double duration, int64_t pos, int serial) {
		Frame *vp;
		if (!(vp = pictq.Writable()))
			return -1;

		vp->sar = src_frame->sample_aspect_ratio;
		vp->uploaded = 0;

		vp->width = src_frame->width;
		vp->height = src_frame->height;
		vp->format = src_frame->format;

		vp->pts = pts;
		vp->duration = duration;
		vp->pos = pos;
		vp->serial = serial;

		av_frame_move_ref(vp->frame, src_frame);
		pictq.Push();
		return 0;
	}

	/*int key_count = 0;
	int frame_num = 0;*/
	int  get_video_frame(AVFrame *frame) {
		int got_picture = 0;
		if ((got_picture = m_decoderVideo.DecodeFrame(frame, nullptr)) < 0)
			return -1;

		if (got_picture) {
			double dpts = NAN;

			if (frame->pts != AV_NOPTS_VALUE)
				dpts = av_q2d(m_stVideo->time_base) * frame->pts;
			/*frame_num++;
			if (frame->pict_type == AV_PICTURE_TYPE_I)
			{
			key_count++;
			WXString str;
			str.Format("pts=%02f key_count=%d frames=%d\n\r", dpts, key_count,frame_num);
			OutputDebugString((LPCWSTR)(str.str()));
			}*/
			frame->sample_aspect_ratio = av_guess_sample_aspect_ratio(m_ic, m_stVideo, frame);

			//音视频同步，丢弃不同步的帧
			if (get_master_sync_type() != AV_SYNC_VIDEO_MASTER) {
				if (frame->pts != AV_NOPTS_VALUE) {
					if (m_bSeek) {
						/*if (dpts < m_ptsSeekPos / 1000000.0 &&
						dpts<m_dDurationVideo-FRAME_DURATION_THRESHOLD_MAX) {*/
						if (dpts < m_ptsSeekPos / 1000000.0 &&
							dpts < m_ptsVideoMax - FRAME_DURATION_THRESHOLD_MAX) {
							av_frame_unref(frame);
							got_picture = 0;
						}
					}
					else {
						double diff = dpts - get_master_clock();
						if (!isnan(diff) && fabs(diff) < AV_NOSYNC_THRESHOLD &&
							diff - frame_last_filter_delay < 0 &&
							m_decoderVideo.pkt_serial == m_clockVideo.serial &&
							videoq.nb_packets) {
							av_frame_unref(frame);
							got_picture = 0;
						}
					}
				}
			}
			/*if (m_bSeek) {
			WXString str;
			str.Format("===============================================================================pts=%02f gotpic=%d vseek=%d  bseek=%d\n\r", dpts, got_picture,m_bVideoSeek?1:0,m_bSeek?1:0);
			OutputDebugString((LPCWSTR)(str.str()));
			}*/
		}

		return got_picture;
	}


	//视频刷新线程
	void video_refresh(double *remaining_time) {
		double time;

		if (m_stVideo) {
		retry:
			if (pictq.Remaining() == 0) {

				/*WXString str;
				str.Format("---------master=%02f  maxvideo=%02f  video_duration=%02f\n\r", get_master_clock(), m_ptsVideoMax,m_dDurationVideo);
				OutputDebugString((LPCWSTR)(str.str()));*/
				//if (m_EOF && m_bSendVideoStop == false) { //文件尾部且无数据
				//	m_bSendVideoStop = true;
				//	avffmpeg_OnError(FFPLAY_ERROR_OK_VIDEO_STOP);  //视频结束回调
				//}
				/*if (m_bSeek) {
				WXString str;
				str.Format("******************************************************************000000000000000000\n\r");
				OutputDebugString((LPCWSTR)(str.str()));
				}*/
			}
			else {
				double last_duration, duration, delay;
				Frame *vp, *lastvp;
				/* dequeue the picture */
				lastvp = pictq.Last();
				vp = pictq.Peek();

				if (vp->serial != videoq.serial) {
					pictq.Next();
					goto retry;
				}

				if (lastvp->serial != vp->serial)
					frame_timer = av_gettime_relative() / 1000000.0;

				if (paused)
					goto display;

				/* compute nominal last_duration */
				last_duration = vp_duration(lastvp, vp);
				delay = compute_target_delay(last_duration);
				/*if (m_bSeek) {
				WXString str;
				str.Format("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++latspts=%02f  curpts=%02f last_duration=%02f  delay=%02f\n\r",lastvp->pts,vp->pts, last_duration,delay);
				OutputDebugString((LPCWSTR)(str.str()));
				}*/
				time = av_gettime_relative() / 1000000.0;
				if (time < frame_timer + delay) {
					*remaining_time = FFMIN(frame_timer + delay - time, *remaining_time);
					/*if (m_bSeek) {
					WXString str;
					str.Format("******************************************************************remaining_time=%02f\n\r", *remaining_time);
					OutputDebugString((LPCWSTR)(str.str()));
					}*/
					goto display;
				}

				frame_timer += delay;
				if (delay > 0 && time - frame_timer > AV_SYNC_THRESHOLD_MAX)
					frame_timer = time;

				SDL_LockMutex(pictq.mutex);
				if (!isnan(vp->pts)) {
					m_clockVideo.Set2(vp->pts, vp->serial, m_fSpeed);
					m_clockExt.Sync(&m_clockVideo);

					//WXAutoLock als(&m_lockSpeed); //改变速度
					//m_ptsVideo = m_clockVideo.pts * 1000 * m_fSpeed;
				}
				SDL_UnlockMutex(pictq.mutex);

				if (pictq.Remaining() > 1) {
					Frame *nextvp = pictq.PeekNext();
					duration = vp_duration(vp, nextvp);
					if (!step && (get_master_sync_type() != AV_SYNC_VIDEO_MASTER) &&
						time > frame_timer + duration) {//丢视频帧，视频播放太慢追音频
						pictq.Next();
						/*WXString str;
						str.Format("******************************************************************drop  duration=%02f\n\r",duration);
						OutputDebugString((LPCWSTR)(str.str()));*/
						goto retry;
					}
				}

				pictq.Next();

				/*if (m_bSeek) {
				WXString str;
				str.Format("******************************************************************seeknext  delay=%02f  video_pts=%02f\n\r",delay, m_clockVideo.pts);
				OutputDebugString((LPCWSTR)(str.str()));
				}*/

				m_bForceRefresh = 1;

				if (step && !paused)
					stream_toggle_pause();
			}
		display:
			/* display picture */
			if (m_bForceRefresh && pictq.rindex_shown)
				if (m_stVideo) {
					video_image_display();
				}

		}
		m_bForceRefresh = 0;

		set_playback_progress();
	}

	//媒体播放进度
	void set_playback_progress() {
		/*
		add by vicky 2018.5.25 判定音视频媒体流播放完成
		文件读取完成（m_EOF=1），if(主时钟>=对应流max_pts)对应流播放完成
		针对无EOF文件或其他情况，if(主时钟>=媒体总时长)所有流播放完成
		*/
		m_fMasterClock = get_master_clock();
		if (!isnan(m_fMasterClock)) {
			{
				//WXAutoLock al(&m_lockSpeed);
				m_fMasterClock *= m_fSpeed;
			}
			if (!m_bSeeking) {
				if (m_fMasterClock < m_fDuration + 2.0) {
					if (m_EOF == 1) {
						if (!m_bSendAudioStop&&m_fMasterClock >= m_ptsAudioMax + PLAY_FINISH_THRESHOLD) {
							WXString str;
							str.Format("###############################master=%02f duration=%02f seek=%d\n\r", m_fMasterClock, m_fDuration, m_bSeeking ? 1 : 0);
							//OutputDebugString((LPCWSTR)(str.str()));
							m_bSendAudioStop = true;//发送音频结束标记
							avffmpeg_OnError(FFPLAY_ERROR_OK_AUDIO_STOP);
						}
						if (!m_bSendVideoStop&&m_fMasterClock >= m_ptsVideoMax + PLAY_FINISH_THRESHOLD) {
							m_bSendVideoStop = true;//发送视频结束标记
							avffmpeg_OnError(FFPLAY_ERROR_OK_VIDEO_STOP);
						}
						if (m_bSendAudioStop&&m_bSendVideoStop && !m_bSendMediaStop) {
							m_bSendMediaStop = true;//发送媒体文件结束标记
							avffmpeg_OnError(FFPLAY_ERROR_OK_FINISH);
						}
					}
					m_ptsMedia = m_fMasterClock;
				}
				else {//有些媒体文件没有eof标志，只能用总时长来判断，例如dvd里面的vob文件
					if (!m_bSendAudioStop) {
						WXString str;
						str.Format("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&master=%02f duration=%02f  seek=%d\n\r", m_fMasterClock, m_fDuration, m_bSeeking ? 1 : 0);
						//OutputDebugString((LPCWSTR)(str.str()));
						m_bSendAudioStop = true;//发送音频结束标记
						avffmpeg_OnError(FFPLAY_ERROR_OK_AUDIO_STOP);
					}
					if (!m_bSendVideoStop) {
						m_bSendVideoStop = true;//发送视频结束标记
						avffmpeg_OnError(FFPLAY_ERROR_OK_VIDEO_STOP);
					}
					if (m_bSendAudioStop&&m_bSendVideoStop && !m_bSendMediaStop) {
						m_bSendMediaStop = true;//发送媒体文件结束标记
						avffmpeg_OnError(FFPLAY_ERROR_OK_FINISH);
					}
					m_ptsMedia = m_fDuration;
				}
			}

			//add by vicky 2018.5.23 用于seek
			if (m_bSeek && !m_bVideoSeek && !m_bAudioSeek) {
				WXAutoLock al(m_lockSeek);
				if (!m_bSeekReq) {
					m_ptsMedia = m_fMasterClock;
					m_bSeek = false;
					m_bSeeking = false;

					/*while (m_stVideo == nullptr) {
					m_fMasterClock = get_master_clock();
					if (!m_bAudioSeek &&!isnan(m_fMasterClock)&&(m_fMasterClock >= m_ptsSeekPos / 1000000.0 || m_fMasterClock >= m_ptsAudioMax - FRAME_DURATION_THRESHOLD_MAX)) {
					m_bSeeking = false;
					break;
					}
					else if (m_bAudioSeek) {
					m_bSeeking = true;
					m_bSeek = true;
					break;
					}
					else
					av_usleep(10);
					}
					if (m_stVideo != nullptr)
					m_bSeeking = false;*/
				}
			}
		}
	}

	int get_master_sync_type() {
		if (m_typeSync == AV_SYNC_VIDEO_MASTER) {
			if (m_stVideo)
				return AV_SYNC_VIDEO_MASTER;
			else
				return AV_SYNC_AUDIO_MASTER;
		}
		else if (m_typeSync == AV_SYNC_AUDIO_MASTER) {
			if (m_stAudio)
				return AV_SYNC_AUDIO_MASTER;
			else
				return AV_SYNC_EXTERNAL_CLOCK;
		}
		else {
			return AV_SYNC_EXTERNAL_CLOCK;
		}
	}

	int configure_audio_filters(const char *afilters, int force_output_format) {
		const enum AVSampleFormat sample_fmts[] = { AV_SAMPLE_FMT_S16, AV_SAMPLE_FMT_NONE };
		int sample_rates[2] = { 0, -1 };
		int64_t channel_layouts[2] = { 0, -1 };
		int channels[2] = { 0, -1 };
		AVFilterContext *filt_asrc = nullptr, *filt_asink = nullptr;
		char aresample_swr_opts[512] = "";
		AVDictionaryEntry *e = nullptr;
		char asrc_args[256];
		int ret;

		avfilter_graph_free(&agraph);
		if (!(agraph = avfilter_graph_alloc()))
			return AVERROR(ENOMEM);

		while ((e = av_dict_get(m_swr_opts, "", e, AV_DICT_IGNORE_SUFFIX)))
			av_strlcatf(aresample_swr_opts, sizeof(aresample_swr_opts), "%s=%s:", e->key, e->value);
		if (strlen(aresample_swr_opts))
			aresample_swr_opts[strlen(aresample_swr_opts) - 1] = '\0';
		av_opt_set(agraph, "aresample_swr_opts", aresample_swr_opts, 0);

		ret = snprintf(asrc_args, sizeof(asrc_args),
			"sample_rate=%d:sample_fmt=%s:channels=%d:time_base=%d/%d",
			audio_filter_src.freq, av_get_sample_fmt_name(audio_filter_src.fmt),
			audio_filter_src.channels,
			1, audio_filter_src.freq);

		if (audio_filter_src.channel_layout)
			snprintf(asrc_args + ret, sizeof(asrc_args) - ret,
				":channel_layout=0x%" PRIx64, audio_filter_src.channel_layout);

		ret = avfilter_graph_create_filter(&filt_asrc,
			avfilter_get_by_name("abuffer"), "m_abuffer",
			asrc_args, nullptr, agraph);
		if (ret < 0)
			goto end;


		ret = avfilter_graph_create_filter(&filt_asink,
			avfilter_get_by_name("abuffersink"), "m_abuffersink",
			nullptr, nullptr, agraph);
		if (ret < 0)
			goto end;

		if ((ret = av_opt_set_int_list(filt_asink, "sample_fmts", sample_fmts, AV_SAMPLE_FMT_NONE, AV_OPT_SEARCH_CHILDREN)) < 0)
			goto end;
		if ((ret = av_opt_set_int(filt_asink, "all_channel_counts", 1, AV_OPT_SEARCH_CHILDREN)) < 0)
			goto end;

		if (force_output_format) {
			channel_layouts[0] = audio_tgt.channel_layout;
			channels[0] = audio_tgt.channels;
			sample_rates[0] = audio_tgt.freq;
			if ((ret = av_opt_set_int(filt_asink, "all_channel_counts", 0, AV_OPT_SEARCH_CHILDREN)) < 0)
				goto end;
			if ((ret = av_opt_set_int_list(filt_asink, "channel_layouts", channel_layouts, -1, AV_OPT_SEARCH_CHILDREN)) < 0)
				goto end;
			if ((ret = av_opt_set_int_list(filt_asink, "channel_counts", channels, -1, AV_OPT_SEARCH_CHILDREN)) < 0)
				goto end;
			if ((ret = av_opt_set_int_list(filt_asink, "sample_rates", sample_rates, -1, AV_OPT_SEARCH_CHILDREN)) < 0)
				goto end;
		}

		if ((ret = configure_filtergraph(agraph, afilters, filt_asrc, filt_asink)) < 0)
			goto end;

		in_audio_filter = filt_asrc;
		out_audio_filter = filt_asink;

	end:
		if (ret < 0)
			avfilter_graph_free(&agraph);
		return ret;
	}

	/* pause or resume the video */
	void stream_toggle_pause() {
		if (paused) {
			frame_timer += av_gettime_relative() / 1000000.0 - m_clockVideo.last_updated;
			if (m_ReadPauseReturn != AVERROR(ENOSYS)) {
				m_clockVideo.paused = 0;
			}
			m_clockVideo.Set2(m_clockVideo.Get(), m_clockVideo.serial, m_fSpeed);
		}
		m_clockExt.Set2(m_clockExt.Get(), m_clockExt.serial, m_fSpeed);
		paused = m_clockAudio.paused = m_clockVideo.paused = m_clockExt.paused = !paused;
	}

	void toggle_pause() { //暂停恢复。。
		stream_toggle_pause();
		step = 0;
	}

	void step_to_next_frame() {//刷新一帧后通知底层暂停
		if (paused)
			stream_toggle_pause();
		step = 1;
	}

	/* get the current master clock value */
	double get_master_clock() {//时间
		double val;

		switch (get_master_sync_type()) {
		case AV_SYNC_VIDEO_MASTER:
			val = m_clockVideo.Get();
			break;
		case AV_SYNC_AUDIO_MASTER:
			val = m_clockAudio.Get();
			break;
		default:
			val = m_clockExt.Get();
			break;
		}
		return val;
	}

	void CloseStream(int stream_index) {
		AVCodecParameters *codecpar;
		if (stream_index < 0 || stream_index >= m_ic->nb_streams)
			return;
		codecpar = m_ic->streams[stream_index]->codecpar;
		switch (codecpar->codec_type) {
		case AVMEDIA_TYPE_AUDIO:
			m_decoderAudio.Abort(&sampq);
			SAFE_SWR_FREE(swr_ctx);
			av_freep(&audio_buf1);
			audio_buf1_size = 0;
			audio_buf = nullptr;
			break;
		case AVMEDIA_TYPE_VIDEO:
			m_decoderVideo.Abort(&pictq);
			break;
		case AVMEDIA_TYPE_SUBTITLE:
			m_decoderSubtitle.Abort(&subpq);
			break;
		default:
			break;
		}

		m_ic->streams[stream_index]->discard = AVDISCARD_ALL;
		switch (codecpar->codec_type) {
		case AVMEDIA_TYPE_AUDIO:
			m_stAudio = nullptr;
			m_iAudioStream = -1;
			break;
		case AVMEDIA_TYPE_VIDEO:
			m_stVideo = nullptr;
			m_iVideoStream = -1;
			break;
		case AVMEDIA_TYPE_SUBTITLE:
			subtitle_st = nullptr;
			m_iSubtitleStream = -1;
			break;
		default:
			break;
		}
	}

	double m_fRed = 0.0;
	double m_fGreen = 0.0;
	double m_fBlue = 0.0;
	double m_fAlpha = 0.0;

	void SetBgColor(double red, double green, double blue, double alpha) {
		m_fRed = red;
		m_fGreen = green;
		m_fBlue = blue;
		m_fAlpha = alpha;
	}

	int  CutSize(AVFrame *frame) {
		int side = 0;
		if (frame->format == AV_PIX_FMT_YUV420P || frame->format == AV_PIX_FMT_YUVJ420P) {
			int w = frame->width;
			int h = frame->height;
			int pitch = frame->linesize[0];
			uint8_t* buf = frame->data[0];
			for (side = 0; side < w / 2; side++) {
				int64_t sum = 0;
				for (int i = 0; i < h; i++) {
					sum += buf[i * pitch + side];
				}
				if (sum / h > 0x20)
					break;
			}
		}
		return side / 2 * 2;//不能为奇数
	}

	void SetUsingCutSide(int b) {
		m_bUsingCutSide = b;
	}
    
    
    WXVideoConvert m_conv;
    WXVideoFrame  m_displayFrame;
    int m_tW = 0;
    int m_tH = 0;
	void video_image_display() {
		Frame *sp = nullptr;
		Frame *vp = pictq.Last();
		/*add by vicky 2018.5.23 用于seek
		seek点之前的帧不显示，如果seek点大于视频流的总时长，
		视频流结尾的帧显示，并更新seek状态
		*/
		if (m_bVideoSeek) {
			double dpts = vp->pts*m_fSpeed;
			/*WXString str;
			str.Format("pts=%02f seekpos=%02f duration=%02f\n\r", dpts, m_ptsSeekPos / 1000000.0f, m_dDurationVideo);
			OutputDebugString((LPCWSTR)(str.str()));*/
			if (dpts < m_ptsSeekPos / 1000000.0f
				&&dpts < m_ptsVideoMax - FRAME_DURATION_THRESHOLD_MAX)
				return;
			else {
				m_bVideoSeek = false;
			}
		}

#define IOS_ALIGN 8
		if (m_cbVideo || m_hwnd) {
            AVFrame *avframe = vp->frame;

			if (m_dataBuffer.m_iBufSize == 0 ||
                m_tW != avframe->width  / IOS_ALIGN * IOS_ALIGN ||
                m_tH != avframe->height  / IOS_ALIGN * IOS_ALIGN) {
                m_tW = avframe->width / IOS_ALIGN * IOS_ALIGN;
                m_tH = avframe->height / IOS_ALIGN * IOS_ALIGN;
			}
            
            if(avframe->format != AV_PIX_FMT_YUV420P && m_displayFrame.m_pFrame == nullptr){
                m_displayFrame.Init(AV_PIX_FMT_YUV420P, m_tW, m_tH );
                m_conv.Init(m_tW , m_tH , avframe->format, AV_PIX_FMT_YUV420P);
            }
            if(m_displayFrame.m_pFrame != nullptr){
                m_conv.Convert(avframe, m_displayFrame.m_pFrame);
                avframe = m_displayFrame.m_pFrame;
            }
            
            
            if(m_cbVideo){
                m_cbVideo( avframe->data, avframe->linesize, m_tW, m_tH);//回调处理
            }
            
            if (m_pRender == nullptr && m_hwnd && m_tW && m_tH) {
                m_pRender = WXVideoRenderCreate(m_hwnd, m_tW,m_tH, m_cbVideo);//渲染句柄
            }

            if (m_pRender) {
                WXVideoRenderDisplay(m_pRender, avframe->data, avframe->linesize);//渲染操作
            }
		}



        
		if (!vp->uploaded) {
			vp->uploaded = 1;
			vp->flip_v = vp->frame->linesize[0] < 0;
		}
	}

	//音视频同步
	double compute_target_delay(double delay) {
		double sync_threshold, diff = 0;
		if (get_master_sync_type() != AV_SYNC_VIDEO_MASTER) {
			diff = m_clockVideo.Get() - get_master_clock();
			sync_threshold = FFMAX(AV_SYNC_THRESHOLD_MIN, FFMIN(AV_SYNC_THRESHOLD_MAX, delay));
			if (!isnan(diff) && fabs(diff) < m_fMaxFrameDuration) {
				if (diff <= -sync_threshold)//视频慢了
					delay = FFMAX(0, delay + diff);
				else if (diff >= sync_threshold && delay > AV_SYNC_FRAMEDUP_THRESHOLD)//视频快了，并且帧时长很大
					delay = delay + diff;
				else if (diff >= sync_threshold)//视频快了，并且帧时长在可控范围内
					delay = 2 * delay;
			}
		}
		return delay;
	}

	double vp_duration(Frame *vp, Frame *nextvp) {
		if (vp->serial == nextvp->serial) {
			double duration = nextvp->pts - vp->pts;
			if (isnan(duration) || duration <= 0 || duration > m_fMaxFrameDuration)
				return vp->duration;
			else
				return duration;
		}
		return 0.0;
	}

public:
	int m_iVolume = 100;
	int m_iMuted = 0;

	int64_t m_ptsStartTime = AV_NOPTS_VALUE;//-ss 起始时间
	int64_t m_ptsDurationTime = AV_NOPTS_VALUE;//播放长度

	float m_fSpeed = 1.0;
	int   m_iNewSpeed = 100;//通过SetSpeed 设置后 m_speed 值需要在滤镜配置成功后才能替换，否则GetCurrTime就不准

	int   m_nSampleRate = 0;

	bool m_bStopVideoThread = false;
	bool m_bChangeVF = false; //通知底层修改滤镜
							  //char *m_vfilters = nullptr;//-vf
	WXString m_strVF = L"";

	bool m_bStopAudioThread = false;
	bool m_bChangeAF = false; //通知底层修改滤镜
							  //char *m_afilters = nullptr;//-af
	WXString m_strAF = L"";

	int m_scan_all_pmts_set = 0;
	int m_iStIndex[AVMEDIA_TYPE_NB] = { -1,-1,-1,-1,-1 };


	AVDictionary *m_sws_dict = nullptr;
	AVDictionary *m_swr_opts = nullptr;
	AVDictionary *m_format_opts = nullptr;
	AVDictionary *m_codec_opts = nullptr;
	AVDictionary *m_resample_opts = nullptr;

	int64_t m_tsAudioCallbackTime = 0;
	int m_bGenPts = 0;//可能有用

	SDL_Thread *m_ThreadReadID = nullptr;
	AVFormatContext *m_ic = nullptr;//文件容器

	int m_bAbortRequest = 0;
	int m_bForceRefresh = 0;
	int paused = 0;
	int m_bLastPaused = 0;
	int m_bQueueQttachmentsReq = 0;
	int m_bSeekReq = 0;
	int m_iSeekFlags = 0;
	int64_t m_ptsSeekPos = 0;
	bool m_bSeek = false;//媒体文件seek标志，由媒体文件中所有流控制
	bool m_bSeeking = false;//seek时，防止向上层返回脏数据，
							//seek发起至seek到数据,返给上层seek时间点(m_ptsSeekPos)
	bool m_bAudioSeek = false;//音频流seek
	bool m_bVideoSeek = false;//视频流seek
	int64_t m_ptsSeekRel = 0;
	int m_ReadPauseReturn = 0;

	Clock m_clockAudio;
	Clock m_clockVideo;
	Clock m_clockExt;

	FrameQueue pictq;//图像队列
	FrameQueue subpq;//字幕队列
	FrameQueue sampq;//音频队列

	Decoder m_decoderAudio;
	Decoder m_decoderVideo;
	Decoder m_decoderSubtitle;

	double audio_clock = 0.0;
	int audio_clock_serial = 0.0;
	double audio_diff_cum = 0.0;
	double audio_diff_avg_coef = 0.0;
	double audio_diff_threshold = 0.0;
	int audio_diff_avg_count = 0.0;
	AVStream *m_stAudio = nullptr;
	PacketQueue audioq;
	int audio_hw_buf_size = 0.0;
	uint8_t *audio_buf = nullptr;
	uint8_t *audio_buf1 = nullptr;
	unsigned int audio_buf_size = 0; /* in bytes */
	unsigned int audio_buf1_size = 0;
	int audio_buf_index = 0; /* in bytes */
	int audio_write_buf_size = 0;

	AudioParams audio_src;
	AudioParams audio_filter_src;
	AudioParams audio_tgt;
	struct SwrContext *swr_ctx = nullptr;

	AVStream *subtitle_st = nullptr;
	PacketQueue subtitleq;

	double frame_timer = 0;
	double frame_last_returned_time = 0;
	double frame_last_filter_delay = 0;

	AVStream *m_stVideo = nullptr;
	PacketQueue videoq;
	double m_fMaxFrameDuration = 10.0;

	int m_EOF = 0;//文件结束标记

	int width = 0, height = 0, xleft = 0, ytop = 0;
	int step = 0;

	AVFilterContext *in_video_filter = nullptr;
	AVFilterContext *out_video_filter = nullptr;
	AVFilterContext *in_audio_filter = nullptr;
	AVFilterContext *out_audio_filter = nullptr;
	AVFilterGraph *agraph = nullptr;
	SDL_cond *continue_read = nullptr;

	int m_iSubtitleStream = -1;
	int m_iVideoStream = -1;
	int m_iAudioStream = -1;
	int m_typeSync = AV_SYNC_AUDIO_MASTER;

public:
	static int decode_interrupt_cb(void *ctx) {
		WXFfplay *is = (WXFfplay *)ctx;
		return is->m_bAbortRequest;
	}

	bool m_bAudioLast = false;
	//播放音频数据  同步
	static void AudioCallback(void *opaque, Uint8 *stream, int len) {
		memset(stream, 0, len);
		WXFfplay *is = (WXFfplay *)opaque;

		int audio_size = 0, len1 = 0;
		is->m_tsAudioCallbackTime = av_gettime_relative();
		while (len > 0) {
			if (is->audio_buf_index >= is->audio_buf_size) {
				audio_size = is->audio_decode_frame();
				if (audio_size < 0) {
					/* if error, just output silence */
					is->audio_buf = nullptr;
					is->audio_buf_size = SDL_AUDIO_MIN_BUFFER_SIZE / is->audio_tgt.frame_size * is->audio_tgt.frame_size;
				}
				else {
					is->audio_buf_size = audio_size;
				}
				is->audio_buf_index = 0;
			}
			len1 = is->audio_buf_size - is->audio_buf_index;
			if (len1 > len)
				len1 = len;
			if (!is->m_iMuted && is->audio_buf && is->m_iVolume == SDL_MIX_MAXVOLUME)
				memcpy(stream, (uint8_t *)is->audio_buf + is->audio_buf_index, len1);
			else {
				if (is->m_iMuted || is->m_iVolume == 0)
					memset(stream, 0, len1);
				if (!is->m_iMuted && is->audio_buf) {
					memcpy(stream, (uint8_t *)is->audio_buf + is->audio_buf_index, len1);
					int16_t *pcm = (int16_t*)stream;
					for (int i = 0; i < len1 / 2; i++) {
						pcm[i] = (pcm[i] * is->m_iVolume / 100);
					}
				}
			}
			len -= len1;
			stream += len1;
			is->audio_buf_index += len1;
		}
		is->audio_write_buf_size = is->audio_buf_size - is->audio_buf_index;



		/* Let's assume the audio driver that is used by SDL has two periods. */
		if (!isnan(is->audio_clock) && (is->sampq.Remaining() > 0 || is->m_bAudioLast)) {// 
			is->m_clockAudio.Set(is->audio_clock - (double)(2 * is->audio_hw_buf_size + is->audio_write_buf_size) / is->audio_tgt.bytes_per_sec,
				is->audio_clock_serial, is->m_tsAudioCallbackTime / 1000000.0);
			is->m_clockExt.Sync(&is->m_clockAudio);


			//WXAutoLock als(&is->m_lockSpeed);//改变速度
			//is->m_ptsAudio = is->m_clockAudio.pts * 1000 * is->m_fSpeed;//音频时间戳

			if (is->sampq.Remaining() == 0 && is->m_bAudioLast &&is->m_iVideoStream >= 0)//(is->m_iVideoStream >= 0|| !is->m_bAudioSeek )
				is->m_bAudioLast = false;
		}

		/*
		add by vicky 2018.5.29
		针对仅播放音频，为了准确seek，只有播放点大于seek点才给出播放进度，防止脏数据
		*/
		if (is->m_bAudioSeek&&is->m_iVideoStream < 0 && is->m_bAudioLast) {
			double dclock = is->get_master_clock();
			if (!isnan(dclock)) {
				//{
				//WXAutoLock al(&(is->m_lockSpeed));
				dclock *= is->m_fSpeed;
				//}
				if (dclock >= is->m_ptsSeekPos / 1000000.0 || dclock >= is->m_ptsAudioMax - FRAME_DURATION_THRESHOLD_MAX) {
					is->m_bAudioSeek = false;
					is->m_bAudioLast = false;
				}
			}
		}
	}

	//解码音频帧至缓冲池
	static int ThreadAudio(void *arg) {
		WXFfplay *is = (WXFfplay *)arg;
		AVFrame *frame = av_frame_alloc();
		Frame *af;

		int64_t dec_channel_layout;
		int reconfigure;

		int got_frame = 0;
		AVRational tb;
		int ret = 0;

		if (!frame)
			return AVERROR(ENOMEM);

		do {
			if ((got_frame = is->m_decoderAudio.DecodeFrame(frame, nullptr)) < 0)
				goto the_end;

			if (got_frame) {
				//tb = (AVRational){1, frame->sample_rate};
				tb.num = 1;
				tb.den = frame->sample_rate;

				/*add by Vicky  2018.5.18
				seek时，丢掉seek点之前的音频帧
				*/
				if (is->m_bSeek) {
					double dpts = NAN;

					if (frame->pts != AV_NOPTS_VALUE)
						dpts = av_q2d(tb) * frame->pts;

					if (is->get_master_sync_type() != AV_SYNC_VIDEO_MASTER) {
						if (frame->pts != AV_NOPTS_VALUE) {
							/*WXString str;
							str.Format("++++++++++++++pts=%02f gotpic=%d\n\r", dpts, got_frame);
							OutputDebugString((LPCWSTR)(str.str()));*/
							/*if (dpts < is->m_ptsSeekPos / 1000000.0&&dpts < is->m_dDurationAudio - FRAME_DURATION_THRESHOLD_MAX) {*/
							if (dpts < is->m_ptsSeekPos / 1000000.0&&dpts < is->m_ptsAudioMax - FRAME_DURATION_THRESHOLD_MAX) {
								continue;
							}
							/*WXString str;
							str.Format("++++++++++++++pts=%02f gotpic=%d\n\r", dpts, got_frame);
							OutputDebugString((LPCWSTR)(str.str()));*/
						}
					}
				}

				dec_channel_layout = get_valid_channel_layout(frame->channel_layout, av_frame_get_channels(frame));

				{
					WXAutoLock al(is->m_lockFilter); //是否重置音频滤镜
					reconfigure =
						cmp_audio_fmts(is->audio_filter_src.fmt, is->audio_filter_src.channels,
						(enum AVSampleFormat) frame->format, av_frame_get_channels(frame)) ||
						is->audio_filter_src.channel_layout != dec_channel_layout ||
						is->audio_filter_src.freq != frame->sample_rate ||
						is->m_bChangeAF == true;

					if (reconfigure) {
						is->m_bChangeAF = false;
						char buf1[1024], buf2[1024];
						av_get_channel_layout_string(buf1, sizeof(buf1), -1, is->audio_filter_src.channel_layout);
						av_get_channel_layout_string(buf2, sizeof(buf2), -1, dec_channel_layout);

						is->audio_filter_src.fmt = (enum AVSampleFormat)frame->format;
						is->audio_filter_src.channels = av_frame_get_channels(frame);
						is->audio_filter_src.channel_layout = dec_channel_layout;
						is->m_nSampleRate = is->audio_filter_src.freq = frame->sample_rate;

						if ((ret = is->configure_audio_filters(is->m_strAF.length() ? is->m_strAF.c_str() : nullptr, 1)) < 0)
							goto the_end;


						WXAutoLock als(is->m_lockSpeed); //改变速度
						if (is->m_bSetSpeed && !is->m_bChangeVF) { //音频滤镜
							is->m_bSetSpeed = false;
							is->m_fSpeed = is->m_iNewSpeed / 100.0f;
						}
					}
				}


				if ((ret = av_buffersrc_add_frame(is->in_audio_filter, frame)) < 0)
					goto the_end;

				while ((ret = av_buffersink_get_frame_flags(is->out_audio_filter, frame, 0)) >= 0) {
					tb = av_buffersink_get_time_base(is->out_audio_filter);

					if (!(af = is->sampq.Writable()))
						goto the_end;

					af->pts = (frame->pts == AV_NOPTS_VALUE) ? NAN : frame->pts * av_q2d(tb);
					af->pos = av_frame_get_pkt_pos(frame);
					af->serial = is->m_decoderAudio.pkt_serial;

					AVRational tbk{ frame->nb_samples, frame->sample_rate };
					af->duration = av_q2d(tbk);

					av_frame_move_ref(af->frame, frame);
					is->sampq.Push();

					if (is->audioq.serial != is->m_decoderAudio.pkt_serial)
						break;
				}
				if (ret == AVERROR_EOF)
					is->m_decoderAudio.finished = is->m_decoderAudio.pkt_serial;
			}

			if (is->m_EOF && is->m_bSendAudioStop && is->m_bSendVideoStop) {
                SLEEPMS(20);
			}

		} while (ret >= 0 || ret == AVERROR(EAGAIN) || ret == AVERROR_EOF);

		is->m_bChangeAF = false;
	the_end:

		avfilter_graph_free(&is->agraph);

		av_frame_free(&frame);
		return ret;
	}
	//解码视频帧至缓冲池
	static int ThreadVideo(void *arg) {
		WXFfplay *is = (WXFfplay *)arg;
		AVFrame *frame = av_frame_alloc();
		double pts;
		double duration;
		int ret;
		AVRational tb = is->m_stVideo->time_base;
		AVRational frame_rate = av_guess_frame_rate(is->m_ic, is->m_stVideo, nullptr);

		AVFilterGraph *graph = avfilter_graph_alloc();
		AVFilterContext *filt_out = nullptr, *filt_in = nullptr;
		int last_w = 0;
		int last_h = 0;
		enum AVPixelFormat last_format = (enum AVPixelFormat) - 2;
		if (!graph) {
			av_frame_free(&frame);
			return AVERROR(ENOMEM);
		}

		if (!frame) {
			avfilter_graph_free(&graph);
			return AVERROR(ENOMEM);
		}

		while (1) {
			ret = is->get_video_frame(frame);
			if (ret < 0)
				goto the_end;
			if (!ret)continue;


			{
				WXAutoLock al(is->m_lockFilter); //是否重置视频滤镜
				if (last_w != frame->width
					|| last_h != frame->height
					|| last_format != frame->format ||
					is->m_bChangeVF == true) {

					is->m_bChangeVF = false;

					avfilter_graph_free(&graph);
					graph = avfilter_graph_alloc();
					if ((ret = is->configure_video_filters(graph, is->m_strVF.length() ? is->m_strVF.c_str() : nullptr, frame)) < 0) {
						goto the_end;
					}
					filt_in = is->in_video_filter;
					filt_out = is->out_video_filter;
					last_w = frame->width;
					last_h = frame->height;
					last_format = (enum AVPixelFormat)frame->format;
					frame_rate = av_buffersink_get_frame_rate(filt_out);

					WXAutoLock als(is->m_lockSpeed); //改变速度
					if (is->m_bSetSpeed && !is->m_bChangeAF) {
						is->m_bSetSpeed = false;
						is->m_fSpeed = is->m_iNewSpeed / 100.0f;
					}
				}
			}

			ret = av_buffersrc_add_frame(filt_in, frame);
			if (ret < 0)
				goto the_end;

			while (ret >= 0) {
				is->frame_last_returned_time = av_gettime_relative() / 1000000.0;

				ret = av_buffersink_get_frame_flags(filt_out, frame, 0);
				if (ret < 0) {
					if (ret == AVERROR_EOF)
						is->m_decoderVideo.finished = is->m_decoderVideo.pkt_serial;
					ret = 0;
					break;
				}

				is->frame_last_filter_delay = av_gettime_relative() / 1000000.0 - is->frame_last_returned_time;
				if (fabs(is->frame_last_filter_delay) > AV_NOSYNC_THRESHOLD / 10.0)
					is->frame_last_filter_delay = 0;
				tb = av_buffersink_get_time_base(filt_out);

				AVRational tbj{ frame_rate.den, frame_rate.num };
				duration = (frame_rate.num && frame_rate.den ? av_q2d(tbj) : 0);
				pts = (frame->pts == AV_NOPTS_VALUE) ? NAN : frame->pts * av_q2d(tb);
				ret = is->queue_picture(frame, pts, duration, av_frame_get_pkt_pos(frame), is->m_decoderVideo.pkt_serial);
				av_frame_unref(frame);
			}

			if (ret < 0)
				goto the_end;

			if (is->m_EOF && is->m_bSendAudioStop && is->m_bSendVideoStop) {
                SLEEPMS(20);
			}
		}

		is->m_bChangeVF = false;
	the_end:
		avfilter_graph_free(&graph);
		av_frame_free(&frame);
		return 0;
	}

	static int ThreadSubtitle(void *arg) {
		WXFfplay *is = (WXFfplay *)arg;
		Frame *sp;
		int got_subtitle;
		double pts;

		for (;;) {
			if (!(sp = is->subpq.Writable()))
				return 0;

			if ((got_subtitle = is->m_decoderSubtitle.DecodeFrame(nullptr, &sp->sub)) < 0)
				break;

			pts = 0;

			if (got_subtitle && sp->sub.format == 0) {
				if (sp->sub.pts != AV_NOPTS_VALUE)
					pts = sp->sub.pts / (double)AV_TIME_BASE;
				sp->pts = pts;
				sp->serial = is->m_decoderSubtitle.pkt_serial;
				sp->width = is->m_decoderSubtitle.m_avctx->width;
				sp->height = is->m_decoderSubtitle.m_avctx->height;
				sp->uploaded = 0;

				/* now we can update the picture count */
				is->subpq.Push();
			}
			else if (got_subtitle) {
				avsubtitle_free(&sp->sub);
			}
		}
		return 0;
	}
	//解复用音视频流（demux） and read data
	static int ThreadRead(void *arg) {
		WXFfplay *is = (WXFfplay *)arg;
		is->avffmpeg_OnError(FFPLAY_ERROR_OK_START);//开始启动线程
		int err = 0, ret = 0;
		AVPacket pkt1, *pkt = &pkt1;
		int64_t pkt_ts = 0;
		double temp_pts_second = 0.0;

		for (;;) {
			if (is->m_bAbortRequest)
				break;
			if (is->m_EOF && is->m_bSendAudioStop && is->m_bSendVideoStop) {
                SLEEPMS(20);
			}

			if (is->paused != is->m_bLastPaused) {
				is->m_bLastPaused = is->paused;
				if (is->paused)
					is->m_ReadPauseReturn = av_read_pause(is->m_ic);
				else
					av_read_play(is->m_ic);
			}

			{
				WXAutoLock al(is->m_lockSeek);
				if (is->m_bSeekReq) {  //多次覆盖Seek值只执行最后一个


					int64_t seek_target = is->m_ptsSeekPos;
					int64_t seek_min = is->m_ptsSeekRel > 0 ? seek_target - is->m_ptsSeekRel + 2 : INT64_MIN;
					int64_t seek_max = is->m_ptsSeekRel < 0 ? seek_target - is->m_ptsSeekRel - 2 : INT64_MAX;
					//ret = avformat_seek_file(is->m_ic, -1, seek_min, seek_target, seek_max, is->m_iSeekFlags);
					ret = av_seek_frame(is->m_ic, -1, seek_target, AVSEEK_FLAG_BACKWARD);

					if (ret >= 0) {
						/*add by vicky 2018.5.23 用于seek
						*/
						is->m_bSeek = true;
						if (is->m_iVideoStream >= 0)
							is->m_bVideoSeek = true;
						if (is->m_iAudioStream >= 0)
							is->m_bAudioSeek = true;
						WXString str;
						str.Format("----------------------------------------------------------------------seek = %02fs %d \n\r", seek_target / 1000000.0f, is->m_bSeeking ? 1 : 0);
						//OutputDebugString((LPCWSTR)(str.str()));

						if (is->m_iAudioStream >= 0) {
							is->audioq.Flush();
							is->audioq.Put(&flush_pkt);
						}
						if (is->m_iSubtitleStream >= 0) {
							is->subtitleq.Flush();
							is->subtitleq.Put(&flush_pkt);
						}
						if (is->m_iVideoStream >= 0) {
							is->videoq.Flush();
							is->videoq.Put(&flush_pkt);
						}
						if (is->m_iSeekFlags & AVSEEK_FLAG_BYTE) {
							is->m_clockExt.Set2(NAN, 0, is->m_fSpeed);
						}
						else {
							is->m_clockExt.Set2(seek_target / (double)AV_TIME_BASE, 0, is->m_fSpeed);
						}
					}
					if (is->m_iAudioStream >= 0)
						is->m_bSendAudioStop = false;
					if (is->m_iVideoStream >= 0)
						is->m_bSendVideoStop = false;
					is->m_bSendMediaStop = false;
					is->m_bSeekReq = 0;
					is->m_bQueueQttachmentsReq = 1;
					is->m_EOF = 0;
					if (is->paused) {
						is->step_to_next_frame();
					}
				}
			}

			if (is->m_bQueueQttachmentsReq) {
				if (is->m_stVideo && is->m_stVideo->disposition & AV_DISPOSITION_ATTACHED_PIC) {
					AVPacket copy;
					if ((ret = av_copy_packet(&copy, &is->m_stVideo->attached_pic)) < 0)
						goto fail;
					is->videoq.Put(&copy);
					is->videoq.PutNullpkt(is->m_iVideoStream);
				}
				is->m_bQueueQttachmentsReq = 0;
			}

			/* if the queue are full, no need to read more */
			if ((is->audioq.size + is->videoq.size + is->subtitleq.size > MAX_QUEUE_SIZE
				|| (is->audioq.stream_has_enough_packets(is->m_stAudio, is->m_iAudioStream) &&
					is->videoq.stream_has_enough_packets(is->m_stVideo, is->m_iVideoStream) &&
					is->subtitleq.stream_has_enough_packets(is->subtitle_st, is->m_iSubtitleStream)))) {
				/* wait 10 ms */
				SDL_Delay(10);
				continue;
			}

			ret = av_read_frame(is->m_ic, pkt);
			if (ret < 0) {
				if ((ret == AVERROR_EOF || avio_feof(is->m_ic->pb)) && !is->m_EOF) {
					if (is->m_iVideoStream >= 0)
						is->videoq.PutNullpkt(is->m_iVideoStream);
					if (is->m_iAudioStream >= 0)
						is->audioq.PutNullpkt(is->m_iAudioStream);
					if (is->m_iSubtitleStream >= 0)
						is->subtitleq.PutNullpkt(is->m_iSubtitleStream);
					is->m_EOF = 1;//文件读取完毕，发送回调
					is->avffmpeg_OnError(FFPLAY_ERROR_OK_GET_EOF);//读取文件
				}
				if (is->m_ic->pb && is->m_ic->pb->error)
					break;
				continue;
			}
			else {
				is->m_EOF = 0;
			}

			int64_t stream_start_time = is->m_ic->streams[pkt->stream_index]->start_time;
			pkt_ts = pkt->pts == AV_NOPTS_VALUE ? pkt->dts : pkt->pts;

			int pkt_in_play_range = is->m_ptsDurationTime == AV_NOPTS_VALUE ||
				(pkt_ts - (stream_start_time != AV_NOPTS_VALUE ? stream_start_time : 0)) *
				av_q2d(is->m_ic->streams[pkt->stream_index]->time_base) -
				(double)(is->m_ptsStartTime != AV_NOPTS_VALUE ? is->m_ptsStartTime : 0) / 1000000
				<= ((double)is->m_ptsDurationTime / 1000000);

			if (pkt->stream_index == is->m_iAudioStream && pkt_in_play_range) {
				temp_pts_second = pkt_ts*av_q2d(is->m_ic->streams[pkt->stream_index]->time_base);
				if (temp_pts_second > is->m_ptsAudioMax) {
					is->m_ptsAudioMax = temp_pts_second;
					is->m_fDuration = FFMAX3(is->m_fDuration, is->m_ptsAudioMax, is->m_ptsVideoMax);
				}
				is->audioq.Put(pkt);
			}
			else if (pkt->stream_index == is->m_iVideoStream && pkt_in_play_range
				&& !(is->m_stVideo->disposition & AV_DISPOSITION_ATTACHED_PIC)) {
				temp_pts_second = pkt_ts*av_q2d(is->m_ic->streams[pkt->stream_index]->time_base);
				if (temp_pts_second > is->m_ptsVideoMax) {
					is->m_ptsVideoMax = temp_pts_second;
					is->m_fDuration = FFMAX3(is->m_fDuration, is->m_ptsAudioMax, is->m_ptsVideoMax);
				}
				is->videoq.Put(pkt);
			}
			else if (pkt->stream_index == is->m_iSubtitleStream && pkt_in_play_range) {
				is->subtitleq.Put(pkt);
			}
			else {
				av_packet_unref(pkt);
			}
		}

	fail:
		is->avffmpeg_OnError(FFPLAY_ERROR_OK_STOP);//退出线程
		return 0;
	}

	bool m_bSetSpeed = false;
	void SetSpeed(int iSpeed) { // 50-200
		WXAutoLock al2(m_mutex);
		WXAutoLock al(m_lockFilter);
		float fspeed = iSpeed / 100.0;

		float fs = av_clipf(fspeed, 0.5, 2.0);

		bool bResume = false;
		if (fabs(fs - m_fSpeed) > 0.1) {
			m_bSetSpeed = true;
			m_iNewSpeed = iSpeed;
			if (m_iState == FFPLAY_STATE_PLAYING) {
				bResume = true;
				Pause();
			}
		}
		else {
			m_bSetSpeed = false;
		}

		if (m_bSetSpeed) {
			/*int64_t timestamp = GetCurrTime() * 1000;
			if (m_ic->start_time != AV_NOPTS_VALUE)timestamp += m_ic->start_time;
			avformat_seek_file(m_ic, -1, INT64_MIN, timestamp, INT64_MAX, 0);*/
			SendChangeFilter();
			if (bResume)Resume();
		}
		//return 1;
	}

	void SendChangeFilter() {
		if (m_ic == nullptr)return;

		WXAutoLock al(m_lockFilter);
		if (m_iVideoStream >= 0 && !(m_stVideo->disposition & AV_DISPOSITION_ATTACHED_PIC)) {
			m_strVF = L"";
			if (m_bSetSpeed) {
				WXString wxstr; wxstr.Format("setpts=%0.2f*PTS", 100.0f / m_iNewSpeed);
				m_strVF.Cat(wxstr, L", ");
			}
			if (m_cropW && m_cropH) {
				WXString wxstr; wxstr.Format("crop=%d:%d:%d:%d", m_cropW, m_cropH, m_cropX, m_cropY);
				m_strVF.Cat(wxstr, L", ");
			}
			if (m_brightness != 0 || m_contrast != 50 || m_saturation != 100) {
				WXString wxstr; wxstr.Format("eq=brightness=%0.2f:contrast=%0.2f:saturation=%0.2f",
					m_brightness / 100.0f, m_contrast / 50.0, m_saturation / 100.0f);
				m_strVF.Cat(wxstr, L", ");
			}

			if (m_vflip) {
				WXString wxstr = L"vflip";//垂直旋转
				m_strVF.Cat(wxstr, L", ");
			}

			if (m_hflip) {
				WXString wxstr = L"hflip";//水平旋转
				m_strVF.Cat(wxstr, L", ");
			}

			HandleSubtitle();

			m_bChangeVF = true;
		}
		else {
			m_bChangeVF = false;
		}

		if (m_iAudioStream >= 0 && m_nSampleRate) {
			m_strAF.Format("asetrate=%0.2f", m_nSampleRate * m_iNewSpeed / 100.0);
			m_bChangeAF = true;
		}
		else {
			m_bChangeAF = false;
		}
	}

	//播放视频帧、同步
	void ThreadDisplay() {
		double remaining_time = 0.0;
		while (!m_bThreadStop) {
			if (remaining_time > 0.0)
				av_usleep((int64_t)(remaining_time * 1000000.0));
			remaining_time = REFRESH_RATE;
			if (!paused || m_bForceRefresh)
				video_refresh(&remaining_time);

			if (m_EOF && m_bSendAudioStop && m_bSendVideoStop) {
                SLEEPMS(20);
			}
		}
	}

public:

	//private
	void  *m_pSoundPlayer = 0;//音频通道序号

	bool m_bSendVideoStop = false;//视频结束标记
	bool m_bSendAudioStop = false;//音频结束标记
	bool m_bSendMediaStop = false;//文件结束标记
	int  m_iState = FFPLAY_STATE_UNAVAILABLE;//

											 //亮度、对比度、饱和度
	int  m_brightness = 0;// 亮度   -100 100   m_brightness/100.0
	int  m_contrast = 50;// 对比度 -100 100   m_contrast/50.0
	int  m_saturation = 100;// 饱和度 0 300  m_saturation / 100

							//图像裁剪
	int  m_cropX = 0;
	int  m_cropY = 0;
	int  m_cropW = 0;
	int  m_cropH = 0;

	int m_iRotate = 0;//旋转角度

	int m_vflip = 0;//垂直翻转
	int m_hflip = 0;//水平翻转

					//字幕文件路径
	WXString m_strSubtitle = L"";
	WXString m_strFileName = L"";

	void SetSubtitle(WXCTSTR wsz) { //设置字幕文件
		WXAutoLock al(m_mutex);
		if (WXStrlen(wsz) > 0) {
#ifdef _WIN32
			//转义处理
			std::wstring temp = L"";//转义字符串替换
			for (int i = 0; i < WXStrlen(wsz); i++) {
				if (wsz[i] == L':')
					temp += L"\\\\:";
				else if (wsz[i] == '\\') {
					temp += L"\\\\\\\\";
				}
				else {
					temp += wsz[i];
				}
			}
			m_strSubtitle.Format(L"%ls", temp.c_str());
#else
			m_strSubtitle.Format(wsz);
#endif
		}
		else {
			m_strSubtitle = L"";
		}
		SendChangeFilter();
	}

	bool SetVolume(int volume) { //音量0-100
		WXAutoLock al(m_mutex);
		m_iVolume = av_clip(volume, 0, 100);
		m_iVolume = av_clip(SDL_MIX_MAXVOLUME * m_iVolume / 100, 0, SDL_MIX_MAXVOLUME);
		return true;
	}

	WXLocker m_lockSeek;//
	bool SetSeek(int64_t pos) { //单位ms
		WXAutoLock al(m_mutex);

		int64_t new_pos = FFMIN(FFMAX(0, pos), m_ptsTotal - FRAME_DURATION_THRESHOLD_MAX * 1000);

		/*if (paused) {
		m_ptsSeekPos = new_pos * 1000;
		m_ptsSeekRel = 0;
		m_iSeekFlags &= ~AVSEEK_FLAG_BYTE;

		WXAutoLock al(&m_lockSeek);
		m_bSeekReq = 1;
		}else */
		{
			/*if (m_bSeeking)
			return true;*/
			/*WXString str;
			str.Format(".................seek");
			OutputDebugString((LPCWSTR)(str.str()));*/
			//暂停状态			
			m_ptsSeekPos = new_pos * 1000;
			m_ptsSeekRel = 0;
			m_iSeekFlags &= ~AVSEEK_FLAG_BYTE;

			WXAutoLock al(m_lockSeek);
			m_bSeekReq = 1;
			m_bSeeking = true;
			if (paused)
				step_to_next_frame();
		}
		return true;
	}

	void Reset() {
		m_iNewSpeed = 100;
		m_cropX = 0;
		m_cropY = 0;
		m_cropW = 0;
		m_cropH = 0;
		m_vflip = 0;
		m_hflip = 0;
		m_iRotate = 0;
		//亮度、对比度、饱和度
		m_brightness = 0;// 亮度   -100 100   m_brightness/100.0
		m_contrast = 50;// 对比度 -100 100   m_contrast/50.0
		m_saturation = 100;// 饱和度 0 300  m_saturation / 100

						   //m_strSubtitle = "";
		m_iVolume = 100;
		SendChangeFilter();
	}

	//设置裁剪区域
	void SetCrop(int x, int y, int w, int h) {
		WXAutoLock al(m_mutex);
		if (m_cropX == x && m_cropY == y && m_cropW == w && m_cropH == h)return;
		m_cropX = x;
		m_cropY = y;
		m_cropW = w;
		m_cropH = h;
		SendChangeFilter();
	}

	//垂直翻转
	void SetVFlip(int b) {
		WXAutoLock al(m_mutex);
		if (b == m_vflip)return;
		m_vflip = b;
		SendChangeFilter();
	}

	//水平翻转
	void SetHFlip(int b) {
		WXAutoLock al(m_mutex);
		if (b == m_hflip)return;
		m_hflip = b;
		SendChangeFilter();
	}

	int  GetVolume() {
		return m_iVolume;
	}

	//旋转角度
	void SetRoate(int rotate) {
		WXAutoLock al(m_mutex);
		if (rotate == m_iRotate)return;
		m_iRotate = (rotate % 360 + 360) % 360;
		SendChangeFilter();
	}

	void   SetPictureQuality(int Brightness, int Contrast, int Saturation) {
		if (Brightness == m_brightness && m_contrast == Contrast  && m_saturation == Saturation)return;
		m_saturation = av_clip(Saturation, 0, 300);
		m_brightness = av_clip(Brightness, -100, 100);
		m_contrast = av_clip(Contrast, -100, 100);
		SendChangeFilter();
	}

	void SetFileName(WXCTSTR  wszFileName) {
		WXAutoLock al(m_mutex);
		m_strFileName.Format(wszFileName);
	}
	void StartTime(int64_t seek) {
		WXAutoLock al(m_mutex);
		m_ptsStartTime = seek * 1000;//ms
	}
	void SetInitSpeed(int speed) {
		m_iNewSpeed = av_clipf(speed, 50, 200);
	}

	int m_bUsingCutSide = FALSE;
	int m_bNetStream = FALSE;
	int m_bCutSide = FALSE;
	int m_nSide = 0;
	//AVFrame *m_pWXDisplay = nullptr;
	WXVideoFrame m_pWXDisplay;

	bool OpenFile() { //	
		av_lockmgr_register(lockmgr);
		av_dict_set(&m_sws_dict, "flags", "bicubic", 0);

		m_iSubtitleStream = -1;
		m_iVideoStream = -1;
		m_iAudioStream = -1;

		/* start video display */
		pictq.Init(&videoq, VIDEO_PICTURE_QUEUE_SIZE, 1);
		subpq.Init(&subtitleq, SUBPICTURE_QUEUE_SIZE, 0);
		sampq.Init(&audioq, SAMPLE_QUEUE_SIZE, 1);
		videoq.Init();
		audioq.Init();
		subtitleq.Init();

		continue_read = SDL_CreateCond();

		m_clockVideo.Init(&videoq.serial);
		m_clockAudio.Init(&audioq.serial);
		m_clockExt.Init(&m_clockExt.serial);
		audio_clock_serial = -1;

		m_iMuted = 0;

		m_ic = avformat_alloc_context();
		m_ic->interrupt_callback.callback = decode_interrupt_cb;
		m_ic->interrupt_callback.opaque = this;
		if (!av_dict_get(m_format_opts, "scan_all_pmts", nullptr, AV_DICT_MATCH_CASE)) {
			av_dict_set(&m_format_opts, "scan_all_pmts", "1", AV_DICT_DONT_OVERWRITE);
			m_scan_all_pmts_set = 1;
		}
		int err = avformat_open_input(&m_ic, m_strFileName.c_str(), nullptr, &m_format_opts);
		if (err < 0) {
			avformat_close_input(&m_ic);
			avformat_free_context(m_ic);
			m_ic = nullptr;
			avffmpeg_OnError(FFMPEG_ERROR_READFILE);
			return 0;
		}

		if (strncmp(m_strFileName.c_str(), "rtmp://", 7) == 0 ||
			strncmp(m_strFileName.c_str(), "rtsp://", 7) == 0 ||

			strncmp(m_strFileName.c_str(), "rtp://", 6) == 0 ||

			strncmp(m_strFileName.c_str(), "udp://", 6) == 0 ||

			strncmp(m_strFileName.c_str(), "http://", 7) == 0 ||
			strncmp(m_strFileName.c_str(), "https://", 8) == 0) {
			m_bNetStream = TRUE;
		}

		m_iVideoStream = -1;
		m_iAudioStream = -1;
		m_iSubtitleStream = -1;
		m_EOF = 0;

		if (m_scan_all_pmts_set)
			av_dict_set(&m_format_opts, "scan_all_pmts", nullptr, AV_DICT_MATCH_CASE);

		AVDictionaryEntry *t = nullptr;
		if ((t = av_dict_get(m_format_opts, "", nullptr, AV_DICT_IGNORE_SUFFIX))) {
			avffmpeg_OnError(FFMPEG_ERROR_READFILE);
			return 0;
		}

		if (m_bGenPts)
			m_ic->flags |= AVFMT_FLAG_GENPTS;

		av_format_inject_global_side_data(m_ic);

		AVDictionary **opts = setup_find_stream_info_opts(m_ic, m_codec_opts);
		int orig_nb_streams = m_ic->nb_streams;

		if (m_bNetStream) {
			m_ic->flags |= AVFMT_FLAG_NOBUFFER;
			//m_ic->probesize = 300 * 1000;
			//m_ic->max_analyze_duration = AV_TIME_BASE/2;
		}
		err = avformat_find_stream_info(m_ic, opts);//堵塞操作,有点慢
		if (err < 0) {
			return 0;
		}

		for (int i = 0; i < orig_nb_streams; i++)
			av_dict_free(&opts[i]);
		av_freep(&opts);


		if (m_ic->pb)
			m_ic->pb->eof_reached = 0;

		m_fMaxFrameDuration = (m_ic->iformat->flags & AVFMT_TS_DISCONT) ? 10.0 : 3600.0;

		/* if seeking requested, we execute it */
		if (m_ptsStartTime != AV_NOPTS_VALUE) {
			int64_t timestamp = m_ptsStartTime;
			if (m_ic->start_time != AV_NOPTS_VALUE)timestamp += m_ic->start_time;
			avformat_seek_file(m_ic, -1, INT64_MIN, timestamp, INT64_MAX, 0);
		}

		for (int i = 0; i < m_ic->nb_streams; i++) {
			AVStream *st = m_ic->streams[i];
			enum AVMediaType type = st->codecpar->codec_type;
			st->discard = AVDISCARD_ALL;
		}

		m_iStIndex[AVMEDIA_TYPE_VIDEO] = av_find_best_stream(m_ic, AVMEDIA_TYPE_VIDEO, m_iStIndex[AVMEDIA_TYPE_VIDEO], -1, nullptr, 0);

		m_iStIndex[AVMEDIA_TYPE_AUDIO] = av_find_best_stream(m_ic, AVMEDIA_TYPE_AUDIO, m_iStIndex[AVMEDIA_TYPE_AUDIO], m_iStIndex[AVMEDIA_TYPE_VIDEO], nullptr, 0);

		m_iStIndex[AVMEDIA_TYPE_SUBTITLE] = av_find_best_stream(m_ic, AVMEDIA_TYPE_SUBTITLE, m_iStIndex[AVMEDIA_TYPE_SUBTITLE],
			(m_iStIndex[AVMEDIA_TYPE_AUDIO] >= 0 ? m_iStIndex[AVMEDIA_TYPE_AUDIO] : m_iStIndex[AVMEDIA_TYPE_VIDEO]), nullptr, 0);

		m_bSendAudioStop = true;
		m_bSendVideoStop = true;
		m_bSendMediaStop = false;
		if (m_iStIndex[AVMEDIA_TYPE_AUDIO] >= 0) {
			int ret1 = OpenStream(m_iStIndex[AVMEDIA_TYPE_AUDIO]);
			if (ret1 != 0) {
				m_iAudioStream = -1;
			}
			else {
				m_bSendAudioStop = false;
			}
		}

		if (m_iStIndex[AVMEDIA_TYPE_VIDEO] >= 0) {
			int ret1 = OpenStream(m_iStIndex[AVMEDIA_TYPE_VIDEO]);
			if (ret1 != 0) {
				m_iVideoStream = -1;
			}
			else {
				m_bSendVideoStop = false;
			}
		}

		if (m_iStIndex[AVMEDIA_TYPE_SUBTITLE] >= 0) {
			int ret1 = OpenStream(m_iStIndex[AVMEDIA_TYPE_SUBTITLE]);
			if (ret1 != 0) {
				m_iSubtitleStream = -1;
			}
		}

		if (m_iVideoStream < 0 && m_iAudioStream < 0) {
			avffmpeg_OnError(FFMPEG_ERROR_READFILE);
			return 0;
		}

		if (m_nSampleRate) {
			SetSpeed(int(m_fSpeed * 100 + 0.5));//设置AVFilter
		}

		m_ThreadReadID = SDL_CreateThread(ThreadRead, "ThreadRead", this);
		Pause();
		m_iState = FFPLAY_STATE_WAITING;//等待Start操作
		m_ptsTotal = m_ic->duration / 1000;
		m_fDuration = m_ic->duration / 1000000.0;
		return true;
	}

	bool Start() {
		WXAutoLock al(m_mutex);
		if (m_ic && m_thread == nullptr) {
			m_iState = FFPLAY_STATE_PLAYING;//正在播放
			m_bThreadStop = false;
			m_thread = new std::thread(&WXFfplay::ThreadDisplay, this);
			m_EOF = 0;

			m_bSendAudioStop = true;
			m_bSendAudioStop = true;

			if (m_iAudioStream >= 0)
				m_bSendAudioStop = false;

			if (m_iVideoStream >= 0)
				m_bSendVideoStop = false;
			m_bSendMediaStop = false;

			if (m_iNewSpeed != 100)
				SetSpeed(m_iNewSpeed);

			Resume();
			return true;
		}
		return false;
	}

	bool Stop() {
		WXAutoLock al(m_mutex);
		if (m_ic && m_thread != nullptr) {
			Pause();
			m_bThreadStop = true;
			m_thread->join();
			delete m_thread;
			m_thread = nullptr;
			g_nStreamWidth = 0;
			g_nStreamHeight = 0;
			m_iState = FFPLAY_STATE_WAITING;//正在播放
			return true;
		}
		return false;
	}

	void Destroy() {
		WXAutoLock al(m_mutex);
		if (m_ic && m_thread != nullptr) {
			m_bThreadStop = true;
			m_thread->join();
			delete m_thread;
			m_thread = nullptr;
		}

		if (m_ic) {

			m_iState = FFPLAY_STATE_UNAVAILABLE;//正在播放

			m_EOF = 1;
			m_bSendVideoStop = true;
			m_bSendAudioStop = true;
			m_bSendMediaStop = true;

			m_bAbortRequest = 1;
			SDL_WaitThread(m_ThreadReadID, nullptr); //主线程退出
			m_ThreadReadID = nullptr;

			/* close each stream */
			if (m_iAudioStream >= 0) {
				CloseStream(m_iAudioStream);
				m_iAudioStream = -1;
			}

			if (m_iVideoStream >= 0) {
				CloseStream(m_iVideoStream);
				m_iVideoStream = -1;
			}


			if (m_iSubtitleStream >= 0) {
				CloseStream(m_iSubtitleStream);
				m_iSubtitleStream = -1;
			}

			videoq.Destroy();
			audioq.Destroy();
			subtitleq.Destroy();

			/* free all pictures */
			pictq.Destroy();
			sampq.Destroy();
			subpq.Destroy();

			if (continue_read) {
				SDL_DestroyCond(continue_read);
				continue_read = nullptr;
			}
			avformat_close_input(&m_ic);
			avformat_free_context(m_ic);
			m_ic = nullptr;
			if (m_pRender) {
#ifdef _WIN32
				if (m_pRender)
					m_pRender->ChangeWindowProc(FALSE);
#endif
                WXVideoRenderDestroy(m_pRender);
				m_pRender = nullptr;
			}

			if (m_swr_opts)
				av_dict_free(&m_swr_opts);

			if (m_sws_dict)
				av_dict_free(&m_sws_dict);

			if (m_format_opts)
				av_dict_free(&m_format_opts);

			if (m_codec_opts)
				av_dict_free(&m_codec_opts);

			if (m_resample_opts)
				av_dict_free(&m_resample_opts);

			av_lockmgr_register(nullptr);

			if (m_pSoundPlayer) {
				WXSoundPlayerDestroy(m_pSoundPlayer);
				m_pSoundPlayer = nullptr;
			}
		}
	}

	//静音
	void SetMute(int b) {
		WXAutoLock al(m_mutex);
		m_iMuted = !!b;
	}

	//暂停
	bool Pause() {
		WXAutoLock al(m_mutex);
		if (!paused) {
			toggle_pause();
			paused = 1;
		}
		step = 1;
		WXSoundPlayerPause(m_pSoundPlayer, 1);
		m_iState = FFPLAY_STATE_PAUSE;//正在播放
		return true;
	}

	void    Refresh() {
		WXAutoLock al(m_mutex);
		{
			if (paused) {
				int Volume = m_iVolume;
				SetVolume(0);//静音
				Resume();
                SLEEPMS(150);
				Pause();
				SetVolume(Volume);//恢复
			}
		}
	}

	//恢复
	bool Resume() {
		WXAutoLock al(m_mutex);
		if (paused) {
			toggle_pause();
			paused = 0;
		}
		step = 0;
		WXSoundPlayerPause(m_pSoundPlayer, 0);
		m_iState = FFPLAY_STATE_PLAYING;//正在播放
		return true;
	}

	//亮度
	void SetBrightness(int brightness) {
		WXAutoLock al(m_mutex);
		if (brightness == m_brightness)return;
		m_brightness = av_clip(brightness, -100, 100);
		SendChangeFilter();
	}

	//对比度
	void SetContrast(int contrast) {
		WXAutoLock al(m_mutex);
		if (m_contrast == contrast)return;
		m_contrast = av_clip(contrast, -100, 100);
		SendChangeFilter();
	}

	//饱和度
	void SetSaturation(int saturation) {
		WXAutoLock al(m_mutex);
		if (m_saturation == saturation)return;
		m_saturation = av_clip(saturation, 0, 300);
		SendChangeFilter();
	}
	//Log

	WXString m_strLog = L"";
	HWND m_hwnd = nullptr;//显示窗口

	WXLocker  m_lockSpeed;//filter锁，动态修改filter需要
	WXLocker  m_lockFilter;//filter锁，动态修改filter需要
	int64_t m_avTotolPts = 0;
	void *m_pRender = nullptr;

	WXFfmpegOnVideoData m_cbVideo = nullptr;
	WXFfmpegOnEvent m_cbEvent = nullptr;


	WXString m_strIDEvent = L"NULL";

	void*  m_ownerEvent = nullptr;//回调对象

	WXDataBuffer m_dataBuffer;
	std::thread *m_thread = nullptr;//视频渲染线程
	bool m_bThreadStop = true;

	//给上层的回调消息
	void avffmpeg_OnError(int code) {
		if (m_cbEvent) {
			m_cbEvent(m_ownerEvent, m_strIDEvent.str(), code, WXFfmpegGetError(code));
		}
	}

	bool SetView(HWND hwnd) {
		WXAutoLock al(m_mutex);
		m_hwnd = hwnd;
		return true;
	}

	void SetVideoCB(WXFfmpegOnVideoData cb) {
		WXAutoLock al(m_mutex);
		m_cbVideo = cb;
	}

	int GetState() { //当前状态
		if (m_EOF && m_bSendAudioStop && m_bSendVideoStop) {
			return FFPLAY_STATE_PLAYING_END;//文件结束标记
		}
		return m_iState;
	}

	int64_t m_ptsTotal = 0;//媒体总时长，毫秒
	double m_fMasterClock = 0.0;//主时钟
	double m_fDuration = 0.0;//媒体总时长，秒
							 //double m_dDurationVideo = 0.0;//视频总时长，秒
							 //double m_dDurationAudio = 0.0;//音频总时长，秒
	double m_ptsVideoMax = 0.0;//视频流最大pts，秒
	double m_ptsAudioMax = 0.0;//音频流最大pts，秒
							   /*double m_ptsAudio = 0;
							   double m_ptsVideo = 0;*/
	double m_ptsMedia = 0.0;

	int64_t GetTotalTime() { //总时间长度，毫秒
		return m_ptsTotal;
	}
	int64_t GetCurrTime() { //单位ms, 标准时间戳
							//int64_t pts = get_master_clock() * 1000;
		int64_t pts = 0;

		if (m_bSeeking)//|| isnan(get_master_clock())
			pts = m_ptsSeekPos / 1000;
		else
			pts = m_ptsMedia * 1000;
		//pts = FFMAX(m_ptsAudio, m_ptsVideo);

		/*WXString str;
		str.Format("master=%02f pts=%02f  %d\n\r", get_master_clock(),pts/1000.0f, m_bSeeking ? 1 : 0);
		OutputDebugString((LPCWSTR)(str.str()));*/
		return FFMIN(pts, m_ptsTotal);
	}

	double   GetSpeed() {
		return m_fSpeed;
	}
};



WXDELOGO_CAPI void WXFfplayDestroy(void* ptr) {
    WXFfplay *play = (WXFfplay *)ptr;
    if (play) {
        delete play;
        play = nullptr;
    }
}

WXDELOGO_CAPI void*    WXFfplayCreate(WXCTSTR wszType, WXCTSTR wszInput, int speed, int64_t seek) {
	WXFfplay *play = new WXFfplay;
	play->SetFileName(wszInput);
	play->SetInitSpeed(speed);
	play->StartTime(seek);
	int bRet = play->OpenFile();
	if (!bRet) {
		WXFfplayDestroy(play);
		return nullptr;
	}
	return (void*)play;
}

WXDELOGO_CAPI void     WXFfplaySetView(void* p, HWND hwnd) {
	WXFfplay *play = (WXFfplay *)p;
	if (play)play->SetView(hwnd);
}


WXDELOGO_CAPI void     WXFfplaySetVideoCB(void* p, WXFfmpegOnVideoData cb) {
	WXFfplay *play = (WXFfplay *)p;
	if (play) play->SetVideoCB(cb);
}

WXDELOGO_CAPI void     WXFfplaySetVolume(void* p, int volume) {
	WXFfplay *play = (WXFfplay *)p;
	if (play)play->SetVolume(volume);
}

WXDELOGO_CAPI int  WXFfplayStart(void* p) {
	WXFfplay *play = (WXFfplay *)p;
	if (play)return play->Start();
	return 0;
}

WXDELOGO_CAPI void     WXFfplayStop(void* p) {
	WXFfplay *play = (WXFfplay *)p;
	if (play) play->Stop();
}

WXDELOGO_CAPI void     WXFfplayPause(void* p) {
	WXFfplay *play = (WXFfplay *)p;
	if (play)play->Pause();
}

WXDELOGO_CAPI void     WXFfplayResume(void* p) {
	WXFfplay *play = (WXFfplay *)p;
	if (play)play->Resume();
}

WXDELOGO_CAPI int64_t  WXFfplayGetCurrTime(void* p) {
	WXFfplay *play = (WXFfplay *)p;
	if (play)return play->GetCurrTime();
	return 0;
}

WXDELOGO_CAPI int64_t  WXFfplayGetTotalTime(void* p) {
	WXFfplay *play = (WXFfplay *)p;
	if (play)return play->GetTotalTime();
	return 0;
}

WXDELOGO_CAPI int  WXFfplayGetVolume(void* p) {
	WXFfplay *play = (WXFfplay *)p;
	if (play)return play->GetVolume();
	return 0;
}

WXDELOGO_CAPI void  WXFfplaySeek(void* p, int64_t pts) {
	WXFfplay *play = (WXFfplay *)p;
	if (play)play->SetSeek(pts);
}

WXDELOGO_CAPI void  WXFfplaySpeed(void* p, int speed) {
	WXFfplay *play = (WXFfplay *)p;
	if (play)play->SetSpeed(speed);
}


WXDELOGO_CAPI int  WXFfplayGetState(void* p) {
	WXFfplay *play = (WXFfplay *)p;
	if (play)return play->GetState();
	return 0;
}




