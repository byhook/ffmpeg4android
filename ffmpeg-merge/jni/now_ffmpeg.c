
#include <jni.h>
#include "include/libavcodec/avcodec.h"
#include "include/libavformat/avformat.h"
#include "include/libavfilter/avfilter.h"

JNIEXPORT jstring JNICALL Java_com_onzhou_ffmpeg_NowStreamer_getAvFormatInfo
  (JNIEnv * env, jobject obj)
{

    char info[40000] = { 0 };
	av_register_all();

	AVInputFormat *if_temp = av_iformat_next(NULL);
	AVOutputFormat *of_temp = av_oformat_next(NULL);
	//input
	while(if_temp!=NULL){
		sprintf(info, "%s[In ][%10s]\n", info, if_temp->name);
		if_temp=if_temp->next;
	}
	//output
	while (of_temp != NULL){
		sprintf(info, "%s[Out][%10s]\n", info, of_temp->name);
		of_temp = of_temp->next;
	}
	//LOGE("%s", info);
	return (*env)->NewStringUTF(env, info);
}