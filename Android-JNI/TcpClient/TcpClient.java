package com.apowersoft.WXMedia;

public class TcpClient {

    static {
        System.loadLibrary("TcpClient");
    }
    public native long  Start(String strIP, int nTcpPort);
    public native void  SendH264(long handle,  byte []data, int size);
    public native void  SendH265(long handle,  byte []data, int size);
    public native void  SendAAC(long handle,  byte []data, int size);
    public native void  Stop(long handle);

    private  long  m_handle = 0;

    //--------------------------------------------------------------
    public boolean Connect(String strIP, int nTcpPort){
        m_handle = Start(strIP, nTcpPort);
        return m_handle != 0;
    }
    public void Disconncet(){
        if(m_handle != 0){
            Stop(m_handle);
            m_handle = 0;
        }
    }
    public void WriteH264(byte []data, int size){
        if(m_handle != 0){
            SendH264(m_handle, bTcp,  bVideo, data, size);
        }
    }
    public void WriteH265(byte []data, int size){
        if(m_handle != 0){
            SendH265(m_handle, bTcp,  bVideo, data, size);
        }
    }
    public void WriteH264(byte []data, int size){
        if(m_handle != 0){
            SendAAC(m_handle, bTcp,  bVideo, data, size);
        }
    }
}
