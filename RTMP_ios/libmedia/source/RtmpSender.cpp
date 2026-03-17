//
//  RtmpSender.h
//  media
//
//  Created by momo on 2021/7/3.
//  Copyright © 2021 TenXie. All rights reserved.
//

#ifndef _RTMP_SENDER_H_
#define _RTMP_SENDER_H_

#include "WXBase.h"
#include <libyuv.h>
#include <VideoToolbox/VideoToolbox.h>

#include "../libfaac/faac.h"
#include "../librtmp/librtmp/rtmp.h"
extern "C" {
#include "../x264/x264.h"
};


#define SAVC(x)    static const AVal av_ ## x = AVC(#x)

static const AVal av_setDataFrame = AVC("@setDataFrame");
static const AVal av_SDKVersion = AVC("LFLiveKit 2.4.0");
SAVC(onMetaData);
SAVC(duration);
SAVC(width);
SAVC(height);
SAVC(videocodecid);
SAVC(videodatarate);
SAVC(framerate);
SAVC(audiocodecid);
SAVC(audiodatarate);
SAVC(audiosamplerate);
SAVC(audiosamplesize);
//SAVC(audiochannels);
SAVC(stereo);
SAVC(encoder);
//SAVC(av_stereo);
SAVC(fileSize);
SAVC(avc1);
SAVC(mp4a);

class RTMP_Sender
{
    
    class HWEncoder{
        uint32_t m_startTime = 0;//shiajincuo
        VTCompressionSessionRef m_session = nullptr;
        int m_iWidth = 0;
        int m_iHeight = 0;
        WXDataBuffer m_extra;
        WXDataBuffer m_out;
        RTMP_Sender *m_sender = nullptr;
        int64_t m_ts = 0;
    public:
        
        void didH264(void *frame, OSStatus status, VTEncodeInfoFlags flags,CMSampleBufferRef sBuf){
            
            //判断是否关键帧
            bool keyframe = !CFDictionaryContainsKey((CFDictionaryRef)(CFArrayGetValueAtIndex(CMSampleBufferGetSampleAttachmentsArray(sBuf, true), 0)),kCMSampleAttachmentKey_NotSync);
            if (keyframe){
                if(m_extra.m_pBuf == nullptr){
                    CMFormatDescriptionRef format = CMSampleBufferGetFormatDescription(sBuf);
                    size_t iSPS = 0, sps_count = 0;
                    const uint8_t *pSPS;
                    OSStatus statusSPS = CMVideoFormatDescriptionGetH264ParameterSetAtIndex(format, 0, &pSPS, &iSPS, &sps_count, 0 );
                    
                    size_t iPPS = 0, pps_count = 0;
                    const uint8_t *pPPS;
                    OSStatus statusPPS = CMVideoFormatDescriptionGetH264ParameterSetAtIndex(format, 1, &pPPS, &iPPS, &pps_count, 0 );
                    if (statusSPS == noErr && statusPPS == noErr){
                        printf("sps = %d, pps=%d", (int)iSPS,(int)iPPS);
                        int extrasize = (int)(iSPS + iPPS + 16);
                        m_extra.Init(NULL, extrasize);
                        m_extra.m_pBuf[0] = 0x17;
                        m_extra.m_pBuf[1] = 0x00;//≤Œ ˝÷°
                        m_extra.m_pBuf[2] = 0x00;
                        m_extra.m_pBuf[3] = 0x00;
                        m_extra.m_pBuf[4] = 0x00;
                        
                        m_extra.m_pBuf[5] = 0x01;
                        m_extra.m_pBuf[6] = pSPS[1];
                        m_extra.m_pBuf[7] = pSPS[2];
                        m_extra.m_pBuf[8] = pSPS[3];
                        m_extra.m_pBuf[9] = 0xff;
                        m_extra.m_pBuf[10] = 0xe1;
                        
                        m_extra.m_pBuf[11] = (iSPS >> 8) & 0xff;
                        m_extra.m_pBuf[12] = iSPS & 0xff;
                        memcpy( m_extra.m_pBuf + 13, pSPS, iSPS);
                        
                        m_extra.m_pBuf[13 + iSPS] = 0x01;
                        
                        m_extra.m_pBuf[14 + iSPS] = (iPPS >> 8) & 0xff;
                        m_extra.m_pBuf[15 + iSPS] = iPPS & 0xff;
                        memcpy( m_extra.m_pBuf + 16 + iSPS, pPPS, iPPS);
                    }
                }
                if(m_extra.m_pBuf != nullptr){
                    m_sender->SendData(m_extra.m_pBuf, m_extra.m_iBufSize,0,true);
                    //printf("HW 264 Encoder Send Extra\n");
                }
            }
            
            //Send H264 MP4 Format Data
            CMBlockBufferRef dataBuffer = CMSampleBufferGetDataBuffer(sBuf);
            size_t data_size = 0, total_size = 0;
            char *pData=NULL;
            OSStatus statusCodeRet = CMBlockBufferGetDataPointer(dataBuffer, 0, &data_size, &total_size, &pData);
            if (statusCodeRet == noErr) {
                m_out.m_pBuf[0] = keyframe ? 0x17 : 0x27;
                m_out.m_pBuf[1] = 0x01;
                m_out.m_pBuf[2] = 0x00;
                m_out.m_pBuf[3] = 0x00;
                m_out.m_pBuf[4] = 0x00;
                memcpy(m_out.m_pBuf + 5, pData, data_size);
                int size = (int)data_size + 5;
                m_sender->SendData(m_out.m_pBuf, size, (int)m_ts,true);
            }
            
        }
        //回调函数!!
        //硬编码回调数据接口函数
        //
        static void didCompressH264(void *ctx, void *frame, OSStatus status, VTEncodeInfoFlags flags,CMSampleBufferRef sBuf){
            //编码输出
            if (status != noErr)
                return;
            HWEncoder* d = (HWEncoder*)ctx;
            d->didH264(frame, status, flags, sBuf);
        }
        bool Open(int iWidth, int iHeight, RTMP_Sender *sender){
            m_iWidth  = iWidth;
            m_iHeight = iHeight;
            int bitrate = iWidth * iHeight * 24 * 12 / 200;
            OSStatus status = VTCompressionSessionCreate(NULL, m_iWidth, m_iHeight, kCMVideoCodecType_H264, NULL, NULL, NULL, didCompressH264, this,  &m_session);//make x264_param_t
            status = VTSessionSetProperty(m_session, kVTCompressionPropertyKey_RealTime, kCFBooleanTrue);//Realtime
            status = VTSessionSetProperty(m_session, kVTCompressionPropertyKey_AllowFrameReordering, kCFBooleanFalse);
            
            status = VTSessionSetProperty(m_session, kVTCompressionPropertyKey_ProfileLevel, kVTProfileLevel_H264_Main_AutoLevel);
            
            
            SInt32 _fps = 24;
            CFNumberRef CFfps= CFNumberCreate(NULL,kCFNumberSInt32Type,&_fps);
            status = VTSessionSetProperty(m_session, kVTCompressionPropertyKey_ExpectedFrameRate,CFfps);
            
            SInt32 _bitrate = bitrate ;
            CFNumberRef CFBitrate= CFNumberCreate(NULL,kCFNumberSInt32Type,&_bitrate);
            status = VTSessionSetProperty(m_session, kVTCompressionPropertyKey_AverageBitRate,CFBitrate);
            
            SInt32 keymax = 24 * 3;
            CFNumberRef CFkeymax = CFNumberCreate(NULL,kCFNumberSInt32Type,&keymax);
            status = VTSessionSetProperty(m_session, kVTCompressionPropertyKey_MaxKeyFrameInterval,CFkeymax);
           
            status = VTCompressionSessionPrepareToEncodeFrames(m_session);//Try to encode
            
            if(status != noErr){
                return false;
            }
            m_out.Init(NULL, iWidth * iHeight);
            m_startTime = RTMP_GetTime();
            m_sender = sender;
            return true;
        }
        void Close(){
            if (m_session) {
                VTCompressionSessionInvalidate(m_session);
                CFRelease(m_session);
                m_session = NULL;
            }
        }
        
        void Encode(uint8_t *buf, int bI420, int64_t ts){
            CVPixelBufferPoolRef pixel_buffer_pool = VTCompressionSessionGetPixelBufferPool(m_session);
            CVPixelBufferRef pixel_buffer = NULL;
            CVPixelBufferPoolCreatePixelBuffer(NULL, pixel_buffer_pool,&pixel_buffer);
            if(pixel_buffer){
                CVPixelBufferLockBaseAddress(pixel_buffer, 0);
                uint8_t* pY = (uint8_t*)CVPixelBufferGetBaseAddressOfPlane(pixel_buffer, 0);
                int PitchY = (int)CVPixelBufferGetBytesPerRowOfPlane(pixel_buffer, 0);
                uint8_t* pUV = (uint8_t*)CVPixelBufferGetBaseAddressOfPlane(pixel_buffer, 1);
                int PitchUV = (int)CVPixelBufferGetBytesPerRowOfPlane(pixel_buffer, 1);
                // Convert I420 to NV12
                if(bI420){
                    libyuv::I420ToNV12(buf,m_iWidth,
                                       buf + m_iWidth * m_iHeight,m_iWidth/2,
                                       buf + m_iWidth * m_iHeight*5/4,m_iWidth/2,
                                       pY, PitchY,
                                       pUV, PitchUV,
                                       m_iWidth,m_iHeight);
                }else{
                    libyuv::ARGBToNV12(buf,m_iWidth*4,
                                       pY, PitchY,
                                       pUV, PitchUV,
                                       m_iWidth,m_iHeight);
                }

                CVPixelBufferUnlockBaseAddress(pixel_buffer, 0);
                VTEncodeInfoFlags flags = 0;
                m_ts = ts;
                VTCompressionSessionEncodeFrame(m_session,pixel_buffer,//m_pic
                                                CMTimeMake(ts, 1000),//time
                                                kCMTimeInvalid,
                                                NULL,
                                                NULL,
                                                &flags);//数据塞到编码器
                CVBufferRelease(pixel_buffer);
            }
            
        }
    };


    class AVCEncoder{
    private:
        x264_t *m_h = NULL;
        x264_param_t m_param;
        x264_picture_t m_pic;
        int m_iWidth = 0;
        int m_iHeight = 0;
        int m_iFps = 24;
        int64_t m_nFrame = 0;
        WXDataBuffer m_out;
        WXDataBuffer m_extra;
        RTMP_Sender *m_sender = nullptr;
        
    public:
        bool Open(int iWidth, int iHeight,RTMP_Sender *sender){
            x264_param_default(&m_param);
            m_param.i_threads = 1;
            m_param.i_frame_total = 0;
            m_param.i_keyint_max = 100;//3 * iFps;//-i 100
            m_iWidth = m_param.i_width = iWidth;
            m_iHeight = m_param.i_height = iHeight;
            m_param.rc.i_qp_min = 10;
            m_param.rc.i_qp_max = 51;
            m_param.i_csp = X264_CSP_I420;//
            m_param.rc.i_lookahead = 0;
            m_param.i_sync_lookahead = 0;
            m_param.i_bframe = 0;
            m_param.b_sliced_threads = 1;
            m_param.b_vfr_input = 0;
            m_param.rc.b_mb_tree = 0;
            m_param.i_frame_reference = 1;//
            m_param.analyse.b_transform_8x8 = 0;
            m_param.b_cabac = 0;//0;
            m_param.i_cqm_preset = X264_CQM_FLAT;
            m_param.psz_cqm_file = NULL;
            m_param.i_bframe = 0;
            m_param.analyse.i_weighted_pred = X264_WEIGHTP_NONE;
            m_param.b_interlaced = 0;
            m_param.b_fake_interlaced = 0;
            m_param.rc.i_rc_method = X264_RC_CQP;
            m_param.analyse.i_me_method = X264_ME_DIA;//-me dia
            m_param.analyse.i_subpel_refine = 2;//-m --subme 2
            m_param.rc.i_qp_constant = 25;
            m_param.b_repeat_headers = 0;//√ø∏ˆπÿº¸÷°”–SPS PPS
            m_param.b_annexb = 0;//≤ª « 00 00 00 01 ªÚ’ﬂ 00 00 01 Õ∑
            m_h = x264_encoder_open(&m_param);
            if (m_h == NULL) {
                return false;
            }
            m_pic.img.plane[0] = NULL;
            m_pic.img.plane[1] = NULL;
            m_pic.img.plane[2] = NULL;
            m_pic.img.plane[3] = NULL;
            x264_picture_alloc(&m_pic, X264_CSP_I420, iWidth, iHeight);
            x264_nal_t *nal = NULL;
            int nnal = 0;
            u_char *pSPS = NULL;
            int iSPS = 0;
            u_char *pPPS = NULL;
            int iPPS = 0;
            x264_encoder_headers(m_h, &nal, &nnal);
            for (int i = 0; i < nnal; i++){
                if (nal[i].i_type == 7) {
                    iSPS = nal[i].i_payload - 4;
                    pSPS = nal[i].p_payload + 4;
                }
                if (nal[i].i_type == 8){
                    iPPS = nal[i].i_payload - 4;
                    pPPS = nal[i].p_payload + 4;
                }
            }
            
            int extrasize = iSPS + iPPS + 16;
            m_extra.Init(NULL, extrasize);
            m_extra.m_pBuf[0] = 0x17;
            m_extra.m_pBuf[1] = 0x00;//≤Œ ˝÷°
            m_extra.m_pBuf[2] = 0x00;
            m_extra.m_pBuf[3] = 0x00;
            m_extra.m_pBuf[4] = 0x00;
            m_extra.m_pBuf[5] = 0x01;
            m_extra.m_pBuf[6] = pSPS[1];
            m_extra.m_pBuf[7] = pSPS[2];
            m_extra.m_pBuf[8] = pSPS[3];
            m_extra.m_pBuf[9] = 0xff;
            m_extra.m_pBuf[10] = 0xe1;
            m_extra.m_pBuf[11] = (iSPS >> 8) & 0xff;
            m_extra.m_pBuf[12] = iSPS & 0xff;
            memcpy( m_extra.m_pBuf + 13, pSPS, iSPS);
            m_extra.m_pBuf[13 + iSPS] = 1;
            m_extra.m_pBuf[14 + iSPS] = (iPPS >> 8) & 0xff;
            m_extra.m_pBuf[15 + iSPS] = iPPS & 0xff;
            memcpy( m_extra.m_pBuf + 16 + iSPS, pPPS, iPPS);
            m_sender = sender;
            m_out.Init(NULL, iWidth * iHeight);
            return true;
        }

        void Encode(uint8_t *buf, int bI420, uint32_t ts) {

            if (!m_h)return;
            x264_nal_t *nal= NULL;
            int i_nal = 0;
            x264_picture_t pic_out;
            if(bI420){
                int imgsize = m_iWidth * m_iHeight;
                libyuv::I420Copy(
                           buf, m_iWidth,
                           buf + imgsize, m_iWidth/2,
                           buf + imgsize * 5 / 4, m_iWidth/2,
                    m_pic.img.plane[0], m_pic.img.i_stride[0],
                    m_pic.img.plane[1], m_pic.img.i_stride[1],
                    m_pic.img.plane[2], m_pic.img.i_stride[2],
                    m_iWidth,m_iHeight
                );
            }else{
                libyuv::ARGBToI420(
                           buf, m_iWidth*4,
                    m_pic.img.plane[0], m_pic.img.i_stride[0],
                    m_pic.img.plane[1], m_pic.img.i_stride[1],
                    m_pic.img.plane[2], m_pic.img.i_stride[2],
                    m_iWidth,m_iHeight
                );
            }

            int i_frame_size = x264_encoder_encode(m_h, &nal, &i_nal, &m_pic, &pic_out);//±‡¬Îπ˝≥Ã
            if (i_frame_size < 0)return;

            //FLV ˝æ›∞¸
            int bKeyFrame = pic_out.i_type == X264_TYPE_IDR;
            if(bKeyFrame){
                m_sender->SendData(m_extra.m_pBuf, m_extra.m_iBufSize,0,true);
               // printf("SendH264 Exrta\n");
            }
            m_out.m_pBuf[0] = bKeyFrame ? 0x17 : 0x27;
            m_out.m_pBuf[1] = 0x01;// ˝æ›÷°
            m_out.m_pBuf[2] = 0x00;
            m_out.m_pBuf[3] = 0x00;
            m_out.m_pBuf[4] = 0x00;//ƒ¨»œµƒ3∏ˆ◊÷Ω⁄µƒ0
            memcpy(m_out.m_pBuf + 5, nal[0].p_payload, i_frame_size);
            m_sender->SendData(m_out.m_pBuf, i_frame_size + 5,ts,true);
        }

        void Close(){
            if (m_h){
                x264_picture_clean(&m_pic);
                x264_encoder_close(m_h);
                m_h = NULL;
            }
        }
    };

    class AACEncoder {
    private:
        WXLocker m_mutex;
        faacEncHandle m_h = NULL;
        unsigned long m_nMax = 0;
        unsigned long m_samplesInput = 0;
        int m_samplerate = 48000;
        int m_channel = 2;
        WXFifo m_fifo;
        WXDataBuffer m_out;
        WXDataBuffer m_buf;
        WXDataBuffer m_extra;
        int64_t m_nFrame = 0;
        RTMP_Sender *m_sender = nullptr;
    private:
        int GetSRIndex(unsigned int sampleRate){
            if (92017 <= sampleRate) return 0;
            if (75132 <= sampleRate) return 1;
            if (55426 <= sampleRate) return 2;
            if (46009 <= sampleRate) return 3;
            if (37566 <= sampleRate) return 4;
            if (27713 <= sampleRate) return 5;
            if (23004 <= sampleRate) return 6;
            if (18783 <= sampleRate) return 7;
            if (13856 <= sampleRate) return 8;
            if (11502 <= sampleRate) return 9;
            if (9391 <= sampleRate) return 10;
            return 11;
        }

    public:
        int Open(int samplerate, int channels, RTMP_Sender *sender){
            WXAutoLock al(m_mutex);
            m_h = faacEncOpen(samplerate, channels, &m_samplesInput, &m_nMax);
            if (m_h == NULL){
                return NULL;
            }
            m_samplerate = samplerate;
            m_channel = channels;
            m_out.Init(NULL, (int)m_nMax);
            faacEncConfigurationPtr cfg = faacEncGetCurrentConfiguration(m_h);
            cfg->aacObjectType = LOW;
            cfg->mpegVersion = MPEG4;
            cfg->useTns = 0;
            cfg->allowMidside = 1;
            cfg->bandWidth = samplerate / 2;
            cfg->bitRate = 96 * 1000;
            cfg->quantqual = 80;
            cfg->outputFormat = 0;
            cfg->inputFormat = FAAC_INPUT_16BIT;
            if (!faacEncSetConfiguration(m_h, cfg)){
                return NULL;
            }
            m_fifo.Init(192000);
            m_buf.Init(NULL, 2048 * m_channel);
            int SRindex = GetSRIndex(samplerate);
            short extra = (cfg->aacObjectType << 11) | (SRindex << 7) | (m_channel << 3);
            m_sender = sender;
            m_extra.Init(NULL, 4);
            m_extra.m_pBuf[0] = 0xAF;
            m_extra.m_pBuf[1] = 0;
            m_extra.m_pBuf[2] = (uint8_t)(extra >> 8);
            m_extra.m_pBuf[3] = (uint8_t)extra;
            return (int)m_samplesInput;
        }

        void  Encode(uint8_t *pbuf,int buf_size) {
            WXAutoLock al(m_mutex);
            if (!m_h)return;
            m_fifo.Write(pbuf, buf_size);
            while (m_fifo.Size() > m_channel * 2048) {
                int64_t ts = m_nFrame * 1000 * 1024 / m_samplerate;
                m_nFrame++;
                if(m_nFrame% 100 == 0){
                    m_sender->SendData(m_extra.m_pBuf, 4, 0, false);
                }
                m_fifo.Read(m_buf.m_pBuf, m_channel * 2048);
                
                m_out.m_pBuf[0] = 0xAF;
                m_out.m_pBuf[1] = 1;
                int encsize = (int)faacEncEncode(m_h, (int *)m_buf.m_pBuf, m_samplesInput, m_out.m_pBuf + 2, m_nMax);
                if(encsize > 0){
                    //Write AAC Data
                    m_sender->SendData(m_out.m_pBuf, encsize + 2, (int)ts, false);
                }
            }
        }
        void Close() {
            WXAutoLock al(m_mutex);
            if (m_h){
                faacEncClose(m_h);
                m_h = NULL;
            }
        }
    };

    WXLocker m_mutex;
    RTMP *m_r = NULL;
    uint32 m_ptsStart = 0;
    AVCEncoder m_avc;
    //  HWEncoder m_avc;
    AACEncoder m_aac;
    int m_nSampleRate = 48000;
    void SetMetaData(int width, int height, int sample_rate,int channel){
        
        RTMPPacket packet;
        RTMPPacket_Reset(&packet);
        memset(&packet, 0, sizeof(RTMPPacket));
        char pbuf[2048], *pend = pbuf + sizeof(pbuf);
        packet.m_nChannel = 0x03;                   // control channel (invoke)
        packet.m_headerType = RTMP_PACKET_SIZE_MEDIUM;
        packet.m_packetType = RTMP_PACKET_TYPE_INFO;
        packet.m_nTimeStamp = 0;
        packet.m_nInfoField2 = m_r->m_stream_id;
        packet.m_hasAbsTimestamp = TRUE;
        packet.m_body = pbuf + RTMP_MAX_HEADER_SIZE;

        char *enc = packet.m_body;
        enc = AMF_EncodeString(enc, pend, &av_setDataFrame);
        enc = AMF_EncodeString(enc, pend, &av_onMetaData);

        *enc++ = AMF_OBJECT;

        enc = AMF_EncodeNamedNumber(enc, pend, &av_duration, 0.0);
        enc = AMF_EncodeNamedNumber(enc, pend, &av_fileSize, 0.0);

        // videosize
        enc = AMF_EncodeNamedNumber(enc, pend, &av_width, width);
        enc = AMF_EncodeNamedNumber(enc, pend, &av_height, height);

        // video
        enc = AMF_EncodeNamedString(enc, pend, &av_videocodecid, &av_avc1);

        enc = AMF_EncodeNamedNumber(enc, pend, &av_videodatarate, 1000.0);
        enc = AMF_EncodeNamedNumber(enc, pend, &av_framerate, 25.0);

        // audio
        enc = AMF_EncodeNamedString(enc, pend, &av_audiocodecid, &av_mp4a);
        enc = AMF_EncodeNamedNumber(enc, pend, &av_audiodatarate, 96.0);

        enc = AMF_EncodeNamedNumber(enc, pend, &av_audiosamplerate, sample_rate);
        enc = AMF_EncodeNamedNumber(enc, pend, &av_audiosamplesize, 16.0);
        enc = AMF_EncodeNamedBoolean(enc, pend, &av_stereo, channel == 2);

        // sdk version
        enc = AMF_EncodeNamedString(enc, pend, &av_encoder, &av_SDKVersion);

        *enc++ = 0;
        *enc++ = 0;
        *enc++ = AMF_OBJECT_END;

        packet.m_nBodySize = (uint32_t)(enc - packet.m_body);
        RTMP_SendPacket(m_r, &packet, 0);
    }
public:
    void SendData(u_char *buf, int buf_size, unsigned int dwTime = 0, bool bVideo = true){
        WXAutoLock al(m_mutex);
        RTMPPacket packet;
        RTMPPacket_Reset(&packet);
        memset(&packet, 0, sizeof(RTMPPacket));
        packet.m_nChannel = 0x04;
        packet.m_headerType = bVideo ? RTMP_PACKET_SIZE_LARGE : RTMP_PACKET_SIZE_MEDIUM;
        packet.m_nTimeStamp = 0;
        packet.m_nInfoField2 = m_r->m_stream_id;
        packet.m_hasAbsTimestamp = 0;
        packet.m_packetType = bVideo ? RTMP_PACKET_TYPE_VIDEO : RTMP_PACKET_TYPE_AUDIO;   /* VIDEO */
        packet.m_body = (char*)buf;
        packet.m_nBodySize = buf_size;
        packet.m_nTimeStamp = dwTime;
        RTMP_SendPacket(m_r, &packet, 0);
    }
    RTMP_Sender(){
        m_r = NULL;
    }
    virtual ~RTMP_Sender() {
        Close();
    }
    bool Open(const char *URL,int width, int height, int sampleRate, int channel){
        WXAutoLock al(m_mutex);
        m_r = RTMP_Alloc();
        RTMP_Init(m_r);
        int err = RTMP_SetupURL(m_r, (char*)URL);
        if (err <= 0)return false;
        RTMP_EnableWrite(m_r);
        err = RTMP_Connect(m_r, NULL);
        if (err <= 0)return false;
        err = RTMP_ConnectStream(m_r, 0);
        if (err <= 0)return false;
        m_ptsStart = RTMP_GetTime();
        m_avc.Open(width,height, this);
        m_aac.Open(sampleRate,channel, this);
        
        SetMetaData(width,height,sampleRate,channel);
        return true;
    }

    void Close() {
        WXAutoLock al(m_mutex);
        if (m_r) {
            RTMP_Close(m_r);
            RTMP_Free(m_r);
            m_r = NULL;
            m_aac.Close();
            m_avc.Close();
        }
    }
    
    void  SendI420(uint8_t* buf){
        uint32_t ts = RTMP_GetTime() - m_ptsStart;
        m_avc.Encode(buf, 1, ts);
    }
    void  SendRGB32(uint8_t* buf){
        uint32_t ts = RTMP_GetTime() - m_ptsStart;
        m_avc.Encode(buf,0, ts);
    }
    void  SendPcm(uint8_t* buf, int buf_size){
        m_aac.Encode(buf, buf_size);
    }
};


void* RtmpSenderCreate(const char* szURL, int width, int height, int SampleRate, int channel){
    RTMP_Sender *obj = new RTMP_Sender;
    bool bRet = obj->Open(szURL, width, height, SampleRate, channel);
    if(bRet){
        return (void*)obj;
    }
    delete obj;
    return nullptr;
}
void  RtmpSenderSendI420(void* ptr,  uint8_t *buf){
    RTMP_Sender *obj = (RTMP_Sender*)ptr;
    obj->SendI420(buf);
}
void  RtmpSenderSendRGB32(void* ptr, uint8_t *buf){
    RTMP_Sender *obj = (RTMP_Sender*)ptr;
    obj->SendRGB32(buf);
}
void  RtmpSenderSendPcm(void* ptr,   uint8_t* buf, int buf_size){
    RTMP_Sender *obj = (RTMP_Sender*)ptr;
    obj->SendPcm(buf,buf_size);
}
void  RtmpSenderDestroy(void* ptr){
    RTMP_Sender *obj = (RTMP_Sender*)ptr;
    obj->Close();
    delete obj;
}

#endif
