package Media;

import java.io.IOException;
import java.util.List;

import android.graphics.ImageFormat;
import android.hardware.Camera;
import android.hardware.Camera.PreviewCallback;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.SurfaceHolder.Callback;

//Video Capture 

public class VideoCapture 
implements SurfaceHolder.Callback, PreviewCallback {
	public Camera m_camera = null;  
	SurfaceView   m_prevewview = null;
	SurfaceHolder m_surfaceHolder = null;
	int m_width = 640;
	int m_height = 480;
	KK_IMediaSink m_sink = null;
	public boolean m_bNV21 = false;
	public boolean m_bYV12 = false;

	@TargetApi(9)  
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
	@TargetApi(9)  
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

	public boolean Open(int width, int height, SurfaceView  view, boolean isFrontCamera) {
		m_prevewview = view;
		m_surfaceHolder = m_prevewview.getHolder(); 
		m_surfaceHolder.setFixedSize(m_width, m_height); 
		m_surfaceHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
		m_surfaceHolder.addCallback((Callback) this);
		int cameraId = isFrontCamera ? FindBackCamera() : FindFrontCamera();// CAMERA_FACING_FRONT为前置摄像头
		try {
			m_camera = Camera.open(cameraId);
			m_camera.setPreviewDisplay(m_surfaceHolder);
			Camera.Parameters parameters = m_camera.getParameters();
			List<Integer> formatsList = parameters.getSupportedPreviewFormats();	//获取设备支持的预览format
			if(formatsList.contains(ImageFormat.YV12)){
				parameters.setPreviewFormat(ImageFormat.YV12);//建议枚举摄像头 支持的分辨率和采集格式
				m_bYV12 = true;
				Log.i("H264 Capture Format", "YV12");	
			}else if(formatsList.contains(ImageFormat.NV21)){
				parameters.setPreviewFormat(ImageFormat.NV21);		//设置预览格式为NV21，默认为NV21
				m_bNV21 = true;
				Log.i("H264 Capture Format", "NV21");
			}
			m_width  = width;
			m_height = height;
			parameters.setPreviewSize(m_width, m_height);
			parameters.setPictureSize(m_width, m_height);
			m_camera.setParameters(parameters);	
			m_camera.setPreviewCallback((PreviewCallback) this);
			m_camera.startPreview();
			Log.d("MSVideoStream", "Camera Open OK!!");
		} catch (IOException e) {
			Log.e("MSVideoStream", "Camera Open error!!");
			return false;
		}
		return true;
	}
	
	//回调接口
	public void SetSink(KK_IMediaSink sink){	
		Log.d("MSVideoStream", "Camera SetSink!!");
		m_sink = sink;
	}
	public void Close() {
		Log.d("MSVideoStream", "Camera Close!!");
		m_sink = null;
		if(m_camera != null){
			m_camera.setPreviewCallback(null); 
			m_camera.stopPreview(); 
			m_camera.release();
			m_camera = null; 
		}
	}
    
    
	@Override
	public void onPreviewFrame(byte[] arg0, Camera arg1) {
		//Log.d("MSVideoStream", "Data Sink");
		if(m_sink != null){
			m_sink.OnData(arg0, arg0.length);
		}
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
		Close();
	}
}
