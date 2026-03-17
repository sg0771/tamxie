package Media;

//H264 Decoder

public class H264DecoderX {

	private int hDecoder = 0;
		
	public boolean open() {	
		if(hDecoder != 0)
			Close(hDecoder);		
		hDecoder = Open();		
		return hDecoder != 0;
	}	
	public void close() {	
		if(hDecoder != 0)
			Close(hDecoder);
		hDecoder = 0;
	}	
	public byte[] decode(byte[] in, int insize) {		
		if(hDecoder == 0)
			return null;		
		int ret = DecodeFrame(hDecoder, in, insize);		
		if(ret <= 0) 
			return null;		
		int nSize = GetWidth(hDecoder) * GetHeight(hDecoder) * 2;
		byte[] data = new byte[GetWidth(hDecoder) * GetHeight(hDecoder) * 2];		
		GetData(hDecoder, data);
		return data;
	}
	public void decode(byte[] in, int insize, byte []out) {		
		if(hDecoder == 0)
			return ;		
		int ret = DecodeFrame(hDecoder, in, insize);		
		if(ret <= 0) 
			return ;		
		GetData(hDecoder, out);
	}	
	public boolean isOpen() {		
		return hDecoder != 0;
	}
	
	public int GetWidth() {		
		if(hDecoder == 0)return 0;		
		return GetWidth(hDecoder);
	}
	
	public int GetHeight() {
		if(hDecoder == 0)return 0;
		return GetHeight(hDecoder);
	}
	
    public native int Open(); 
    public native int Close(int hDecoder);       
    public native int DecodeFrame(int h, byte[] in, int insize);
    public native int GetData(int h, byte[] out);
    public native int GetWidth(int h);
    public native int GetHeight(int h);     
    static {    	
        System.loadLibrary("AVCDecoderX");
    }
}

