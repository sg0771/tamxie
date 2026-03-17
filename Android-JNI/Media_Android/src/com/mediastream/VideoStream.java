package com.mediastream;

import java.io.IOException;
import java.util.List;
import android.app.Activity;
import android.content.pm.ActivityInfo;
import android.graphics.Bitmap;
import android.graphics.ImageFormat;
import android.hardware.Camera;
import android.hardware.Camera.PreviewCallback;
import android.util.Log;
import android.view.Display;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.SurfaceHolder.Callback;
import android.view.WindowManager;

public class VideoStream implements SurfaceHolder.Callback, PreviewCallback{
	
	private int FindFrontCamera(){  
		int cameraCount = 0;  
		Camera.CameraInfo cameraInfo = new Camera.CameraInfo();  
		cameraCount = Camera.getNumberOfCameras(); // get cameras number  
                
		for ( int camIdx = 0; camIdx < cameraCount;camIdx++ ) {  
			Camera.getCameraInfo( camIdx, cameraInfo ); // get camerainfo  
			if ( cameraInfo.facing ==Camera.CameraInfo.CAMERA_FACING_FRONT ) {   
				// 代表摄像头的方位，目前有定义值两个分别为CAMERA_FACING_FRONT前置和CAMERA_FACING_BACK后置  
				return camIdx;  
			}  
		}  
		return -1;  
	}  

	private int FindBackCamera(){  
		int cameraCount = 0;  
		Camera.CameraInfo cameraInfo = new Camera.CameraInfo();  
		cameraCount = Camera.getNumberOfCameras(); // get cameras number  
                
		for ( int camIdx = 0; camIdx < cameraCount;camIdx++ ) {  
		Camera.getCameraInfo( camIdx, cameraInfo ); // get camerainfo  
			if ( cameraInfo.facing ==Camera.CameraInfo.CAMERA_FACING_BACK ) {   
				// 代表摄像头的方位，目前有定义值两个分别为CAMERA_FACING_FRONT前置和CAMERA_FACING_BACK后置  
				return camIdx;  
			}  
		}  
		return -1;  
	}  
	
	//Video Capture
	public Camera m_camera = null;
	int   iFront = 0;
	SurfaceView   m_prevewview = null;//摄像头预览窗口
	SurfaceHolder m_surfaceHolder = null;
	public boolean m_bNV21 = false;
	public boolean m_bYV12 = false;
	
	SurfaceView m_view1 = null;//本地YV12数据预览窗口
	public void SetPreview(SurfaceView view){
		m_view1 = view;
	}
	

	   /**
     * 判断横竖屏
     * @param activity
     * @return 1：竖 | 0：横
     */
    @SuppressWarnings("deprecation")
	public static int ScreenOrient(Activity activity)
    {
        int orient = activity.getRequestedOrientation(); 
        if(orient != ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE && orient != ActivityInfo.SCREEN_ORIENTATION_PORTRAIT) {
            WindowManager windowManager = activity.getWindowManager();  
            Display display = windowManager.getDefaultDisplay();  
			int screenWidth  = display.getWidth();  
            int screenHeight = display.getHeight();  
            orient = screenWidth < screenHeight ? ActivityInfo.SCREEN_ORIENTATION_PORTRAIT : ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE;
        }
        return orient;
    }
	//App 当前旋转角度
	public static int getDisplayRotation(Activity activity) {
		  int rotation = activity.getWindowManager().getDefaultDisplay().getRotation();
		  switch (rotation) {
		    case Surface.ROTATION_0: return 0;
		    case Surface.ROTATION_90: return 90;
		    case Surface.ROTATION_180: return 180;
		    case Surface.ROTATION_270: return 270;
		  }
		  return 0;
	}

	//分辨率
	public final static int SIZE_320P = 0;//320*240
	public final static int SIZE_640P = 1;//640*480
	public final static int SIZE_720P = 2;//1280*720
	public final static int SIZE_1080P = 3;//1920*1080
	private int m_rotate = 0;//旋转角度
	private int m_front = 0;//前置摄像头为0，后置摄像头为1
	private byte[] m_pYV12 = null;
	private ImageConvert IMAGE_CONVERT = null;
	
	//VideoSize 分辨率
	//Activity activity 当前activity
	//SurfaceView  view 预览窗口
	//isFrontCamera 是否前置摄像头
	private void ConfigureVideoSize(int VideoSize, Activity activity){
		if(ScreenOrient(activity) == 0){
			switch(VideoSize){
			case SIZE_320P:
				m_iWidth = 320;
				m_iHeight = 240;
				break;
			case SIZE_640P:
				m_iWidth = 640;
				m_iHeight = 480;
				break;
			case SIZE_720P:
				m_iWidth = 1280;
				m_iHeight = 720;
				break;
			case SIZE_1080P:
				m_iWidth = 1920;
				m_iHeight = 1080;
				break;
			default:
				m_iWidth  = 320;
				m_iHeight = 240;
			}
		}else{
			switch(VideoSize){
			case SIZE_320P:
				m_iWidth  = 240;
				m_iHeight = 320;
				break;
			case SIZE_640P:
				m_iWidth  = 480;
				m_iHeight = 640;
				break;
			case SIZE_720P:
				m_iWidth  = 720;
				m_iHeight = 1280;
				break;
			case SIZE_1080P:
				m_iWidth  = 1080;
				m_iHeight = 1920;
				break;
			default:
				m_iWidth  = 240;
				m_iHeight = 320;
			}
		}
	}
	
	public H264Encoder m_H264Encoder = null;
	public H264Decoder m_H264Decoder = null;
	
	//这个手机荣耀6比较扯淡
	//不支持320*240...
	//如果图像的size 或者 预览的 size 都不在列表里面
	//就会Crash，而且无法捕捉 
	//因为我们的采集时在回调里面抓数据
	
	public void CameraSize() {
		Camera camera = Camera.open(0);
        List<Camera.Size> pictureSizes = camera.getParameters().getSupportedPictureSizes();
        List<Camera.Size> previewSizes = camera.getParameters().getSupportedPreviewSizes();
        Camera.Size psize;
        for (int i = 0; i < pictureSizes.size(); i++) {
            psize = pictureSizes.get(i);
           Log.e("pictureSize", "F" + i + " = " + psize.width+" x "+psize.height);
        }
        for (int i = 0; i < previewSizes.size(); i++) {
            psize = previewSizes.get(i);
            Log.e("previewSize","F" + i + " = " + psize.width+" x "+psize.height);
        }
        camera.release();
    }
	
	private boolean OpenCapture(
			int VideoSize, 
			Activity activity, 
			SurfaceView  view, 
			boolean isFrontCamera) {
		ConfigureVideoSize(VideoSize,activity);//配置分辨率
		String vss = "Set Size = " + m_iWidth + "X" + m_iHeight;
		Log.e("Set Video Size", vss);
		m_prevewview = view;
		int cameraId = isFrontCamera ? FindFrontCamera() : FindBackCamera();// CAMERA_FACING_FRONT为前置摄像头
		try {
			
			m_front = isFrontCamera ? 1 : 0;//前摄像头
			m_rotate = getDisplayRotation(activity);
			
			m_camera = Camera.open(cameraId);
			Camera.CameraInfo info = new Camera.CameraInfo();
			Camera.getCameraInfo(cameraId, info);
			int result;
			if (info.facing == Camera.CameraInfo.CAMERA_FACING_FRONT) {
			    result = (info.orientation + m_rotate) % 360;
			    result = (360 - result) % 360; // compensate the mirror
			    String msg = "FRONT+++result = " +result + " " + m_front +  " " + m_rotate;Log.e("FRONT", msg);
			 } else { // back-facing
			    result = (info.orientation - m_rotate + 360) % 360;
			    String msg = "BACK+++ result = " +result + " " + m_front +  " " + m_rotate;Log.e("BACK", msg);		
			}
			m_camera.setDisplayOrientation(result);//旋转预览图像
			m_rotate = result;//
			
			m_surfaceHolder = m_prevewview.getHolder(); 
			m_surfaceHolder.setFixedSize(m_iWidth, m_iHeight); 
			m_surfaceHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
			m_surfaceHolder.addCallback((Callback) this);
			m_camera.setPreviewDisplay(m_surfaceHolder);
			Camera.Parameters parameters = m_camera.getParameters();
			if(m_rotate == 0){  //ok
				parameters.setPreviewSize(m_iWidth, m_iHeight);
				parameters.setPictureSize(m_iWidth, m_iHeight);
			}else{
				parameters.setPreviewSize(m_iHeight, m_iWidth);
				parameters.setPictureSize(m_iHeight, m_iWidth);
			}
			List<Integer> formatsList = parameters.getSupportedPreviewFormats();	//获取设备支持的预览format
			if(formatsList.contains(ImageFormat.YV12)){
				parameters.setPreviewFormat(ImageFormat.YV12);//建议枚举摄像头 支持的分辨率和采集格式
				m_bYV12 = true;
				Log.e("Set Video Formar", "YV12");
			}else if(formatsList.contains(ImageFormat.NV21)){
				parameters.setPreviewFormat(ImageFormat.NV21);		//设置预览格式为NV21，默认为NV21
				m_bNV21 = true;
				Log.e("Set Video Formar", "NV21");
			}
			Log.e("MSVideoStream", "Camera Open OK11111!!");
			m_camera.setParameters(parameters);	
			m_camera.setPreviewCallback((PreviewCallback)this);
			
			Log.e("MSVideoStream", "Camera Open OK22222!!");
			
			IMAGE_CONVERT = new ImageConvert();
			m_H264Decoder = new H264Decoder();
			m_H264Decoder.H264Open();
			m_H264Encoder = new H264Encoder();
			m_H264Encoder.X264Open(m_iWidth,  m_iHeight,  m_iFps*5, 30, m_iFps);
		
			m_camera.startPreview();
			Log.e("MSVideoStream", "Camera Open OK!!");
		} catch (IOException e) {
			Log.e("MSVideoStream", "Camera Open error!!");
			return false;
		}
		return true;
	}
	
	private void CloseCapture() {
		if(m_camera != null){
			try {
				if(m_view1 != null){
					//m_draw.RenderDestroy();
					m_draw = null;
					m_view1 = null;
				}
				//关闭摄像头
				m_camera.setPreviewCallback(null); 
				m_camera.setPreviewDisplay(null);
				m_camera.stopPreview(); 
				m_camera.release();
				m_camera = null; 
				Log.e("MSVideoStream", "Camera Close!!");
				
				if(m_H264Encoder != null){
					m_H264Encoder.X264Close();
					m_H264Encoder = null;
				}
		    	if(m_H264Decoder != null){
		    		m_H264Decoder.H264Close();
		    		m_H264Decoder = null;
		    	}
			} catch (IOException e) {
				Log.e("MSVideoStream", "Camera Close error!!");
			}
		}
	}
    
	@Override
	public void onPreviewFrame(byte[] arg0, Camera arg1) {
		long Time = System.currentTimeMillis();
		if(Time - lastTime >=  m_time){
			if(m_bYV12)OnDataYV12(arg0, arg0.length); //标准格式数据
			else OnDataNV21(arg0, arg0.length);//NV21数据
			lastTime = Time;
		}
	}

	byte[] m_pYV12_t = null;
	Bitmap m_bmpPreview = null;
	
	int ClipWidth  = 0;
	int ClipHeight = 0;
	
	MSDraw2 m_draw = null;
	public void OnEncoder(){
		
		if(m_view1 != null){
			if(m_draw == null)m_draw = new MSDraw2();
			if(m_draw!=null){
				//m_view1.getHolder().setFixedSize(m_iWidth, m_iHeight);
				
				m_draw.Open(m_view1, m_iWidth, m_iHeight,100 * 3,100 * 3);
				m_draw.Draw(m_pYV12);
			}
		}
		byte tmp[] = new byte[m_iWidth * m_iHeight];
		int len = m_H264Encoder.X264Encode(m_pYV12, tmp,0);
		//boolean key  = (len & 0xFF000000) > 0;
		int    length = len & 0x00FFFFFF;
		if(m_H264Decoder != null){
			m_H264Decoder.H264Decode(tmp, length);
		}
	}
	
	public void OnDataYV12(byte data[], int size){
		if(m_pYV12 == null){
			m_pYV12 = new byte[m_iWidth * m_iHeight * 3 / 2];
		}
		IMAGE_CONVERT.I420Rotate(data, m_pYV12, m_iWidth, m_iHeight, m_rotate + (m_rotate / 90) * m_front * 180);
		OnEncoder();
	}
	public void OnDataNV21(byte data[], int size){
		if(m_pYV12 == null){
			m_pYV12 = new byte[m_iWidth * m_iHeight * 3 / 2];
		}
		IMAGE_CONVERT.NV21ToI420Rotate(data, m_pYV12, m_iWidth, m_iHeight, m_rotate + (m_rotate / 90) * m_front * 180);
		//Log.e("NV21", "NV21");
		OnEncoder();
	}
	
	@Override
	public void surfaceChanged(SurfaceHolder arg0, int arg1, int arg2, int arg3) {
		// TODO Auto-generated method stub
	}

	@Override
	public void surfaceCreated(SurfaceHolder arg0) {
	
	}

	@Override
	public void surfaceDestroyed(SurfaceHolder arg0) {
		CloseCapture();
	}
	

	boolean m_bOpenCapture = false;
	int  m_iWidth =0;
	int  m_iHeight = 0;
	int  m_iFps = 0;
	long  m_time = 0;
	long lastTime = 0;
	
	

	//鏀瑰彉鍙傛暟鍓嶅厛Stop
	public boolean Open(int VideoSIze, int fps, Activity activity, SurfaceView  view, boolean bFront){
		Close();
		m_iFps = fps ;
		m_time = 1000 / fps;
		lastTime = System.currentTimeMillis();
		m_bOpenCapture = OpenCapture(VideoSIze, activity, view, bFront);
		return m_bOpenCapture;
	}
	public void Close(){
		if(!m_bOpenCapture)return;
		CloseCapture();
		m_bOpenCapture = false;
		m_iWidth = 0;
		m_iHeight = 0;
		m_iFps = 0;
		m_time = 0;
	}
}
