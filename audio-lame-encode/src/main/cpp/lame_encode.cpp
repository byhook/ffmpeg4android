
#include <jni.h>
#include <stdio.h>
#include "lame_encode.h"
#include "mp3_encode.h"

/**
 * 动态注册
 */
JNINativeMethod methods[] = {
        {"encode",  "(Ljava/lang/String;Ljava/lang/String;III)V", (void *) encode},
        {"destroy", "()V",                                        (void *) destroy}
};

/**
 * 动态注册
 * @param env
 * @return
 */
jint registerNativeMethod(JNIEnv *env) {
    jclass cl = env->FindClass("com/onzhou/lame/encode/LameEncoder");
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


Mp3Encoder *encoder = NULL;

JNIEXPORT void JNICALL encode(JNIEnv *env, jobject obj, jstring pcmFilePath, jstring mp3FilePath, jint sampleRate,
       jint channels,
       jint bitRate) {
    const char *pcmPath = env->GetStringUTFChars(pcmFilePath, NULL);
    const char *mp3Path = env->GetStringUTFChars(mp3FilePath, NULL);
    encoder = new Mp3Encoder();
    encoder->Init(pcmPath, mp3Path, sampleRate, channels, bitRate);
    //开始编码
    encoder->Encode();

    env->ReleaseStringUTFChars(pcmFilePath, mp3Path);
    env->ReleaseStringUTFChars(mp3FilePath, pcmPath);
}

JNIEXPORT void JNICALL destroy(JNIEnv * env, jobject obj) {
    if (NULL != encoder) {
        encoder->Release();
        encoder = NULL;
    }
}
