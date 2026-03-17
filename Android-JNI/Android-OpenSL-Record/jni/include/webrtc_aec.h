#ifndef _WEBRTC_AEC_H_
#define _WEBRTC_AEC_H_

// Errors
#define AEC_UNSPECIFIED_ERROR           12000
#define AEC_UNSUPPORTED_FUNCTION_ERROR  12001
#define AEC_UNINITIALIZED_ERROR         12002
#define AEC_NULL_POINTER_ERROR          12003
#define AEC_BAD_PARAMETER_ERROR         12004

// Warnings
#define AEC_BAD_PARAMETER_WARNING       12050

enum {
    kAecNlpConservative = 0,
    kAecNlpModerate,
    kAecNlpAggressive
};

enum {
    kAecFalse = 0,
    kAecTrue
};

typedef struct {
    short nlpMode;        // default kAecNlpModerate
    short skewMode;       // default kAecFalse
    short metricsMode;    // default kAecFalse
    int delay_logging;            // default kAecFalse
    //float realSkew;
} AecConfig;

typedef struct {
    short instant;
    short average;
    short max;
    short min;
} AecLevel;

typedef struct {
    AecLevel rerl;
    AecLevel erl;
    AecLevel erle;
    AecLevel aNlp;
} AecMetrics;

#ifdef __cplusplus
extern "C" {
#endif


int WebRtcAec_Create(void **aecInst);
int WebRtcAec_Free(void *aecInst);

//初始化 
//采集频率
//参考频率
int WebRtcAec_Init(void *aecInst,int sampFreq, int scSampFreq);

//填充参考数据 far[size]
int WebRtcAec_BufferFarend(void *aecInst,const short *farend,short nrOfSamples);

//回音消除函数
int WebRtcAec_Process(void *aecInst,
                      const short *nearend,  //采集端低频
                      const short *nearendH, //采集端高频，可以填NULL
                      short *out,			 //输出低频
                      short *outH,           //输出高频，可以填NULL
                      short nrOfSamples,     //每个声音样本的数据量
					  short msInSndCardBuf,  //采集与回放的时间差，填100测试一下
                      int skew);//填0

//配置，参考webrtc写法
int WebRtcAec_set_config(void *aecInst, AecConfig config);
int WebRtcAec_get_config(void *aecInst, AecConfig *config);

//不知道干什么的，无视
int WebRtcAec_get_echo_status(void *aecInst, short *status);
int WebRtcAec_GetMetrics(void *aecInst, AecMetrics *metrics);
int WebRtcAec_GetDelayMetrics(void* handle, int* median, int* std);
int WebRtcAec_get_error_code(void *aecInst);

#ifdef __cplusplus
}
#endif
#endif  // WEBRTC_MODULES_AUDIO_PROCESSING_AEC_INCLUDE_ECHO_CANCELLATION_H_
