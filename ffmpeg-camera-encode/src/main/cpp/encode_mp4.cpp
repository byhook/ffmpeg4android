
#include <libyuv.h>
#include "logger.h"
#include "encode_mp4.h"

void MP4Encoder::InitEncoder(const char *mp4Path, int width, int height) {
    this->mp4Path = mp4Path;
    this->width = width;
    this->height = height;
}

int MP4Encoder::FlushEncoder(AVFormatContext *fmt_ctx, unsigned int stream_index) {
    int ret;
    int got_frame;
    AVPacket enc_pkt;
    if (!(fmt_ctx->streams[stream_index]->codec->codec->capabilities &
          CODEC_CAP_DELAY))
        return 0;
    while (1) {
        printf("Flushing stream #%u encoder\n", stream_index);
        enc_pkt.data = NULL;
        enc_pkt.size = 0;
        av_init_packet(&enc_pkt);
        ret = avcodec_encode_video2(fmt_ctx->streams[stream_index]->codec, &enc_pkt,
                                    NULL, &got_frame);
        av_frame_free(NULL);
        if (ret < 0)
            break;
        if (!got_frame) {
            ret = 0;
            break;
        }
        LOGE("success encoder 1 frame");

        // parpare packet for muxing
        enc_pkt.stream_index = stream_index;
        av_packet_rescale_ts(&enc_pkt,
                             fmt_ctx->streams[stream_index]->codec->time_base,
                             fmt_ctx->streams[stream_index]->time_base);
        ret = av_interleaved_write_frame(fmt_ctx, &enc_pkt);
        if (ret < 0)
            break;
    }
    return ret;
}

void MP4Encoder::EncodeStart() {
    //注册所有组件
    av_register_all();
    //初始化AVFormatContext结构体,根据文件名获取到合适的封装格式
    avformat_alloc_output_context2(&pFormatCtx, NULL, NULL, this->mp4Path);
    fmt = pFormatCtx->oformat;

    //打开待输出的视频文件
    if (avio_open(&pFormatCtx->pb, this->mp4Path, AVIO_FLAG_READ_WRITE)) {
        LOGE("open output file failed");
        return;
    }

    //初始化视频码流
    video_st = avformat_new_stream(pFormatCtx, 0);
    if (video_st == NULL) {
        printf("allocating output stream failed");
        return;
    }
    video_st->time_base.num = 1;
    video_st->time_base.den = 25;

    //编码器Context设置参数
    pCodecCtx = video_st->codec;
    pCodecCtx->codec_id = fmt->video_codec;
    pCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
    pCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
    pCodecCtx->width = height;
    pCodecCtx->height = width;
    pCodecCtx->time_base.num = 1;
    pCodecCtx->time_base.den = 25;
    pCodecCtx->bit_rate = 400000;
    pCodecCtx->gop_size = 12;

    //寻找编码器并打开编码器
    pCodec = avcodec_find_encoder(pCodecCtx->codec_id);
    if (!pCodec) {
        LOGE("no right encoder!");
        return;
    }
    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        LOGE("open encoder fail!");
        return;
    }

    //输出格式信息
    av_dump_format(pFormatCtx, 0, this->mp4Path, 1);

    //初始化帧
    picture = av_frame_alloc();
    picture->width = pCodecCtx->width;
    picture->height = pCodecCtx->height;
    picture->format = pCodecCtx->pix_fmt;
    int size = avpicture_get_size(pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height);
    picture_buf = (uint8_t *) av_malloc(size);
    avpicture_fill((AVPicture *) picture, picture_buf, pCodecCtx->pix_fmt, pCodecCtx->width,
                   pCodecCtx->height);

    //写文件头部
    avformat_write_header(pFormatCtx, NULL);

    //创建已编码帧
    av_new_packet(&avPacket, size * 3);

    //标记正在转换
    this->transform = true;
}

void MP4Encoder::EncodeBuffer(unsigned char *nv21Buffer) {

    uint8_t *i420_y = picture_buf;
    uint8_t *i420_u = picture_buf + width * height;
    uint8_t *i420_v = picture_buf + width * height * 5 / 4;

    //NV21转I420
    libyuv::ConvertToI420(nv21Buffer, width * height, i420_y, height, i420_u, height / 2, i420_v,
                          height / 2, 0, 0, width, height, width, height, libyuv::kRotate270,
                          libyuv::FOURCC_NV21);

    picture->data[0] = i420_y;
    picture->data[1] = i420_u;
    picture->data[2] = i420_v;

    //AVFrame PTS
    picture->pts = index++;
    int got_picture = 0;

    //编码
    int ret = avcodec_encode_video2(pCodecCtx, &avPacket, picture, &got_picture);
    if (ret < 0) {
        LOGE("encoder failed");
        return;
    }

    if (got_picture == 1) {
        avPacket.stream_index = video_st->index;
        av_packet_rescale_ts(&avPacket, pCodecCtx->time_base, video_st->time_base);
        avPacket.pos = -1;
        av_interleaved_write_frame(pFormatCtx, &avPacket);
        av_free_packet(&avPacket);
    }
}

void MP4Encoder::EncodeStop() {
    //标记转换结束
    this->transform = false;

    int ret = FlushEncoder(pFormatCtx, 0);
    if (ret < 0) {
        LOGE("flushing encoder failed!");
        return;
    }
    //封装文件尾
    av_write_trailer(pFormatCtx);

    //释放内存
    if (video_st) {
        avcodec_close(video_st->codec);
        av_free(picture);
        av_free(picture_buf);
    }
    if (pFormatCtx) {
        avio_close(pFormatCtx->pb);
        avformat_free_context(pFormatCtx);
    }
}