#include <jni.h>

#ifndef NATIVE_AUDIO_CODE_H
#define NATIVE_AUDIO_CODE_H

#ifdef __cplusplus
extern "C" {
#endif


JNIEXPORT void JNICALL onAudioFrame(JNIEnv *, jobject, jbyteArray, jint);

JNIEXPORT void JNICALL encodeAudioStart(JNIEnv *, jobject, jstring);

JNIEXPORT void JNICALL encodeAudioStop(JNIEnv *, jobject);

#ifdef __cplusplus
}
#endif
#endif
