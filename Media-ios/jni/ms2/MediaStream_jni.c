#include <jni.h>

#include <mediastreamer2/mscommon.h>
#include <mediastreamer2/msticker.h>
#include <mediastreamer2/msfilter.h>
#include <mediastreamer2/mssndcard.h>
#include "ms2.h"

static JavaVM *jvm=0;
jint JNI_OnLoad(JavaVM *ajvm, void *reserved){
	ms_warning("JNI_OnLoad");
	ms_set_jvm(ajvm);
	jvm=ajvm;
	ms_voip_init();
	return JNI_VERSION_1_2;
}

JNIEXPORT int Java_com_mediastream_AudioStream_Create(JNIEnv* env, jclass thiz, int bPCM) {
	ms_warning("xCreate start");
	MSAudioStream* stream = MSAudioStream_Create(bPCM);//先用PCM测试效果
	ms_warning("xCreate  end");
	return (jint)stream;
}

JNIEXPORT void Java_com_mediastream_AudioStream_Destroy(JNIEnv* env, jclass thiz, int handle){
	ms_warning("xDestroy");
	MSAudioStream* stream = (MSAudioStream*)handle;
	MSAudioStream_Destroy(stream);
	handle = 0;
}

JNIEXPORT int Java_com_mediastream_AudioStream_AddNode(JNIEnv* env, jclass thiz, int handle){
	ms_warning("xAddNode");
	MSAudioStream* stream = (MSAudioStream*)handle;
	return MSAudioStream_Receive_Add(stream);
}

JNIEXPORT void Java_com_mediastream_AudioStream_RemoveNode(JNIEnv* env, jclass thiz, int handle,int channel){
	ms_warning("xRemoveNode");
	MSAudioStream* stream = (MSAudioStream*)handle;
	return MSAudioStream_Receive_Remove(stream,channel);
}

//写入数据
JNIEXPORT void Java_com_mediastream_AudioStream_WriteData(JNIEnv* env, jclass thiz, int handle,int channel, jbyteArray Input, int size){
	//ms_warning("WriteData chan[%d] size= %d",channel,size);
	MSAudioStream* stream = (MSAudioStream*)handle;
	jbyte  * buf   =   (*env)-> GetByteArrayElements(env, Input,   NULL);
	MSAudioStream_Receive_WriteData(stream,channel, (uint8_t*)buf, size);
	(*env)-> ReleaseByteArrayElements(env, Input,   buf, 0);
}
//获取数据
JNIEXPORT int Java_com_mediastream_AudioStream_GetData(JNIEnv* env, jclass thiz, int handle,jbyteArray Output){
	MSAudioStream* stream = (MSAudioStream*)handle;
	jbyte  * buf   =   (*env)-> GetByteArrayElements(env, Output,   NULL);
	int len = MsAudioStream_Sender_GetData(stream,(uint8_t*)buf);
	(*env)-> ReleaseByteArrayElements(env, Output,   buf, 0);
	return len;
}

//获取数据
JNIEXPORT int Java_com_mediastream_AudioStream_GetMax(JNIEnv* env, jclass thiz, int handle, int channel){
	MSAudioStream* stream = (MSAudioStream*)handle;
	int value = MSAudioStream_Receive_GetMax(stream,channel);
	return value;
}

//获取数据
JNIEXPORT int Java_com_mediastream_AudioStream_GetLocalMax(JNIEnv* env, jclass thiz, int handle){
	MSAudioStream* stream = (MSAudioStream*)handle;
	int value = MSAudioStream_Receive_GetLocalMax(stream);
	return value;
}
