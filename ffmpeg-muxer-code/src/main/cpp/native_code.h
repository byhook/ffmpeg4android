#include <jni.h>

#ifndef NATIVE_MUXER_ENCODE_H
#define NATIVE_MUXER_ENCODE_H

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void JNICALL muxerVideoAudio(JNIEnv *, jobject, jstring , jstring, jstring);

#ifdef __cplusplus
}
#endif


#endif
