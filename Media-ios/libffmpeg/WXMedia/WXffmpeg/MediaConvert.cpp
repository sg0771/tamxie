
#include "WXMediaAPI.h"
#include "MediaConvert.h"
#include "FfmpegConvertClass.h"
MediaConvert::MediaConvert()
{
	if (m_pFfmpegConv == NULL)
		m_pFfmpegConv = new FfmpegConvert;
}

MediaConvert::~MediaConvert()
{
	if (m_pFfmpegConv != NULL)
		delete m_pFfmpegConv;
}

void MediaConvert::SetRoate(int rotate) {
	m_iRotate = (rotate % 360 + 360) % 360;
}

//垂直翻转
void MediaConvert::SetVFlip(int b) {
	m_bVFilp = b;
}

//水平翻转
void MediaConvert::SetHFlip(int b) {
	m_bHFilp = b;
}

//设置裁剪区域
void MediaConvert::SetCrop(int x, int y, int w, int h) {
	//设置裁剪尺寸， 首先设置尺寸
	if (x < 0 || y < 0 || w <= 0 || h <= 0)
		return;
	m_iCropX = x;
	m_iCropY = y;
	m_iCropW = w;
	m_iCropH = h;
}

//速度
void MediaConvert::SetSpeed(float speed) {
	m_iSpeed = av_clip(speed, 50, 200);
}


void MediaConvert::SetVolume(int volume) {
	m_iVolume = av_clip(volume, 0, 1000);
}

void MediaConvert::SetPictureQuality(int brightness, int contrast, int saturation) { //亮度,对比度,饱和度 0 50 100
	m_iBrightness = av_clip(brightness, -100, 100);//0
	m_iContrast = av_clip(contrast, -100, 100);  // 50
	m_iSaturation = av_clip(saturation, 0, 300);    // 100
}

void MediaConvert::SetEventOwner(void *ownerEvent)
{
	avffmpeg_owner = ownerEvent;
	m_pFfmpegConv->SetFfmpegOwner(ownerEvent);
}

void MediaConvert::SetEventCb(WXFfmpegOnEvent cbEvent)
{
	m_cbEvent = cbEvent;
}

void MediaConvert::SetEventID(WXCTSTR  szEvent)
{
	m_szEvent = szEvent;
}

void MediaConvert::SetConvertTime(int64_t ptsStart, int64_t ptsDuration) {
	m_ptsStart = ptsStart;
	m_ptsDuration = ptsDuration;
}


//字幕
void MediaConvert::SetSubtitle(WXCTSTR wsz) {
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

void MediaConvert::SetSubtitleFont(WXCTSTR wsz, int FontSize, int FontColor) {
	m_strSubtitleFontName = _T("");
#ifdef _WIN32
	if (wsz != nullptr && WXStrlen(wsz) > 0)
		m_strSubtitleFontName = wsz;
#endif            
	m_iSubtitleFontSize = FontSize;
	m_iSubtitleFontColor = FontColor;
}

void MediaConvert::SetSubtitleAlpha(int alpha) {
	m_iSubtitleFontAlpha = av_clip_c(alpha, 0, 255);
}

void MediaConvert::SetSubtitlePostion(int postion) {
	m_iSubtitlePostion = postion;
}

void MediaConvert::SetSubtitleAlignment(int alignment) {
	int Align = av_clip(alignment, 0, 2);
	if (Align == 0)
		m_iAlignment = 2;
	else if (Align == 1)
		m_iAlignment = 10;
	else
		m_iAlignment = 6;
}


//video
void MediaConvert::SetVideoCB(onFfmpegVideoData cb) {
	m_cbVideo = cb;
	m_pFfmpegConv->SetFfmpegVideoData(cb);
}

void MediaConvert::SetVideoFmtStr(WXCTSTR wsz) {
	m_strVideoFmt = wsz;
}

void MediaConvert::SetVideoCodecStr(WXCTSTR wsz) {
	m_strVideoCodec = wsz;
	if (m_strVideoCodec == _T("xvid"))m_strVideoCodec = _T("libxvid");
	if (m_strVideoCodec == _T("ogv"))m_strVideoCodec = _T("libtheora");
	if (m_strVideoCodec == _T("ogg"))m_strVideoCodec = _T("libtheora");
	if (m_strVideoCodec == _T("vp8"))m_strVideoCodec = _T("libvpx");
	if (m_strVideoCodec == _T("vp9"))m_strVideoCodec = _T("libvpx-vp9");
}

void MediaConvert::SetVideoCodecMode(int mode) {
	m_iVodeoCodecMode = av_clip(mode, 0, 2);
}

void MediaConvert::SetVideoFps(double fps) {
	m_fFps = fps;
}

void MediaConvert::SetVideoSize(int width, int height) {
	m_iWidth = width;
	m_iHeight = height;
}

void MediaConvert::SetVideoDar(int dar_width, int dar_height) {
	m_iDARWidth = dar_width;
	m_iDARHeight = dar_height;
}

void MediaConvert::SetVideoBitrate(int bitrate) {
	m_iVideoBitrate = bitrate;
	if (m_iVideoBitrate < 1000)
		m_iVideoBitrate *= 1000;
}



//audio
void MediaConvert::SetAudioCodecStr(WXCTSTR wsz) {
	m_strAudioCodec = wsz;
}

void MediaConvert::SetAudioBitrate(int bitrate) {
	m_iAudioBitrate = bitrate;
	if (m_iAudioBitrate < 1000)
		m_iAudioBitrate *= 1000;
}

void MediaConvert::SetAudioSampleRate(int sample_rate) {
	m_iAudioSampleRate = sample_rate;
}

void MediaConvert::SetAudioChannel(int channel) {
	m_iAudioChannel = channel;
}



//辅助
void MediaConvert::AddInput(WXCTSTR wszInput) {
	WXString wxstr = wszInput;
	m_arrInput.push_back(wxstr);
}

int MediaConvert::LogRet(int ret) {
	WXLogWriteNew("Convert Param");
	for (int i = 0; i < m_argc; i++) {
		WXLogWriteNew(_T("argv[%d] = %s"), i, m_arrStrArgv[i].c_str());
	}
	WXString wxstr = WXFfmpegGetError(ret);
	WXLogWriteNew("WXConvert Result = %s", wxstr.c_str());
	return ret;
}

int MediaConvert::WX_GCD(int m, int n) {
	int r;
	while (n != 0)
	{
		r = (m >= n) ? (m - n) : m;
		m = n;
		n = r;
	}
	return m;
}

int64_t MediaConvert::GetCurrTime()
{
	if (m_pFfmpegConv)
		return m_pFfmpegConv->GetCurrTime();
	return -1;
}

int64_t MediaConvert::GetTotalTime()
{
	if (m_pFfmpegConv)
		return m_pFfmpegConv->GetTotalTime();
	return -1;
}

int MediaConvert::GetState()
{
	if (m_pFfmpegConv)
		return m_pFfmpegConv->GetState();
	return -1;
}

void MediaConvert::Break()
{
	if (m_pFfmpegConv)
		return m_pFfmpegConv->Break();
}

int MediaConvert::Convert(int argc, char **argv)
{
	if (m_pFfmpegConv)
		return m_pFfmpegConv->Convert(argc, argv);
    return -1;
}

//功能
void MediaConvert::AddWMImage(WXCTSTR szImage, int x, int y, int w, int h) {
	WM_Data data;
	data.m_strImage = szImage;
	data.m_iWMPosX = x;
	data.m_iWMPosY = y;
	data.m_iWMWidth = w;
	data.m_iWMHeight = h;
	m_arrWatermark.push_back(data);
}

int MediaConvert::ConvertVideoFast(WXCTSTR strInput, WXCTSTR strOutput) {
	m_argc = 0;
	m_arrStrArgv[m_argc++].Format("ffmpeg");
	m_arrStrArgv[m_argc++].Format("-i");
	m_arrStrArgv[m_argc++].Format(strInput);
	m_arrStrArgv[m_argc++].Format("-c");
	m_arrStrArgv[m_argc++].Format("copy");
	m_arrStrArgv[m_argc++].Format(strOutput);
	for (int i = 0; i < m_argc; i++)
		m_argv[i] = (char*)m_arrStrArgv[i].c_str();
	return m_pFfmpegConv->Convert(m_argc, m_argv);
}

//H264+AAC--->MP4
int MediaConvert::MixerAV(WXCTSTR strVideo, WXCTSTR strAudio, WXCTSTR strMixer) {
	m_argc = 0;
	m_arrStrArgv[m_argc++].Format("ffmpeg");
	m_arrStrArgv[m_argc++].Format("-i");
	m_arrStrArgv[m_argc++].Format(strVideo);
	m_arrStrArgv[m_argc++].Format("-i");
	m_arrStrArgv[m_argc++].Format(strAudio);
	m_arrStrArgv[m_argc++].Format("-c");
	m_arrStrArgv[m_argc++].Format("copy");
	m_arrStrArgv[m_argc++].Format(strMixer);
	for (int i = 0; i < m_argc; i++)
		m_argv[i] = (char*)m_arrStrArgv[i].c_str();
	return m_pFfmpegConv->Convert(m_argc, m_argv);
}

int MediaConvert::ReplaceAudio(WXCTSTR strVideo, WXCTSTR strAudio, WXCTSTR strMixer, int copy) {
	m_argc = 0;
	m_arrStrArgv[m_argc++].Format("ffmpeg");
	m_arrStrArgv[m_argc++].Format("-i");
	m_arrStrArgv[m_argc++].Format(strVideo);
	m_arrStrArgv[m_argc++].Format("-i");
	m_arrStrArgv[m_argc++].Format(strAudio);
	m_arrStrArgv[m_argc++].Format("-map");
	m_arrStrArgv[m_argc++].Format("0:v");
	m_arrStrArgv[m_argc++].Format("-map");
	m_arrStrArgv[m_argc++].Format("1:a");


	if (copy) {
		m_arrStrArgv[m_argc++].Format("-c");
		m_arrStrArgv[m_argc++].Format("copy");
	}
	m_arrStrArgv[m_argc++].Format(strMixer);
	for (int i = 0; i < m_argc; i++)
		m_argv[i] = (char*)m_arrStrArgv[i].c_str();
	return m_pFfmpegConv->Convert(m_argc, m_argv);
}

//文件裁剪
int MediaConvert::CutFile(WXCTSTR strInput, WXCTSTR strOutput, int64_t ptsStart, int64_t ptsDuration, int Fast) {
	m_argc = 0;
	m_arrStrArgv[m_argc++].Format("ffmpeg");
	m_arrStrArgv[m_argc++].Format("-i");
	m_arrStrArgv[m_argc++].Format(strInput);

	if (ptsStart != 0 || ptsDuration != 0) {
		m_arrStrArgv[m_argc++].Format("-ss");
		m_arrStrArgv[m_argc++].Format("%0.2f", ptsStart / 1000.0f);
		m_arrStrArgv[m_argc++].Format("-t");
		m_arrStrArgv[m_argc++].Format("%0.2f", ptsDuration / 1000.0f);
	}

	if (Fast) {
		m_arrStrArgv[m_argc++].Format("-c");
		m_arrStrArgv[m_argc++].Format("copy");
	}
	m_arrStrArgv[m_argc++].Format(strOutput);
	for (int i = 0; i < m_argc; i++)
		m_argv[i] = (char*)m_arrStrArgv[i].c_str();
	return m_pFfmpegConv->Convert(m_argc, m_argv);
}

void MediaConvert::HandleWaterMark() {
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
void MediaConvert::HandleSubtitle() {
	if (m_strSubtitle.length() != 0) {
		WXString wxstr;
		wxstr.Format("subtitles=%s", m_strSubtitle.c_str());
		WXString wxstrForce_Style;
		uint32_t color = (m_iSubtitleFontAlpha << 24) | m_iSubtitleFontColor;
		if (m_strSubtitleFontName.length() > 0) {
			wxstrForce_Style.Format(":force_style=\'FontName=%s,FontSize=%d,PrimaryColour=&H%08x&,MarginV=%d,Alignment=%d\'",
				m_strSubtitleFontName.c_str(), m_iSubtitleFontSize, color, m_iSubtitlePostion, m_iAlignment);
		}
		else if (m_strSubtitleFontName.length() == 0) {
			wxstrForce_Style.Format(":force_style=\'FontSize=%d,PrimaryColour=&H%08x&,MarginV=%d,Alignment=%d\'",
				m_iSubtitleFontSize, color, m_iSubtitlePostion, m_iAlignment);
		}
		wxstr += wxstrForce_Style;
		m_strVideoFilter.Cat(wxstr, _T(", "));
	}
}

void MediaConvert::HandleFilters() {
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
			WXString wxstr = _T("transpose=clock");
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

//视频格式转换
int MediaConvert::ConvertVideo(WXCTSTR strInput, WXCTSTR strOutput) {
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
	m_arrStrArgv[m_argc++].Format("ffmpeg");

	if (m_ptsStart > 0 || m_ptsDuration > 0) { //-ss tsStart -t tsDuration
		m_arrStrArgv[m_argc++].Format("-ss");
		m_arrStrArgv[m_argc++].Format("%0.2f", m_ptsStart / 1000.0f);
		m_arrStrArgv[m_argc++].Format("-t");
		m_arrStrArgv[m_argc++].Format("%0.2f", m_ptsDuration / 1000.0f);
	}

	//输入文件
	m_arrStrArgv[m_argc++].Format("-i");
	m_arrStrArgv[m_argc++].Format(wsxtrInput.c_str());

	//水印图片
	if (m_arrWatermark.size() > 0) {
		for (int i = 0; i < m_arrWatermark.size(); i++) {
			if (m_arrWatermark[i].m_strImage.length() > 0) { //水印图片
				m_arrStrArgv[m_argc++].Format("-i");
				m_arrStrArgv[m_argc++].Format(m_arrWatermark[i].m_strImage.c_str());
			}
		}
	}

	if (WXStrcmp(Last3, _T(".dv")) == 0) { //dvvideo DV视频格式
		m_arrStrArgv[m_argc++].Format("-target");
		m_strVideoCodec == _T("ntscdv") ? m_arrStrArgv[m_argc++].Format("ntsc-dv") : m_arrStrArgv[m_argc++].Format("pal-dv");
		m_arrStrArgv[m_argc++].Format("-aspect");
		m_arrStrArgv[m_argc++].Format("4:3");
		m_arrStrArgv[m_argc++].Format(strOutput);
	}
	else if (WXStrcmp(Last4, _T(".vob")) == 0) { //vob 视频格式
		m_arrStrArgv[m_argc++].Format("-target");
		m_arrStrArgv[m_argc++].Format("pal-dv");
		m_arrStrArgv[m_argc++].Format(wxstrOutput.c_str());
		if (m_iWidth == 720 && m_iHeight == 576) {
			m_arrStrArgv[m_argc++].Format("pal-dvd");
		}
		else  if (m_iWidth == 720 && m_iHeight == 480) {
			m_arrStrArgv[m_argc++].Format("ntsc-dvd");
		}
		else if (m_iWidth == 480 && m_iHeight == 576) {
			m_arrStrArgv[m_argc++].Format("pal-svcd");
		}
		else  if (m_iWidth == 480 && m_iHeight == 480) {
			m_arrStrArgv[m_argc++].Format("ntsc-svcd");
		}
		else if (m_iWidth == 352 && m_iHeight == 288) {
			m_arrStrArgv[m_argc++].Format("pal-vcd");
		}
		else  if (m_iWidth == 352 && m_iHeight == 240) {
			m_arrStrArgv[m_argc++].Format("ntsc-vcd");
		}
	}
	else {
		bool setSize = 0;
		if (m_iWidth != 0 && m_iHeight != 0) { //-s 352x288  分辨率设置
			setSize = 1;
			//设置DAR
			{
				WXString wxstr;
				wxstr.Format("setsar=sar=1/1");//设置输出SAR为1：1
				m_strVideoFilter.Cat(wxstr, _T(","));

				m_arrStrArgv[m_argc++].Format("-aspect");//设置输出比例为PAR，使得在MAC分辨率正常
				int GCD = WX_GCD(m_iWidth, m_iHeight);
				m_arrStrArgv[m_argc++].Format("%d:%d", m_iWidth / GCD, m_iHeight / GCD);

				if (m_iDARWidth && m_iDARHeight) { //设置DAR输出，裁剪输出窗口，填充黑边，使
					int deltaW = 0;
					int deltaH = 0;
					if (m_iDARWidth * m_iHeight < m_iDARHeight * m_iWidth) {
						deltaW = (m_iWidth - m_iDARWidth * m_iHeight / m_iDARHeight) / 2;
					}
					else {
						deltaH = (m_iHeight - m_iDARHeight * m_iWidth / m_iDARWidth) / 2;
					}
					WXString wxstr2;
					wxstr2.Format("scale=%d:%d,pad=%d:%d:%d:%d", m_iWidth - 2 * deltaW, m_iHeight - 2 * deltaH,
						m_iWidth, m_iHeight, deltaW, deltaH);//设置输出SAR为1：1
					m_strVideoFilter.Cat(wxstr2, _T(","));
				}
			}

			m_arrStrArgv[m_argc++].Format("-s");
			m_arrStrArgv[m_argc++].Format("%dx%d", m_iWidth, m_iHeight);
		}

		if (m_strVideoCodec != _T("noset")) { //-vcodec libx264
			m_arrStrArgv[m_argc++].Format("-vcodec");
			m_arrStrArgv[m_argc++].Format(m_strVideoCodec.c_str());
		}


		if (m_strVideoFmt != _T("noset")) { //-pix_fmt yuv420p
			m_arrStrArgv[m_argc++].Format("-pix_fmt");
			m_arrStrArgv[m_argc++].Format(m_strVideoFmt.c_str());

			if (setSize == 0) { //没有手动设置分辨率
								//保持了分辨率，但是奇数分辨率使用 -pix_fmt yuv420p 会报错！！
				int error_v = 0;
				void *info = WXMediaInfoCreate(strInput, &error_v);
				if (info) {
					m_iWidth = WXMediaInfoGetVideoWidth(info) / 2 * 2;
					m_iHeight = WXMediaInfoGetVideoHeight(info) / 2 * 2;
					WXMediaInfoDestroy(info);
				}
				m_iWidth = m_iWidth / 2 * 2;
				m_iHeight = m_iHeight / 2 * 2;
				m_arrStrArgv[m_argc++].Format("-s");
				m_arrStrArgv[m_argc++].Format("%dx%d", m_iWidth, m_iHeight);
			}
		}

		if (fabs(m_fFps - 0.0) > 1.0) { //-r 25
			m_arrStrArgv[m_argc++].Format("-r");
			m_arrStrArgv[m_argc++].Format("%02f", m_fFps);
		}

		if (m_iVideoBitrate != 0) { //b:v
			m_arrStrArgv[m_argc++].Format("-b:v");
			m_arrStrArgv[m_argc++].Format("%d", m_iVideoBitrate);
		}


		if (m_strAudioCodec != _T("noset")) { //-acodec aac
			m_arrStrArgv[m_argc++].Format("-acodec");
			m_arrStrArgv[m_argc++].Format(m_strAudioCodec.c_str());
		}

		if (m_iAudioSampleRate != 0) { //-ar 44100
			m_arrStrArgv[m_argc++].Format("-ar");
			m_arrStrArgv[m_argc++].Format("%d", m_iAudioSampleRate);
		}

		if (m_iAudioChannel != 0) { //-ac 2
			m_arrStrArgv[m_argc++].Format("-ac");
			m_arrStrArgv[m_argc++].Format("%d", m_iAudioChannel);
		}

		if (m_iAudioBitrate != 0) { //-ab 128k
			m_arrStrArgv[m_argc++].Format("-b:a");
			m_arrStrArgv[m_argc++].Format("%d", m_iAudioBitrate);
		}

		if (m_iVolume != 256) { //-vol 1000 默认音量256  或者用 -af volume=10db ?
			m_arrStrArgv[m_argc++].Format("-vol");
			m_arrStrArgv[m_argc++].Format("%d", m_iVolume);
		}

		if (m_strVideoFilter.length() != 0) {
			m_arrStrArgv[m_argc++].Format("-filter_complex");
			m_arrStrArgv[m_argc++].Format(m_strVideoFilter.c_str());
		}

		if (m_strAudioFilter.length() > 0) { // "-af"
			m_arrStrArgv[m_argc++].Format("-af");
			m_arrStrArgv[m_argc++].Format(m_strAudioFilter.c_str());
		}
		m_arrStrArgv[m_argc++].Format(wxstrOutput.c_str());
	}
	for (int i = 0; i < m_argc; i++)
		m_argv[i] = (char*)m_arrStrArgv[i].c_str();
	return m_pFfmpegConv->Convert(m_argc, m_argv);
}

//音频格式转换
int MediaConvert::ConvertAudio(WXCTSTR strInput, WXCTSTR strOutput) {
	WXString wxstrOutput = strOutput;
	//后缀名判断
	WXCTSTR Last3 = wxstrOutput.Left(3);
	WXCTSTR Last4 = wxstrOutput.Left(4);

	if (WXStrcmp(Last4, _T(".aac")) == 0 || WXStrcmp(Last4, _T(".mp3")) == 0) {
		m_iAudioBitrate = 44100;
		m_iAudioChannel = 2;
		if (m_iAudioBitrate == 0)
			m_iAudioBitrate = 128000;
	}


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
	m_arrStrArgv[m_argc++].Format("ffmpeg");

	if (m_ptsStart > 0 || m_ptsDuration > 0) { //-ss tsStart -t tsDuration
		m_arrStrArgv[m_argc++].Format("-ss");
		m_arrStrArgv[m_argc++].Format("%0.2f", m_ptsStart / 1000.0f);
		m_arrStrArgv[m_argc++].Format("-t");
		m_arrStrArgv[m_argc++].Format("%0.2f", m_ptsDuration / 1000.0f);
	}

	//输入文件
	m_arrStrArgv[m_argc++].Format("-i");
	m_arrStrArgv[m_argc++].Format(strInput);
	m_arrStrArgv[m_argc++].Format("-vn");

	if (WXStrcmp(Last4, _T(".dts")) == 0) {//DTS 音频格式
		m_arrStrArgv[m_argc++].Format("-strict");
		m_arrStrArgv[m_argc++].Format("-2");
		if (m_iAudioBitrate == 0) {
			m_arrStrArgv[m_argc++].Format("-b:a");
			m_arrStrArgv[m_argc++].Format("%d", m_iAudioBitrate);
		}
	}
	else {//某些纯音频格式
		if (m_iAudioSampleRate != 0) { //-ar 44100
			m_arrStrArgv[m_argc++].Format("-ar");
			m_arrStrArgv[m_argc++].Format("%d", m_iAudioSampleRate);
		}
		if (m_iAudioChannel != 0) { //-ac 2
			m_arrStrArgv[m_argc++].Format("-ac");
			m_arrStrArgv[m_argc++].Format("%d", m_iAudioChannel);
		}
		if (WXStrcmp(Last4, _T("flac")) &&
			WXStrcmp(Last4, _T(".wav")) &&
			WXStrcmp(Last4, _T("aiff"))) {
			if (m_iAudioBitrate != 0) {
				m_arrStrArgv[m_argc++].Format("-b:a");
				m_arrStrArgv[m_argc++].Format("%d", m_iAudioBitrate);
			}
		}
	}
	m_arrStrArgv[m_argc++].Format(wxstrOutput.c_str());
	for (int i = 0; i < m_argc; i++)
		m_argv[i] = (char*)m_arrStrArgv[i].c_str();
	return m_pFfmpegConv->Convert(m_argc, m_argv);
}


// 截图操作
int MediaConvert::ShotPicture(WXCTSTR strInput, int64_t ts, WXCTSTR strOutput) {

	WXString wxstrOutput = strOutput;
	m_argc = 0;
	m_arrStrArgv[m_argc++].Format("ffmpeg");
	m_arrStrArgv[m_argc++].Format("-ss");
	m_arrStrArgv[m_argc++].Format("%f", (double)(ts) / 1000.0);//传入毫秒
	m_arrStrArgv[m_argc++].Format("-i");
	m_arrStrArgv[m_argc++].Format(strInput);//输入文件
	m_arrStrArgv[m_argc++].Format(wxstrOutput.c_str());//输出文件
	m_arrStrArgv[m_argc++].Format("-r");
	m_arrStrArgv[m_argc++].Format("1");
	m_arrStrArgv[m_argc++].Format("-vframes");
	m_arrStrArgv[m_argc++].Format("1");
	m_arrStrArgv[m_argc++].Format("-an");
	m_arrStrArgv[m_argc++].Format("-f");
	m_arrStrArgv[m_argc++].Format("mjpeg");

	for (int i = 0; i < m_argc; i++)
		m_argv[i] = (char*)m_arrStrArgv[i].c_str();
	for (int i = 0; i < m_argc; i++)
		m_argv[i] = (char*)m_arrStrArgv[i].c_str();
	int ret = m_pFfmpegConv->Convert(m_argc, m_argv);
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
		}
		else {
			WXLogWriteNew("ShotPicture Result = %s", WXFfmpegGetError(FFMPEG_ERROR_NOFILE));
			return FFMPEG_ERROR_NOFILE;//No output file
		}
	}
	return ret;
}




//新增接口
// 多个文件依次合并
int MediaConvert::MergerFile(WXCTSTR strOutput, WXCTSTR strTemp, int fast) {  //合并文件，多输入，设置临时文件，输出文件
	WXString wxstrTemp = strTemp;
	FILE* fout = nullptr;
	if (strTemp != nullptr)
#ifdef _WIN32
		fout = _tfopen(strTemp, L"wb");//临时文件
#elif __LINUX__
		fout = fopen(strTemp, "wb");
#endif 


	if (fout == nullptr) {
		wxstrTemp.Format("filelist.txt");
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
		m_arrStrArgv[m_argc++].Format("ffmpeg");
		m_arrStrArgv[m_argc++].Format("-f");
		m_arrStrArgv[m_argc++].Format("concat");
		//m_arrStrArgv[m_argc++].Format("-safe");
		//m_arrStrArgv[m_argc++].Format("0");
		m_arrStrArgv[m_argc++].Format("-i");
		m_arrStrArgv[m_argc++].Format(wxstrTemp.str());

		if (fast) {
			m_arrStrArgv[m_argc++].Format("-c");
			m_arrStrArgv[m_argc++].Format("copy");
		}

		m_arrStrArgv[m_argc++].Format(strOutput);
		for (int i = 0; i < m_argc; i++)
			m_argv[i] = (char*)m_arrStrArgv[i].c_str();
		return m_pFfmpegConv->Convert(m_argc, m_argv);
	}
	else {
		WXLogWriteNew("Create Temp File Error %s", wxstrTemp.c_str());
		return -1;
	}
	return 0;
}

//视频转GIF
int MediaConvert::ConvertVideo2Gif(WXCTSTR wszInput, WXCTSTR wszOutput, int64_t t_start, int64_t t_duration)
{
	m_argc = 0;
	m_arrStrArgv[m_argc++].Format("ffmpeg");
	m_arrStrArgv[m_argc++].Format("-i");
	m_arrStrArgv[m_argc++].Format(wszInput);
	if (t_start > 0 && t_duration > 0)
	{
		m_arrStrArgv[m_argc++].Format("-ss");
		m_arrStrArgv[m_argc++].Format("%0.2f", t_start / 1000.0f);
		m_arrStrArgv[m_argc++].Format("-t");
		m_arrStrArgv[m_argc++].Format("%0.2f", t_duration / 1000.0f);
	}
	m_arrStrArgv[m_argc++].Format(wszOutput);
	for (int i = 0; i < m_argc; i++)
	{
		m_argv[i] = (char*)m_arrStrArgv[i].c_str();
	}
	return m_pFfmpegConv->Convert(m_argc, m_argv);
}

//从视频提取音频
int MediaConvert::GetAudioFromVideo(WXCTSTR wszInput, WXCTSTR wszOutput, WXCTSTR wszFormat, int nb_Samples)
{
	m_argc = 0;
	m_arrStrArgv[m_argc++].Format("ffmpeg");
	m_arrStrArgv[m_argc++].Format("-i");
	m_arrStrArgv[m_argc++].Format(wszInput);
	m_arrStrArgv[m_argc++].Format("-f");
	m_arrStrArgv[m_argc++].Format(wszFormat);
	m_arrStrArgv[m_argc++].Format("-ar");
	m_arrStrArgv[m_argc++].Format("%d", nb_Samples);
	m_arrStrArgv[m_argc++].Format("-vn");
	m_arrStrArgv[m_argc++].Format(wszOutput);

	for (int i = 0; i < m_argc; i++)
	{
		m_argv[i] = (char*)m_arrStrArgv[i].c_str();
	}
	return m_pFfmpegConv->Convert(m_argc, m_argv);
}
