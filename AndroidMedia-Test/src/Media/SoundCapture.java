package Media;

import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaRecorder;

//声音采集类
public class SoundCapture  extends Thread {
	boolean m_bOpen = false;
	int bufferSizeInBytes = 0;//最小缓冲区大小
	int channelConfig = 0;//声道格式
	AudioRecord audioRecord = null;//录制对象	
	private KK_IMediaSink m_sink = null;//回调对象
	//重载线程函数
	@Override
	public void run(){
        // new一个byte数组用来存一些字节数据，大小为缓冲区大小
        byte[] audiodata = new byte[bufferSizeInBytes];
		while(m_bOpen){
            int readsize = audioRecord.read(audiodata, 0, bufferSizeInBytes);
            if (AudioRecord.ERROR_INVALID_OPERATION != readsize) {
                if(m_sink != null){
                	m_sink.OnData(audiodata, bufferSizeInBytes);//回调函数
                }
            }
		}
	}	
	//初始化函数
	public void Open(int sampleRateInHz, int channel, KK_IMediaSink sink){		
		channelConfig = (channel == 1) ? AudioFormat.CHANNEL_CONFIGURATION_MONO : AudioFormat.CHANNEL_IN_STEREO;
        // 获得缓冲区字节大小
        bufferSizeInBytes = AudioRecord.getMinBufferSize(sampleRateInHz, channelConfig, AudioFormat.ENCODING_PCM_16BIT);        // 创建AudioRecord对象
        audioRecord = new AudioRecord(MediaRecorder.AudioSource.MIC, sampleRateInHz, channelConfig,  AudioFormat.ENCODING_PCM_16BIT, bufferSizeInBytes);
        audioRecord.startRecording();
        m_sink = sink;
		m_bOpen = true;
		this.start();
	}
	public void Close(){		
		if(m_bOpen){
			m_bOpen = false;
	        audioRecord.stop();
	        audioRecord.release();//释放资源
	        audioRecord = null;
		}
	}
}
