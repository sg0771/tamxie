package com.mediastream;
import android.view.Surface;
import android.view.SurfaceView;

//保持比例，显示全图，w或者h填充完，另外一边留黑
public class MSDraw2{
//=============================================================================
	public native int     xCreate(int src_w, int src_h, int win_w, int win_h);	
	public native void  xDraw(int handle, Surface surface, byte []buf);
	public native void  xDestroy(int handle);
//=============================================================================
	private int m_handle = 0;
	SurfaceView m_view = null;
//=============================================================================
	public void Open(SurfaceView view, int src_w, int src_h, int win_w, int win_h){
		if(m_handle == 0){
			m_handle = xCreate(src_w, src_h, win_w, win_h);
			m_view = view;
		}
	}
	public void Draw(byte []buf){
		if(m_handle != 0){
			xDraw(m_handle, m_view.getHolder().getSurface(), buf);
		}
	}
	public void Close(){
		if(m_handle != 0){
			xDestroy(m_handle);
			m_handle = 0;
		}
	}
}
