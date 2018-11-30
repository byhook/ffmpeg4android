#include <jni.h>

#include "native_publish.h"
#include "h264_publish.h"

/**
 * 动态注册
 */
JNINativeMethod methods[] = {
        {"onPreviewFrame", "([BII)V",                 (void *) onPreviewFrame},
        {"startPublish",   "(Ljava/lang/String;II)V", (void *) startPublish},
        {"stopPublish",    "()V",                     (void *) stopPublish}
};

/**
 * 动态注册
 * @param env
 * @return
 */
jint registerNativeMethod(JNIEnv *env) {
    jclass cl = env->FindClass("com/onzhou/ffmpeg/stream/NativeStreamer");
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


VideoPublisher *videoPublisher = NULL;

/**
 * 编码开始
 * @param env
 * @param obj
 * @param jmp4Path
 * @param width
 * @param height
 */
void startPublish(JNIEnv *env, jobject obj, jstring jmp4Path, jint width, jint height) {
    const char *mp4Path = env->GetStringUTFChars(jmp4Path, NULL);

    if (videoPublisher == NULL) {
        videoPublisher = new H264Publisher();
    }
    videoPublisher->InitPublish(mp4Path, width, height);
    videoPublisher->StartPublish();

    env->ReleaseStringUTFChars(jmp4Path, mp4Path);
}

/**
 * 编码结束
 * @param env
 * @param obj
 * @param jmp4Path
 * @param width
 * @param height
 */
void stopPublish(JNIEnv *env, jobject obj) {
    if (NULL != videoPublisher) {
        videoPublisher->StopPublish();
        videoPublisher = NULL;
    }
}

/**
 * 处理相机回调的预览数据
 * @param env
 * @param obj
 * @param yuvArray
 * @param width
 * @param height
 */
void onPreviewFrame(JNIEnv *env, jobject obj, jbyteArray yuvArray, jint width,
                    jint height) {
    if (NULL != videoPublisher && videoPublisher->isTransform()) {
        jbyte *yuv420Buffer = env->GetByteArrayElements(yuvArray, 0);
        videoPublisher->EncodeBuffer((unsigned char *) yuv420Buffer);
        env->ReleaseByteArrayElements(yuvArray, yuv420Buffer, 0);
    }
}
