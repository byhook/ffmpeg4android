
#include <jni.h>
#include <stdio.h>
#include <time.h>
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavfilter/avfilter.h"
#include "libavutil/log.h"

#ifdef ANDROID

#include <android/log.h>

#define LOG_TAG    "HelloFFmpeg"
#define LOGE(format, ...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, format, ##__VA_ARGS__)
#define LOGI(format, ...)  __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, format, ##__VA_ARGS__)
#else
#define LOGE(format, ...)  printf(LOG_TAG format "\n", ##__VA_ARGS__)
#define LOGI(format, ...)  printf(LOG_TAG format "\n", ##__VA_ARGS__)
#endif

int exec(int argc, char **argv);

void custom_log(void *ptr, int level, const char *fmt, va_list vl) {
    FILE *fp=fopen("/storage/emulated/0/Android/data/com.onzhou.ffmpeg.cmd/files/av_log.txt","a+");
    if(fp){
        vfprintf(fp,fmt,vl);
        fflush(fp);
        fclose(fp);
    }
}

JNIEXPORT jint JNICALL Java_com_onzhou_ffmpeg_cmd_FFmpegCmd_exec
        (JNIEnv *env, jobject obj, jint cmdnum, jobjectArray cmdline) {

    av_log_set_callback(custom_log);

    int argc = cmdnum;
    char **argv = (char **) malloc(sizeof(char *) * argc);

    int i = 0;
    for (i = 0; i < argc; i++) {
        jstring string = (*env)->GetObjectArrayElement(env, cmdline, i);
        const char *tmp = (*env)->GetStringUTFChars(env, string, 0);
        argv[i] = (char *) malloc(sizeof(char) * 1024);
        strcpy(argv[i], tmp);
    }

    exec(argc, argv);

    for (i = 0; i < argc; i++) {
        free(argv[i]);
    }
    free(argv);
    return 0;
}
