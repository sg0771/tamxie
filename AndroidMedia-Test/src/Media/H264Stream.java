package Media;

import android.hardware.Camera;
import android.media.MediaRecorder;
import android.net.LocalServerSocket;
import android.net.LocalSocket;
import android.net.LocalSocketAddress;
import android.os.Handler;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import java.io.DataInputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.FileDescriptor;
import java.io.RandomAccessFile;

public class H264Stream extends Thread
implements SurfaceHolder.Callback{
	
	KK_IMediaSink m_sink = null;
	final byte [] avcC = new byte[] { 0x61, 0x76, 0x63, 0x43 };//avcC数组
	final byte [] head = new byte[] { 0, 0 , 0, 1};
	final byte [] mdat = new byte[] { 0x6d, 0x64, 0x61, 0x74 };
	//sps pps
	private int  m_len =0;
	private byte m_spspps[];	
	private String m_szMP4;
	private String m_szParam;	
	public void GetSPSPPS(String PathSPSPPS){//从已经存在SD卡的参考数据读取SPSPPS参数
		try {
			RandomAccessFile fin = new RandomAccessFile(PathSPSPPS, "r");
			try {
				m_len = (int)fin.length();
				m_spspps = new byte[m_len];
				fin.read(m_spspps, 0,  m_len);
			} catch (IOException e) {
				Log.e("ftanx", "No find SPSPPS File");
			}
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			Log.e("ftanx", "No find SPSPPS File");
		}	
	}
	public void ConvertSPSPPS(String PathFileMP4, String PathSPSPPS){//从一个参考MP4文件获取SPSPPS数据
		byte [] tmp = new byte[4];
		int pos = 0;		
		try {
			RandomAccessFile fin = new RandomAccessFile(PathFileMP4, "r");		
			try {
				for(int i = 0; i < fin.length(); i++){
					fin.seek(i);
					fin.read(tmp);
					if(	avcC[0] == tmp[0]
					&&	avcC[1] == tmp[1]
					&&	avcC[2] == tmp[2]
					&& 	avcC[3] == tmp[3])
					{
						pos = i;break;
					}							 
				}
				if(pos == 0){
					Log.e("ftanx", "No find SPSPPS");
					return;
				}
				else{
					short spslen = fin.readShort();
					spslen = fin.readShort();
					spslen = fin.readShort();
					spslen = fin.readShort();//第四个是有效长度
					byte SPS[] = new byte[spslen];
					fin.read(SPS);
					byte skip = fin.readByte();//跳过
					short ppslen = fin.readShort();
					byte PPS[] = new byte[ppslen];
					fin.read(PPS);
					m_len = 8 + spslen + ppslen;
					m_spspps = new byte[m_len];
					System.arraycopy(head, 0, m_spspps, 0, 4);
					System.arraycopy(SPS,  0, m_spspps, 4, spslen);
					System.arraycopy(head, 0, m_spspps, spslen + 4, 4);
					System.arraycopy(PPS,  0, m_spspps, spslen + 8, ppslen);
					String str = "POS = " + pos + " SPS = " + spslen + " PPS = " + ppslen;
					Log.d("ftanx", str);					
				}
			} catch (IOException e) {
				Log.e("ftanx", "No find SPSPPS");
				return;
			}
			
		} catch (FileNotFoundException e) {
			Log.e("ftnx", "IO Error for Read MP4");
			return;
		}	
		
		try {
			RandomAccessFile fout = new RandomAccessFile(PathSPSPPS, "rw");
			try {
				fout.write(m_spspps, 0, m_len);
			} catch (IOException e) {
				// TODO Auto-generated catch block
				Log.e("ftnx", "IO Error for write spspps");
			}
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			Log.e("ftnx", "IO Error for write spspps");
		}	
	}
	
	//h264 encoder
    private Camera myCamera = null;
    private MediaRecorder myMediaRecorder = null;
    private SurfaceHolder myCamSHolder;
    private SurfaceView	myCameraSView;

    public void Open(SurfaceView sv, int wid, int hei, int bitrate, KK_IMediaSink sink){ 
    	
    	m_sink = sink;//回调操作
    	myCameraSView = sv;
    	myCamSHolder = myCameraSView.getHolder();
    	myCamSHolder.addCallback(this);
    	myCamSHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);        
        myCamera = Camera.open();        
        myMediaRecorder =  new MediaRecorder();
        myCamera.stopPreview();
        myCamera.unlock();
        myMediaRecorder.setCamera(myCamera);
        myMediaRecorder.setVideoSource(MediaRecorder.VideoSource.CAMERA); 
        myMediaRecorder.setOutputFormat(MediaRecorder.OutputFormat.MPEG_4); 
        myMediaRecorder.setVideoEncoder(MediaRecorder.VideoEncoder.H264); 
        myMediaRecorder.setVideoSize(wid, hei); 
        myMediaRecorder.setVideoEncodingBitRate(bitrate * 1024);
        myMediaRecorder.setVideoFrameRate(15);
        
    	m_szMP4   = "/sdcard/" + wid + "x" + hei + "x" + bitrate + ".mp4";   // SPSPPS 参考文件
    	m_szParam = "/sdcard/" + wid + "x" + hei + "x" + bitrate + ".spspps";// SPSPPS 参考文件
    	File file = new File(m_szParam);
    	if(!file.exists()){ //创建SPSPPS文件            
            StartRecording(m_szMP4); 
            
			Log.d("ftanx", "Create a file to parse spspps!");
            new Handler().postDelayed(new Runnable() {            	
                public void run() {                 	
                	StopMedia();
    				ConvertSPSPPS(m_szMP4, m_szParam);
    				Log.d("ftanx", "Parse MP4 To SPSPPS!");
    				SendData();
                } 
            }, 1000); 
    	}
    	else{
			Log.d("ftanx", "get SPSPPS from data!");
    		GetSPSPPS(m_szParam);
    		//发数据了
			SendData();
    	}
    }
    public void Close(){
    	m_sink = null;
    	m_bRun = false;
    	//stop();
    	StopMedia();
    	if(myCamera != null){
    		myCamera.stopPreview();
    		myCamera.release();
    		myCamera = null;
    	}
    }
    
	LocalSocket receiver, sender;
	LocalServerSocket lss;
    boolean m_bRun;
    private void SendData(){
    	Log.d("ftanx","Send H264 Raw Data");
		receiver = new LocalSocket();
		try {
			lss = new LocalServerSocket("VideoCamera");
			receiver.connect(new LocalSocketAddress("VideoCamera"));
			receiver.setReceiveBufferSize(500000);
			receiver.setSendBufferSize(500000);
			sender = lss.accept();
			sender.setReceiveBufferSize(500000);
			sender.setSendBufferSize(500000);
			StartStreaming(sender.getFileDescriptor());//流方式发送
	        Log.i("ftanx", "##startRecording....");
	        m_bRun = true;
	        start();//录制线程启动     
		} catch (IOException e) {
			Log.e("ftanx", "LocalSocket Error!");
			return;
		}
    }
    
    @Override
    public void run()
    {
        Log.i("ftanx", "##Video Camera Thread Run....");
        DataInputStream dataInputStream = null; 
        //获得编码输入流指针
        try{
            dataInputStream = new DataInputStream(receiver.getInputStream());
        } catch (IOException e2){
            Log.e("ftanx","No Stream!");
            //stop();
            return;
        }    
        
        //Sleep一会，确保输入流中有数据
        try{
            Thread.currentThread().sleep(500);
        } catch (InterruptedException e1){
            Log.e("ftanx","Sleep Error!");
            //stop();
            return;
        }
        
        //查找 mdat
        int iMdat = 0x6D646174;
        try {
			while(iMdat != dataInputStream.readInt()){
				//查找mdat
			}
	        byte buffer[] = new byte[256 * 1024];//Data
	        byte buffer2[] = new byte[256 * 1024];//Data
			System.arraycopy(m_spspps, 0, buffer, 0, m_len);
			System.arraycopy(head, 0, buffer, m_len, 4);//关键帧的头
			
			System.arraycopy(head, 0, buffer2, 0, 4);//非关键帧
			
			while(m_bRun){  //执行				
	            int h264length = dataInputStream.readInt(); //H264帧长度	            
	            int offSet = 0;
	            while (offSet < h264length){
	                int beLeft = h264length - offSet;//剩下的数据
	                int tmp = dataInputStream.read(buffer2, 4 + offSet, beLeft);
	                offSet += tmp;
	            }		
	            int NalType = buffer2[4] & 0x0f;
	            if(m_sink != null){
	            	if(NalType != 5){
	            		m_sink.OnData(buffer2, 4 + h264length);//非关键帧
	            	}
	            	else{
	        			System.arraycopy(buffer2, 0, buffer, m_len  + 4, h264length);
	            		m_sink.OnData(buffer, m_len + 4 + h264length);//关键帧
	            	}
	            }
			}
		} catch (IOException e) {
			Log.d("ftanx", "IO Error");
		}      
    }
    
    private boolean realyStart() {
       
        myMediaRecorder.setPreviewDisplay(myCamSHolder.getSurface());
        try {
        	myMediaRecorder.prepare();
	    } catch (IllegalStateException e) {
	        releaseMediaRecorder();	
	        Log.d("ftanx", "JAVA:  camera prepare illegal error");
            return false;
	    } catch (IOException e) {
	        releaseMediaRecorder();	    
	        Log.d("ftanx", "JAVA:  camera prepare io error");
            return false;
	    }
	    
        try {
            myMediaRecorder.start();
        } catch( Exception e) {
            releaseMediaRecorder();
	        Log.d("ftanx", "JAVA:  camera start error");
            return false;
        }
        return true;
    }

    public boolean StartStreaming(FileDescriptor targetFd) {
        myMediaRecorder.setOutputFile(targetFd);
        myMediaRecorder.setMaxDuration(9600000); 	// Set max duration 4 hours
        myMediaRecorder.setOnInfoListener(streamingEventHandler);
        return realyStart();
    }

    public boolean StartRecording(String targetFile) {
        myMediaRecorder.setOutputFile(targetFile);                
        return realyStart();
    }
    
    public void StopMedia() {
        myMediaRecorder.stop();
        releaseMediaRecorder();        
    }

    private void releaseMediaRecorder(){
        if (myMediaRecorder != null) {
        	myMediaRecorder.reset();   // clear recorder configuration
        	myMediaRecorder.release(); // release the recorder object
        	myMediaRecorder = null;
            myCamera.lock();           // lock camera for later use
            myCamera.startPreview();
        }
        myMediaRecorder = null;
    }

     
    private MediaRecorder.OnInfoListener streamingEventHandler = new MediaRecorder.OnInfoListener() {
        @Override
        public void onInfo(MediaRecorder mr, int what, int extra) {
            Log.d("TEAONLY", "MediaRecorder event = " + what);    
        }
    };

    @Override
    public void surfaceChanged(SurfaceHolder sh, int format, int w, int h){
    	if ( myCamera != null && myMediaRecorder == null) {
            myCamera.stopPreview();
            try {
                myCamera.setPreviewDisplay(sh);
            } catch ( Exception ex) {
                ex.printStackTrace(); 
            }
            myCamera.startPreview();
        }
    }
    
	@Override
    public void surfaceCreated(SurfaceHolder sh){
    }
    
	@Override
    public void surfaceDestroyed(SurfaceHolder sh){
    }
}
