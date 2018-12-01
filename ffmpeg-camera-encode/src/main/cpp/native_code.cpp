#include <jni.h>

#include "native_code.h"
#include "encode_mp4.h"
#include "encode_jpeg.h"

/**
 * 动态注册
 */
JNINativeMethod methods[] = {
        {"onPreviewFrame", "([BII)V",                 (void *) onPreviewFrame},
        {"encodeMP4Start", "(Ljava/lang/String;II)V", (void *) encodeMP4Start},
        {"encodeMP4Stop",  "()V",                     (void *) encodeMP4Stop},
        {"encodeJPEG",     "(Ljava/lang/String;II)V", (void *) encodeJPEG}
};

/**
 * 动态注册
 * @param env
 * @return
 */
jint registerNativeMethod(JNIEnv *env) {
    jclass cl = env->FindClass("com/onzhou/ffmpeg/encode/NativeEncoder");
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


VideoEncoder *videoPublisher = NULL;

/**
 * 编码开始
 * @param env
 * @param obj
 * @param jmp4Path
 * @param width
 * @param height
 */
void encodeMP4Start(JNIEnv *env, jobject obj, jstring jmp4Path, jint width, jint height) {
    const char *mp4Path = env->GetStringUTFChars(jmp4Path, NULL);

    if (videoPublisher == NULL) {
        videoPublisher = new MP4Encoder();
    }
    videoPublisher->InitEncoder(mp4Path, width, height);
    videoPublisher->EncodeStart();

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
void encodeMP4Stop(JNIEnv *env, jobject obj) {
    if (NULL != videoPublisher) {
        videoPublisher->EncodeStop();
        videoPublisher = NULL;
    }
}


JPEGEncoder *jpegEncoder = NULL;

/**
 *
 * @param env
 * @param obj
 * @param jmp4Path
 * @param width
 * @param height
 */
void encodeJPEG(JNIEnv *env, jobject obj, jstring jjpegPath, jint width, jint height) {
    if (NULL == jpegEncoder) {
        const char *jpegPath = env->GetStringUTFChars(jjpegPath, NULL);
        jpegEncoder = new JPEGEncoder(jpegPath, width, height);
        env->ReleaseStringUTFChars(jjpegPath, jpegPath);
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

    if (NULL != jpegEncoder) {
        if (jpegEncoder->isTransform()) {
            jbyte *yuv420Buffer = env->GetByteArrayElements(yuvArray, 0);
            jpegEncoder->EncodeJPEG((unsigned char *) yuv420Buffer);
            env->ReleaseByteArrayElements(yuvArray, yuv420Buffer, 0);
        }
        //释放
        delete jpegEncoder;
        jpegEncoder = NULL;
    }

}