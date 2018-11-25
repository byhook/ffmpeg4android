#include <jni.h>

#include "native_frame.h"


/**
 * 动态注册
 */
JNINativeMethod methods[] = {
        {"onPreviewFrame", "([BII)V", (void *) onPreviewFrame}
};

/**
 * 动态注册
 * @param env
 * @return
 */
jint registerNativeMethod(JNIEnv *env) {
    jclass cl = env->FindClass("com/onzhou/ffmpeg/camera/NativeFrame");
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


JNIEXPORT void JNICALL onPreviewFrame(JNIEnv *env, jobject obj, jbyteArray yuvArray, jint width,
                                      jint height) {
    jbyte *yuv420Buffer = env->GetByteArrayElements(yuvArray, 0);


    env->ReleaseByteArrayElements(yuvArray, yuv420Buffer, 0);
}
