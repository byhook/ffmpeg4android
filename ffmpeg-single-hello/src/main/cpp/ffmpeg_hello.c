
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

JNIEXPORT jstring JNICALL Java_com_onzhou_ffmpeg_hello_FFmpegHello_urlprotocolinfo
  (JNIEnv * env, jobject obj){

    char info[40000]={0};
	av_register_all();

	struct URLProtocol *pup = NULL;
	//input
	struct URLProtocol **p_temp = &pup;
	avio_enum_protocols((void **)p_temp, 0);
	while ((*p_temp) != NULL){
		sprintf(info, "%s[in ][%10s]\n", info, avio_enum_protocols((void **)p_temp, 0));
	}
	pup = NULL;
	//output
	avio_enum_protocols((void **)p_temp, 1);
	while ((*p_temp) != NULL){
		sprintf(info, "%s[out][%10s]\n", info, avio_enum_protocols((void **)p_temp, 1));
	}
	LOGE("%s", info);
	return (*env)->NewStringUTF(env, info);
}


JNIEXPORT jstring JNICALL Java_com_onzhou_ffmpeg_hello_FFmpegHello_avformatinfo
  (JNIEnv * env, jobject obj){

    char info[40000] = { 0 };

	av_register_all();

	AVInputFormat *if_temp = av_iformat_next(NULL);
	AVOutputFormat *of_temp = av_oformat_next(NULL);
	//input
	while(if_temp!=NULL){
		sprintf(info, "%s[in ][%10s]\n", info, if_temp->name);
		if_temp=if_temp->next;
	}
	//output
	while (of_temp != NULL){
		sprintf(info, "%s[out][%10s]\n", info, of_temp->name);
		of_temp = of_temp->next;
	}
	LOGE("%s", info);
	return (*env)->NewStringUTF(env, info);
}


JNIEXPORT jstring JNICALL Java_com_onzhou_ffmpeg_hello_FFmpegHello_avcodecinfo
  (JNIEnv * env, jobject obj){

    char info[40000] = { 0 };

	av_register_all();

	AVCodec *c_temp = av_codec_next(NULL);

	while(c_temp!=NULL){
		if (c_temp->decode!=NULL){
			sprintf(info, "%s[dec]", info);
		}
		else{
			sprintf(info, "%s[enc]", info);
		}
		switch (c_temp->type){
		case AVMEDIA_TYPE_VIDEO:
			sprintf(info, "%s[video]", info);
			break;
		case AVMEDIA_TYPE_AUDIO:
			sprintf(info, "%s[audio]", info);
			break;
		default:
			sprintf(info, "%s[other]", info);
			break;
		}
		sprintf(info, "%s[%10s]\n", info, c_temp->name);

		c_temp=c_temp->next;
	}
	LOGE("%s", info);
	return (*env)->NewStringUTF(env, info);
}


JNIEXPORT jstring JNICALL Java_com_onzhou_ffmpeg_hello_FFmpegHello_avfilterinfo
  (JNIEnv * env, jobject obj){

    char info[40000] = { 0 };

	avfilter_register_all();

	AVFilter *f_temp = (AVFilter *)avfilter_next(NULL);
	while (f_temp != NULL){
		sprintf(info, "%s[%10s]\n", info, f_temp->name);
	}
	LOGE("%s", info);
	return (*env)->NewStringUTF(env, info);
}


JNIEXPORT jstring JNICALL Java_com_onzhou_ffmpeg_hello_FFmpegHello_configurationinfo
  (JNIEnv * env, jobject obj){

    char info[10000] = { 0 };
	av_register_all();

	sprintf(info, "%s\n", avcodec_configuration());

	LOGE("%s", info);
	return (*env)->NewStringUTF(env, info);
}