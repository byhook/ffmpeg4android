
#include <jni.h>
#include "include/libavcodec/avcodec.h"

JNIEXPORT jstring JNICALL Java_cn_byhook_ffmpegwithx264_FFmpegPlayer_getMessage
  (JNIEnv * env, jobject obj)
{
      char info[10000] = {0};
         sprintf(info, "%s\n", avcodec_configuration());
         return (*env)->NewStringUTF(env, info);
}