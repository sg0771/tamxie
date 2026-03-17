package com.apowersoft.WXMedia;

public class AacDecoder {

    static {
        System.loadLibrary("AacDecoder");
    }
    public native long  Open(byte []data, int size);
    public native long  GetSampleRate(long handle);
    public native long  GetChannel(long handle);
    public native void  Decode(long handle,  byte []data, int size, byte []out_data);
    public native void  Close(long handle);
}
