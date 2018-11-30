#include <jni.h>

#ifndef INCLUDE_NATIVE_CODE_H
#define INCLUDE_NATIVE_CODE_H


#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jint JNICALL mp4ToYuv(JNIEnv *env, jobject obj, jstring videoPath, jstring yuvPath);

#ifdef __cplusplus
}
#endif


#endif //INCLUDE_NATIVE_CODE_H
