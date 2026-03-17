package cn.wps.moffice_eng;

import com.apowersoft.WXMedia.MediaConvert;
import com.apowersoft.WXMedia.PCCastRecv;

import android.content.Context;
import android.os.Bundle;

import com.google.android.material.floatingactionbutton.FloatingActionButton;
import com.google.android.material.snackbar.Snackbar;

import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;

import android.os.Environment;
import android.os.Handler;
import android.util.Log;
import android.view.View;

import android.view.Menu;
import android.view.MenuItem;
import android.widget.TextView;

import java.io.File;

public class MainActivity extends AppCompatActivity {
    MediaConvert m_MC = null; //SDK
    int m_bMCInit = 0; //SDK初始化标记
    long m_MChandle = 0;//操作句柄
    private TextView textView;

    private Handler handler = new Handler();
    private Runnable runnable = new Runnable() {
        public void run () {
            if(m_MChandle != 0){
                int rate = m_MC.GetState(m_MChandle);
                if(rate == 100){
                    textView.setText("压缩成功");
                    m_MC.Destroy(m_MChandle);
                    m_MChandle = 0;
                }else   if(rate == -1){
                    textView.setText("压缩失败");
                    m_MC.Destroy(m_MChandle);
                    m_MChandle = 0;
                }else  if(rate == -2){
                    textView.setText("没有开始压缩");
                }else{
                    String str = "当前转换进度 " + rate;
                    textView.setText(str);
                }
            }else{
                textView.setText("没有转换");
            }
            handler.postDelayed(this,1000);
        }
    };


    PCCastRecv m_obj;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Toolbar toolbar = findViewById(R.id.toolbar);
        textView =findViewById(R.id.textView);
        setSupportActionBar(toolbar);
        m_obj = new PCCastRecv();
        m_obj.Start(13838);

        if(0==1){
            File file= Environment.getExternalStorageDirectory(); //Android 10 以下获取 SD卡路径
            String strMp4 = file.getAbsolutePath() + "/c.mp4";
            Log.e("WX-----","strMp4 " + " " +strMp4); //源数据

            m_MC = new MediaConvert(); //MC Java Obj
            m_bMCInit = m_MC.LibraryInit(getApplicationContext()); //SDK初始化
            if(m_bMCInit != 0){
                m_MChandle = m_MC.Create(strMp4);// 解析视频文件
                if(m_MChandle != 0){ //解析成功
                    //截图测试
                    if(0 == 1){
                        String strJpeg = file.getAbsolutePath() + "/kkkkk.jpg";
                        Log.e("WX -----","strJpeg " + " " +strJpeg); //源数据
                        int ret = m_MC.GetThumb(m_MChandle, strJpeg);
                        if(ret > 0){
                            Log.e("WX -----","截图成功 " + " " + strJpeg); //源数据
                        }else{
                            Log.e("WX -----","截图失败 " + " " + strJpeg); //源数据
                        }
                    }


                    int m_iWidth =  m_MC.GetWidth(m_MChandle);
                    int m_iHeight = m_MC.GetHeight(m_MChandle);
                    int m_iFps    = m_MC.GetFps(m_MChandle);
                    Log.e("WX size = ", m_iWidth + "x" +m_iHeight + " " + m_iFps + "fps");

                    /* 计算不同模式下输出视频的预计大小 */
                    //单位MB
                    int size0 = m_MC.GetLength(m_MChandle, m_iWidth, m_iHeight, m_iFps, m_MC.MODE_LOW);
                    int size1 = m_MC.GetLength(m_MChandle, m_iWidth, m_iHeight, m_iFps, m_MC.MODE_NORMAL);
                    int size2 = m_MC.GetLength(m_MChandle, m_iWidth, m_iHeight, m_iFps, m_MC.MODE_HIGH);
                    String str2 = "TargetSize = " + size0 + " " + size1 + " " + size2;
                    Log.e("WX --", str2);

                    //开始转换，线程或者定时器查询进度并显示
                    //开始计时
                    String strOutput = file.getAbsolutePath() + "/output2.mp4";
                    Log.e("WX -----","strOutput " + " " +strOutput); //源数据
                    m_MC.Excute(m_MChandle,m_iWidth,m_iHeight,m_iFps, size1, strOutput);
                    handler.removeCallbacks(runnable);
                    handler.postDelayed(runnable,1000);
                }
            }
        }


        FloatingActionButton fab = findViewById(R.id.fab);
        fab.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Snackbar.make(view, "Replace with your own action", Snackbar.LENGTH_LONG)
                        .setAction("Action", null).show();
            }
        });
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }
}