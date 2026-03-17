package com.wh2007.media;

import android.util.Log;

import com.mediastream.AudioStream;

//声音采集类
public class SoundCapture  extends Thread {
	
	AudioStream m_st = null;
	int m_handle = 0;
	int m_channel = -1;
	fout m_file = null;//输出文件
	boolean m_bOpen = false;
	//重载线程函数
	@Override
	public void run(){
        // new一个byte数组用来存一些字节数据，大小为缓冲区大小
		int size = 1600;
        byte[] buf = new byte[size];
		while(m_bOpen){
           int length = m_st.GetData(m_handle, buf);//从音频流的输出获取数据，长度大于0说明buf有数据
           if(length > 0){
        	   //Log.w("XXXXX","Data");
        	   m_file.Write(buf, length);
           }
           try {
			Thread.sleep(48);
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		}
	}	
	//初始化函数
	public void Open(AudioStream st, String filename, int handle){		
		m_st = st;
		m_handle = handle;
		m_file = new fout();
		m_file.Open(filename);
		m_bOpen = true;
		this.start();
	}
	public void Close(){		
		if(m_bOpen){
			m_bOpen = false;
			m_file.Close();
			m_st = null;
			m_channel = -1;
		}
	}
}
