#include "WXTcp_impl.h"

static JavaVM *s_jVM = nullptr;//ÐéÄâ»ú
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {

	JNIEnv *env = NULL;
	if (vm->GetEnv((void **)&env, JNI_VERSION_1_6) != JNI_OK) {
		return JNI_ERR; // JNI version not supported.
	}

	InitTcpClient();
	InitTcpServer();
	return JNI_VERSION_1_6;
}

//»ñÈ¡env
JNIEnv *Android_JNI_GetEnv(void) {
	JNIEnv *env = NULL;
	int status = s_jVM->AttachCurrentThread(&env, NULL);
	if (status < 0) {
		LOGE("failed to attach current thread");
		return 0;
	}
	return env;
}