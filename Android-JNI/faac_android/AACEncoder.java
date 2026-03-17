package Apowersoft.WXMedia;

/**
 * AAC编码器JNI封装类
 * 依赖libfaac.so动态库，提供AAC编码核心能力
 */
public class AACEncoder {

    // 静态加载libfaac.so库（加载时机：类初始化时）
    static {
        System.loadLibrary("faac");
    }

    /**
     * 创建AAC编码器实例
     * @param samplerate 采样率（如8000、16000、44100等）
     * @param channel 声道数（1=单声道，2=双声道）
     * @return 编码器句柄（非0表示成功，0表示失败）
     */
    public native long Create(int samplerat, int channel);

    /**
     * 编码PCM数据为AAC格式
     * @param handle 编码器句柄（Create方法返回值）
     * @param in 输入PCM数据（S16格式：单声道2048字节，双声道4096字节）
     * @param out 输出AAC数据缓冲区（需提前申请，长度≥1024字节）
     * @return 实际编码后的AAC数据长度（负数表示编码失败）
     * @throws NullPointerException 输入/输出缓冲区为空时抛出
     * @throws IllegalArgumentException 输入缓冲区长度不合法时抛出
     */
    public native int Encode(long handle, byte[] in, byte[] out);

    /**
     * 销毁编码器实例，释放资源
     * @param handle 编码器句柄
     */
    public native void Destroy(long handle);
}