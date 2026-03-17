/*
ffmpeg 视频转换功能封装
首先要 执行初始化函数 WXMediaFfmpeg_Init()
如果对ffmpeg命令行属性，可以直接调用
WXFFMPEG_CAPI int64_t WXMediaFfmpeg_Convert(int argc, char **argv);
相当于 ffmpeg(argc,argv);
*/

#include "FfmpegIncludes.h"

#include <WXBase.h>

class WXConvert :public WXLocker {
	enum { MAX_FFMPEG_ARGC = 50 };
	int  m_argc = 0;
	char *m_argv[MAX_FFMPEG_ARGC];
	WXCtx *m_ctx = NULL;


	//Convert
	int64_t   m_ptsStart = 0;  //-ss
	int64_t   m_ptsDuration = 0;  //-t

	int  m_iVodeoCodecMode = 1;// 0 Faset 1 Normal 2 Best
	int  m_iVolume = 256;//音量  0-1000 //默认音量256
	int  m_iRotate = 0;//旋转角度

	std::vector<WXString> m_arrInput;//inputs

	WXString  m_strVideoCodec = _T("noset");// -vcodec libx264
	WXString  m_strAudioCodec = _T("noset"); // -acodec aac

	double    m_fFps = 0.0;//
	int       m_iVideoBitrate = 0;  //-b:v
	int       m_iAudioBitrate = 0; // -b:a
	int       m_iAudioSampleRate = 0; // -ar 8000
	int       m_iAudioChannel = 0;  // -ac 1

	int       m_iSpeed = 100;//速度 0.5--2.0 //默认速度 1.0   100

	//亮度，对比度，饱和度
	int m_iBrightness = 0;
	int m_iContrast = 50;
	int m_iSaturation = 100;

	int m_iSrcWidth = 0;
	int m_iSrcHeight = 0;

	int m_iWidth = 0;
	int m_iHeight = 0;

	int m_iDARWidth = 0;
	int m_iDARHeight = 0;

	int m_iCropX = 0;
	int m_iCropY = 0;
	int m_iCropW = 0;
	int m_iCropH = 0;
	int m_bVFilp = 0;
	int m_bHFilp = 0;

	WXString m_strSubtitle = _T("");
	WXString  m_strSubtitleFontName = _T("");
	int       m_iSubtitleFontSize = 20;
	int       m_iSubtitleFontColor = 0xFFFFFF;
	int       m_iSubtitleFontAlpha = 0;
	int       m_iSubtitlePostion = 0;//0--270
    int       m_iAlignment = 2;//2 Bottom 10 center 6 Top

	struct WM_Data {
		WXString m_strImage = _T(""); //WaterMark Image
		int m_iWMPosX = 0;
		int m_iWMPosY = 0;
		int m_iWMWidth = 0;
		int m_iWMHeight = 0;
	};
	std::vector<WM_Data>m_arrWatermark;

	//水印filter
	void HandleWaterMark() {
		if (m_arrWatermark.size() == 0)return;

		WXString strScale = _T("");
		WXString strWM = _T("");
		int size = (int)m_arrWatermark.size();

		for (int i = 0; i < size; i++) {
			{
				WXString wxstr;
				wxstr.Format("[%d:v]scale=%d:%d[img%d]",
					i + 1, m_arrWatermark[i].m_iWMWidth, m_arrWatermark[i].m_iWMHeight, i + 1);
				strScale.Cat(wxstr, _T(";"));
			}

			{
				WXString wxstr;
				if (i == 0 && size == 1) {
					wxstr.Format("[0:v][img1]overlay=%d:%d",
						m_arrWatermark[i].m_iWMPosX, m_arrWatermark[i].m_iWMPosY); //只有一个水印！！
				}
				else if (i == 0 && size != 1) {
					wxstr.Format("[0:v][img%d]overlay=%d:%d[bkg%d]",
						i + 1, m_arrWatermark[i].m_iWMPosX, m_arrWatermark[i].m_iWMPosY, i + 1); //多个水印中的第一个
				}
				else if (i != 0 && i != size - 1) {
					wxstr.Format("[bkg%d][img%d]overlay=%d:%d[bkg%d]",
						i, i + 1, m_arrWatermark[i].m_iWMPosX, m_arrWatermark[i].m_iWMPosY, i + 1); //多个水印，但不是最后一个水印
				}
				else if (i != 0 && i == size - 1) {
					wxstr.Format("[bkg%d][img%d]overlay=%d:%d",
						i, i + 1, m_arrWatermark[i].m_iWMPosX, m_arrWatermark[i].m_iWMPosY); //多个水印中最后一个水印
				}
				strWM.Cat(wxstr, _T(";"));
			}
		}
		strScale.Cat(strWM, _T(";"));
		m_strVideoFilter.Cat(strScale, _T(";"));
	}

	//字幕filter
	void HandleSubtitle() {
		if (m_strSubtitle.length() != 0) {
			WXString wxstr;
			wxstr.Format("subtitles=%s", m_strSubtitle.c_str());
			WXString wxstrForce_Style;
			uint32_t color = (m_iSubtitleFontAlpha << 24) | m_iSubtitleFontColor;
			if (m_strSubtitleFontName.length() > 0) {
				wxstrForce_Style.Format(":force_style=\'FontName=%s,FontSize=%d,PrimaryColour=&H%08x&,MarginV=%d,Alignment=%d\'",
					m_strSubtitleFontName.c_str(), m_iSubtitleFontSize, color, m_iSubtitlePostion,m_iAlignment);
			}else if (m_strSubtitleFontName.length() == 0) {
				wxstrForce_Style.Format(":force_style=\'FontSize=%d,PrimaryColour=&H%08x&,MarginV=%d,Alignment=%d\'",
					m_iSubtitleFontSize, color, m_iSubtitlePostion,m_iAlignment);
			}
			wxstr += wxstrForce_Style;
			m_strVideoFilter.Cat(wxstr, _T(", "));
		}
	}

	void HandleFilters() {
		HandleWaterMark();//水印处理

		if (m_iCropW > 0 && m_iCropH > 0) {
			WXString wxstr;
			wxstr.Format("crop=%d:%d:%d:%d", m_iCropW, m_iCropH, m_iCropX, m_iCropY);
			m_strVideoFilter.Cat(wxstr, _T(","));
			m_iWidth = m_iCropW;
			m_iHeight = m_iCropH;
			if (m_iRotate == 90 || m_iRotate == 270) {
				m_iWidth = m_iCropH;
				m_iHeight = m_iCropW;
			}
		}

		if (m_bVFilp) {
			WXString wxstr = _T("vflip");//垂直旋转
			m_strVideoFilter.Cat(wxstr, _T(","));
		}

		if (m_bHFilp) {
			WXString wxstr = _T("hflip");//垂直旋转
			m_strVideoFilter.Cat(wxstr, _T(","));
		}

		if (m_iRotate != 0) {
			if (m_iRotate == 90) {
				WXString wxstr = _T("transpose=cclock");
				m_strVideoFilter.Cat(wxstr, _T(","));
			}
			else if (m_iRotate == 270) {
				WXString wxstr = _T("transpose=cclock");
				m_strVideoFilter.Cat(wxstr, _T(","));
			}
			else {
				WXString wxstr;
				wxstr.Format("rotate=%d*PI/180", m_iRotate);
				m_strVideoFilter.Cat(wxstr, _T(","));
			}
		}

		if (m_iSpeed != 100) { //播放速度
			m_strAudioFilter.Format("atempo=%0.3f", m_iSpeed / 100.0);
			WXString wxstr;
			wxstr.Format("setpts=%0.3f*PTS", 100.0 / m_iSpeed);
			m_strVideoFilter.Cat(wxstr, _T(","));
		}

		if (m_iBrightness != 0 || m_iContrast != 50 || m_iSaturation != 100) { //亮度、对比度、饱和度
			WXString wxstr;
			wxstr.Format("eq=brightness=%0.2f:contrast=%0.2f:saturation=%0.2f",
				m_iBrightness / 100.0f, m_iContrast / 50.0, m_iSaturation / 100.0f);
			m_strVideoFilter.Cat(wxstr, _T(","));
		}

		HandleSubtitle();
	}

public:

	WXConvert() {
		WXAutoLock al(this);
		m_ctx = avffmpeg_create();
		for (int i = 0; i < MAX_FFMPEG_ARGC; i++) {
			m_argv[i] = NULL;
		}
	}

	~WXConvert() {
		WXAutoLock al(this);
		if (m_ctx) {
			if (m_thread) {
				m_thread->join();
				delete m_thread;
				m_thread = NULL;
			}
			avffmpeg_destroy(m_ctx);
			m_ctx = NULL;
			for (int i = 0; i < MAX_FFMPEG_ARGC; i++) {
				if (m_argv[i]) {
					av_free(m_argv[i]);
				}
				m_argv[i] = NULL;
			}
		}
	}

	int LogRet(int ret) {
        WXLogWriteNew("Convert Param");
        for (int i = 0; i < m_argc; i++) {
             WXLogWriteNew("argv[%d] = %s", i, m_argv[i]);
        }
        WXString wxstr = WXFfmpegGetError(ret);
        WXLogWriteNew("MergerAV Result = %s", wxstr.c_str());
        
		return ret;
	}

	int gcd(int m, int n){
		int r;
		while (n != 0)
		{
			r = (m >= n) ? (m - n) : m;
			m = n;
			n = r;
		}
		return m;
	}

public:
	std::thread *m_thread = nullptr;
	int Function(int async) {
		if (async) {
			m_thread = new std::thread(&WXConvert::ThreadFunction, this);//异步执行
		}else {
			return ThreadFunction();
		}
		return 0;
	}
	int ThreadFunction() {
		int ret = avffmpeg_convert(m_ctx, m_argc, m_argv);
		LogRet(ret);
		return ret;
	}
public:
	//H264+AAC--->MP4
	int MixerAV(WXCTSTR strVideo, WXCTSTR strAudio, WXCTSTR strMixer, int async) {  
		WXAutoLock al(this);
		WXString wxstrVideo = strVideo;
		WXString wxstrAudio = strAudio;
		WXString wxstrOut = strMixer;
		m_argc = 0;
		m_argv[m_argc++] = av_strdup("ffmpeg");
		m_argv[m_argc++] = av_strdup("-i");
		m_argv[m_argc++] = av_strdup(wxstrVideo.c_str());
		m_argv[m_argc++] = av_strdup("-i");
		m_argv[m_argc++] = av_strdup(wxstrAudio.c_str());
		m_argv[m_argc++] = av_strdup("-c");
		m_argv[m_argc++] = av_strdup("copy");
		m_argv[m_argc++] = av_strdup(wxstrOut.c_str());
		return Function(async);
	}

	int ReplaceAudio(WXCTSTR strVideo, WXCTSTR strAudio, WXCTSTR strMixer, int copy, int async) {
		WXAutoLock al(this);
		WXString wxstrVideo = strVideo;
		WXString wxstrAudio = strAudio;
		WXString wxstrOut = strMixer;

		m_argc = 0;
		m_argv[m_argc++] = av_strdup("ffmpeg");
		m_argv[m_argc++] = av_strdup("-i");
		m_argv[m_argc++] = av_strdup(wxstrVideo.c_str());
		m_argv[m_argc++] = av_strdup("-i");
		m_argv[m_argc++] = av_strdup(wxstrAudio.c_str());
		m_argv[m_argc++] = av_strdup("-map");
		m_argv[m_argc++] = av_strdup("0:v");
		m_argv[m_argc++] = av_strdup("-map");
		m_argv[m_argc++] = av_strdup("1:a");


		if (copy) {
			m_argv[m_argc++] = av_strdup("-c");
			m_argv[m_argc++] = av_strdup("copy");
		}
		m_argv[m_argc++] = av_strdup(wxstrOut.c_str());
		return Function(async);
	}
	
	//文件裁剪
	int CutFile(WXCTSTR strInput, WXCTSTR strOutput,
		int64_t ptsStart, int64_t ptsDuration, int Fast, int async) {

		WXAutoLock al(this);
		WXString wxstrIn = strInput;
		WXString wxstrOut = strOutput;
		m_argc = 0;
		m_argv[m_argc++] = av_strdup("ffmpeg");
		m_argv[m_argc++] = av_strdup("-i");
		m_argv[m_argc++] = av_strdup(wxstrIn.c_str());

		if (ptsStart != 0 || ptsDuration != 0) {
			m_argv[m_argc++] = av_strdup("-ss");
			m_argv[m_argc++] = av_asprintf("%0.2f", ptsStart / 1000.0f);
			m_argv[m_argc++] = av_strdup("-t");
			m_argv[m_argc++] = av_asprintf("%0.2f", ptsDuration / 1000.0f);
		}

		if (Fast) {
			m_argv[m_argc++] = av_strdup("-c");
			m_argv[m_argc++] = av_strdup("copy");
		}
		m_argv[m_argc++] = av_strdup(wxstrOut.c_str());
		return Function(async);
	}

	//视频格式转换
	int ConvertVideo(WXCTSTR strInput, WXCTSTR strOutput, int async) {
		WXAutoLock al(this);
		HandleFilters();

		WXString wsxtrInput = strInput;
		WXString wxstrOutput = strOutput;

		//后缀名判断
		WXCTSTR Last3 = wxstrOutput.Left(3);
		WXCTSTR Last4 = wxstrOutput.Left(4);

		if (WXStrcmp(Last4, _T(".3gp")) == 0) {//3gp 视频格式	H263+AMR_NB(8000 MOMO)
			bool bFindSize = false;
			if ((m_iWidth == 1408 && m_iHeight == 1152) ||
				(m_iWidth == 704 && m_iHeight == 576) ||
				(m_iWidth == 352 && m_iHeight == 288) ||
				(m_iWidth == 176 && m_iHeight == 144) ||
				(m_iWidth == 120 && m_iHeight == 96)) {
				bFindSize = true;//配置正确的分辨率
			}
			if (!bFindSize) {
				m_iWidth = 704;
				m_iHeight = 576;
			}
			m_strVideoCodec = _T("h263");
			m_strAudioCodec = _T("amr_nb");
			m_iAudioSampleRate = 8000;
			m_iAudioChannel = 1;
			m_iVideoBitrate = 0;
			m_iAudioBitrate = 0;
		}
		m_argc = 0;
		m_argv[m_argc++] = av_strdup("ffmpeg");


		if (m_ptsStart > 0 || m_ptsDuration > 0) { //-ss tsStart -t tsDuration
			m_argv[m_argc++] = av_strdup("-ss");
			m_argv[m_argc++] = av_asprintf("%0.2f", m_ptsStart / 1000.0f);
			m_argv[m_argc++] = av_strdup("-t");
			m_argv[m_argc++] = av_asprintf("%0.2f", m_ptsDuration / 1000.0f);
		}
		  
		//输入文件
		m_argv[m_argc++] = av_strdup("-i");
		m_argv[m_argc++] = av_strdup(wsxtrInput.c_str());

		//水印图片
		if (m_arrWatermark.size() > 0) { 
			for (int i = 0; i < m_arrWatermark.size(); i++) {
				if (m_arrWatermark[i].m_strImage.length() > 0) { //水印图片
					m_argv[m_argc++] = av_strdup("-i");
					m_argv[m_argc++] = av_strdup(m_arrWatermark[i].m_strImage.c_str());
				}
			}
		}

        if (WXStrcmp(Last3, _T(".dv")) == 0) { //dvvideo DV视频格式
			m_argv[m_argc++] = av_strdup("-target");
			m_argv[m_argc++] = (m_strVideoCodec == _T("ntscdv")) ? av_strdup("ntsc-dv") : av_strdup("pal-dv");
			m_argv[m_argc++] = av_strdup("-aspect");
			m_argv[m_argc++] = av_strdup("4:3");
			m_argv[m_argc++] = av_strdup(wxstrOutput.c_str());
		}else if (WXStrcmp(Last4, _T(".vob")) == 0) { //vob 视频格式
			m_argv[m_argc++] = av_strdup("-target");
			m_argv[m_argc++] = av_strdup("pal-dv");
			m_argv[m_argc++] = av_strdup(wxstrOutput.c_str());
			if (m_iWidth == 720 && m_iHeight == 576) {
				m_argv[m_argc++] = av_strdup("pal-dvd");
			}else  if (m_iWidth == 720 && m_iHeight == 480) {
				m_argv[m_argc++] = av_strdup("ntsc-dvd");
			}else if (m_iWidth == 480 && m_iHeight == 576) {
				m_argv[m_argc++] = av_strdup("pal-svcd");
			}else  if (m_iWidth == 480 && m_iHeight == 480) {
				m_argv[m_argc++] = av_strdup("ntsc-svcd");
			}else if (m_iWidth == 352 && m_iHeight == 288) {
				m_argv[m_argc++] = av_strdup("pal-vcd");
			}else  if (m_iWidth == 352 && m_iHeight == 240) {
				m_argv[m_argc++] = av_strdup("ntsc-vcd");
			}
		}else {
			if (m_iWidth != 0 && m_iHeight != 0) { //-s 352x288  分辨率设置

				//设置DAR
				{
					WXString wxstr;
					wxstr.Format("setsar=sar=1/1");//设置输出SAR为1：1
					m_strVideoFilter.Cat(wxstr, _T(","));

					m_argv[m_argc++] = av_strdup("-aspect");
					int GCD = gcd(m_iWidth, m_iHeight);
					m_argv[m_argc++] = av_asprintf("%d:%d", m_iWidth / GCD, m_iHeight / GCD);

					if (m_iDARWidth && m_iDARHeight) { //设置DAR输出，黑边模式！！！
						int deltaW = 0;
						int deltaH = 0;
						if (m_iDARWidth * m_iHeight < m_iDARHeight * m_iWidth) {
							deltaW = (m_iWidth - m_iDARWidth * m_iHeight / m_iDARHeight) / 2;
						}else {
							deltaH =(m_iHeight - m_iDARHeight * m_iWidth / m_iDARWidth) / 2;
						}
						WXString wxstr2;
						wxstr2.Format("scale=%d:%d,pad=%d:%d:%d:%d", m_iWidth - 2 * deltaW, m_iHeight - 2 * deltaH,
							m_iWidth, m_iHeight, deltaW, deltaH);//设置输出SAR为1：1
						m_strVideoFilter.Cat(wxstr2, _T(","));
					}
				}

				m_argv[m_argc++] = av_strdup("-s");
				m_argv[m_argc++] = av_asprintf("%dx%d",m_iWidth,m_iHeight);
			}

			if (m_strVideoCodec != _T("noset")) { //-vcodec libx264
				m_argv[m_argc++] = av_strdup("-vcodec");
				m_argv[m_argc++] = av_strdup(m_strVideoCodec.c_str());
			}

			if (fabs(m_fFps - 0.0) > 1) { //-r 25
				m_argv[m_argc++] = av_strdup("-r");
				m_argv[m_argc++] = av_asprintf("%02f", m_fFps);
			}

			if (m_iVideoBitrate !=  0) { //b:v
				m_argv[m_argc++] = av_strdup("-b:v");
				m_argv[m_argc++] = av_asprintf("%d", m_iVideoBitrate);
			}


			if (m_strAudioCodec != _T("noset")) { //-acodec aac
				m_argv[m_argc++] = av_strdup("-acodec");
				m_argv[m_argc++] = av_strdup(m_strAudioCodec.c_str());
			}

			if (m_iAudioSampleRate != 0) { //-ar 44100
				m_argv[m_argc++] = av_strdup("-ar");
				m_argv[m_argc++] = av_asprintf("%d", m_iAudioSampleRate);
			}

			if (m_iAudioChannel != 0) { //-ac 2
				m_argv[m_argc++] = av_strdup("-ac");
				m_argv[m_argc++] = av_asprintf("%d", m_iAudioChannel);
			}

			if (m_iAudioBitrate != 0) { //-ab 128k
				m_argv[m_argc++] = av_strdup("-b:a");
				m_argv[m_argc++] = av_asprintf("%d", m_iAudioBitrate);
			}

			if (m_iVolume != 256) { //-vol 1000 默认音量256  或者用 -af volume=10db ?
				m_argv[m_argc++] = av_strdup("-vol");
				m_argv[m_argc++] = av_asprintf("%d", m_iVolume);
			}

			if (m_strVideoFilter.length() != 0) {
				m_argv[m_argc++] = av_strdup("-filter_complex");
				m_argv[m_argc++] = av_strdup(m_strVideoFilter.c_str());
			}

			if (m_strAudioFilter.length() > 0) { // "-af"
				m_argv[m_argc++] = av_strdup("-af");
				m_argv[m_argc++] = av_strdup(m_strAudioFilter.c_str());
			}
			m_argv[m_argc++] = av_strdup(wxstrOutput.c_str());
		}
        
		return Function(async);
	}

	//音频格式转换
	int ConvertAudio(WXCTSTR strInput, WXCTSTR strOutput, int async) {
		WXAutoLock al(this);
		WXString wxstrInput  = strInput;
		WXString wxstrOutput = strOutput;
		//后缀名判断
		WXCTSTR Last3 = wxstrOutput.Left(3);
		WXCTSTR Last4 = wxstrOutput.Left(4);

		if (WXStrcmp(Last4, _T(".3gp")) == 0) {//3gp 视频格式			
			m_iAudioBitrate = 0;
			m_strAudioCodec = _T("amr_nb");
			m_iAudioSampleRate = 8000;
			m_iAudioChannel = 1;
		}

		if (m_iSpeed != 100) { //播放速度
			m_strAudioFilter.Format("atempo=%0.3f", m_iSpeed / 100.0);
		}

		m_argc = 0;
		m_argv[m_argc++] = av_strdup("ffmpeg");

		if (m_ptsStart > 0 || m_ptsDuration > 0) { //-ss tsStart -t tsDuration
			m_argv[m_argc++] = av_strdup("-ss");
			m_argv[m_argc++] = av_asprintf("%0.2f", m_ptsStart / 1000.0f);
			m_argv[m_argc++] = av_strdup("-t");
			m_argv[m_argc++] = av_asprintf("%0.2f", m_ptsDuration / 1000.0f);
		}

	    //输入文件
		m_argv[m_argc++] = av_strdup("-i");
		m_argv[m_argc++] = av_strdup(wxstrInput.c_str());
		m_argv[m_argc++] = av_strdup("-vn");
		
		if (WXStrcmp(Last4, _T(".dts")) == 0) {//DTS 音频格式
			m_argv[m_argc++] = av_strdup("-strict");
			m_argv[m_argc++] = av_strdup("-2");
			if (m_iAudioBitrate == 0) {
				m_argv[m_argc++] = av_strdup("-b:a");
				m_argv[m_argc++] = av_asprintf("%d", m_iAudioBitrate);
			}
		}else{//某些纯音频格式
			if (m_iAudioSampleRate != 0) { //-ar 44100
				m_argv[m_argc++] = av_strdup("-ar");
				m_argv[m_argc++] = av_asprintf("%d", m_iAudioSampleRate);
			}
			if (m_iAudioChannel != 0) { //-ac 2
				m_argv[m_argc++] = av_strdup("-ac");
				m_argv[m_argc++] = av_asprintf("%d", m_iAudioChannel);
			}
			if (WXStrcmp(Last4, _T("flac")) && 
				WXStrcmp(Last4, _T(".wav")) &&
				WXStrcmp(Last4, _T("aiff"))) {
				if (m_iAudioBitrate == 0) {
					m_argv[m_argc++] = av_strdup("-b:a");
					m_argv[m_argc++] = av_asprintf("%d", m_iAudioBitrate);
				}
			}
		}
		m_argv[m_argc++] = av_strdup(wxstrOutput.c_str());
		return Function(async);
	}

	// 多个文件依次合并
	int MergerFile(WXCTSTR strOutput, WXCTSTR strTemp, int async) {  //合并文件，多输入，设置临时文件，输出文件
		WXAutoLock al(this);
		WXString wxstrOut = strOutput;
		WXString wxstrTemp = strTemp;
		FILE *fout = fopen(wxstrTemp.c_str(), "wb");//临时文件
		if (fout == NULL) {
			wxstrTemp = _T("filelist.txt");
			fout = fopen(wxstrTemp.c_str(), "wb");
		}
		if (fout) {
			int num = (int)m_arrInput.size();
			for (int i = 0; i < num; i++) {
				char sz[1024];
#ifdef _WIN32
				sprintf(sz, "file '%ws'\n", m_arrInput[i].str());
#else
				sprintf(sz, "file '%s'\n", m_arrInput[i].c_str());
#endif
				fwrite(sz, strlen(sz), 1, fout);
			}
			fclose(fout);
			m_argc = 0;
			m_argv[m_argc++] = av_strdup("ffmpeg");
			m_argv[m_argc++] = av_strdup("-f");
			m_argv[m_argc++] = av_strdup("concat");
			m_argv[m_argc++] = av_strdup("-safe");
			m_argv[m_argc++] = av_strdup("0");
			m_argv[m_argc++] = av_strdup("-i");
			m_argv[m_argc++] = av_strdup(wxstrTemp.c_str());
            
            if(strcmp(m_strVideoCodec.c_str(), "copy") == 0 ||
               strcmp(m_strAudioCodec.c_str(), "copy") ==0){
                m_argv[m_argc++] = av_strdup("-c");
                m_argv[m_argc++] = av_strdup("copy");
            }
            
			m_argv[m_argc++] = av_strdup(wxstrOut.c_str());
			return Function(async);
		}
		else {
			WXLogWriteNew("Create Temp File Error %s", wxstrTemp.c_str());
			return -1;
		}
		return 0;
	}

	// 截图操作
	int ShotPicture(WXCTSTR strInput,int64_t ts,WXCTSTR strOutput) {
		WXAutoLock al(this);
		WXString wxstrInput = strInput;
		WXString wxstrOutput = strOutput;
		m_argc = 0;
		m_argv[m_argc++] = av_strdup("ffmpeg");
		m_argv[m_argc++] = av_strdup("-ss");
		m_argv[m_argc++] = av_asprintf("%f", (double)(ts) / 1000.0);
		m_argv[m_argc++] = av_strdup("-i");
		m_argv[m_argc++] = av_strdup(wxstrInput.c_str());
		m_argv[m_argc++] = av_strdup(wxstrOutput.c_str());
		m_argv[m_argc++] = av_strdup("-r");
		m_argv[m_argc++] = av_strdup("1");
		m_argv[m_argc++] = av_strdup("-vframes");
		m_argv[m_argc++] = av_strdup("1");
		m_argv[m_argc++] = av_strdup("-an");
		m_argv[m_argc++] = av_strdup("-f");
		m_argv[m_argc++] = av_strdup("mjpeg");
		int ret = avffmpeg_convert(m_ctx, m_argc, m_argv);
		LogRet(ret);
		if (ret == FFMPEG_ERROR_OK) {
			struct stat st;
			int error = stat(wxstrOutput.c_str(), &st);
			if (error == 0) {
				if (st.st_size <= 0) {
					WXLogWriteNew("Convert Result = %s", WXFfmpegGetError(FFMPEG_ERROR_EMPTYFILE));
					return FFMPEG_ERROR_EMPTYFILE;
				}
				else {
					return FFMPEG_ERROR_OK;//OK
				}
			}else {
				WXLogWriteNew("ShotPicture Result = %s", WXFfmpegGetError(FFMPEG_ERROR_NOFILE));
				return FFMPEG_ERROR_NOFILE;//No output file
			}
		}
		return ret;
	}

public: //Set Param
	//设置裁剪区域
	void SetCrop(int x, int y, int w, int h) {
		WXAutoLock al(this);
		//设置裁剪尺寸， 首先设置尺寸
		if (x < 0 || y < 0 || w <= 0 || h <= 0)
			return;
		m_iCropX = x;
		m_iCropY = y;
		m_iCropW = w;
		m_iCropH = h;
	}

	//垂直翻转
	void SetVFlip(int b) {
		WXAutoLock al(this);
		m_bVFilp = b;
	}

	//水平翻转
	void SetHFlip(int b) {
		WXAutoLock al(this);
		m_bHFilp = b;
	}

	//速度
	void SetSpeed(float speed) {
		WXAutoLock al(this);
		m_iSpeed = av_clip(speed, 50, 200);
	}


	void SetPictureQuality(int brightness, int contrast, int saturation) { //亮度,对比度,饱和度 0 50 100
		WXAutoLock al(this);
		m_iBrightness = av_clip(brightness, -100, 100);//0
		m_iContrast = av_clip(contrast, -100, 100);  // 50
		m_iSaturation = av_clip(saturation, 0, 300);    // 100
	}

	WXString m_strVideoFilter = _T(""); //水印
	WXString m_strAudioFilter = _T(""); //-af

	void AddWMImage(WXCTSTR szImage, int x = 0, int y = 0, int w = 0, int h = 0) {
		WXAutoLock al(this);
		WM_Data data;
		data.m_strImage = szImage;
		data.m_iWMPosX = x;
		data.m_iWMPosY = y;
		data.m_iWMWidth = w;
		data.m_iWMHeight = h;
		m_arrWatermark.push_back(data);
	}

	void SetSubtitle(WXCTSTR wsz) {
		WXAutoLock al(this);
        if (WXStrlen(wsz) > 0) {
#ifdef _WIN32
			std::wstring temp = L"";
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
			m_strSubtitle = temp.c_str();
#else
			m_strSubtitle = wsz;
#endif
		}
		else {
			m_strSubtitle = _T("");
		}
	}

	void SetSubtitleFont(WXCTSTR wsz, int FontSize, int FontColor) {
		WXAutoLock al(this);
		m_strSubtitleFontName = _T("");
#ifdef _WIN32
		if (wsz != nullptr && WXStrlen(wsz) > 0)
			m_strSubtitleFontName = wsz;
#endif            
		m_iSubtitleFontSize = FontSize;
		m_iSubtitleFontColor = FontColor;
	}

	void SetSubtitleAlpha(int alpha) {
		WXAutoLock al(this);
		m_iSubtitleFontAlpha = av_clip_c(alpha, 0, 255);
	}

	void SetSubtitlePostion(int postion) {
		WXAutoLock al(this);
		m_iSubtitlePostion = postion;
	}
    
    void SetSubtitleAlignment(int alignment){
        int Align = av_clip(alignment,0,2);
        if(Align == 0)
            m_iAlignment = 2;
        else if(Align == 1)
            m_iAlignment = 10;
        else
            m_iAlignment = 6;
    }

	void SetEventOwner(void *ownerEvent) {
		WXAutoLock al(this);
		avffmpeg_setEventOwner(m_ctx, ownerEvent);
	}

	void SetEventCb(WXFfmpegOnEvent cbEvent) {
		WXAutoLock al(this);
		avffmpeg_setEvent(m_ctx, cbEvent);
	}

	void SetEventID(WXCTSTR szEvent) {
		WXAutoLock al(this);
		avffmpeg_setEventID(m_ctx, szEvent);
	}

	void SetVolume(int volume) {
		WXAutoLock al(this);
		m_iVolume = av_clip(volume, 0, 1000);
	}

	void SetRoate(int rotate) {
		WXAutoLock al(this);
		m_iRotate = (rotate % 360 + 360) % 360;
	}

	void AddInput(WXCTSTR wszInput) {
		WXAutoLock al(this);
		WXString wxstr = wszInput;
		m_arrInput.push_back(wxstr);
	}

	void SetConvertTime(int64_t ptsStart, int64_t ptsDuration) {
		WXAutoLock al(this);
		m_ptsStart = ptsStart;
		m_ptsDuration = ptsDuration;
	}

	void SetVideoCodecStr(WXCTSTR wsz) {
		WXAutoLock al(this);
		m_strVideoCodec = wsz;
		if (m_strVideoCodec == _T("xvid"))m_strVideoCodec = _T("libxvid");
		if (m_strVideoCodec == _T("ogv"))m_strVideoCodec = _T("libtheora");
		if (m_strVideoCodec == _T("ogg"))m_strVideoCodec = _T("libtheora");
		if (m_strVideoCodec == _T("vp8"))m_strVideoCodec = _T("libvpx");
		if (m_strVideoCodec == _T("vp9"))m_strVideoCodec = _T("libvpx-vp9");
	}

	void SetVideoCodecMode(int mode) {
		WXAutoLock al(this);
		m_iVodeoCodecMode = av_clip(mode, 0, 2);
		avffmpeg_set_video_encode_mode(m_ctx, m_iVodeoCodecMode);
	}

	void SetVideoFps(double fps) {
		WXAutoLock al(this);
		m_fFps = fps;
	}

	void SetVideoSize(int width, int height) {
		WXAutoLock al(this);
		m_iWidth = width;
		m_iHeight = height;
	}

	void SetVideoDar(int dar_width, int dar_height) {
		WXAutoLock al(this);
		m_iDARWidth = dar_width;
		m_iDARHeight = dar_height;
	}

	void SetVideoBitrate(int bitrate) {
		WXAutoLock al(this);
		m_iVideoBitrate = bitrate;
		if (m_iVideoBitrate < 1000)
			m_iVideoBitrate *= 1000;
	}

	void SetAudioCodecStr(WXCTSTR wsz) {
		WXAutoLock al(this);
		m_strAudioCodec = wsz;
	}

	void SetAudioBitrate(int bitrate) {
		WXAutoLock al(this);
		m_iAudioBitrate = bitrate;
		if (m_iAudioBitrate < 1000)
			m_iAudioBitrate *= 1000;
	}

	void SetAudioSampleRate(int sample_rate) {
		WXAutoLock al(this);
		m_iAudioSampleRate = sample_rate;
	}

	void SetAudioChannel(int channel) {
		WXAutoLock al(this);
		m_iAudioChannel = channel;
	}

	int64_t GetCurrTime() {
		return avffmpeg_getCurrTime(m_ctx) * m_iSpeed / 100;
	}

	int64_t GetTotalTime() {
		return  m_ptsDuration ? m_ptsDuration : avffmpeg_getTotalTime(m_ctx);//设置了处理长度
		//return avffmpeg_getTotalTime(m_ctx);
	}

	int GetState() {
		return avffmpeg_getState(m_ctx);
	}

	void Interrupt() {
		avffmpeg_interrupt(m_ctx);
	}
};

WXFFMPEG_CAPI void WXFfmpegParamSetRotate(void * p, int rotate) {
	WXConvert *obj = (WXConvert *)p;
	if (obj) obj->SetRoate(rotate);
}

WXFFMPEG_CAPI void WXFfmpegParamSetVFlip(void * p, int b) {
	WXConvert *obj = (WXConvert *)p;
	if (obj) obj->SetVFlip(b);
}

WXFFMPEG_CAPI void WXFfmpegParamSetHFlip(void * p, int b) {
	WXConvert *obj = (WXConvert *)p;
	if (obj) obj->SetHFlip(b);
}

WXFFMPEG_CAPI void WXFfmpegParamSetCrop(void * p, int x, int y, int w, int h) {
	WXConvert *obj = (WXConvert *)p;
	if (obj) obj->SetCrop(x, y, w, h);
}

WXFFMPEG_CAPI void WXFfmpegParamSetSpeed(void * p, int speed) {
	WXConvert *obj = (WXConvert *)p;
	if (obj)obj->SetSpeed(speed);
}

WXFFMPEG_CAPI void WXFfmpegParamSetVolume(void * p, int volume) {
	WXConvert *obj = (WXConvert *)p;
	if (obj) obj->SetVolume(volume);
}

WXFFMPEG_CAPI void WXFfmpegParamSetPictureQuality(void * p, int brightness, int contrast, int saturation) { //亮度
	WXConvert *obj = (WXConvert *)p;
	if (obj)obj->SetPictureQuality(brightness, contrast, saturation);
}

WXFFMPEG_CAPI void *WXFfmpegParamCreate() {
	WXConvert *obj = new WXConvert;
	return (void*)obj;
}

WXFFMPEG_CAPI void WXFfmpegParamDestroy(void * p) {
	WXConvert *obj = (WXConvert *)p;
	if (obj)delete obj;
}

WXFFMPEG_CAPI void WXFfmpegParamAddWMImage(void * p, WXCTSTR wszImage,int x, int y, int w, int h) {
	WXConvert *obj = (WXConvert *)p;
	if (obj)obj->AddWMImage(wszImage, x, y, w, h);
}

WXFFMPEG_CAPI void WXFfmpegParamSetSubtitle(void * p, WXCTSTR sz) {
	WXConvert *obj = (WXConvert *)p;
	if (obj)obj->SetSubtitle(sz);
}

WXFFMPEG_CAPI void WXFfmpegParamSetSubtitleFont(void * p, WXCTSTR sz, int FontSize, int FontColor) {
	WXConvert *obj = (WXConvert *)p;
	if (obj)obj->SetSubtitleFont(sz, FontSize, FontColor);
}

WXFFMPEG_CAPI void WXFfmpegParamSetSubtitleAlpha(void * p, int alpha) {
	WXConvert *obj = (WXConvert *)p;
	if (obj)obj->SetSubtitleAlpha(alpha);
}

WXFFMPEG_CAPI void WXFfmpegParamSetSubtitlePostion(void * p, int postion) {
	WXConvert *obj = (WXConvert *)p;
	if (obj)obj->SetSubtitlePostion(postion);
}

WXFFMPEG_CAPI void WXFfmpegParamSetSubtitleAlignment(void * p, int alignment) {
    WXConvert *obj = (WXConvert *)p;
    if (obj)obj->SetSubtitleAlignment(alignment);
}

WXFFMPEG_CAPI void WXFfmpegParamSetEventOwner(void * p, void *ownerEvent) {
	WXConvert *obj = (WXConvert *)p;
	if (obj)obj->SetEventOwner(ownerEvent);
}

WXFFMPEG_CAPI void WXFfmpegParamSetEventCb(void * p, WXFfmpegOnEvent cbEvent) {
	WXConvert *obj = (WXConvert *)p;
	if (obj)obj->SetEventCb(cbEvent);
}

WXFFMPEG_CAPI void WXFfmpegParamSetEventID(void * p, WXCTSTR  szEvent) {
	WXConvert *obj = (WXConvert *)p;
	if (obj)obj->SetEventID(szEvent);
}

//if Merger File, Maybey has some SetInput By order
WXFFMPEG_CAPI void WXFfmpegParamAddInput(void * p, WXCTSTR szInput) {
	WXConvert *obj = (WXConvert *)p;
	if (obj)obj->AddInput(szInput);
}

WXFFMPEG_CAPI void WXFfmpegParamSetConvertTime(void * p, int64_t ptsStart, int64_t ptsDuration) {
	WXConvert *obj = (WXConvert *)p;
	if (obj) obj->SetConvertTime(ptsStart, ptsDuration);
}

WXFFMPEG_CAPI void WXFfmpegParamSetVideoCodecStr(void * p, WXCTSTR sz) {
	WXConvert *obj = (WXConvert *)p;
	if (obj) obj->SetVideoCodecStr(sz);
}

WXFFMPEG_CAPI void  WXFfmpegParamSetVideoMode(void * p, int mode) {
	WXConvert *obj = (WXConvert *)p;
	if (obj) obj->SetVideoCodecMode(mode);
}

WXFFMPEG_CAPI void WXFfmpegParamSetVideoFps(void * p, double fps) {
	WXConvert *obj = (WXConvert *)p;
	if (obj) obj->SetVideoFps(fps);
}

WXFFMPEG_CAPI void WXFfmpegParamSetVideoSize(void * p, int width, int height) {
	WXConvert *obj = (WXConvert *)p;
	if (obj) obj->SetVideoSize(width, height);
}

WXFFMPEG_CAPI void WXFfmpegParamSetVideoDar(void * p, int dar_width, int dar_height) {
	WXConvert *obj = (WXConvert *)p;
	if (obj) obj->SetVideoDar(dar_width, dar_height);
}

WXFFMPEG_CAPI void WXFfmpegParamSetVideoBitrate(void * p, int bitrate) {
	WXConvert *obj = (WXConvert *)p;
	if (obj) obj->SetVideoBitrate(bitrate);
}

WXFFMPEG_CAPI void WXFfmpegParamSetAudioCodecStr(void * p, WXCTSTR sz) {
	WXConvert *obj = (WXConvert *)p;
	if (obj) obj->SetAudioCodecStr(sz);
}

WXFFMPEG_CAPI void WXFfmpegParamSetAudioBitrate(void * p, int bitrate) {
	WXConvert *obj = (WXConvert *)p;
	if (obj) obj->SetAudioBitrate(bitrate);
}

WXFFMPEG_CAPI void WXFfmpegParamSetAudioSampleRate(void * p, int sample_rate) {
	WXConvert *obj = (WXConvert *)p;
	if (obj) obj->SetAudioSampleRate(sample_rate);
}

WXFFMPEG_CAPI void WXFfmpegParamSetAudioChannel(void * p, int channel) {
	WXConvert *obj = (WXConvert *)p;
	if (obj) obj->SetAudioChannel(channel);
}


WXFFMPEG_CAPI int  WXFfmpegCutFile(void * p, WXCTSTR strInput, WXCTSTR strOutput,
	int64_t ptsStart, int64_t ptsDuration, int Fast, int async) {
	WXConvert *obj = (WXConvert *)p;
	return  obj ? obj->CutFile(strInput, strOutput, ptsStart, ptsDuration, Fast, async) : -1;
}

WXFFMPEG_CAPI int  WXFfmpegConvertVideo(void * p, WXCTSTR strInput, WXCTSTR strOutput, int async) {
	WXConvert *obj = (WXConvert *)p;
	return  obj ? obj->ConvertVideo(strInput, strOutput, async) : -1;
}

WXFFMPEG_CAPI int  WXFfmpegConvertAudio(void * p, WXCTSTR strInput, WXCTSTR strOutput, int async) {
	WXConvert *obj = (WXConvert *)p;
	return  obj ? obj->ConvertAudio(strInput, strOutput, async) : -1;
}

WXFFMPEG_CAPI int WXFfmpegMergerFile(void * p, WXCTSTR strOutput,WXCTSTR strTemp, int async) {
	WXConvert *obj = (WXConvert *)p;
	return obj ? obj->MergerFile(strOutput, strTemp, async) : -1;
}

WXFFMPEG_CAPI int WXFfmpegMixerAV(void * p, WXCTSTR strAudio, WXCTSTR strVideo, WXCTSTR strMixer, int async) {
	WXConvert *obj = (WXConvert *)p;
	return obj ? obj->MixerAV(strVideo, strAudio, strMixer, async) : -1;
}

WXFFMPEG_CAPI int WXFfmpegReplaceAudio(void * p, WXCTSTR strVideo, WXCTSTR strAudio, WXCTSTR strOutput, int copy, int async){
    WXConvert *obj = (WXConvert *)p;
    return obj ? obj->ReplaceAudio(strVideo, strAudio, strOutput, copy, async) : -1;
}

WXFFMPEG_CAPI int WXFfmpegShotPicture(void * p, WXCTSTR strInput, int64_t ts, WXCTSTR strOutput) {
	WXConvert *obj = (WXConvert *)p;
	return obj ? obj->ShotPicture(strInput, ts, strOutput) : -1;
}

WXFFMPEG_CAPI int64_t WXFfmpegGetCurrTime(void * p) {
	WXConvert *obj = (WXConvert *)p;
	return obj ? obj->GetCurrTime() : 0;
}

WXFFMPEG_CAPI int64_t WXFfmpegGetTotalTime(void * p) {
	WXConvert *obj = (WXConvert *)p;
	return obj ? obj->GetTotalTime() : 0;
}

WXFFMPEG_CAPI int WXFfmpegGetState(void * p) {
	WXConvert *obj = (WXConvert *)p;
	return obj ? obj->GetState() : -1;
}

WXFFMPEG_CAPI void WXFfmpegInterrupt(void * p) {
	WXConvert *obj = (WXConvert *)p;
	if (obj)obj->Interrupt();
}
