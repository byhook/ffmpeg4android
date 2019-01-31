#include <jni.h>
#include <stdio.h>
#include "native_audio.h"
#include "opensles_recorder.h"

/**
 * 动态注册
 */
JNINativeMethod methods[] = {
        {"startRecord", "(IIILjava/lang/String;)V", (void *) startRecord},
        {"stopRecord",  "()V",                      (void *) stopRecord}
};

/**
 * 动态注册
 * @param env
 * @return
 */
jint registerNativeMethod(JNIEnv *env) {
    jclass cl = env->FindClass("com/onzhou/opensles/recorder/AudioRecorder");
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


OpenSLESRecorder *recorder = NULL;

JNIEXPORT void JNICALL startRecord(JNIEnv *env, jobject obj,
                                   jint sampleRate,
                                   jint channels,
                                   jint bitRate,
                                   jstring pcmFilePath) {
    const char *pcmPath = env->GetStringUTFChars(pcmFilePath, NULL);

    recorder = new OpenSLESRecorder();
    recorder->StartRecord(pcmPath, sampleRate, channels, bitRate);

    env->ReleaseStringUTFChars(pcmFilePath, pcmPath);
}

JNIEXPORT void JNICALL stopRecord(JNIEnv *env, jobject obj) {
    if (NULL != recorder) {
        recorder->StopRecord();
        recorder = NULL;
    }
}
