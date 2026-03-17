package com.apowersoft.WXMedia;

import android.util.Log;

public class PCCastRecv{

    static {
        System.loadLibrary("PCCastRecv");
    }
    //----------------------NDK API ----------------------------//
    public native long  Start(int nTcpPort);
    public native void  Stop(long handle);

    //------------------ JNI 回调函数 --------------------------//
    //JNI 数据回调函数
    //uid 连接ID
    //bVideo: 0 表示音频 1表示视频
    //data: 回调数据地址
    //data_size: 回调数据长度
    public void OnData(int uid, int bVideo, byte[] data, int data_size){
        //
    }

    //JNI 消息回调函数
    //uid:  连接ID
    //bConnected:   1 表示新连接， 0 表示连接断开
    public void OnEvent(int uid, int bConnected){
        //
    }

    //-------------------- JAVA --------------------------------//      
    private  long  m_handle = 0;
    public boolean PCCastStart(int nTcpPort){
        m_handle = Start(nTcpPort);
        String str = "WX Start TCP " + m_handle;

        Log.e("WX", str);
        return m_handle != 0;
    }
    
    public void PCCastStop(){
        if(m_handle != 0){
            Stop(m_handle);
            m_handle = 0;
        }
    }
}
