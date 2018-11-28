#include <jni.h>

#ifndef NATIVE_STREAM_H
#define NATIVE_STREAM_H
#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jint JNICALL startPublish(JNIEnv *, jobject, jstring, jstring);

JNIEXPORT void JNICALL stopPublish(JNIEnv *, jobject);

#ifdef __cplusplus
}
#endif
#endif