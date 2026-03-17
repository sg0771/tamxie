/*
瑙嗛瑙ｇ爜鍣紝杈撳嚭涓篟GBA鏍煎紡
*/

package com.mediastream;
import android.view.SurfaceView;


public class H264Decoder{
	
	static{
	     System.loadLibrary("ms2");
	}
	public native int    Open();
	public native int    Close(int handle);
	public native int    Decode(int handle, byte []data, int size);
	public native int    GetWidth(int handle);
	public native int    GetHeight(int handle);
	public native int    GetYUVData(int handle, byte []buf);
	//public native int    GetYUVData2(int handle, byte []buf, int dst_w, int dst_h);	
	
	int m_handle = 0;
	int m_iWidth  =0;
	int m_iHeight = 0;
	byte []m_bufYUV  = null;
	SurfaceView m_view = null;
	ImageConvert m_convert = null;
	MSDraw2 m_draw = null;
	
	private int  H264GetWidth(){
		if(m_handle == 0)return 0;
		return GetWidth(m_handle);		
	}
	private int  H264GetHeight(){
		if(m_handle == 0)return 0;
		return GetHeight(m_handle);	
	}
	private int H264GetData(byte []buf){
		if(m_handle == 0)return 0;
		return GetYUVData(m_handle, buf);	
	}
	
	public void H264Open(){
		m_handle = Open();
		m_convert = new ImageConvert();
		m_draw = new MSDraw2();
	}
	//不绘制就传个null进去
	public void H264SetView(SurfaceView view){
		m_view = view;
	}
	public void H264Close(){
		if(m_handle == 0)return;
		Close(m_handle);
		m_handle = 0;
		m_view = null;
	}
	

	public int  H264Decode(byte []data, int size){
		if(m_handle == 0)return 0;
		int ret = Decode(m_handle,data,size);
		if(ret > 0){
			if(m_iWidth == 0) m_iWidth = H264GetWidth();
			if(m_iHeight == 0)m_iHeight = H264GetHeight();
			if(m_iWidth * m_iHeight != 0){
				if(m_bufYUV == null) m_bufYUV  = new byte[m_iWidth * m_iHeight * 3/2];
				H264GetData(m_bufYUV);//Get YUV Data
				if(m_view != null){
					m_view.getHolder().setFixedSize(m_iWidth, m_iHeight);
					m_draw.Open(m_view, m_iWidth, m_iHeight,200 * 3,100 * 3); //这里的手机在XML的1dp 等于实际的3个像素
					m_draw.Draw(m_bufYUV);
				}
			}
		}
		return ret;
	}
}