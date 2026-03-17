/*
Android OpenSLES  16000 Mono 16Bit 录音
每个包10ms
*/

#include <jni.h>
#include <string>

#include <stdint.h>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <android/log.h>

#include <webrtc_ns.h>
#include <webrtc_vad.h>

#define  LOG_DEBUG true
#define  LOGD(FORMAT,...)__android_log_print(ANDROID_LOG_DEBUG,"JniThread",FORMAT, ##__VA_ARGS__);
#define  LOGE(FORMAT,...)__android_log_print(ANDROID_LOG_ERROR,"JniThread",FORMAT, ##__VA_ARGS__);

#include "faac.h"

class FAACEncoder
{
private:
	faacEncHandle faac_handle_ = nullptr;//编码器
	unsigned long input_sams_ = 0;//每次输入的sample数量
	unsigned long max_output_bytes_ = 0;//最大输出
	uint8_t m_pOut[4096];//编码缓冲区域
public:
	int InputSamples() { return (int)input_sams_; }

	int MaxOutBytes() { return (int)max_output_bytes_; }

	void Open(unsigned int samRate, unsigned int channels){
		faac_handle_ = faacEncOpen(samRate, channels, &input_sams_, &max_output_bytes_);
		faacEncConfigurationPtr enc_cfg = faacEncGetCurrentConfiguration(faac_handle_);
		enc_cfg->inputFormat = FAAC_INPUT_16BIT;
		enc_cfg->mpegVersion = MPEG2;
		enc_cfg->aacObjectType = LOW;
		enc_cfg->allowMidside = 1;
		enc_cfg->useLfe = 0;
		enc_cfg->useTns = 0;
		enc_cfg->bitRate = samRate * channels * 16 / 5;
		enc_cfg->quantqual = 100;
		enc_cfg->bandWidth = 0;
		enc_cfg->outputFormat = 1; //ADTS
		faacEncSetConfiguration(faac_handle_, enc_cfg);
	}

	void Encode(int16_t* inputBuf, int samCount, uint8_t*& outBuf, int& bufSize){
		outBuf = nullptr;
		bufSize = 0;
		int size = faacEncEncode(faac_handle_, (int*)inputBuf, samCount, m_pOut, max_output_bytes_);
		if (size > 0) {
			outBuf = m_pOut;
			bufSize = size;
		}
	}

	void Close(){
		if (faac_handle_) {
			faacEncClose(faac_handle_);
			faac_handle_ = nullptr;
		}
	}
};


//内存数据管理
class WXDataBuffer {
public:
	uint8_t *m_pBuf = nullptr;
	int     m_iBufSize = 0;
	int     m_iPos = 0;
	int64_t extra1 = 0;
	int64_t extra2 = 0;
public:
	void Init(uint8_t *buf, int size) {
		if (m_pBuf)delete[]m_pBuf;
		m_pBuf = new uint8_t[size];
		if (buf != nullptr) {
			memcpy(m_pBuf, buf, size);
		}else {
			memset(m_pBuf, 0, size);
		}
		m_iBufSize = size;
	}

	WXDataBuffer() {}

	WXDataBuffer(uint8_t *buf, int size) {
		Init(buf, size);
	}

	virtual ~WXDataBuffer() {
		if (m_pBuf) {
			delete[]m_pBuf;
			m_pBuf = nullptr;
			m_iBufSize = 0;
		}
	}
};

//单向队列
class WXFifo {
public:
	volatile int m_bEnable = 1;//是否可写
	WXDataBuffer m_dataBuffer;
	int64_t m_nTotalSize = 192000;
	int64_t m_nPosRead = 0;//读位置
	int64_t m_nPosWrite = 0;//写位置
public:
	inline int64_t Size() { return m_nPosWrite - m_nPosRead; }
public:
	WXFifo(int totolsize = 192000) {
		m_dataBuffer.Init(nullptr, totolsize);
		m_nTotalSize = totolsize;
	}

	void Init(int totolsize = 192000) {
		m_dataBuffer.Init(nullptr, totolsize);
		m_nTotalSize = totolsize;
	}

	void Reset() {
		m_nPosRead = 0;
		m_nPosWrite = 0;
		memset(m_dataBuffer.m_pBuf, 0, m_dataBuffer.m_iBufSize);
	}

	virtual ~WXFifo() {
		m_nPosRead = 0;
		m_nPosWrite = 0;
	}

	void Write(uint8_t *pBuf, int nSize) { //写数据
		if (m_bEnable) {
			int64_t newSize = m_nPosWrite - m_nPosRead + nSize;//可写区域
			if (newSize < m_nTotalSize) { //数据区可写
				int64_t posWirte = m_nPosWrite % m_nTotalSize;//实际写入位置
				int64_t posLeft = m_nTotalSize - posWirte;//在实际位置上buffer还剩多少可以写入的空间
				if (posLeft >= nSize) { //数据区足够
					memcpy(m_dataBuffer.m_pBuf + posWirte, pBuf, nSize);
				}else {  //有部分数据需要写到RingBuffer的头部
					memcpy(m_dataBuffer.m_pBuf + posWirte, pBuf, posLeft);//写到RingBuffer尾部
					memcpy(m_dataBuffer.m_pBuf, pBuf + posLeft, nSize - posLeft);//写到RingBuffer头部
				}
				m_nPosWrite += nSize;//更新写位置
			}
		}
	}

	int Read(uint8_t *pBuf, int nSize) {//读数据
		int64_t nowSize = m_nPosWrite - m_nPosRead;//可读区域
		if (nSize && nowSize >= nSize) { //数据足够读
			if (pBuf) {
				int64_t posRead = m_nPosRead % m_nTotalSize;//实际读取位置
				int64_t posLeft = m_nTotalSize - posRead;//在实际位置上buffer还剩多少可以写入的空间
				if (posLeft >= nSize) { //数据区足够
					memcpy(pBuf, m_dataBuffer.m_pBuf + posRead, nSize);
				}
				else {  //有部分数据需要写到RingBuffer的头部
					memcpy(pBuf, m_dataBuffer.m_pBuf + posRead, posLeft);//从尾部拷贝数据
					memcpy(pBuf + posLeft, m_dataBuffer.m_pBuf, nSize - posLeft);//从头部拷贝数据
				}
			}
			m_nPosRead += nSize;
			return nSize;
		}
		return 0;
	}

	int Read2(uint8_t *buf, int size) {//buf=nullptr Skip
		memset(buf, 0, size);
		int64_t nowSize = m_nPosWrite - m_nPosRead;//有效数据区
		if (nowSize >= size) {
			return Read(buf, size);
		}else if(nowSize > 0){
			return Read(buf, nowSize);
		}
		return 0;
	}
};

//全局？
static SLObjectItf   g_slObjectEngine = NULL;//用SLObjectltf声明引擎接口对象
static SLEngineItf  g_engineItf = NULL;//声明具体的引擎对象实例
void   bgRecorderCallback(SLAndroidSimpleBufferQueueItf bg, void *context);//录制回调函数

class SLRecord {
public:
	SLObjectItf  m_recordObj = NULL;//用SLObjectltf声明引擎接口对象
	SLRecordItf  m_recordItf = NULL;
	SLAndroidSimpleBufferQueueItf  m_recorderBufferQueue = NULL;//Buffer接口
	
	int16_t m_recBuffer[2][160];
	int m_nIndex = -1;
	
	int16_t m_temp[160];	
	NsHandle *m_pNsInst = nullptr;//NS
	VadInst* m_pVadInst = nullptr;//VAD

	WXFifo m_fifo;
	WXDataBuffer m_bufAac;//AAC 编码帧 1024*2
	FAACEncoder m_aac;
	
	FILE *m_pFout = NULL;
	bool m_finish = false;
	int  Start(const char *fileName) {
		m_pFout = fopen(fileName, "wb");
		if (m_pFout) {
			
			if (1) { //降噪处理
				int ret = WebRtcNs_Create(&m_pNsInst);
				ret = WebRtcNs_Init(m_pNsInst, 16000);
				ret = WebRtcNs_set_policy(m_pNsInst, 1);
			}

			if (1) { //人声失败
				int ret = WebRtcVad_Create(&m_pVadInst);
				ret = WebRtcVad_Init(m_pVadInst);
				ret = WebRtcVad_set_mode(m_pVadInst, 2);
			}
		
		
			if (g_slObjectEngine == NULL) {
				//第一步：创建引擎
				slCreateEngine(&g_slObjectEngine, 0, NULL, 0, NULL, NULL);
				//第二步：实现(Realize)engineObject,SL_BOOLEAN_FALSE);实例化这个对象
				(*g_slObjectEngine)->Realize(g_slObjectEngine, SL_BOOLEAN_FALSE);
				//第三步：通过engineObject的GetInterface方法初始化enngineEngine,从这个对象里面获取引擎接口
				(*g_slObjectEngine)->GetInterface(g_slObjectEngine, SL_IID_ENGINE, &g_engineItf);
			}


			//4. 设置IO设备(麦克风)
			SLDataLocator_IODevice loc_dev = {
				SL_DATALOCATOR_IODEVICE,//类型
				SL_IODEVICE_AUDIOINPUT,//device类型 选择了音频输入类型
				SL_DEFAULTDEVICEID_AUDIOINPUT,//deviceID
				NULL            //device实例
			};

			SLDataSource audioStr = {
				&loc_dev, //SLDataLocator_IODevice配置输入
				NULL      //输入格式,采集的并不需要
			};
			//5. 设置输出buffer队列
			SLDataLocator_AndroidSimpleBufferQueue loc_bq = {
				SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,//类型 这里只能是这个常量
				2,//buffer的数量

			};
			//6. 设置输出数据的格式
			SLDataFormat_PCM format_pcm = {
				SL_DATAFORMAT_PCM,////输出PCM格式的数据
				(SLuint32)1,//输出的声道数量
				SL_SAMPLINGRATE_16,//输出的采样频率，这里是44100Hz
				SL_PCMSAMPLEFORMAT_FIXED_16,//输出的采样格式，这里是16bit
				SL_PCMSAMPLEFORMAT_FIXED_16,//一般来说，跟随上一个参数
				SL_SPEAKER_FRONT_CENTER,//双声道配置，如果单声道可以用 SL_SPEAKER_FRONT_CENTER
				SL_BYTEORDER_LITTLEENDIAN//PCM数据的大小端排列
			};

			SLDataSink audioSink = {
				&loc_bq,     //SLDataFormat_PCM配置输出
				&format_pcm  //输出数据格式
			};


			//7. 创建录制的对象
			const SLInterfaceID  id[1] = { SL_IID_ANDROIDSIMPLEBUFFERQUEUE };
			const SLboolean req[1] = { SL_BOOLEAN_TRUE };

			//创建录音器
			(*g_engineItf)->CreateAudioRecorder(
				g_engineItf,  //引擎接口
				&m_recordObj, //录制对象地址，用于传出对象
				&audioStr,  //输入配置
				&audioSink,  //输出配置
				1,            //支持的接口数量
				id,          //具体的要支持的接口
				req          //具体的要支持的接口是开放的还是关闭的
			);
			////8. 实例化这个录制对象
			(*m_recordObj)->Realize(m_recordObj, SL_BOOLEAN_FALSE);
			//9. 获取录制接口
			(*m_recordObj)->GetInterface(m_recordObj, SL_IID_RECORD, &m_recordItf);
			//10. 获取Buffer接口
			(*m_recordObj)->GetInterface(m_recordObj, SL_IID_ANDROIDSIMPLEBUFFERQUEUE, &m_recorderBufferQueue);

			m_nIndex++;
			(*m_recorderBufferQueue)->Enqueue(m_recorderBufferQueue, (const void*)m_recBuffer[m_nIndex] , 160 * 2);

			(*m_recorderBufferQueue)->RegisterCallback(m_recorderBufferQueue, bgRecorderCallback, this);
			//11. 开始录音
			(*m_recordItf)->SetRecordState(m_recordItf, SL_RECORDSTATE_RECORDING);
			
			m_aac.Open(16000,1);
			m_bufAac.Init(nullptr,2048);
			return 0;
		}
		return -1;
	}
	
	void Stop() {
		if (NULL != m_recordItf) {
			m_finish = true;
		}
	}
	
	void StopImpl(){
			//刷新缓冲区后，关闭流
			(*m_recordItf)->SetRecordState(m_recordItf, SL_RECORDSTATE_STOPPED);
			(*m_recorderBufferQueue)->Enqueue(m_recorderBufferQueue, m_recBuffer[0], 0);
			(*m_recorderBufferQueue)->Enqueue(m_recorderBufferQueue, m_recBuffer[1], 0);
			(*m_recorderBufferQueue)->Clear(m_recorderBufferQueue);
			if (m_recordObj != NULL) {
				(*m_recordObj)->Destroy(m_recordObj);
				m_recordObj = NULL;
				m_recordItf = NULL;
				m_recorderBufferQueue = NULL;
			}

			fclose(m_pFout);
			m_pFout = NULL;
			LOGE("录制完成");
			m_aac.Close();
			if (m_pNsInst) {
				WebRtcNs_Free(m_pNsInst);
				m_pNsInst = nullptr;
			}
			if (m_pVadInst) {
				WebRtcVad_Free(m_pVadInst);
				m_pVadInst = nullptr;
			}	
	}
	
	void CbFunc() {

		if (m_pNsInst) { //降噪处理
			int ret = WebRtcNs_Process(m_pNsInst, m_recBuffer[m_nIndex], NULL, m_temp, NULL);
			memcpy(m_recBuffer[m_nIndex], m_recBuffer[m_nIndex], 160 * 2);
		}
		
		if (m_pVadInst) { //人声检测
			int ret = WebRtcVad_Process(m_pVadInst, 16000, m_recBuffer[m_nIndex], 160);
			if (ret != 1) { //背景声
				int16_t *pcm = (int16_t *)m_recBuffer[m_nIndex];
				for (int i = 0; i < 160; i++)
					pcm[i] = pcm[i] / 2;
			}
		}
			
		//
		m_fifo.Write((uint8_t*)m_recBuffer[m_nIndex], 160 * 2);
		if(m_fifo.Size() >= 2048){
			m_fifo.Read(m_bufAac.m_pBuf,2048);
			uint8_t *aacData = nullptr;
			int aacSize = 0;
			m_aac.Encode((int16_t*)m_bufAac.m_pBuf,1024,aacData,aacSize);
			if(aacSize > 0 ){
				fwrite((const void*)aacData, 1, aacSize, m_pFout);
			}
		}
		//fwrite((const void*)m_recBuffer[m_nIndex], 2, 160, m_pFout);

		if (m_finish){
			StopImpl();
		}else {
			m_nIndex++;
			m_nIndex = m_nIndex % 2;
			(*m_recorderBufferQueue)->Enqueue(m_recorderBufferQueue, (const void*)m_recBuffer[m_nIndex], 160 * 2);
			//LOGE("正在录制");
		};
	}
};

//取出缓冲数据回调
void bgRecorderCallback(SLAndroidSimpleBufferQueueItf bg, void *context) {
	SLRecord *obj = (SLRecord*)context;
	obj->CbFunc();
}

extern "C" JNIEXPORT jlong JNICALL Java_com_apowersoft_SLRecord_Start(JNIEnv *env, jobject instance,
	jstring path_) {
	SLRecord *obj = new SLRecord;
	const char *path = env->GetStringUTFChars(path_, 0);
	int ret = obj->Start(path);
	env->ReleaseStringUTFChars(path_, path);
	if (ret == 0) {
		return (jlong)obj;
	}else {
		delete obj;
		return 0;
	}
}

extern "C" JNIEXPORT void JNICALL Java_com_apowersoft_SLRecord_Stop(JNIEnv *env, jobject instance, jlong handle) {
	SLRecord *obj =  (SLRecord*)handle;
	obj->Stop();
}