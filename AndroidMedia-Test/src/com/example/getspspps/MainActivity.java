package com.example.getspspps;


import Media.*;
import android.os.Bundle;
import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.pm.ActivityInfo;
import android.view.Menu;
import android.view.SurfaceView;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemSelectedListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.TextView;

public class MainActivity extends Activity implements OnItemSelectedListener{
	boolean bTh = true;
	H264Stream m_Cam = null;//硬编码采集输出
	WriteToFile m_file = null;//写文件回调
	Button m_btn1 = null;	//按钮事件
	TextView m_TV = null;//Log输出 
	SurfaceView m_SV1 = null;//录制预览界面
	SurfaceView m_SV2 = null;//解码1界面	
	SurfaceView m_SV3 = null;//解码2界面
	SurfaceView m_SV4 = null;//录制预览界面
	SurfaceView m_SV5 = null;//解码1界面	
	SurfaceView m_SV6 = null;//解码2界面
	SurfaceView m_SV7 = null;//录制预览界面
	SurfaceView m_SV8 = null;//解码1界面	
	
	EditText m_ET1 = null;
	EditText m_ET2 = null;
	Spinner  m_Spinner = null;//box
	int m_index;
	
	VideoCapture m_VideoCapture = null;
	
    @Override
    protected void onCreate(Bundle savedInstanceState) {
    	   	
        super.onCreate(savedInstanceState);
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);// 横屏 
        setContentView(R.layout.activity_main);
        m_btn1 = (Button)findViewById(R.id.btn1);//按钮
        m_btn1.setOnClickListener(BtnFunc);     
        m_ET1 = (EditText)findViewById(R.id.ET1);
        m_ET2 = (EditText)findViewById(R.id.ET2);        
        m_TV = (TextView)findViewById(R.id.TVLOG);//写文字
        m_SV1 = (SurfaceView)findViewById(R.id.surfaceView1); 
        m_SV2 = (SurfaceView)findViewById(R.id.surfaceView2);
        m_SV3 = (SurfaceView)findViewById(R.id.surfaceView3);
        m_SV4 = (SurfaceView)findViewById(R.id.surfaceView4); 
        m_SV5 = (SurfaceView)findViewById(R.id.surfaceView5);
        m_SV6 = (SurfaceView)findViewById(R.id.surfaceView6);       
        m_SV7 = (SurfaceView)findViewById(R.id.surfaceView7); 
        m_SV8 = (SurfaceView)findViewById(R.id.surfaceView8);       
        
        m_Spinner = (Spinner)findViewById(R.id.box1);
        m_Spinner.setOnItemSelectedListener(this);  
        
    }
        
    //MediaTest
    private OnClickListener BtnFunc = new OnClickListener() {
        @SuppressWarnings("resource")
		@Override
        public void onClick(View v) {
        	String strIn  = m_ET1.getText().toString();
           	String strOut = m_ET2.getText().toString();
           	String strSDIn = "/sdcard/" + strIn;
           	String strSDOut = "/sdcard/" + strOut;
           	switch(m_index){
           	case 0:
           		StartMediaCodecH264(strSDOut);     		
           		break;
           	case 1:
           		StopMediaCodecH264();           		
           		break;
           	case 2:
           		GetEncoderInfo();
           		break;
           	case 3:
           		MulTest();
           		break;	
           	case 4:
           		VideoCaptureStart(strSDOut);
           		break;
           	case 5:
           		VideoCaptureStop();
           		break;
           	case 6:
           		H264Start(strSDOut);
           		break;      
           	case 7:      		
           		H264Stop();
               	break;  
           	case 8:
           		OpusTest(strSDIn, strSDOut);
               	break;               	
           	default:
           		LOG("Test!!");
           		break;
           	}
        }
    };

    void LOG(String str){
    	m_TV.setText(str);
    }
   
    void GetEncoderInfo()
    {
    	AVCEncoder.GetSupportType();    	
       	AVCDecoder.GetSupportType();      	
    }
    
    //视频采集+MediaCodec
    YUV2AVC_toFile m_fileAVC = null;
    AVCDecoder m_decoder  = null;   

    void MulTest()//多路解码测试 OK
    {
    	//mul h264 decode test
    	MulThread thread = new MulThread(m_SV1, m_SV2, m_SV3,m_SV4,
    			m_SV5,m_SV6,m_SV7,m_SV8);
    	thread.start();
     }
    
    @SuppressLint("SdCardPath")
	class MulThread extends Thread
    {
    	boolean m_bStop;
        fin m_f1 = null;
        fin m_f2 = null;
        fin m_f3 = null; 
        fin m_f4 = null;
        fin m_f5 = null;
        fin m_f6 = null;        
        fin m_f7 = null;
        fin m_f8 = null;       
        
        AVCDecoder m_h264_1 = null;
        AVCDecoder m_h264_2 = null;  
        AVCDecoder m_h264_3 = null;  
        AVCDecoder m_h264_4 = null;
        AVCDecoder m_h264_5 = null;  
        AVCDecoder m_h264_6 = null;          
        AVCDecoder m_h264_7 = null;
        AVCDecoder m_h264_8 = null;    
        
    	@SuppressLint("SdCardPath")
		public MulThread(SurfaceView sv1, SurfaceView sv2, SurfaceView sv3, SurfaceView sv4,
						 SurfaceView sv5, SurfaceView sv6, SurfaceView sv7, SurfaceView sv8)
    	{
        	//if(m_f1 == null || m_f2 == null || m_f3 == null)return ;
            m_f1 = new fin();
            m_f2 = new fin();
            m_f3 = new fin();  
            m_f4 = new fin();
            m_f5 = new fin();
            m_f6 = new fin();  
            m_f7 = new fin();
            m_f8 = new fin();
            m_f1.Open("/sdcard/" + "1280x720.264");
            m_f2.Open("/sdcard/" + "640x480.264");
            m_f3.Open("/sdcard/" + "352x288.264");
            m_f4.Open("/sdcard/" + "320x240a.264");
            m_f5.Open("/sdcard/" + "640x480a.264");
            m_f6.Open("/sdcard/" + "640x480b.264");
            m_f7.Open("/sdcard/" + "640x480c.264");
            m_f8.Open("/sdcard/" + "640x480d.264");
            m_h264_1 = new AVCDecoder();
            m_h264_2 = new AVCDecoder();
            m_h264_3 = new AVCDecoder();  
            m_h264_4 = new AVCDecoder();
            m_h264_5 = new AVCDecoder();
            m_h264_6 = new AVCDecoder();             
            m_h264_7 = new AVCDecoder();
            m_h264_8 = new AVCDecoder();           
            
            m_h264_1.Open(1280,720, sv1);
            m_h264_2.Open(640, 480, sv2);
            m_h264_3.Open(352, 288, sv3);              
            m_h264_4.Open(320, 240, sv4);
            
            m_h264_5.Open(640,480, sv5);
            m_h264_6.Open(640,480, sv6);                
            m_h264_7.Open(640,480, sv7);
            m_h264_8.Open(640,480, sv8);           
            
            m_bStop = false;
    	}
		public void realse()
		{
			m_bStop = true;
		}
		
		public void run()
		{
			while(!m_bStop){
				try {
					int len1 = m_f1.ReadLength();
					int len2 = m_f2.ReadLength();
					int len3 = m_f3.ReadLength();
					int len4 = m_f4.ReadLength();
					
					int len5 = m_f5.ReadLength();
					int len6 = m_f6.ReadLength();
					int len7 = m_f7.ReadLength();
					int len8 = m_f8.ReadLength();				
					if(len1 > 0 && len2 > 0 && len3 > 0 && len4 > 0
					&& len5 > 0 && len6 > 0 && len7 > 0 && len8 >0)
					{
						byte []buf1 = new byte[len1];m_f1.Read(buf1, len1);m_h264_1.Decode(buf1, len1);	
						byte []buf2 = new byte[len2];m_f2.Read(buf2, len2);m_h264_2.Decode(buf2, len2);	
						byte []buf3 = new byte[len3];m_f3.Read(buf3, len3);m_h264_3.Decode(buf3, len3);	
						byte []buf4 = new byte[len4];m_f4.Read(buf4, len4);m_h264_4.Decode(buf4, len4);											
						byte []buf5 = new byte[len5];m_f5.Read(buf5, len5);m_h264_5.Decode(buf5, len5);	
						byte []buf6 = new byte[len6];m_f6.Read(buf6, len6);m_h264_6.Decode(buf6, len6);	
						byte []buf7 = new byte[len7];m_f7.Read(buf7, len7);m_h264_7.Decode(buf7, len7);	
						byte []buf8 = new byte[len8];m_f8.Read(buf8, len8);m_h264_8.Decode(buf8, len8);					
					}
					else
					{
						m_bStop = true;;
					}						
					
					Thread.sleep(100);
						
				} catch (InterruptedException e) {
					// TODO Auto-generated catch block
					//e.printStackTrace();
				}
			}
		}
    }
    
    MediaCodecVideoDecoder m_xxxdec = null;
    void StartMediaCodecH264(String strOut)
    {
    	if(m_VideoCapture == null)
    	{
    		int w = 320;
    		int h = 240;
    		int fps = 30;
    		int bitrate = 550 * 1000;
    		m_VideoCapture = new VideoCapture();// (H264File)findViewById(R.id.surfaceView2);
            m_fileAVC = new YUV2AVC_toFile();  
            m_decoder = new AVCDecoder();
            m_decoder.Open(w, h, m_SV2);
            m_fileAVC.SetDecoder(m_decoder);
            
            m_xxxdec = new MediaCodecVideoDecoder();
            m_xxxdec.initDecode(w, h, m_SV3);
            m_fileAVC.SetDecoder2(m_xxxdec);                        
 
            m_fileAVC.Open(w, h, fps, bitrate, strOut); 
            m_VideoCapture.Open(w, h, m_SV1);
            m_VideoCapture.SetSink((KK_IMediaSink)m_fileAVC);            
 
            if(m_VideoCapture.m_bNV21)
                LOG("启动视频采集+MediaCodec! By NV21"); 
            else
                LOG("启动视频采集+MediaCodec! By YV12"); 
            	
    	} 
    }
    void StopMediaCodecH264()
    {
    	if(m_VideoCapture != null)
    	{
    		m_VideoCapture.Close(); 
    		
    		m_fileAVC.Close();
    		m_fileAVC = null;
    		//m_decoder.Close();
    		//m_decoder = null;
            LOG("结束视频采集+MediaCodec!");
            m_VideoCapture = null;
    	}
    }
    
    //视频采集启动
    void VideoCaptureStart(String strOut)
    {
    	int w = 1280;
    	int h = 720;
    	if(m_VideoCapture == null)
    	{
    		m_VideoCapture = new VideoCapture();// (H264File)findViewById(R.id.surfaceView2);
            m_file = new WriteToFile();            
            m_file.Open(strOut); 
            m_VideoCapture.Open(w, h, m_SV1);
            m_VideoCapture.SetSink((KK_IMediaSink)m_file);            
            LOG("启动视频采集!");  
    	}  
    }
    //视频采集关闭
    void VideoCaptureStop()
    {
    	if(m_VideoCapture != null)
    	{
    		m_VideoCapture.Close(); 
    		m_file.Close();
    		m_file = null;
            LOG("结束视频采集!");
            m_VideoCapture = null;
    	}
    }
    //H264硬编解码测试
    void H264Start(String strOut) {
    	if(m_Cam == null)
    	{
            m_Cam = new H264Stream();// (H264File)findViewById(R.id.surfaceView2);
            m_file = new WriteToFile();            
            m_file.Open(strOut); 
            m_Cam.Open(m_SV1, 1920, 1080, 1024, (KK_IMediaSink)m_file);            
            LOG("启动H264硬编码!");  
    	}          
    }
    void H264Stop(){
    	if(m_Cam != null)
    	{
    		m_decoder.Close();
    		m_decoder = null;
    		
            m_Cam.Close(); 
            m_file.Close();
            m_file  = null;
            LOG("结束H264硬编码!");
            m_Cam = null;
    	}
    }
    //2
    void OpusTest(String strIN, String strOUT){
		LOG("Opus Test - Start!");
    	fin Fin = new fin();
    	boolean bRead = Fin.Open(strIN);
    	fout Fout = new fout();
    	boolean bWrite = Fout.Open(strOUT);
    	String str1 = "Opus Test File = " + bRead + " " + bWrite;
    	LOG(str1);
    	if(bRead && bWrite){
    		OpusEncoder enc = new OpusEncoder();
    		OpusDecoder dec = new OpusDecoder();
    		dec.open();
    		enc.open();
    		int total_pcm  = 0;
    		int total_opus = 0;
    		byte[] pcm = new byte[1600];//
    		while(Fin.Read(pcm, 1600)){
    			int length = enc.encode(pcm, 1600);
    			total_pcm += 1600;
    			total_opus += length;
    			int declen = dec.decode(enc.m_pout, length);
    			if(declen > 0)
    				Fout.Write(dec.m_pout, declen);
    		}
    		double cp = total_pcm / total_opus;
    		String szlog = "PCM = " + total_pcm +" " + " OPUS = "+total_opus + " CP = " + cp; 
			LOG(szlog);
    		Fin.Close();
    		Fout.Close();
    	}
    	else{
    		LOG("Opus Test - IO Erorr!");
    	}
    }

    
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.main, menu);
        return true;
    }

	@Override
	public void onItemSelected(AdapterView<?> arg0, View arg1, int arg2,
			long arg3) {
		// TODO Auto-generated method stub
		m_index = arg2;
	}

	@Override
	public void onNothingSelected(AdapterView<?> arg0) {
		// TODO Auto-generated method stub
		
	}    
}
