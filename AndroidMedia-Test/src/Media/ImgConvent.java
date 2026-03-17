package Media;

public class ImgConvent {
	
    public static void swapYV12toYUV420P(byte[] yv12bytes, byte[] dst, int width, int height) 
    {      
    	System.arraycopy(yv12bytes, 0, dst, 0,width*height);
    	System.arraycopy(yv12bytes, width * height, dst, width * height * 5 / 4, width * height / 4);
    	System.arraycopy(yv12bytes, width * height * 5 / 4, dst, width * height, width * height / 4);
    }	
    public static void swapYV12toYUV420SP(byte[] yv12bytes, byte[] dst, int width, int height) 
    {      
    	System.arraycopy(yv12bytes, 0, dst, 0,width*height);
    	int uoffset = width * height;
    	int voffset = width * height * 5 / 4;
    	for(int i = 0; i < uoffset / 4; i++)
    	{
    		dst[uoffset + i * 2]     = yv12bytes[uoffset + i];
       		dst[uoffset + i * 2 + 1] = yv12bytes[voffset + i];       	   		
    	}
    }  
    public static void swapNV21toYUV420P(byte[] nv21bytes, byte[] dst, int width, int height) 
    {      
    	System.arraycopy(nv21bytes, 0, dst, 0, width*height);   	
    	int uoffset = width * height;
    	int voffset = width * height * 5 / 4;
    	for(int i = 0; i < uoffset / 4; i++)
    	{
    		dst[uoffset + i] = nv21bytes[uoffset + i * 2 + 1];
       		dst[voffset + i] = nv21bytes[uoffset + i * 2];       	   		
    	}
    }
    public static void swapNV21toYUV420SP(byte[] nv21bytes, byte[] dst, int width, int height) 
    {      
    	System.arraycopy(nv21bytes, 0, dst, 0, width*height);   	
    	System.arraycopy(nv21bytes, width*height, dst, width*height + 1, width*height / 2 - 1);
    } 
}
