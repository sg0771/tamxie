package com.wh2007.media;

import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.RandomAccessFile;

import android.util.Log;


//文件输出
public class fout {
	RandomAccessFile m_raf = null;//读指针
	public boolean Open(String fm){
		try {
			m_raf = new RandomAccessFile(fm, "rw");
			Log.e("ftanx", "fout open OK");
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			//e.printStackTrace();
			Log.e("ftanx", "fout open error");
			return false;
		}
		return m_raf != null;
	}
	public boolean Write(byte []buf, int size){
		try {
			m_raf.write(buf, 0, size);
			String str = "Write " + size + " Data To File!!";
			Log.e("ftanx", str);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			//e.printStackTrace();
			Log.e("ftanx", "file IO Error!");
			return false;
		}
		return true;
	}
	public void Close(){
		if(m_raf != null){
			try {
				m_raf.close();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				//e.printStackTrace();
				Log.e("ftanx", "file IO Error!");
			}
		}
	}
}
