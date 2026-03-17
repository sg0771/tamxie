/*
WXImage 封装为 JNI 接口
*/

#ifndef _ANDROID

#include <jni.h>
#include <android/log.h>  

#include "WXImage.h"

#ifndef EXTERN_C
#define EXTERN_C extern "C"
#endif

#define  LOGD(FORMAT, ...)   __android_log_print(ANDROID_LOG_DEBUG,  "WXImage_Jni",FORMAT, ##__VA_ARGS__);
#define  LOGE(FORMAT, ...)   __android_log_print(ANDROID_LOG_ERROR,  "WXImage_Jni",FORMAT, ##__VA_ARGS__);

EXTERN_C JNIEXPORT void JNICALL Java_com_apowersoft_WXImage_SetLog(JNIEnv* env, jobject jobj, jint bUse) {
	LOGE("%s", __FUNCTION__);
	WXImage_SetLog(bUse);
}


EXTERN_C JNIEXPORT jlong JNICALL Java_com_apowersoft_WXImage_HandlerCreate(JNIEnv* env, jobject jobj) {
	LOGE("%s", __FUNCTION__);
	return (jlong)HandlerCreate();
}

EXTERN_C JNIEXPORT jint JNICALL Java_com_apowersoft_WXImage_HandlerGetData(JNIEnv* env, jobject jobj, jlong handle, jbyteArray jbuf) {
	LOGE("%s", __FUNCTION__);
	uint8_t* buf = (uint8_t*)(env->GetByteArrayElements(jbuf, 0));
	int ret =  (jint)HandlerGetData((void*)handle, buf);
	env->ReleaseByteArrayElements(jbuf, (jbyte*)buf, 0);
	return ret;
}


EXTERN_C JNIEXPORT jint JNICALL Java_com_apowersoft_WXImage_HandlerGetSize(JNIEnv* env, jobject jobj, jlong handle) {
	LOGE("%s", __FUNCTION__);
	return (jint)HandlerGetSize((void*)handle);
}


EXTERN_C JNIEXPORT jint JNICALL Java_com_apowersoft_WXImage_HandlerDestroy(JNIEnv * env, jobject jobj, jlong handle) {
	LOGE("%s", __FUNCTION__);
	return (jint)HandlerDestroy((void*)handle);
}

EXTERN_C JNIEXPORT jint JNICALL Java_com_apowersoft_WXImage_SetQuality(JNIEnv* env, jobject jobj, jint nMinQuality, jint nDefaultQuality, jint nMaxQuality){
	LOGE("%s", __FUNCTION__);
	return (jint)SetQuality(nMinQuality, nDefaultQuality, nMaxQuality);
}

EXTERN_C JNIEXPORT jint Java_com_apowersoft_WXImage_CompressQualityBufferToBuffer(JNIEnv* env, jobject jobj,
	jbyteArray jbuf, jint buf_size,
	jlong handle, jint target_type, jint target_level, jint dst_width, jint dst_height) {
	LOGE("%s", __FUNCTION__);
	uint8_t* buf = (uint8_t*)(env->GetByteArrayElements(jbuf, 0));
	int ret = CompressQuality_BufferToBuffer(buf, buf_size,
		(void*)handle, target_type, target_level, dst_width, dst_height);
	env->ReleaseByteArrayElements(jbuf, (jbyte*)buf, 0);
	return ret;
}

EXTERN_C JNIEXPORT jint Java_com_apowersoft_WXImage_CompressQualityFileToFile(JNIEnv* env, jobject jobj, 
	jstring jstrInput, jstring jstrOutput,
	jint target_type, jint target_level, jint dst_width, jint dst_height) {
	LOGE("%s", __FUNCTION__);
	const char* strInput = (const char*)(env->GetStringUTFChars(jstrInput, 0));
	const char* strOutput = (const char*)(env->GetStringUTFChars(jstrOutput, 0));
	int ret = CompressQuality_FileToFile(strInput, strOutput, target_type, target_level, dst_width, dst_height);
	env->ReleaseStringUTFChars(jstrInput, strInput);
	env->ReleaseStringUTFChars(jstrOutput, strOutput);
	return ret;
}

EXTERN_C JNIEXPORT jint Java_com_apowersoft_WXImage_CompressQualityFileToBuffer(JNIEnv* env, jobject jobj, 
	jstring jstrInput, jlong handle,
	int target_type, int target_level, int dst_width, int dst_height)
{
	LOGE("%s", __FUNCTION__);
	const char* strInput = (const char*)(env->GetStringUTFChars(jstrInput, 0));
	int ret =  CompressQuality_FileToBuffer(strInput, (void*) handle,target_type,  target_level,  dst_width,  dst_height);
	env->ReleaseStringUTFChars(jstrInput, strInput);
	return ret;
 }


EXTERN_C JNIEXPORT jint Java_com_apowersoft_WXImage_CompressQualityBufferToFile(JNIEnv* env, jobject jobj,
	jbyteArray jbuf, jint buf_size, jstring jstrOutput,
	jint target_type, jint target_level, jint dst_width, jint dst_height) {
	LOGE("%s", __FUNCTION__);
	uint8_t* buf = (uint8_t*)(env->GetByteArrayElements(jbuf, 0));
	const char* strOutput = (const char*)(env->GetStringUTFChars(jstrOutput, 0));
	int ret = CompressQuality_BufferToFile(buf,  buf_size,strOutput,target_type,  target_level,  dst_width,  dst_height);
	env->ReleaseStringUTFChars(jstrOutput, strOutput);
	env->ReleaseByteArrayElements(jbuf, (jbyte*)buf, 0);
	return ret;
}


EXTERN_C JNIEXPORT jint Java_com_apowersoft_WXImage_CompressSizeBufferToBuffer(JNIEnv* env, jobject jobj, 
	jbyteArray jbuf, jint buf_size, jlong handle,
	jint target_type, jint target_size, jint dst_width, jint dst_height) {
	LOGE("%s", __FUNCTION__);
	uint8_t* buf = (uint8_t*)(env->GetByteArrayElements(jbuf, 0));
	int ret = CompressSize_BufferToBuffer(buf,  buf_size, (void*) handle,
		 target_type,  target_size,  dst_width,  dst_height);
	env->ReleaseByteArrayElements(jbuf, (jbyte*)buf, 0);
	return ret;
}

EXTERN_C JNIEXPORT jint Java_com_apowersoft_WXImage_CompressSizeFileToFile(JNIEnv* env, jobject jobj, 
	jstring jstrInput, jstring jstrOutput,
	jint target_type, jint target_size, jint dst_width, jint dst_height) {
	LOGE("%s", __FUNCTION__);

	const char* strInput = (const char*)(env->GetStringUTFChars(jstrInput, 0));
	const char* strOutput = (const char*)(env->GetStringUTFChars(jstrOutput, 0));
	int ret = CompressSize_FileToFile( strInput, strOutput,
		 target_type,  target_size,  dst_width,  dst_height);
	env->ReleaseStringUTFChars(jstrInput, strInput);
	env->ReleaseStringUTFChars(jstrOutput, strOutput);
	return ret;
}

EXTERN_C JNIEXPORT jint Java_com_apowersoft_WXImage_CompressSizeBufferToFile(JNIEnv* env, jobject jobj, 
	jbyteArray jbuf, jint buf_size, jstring jstrOutput,
	jint target_type, jint target_size, jint dst_width, jint dst_height) {
	LOGE("%s", __FUNCTION__);

	uint8_t* buf = (uint8_t*)(env->GetByteArrayElements(jbuf, 0));
	const char* strOutput = (const char*)(env->GetStringUTFChars(jstrOutput, 0));
	int ret = CompressSize_BufferToFile( buf,  buf_size,  strOutput,
		 target_type,  target_size,  dst_width, dst_height);

	env->ReleaseByteArrayElements(jbuf, (jbyte*)buf, 0);
	env->ReleaseStringUTFChars(jstrOutput, strOutput);
	return ret;
}

EXTERN_C JNIEXPORT jint Java_com_apowersoft_WXImage_CompressSizeFileToBuffer(JNIEnv* env, jobject jobj, 
	jstring jstrInput, jlong handle,
	jint target_type, jint target_size, jint dst_width, jint dst_height) {
	LOGE("%s", __FUNCTION__);
	const char* strInput = (const char*)(env->GetStringUTFChars(jstrInput, 0));
	int ret = CompressSize_FileToBuffer( strInput, (void*) handle,
		 target_type,  target_size,  dst_width,  dst_height);
	env->ReleaseStringUTFChars(jstrInput, strInput);
	return ret;
}

EXTERN_C JNIEXPORT jint Java_com_apowersoft_WXImage_CompressPNG8BufferToBuffer(JNIEnv* env, jobject jobj, 
	jbyteArray jbuf, jint buf_size,
	jlong handle, jint quality, jint dst_width, jint dst_height) {
	LOGE("%s", __FUNCTION__);
	uint8_t* buf = (uint8_t*)(env->GetByteArrayElements(jbuf, 0));
	int ret = CompressPNG8_BufferToBuffer(buf,  buf_size,
		(void* )handle,  quality,  dst_width,  dst_height);
	env->ReleaseByteArrayElements(jbuf, (jbyte*)buf, 0);
	return ret;
}

EXTERN_C JNIEXPORT jint Java_com_apowersoft_WXImage_CompressPNG8BufferToFile(JNIEnv* env, jobject jobj, 
	jbyteArray jbuf, jint buf_size,
	jstring jstrOutput, jint quality, jint dst_width, jint dst_height) {
	LOGE("%s", __FUNCTION__);
	uint8_t* buf = (uint8_t*)(env->GetByteArrayElements(jbuf, 0));
	const char* strOutput = (const char*)(env->GetStringUTFChars(jstrOutput, 0));

	int ret = CompressPNG8_BufferToFile( buf,  buf_size,
		 strOutput,  quality,  dst_width,  dst_height);

	env->ReleaseByteArrayElements(jbuf, (jbyte*)buf, 0);
	env->ReleaseStringUTFChars(jstrOutput, strOutput);
	return ret;
}

EXTERN_C JNIEXPORT jint Java_com_apowersoft_WXImage_CompressPNG8FileToBuffer(JNIEnv* env, jobject jobj, 
	jstring jstrInput,
	jlong handle, jint quality, jint dst_width, jint dst_height) {
	LOGE("%s", __FUNCTION__);
	const char* strInput = (const char*)(env->GetStringUTFChars(jstrInput, 0));
	int ret = CompressPNG8_FileToBuffer(strInput,
		(void*) handle,  quality,  dst_width,  dst_height);
	env->ReleaseStringUTFChars(jstrInput, strInput);
	return ret;
}

EXTERN_C JNIEXPORT jint Java_com_apowersoft_WXImage_CompressPNG8FileToFile(JNIEnv* env, jobject jobj, 
	jstring jstrInput, jstring jstrOutput, jint quality, jint dst_width, jint dst_height) {
	LOGE("%s", __FUNCTION__);
	const char* strInput = (const char*)(env->GetStringUTFChars(jstrInput, 0));
	const char* strOutput = (const char*)(env->GetStringUTFChars(jstrOutput, 0));
	int ret = CompressPNG8_FileToFile( strInput, strOutput,  quality,  dst_width,  dst_height);
	env->ReleaseStringUTFChars(jstrInput, strInput);
	env->ReleaseStringUTFChars(jstrOutput, strOutput);
	return ret;
}


#endif