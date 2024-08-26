#include <jnihook.h>
#include <pthread.h>
#include <iostream>

void JNICALL hk_hurtTo(JNIEnv *env, jobject obj, jfloat f)
{
	std::cout << "[MCH] LocalPlayer::hurtTo(" << f << ") called!" << std::endl;
}

void *thread_main(void *)
{
	JNIEnv *env;
	JavaVM *jvm;
	jnihook_t jnihook;

	freopen("/tmp/mchook.log", "w", stdout);

	std::cout << "[MCH] Library loaded!" << std::endl;

	if (JNI_GetCreatedJavaVMs(&jvm, 1, NULL) != JNI_OK) {
		std::cout << "[DH] Failed to retrieve JavaVM pointer" << std::endl;
		return NULL;
	}

	std::cout << "[MCH] JavaVM: " << jvm << std::endl;

	if (jvm->AttachCurrentThread((void **)&env, NULL) != JNI_OK) {
		std::cout << "[DH] Failed to retrieve JNI environment" << std::endl;
		return NULL;
	}

	std::cout << "[MCH] JNIEnv: " << env << std::endl;

	auto LocalPlayer_class = env->FindClass("geb");
	std::cout << "[MCH] LocalPlayer class: " << LocalPlayer_class << std::endl;

	jmethodID hurtTo_method = env->GetMethodID(LocalPlayer_class, "H", "(F)V");
	std::cout << "[MCH] LocalPlayer::hurtTo: " << hurtTo_method << std::endl;

	std::cout << "[MCH] JNIHook_Init result: " << JNIHook_Init(env, &jnihook) << std::endl;

	std::cout << "[MCH] JNIHook_Attach result: " <<
		JNIHook_Attach(&jnihook, hurtTo_method, (void *)hk_hurtTo, NULL) << std::endl;

	jvm->DetachCurrentThread();

	return NULL;
}

void __attribute__((constructor))
dl_main()
{
	pthread_t th;
	pthread_create(&th, NULL, thread_main, NULL);
}
