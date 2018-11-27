#include <stdio.h>
#include <time.h>

#include "native_decode.h"
#include "decode_mp4.h"

#ifdef ANDROID

#include <jni.h>
#include <android/log.h>
#include <libavutil/imgutils.h>

#define LOGE(format, ...)  __android_log_print(ANDROID_LOG_ERROR, "(>_<)", format, ##__VA_ARGS__)
#define LOGI(format, ...)  __android_log_print(ANDROID_LOG_INFO,  "(^_^)", format, ##__VA_ARGS__)
#else
#define LOGE(format, ...)  printf("(>_<) " format "\n", ##__VA_ARGS__)
#define LOGI(format, ...)  printf("(^_^) " format "\n", ##__VA_ARGS__)
#endif


/**
 * 动态注册
 */
JNINativeMethod methods[] = {
        {"decodeMP4", "(Ljava/lang/String;Ljava/lang/String;)V", (void *) decodeMP4}
};

/**
 * 动态注册
 * @param env
 * @return
 */
jint registerNativeMethod(JNIEnv *env) {
    jclass cl = env->FindClass("com/onzhou/ffmpeg/decode/NativeDecode");
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

void decodeMP4(JNIEnv *env, jobject obj, jstring jmp4Path, jstring jyuvPath) {
    VideoDecoder *mp4Decoder = new MP4Decoder();

    const char *mp4Path = env->GetStringUTFChars(jmp4Path, NULL);
    const char *yuvPath = env->GetStringUTFChars(jyuvPath, NULL);

    mp4Decoder->InitDecoder(mp4Path);
    mp4Decoder->DecodeFile(yuvPath);

    env->ReleaseStringUTFChars(jmp4Path, mp4Path);
    env->ReleaseStringUTFChars(jyuvPath, yuvPath);

    delete mp4Decoder;
}