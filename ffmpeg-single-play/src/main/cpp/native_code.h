
#include <jni.h>

#ifndef NATIVE_CODE_PLAYER_H
#define NATIVE_CODE_PLAYER_H

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jint JNICALL playVideo(JNIEnv *env, jobject obj, jstring videoPath, jobject surface);

#ifdef __cplusplus
}
#endif

#endif //NATIVE_CODE_PLAYER_H
