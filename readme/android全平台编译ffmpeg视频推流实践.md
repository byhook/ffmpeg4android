
#### 本文目录

- [概述](#概述)
- [配置环境](#配置环境)
- [新建streamer工程](#新建streamer工程)
- [播放串流](#播放串流)

#### 概述

还是先从最简单的搞起来，先从最基本的视频推流开始，要知道在电脑上使用`ffmpeg`完成推流，简直不要太简单，直接使用`ffmpeg`的推流命令即可，今天想在`android平台实践一下`，具体操作大家也可以看看`雷神的博客`。

```java
ffmpeg -re -i input.mp4 -vcodec copy -f flv rtmp://192.168.1.102:1935/onzhou/live
```

#### 配置环境

`操作系统: ubuntu 16.05`

`注意: ffmpeg库`的编译使用的是`android-ndk-r10e版本`，使用高版本编译会报错。

而`android-studio`工程中配合`cmake`使用的版本则是`android-ndk-r16b版本`

![](https://github.com/byhook/ffmpeg4android/blob/master/readme/images/media_060_water.png)

#### 新建工程`ffmpeg-single-streamer`

![](https://github.com/byhook/ffmpeg4android/blob/master/readme/images/选区_078.png)

- `build.gradle`配置比较简单，可以参考之前的文章

- 新建`CMakeLists.txt文件`，配置如下

```java

cmake_minimum_required(VERSION 3.4.1)


add_library(ffmpeg-streamer
           SHARED
           src/main/cpp/ffmpeg_streamer.c)

find_library(log-lib
            log)

#获取上级目录
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

#头文件
include_directories(${LIBRARY_DIR}/libs/${ANDROID_ABI}/include)

target_link_libraries(ffmpeg-streamer ffmpeg-single ${log-lib})
```

- 新建`NowStreamer.java文件`

```java
package com.onzhou.ffmpeg.streamer;

public class NowStreamer {

    static {
        System.loadLibrary("ffmpeg");
        System.loadLibrary("ffmpeg-streamer");
    }

    public native int startPublish(String input, String output);

}
```

- 在`src/main/cpp目录`新建源文件`ffmpeg_streamer.c`

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
#include <libavutil/time.h>

#define LOG_TAG    "NowStreamer"
#define LOGE(format, ...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, format, ##__VA_ARGS__)
#define LOGI(format, ...)  __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, format, ##__VA_ARGS__)
#else
#define LOGE(format, ...)  printf(LOG_TAG format "\n", ##__VA_ARGS__)
#define LOGI(format, ...)  printf(LOG_TAG format "\n", ##__VA_ARGS__)
#endif

//输出日志
void log_callback(void *ptr, int level, const char *fmt, va_list vl) {
    //写入日志文件
    FILE *fp = fopen("/storage/emulated/0/av_log.txt", "a+");
    if (fp) {
        vfprintf(fp, fmt, vl);
        fflush(fp);
        fclose(fp);
    }
    //LOGE(fmt, vl);
}

JNIEXPORT jint JNICALL Java_com_onzhou_ffmpeg_streamer_NowStreamer_startPublish
        (JNIEnv *env, jobject obj, jstring input_jstr, jstring output_jstr) {

    AVOutputFormat *ofmt = NULL;
    AVFormatContext *ifmt_ctx = NULL, *ofmt_ctx = NULL;
    AVPacket pkt;

    int ret, i;
    char input_str[500] = {0};
    char output_str[500] = {0};
    char info[1000] = {0};

    sprintf(input_str, "%s", (*env)->GetStringUTFChars(env, input_jstr, NULL));
    sprintf(output_str, "%s", (*env)->GetStringUTFChars(env, output_jstr, NULL));

    //日志回调写入文件
    av_log_set_callback(log_callback);

    av_register_all();
    //网络初始化
    avformat_network_init();

    //Input
    if ((ret = avformat_open_input(&ifmt_ctx, input_str, 0, 0)) < 0) {
        LOGE("Could not open input file.");
        goto end;
    }
    if ((ret = avformat_find_stream_info(ifmt_ctx, 0)) < 0) {
        LOGE("Failed to retrieve input stream information");
        goto end;
    }

    int videoindex = -1;
    for (i = 0; i < ifmt_ctx->nb_streams; i++)
        if (ifmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoindex = i;
            break;
        }
    //Output
    avformat_alloc_output_context2(&ofmt_ctx, NULL, "flv", output_str); //RTMP
    //avformat_alloc_output_context2(&ofmt_ctx, NULL, "mpegts", output_str);//UDP

    if (!ofmt_ctx) {
        LOGE("Could not create output context\n");
        ret = AVERROR_UNKNOWN;
        goto end;
    }
    ofmt = ofmt_ctx->oformat;
    for (i = 0; i < ifmt_ctx->nb_streams; i++) {
        //Create output AVStream according to input AVStream
        AVStream *in_stream = ifmt_ctx->streams[i];
        AVStream *out_stream = avformat_new_stream(ofmt_ctx, in_stream->codec->codec);
        if (!out_stream) {
            LOGE("Failed allocating output stream\n");
            ret = AVERROR_UNKNOWN;
            goto end;
        }
        //Copy the settings of AVCodecContext
        ret = avcodec_copy_context(out_stream->codec, in_stream->codec);
        if (ret < 0) {
            LOGE("Failed to copy context from input to output stream codec context\n");
            goto end;
        }
        out_stream->codec->codec_tag = 0;
        if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
            out_stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
    }

    //Open output URL
    if (!(ofmt->flags & AVFMT_NOFILE)) {
        ret = avio_open(&ofmt_ctx->pb, output_str, AVIO_FLAG_WRITE);
        if (ret < 0) {
            LOGE("Could not open output URL '%s'", output_str);
            goto end;
        }
    }
    //Write file header
    ret = avformat_write_header(ofmt_ctx, NULL);
    if (ret < 0) {
        LOGE("Error occurred when opening output URL\n");
        goto end;
    }

    int frame_index = 0;

    int64_t start_time = av_gettime();
    while (1) {
        AVStream *in_stream, *out_stream;
        //Get an AVPacket
        ret = av_read_frame(ifmt_ctx, &pkt);
        if (ret < 0)
            break;
        //FIX：No PTS (Example: Raw H.264)
        //Simple Write PTS
        if (pkt.pts == AV_NOPTS_VALUE) {
            //Write PTS
            AVRational time_base1 = ifmt_ctx->streams[videoindex]->time_base;
            //Duration between 2 frames (us)
            int64_t calc_duration =
                    (double) AV_TIME_BASE / av_q2d(ifmt_ctx->streams[videoindex]->r_frame_rate);
            //Parameters
            pkt.pts = (double) (frame_index * calc_duration) /
                      (double) (av_q2d(time_base1) * AV_TIME_BASE);
            pkt.dts = pkt.pts;
            pkt.duration = (double) calc_duration / (double) (av_q2d(time_base1) * AV_TIME_BASE);
        }
        //Important:Delay
        if (pkt.stream_index == videoindex) {
            AVRational time_base = ifmt_ctx->streams[videoindex]->time_base;
            AVRational time_base_q = {1, AV_TIME_BASE};
            int64_t pts_time = av_rescale_q(pkt.dts, time_base, time_base_q);
            int64_t now_time = av_gettime() - start_time;
            if (pts_time > now_time)
                av_usleep(pts_time - now_time);

        }

        in_stream = ifmt_ctx->streams[pkt.stream_index];
        out_stream = ofmt_ctx->streams[pkt.stream_index];
        /* copy packet */
        //Convert PTS/DTS
        pkt.pts = av_rescale_q_rnd(pkt.pts, in_stream->time_base, out_stream->time_base,
                                   AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX);
        pkt.dts = av_rescale_q_rnd(pkt.dts, in_stream->time_base, out_stream->time_base,
                                   AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX);
        pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);
        pkt.pos = -1;
        //Print to Screen
        if (pkt.stream_index == videoindex) {
            LOGE("Send %8d video frames to output URL\n", frame_index);
            frame_index++;
        }
        //ret = av_write_frame(ofmt_ctx, &pkt);
        ret = av_interleaved_write_frame(ofmt_ctx, &pkt);

        if (ret < 0) {
            LOGE("Error muxing packet\n");
            break;
        }
        av_free_packet(&pkt);

    }
    //Write file trailer
    av_write_trailer(ofmt_ctx);
end:
    avformat_close_input(&ifmt_ctx);
    /* close output */
    if (ofmt_ctx && !(ofmt->flags & AVFMT_NOFILE))
        avio_close(ofmt_ctx->pb);
    avformat_free_context(ofmt_ctx);
    if (ret < 0 && ret != AVERROR_EOF) {
        LOGE("Error occurred.\n");
        return -1;
    }
    return 0;
}
```

`推流服务器的搭建，可以参考之前的文章`

```java
public void onStartClick(View view) {
		if (nowStreamer == null) {
				nowStreamer = new NowStreamer();
		}
		if (publishDisposable == null) {
				publishDisposable = Schedulers.newThread().scheduleDirect(new Runnable() {
						@Override
						public void run() {
							  //推流本地的一个mp4文件
								final File intputVideo = new File(getExternalFilesDir(null), "input.mp4");
								nowStreamer.startPublish(intputVideo.getAbsolutePath(), PUBLISH_ADDRESS);
						}
				});
		}
}
```

- 编译打包运行，开始推流，我们在局域网中使用`vlc播放器`，打开网络串流

`rtmp://192.168.1.102:1935/onzhou/live`

![](https://github.com/byhook/ffmpeg4android/blob/master/readme/images/vlc_player.png)

项目地址:
https://github.com/byhook/ffmpeg4android

参考雷神:
https://blog.csdn.net/leixiaohua1020/article/details/47056051
