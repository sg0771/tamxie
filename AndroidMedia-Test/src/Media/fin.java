package Media;

import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.RandomAccessFile;

import android.util.Log;

public class fin {
	RandomAccessFile m_raf = null;//∂¡÷∏’Î
	public boolean Open(String fm){
		try {
			m_raf = new RandomAccessFile(fm, "r");
		} catch (FileNotFoundException e) {
			Log.e("ftanx", "fin open error");
			return false;
		}
		return m_raf != null;
	}
	public boolean Read(byte []buf, int size){
		try {
			int length = m_raf.read(buf, 0, size);
			return length == size;
		} catch (IOException e) {
			Log.e("ftanx", "file IO Error!");
			return false;
		}
	}
	public  int ReadLength()
	{
		int length = 0;
		try {
			length = m_raf.readInt();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			//e.printStackTrace();
			Close();
			Log.e("No", "IO error");
		}
		return length;
	}
	public void Close(){
		if(m_raf != null){
			try {
				m_raf.close();
				m_raf = null;
			} catch (IOException e) {
				// TODO Auto-generated catch block
				//e.printStackTrace();
				Log.e("ftanx", "file IO Error!");
			}
		}
	}
}
