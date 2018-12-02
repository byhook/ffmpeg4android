## 概述

其实如果对`YUV和RGB数据`进行转换，更推荐使用`libyuv库`进行转换，效率更高一些，今天记录一下`ffmpeg库`自带的`swscale模块`进行转码工作。

## 环境配置

**操作系统：`ubuntu 16.05`
ndk版本：`android-ndk-r16b`
ffmpeg版本：`ffmpeg-3.3.8`**

## 流程分析

![](https://github.com/byhook/blog/blob/master/ndk/images/20181201211319110.png)

## 工程实践

**核心的转码实现：`Transform`**

```java

#include "native_swscale.h"
#include "logger.h"

int NativeSwscale::Transform(const char *videoPath, const char *outputPath) {

    // 1.注册所有组件
    av_register_all();
    // 2.创建AVFormatContext结构体
    pFormatCtx = avformat_alloc_context();

    // 3.打开一个输入文件
    if (avformat_open_input(&pFormatCtx, videoPath, NULL, NULL) != 0) {
        LOGE("Could not open input stream");
        goto end_line;
    }
    // 4.获取媒体的信息
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        LOGE("Could not find stream information");
        goto end_line;
    }
    //获取视频轨的下标
    for (int i = 0; i < pFormatCtx->nb_streams; i++)
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoIndex = i;
            break;
        }
    if (videoIndex == -1) {
        LOGE("Could not find a video stream");
        goto end_line;
    }
    // 5.查找解码器
    pCodec = avcodec_find_decoder(pFormatCtx->streams[videoIndex]->codecpar->codec_id);
    if (pCodec == NULL) {
        LOGE("Could not find Codec");
        goto end_line;
    }

    // 6.配置解码器
    pCodecCtx = avcodec_alloc_context3(pCodec);
    avcodec_parameters_to_context(pCodecCtx, pFormatCtx->streams[videoIndex]->codecpar);
    pCodecCtx->thread_count = 1;

    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        LOGE("Could not open codec");
        goto end_line;
    }

    pFrame = av_frame_alloc();
    vPacket = (AVPacket *) av_malloc(sizeof(AVPacket));
    pFrameYUV = av_frame_alloc();


    bufferSize = av_image_get_buffer_size(AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height,
                                          1);
    out_buffer = (uint8_t *) av_malloc(bufferSize * sizeof(uint8_t));

    av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize, out_buffer, AV_PIX_FMT_YUV420P,
                         pCodecCtx->width, pCodecCtx->height, 1);

    //RGB24转换
    /*bufferSize = av_image_get_buffer_size(AV_PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height, 1);
    out_buffer = (uint8_t *) av_malloc(bufferSize * sizeof(uint8_t));
    av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize, out_buffer, AV_PIX_FMT_RGB24,
                         pCodecCtx->width, pCodecCtx->height, 1);*/

    //转换
    sws_ctx = sws_getContext(pCodecCtx->width,
                             pCodecCtx->height,
                             pCodecCtx->pix_fmt,
                             pCodecCtx->width,
                             pCodecCtx->height,
                             AV_PIX_FMT_YUV420P, //AV_PIX_FMT_RGB24
                             SWS_BICUBIC,
                             NULL, NULL, NULL);

    outputFile = fopen(outputPath, "wb+");

    while (av_read_frame(pFormatCtx, vPacket) >= 0) {
        if (vPacket->stream_index == videoIndex) {
            if (avcodec_send_packet(pCodecCtx, vPacket) != 0) {
                return -1;
            }
            while (avcodec_receive_frame(pCodecCtx, pFrame) == 0) {

                sws_scale(sws_ctx, (const uint8_t *const *) pFrame->data, pFrame->linesize,
                          0, pCodecCtx->height, pFrameYUV->data, pFrameYUV->linesize);

                //YUV420P
                fwrite(pFrameYUV->data[0], (pCodecCtx->width) * (pCodecCtx->height), 1, outputFile);
                fwrite(pFrameYUV->data[1], (pCodecCtx->width) * (pCodecCtx->height) / 4, 1,
                       outputFile);
                fwrite(pFrameYUV->data[2], (pCodecCtx->width) * (pCodecCtx->height) / 4, 1,
                       outputFile);

                //RGB24
                //fwrite(pFrameYUV->data[0], (pCodecCtx->width) * (pCodecCtx->height) * 3, 1, outputFile);
                //UYVY
                //fwrite(pFrameYUV->data[0],(pCodecCtx->width)*(pCodecCtx->height),2,outputFile);
            }
        }
        av_packet_unref(vPacket);
    }

    if (outputFile != NULL) {
        fclose(outputFile);
        outputFile = NULL;
    }

    sws_freeContext(sws_ctx);

    end_line:

    avcodec_close(pCodecCtx);
    avcodec_free_context(&pCodecCtx);
    av_free(pFrame);
    av_free(pFrameYUV);

    avformat_close_input(&pFormatCtx);
    avformat_free_context(pFormatCtx);
    return 0;
}
```

最终输出一个`output.yuv文件`，同步到本地电脑上，使用`ffplay可以直接播放：`

```java
ffplay -f rawvideo -video_size 480x960 output.yuv
```

![](https://github.com/byhook/blog/blob/master/ndk/images/2018120121174311.png)

参考：

https://blog.csdn.net/leixiaohua1020/article/details/14215391
