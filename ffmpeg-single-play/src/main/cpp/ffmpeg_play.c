
#include <jni.h>
#include <stdio.h>
#include <time.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>

#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavfilter/avfilter.h"
#include "libavutil/log.h"

#ifdef ANDROID

#include <android/log.h>
#include <assert.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>

#define LOG_TAG    "FFmpegPlay"
#define LOGE(format, ...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, format, ##__VA_ARGS__)
#define LOGI(format, ...)  __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, format, ##__VA_ARGS__)
#else
#define LOGE(format, ...)  printf(LOG_TAG format "\n", ##__VA_ARGS__)
#define LOGI(format, ...)  printf(LOG_TAG format "\n", ##__VA_ARGS__)
#endif

static AVPacket *vPacket;
static AVFrame *vFrame, *pFrameRGBA;
static AVCodecContext *vCodecCtx;
struct SwsContext *img_convert_ctx;
static AVFormatContext *pFormatCtx;
ANativeWindow *nativeWindow;
ANativeWindow_Buffer windowBuffer;
uint8_t *v_out_buffer;

JNIEXPORT jint JNICALL Java_com_onzhou_ffmpeg_play_FFmpegPlay_playVideo
        (JNIEnv *env, jobject obj, jstring videoPath, jobject surface) {
    int i;
    AVCodec *vCodec;
    char input_str[500] = {0};
    //读取输入的视频频文件地址
    sprintf(input_str, "%s", (*env)->GetStringUTFChars(env, videoPath, NULL));
    //初始化
    av_register_all();
    //分配一个AVFormatContext结构
    pFormatCtx = avformat_alloc_context();
    //打开文件
    if (avformat_open_input(&pFormatCtx, input_str, NULL, NULL) != 0) {
        LOGE("Couldn't open input stream.\n");
        return -1;
    }
    //查找文件的流信息
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        LOGE("Couldn't find stream information.\n");
        return -1;
    }
    //在流信息中找到视频流
    int videoindex = -1;
    for (i = 0; i < pFormatCtx->nb_streams; i++) {
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoindex = i;
            break;
        }
    }
    if (videoindex == -1) {
        LOGE("Couldn't find a video stream.\n");
        return -1;
    }
    //获取相应视频流的解码器
    vCodecCtx = pFormatCtx->streams[videoindex]->codec;
    vCodec = avcodec_find_decoder(vCodecCtx->codec_id);
    assert(vCodec != NULL);
    //打开解码器
    if (avcodec_open2(vCodecCtx, vCodec, NULL) < 0) {
        LOGE("Couldn't open codec.\n");
        return -1;
    }
    //获取界面传下来的surface
    nativeWindow = ANativeWindow_fromSurface(env, surface);
    if (0 == nativeWindow) {
        LOGE("Couldn't get native window from surface.\n");
        return -1;
    }
    int width = vCodecCtx->width;
    int height = vCodecCtx->height;
    //分配一个帧指针，指向解码后的原始帧
    vFrame = av_frame_alloc();
    vPacket = (AVPacket *) av_malloc(sizeof(AVPacket));
    pFrameRGBA = av_frame_alloc();
    //绑定输出buffer
    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGBA, width, height, 1);
    v_out_buffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));
    av_image_fill_arrays(pFrameRGBA->data, pFrameRGBA->linesize, v_out_buffer, AV_PIX_FMT_RGBA,
                         width, height, 1);
    img_convert_ctx = sws_getContext(width, height, vCodecCtx->pix_fmt,
                                     width, height, AV_PIX_FMT_RGBA, SWS_BICUBIC, NULL, NULL, NULL);
    if (0 >
        ANativeWindow_setBuffersGeometry(nativeWindow, width, height, WINDOW_FORMAT_RGBA_8888)) {
        LOGE("Couldn't set buffers geometry.\n");
        ANativeWindow_release(nativeWindow);
        return -1;
    }
    //读取帧
    while (av_read_frame(pFormatCtx, vPacket) >= 0) {
        if (vPacket->stream_index == videoindex) {
            //视频解码
            int ret = avcodec_send_packet(vCodecCtx, vPacket);
            if (ret < 0 && ret != AVERROR(EAGAIN) && ret != AVERROR_EOF)
                return -1;
            ret = avcodec_receive_frame(vCodecCtx, vFrame);
            if (ret < 0 && ret != AVERROR_EOF)
                return -1;
            //转化格式
            sws_scale(img_convert_ctx, (const uint8_t *const *) vFrame->data, vFrame->linesize, 0,
                      vCodecCtx->height,
                      pFrameRGBA->data, pFrameRGBA->linesize);
            if (ANativeWindow_lock(nativeWindow, &windowBuffer, NULL) < 0) {
                LOGE("cannot lock window");
            } else {
                //由于window的stride和帧的stride不同,因此需要逐行复制
                uint8_t *dst = (uint8_t *) windowBuffer.bits;
                for (int h = 0; h < height; h++) {
                    memcpy(dst + h * windowBuffer.stride * 4,
                           v_out_buffer + h * pFrameRGBA->linesize[0],
                           pFrameRGBA->linesize[0]);
                }
                ANativeWindow_unlockAndPost(nativeWindow);
            }
        }
        av_packet_unref(vPacket);
    }
    //释放内存
    sws_freeContext(img_convert_ctx);
    av_free(vPacket);
    av_free(pFrameRGBA);
    avcodec_close(vCodecCtx);
    avformat_close_input(&pFormatCtx);
    return 0;
}
