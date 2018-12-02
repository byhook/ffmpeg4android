## 概述

直播中`推流和拉流`是最核心的两个功能，之前的一篇博客记录过关于推流本地的`mp4文件`到推流服务器，而本节内容主要记录`从相机采集数据到编码H264格式，最后通过RTMP推流出去的一个过程`，为了从简单入手，本节内容`暂不涉及音频的编码操作`。

## 环境配置
操作系统：`ubuntu 16.05`
ndk版本：`android-ndk-r16b`
ffmpeg版本：`ffmpeg-3.3.8`

## 流程分析

![](https://github.com/byhook/blog/blob/master/ndk/images/20181129223643609.png)

## 开始实践
android相机采集的数据默认是`NV21格式的`，并且前置摄像头都是需要旋转`270度`。`StartPublish`主要完成一些初始化工作，并且标记`transform=true`，此时相机的数据才会交由`EncodeBuffer`来处理，完成编码H264数据并推送出去。

```java
void H264Publisher::StartPublish() {

    //1.注册所有组件
    av_register_all();
    //2.初始化网络
    avformat_network_init();

    //3.初始化输出码流的AVFormatContext
    avformat_alloc_output_context2(&out_fmt, NULL, "flv", outputPath);
    //4.查找H.264编码器
    pCodec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (!pCodec) {
        LOGE("avcodec not found!");
        return;
    }
    //5.分配编码器并设置参数
    pCodecCtx = avcodec_alloc_context3(pCodec);
    //编码器的ID,这里是H264编码器
    pCodecCtx->codec_id = pCodec->id;
    //编码器编码的数据类型
    pCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
    //像素的格式
    pCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
    //前置摄像头需要旋转270度,宽和高要互换
    pCodecCtx->width = height;
    pCodecCtx->height = width;
    pCodecCtx->framerate = (AVRational) {fps, 1};
    pCodecCtx->time_base = (AVRational) {1, fps};
    pCodecCtx->gop_size = 50;
    pCodecCtx->max_b_frames = 0;
    pCodecCtx->qmin = 10;
    pCodecCtx->qmax = 50;
    pCodecCtx->bit_rate = 100 * 1024 * 8;
    pCodecCtx->level = 41;
    pCodecCtx->refs = 1;
    pCodecCtx->qcompress = 0.6;

    if (out_fmt->oformat->flags & AVFMT_GLOBALHEADER) {
        pCodecCtx->flags |= CODEC_FLAG_GLOBAL_HEADER;
    }

    //H.264
    AVDictionary *opts = NULL;
    if (pCodecCtx->codec_id == AV_CODEC_ID_H264) {
        av_dict_set(&opts, "preset", "superfast", 0);
        av_dict_set(&opts, "tune", "zerolatency", 0);
    }
    //6.打开编码器
    int result = avcodec_open2(pCodecCtx, pCodec, &opts);
    if (result < 0) {
        LOGE("open encoder failed %d", result);
        return;
    }

    //7.创建一个输出流
    pStream = avformat_new_stream(out_fmt, pCodec);
    if (!pStream) {
        LOGE("Failed allocating output outputPath");
        return;
    }
    pStream->time_base.num = 1;
    pStream->time_base.den = fps;
    pStream->codecpar->codec_tag = 0;
    if (avcodec_parameters_from_context(pStream->codecpar, pCodecCtx) < 0) {
        LOGE("Failed av codec parameters_from_context");
        return;
    }

    //8.打开网络输出流
    if (avio_open(&out_fmt->pb, outputPath, AVIO_FLAG_READ_WRITE) < 0) {
        LOGE("Failed to open output file!\n");
        return;
    }
    //9.写文件头部
    result = avformat_write_header(out_fmt, NULL);
    if (result < 0) {
        LOGE("Error occurred when opening output URL %d", result);
        return;
    }

    //初始化帧
    pFrame = av_frame_alloc();
    pFrame->width = pCodecCtx->width;
    pFrame->height = pCodecCtx->height;
    pFrame->format = pCodecCtx->pix_fmt;
    int bufferSize = av_image_get_buffer_size(pCodecCtx->pix_fmt, pCodecCtx->width,
                                              pCodecCtx->height, 1);
    pFrameBuffer = (uint8_t *) av_malloc(bufferSize);
    av_image_fill_arrays(pFrame->data, pFrame->linesize, pFrameBuffer, pCodecCtx->pix_fmt,
                         pCodecCtx->width, pCodecCtx->height, 1);

    //创建已编码帧
    av_new_packet(&avPacket, bufferSize * 3);

    //标记正在转换
    this->transform = true;
}
```

接着就是核心的`编码函数`：

```java
int H264Publisher::EncodeFrame(AVCodecContext *pCodecCtx, AVFrame *pFrame, AVPacket *avPacket) {
    int ret = avcodec_send_frame(pCodecCtx, pFrame);
    if (ret < 0) {
        //failed to send frame for encoding
        return -1;
    }
    while (!ret) {
        ret = avcodec_receive_packet(pCodecCtx, avPacket);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            return 0;
        } else if (ret < 0) {
            //error during encoding
            return -1;
        }

        //AVFrame PTS
        pFrame->pts = index;

        //编码数据
        avPacket->stream_index = pStream->index;
        AVRational time_base = out_fmt->streams[0]->time_base;//{ 1, 1000 };
        avPacket->pts = index * (pStream->time_base.den) / ((pStream->time_base.num) * fps);
        avPacket->dts = avPacket->pts;
        avPacket->duration = (pStream->time_base.den) / ((pStream->time_base.num) * fps);
        LOGI("Send frame index:%d,pts:%lld,dts:%lld,duration:%lld,time_base:%d,%d",
             index,
             (long long) avPacket->pts,
             (long long) avPacket->dts,
             (long long) avPacket->duration,
             time_base.num, time_base.den);
        avPacket->pos = -1;
        //写出数据
        ret = av_interleaved_write_frame(out_fmt, avPacket);
        if (ret != 0) {
            LOGE("av_interleaved_write_frame failed");
        }
        av_packet_unref(avPacket);

        index++;
    }
    return 0;
}

void H264Publisher::EncodeBuffer(unsigned char *nv21Buffer) {

    uint8_t *i420_y = pFrameBuffer;
    uint8_t *i420_u = pFrameBuffer + width * height;
    uint8_t *i420_v = pFrameBuffer + width * height * 5 / 4;

    //NV21转I420
    libyuv::ConvertToI420(nv21Buffer, width * height, i420_y, height, i420_u, height / 2, i420_v,
                          height / 2, 0, 0, width, height, width, height, libyuv::kRotate270,
                          libyuv::FOURCC_NV21);

    pFrame->data[0] = i420_y;
    pFrame->data[1] = i420_u;
    pFrame->data[2] = i420_v;

    //编码H.264
    EncodeFrame(pCodecCtx, pFrame, &avPacket);
}
```

我们结束推流的时候，将`transform`标记为`false`状态，然后处理完最后一帧数据，最后关闭相关的句柄，回收资源。

```java
void H264Publisher::StopPublish() {
    //标记转换结束
    this->transform = false;

    int result = EncodeFrame(pCodecCtx, NULL, &avPacket);
    if (result >= 0) {
        //封装文件尾
        av_write_trailer(out_fmt);
        //释放内存
        if (pCodecCtx != NULL) {
            avcodec_close(pCodecCtx);
            avcodec_free_context(&pCodecCtx);
            pCodecCtx = NULL;
        }
        if (pFrame != NULL) {
            av_free(pFrame);
            pFrame = NULL;
        }
        if (pFrameBuffer != NULL) {
            av_free(pFrameBuffer);
            pFrameBuffer = NULL;
        }
        if (out_fmt != NULL) {
            avio_close(out_fmt->pb);
            avformat_free_context(out_fmt);
            out_fmt = NULL;
        }
    }
}
```

注意：之前也说过了，`android相机`默认采集的数据是`NV21格式的`，我们是无法直接处理的，需要先将`NV21格式转换成Y420P格式的数据，并且旋转270度(后置相机旋转90度)`，得到最终的数据，送入编码器中进行`编码成H264格式`的数据，最后才通过`av_interleaved_write_frame`推流到服务器上去。



用`ffplay`播放我们的局域网直播流`rtmp://192.168.1.102:1935/onzhou/live`

```java
ffplay rtmp://192.168.1.102:1935/onzhou/live
```

![](https://github.com/byhook/blog/blob/master/ndk/images/20181129222128484.png)

## 存在的问题

目前只处理了视频编码推流部分，为了更方便的了解推流过程，音频编码暂未处理，后续会有音频相关的内容。


参考：

https://blog.csdn.net/leixiaohua1020/article/details/47056051

https://blog.csdn.net/leixiaohua1020/article/details/39803457

https://blog.csdn.net/bixinwei22/article/details/78770090
