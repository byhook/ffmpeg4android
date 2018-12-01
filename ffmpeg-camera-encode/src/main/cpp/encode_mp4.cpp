
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
        //printf("Write frame %d, size=%d\n", avPacket->pts, avPacket->size);
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
    pCodec = avcodec_find_encoder(AV_CODEC_ID_MPEG4);
    if (!pCodec) {
        LOGE("could not find encoder");
        return;
    }

    //6. 分配编码器并设置参数
    pCodecCtx = avcodec_alloc_context3(pCodec);
    pCodecCtx->codec_id = pCodec->id;
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

    AVDictionary *opt = 0;
    //H.264
    if (pCodecCtx->codec_id == AV_CODEC_ID_H264) {
        av_dict_set_int(&opt, "video_track_timescale", 25, 0);
        av_dict_set(&opt, "preset", "slow", 0);
        av_dict_set(&opt, "tune", "zerolatency", 0);
    }
    //8. 写文件头
    avformat_write_header(pFormatCtx, &opt);

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