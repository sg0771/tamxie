package com.apowersoft.WXMedia;

public class TcpClient {

    static {
        System.loadLibrary("WXTcp");
    }
    //----------------------NDK API ----------------------------
    public native long  Init(String strIP, int nTcpPort);
    public native void  WriteH264(long handle,  byte []data, int size);
    public native void  WriteH265(long handle,  byte []data, int size);
    public native void  WriteAudioConfig(long handle,  int nSampleRate, int nChannel);
    public native void  WriteAAC(long handle,    byte []data, int size);   
    public native void  WriteOpus(long handle,  byte []data, int size);
    public native void  Deinit(long handle);
    private  long           m_handle = 0;

    //------------------------- Java API --------------------------
    public boolean Start(String strIP, int nTcpPort){
        m_handle = Start(strIP, nTcpPort);
        return m_handle != 0;
    }
    
    public void Stop(){
        if(m_handle != 0){
            Stop(m_handle);
            m_handle = 0;
        }
    }
    
    public void WriteH264(byte []data, int size){
        if(m_handle != 0){
            WriteH264(m_handle, 1, data, size);
        }
    }
    public void WriteH265(byte []data, int size){
        if(m_handle != 0){
            WriteH265(m_handle, 1, data, size);
        }
    }
    public void WriteAudioConfig(int nSampleRate, int nChannel){
        if(m_handle != 0){
            WriteAudioConfig(m_handle, 0,  nSampleRate, nChannel);
        }
    }
    public void WriteAAC(byte []data, int size){
        if(m_handle != 0){
            WriteAAC(m_handle, 0, data, size);
        }
    }
        public void WriteOpus(byte []data, int size){
        if(m_handle != 0){
            WriteOpus(m_handle, 0, data, size);
        }
    }
}
