## 概述

在音视频开发中几乎都要涉及两个非常重要的环节：`编码`和`解码`，今天要记录的就是其中的编码环节，笔者这里不打算引入`OpenGLES`对相机预览数据进行渲染，而是直接使用系统提供的方式，因为重点在`视频编码`这块，因为android相机默认采集的原始数据基本都是`NV21格式的`，并且是横向的，因此我们通过`libyuv库`先转换为`I420(即YUV420P)格式`，然后`旋转270度`，最终进行编码到视频文件中去。

## 环境配置
操作系统：`ubuntu 16.05`
ndk版本：`android-ndk-r16b`
ffmpeg版本：`ffmpeg-3.3.8版本`

ffmpeg的编译不是本文的内容，可以参考之前的博客[android全平台编译ffmpeg合并为单个库实践](https://blog.csdn.net/byhook/article/details/83576698)

下面给出最新的流程图：

![](https://github.com/byhook/blog/blob/master/ndk/images/20181127125710971.png)

**关键函数说明**

>//注册FFmpeg所有编解码器。
`av_register_all()`
//初始化输出码流的AVFormatContext。
`avformat_alloc_output_context2()`
//打开输出文件。
`avio_open()`
//创建输出码流的AVStream。
`av_new_stream()`
//查找编码器。
`avcodec_find_encoder()`
//分配编码器上下文参数。
`avcodec_alloc_context3()`
//打开编码器。
`avcodec_open2()`
//写文件头(对于某些没有文件头的封装格式，不需要此函数。比如说MPEG2TS)。
`avformat_write_header()`
//发送AVFrame数据给编码器
`avcodec_send_frame()`
//获取编码为AVPacket
`avcodec_receive_packet()`
//将编码后的视频码流写入文件。
`av_interleaved_write_frame()`
//用于输出编码器中剩余的AVPacket
`flush_encoder()`
//写文件尾(对于某些没有文件头的封装格式，不需要此函数。比如说MPEG2TS)。
`av_write_trailer()`

## 工程实践

基于之前的[项目工程](https://github.com/byhook/ffmpeg4android)，新建子工程`ffmpeg-camera-encode`

![](https://github.com/byhook/blog/blob/master/ndk/images/20181127130206893.png)

`关于CMakeLists.txt以及build.gradle配置，这里不再赘述`

## 开始编码
编写`java层`的`NativeEncoder类`，用来将`相机采集到的数据回传到native层处理`。
```java
package com.onzhou.ffmpeg.encode;

public class NativeEncoder {

    static {
        System.loadLibrary("native-encode");
    }

    public native void encodeMP4Start(String mp4Path, int width, int height);

    public native void encodeMP4Stop();

    public native void onPreviewFrame(byte[] yuvData, int width, int height);

}
```
定义`native层的类`：
```java
VideoEncoder *videoEncoder = NULL;

/**
 * 编码开始
 * @param env
 * @param obj
 * @param jmp4Path
 * @param width
 * @param height
 */
void encodeMP4Start(JNIEnv *env, jobject obj, jstring jmp4Path, jint width, jint height) {
    const char *mp4Path = env->GetStringUTFChars(jmp4Path, NULL);

    if (videoEncoder == NULL) {
        videoEncoder = new MP4Encoder();
    }
    videoEncoder->InitEncoder(mp4Path, width, height);
    videoEncoder->EncodeStart();

    env->ReleaseStringUTFChars(jmp4Path, mp4Path);
}

/**
 * 编码结束
 * @param env
 * @param obj
 * @param jmp4Path
 * @param width
 * @param height
 */
void encodeMP4Stop(JNIEnv *env, jobject obj) {
    if (NULL != videoEncoder) {
        videoEncoder->EncodeStop();
        videoEncoder = NULL;
    }
}

/**
 * 处理相机回调的预览数据
 * @param env
 * @param obj
 * @param yuvArray
 * @param width
 * @param height
 */
void onPreviewFrame(JNIEnv *env, jobject obj, jbyteArray yuvArray, jint width,
                    jint height) {
    if (NULL != videoEncoder && videoEncoder->isTransform()) {
        jbyte *yuv420Buffer = env->GetByteArrayElements(yuvArray, 0);
        videoEncoder->EncodeBuffer((unsigned char *) yuv420Buffer);
        env->ReleaseByteArrayElements(yuvArray, yuv420Buffer, 0);
    }
}
```

考虑到后续可能会有不同的编码器，这里定义了一个视频编码的基类：

```java
class VideoEncoder {

protected:

    bool transform = false;

public:

    virtual void InitEncoder(const char *mp4Path, int width, int height) = 0;

    virtual void EncodeStart() = 0;

    virtual void EncodeBuffer(unsigned char *nv21Buffer) = 0;

    virtual void EncodeStop() = 0;

    bool isTransform();

};
```
接下来就是具体的`MP4的编码器实现类了`：`encode_mp4.cpp`

```java
#include <libyuv.h>
#include "logger.h"
#include "encode_mp4.h"

void MP4Encoder::InitEncoder(const char *mp4Path, int width, int height) {
    this->mp4Path = mp4Path;
    this->width = width;
    this->height = height;
}

int MP4Encoder::EncodeFrame(AVCodecContext *pCodecCtx, AVFrame *pFrame, AVPacket *avPacket) {
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
        printf("Write frame %d, size=%d\n", avPacket->pts, avPacket->size);
        avPacket->stream_index = pStream->index;
        av_packet_rescale_ts(avPacket, pCodecCtx->time_base, pStream->time_base);
        avPacket->pos = -1;
        av_interleaved_write_frame(pFormatCtx, avPacket);
        av_packet_unref(avPacket);
    }
    return 0;
}

void MP4Encoder::EncodeStart() {
    //1. 注册所有组件
    av_register_all();
    //2. 初始化输出码流的AVFormatContext
    avformat_alloc_output_context2(&pFormatCtx, NULL, NULL, this->mp4Path);
    fmt = pFormatCtx->oformat;

    //3. 打开待输出的视频文件
    if (avio_open(&pFormatCtx->pb, this->mp4Path, AVIO_FLAG_READ_WRITE)) {
        LOGE("open output file failed");
        return;
    }
    //4. 初始化视频码流
    pStream = avformat_new_stream(pFormatCtx, NULL);
    if (pStream == NULL) {
        LOGE("allocating output stream failed");
        return;
    }
    //5. 寻找编码器并打开编码器
    pCodec = avcodec_find_encoder(fmt->video_codec);
    if (!pCodec) {
        LOGE("could not find encoder");
        return;
    }

    //6. 分配编码器并设置参数
    pCodecCtx = avcodec_alloc_context3(pCodec);
    pCodecCtx->codec_id = fmt->video_codec;
    pCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
    pCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
    pCodecCtx->width = height;
    pCodecCtx->height = width;
    pCodecCtx->time_base.num = 1;
    pCodecCtx->time_base.den = 25;
    pCodecCtx->bit_rate = 400000;
    pCodecCtx->gop_size = 12;

    //将AVCodecContext的成员复制到AVCodecParameters结构体
    avcodec_parameters_from_context(pStream->codecpar, pCodecCtx);
    av_stream_set_r_frame_rate(pStream, {1, 25});

    //7. 打开编码器
    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        LOGE("open encoder fail!");
        return;
    }

    //输出格式信息
    av_dump_format(pFormatCtx, 0, this->mp4Path, 1);

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

    //8. 写文件头
    avformat_write_header(pFormatCtx, NULL);

    //创建已编码帧
    av_new_packet(&avPacket, bufferSize * 3);

    //标记正在转换
    this->transform = true;
}

void MP4Encoder::EncodeBuffer(unsigned char *nv21Buffer) {

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

    //AVFrame PTS
    pFrame->pts = index++;

    //编码数据
    EncodeFrame(pCodecCtx, pFrame, &avPacket);
}

void MP4Encoder::EncodeStop() {
    //标记转换结束
    this->transform = false;

    int result = EncodeFrame(pCodecCtx, NULL, &avPacket);
    if (result >= 0) {
        //封装文件尾
        av_write_trailer(pFormatCtx);
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
        if (pFormatCtx != NULL) {
            avio_close(pFormatCtx->pb);
            avformat_free_context(pFormatCtx);
            pFormatCtx = NULL;
        }
    }
}
```

因为相机的数据都是实时的通过`onPreviewFrame`传递到`native层来`，如果我们想要编码`MP4视频文件`，这个过程需要先初始化好相关的`编码器和上下文环境`，然后`编码数据`，最后写文件尾结束，因此笔者这里使用了一个`transform字段来标记是否需要编码数据`。

**注意：**

上述的`EncodeStart函数中第6个步骤`：这里将宽和高互换的原因在于，`android的前置相机采集的数据是横向的，需要旋转270度才能正常显示`。

```java
pCodecCtx->width = height;
pCodecCtx->height = width;
```

上述的`EncodeBuffer函数开头部分`：因为android相机默认采集的数据是`NV21格式的`，并且是横向的，因此我们需要转换成`I420(就是YUV420P)的格式，并且旋转270度，旋转后的宽和高要互换`，当然了，不进行这一步也是可以的，不过你采集的`前置摄像头数据`，最终显示的就是`横向显示的视频文件`。

```java
uint8_t *i420_y = pFrameBuffer;
    uint8_t *i420_u = pFrameBuffer + width * height;
    uint8_t *i420_v = pFrameBuffer + width * height * 5 / 4;

    //NV21转I420
    libyuv::ConvertToI420(nv21Buffer, width * height, i420_y, height, i420_u, height / 2, i420_v,
                          height / 2, 0, 0, width, height, width, height, libyuv::kRotate270,
                          libyuv::FOURCC_NV21);
```

所以我们在应用程序中打开相机之后，通过`onPreviewFrame`拿到相机预览数据之后，回传给`native层处理`，通过手动点击按钮决定`开始编码`和`停止编码`。

```java
@Override
    public void onPreviewFrame(byte[] data, Camera camera) {
        this.mPreviewSize = camera.getParameters().getPreviewSize();
        if (mNativeFrame != null) {
            mNativeFrame.onPreviewFrame(data, mPreviewSize.width, mPreviewSize.height);
        }
    }
```

![](https://github.com/byhook/blog/blob/master/ndk/images/20181127140733249.png)

将编码后的文件同步下来，即可看到相关信息，因为我们还没有编码音频，所以音频不可用。

![](https://github.com/byhook/blog/blob/master/ndk/images/20181127140633767.png)


参考：

https://blog.csdn.net/leixiaohua1020/article/details/25430425

https://blog.csdn.net/luotuo44/article/details/54981809

http://www.cnblogs.com/yongdaimi/p/9804699.html
