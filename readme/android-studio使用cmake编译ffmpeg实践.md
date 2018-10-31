
本例使用的是合并的`libffmpeg库`，可参考之前的实践操作

[android全平台编译ffmpeg合并为单个库实践](https://github.com/byhook/ffmpeg4android/blob/master/readme/android全平台编译ffmpeg合并为单个库实践.md)

### 目录

- [配置环境](#配置环境)
- [新建hello工程](#新建hello工程)

#### 配置环境

`操作系统: ubuntu 16.05`

`注意: ffmpeg库`的编译使用的是`android-ndk-r10e版本`，使用高版本编译会报错

而`android-studio`工程中配合`cmake`使用的版本则是`android-ndk-r16b版本`

![](https://github.com/byhook/ffmpeg4android/blob/master/readme/images/media_060_water.png)


#### 新建工程`ffmpeg-single-hello`

![](https://github.com/byhook/ffmpeg4android/blob/master/readme/images/screenshot_004_water.png)

- 配置`build.gradle`如下

```java
android {
    ......
    defaultConfig {
        ......
        externalNativeBuild {
            cmake {
                cppFlags ""
            }
            ndk {
                abiFilters "armeabi-v7a"
            }
        }
        sourceSets {
            main {
                //库地址
                jniLibs.srcDirs = ['libs']
            }
        }
    }
    ......
    externalNativeBuild {
        cmake {
            path "CMakeLists.txt"
        }
    }
}
```
- 新建`CMakeLists.txt文件`，配置如下

```java

cmake_minimum_required(VERSION 3.4.1)

add_library(ffmpeg-hello
           SHARED
           src/main/cpp/ffmpeg_hello.c)

find_library(log-lib
            log)

#获取上级目录的路径
get_filename_component(PARENT_DIR ${CMAKE_SOURCE_DIR} PATH)
set(LIBRARY_DIR ${PARENT_DIR}/ffmpeg-single)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
set(CMAKE_VERBOSE_MAKEFILE on)

add_library(ffmpeg-single
           SHARED
           IMPORTED)

set_target_properties(ffmpeg-single
                    PROPERTIES IMPORTED_LOCATION
                    ${LIBRARY_DIR}/libs/${ANDROID_ABI}/libffmpeg.so
                    )

#包含头文件
include_directories(${LIBRARY_DIR}/libs/${ANDROID_ABI}/include)

target_link_libraries(ffmpeg-hello ffmpeg-single ${log-lib})
```

>笔者这里考虑到后续的都是基于同一个`libffmpeg.so库`，因此不必每个`module`都放置一个，因此单独分离了一个组件`ffmpeg-single`，将所需要的`头文件和库`放置在`libs`目录
其他的`单个库例子`则都是依赖这个库

![](https://github.com/byhook/ffmpeg4android/blob/master/readme/images/screenshot_003_water.png)

- 新建类`FFmpegHello.java`
```java
package com.onzhou.ffmpeg.hello;

public class FFmpegHello {

    static {
        System.loadLibrary("ffmpeg");
        System.loadLibrary("ffmpeg-hello");
    }

    public native String urlprotocolinfo();

    public native String avformatinfo();

    public native String avcodecinfo();

    public native String avfilterinfo();

    public native String configurationinfo();

}
```

- 在`src/main/cpp目录新建源文件ffmpeg_hello.c`

```java

#include <jni.h>
#include <stdio.h>
#include <time.h>
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavfilter/avfilter.h"
#include "libavutil/log.h"

#ifdef ANDROID
#include <android/log.h>
#define LOG_TAG    "FFmpegHello"
#define LOGE(format， ...)  __android_log_print(ANDROID_LOG_ERROR， LOG_TAG， format， ##__VA_ARGS__)
#define LOGI(format， ...)  __android_log_print(ANDROID_LOG_INFO，  LOG_TAG， format， ##__VA_ARGS__)
#else
#define LOGE(format， ...)  printf(LOG_TAG format "\n"， ##__VA_ARGS__)
#define LOGI(format， ...)  printf(LOG_TAG format "\n"， ##__VA_ARGS__)
#endif

JNIEXPORT jstring JNICALL Java_com_onzhou_ffmpeg_hello_FFmpegHello_urlprotocolinfo
  (JNIEnv * env， jobject obj){

    char info[40000]={0};
	av_register_all();

	struct URLProtocol *pup = NULL;
	//input
	struct URLProtocol **p_temp = &pup;
	avio_enum_protocols((void **)p_temp， 0);
	while ((*p_temp) != NULL){
		sprintf(info， "%s[in ][%10s]\n"， info， avio_enum_protocols((void **)p_temp， 0));
	}
	pup = NULL;
	//output
	avio_enum_protocols((void **)p_temp， 1);
	while ((*p_temp) != NULL){
		sprintf(info， "%s[out][%10s]\n"， info， avio_enum_protocols((void **)p_temp， 1));
	}
	LOGE("%s"， info);
	return (*env)->NewStringUTF(env， info);
}


JNIEXPORT jstring JNICALL Java_com_onzhou_ffmpeg_hello_FFmpegHello_avformatinfo
  (JNIEnv * env， jobject obj){

    char info[40000] = { 0 };

	av_register_all();

	AVInputFormat *if_temp = av_iformat_next(NULL);
	AVOutputFormat *of_temp = av_oformat_next(NULL);
	//input
	while(if_temp!=NULL){
		sprintf(info， "%s[in ][%10s]\n"， info， if_temp->name);
		if_temp=if_temp->next;
	}
	//output
	while (of_temp != NULL){
		sprintf(info， "%s[out][%10s]\n"， info， of_temp->name);
		of_temp = of_temp->next;
	}
	LOGE("%s"， info);
	return (*env)->NewStringUTF(env， info);
}


JNIEXPORT jstring JNICALL Java_com_onzhou_ffmpeg_hello_FFmpegHello_avcodecinfo
  (JNIEnv * env， jobject obj){

    char info[40000] = { 0 };

	av_register_all();

	AVCodec *c_temp = av_codec_next(NULL);

	while(c_temp!=NULL){
		if (c_temp->decode!=NULL){
			sprintf(info， "%s[dec]"， info);
		}
		else{
			sprintf(info， "%s[enc]"， info);
		}
		switch (c_temp->type){
		case AVMEDIA_TYPE_VIDEO:
			sprintf(info， "%s[video]"， info);
			break;
		case AVMEDIA_TYPE_AUDIO:
			sprintf(info， "%s[audio]"， info);
			break;
		default:
			sprintf(info， "%s[other]"， info);
			break;
		}
		sprintf(info， "%s[%10s]\n"， info， c_temp->name);

		c_temp=c_temp->next;
	}
	LOGE("%s"， info);
	return (*env)->NewStringUTF(env， info);
}


JNIEXPORT jstring JNICALL Java_com_onzhou_ffmpeg_hello_FFmpegHello_avfilterinfo
  (JNIEnv * env， jobject obj){

    char info[40000] = { 0 };

	avfilter_register_all();

	AVFilter *f_temp = (AVFilter *)avfilter_next(NULL);
	while (f_temp != NULL){
		sprintf(info， "%s[%10s]\n"， info， f_temp->name);
	}
	LOGE("%s"， info);
	return (*env)->NewStringUTF(env， info);
}


JNIEXPORT jstring JNICALL Java_com_onzhou_ffmpeg_hello_FFmpegHello_configurationinfo
  (JNIEnv * env， jobject obj){

    char info[10000] = { 0 };
	av_register_all();

	sprintf(info， "%s\n"， avcodec_configuration());

	LOGE("%s"， info);
	return (*env)->NewStringUTF(env， info);
}
```

- 编译打包运行，`输出如下信息:`

![](https://github.com/byhook/ffmpeg4android/blob/master/readme/images/xxxx_water.png)

项目地址:
https://github.com/byhook/ffmpeg4android

参考:
https://blog.csdn.net/leixiaohua1020/article/details/47008825
