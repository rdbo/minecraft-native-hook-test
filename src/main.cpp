#include <jnihook.h>
#include <pthread.h>
#include <iostream>

void JNICALL hk_hurtTo(JNIEnv *env, jobject obj, jfloat f)
{
	std::cout << "[MCH] LocalPlayer::hurtTo(" << f << ") called!" << std::endl;
}

jclass orig_Minecraft_class;
jmethodID orig_Minecraft_tick;
void JNICALL hk_tick(JNIEnv *env, jobject obj)
{
	static unsigned long long tick_count = 0;
	if (tick_count % 1000 == 0)
		std::cout << "[MCH] Minecraft::tick() -> ticks: " << tick_count << std::endl;

	// Call original Minecraft::tick()
	env->CallNonvirtualVoidMethod(obj, orig_Minecraft_class, orig_Minecraft_tick);
}

void *thread_main(void *)
{
	JNIEnv *env;
	JavaVM *jvm;

	// freopen("/tmp/mchook.log", "w", stdout);

	std::cout << "[MCH] Library loaded!" << std::endl;

	if (JNI_GetCreatedJavaVMs(&jvm, 1, NULL) != JNI_OK) {
		std::cout << "[MCH] Failed to retrieve JavaVM pointer" << std::endl;
		return NULL;
	}

	std::cout << "[MCH] JavaVM: " << jvm << std::endl;

	if (jvm->AttachCurrentThread((void **)&env, NULL) != JNI_OK) {
		std::cout << "[MCH] Failed to retrieve JNI environment" << std::endl;
		return NULL;
	}

	std::cout << "[MCH] JNIEnv: " << env << std::endl;

	auto LocalPlayer_class = env->FindClass("geb");
	std::cout << "[MCH] LocalPlayer class: " << LocalPlayer_class << std::endl;

	auto Minecraft_class = env->FindClass("fgo");
	std::cout << "[MCH] Minecraft class: " << Minecraft_class << std::endl;

	jmethodID hurtTo_method = env->GetMethodID(LocalPlayer_class, "H", "(F)V");
	std::cout << "[MCH] LocalPlayer::hurtTo(): " << hurtTo_method << std::endl;

	jmethodID tick_method = env->GetMethodID(Minecraft_class, "t", "()V");
	std::cout << "[MCH] Minecraft::tick():" << tick_method << std::endl;

	std::cout << "[MCH] JNIHook_Init result: " << JNIHook_Init(jvm) << std::endl;

	std::cout << "[MCH] JNIHook_Attach result: " <<
		JNIHook_Attach(hurtTo_method, (void *)hk_hurtTo, NULL, NULL) << std::endl;

	std::cout << "[MCH] JNIHook_Attach result: " <<
		JNIHook_Attach(tick_method, (void *)hk_tick, &orig_Minecraft_tick, &orig_Minecraft_class) << std::endl;

	std::cout << "[MCH] Original Minecraft::tick(): " << orig_Minecraft_tick << std::endl;

	jvm->DetachCurrentThread();

	return NULL;
}

void __attribute__((constructor))
dl_main()
{
	pthread_t th;
	pthread_create(&th, NULL, thread_main, NULL);
}
