
#include "native_code.h"
#include "media_encoder.h"

/**
 * 动态注册
 */
JNINativeMethod methods[] = {
        {"muxerVideoAudio", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V", (void *) muxerVideoAudio}
};

/**
 * 动态注册
 * @param env
 * @return
 */
jint registerNativeMethod(JNIEnv *env) {
    jclass cl = env->FindClass("com/onzhou/ffmpeg/muxer/NativeMuxer");
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


void muxerVideoAudio(JNIEnv *env, jobject obj, jstring jvideoPath, jstring jaudioPath,
                     jstring joutputPath) {

    const char *videoPath = env->GetStringUTFChars(jvideoPath, NULL);
    const char *audioPath = env->GetStringUTFChars(jaudioPath, NULL);
    const char *outputPath = env->GetStringUTFChars(joutputPath, NULL);

    MP4Encoder *mp4Encoder = new MP4Encoder();

    mp4Encoder->Transform(videoPath, audioPath, outputPath);

    delete mp4Encoder;

    env->ReleaseStringUTFChars(jvideoPath, videoPath);
    env->ReleaseStringUTFChars(jaudioPath, audioPath);
    env->ReleaseStringUTFChars(joutputPath, outputPath);
}
