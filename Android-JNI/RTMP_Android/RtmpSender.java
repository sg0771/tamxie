package com.apowersoft.WXMedia;

public class RtmpSender {

    static {
        System.loadLibrary("wxrtmp");
    }
    //----------------------NDK API ----------------------------
   public native void SetVideoBitrate(int bitrate);//设置默认视频码率，底层默认1200
   public native void SetAudioBitrate(int bitrate);//设置默认音频码率，底层默认96
   public native void SetMetadata(int bSetMeta, int width, int height, int nVideoBitrate,int nAudioBitrate);//设置Metadata信息，如果设置，参数帧就使用预设数据，如果取消设置，所有参数填0
   public native void SetMetadata2(int bSetMeta, int width, int height, int fps, int nVideoBitrate,int nAudioBitrate);//设置Metadata信息，如果设置，参数帧就使用预设数据，如果取消设置，所有参数填0
   public native int    IsRunning();

   public native long Connect(String strURL, int width, int height, int sample_rate. int  channel);
   public native void SendNV21(long handle, byte []date, int width, int height);
   public native void ClipNV21(long handle, byte []date, int src_width, int src_height, int clip_width, int clip_height);//裁剪编码
   public native void SendPCM(long handle, byte []dat, int data_size);
   public native void Disconnect(long handle);

}
