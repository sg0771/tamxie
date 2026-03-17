package com.wh2007.media;
import android.app.Activity;
import android.content.Context;
import android.media.AudioManager;
import android.util.Log;

import com.mediastream.*;

//声音回放类
public class SoundPlay  extends Thread {
	
	AudioStream m_st = null;
	int m_handle = 0;
	int m_channel = -1;
	fin m_file = null;
	boolean m_bOpen = false;
	
	
	private static int currVolume = 0;
	/**
	* 打开扬声器
	*/
	public static void openSpeaker(Activity activity) {
	    try{
	      AudioManager audioManager = (AudioManager) activity.getSystemService(Context.AUDIO_SERVICE);
	      audioManager.setMode(AudioManager.ROUTE_SPEAKER);
	      currVolume = audioManager.getStreamVolume(AudioManager.STREAM_VOICE_CALL);
	      if(!audioManager.isSpeakerphoneOn()) {
	        //setSpeakerphoneOn() only work when audio mode set to MODE_IN_CALL.
	        audioManager.setMode(AudioManager.MODE_IN_CALL);
	        audioManager.setSpeakerphoneOn(true);
	        audioManager.setStreamVolume(AudioManager.STREAM_VOICE_CALL,
	            audioManager.getStreamMaxVolume(AudioManager.STREAM_VOICE_CALL ),
	            AudioManager.STREAM_VOICE_CALL);
	      }
	    } catch (Exception e) {
	      e.printStackTrace();
	    }
	}
	/**
	* 关闭扬声器
	*/
	public static void closeSpeaker(Activity activity) {
	    try {
	      AudioManager audioManager = (AudioManager) activity.getSystemService(Context.AUDIO_SERVICE);
	      if(audioManager != null) {
	        if(audioManager.isSpeakerphoneOn()) {
	          audioManager.setSpeakerphoneOn(false);
	          audioManager.setStreamVolume(AudioManager.STREAM_VOICE_CALL,currVolume,
	              AudioManager.STREAM_VOICE_CALL);
	        }
	      }
	    } catch (Exception e) {
	      e.printStackTrace();
	    }
	}
	
	//重载线程函数
	@Override
	public void run(){
        // new一个byte数组用来存一些字节数据，大小为缓冲区大小
		int size = 1600;
        byte[] buf = new byte[size];
		while(m_bOpen){
           m_file.Read(buf, size);
           m_st.WriteData(m_handle, m_channel, buf, size);
           try {
        	   Thread.sleep(50);
           } catch (InterruptedException e) {
        	   // TODO Auto-generated catch block
        	   e.printStackTrace();
        	   Log.e("ftanx", "Run func error!");
           }
		}
	}	
	//初始化函数
	public void Open(AudioStream st, String filename, int handle, int channel){		
		m_st = st;
		m_handle = handle;
		m_channel = channel;
		m_file = new fin();
		m_file.Open(filename);
		m_bOpen = true;
		this.start();
	}
	public void Close(){		
		if(m_bOpen){
			m_bOpen = false;
			 try {
				Thread.sleep(1000);
				Log.e("ftanx", "Thread.sleep 1000!");
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				Log.e("ftanx", "Thread.sleep error!");
			}
			m_file.Close();
			m_st = null;
			m_channel = -1;
		}
	}
}
