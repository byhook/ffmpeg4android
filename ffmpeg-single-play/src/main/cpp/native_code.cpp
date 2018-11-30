
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include "logger.h"
#include "native_code.h"
#include "native_play.h"


/**
 * 动态注册
 */
JNINativeMethod methods[] = {
        {"playVideo", "(Ljava/lang/String;Ljava/lang/Object;)I", (void *) playVideo}
};

/**
 * 动态注册
 * @param env
 * @return
 */
jint registerNativeMethod(JNIEnv *env) {
    jclass cl = env->FindClass("com/onzhou/ffmpeg/player/NativePlayer");
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


int playVideo(JNIEnv *env, jobject obj, jstring jvideoPath, jobject surface) {

    const char *videoPath = env->GetStringUTFChars(jvideoPath, NULL);

    //获取界面传下来的surface
    ANativeWindow *nativeWindow = ANativeWindow_fromSurface(env, surface);
    if (0 == nativeWindow) {
        LOGE("Could not get native window from surface");
        return -1;
    }

    NativePlayer nativePlayer;
    nativePlayer.PlayVideo(videoPath, nativeWindow);

    env->ReleaseStringUTFChars(jvideoPath, videoPath);
    return 0;
}