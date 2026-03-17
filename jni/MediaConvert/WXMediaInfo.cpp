/*
Get Media Info
*/
#include "FfmpegIncludes.h"

#include "WXBase.h"

class WXChannelInfo {
public:
	int  m_iType = 0; //0 Audio 1 Video 2 Attach
	int m_iAudioBitrate = 0;
	int m_iAudioSampleRate = 0;;
	int m_iAudioChannel = 0;
	WXString m_strCodec = _T("");//Codec Name
	WXDataBuffer m_frame;// attach 的数据帧

	WXChannelInfo() {}
	~WXChannelInfo() {
		//SAFE_DELETE(m_frame);
	}
};

class WXMediaInfo {
public:
	WXString m_strFileName = _T("");
	int64_t m_iFileSize = 0;//File Size
	int64_t m_iFileTime = 0;//File Time
	WXString m_strFormat = _T("");

	//Video
	int  m_iVideoWidth = 0;//视频宽度
	int  m_iVideoHeight = 0;//视频高度
	int64_t m_iVideoBitrate = 0;//视频码率 bps
	int m_iVideoOrientation = 1;//视频图像旋转信息
	
	int m_iAudioChannels = 0;
	int m_iVideoChannels = 0;
	int m_iAttachChannels = 0;//属性，比如MP3的封面
	
	std::vector<WXChannelInfo*> m_arrInfo;

	AVRational m_sar;
	AVRational m_par;
	AVRational m_dar;
public:
	~WXMediaInfo() {
		if (m_arrInfo.size()) {
			for (int i = 0; i < m_arrInfo.size(); i++) {
				delete m_arrInfo[i];
			}
			m_arrInfo.clear();
		}
	}
};

WXDELOGO_CAPI void WXMediaInfoDestroy(void *p) {
	WXMediaInfo *info = (WXMediaInfo*)p;
	if (info) {
		delete info;
		p = nullptr;
	}
}

WXDELOGO_CAPI int  WXMediaInfoGetAudioChannelNumber(void *p) {
	WXMediaInfo *info = (WXMediaInfo*)p;
	if (info) {
		return info->m_iAudioChannels;
	}
	return 0;
}

WXDELOGO_CAPI int    WXMediaInfoGetAttachChannelNumber(void *p) {
	WXMediaInfo *info = (WXMediaInfo*)p;
	if (info) {
		return info->m_iAttachChannels;
	}
	return 0;
}

WXDELOGO_CAPI int   WXMediaInfoGetVideoChannelNumber(void *p) {
	WXMediaInfo *info = (WXMediaInfo*)p;
	if (info) {
		return info->m_iVideoChannels;
	}
	return 0;
}


static 	int gcd(int m, int n) {
	int r;
	while (n != 0)
	{
		r = (m >= n) ? (m - n) : m;
		m = n;
		n = r;
	}
	return m;
}
WXDELOGO_CAPI void*  WXMediaInfoCreate(WXCTSTR wszFileName, int *error) {
	WXString wxstr = wszFileName;
	struct stat st;
	int ret = stat(wxstr.c_str(), &st);

	AVFormatContext *ic = nullptr;
	int err = avformat_open_input(&ic, wxstr.c_str(), nullptr, nullptr);//打开文件
	if (err < 0) {
		*error = FFMPEG_ERROR_READFILE;
		return nullptr;
	}
	err = avformat_find_stream_info(ic, nullptr);
	if (err < 0) {
		avformat_close_input(&ic);
		*error = FFMPEG_ERROR_PARSE;
		return nullptr;
	}

	if (ic->nb_streams) {	
		WXMediaInfo *info = new WXMediaInfo;
		info->m_strFileName = wszFileName;
		info->m_iFileSize = st.st_size;
		info->m_iFileTime = ic->duration / 1000; //ms
		info->m_strFormat.Format("%s",ic->iformat->name);
		for (int i = 0; i < ic->nb_streams; i++) {
			if (ic->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
				WXChannelInfo *cinfo = new WXChannelInfo;
				
                if (ic->streams[i]->disposition & AV_DISPOSITION_ATTACHED_PIC) {
					cinfo->m_iType = 2;
					info->m_iAttachChannels++;
					AVPacket pkt = ic->streams[i]->attached_pic;
					if (pkt.size) {
						cinfo->m_frame.Init(pkt.data, pkt.size);
					}
				}else {
					cinfo->m_iType = 1;
					info->m_iVideoChannels++;
                    info->m_iVideoBitrate = ic->streams[i]->codec->bit_rate;//视频码率
					info->m_iVideoWidth  = ic->streams[i]->codec->width;
					info->m_iVideoHeight = ic->streams[i]->codec->height;

					/*获取视频旋转信息*/
					uint8_t* displaymatrix = av_stream_get_side_data(ic->streams[i], AV_PKT_DATA_DISPLAYMATRIX, NULL);				
					if (displaymatrix) {
						double theta = 0;
						theta = -av_display_rotation_get((int32_t*)displaymatrix);
						theta -= 360 * floor(theta / 360 + 0.9 / 360);

						if (fabs(theta - 90) < 1.0 || fabs(theta - 270) < 1.0) {//90,270
							info->m_iVideoWidth = ic->streams[i]->codec->height;
							info->m_iVideoHeight = ic->streams[i]->codec->width;
						}
					}

					int gwh = gcd(info->m_iVideoWidth, info->m_iVideoHeight);
					info->m_par.num = info->m_iVideoWidth/gwh;
					info->m_par.den = info->m_iVideoHeight/gwh;
					info->m_sar.num = ic->streams[i]->codec->sample_aspect_ratio.num;
					info->m_sar.den = ic->streams[i]->codec->sample_aspect_ratio.den;
					if (info->m_sar.num == 0 || info->m_sar.den == 0) {
						info->m_sar.num = 1;
						info->m_sar.den = 1;
					}
					info->m_dar.num = info->m_sar.num * info->m_par.num;
					info->m_dar.den = info->m_sar.den * info->m_par.den;
					int gdar = gcd(info->m_dar.num, info->m_dar.den);
					info->m_dar.num /= gdar;
					info->m_dar.den /= gdar;
				}

				

				AVCodecID codec_id = ic->streams[i]->codec->codec_id;
				AVCodec *codec = avcodec_find_decoder(codec_id);
				cinfo->m_strCodec.Format("%s",codec->long_name);				
				info->m_arrInfo.push_back(cinfo);

				/*
				jpeg图像提取图像的旋转信息
				*/
				if (codec_id == AV_CODEC_ID_MJPEG || codec_id == AV_CODEC_ID_MJPEGB ||
					codec_id == AV_CODEC_ID_LJPEG || codec_id == AV_CODEC_ID_JPEGLS ||
					codec_id == AV_CODEC_ID_JPEG2000) {
					AVCodecContext *codecctx = ic->streams[i]->codec;

					int res = avcodec_open2(codecctx, codec, NULL);
					if (res < 0) {
						continue;
					}
					AVPacket pkt = { 0 };
					AVFrame *picture = av_frame_alloc();

					while (av_read_frame(ic, &pkt) == 0)
					{
						if (pkt.stream_index == i)
						{
							int got_picture = 0;

							int bytes_used = avcodec_decode_video2(codecctx, picture, &got_picture, &pkt);
							if (got_picture)
							{
								AVDictionary *tags = picture->metadata;
								AVDictionaryEntry *tag = NULL;

								while ((tag = av_dict_get(tags, "", tag, AV_DICT_IGNORE_SUFFIX))) {
									if (strcmp(tag->key, "Orientation") == 0) {
										info->m_iVideoOrientation = atoi(tag->value);
										break;
									}
								}
								av_packet_unref(&pkt);
								break;
							}
						}

						av_packet_unref(&pkt);
					}
					av_free(picture);
					avcodec_close(codecctx);
				}

			}
			else if (ic->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
				WXChannelInfo * cinfo = new WXChannelInfo;
				cinfo->m_iType = 0;
				AVCodec *codec = avcodec_find_decoder(ic->streams[i]->codec->codec_id);
				cinfo->m_strCodec.Format("%s", codec->long_name);
				cinfo->m_iAudioBitrate = ic->streams[i]->codec->bit_rate;
				cinfo->m_iAudioSampleRate = ic->streams[i]->codec->sample_rate;
				cinfo->m_iAudioChannel = ic->streams[i]->codec->channels;
				info->m_iAudioChannels++;
				info->m_arrInfo.push_back(cinfo);
			}
		}
		avformat_close_input(&ic);
		*error = FFMPEG_ERROR_OK;
		return (void*)info;
	}
	*error = FFMPEG_ERROR_NO_MEIDADATA;
	return nullptr;
	*error = FFMPEG_ERROR_NOFILE;
	return nullptr;
}


WXDELOGO_CAPI int WXMediaInfoChannelGetType(void *p, int index) {
	WXMediaInfo *info = (WXMediaInfo*)p;
	if (info) {
		return info->m_arrInfo[index]->m_iType;
	}
	return 0;
}


WXDELOGO_CAPI WXCTSTR  WXMediaInfoChannelCodec(void *p, int index) {
	WXMediaInfo *info = (WXMediaInfo*)p;
	if (info) {
		return info->m_arrInfo[index]->m_strCodec.str();
	}
	return nullptr;
}

WXDELOGO_CAPI int WXMediaInfoChannelAudioBitrate(void *p, int index) {
	WXMediaInfo *info = (WXMediaInfo*)p;
	if (info) {
		return info->m_arrInfo[index]->m_iAudioBitrate;
	}
	return 0;
}

WXDELOGO_CAPI int WXMediaInfoChannelAudioSampleRate(void *p, int index) {
	WXMediaInfo *info = (WXMediaInfo*)p;
	if (info) {
		return info->m_arrInfo[index]->m_iAudioSampleRate;
	}
	return 0;
}

WXDELOGO_CAPI int WXMediaInfoChannelAudioChanels(void *p, int index) {
	WXMediaInfo *info = (WXMediaInfo*)p;
	if (info) {
		return info->m_arrInfo[index]->m_iAudioChannel;
	}
	return 0;
}

WXDELOGO_CAPI int WXMediaInfoGetVideoWidth(void *p) {
	WXMediaInfo *info = (WXMediaInfo*)p;
	if (info) {
		return info->m_iVideoWidth;
	}
	return 0;
}

WXDELOGO_CAPI int WXMediaInfoGetVideoHeight(void *p) {
	WXMediaInfo *info = (WXMediaInfo*)p;
	if (info) {
		return info->m_iVideoHeight;
	}
	return 0;
}

WXDELOGO_CAPI int      WXMediaInfoGetVideoDisplayRatioWidth(void *p) {
	WXMediaInfo *info = (WXMediaInfo*)p;
	if (info) {
		return info->m_dar.num;
	}
	return 0;
}

WXDELOGO_CAPI int      WXMediaInfoGetVideoDisplayRatioHeight(void *p) {
	WXMediaInfo *info = (WXMediaInfo*)p;
	if (info) {
		return info->m_dar.den;
	}
	return 0;
}

WXDELOGO_CAPI int64_t WXMediaInfoGetFileSize(void *p) {
	WXMediaInfo *info = (WXMediaInfo*)p;
	if (info) {
		return info->m_iFileSize;
	}
	return 0;
}

WXDELOGO_CAPI int64_t WXMediaInfoGetFileDuration(void *p) {
	WXMediaInfo *info = (WXMediaInfo*)p;
	if (info) {
		return info->m_iFileTime;
	}
	return 0;
}

WXDELOGO_CAPI int64_t WXMediaInfoGetVideoBitrate(void *p) {
	WXMediaInfo *info = (WXMediaInfo*)p;
	if (info) {
		return info->m_iVideoBitrate;
	}
	return 0;
}

WXDELOGO_CAPI int      WXMediaInfoGetVideoOrientation(void *p)
{
	WXMediaInfo *info = (WXMediaInfo*)p;
	if (info) {
		return info->m_iVideoOrientation;
	}
	return 0;
}

WXDELOGO_CAPI WXCTSTR  WXMediaInfoGetFormat(void *p) {
	WXMediaInfo *info = (WXMediaInfo*)p;
	if (info) {
		return info->m_strFormat.str();
	}
	return nullptr;
}


WXDELOGO_CAPI int WXMediaInfoGetChannelNumber(void *p) {
	WXMediaInfo *info = (WXMediaInfo*)p;
	if (info) {
		return (int)info->m_arrInfo.size();
	}
	return 0;
}

////有的ret返回0，但是没有生成对应文件！！
//WXDELOGO_CAPI int   WXMediaInfoGetPicture(void *p, WXCTSTR wszFileName) {
//	WXString wxstr2 = wszFileName;
//	WXMediaInfo *info = (WXMediaInfo*)p;
//	if (info) {
//		const char *szSrc = info->m_strFileName.c_str();
//		WXCtx *ffmpeg = avffmpeg_create();
//		int argc = 8;
//		char *argv[8] = { "ffmpeg","-ss", "0", "-i", (char*)szSrc, "-vframes", "1", (char*)wxstr2.c_str() };
//		int ret = avffmpeg_convert(ffmpeg, argc, argv);
//		avffmpeg_destroy(ffmpeg);
//
//		//WXLogWriteNew("WXMediaInfoGetPicture Param");
//		for (int i = 0; i < argc; i++) {
//			WXLogWriteNew("argv[%d] = %s", i, argv[i]);
//		}
//		WXString wxstr = WXFfmpegGetError(ret);
//		WXLogWriteNew("WXMediaInfoGetPicture Result = %s", wxstr.c_str());
//		return ret;
//	}
//	return -1;
//}

////attach MJPG , 获得MP3 专辑封面的 JPG内存数据
//WXDELOGO_CAPI int WXMediaInfoChannelGetAttachSize(void *p, int index) {
//	WXMediaInfo *info = (WXMediaInfo*)p;
//	if (info && info->m_arrInfo[index]->m_iType == 2) {
//		return info->m_arrInfo[index]->m_frame->m_iBufSize;
//	}
//	return 0;
//}
//
//WXDELOGO_CAPI uint8_t*       WXMediaInfoChannelGetAttachData(void *p, int index) {
//	WXMediaInfo *info = (WXMediaInfo*)p;
//	if (info && info->m_arrInfo[index]->m_iType == 2) {
//		return info->m_arrInfo[index]->m_frame->m_pBuf;
//	}
//	return nullptr;
//}
//
//WXDELOGO_CAPI int WXMediaInfoGetAudioPitcutre(void *p, WXCTSTR strName) {
//	WXMediaInfo *info = (WXMediaInfo*)p;
//	if (info) {
//		for (int index = 0; index < info->m_arrInfo.size(); index++) {
//			if (info->m_arrInfo[index]->m_iType == 2) {
//				WXString str = strName;
//				FILE *f = fopen(str.c_str(), "wb");
//				if (f) {
//					fwrite(info->m_arrInfo[index]->m_frame->m_pBuf, info->m_arrInfo[index]->m_frame->m_iBufSize, 1, f);
//					fclose(f);
//				}
//				return 1;
//			}
//		}
//	}
//	return 0;
//}
