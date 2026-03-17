#include "X264Encoder.h"


BOOL X264Encoder::OpenH264(int ForceHeight, int nWidth, int nHeight, int nFps, int iQP, bool bFLV) {
	WXTcpAutoLock al(m_mutex);
	x264_param_default(&m_param);

	m_iSrcWidth = nWidth;
	m_iSrcHeight = nHeight;
	if (ForceHeight != 0) {
		m_iHeight = ForceHeight;
		m_iWidth = ForceHeight * m_iSrcWidth / m_iSrcHeight;
		m_iWidth = m_iWidth / 2 * 2;
	}
	else {
		m_iWidth = nWidth;
		m_iHeight = nHeight;
	}

	x264_param_default_preset(&m_param, "veryfast", "zerolatency");
	x264_param_apply_profile(&m_param, "baseline");

	m_param.i_width = m_iWidth;
	m_param.i_height = m_iHeight;
	m_param.rc.i_qp_min = 10;
	m_param.rc.i_qp_max = 51;
	m_param.i_csp = X264_CSP_I420;//

	m_param.i_fps_num = nFps;
	m_param.i_fps_den = 1;

	m_param.i_frame_reference = 1;//
	//m_param.b_cabac = 1;

	m_param.i_threads = 2;

	m_param.rc.i_rc_method = X264_RC_CQP;
	m_param.rc.i_qp_constant = iQP;

	m_param.i_keyint_max = 125;//运动剧烈的视频可能会产生花屏,默认125

	//FLV 不需要 00 00 00 01 起始码
	m_bFLV = 0;// bFLV;
	if (m_bFLV) {
		m_param.b_repeat_headers = 0;//每个关键帧有SPS PPS
		m_param.b_annexb = 0;//不是 00 00 00 01 或者 00 00 01 头
	}
	else {
		m_param.b_repeat_headers = 1;//每个关键帧有SPS PPS
		m_param.b_annexb = 1;//不是 00 00 00 01 或者 00 00 01 头
	}

	//m_param.i_slice_max_size = 1360;//RTP 限制
	m_h = x264_encoder_open(&m_param);
	if (m_h == NULL) {
		return FALSE;
	}
	x264_picture_alloc(&m_pic, X264_CSP_I420, m_iWidth, m_iHeight);
	m_srcFrame.Init(m_iWidth, m_iHeight);

	//SPS PPS参数
	x264_nal_t* nal = NULL;
	int nnal = 0;

	uint8_t* pSPS = NULL;
	int iSPS = 0;

	uint8_t* pPPS = NULL;
	int iPPS = 0;
	x264_encoder_headers(m_h, &nal, &nnal);
	for (int i = 0; i < nnal; i++) {
		if (nal[i].i_type == 7) {
			iSPS = nal[i].i_payload - 4;
			pSPS = nal[i].p_payload + 4;
		}
		if (nal[i].i_type == 8) {
			iPPS = nal[i].i_payload - 4;
			pPPS = nal[i].p_payload + 4;
		}
	}
	memset(m_extradata, 0, 200);
	m_extradata[0] = 0x01;
	m_extradata[1] = pSPS[1];
	m_extradata[2] = pSPS[2];
	m_extradata[3] = pSPS[3];
	m_extradata[4] = 0xff;
	m_extradata[5] = 0xe1;
	m_extradata[6] = (iSPS >> 8) & 0xff;
	m_extradata[7] = iSPS & 0xff;
	memcpy(m_extradata + 8, pSPS, iSPS);
	m_extradata[8 + iSPS] = 1;
	m_extradata[9 + iSPS] = (iPPS >> 8) & 0xff;
	m_extradata[10 + iSPS] = iPPS & 0xff;
	memcpy(m_extradata + 11 + iSPS, pPPS, iPPS);
	m_extradata_size = iSPS + iPPS + 11;
	m_pOut = new uint8_t[m_iWidth * m_iHeight];//H264 编码输出
	return TRUE;
}

//直接编码AVC
BOOL X264Encoder::Encode(WXTcpVideoFrame* frame, unsigned char*& pOut, int& outlen) {
	WXTcpAutoLock al(m_mutex);
	if (m_iSrcWidth != m_iWidth || m_iSrcHeight != m_iHeight) {//先缩放再转换更快
		libyuv::ARGBScale(frame->m_pBuf, m_iSrcWidth * 4,
			m_iSrcWidth, m_iSrcHeight,
			m_srcFrame.m_pBuf, m_iWidth * 4,
			m_iWidth, m_iHeight,
			libyuv::FilterMode::kFilterBox
		);
		libyuv::ARGBToI420(frame->m_pBuf, m_iWidth * 4,
			m_pic.img.plane[0], m_pic.img.i_stride[0],
			m_pic.img.plane[1], m_pic.img.i_stride[1],
			m_pic.img.plane[2], m_pic.img.i_stride[2],
			m_iWidth, m_iHeight);
	}
	else {
		libyuv::ARGBToI420(frame->m_pBuf, m_iSrcWidth * 4,
			m_pic.img.plane[0], m_iSrcWidth,
			m_pic.img.plane[1], m_iSrcWidth / 2,
			m_pic.img.plane[2], m_iSrcWidth / 2,
			m_iSrcWidth, m_iSrcHeight);
	}

	x264_nal_t* nal = nullptr;
	int i_nal = 0;
	x264_picture_t pic_out;
	int i_frame_size = x264_encoder_encode(m_h, &nal, &i_nal, &m_pic, &pic_out);//编码过程
	if (i_frame_size <= 0)return FALSE;

	memcpy(m_pOut, nal[0].p_payload, i_frame_size);
	pOut = m_pOut;
	outlen = i_frame_size;
	return TRUE;
}

void X264Encoder::Close() {
	WXTcpAutoLock al(m_mutex);
	if (m_h) {
		x264_picture_clean(&m_pic);
		x264_encoder_close(m_h);
		m_h = NULL;
		delete[]m_pOut;
	}
}