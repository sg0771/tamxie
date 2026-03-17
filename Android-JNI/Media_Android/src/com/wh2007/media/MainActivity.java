package com.wh2007.media;

import java.io.File;
import com.mediastream.*;
import com.apowersoft.*;
import android.app.Activity;
import android.content.pm.ActivityInfo;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.SurfaceView;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.ImageView;



public class MainActivity extends Activity {
	
	
	
	AudioStream test_stream = null;//媒体流对象
	int test_handle = 0;
	SoundPlay test_play1 = null;
	SoundPlay test_play2 = null;
	SoundPlay test_play3 = null;
	int m_node0 = 0;
	int m_node1 = 0;
	int m_node2 = 0;
	SoundCapture test_capture = null;

	MSDraw2 m_draw = null;

	ImageView m_preview = null;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
    	
    	requestWindowFeature(Window.FEATURE_NO_TITLE);//隐藏标题
    	getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
    	WindowManager.LayoutParams.FLAG_FULLSCREEN);//设置全屏
    	
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        m_view1 = (SurfaceView)findViewById(R.id.SV1);
        m_view2 = (SurfaceView)findViewById(R.id.SV2);
        m_view3 = (SurfaceView)findViewById(R.id.SV3);
        
        m_view2 = (SurfaceView)findViewById(R.id.SV2);
        
        /*
        test_stream = new AudioStream();//JNI 接口类
        test_handle = test_stream.Create(1);//音频流handle
        
        m_draw = new MSDraw2();
        
        */
        Log.e("SD card is",getSDPath());
        

    	return;
    	
    }
    
    

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();
        if (id == R.id.action_settings) {
            return true;
        }
        return super.onOptionsItemSelected(item);
    }
    
    
    //SD卡路径
    public String getSDPath(){ 
        File sdDir = null; 
        boolean sdCardExist = Environment.getExternalStorageState()   
        .equals(android.os.Environment.MEDIA_MOUNTED);//判断sd卡是否存在
        if(sdCardExist)   
        {                               
          sdDir = Environment.getExternalStorageDirectory();//获取跟目录
       }   
        return sdDir.toString(); 
    }

    public void onStartPlay(View view){
    	Log.e("SDDD", getSDPath());
    	
    	WXMedia ffmpeg_obj = new WXMedia();
    	
    	int s = 1;
    	if(s==0) {
    		ffmpeg_obj.cutting2("/storage/emulated/0/fk.mp4", "/storage/emulated/0/fk2.mp4", 10, 100);
    	}else {	
    		String []commond = new String[8];
    		commond[0] = "ffmpeg";
    		commond[1] = "-ss";
    		commond[2] = "10";
    		commond[3] = "-t";
    		commond[4] = "100";
    		commond[5] = "-i";
    		commond[6] = "/storage/emulated/0/fk.mp4";
    		commond[7] = "/storage/emulated/0/ftanx.mp4";
    		ffmpeg_obj.ffmpeg(commond);
    	}
				
    	/*
    	if(test_handle != 0){
    		//test_stream.Destroy(test_handle);
   
            test_play1 = new SoundPlay();
            test_play2 = new SoundPlay();
            test_play3 = new SoundPlay();           
            
        	String test_fm1 = getSDPath() + "/1.pcm";
        	String test_fm2 = getSDPath() + "/2.pcm";
        	String test_fm3 = getSDPath() + "/3.pcm";
        	Log.e("file is",test_fm1);
        	Log.e("file is",test_fm2);
        	Log.e("file is",test_fm3);
    		m_node0 = test_stream.AddNode(test_handle);
    		test_play1.Open(test_stream, test_fm1, test_handle, m_node0);
    		
    		m_node1 = test_stream.AddNode(test_handle);
    		test_play2.Open(test_stream, test_fm2, test_handle, m_node1);
    		m_node2 = test_stream.AddNode(test_handle);
    		test_play3.Open(test_stream, test_fm3, test_handle, m_node2);
    	}
    	*/
    }
    
    public void onStopPlay(View view){
    	
    	if(test_handle != 0){
    		
    		test_stream.RemoveNode(test_handle, m_node0);
    		test_stream.RemoveNode(test_handle, m_node1);
    		test_stream.RemoveNode(test_handle, m_node2);
    		test_play1.Close();
    		test_play2.Close();
    		test_play3.Close();	
            test_play1 = null;
            test_play2 = null;
            test_play3 = null;
            test_stream.Destroy(test_handle);
    		
            test_handle = 0;
    	}
    	
    }
    
    
    static int fm_s = 0;
	public void onStartRec(View view){
		if(test_handle != 0){
			String file_name_rec = getSDPath() + "/Test_rec_" + fm_s + ".pcm";
			fm_s ++;
			test_capture = new SoundCapture();
			Log.w("ftanx", file_name_rec);
			test_capture.Open(test_stream, file_name_rec, test_handle);
		}
    }
    
    public void onStopRec(View view){
    	if(test_handle != 0){
    		if(test_capture != null){
    			test_capture.Close();
    			test_capture = null;
    		}
    	}
    }
    
    
    static int openSp = 0;
    public void onSpeak(View view){
    	if(openSp == 0){
    		openSp = 1;
    		SoundPlay.openSpeaker(this);
    	}else{
    		openSp = 0;
    		SoundPlay.closeSpeaker(this);
    	}
    }
    

	@Override
	protected void onResume() {
		//横屏 rotate = 0 // ok
		//if(getRequestedOrientation()!=ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE)setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
		if(getRequestedOrientation()!=ActivityInfo.SCREEN_ORIENTATION_PORTRAIT) setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
		super.onResume();
	}
	//要设置成竖屏设置成 SCREEN_ORIENTATION_PORTRAIT
	
    SurfaceView  m_view1 = null;
    SurfaceView  m_view2 = null;
    SurfaceView  m_view3 = null; 
    
    VideoStream  m_vs = null;
    public void onStartVideo320(View view){
    	if(m_vs != null){
    		m_vs.Close();m_vs =null;
    	}
    	m_vs = new VideoStream();
    	m_vs.SetPreview(m_view2);
    	m_vs.Open(0, 10,this, m_view1, true);
        m_vs.m_H264Decoder.H264SetView(m_view3);
    }    
   
    public void onStartVideo640(View view){
    	if(m_vs != null){
    		m_vs.Close();m_vs =null;
    	}
    	m_vs = new VideoStream();
    	m_vs.SetPreview(m_view2);
    	m_vs.Open(1, 10,this, m_view1, true);
        m_vs.m_H264Decoder.H264SetView(m_view3);
    }
    
    public void onStartVideo720(View view){
    	if(m_vs != null){
    		m_vs.Close();m_vs =null;
    	}
    	m_vs = new VideoStream();
    	m_vs.SetPreview(m_view2);
    	m_vs.Open(2, 10,this, m_view1, true);
        m_vs.m_H264Decoder.H264SetView(m_view3);
    }
    public void onStopVideo(View view){
    	if(m_vs != null){
    		m_vs.Close();m_vs =null;
    	}
    }
    
    //枚举支持的分辨率
    public void MJ(View view){
    	byte[] buf = new byte[100*100*3/2];
    	for(int i = 0;i < 100*100*3/2;i++)
    		buf[i] = 92;
		m_draw.Open(m_view1, 100, 100, 100,100);
		m_draw.Draw(buf);
    }
    public void MJ2(View view){
    	byte[] buf = new byte[100*100*3/2];
    	for(int i = 0;i < 100*100*3/2;i++)
    		buf[i] = 0;
		m_draw.Open(m_view1, 100, 100,100,100);
		m_draw.Draw(buf);
    }
    public void MJ3(View view){
    	byte[] buf = new byte[100*100*3/2];
    	for(int i = 0;i < 100*100*3/2;i++)
    		buf[i] = 125;
		m_draw.Open(m_view1, 100, 100,100,100);
		m_draw.Draw(buf);
    }
 }
