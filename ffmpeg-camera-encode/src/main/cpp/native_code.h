#include <jni.h>

#ifndef NATIVE_ENCODE_H
#define NATIVE_ENCODE_H
#ifdef __cplusplus
extern "C" {
#endif

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>


JNIEXPORT void JNICALL onPreviewFrame(JNIEnv *, jobject, jbyteArray, jint, jint);

JNIEXPORT void JNICALL encodeMP4Start(JNIEnv *, jobject, jstring, jint, jint);

JNIEXPORT void JNICALL encodeMP4Stop(JNIEnv *, jobject);

JNIEXPORT void JNICALL encodeJPEG(JNIEnv *, jobject, jstring, jint, jint);

#ifdef __cplusplus
}
#endif
#endif
