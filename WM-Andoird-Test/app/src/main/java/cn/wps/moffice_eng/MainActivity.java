package cn.wps.moffice_eng;

import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;

import com.apowersoft.WXMedia.MediaConvert;
import com.google.android.material.floatingactionbutton.FloatingActionButton;
import com.google.android.material.snackbar.Snackbar;

import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.util.Objects;

public class MainActivity extends AppCompatActivity {
    MediaConvert m_MC = null; //转换类
    long m_MChandle = 0;//操作句柄

    int m_bMCInit = 0; //SDK初始化标记

    private File sourceFile;
    private TextView textView;

    private Handler m_handler = new Handler();
    private Runnable m_runnable = new Runnable() {
        public void run() {
            if (m_MChandle != 0) {
                int rate = m_MC.GetConvertProgress(m_MChandle);//获取进度
                if (rate == 100) {
                    textView.setText("压缩成功");
                    m_MC.DestroyConvert(m_MChandle);//销毁句柄
                    m_MChandle = 0;
                } else if (rate < 0) {
                    textView.setText("压缩失败");
                    m_MC.DestroyConvert(m_MChandle);//销毁句柄
                    m_MChandle = 0;
                } else {
                    String str = "当前转换进度 " + rate;
                    textView.setText(str);
                }
            } else {
                // textView.setText("没有转换");
            }
            m_handler.postDelayed(this, 1000);
        }
    };


    //PCCastRecv m_obj;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Toolbar toolbar = findViewById(R.id.toolbar);
        textView = findViewById(R.id.textView);
        setSupportActionBar(toolbar);

        copyVideoToSandBox();

        FloatingActionButton fab = findViewById(R.id.fab);
        fab.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                startConvert();
            }
        });
    }

    private void copyVideoToSandBox() {
        new Thread(() -> {
            try {
                String path = Objects.requireNonNull(getExternalFilesDir(null)).getAbsolutePath() + "temp";
                File file = new File(path);
                if (!file.exists()) {
                    file.mkdirs();
                }
                File destFile = new File(file, "test.mp4");
                if (destFile.exists()) {
                    Log.d("WX-----", "destFile exists: " + destFile.getAbsolutePath());
                } else {
                    // copy asset mp4 file to sandbox
                    FileOutputStream fos = new FileOutputStream(destFile);
                    InputStream fis = getAssets().open("test.mp4");
                    byte[] buffer = new byte[1024];
                    int byteCount;
                    while ((byteCount = fis.read(buffer)) != -1) {
                        fos.write(buffer, 0, byteCount);
                    }
                    fos.flush();
                    fis.close();
                    fos.close();
                    Log.d("WX-----", "copyVideoToSandBox: " + destFile.getAbsolutePath());
                }
                sourceFile = destFile;
            } catch (Exception e) {
                throw new RuntimeException(e);
            }
        }).start();
    }

    private void startConvert() {
        String strInput = sourceFile.getAbsolutePath();
        String strOutput = Objects.requireNonNull(getExternalFilesDir(null)).getAbsolutePath() + "temp" + "/1_out.mp4";
        Log.e("WX-----", "strInput " + " " + strInput); //源数据
        Log.e("WX-----", "strOutput " + " " + strOutput); //源数据
        m_MC = new MediaConvert(); //Java对象
        m_MChandle = m_MC.CreateConvert();//转换对象
        Log.e("WX-----", "m_MChandle " + " " + m_MChandle); //源数据
        int ret = m_MC.InitSourceEx(m_MChandle, strInput, strOutput,
                70, 70, 250, 250); //初始化转换参数
        Log.e("WX-----", "InitSourceEx " + " " + ret); //源数据

        if (ret > 0) {
            m_MC.StartConvert(m_MChandle);//开始转换，在别的线程轮询进度
            m_handler.removeCallbacks(m_runnable);
            m_handler.postDelayed(m_runnable, 1000);
        }
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