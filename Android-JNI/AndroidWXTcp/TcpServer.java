package com.apowersoft.WXMedia;

public class TcpServer {

    static {
        System.loadLibrary("WXTcp");
    }
    //----------------------NDK API ----------------------------
    public native long  Init(int nTcpPort);
    public native void  CloseChannel(long handle,  long uid);
    public native void  Deinit(long handle);
    private  long           m_handle = 0;
    
    
     public void OnVideoData(byte[] data){
		//JNI调用
		//回调接收的H264数据
     }
     public void OnAudioData(byte[] data){
     		//JNI调用
     		//回调接收后解码的PCM数据
     }

    public boolean Start(int nTcpPort){
        m_handle = Init(nTcpPort);
        return m_handle != 0;
    }
    
    public void Stop(){
        if(m_handle != 0){
            Deinit(m_handle);
            m_handle = 0;
        }
    }
    
    public void CloseChannel(long uid){
            if(m_handle != 0){
            CloseChannel(m_handle,uid);
            m_handle = 0;
        }
    }
}
