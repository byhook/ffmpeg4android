
#include "native_stream.h"
#include "mp4_stream.h"


/**
 * 动态注册
 */
JNINativeMethod methods[] = {
        {"startPublish", "(Ljava/lang/String;Ljava/lang/String;)I", (void *) startPublish},
        {"stopPublish",  "()V",                                     (void *) stopPublish},
};

/**
 * 动态注册
 * @param env
 * @return
 */
jint registerNativeMethod(JNIEnv *env) {
    jclass cl = env->FindClass("com/onzhou/ffmpeg/streamer/NativeStreamer");
    if ((env->RegisterNatives(cl, methods, sizeof(methods) / sizeof(methods[0]))) < 0) {
        return -1;
    }
    return 0;
}

/**
 * 加载默认回调
 * @param vm
 * @param reserved
 * @return
 */
jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env = NULL;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }
    //注册方法
    if (registerNativeMethod(env) != JNI_OK) {
        return -1;
    }
    return JNI_VERSION_1_6;
}


MP4Stream * mp4Stream = NULL;

jint startPublish(JNIEnv *env, jobject obj, jstring jmp4Path, jstring jstream) {

    const char *mp4Path = env->GetStringUTFChars(jmp4Path, NULL);
    const char *stream = env->GetStringUTFChars(jstream, NULL);

    if(mp4Stream==NULL){
        mp4Stream = new MP4Stream();
    }
    mp4Stream->start_publish(mp4Path, stream);

    env->ReleaseStringUTFChars(jmp4Path, mp4Path);
    env->ReleaseStringUTFChars(jstream, stream);

    return 0;
}

void stopPublish(JNIEnv *env, jobject obj) {
    if(mp4Stream!=NULL){
        mp4Stream->stop_publish();
    }
    mp4Stream = NULL;
}