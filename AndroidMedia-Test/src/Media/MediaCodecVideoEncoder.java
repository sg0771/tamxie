package Media;

import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaCodecList;
import android.media.MediaFormat;
import android.media.MediaCodecInfo.CodecCapabilities;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;

import java.nio.ByteBuffer;


class MediaCodecVideoEncoder {
	
	MediaCodec mediaCodec = null;//硬编码器
	ByteBuffer[] outputBuffers = null;
	Thread mediaCodecThread;
	
	private static String TAG = "MediaCodecVideoEncoder";
	private static final int VIDEO_ControlRateConstant = 2;
	private static final String AVC_MIME_TYPE = "video/avc";
	private static int DEQUEUE_TIMEOUT = -1;
	
	public MediaCodecVideoEncoder() {}

	public static class EncoderProperties {
		EncoderProperties(String codecName, int colorFormat, boolean bNV21) {
			this.codecName = codecName;
			this.colorFormat = colorFormat;
			this.bNV21 = bNV21;
		}
		public final String codecName;
		public final int colorFormat;
		public final boolean bNV21;
	}

	public static EncoderProperties findHwEncoder() {
	    for (int i = 0; i < MediaCodecList.getCodecCount(); i++){
	        MediaCodecInfo info = MediaCodecList.getCodecInfoAt(i);
	    	if(info.isEncoder()) continue;      
	        String[] types = info.getSupportedTypes();     
	        for(int j = 0 ; j < types.length; j++){
	        	if (types[j].equals("video/avc")){
	                MediaCodecInfo.CodecCapabilities capabilities = info.getCapabilitiesForType("video/avc");
	                int len = capabilities.colorFormats.length;
	                if(len > 0){
	                	int gFormat = capabilities.colorFormats[0];
	                	boolean bNV21 = false;
                        switch (gFormat) { 
                        case MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420SemiPlanar:
                        case MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420PackedSemiPlanar:
                        case MediaCodecInfo.CodecCapabilities.COLOR_TI_FormatYUV420PackedSemiPlanar:
                        case MediaCodecInfo.CodecCapabilities.COLOR_QCOM_FormatYUV420SemiPlanar:
                        	bNV21 = true;
                        case MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420Planar:
                        case MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420PackedPlanar:
                        	String gName = info.getName();
                        	Log.i("H264 Encodec Properties", "gName = " + gName + " format = " + Integer.toHexString(gFormat));            	
    	                	return new EncoderProperties(gName, gFormat, bNV21); //找到一个能用的就行了   
                        default:
                        		break;
                        }              		                	       	
	                }
	          	}
	        }            
	    }
	    return null;  
  }

  private static boolean isPlatformSupported() {
    return findHwEncoder() != null;
  }

  private static int bitRate(int kbps) {
    return kbps * 950;
  }

  /* 没有线程类
  private void checkOnMediaCodecThread() {
    if (mediaCodecThread.getId() != Thread.currentThread().getId()) {
      throw new RuntimeException(
          "MediaCodecVideoEncoder previously operated on " + mediaCodecThread +
          " but is now called on " + Thread.currentThread());
    }
  }
   */
  
  // Return the array of input buffers, or null on failure.
  public ByteBuffer[] initEncode(int width, int height, int kbps, int fps) {

    EncoderProperties properties = findHwEncoder();
    if (properties == null) {
      return null;
    }

    try {
      MediaFormat format = MediaFormat.createVideoFormat(AVC_MIME_TYPE, width, height);
      format.setInteger(MediaFormat.KEY_BIT_RATE, bitRate(kbps));
      format.setInteger("bitrate-mode", VIDEO_ControlRateConstant);
      format.setInteger(MediaFormat.KEY_COLOR_FORMAT, properties.colorFormat);
      format.setInteger(MediaFormat.KEY_FRAME_RATE, fps);
      format.setInteger(MediaFormat.KEY_I_FRAME_INTERVAL, 10);
      mediaCodec = MediaCodec.createByCodecName(properties.codecName);
      mediaCodec.configure(format, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE);
      mediaCodec.start();
      outputBuffers = mediaCodec.getOutputBuffers();//输出缓冲
      ByteBuffer[] inputBuffers = mediaCodec.getInputBuffers();//输入缓冲
      return inputBuffers;
    } catch (IllegalStateException e) {
      Log.e(TAG, "initEncode failed", e);
      return null;
    }
  }

  public boolean encode(boolean isKeyframe, int inputBuffer, int size, long presentationTimestampUs) {
    //checkOnMediaCodecThread();
    try {
      if (isKeyframe) {
        Log.d(TAG, "Sync frame request");
        Bundle b = new Bundle();
        b.putInt(MediaCodec.PARAMETER_KEY_REQUEST_SYNC_FRAME, 0);
        mediaCodec.setParameters(b);
      }
      mediaCodec.queueInputBuffer(inputBuffer, 0, size, presentationTimestampUs, 0);
      return true;
    }
    catch (IllegalStateException e) {
      Log.e(TAG, "encode failed", e);
      return false;
    }
  }

  private void release() {
    Log.d(TAG, "Java releaseEncoder");
    //checkOnMediaCodecThread();
    try {
      mediaCodec.stop();
      mediaCodec.release();
    } catch (IllegalStateException e) {
      Log.e(TAG, "release failed", e);
    }
    mediaCodec = null;
    //mediaCodecThread = null;
  }

  private boolean setRates(int kbps, int frameRateIgnored) {

    //checkOnMediaCodecThread();
    Log.v(TAG, "setRates: " + kbps + " kbps. Fps: " + frameRateIgnored);
    try {
      Bundle params = new Bundle();
      params.putInt(MediaCodec.PARAMETER_KEY_VIDEO_BITRATE, bitRate(kbps));
      mediaCodec.setParameters(params);
      return true;
    } catch (IllegalStateException e) {
      Log.e(TAG, "setRates failed", e);
      return false;
    }
  }

  //从队列释放数据
  //找到可用的输入缓冲索引值
  //编码数据输入由上层控制
  public int dequeueInputBuffer() {
    try {
      return mediaCodec.dequeueInputBuffer(DEQUEUE_TIMEOUT);
    } catch (IllegalStateException e) {
      Log.e(TAG, "dequeueIntputBuffer failed", e);
      return -2;
    }
  }

  // Helper struct for dequeueOutputBuffer() below.
  private static class OutputBufferInfo {
    public OutputBufferInfo(
        int index, ByteBuffer buffer, boolean isKeyFrame,
        long presentationTimestampUs) {
      this.index = index;
      this.buffer = buffer;
      this.isKeyFrame = isKeyFrame;
      this.presentationTimestampUs = presentationTimestampUs;
    }

    private final int index;
    private final ByteBuffer buffer;
    private final boolean isKeyFrame;
    private final long presentationTimestampUs;
  }

  // Dequeue and return an output buffer, or null if no output is ready.  Return
  // a fake OutputBufferInfo with index -1 if the codec is no longer operable.
  private OutputBufferInfo dequeueOutputBuffer() {
    //checkOnMediaCodecThread();
    try {
      MediaCodec.BufferInfo info = new MediaCodec.BufferInfo();
      int result = mediaCodec.dequeueOutputBuffer(info, DEQUEUE_TIMEOUT);
      if (result >= 0) {
        // MediaCodec doesn't care about Buffer position/remaining/etc so we can
        // mess with them to get a slice and avoid having to pass extra
        // (BufferInfo-related) parameters back to C++.
        ByteBuffer outputBuffer = outputBuffers[result].duplicate();
        outputBuffer.position(info.offset);
        outputBuffer.limit(info.offset + info.size);
        boolean isKeyFrame =
            (info.flags & MediaCodec.BUFFER_FLAG_SYNC_FRAME) != 0;
        if (isKeyFrame) {
          Log.d(TAG, "Sync frame generated");
        }
        return new OutputBufferInfo(
            result, outputBuffer.slice(), isKeyFrame, info.presentationTimeUs);
      } else if (result == MediaCodec.INFO_OUTPUT_BUFFERS_CHANGED) {
        outputBuffers = mediaCodec.getOutputBuffers();
        return dequeueOutputBuffer();
      } else if (result == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED) {
        return dequeueOutputBuffer();
      } else if (result == MediaCodec.INFO_TRY_AGAIN_LATER) {
        return null;
      }
      throw new RuntimeException("dequeueOutputBuffer: " + result);
    } catch (IllegalStateException e) {
      Log.e(TAG, "dequeueOutputBuffer failed", e);
      return new OutputBufferInfo(-1, null, false, -1);
    }
  }

  private boolean releaseOutputBuffer(int index) {
    //checkOnMediaCodecThread();
    try {
      mediaCodec.releaseOutputBuffer(index, false);
      return true;
    } catch (IllegalStateException e) {
      Log.e(TAG, "releaseOutputBuffer failed", e);
      return false;
    }
  }
}
