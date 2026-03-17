package com.apowersoft;

public class WXMedia {
	
	static{
	     System.loadLibrary("wxffmpeg");
	}
	
	public native int	cutting( String strInput, String strOutput,int tsStart, int tsDuration);
	public native int	cutting2( String strInput, String strOutput,int tsStart, int tsDuration);
	public native int	ffmpeg(String[] common);//ffmpeg(argc,argv)
	
	public native void	interrupt();
	public native int 	getCurrTime();
	public native int  	getTotalTime();
	public native int  	getState();
}