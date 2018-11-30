
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include "logger.h"
#include "native_code.h"
#include "native_swscale.h"


/**
 * 动态注册
 */
JNINativeMethod methods[] = {
        {"mp4ToYuv", "(Ljava/lang/String;Ljava/lang/String;)I", (void *) mp4ToYuv}
};

/**
 * 动态注册
 * @param env
 * @return
 */
jint registerNativeMethod(JNIEnv *env) {
    jclass cl = env->FindClass("com/onzhou/ffmpeg/swscale/NativeSwscale");
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


jint mp4ToYuv(JNIEnv *env, jobject obj, jstring jvideoPath, jstring jyuvPath) {
    const char *videoPath = env->GetStringUTFChars(jvideoPath, NULL);
    const char *yuvPath = env->GetStringUTFChars(jyuvPath, NULL);

    NativeSwscale nativeSwscale;
    nativeSwscale.Transform(videoPath, yuvPath);

    env->ReleaseStringUTFChars(jvideoPath, videoPath);
    env->ReleaseStringUTFChars(jyuvPath, yuvPath);

    return 0;
}