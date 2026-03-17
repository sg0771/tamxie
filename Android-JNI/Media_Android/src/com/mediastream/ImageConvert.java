package com.mediastream;

public class ImageConvert {
	static{
	     System.loadLibrary("ms2");
	}
	public native void I420Rotate(byte []in, byte []out, int dst_width, int dst_height, int rotate);
	public native void NV21ToI420Rotate(byte []in, byte []out, int dst_width, int dst_height, int rotate);
}
