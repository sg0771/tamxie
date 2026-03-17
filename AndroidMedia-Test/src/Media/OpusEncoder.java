package Media;

public class OpusEncoder extends Opus {
	
	public byte []m_pout;
	int m_h = 0;

	public void open(){
		m_pout = new byte[2000];
		m_h = Open();
	}
	
	public int encode(byte in[], int size){
		if(m_h == 0)return 0;
		int length = Encode(m_h, in, size, m_pout);
		return length;
	}
	
	public void close(){
		Close(m_h);
		m_h = 0;
	}
		
	native int Open();
	native int Encode(int handle, byte in[], int size, byte out[]);
	native void Close(int handle);	
}
