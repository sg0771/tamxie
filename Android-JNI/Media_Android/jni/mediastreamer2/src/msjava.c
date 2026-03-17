#include "mediastreamer2/msjava.h"
#include "mediastreamer2/mscommon.h"

static JavaVM *ms2_vm=NULL;
#include <pthread.h>
static pthread_key_t jnienv_key;
void _android_key_cleanup(void *data){
	ms_message("Thread end, detaching jvm from current thread");
	JNIEnv* env=(JNIEnv*)pthread_getspecific(jnienv_key);
	if (env != NULL) {
		(*ms2_vm)->DetachCurrentThread(ms2_vm);
		pthread_setspecific(jnienv_key,NULL);
	}
}

void ms_set_jvm(JavaVM *vm){
	ms2_vm=vm;
	pthread_key_create(&jnienv_key,_android_key_cleanup);
}

JavaVM *ms_get_jvm(void){
	return ms2_vm;
}

JNIEnv *ms_get_jni_env(void){
	JNIEnv *env=NULL;
	if (ms2_vm==NULL){
		ms_fatal("Calling ms_get_jni_env() while no jvm has been set using ms_set_jvm().");
	}else{
		env=(JNIEnv*)pthread_getspecific(jnienv_key);
		if (env==NULL){
			if ((*ms2_vm)->AttachCurrentThread(ms2_vm,&env,NULL)!=0){
				ms_fatal("AttachCurrentThread() failed !");
				return NULL;
			}
			pthread_setspecific(jnienv_key,env);
		}
	}
	return env;
}