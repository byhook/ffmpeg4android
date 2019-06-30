
#include <jni.h>

#include "logger.h"
#include "mp4_stream.h"


int MP4Stream::start_publish(const char *mp4Path, const char *stream) {
    //记录帧下标
    int frame_index = 0;
    //退出标记
    exit_flag = 1;
    //1.注册所有组件
    av_register_all();
    //2.初始化网络
    avformat_network_init();
    //3.打开文件输入
    if (avformat_open_input(&in_fmt, mp4Path, 0, 0) < 0) {
        LOGE("Could not open input file.");
        goto end_line;
    }
    //4.查找相关流信息
    if (avformat_find_stream_info(in_fmt, 0) < 0) {
        LOGE("Failed to retrieve input stream information");
        goto end_line;
    }
    //遍历视频轨
    for (int index = 0; index < in_fmt->nb_streams; index++){
        if (in_fmt->streams[index]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoIndex = index;
            break;
        }
    }
    //5.初始化输出码流的AVFormatContext
    avformat_alloc_output_context2(&out_fmt, NULL, "flv", stream); //RTMP
    if (!out_fmt) {
        LOGE("Could not create output context");
        goto end_line;
    }
    ofmt = out_fmt->oformat;
    for (int index = 0; index < in_fmt->nb_streams; index++) {
        //6. 根据输入流创建一个输出流
        AVStream *in_stream = in_fmt->streams[index];
        codec_ctx = avcodec_alloc_context3(NULL);
        avcodec_parameters_to_context(codec_ctx, in_stream->codecpar);
        AVStream *out_stream = avformat_new_stream(out_fmt, codec_ctx->codec);
        if (!out_stream) {
            LOGE("Failed allocating output stream");
            goto end_line;
        }
        codec_ctx->codec_tag = 0;
        if (out_fmt->oformat->flags & AVFMT_GLOBALHEADER) {
            codec_ctx->flags |= CODEC_FLAG_GLOBAL_HEADER;
        }
        if (avcodec_parameters_from_context(out_stream->codecpar, codec_ctx) < 0) {
            goto end_line;
        }
    }
    //7.打开网络输出流
    if (!(ofmt->flags & AVFMT_NOFILE)) {
        if (avio_open(&out_fmt->pb, stream, AVIO_FLAG_WRITE) < 0) {
            LOGE("Could not open output URL '%s'", stream);
            goto end_line;
        }
    }
    //8.写文件头部
    if (avformat_write_header(out_fmt, NULL) < 0) {
        LOGE("Error occurred when opening output URL");
        goto end_line;
    }

    //记录开始时间
    start_time = av_gettime();
    //读取帧数据AVPacket
    while (exit_flag && av_read_frame(in_fmt, &avPacket) >= 0) {
        if (avPacket.stream_index == videoIndex) {
            //时间基
            AVRational time_base = in_fmt->streams[videoIndex]->time_base;
            AVRational time_base_q = {1, AV_TIME_BASE};
            int64_t pts_time = av_rescale_q(avPacket.dts, time_base, time_base_q);
            int64_t now_time = av_gettime() - start_time;
            if (pts_time > now_time) {
                av_usleep(pts_time - now_time);
            }
        }
        in_stream = in_fmt->streams[avPacket.stream_index];
        out_stream = out_fmt->streams[avPacket.stream_index];

        //PTS主要用于度量解码后的视频帧什么时候被显示出来
        avPacket.pts = av_rescale_q_rnd(avPacket.pts, in_stream->time_base, out_stream->time_base,
                                        (AVRounding) (AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
        //DTS主要是标识读入内存中的字节流在什么时候开始送入解码器中进行解码
        avPacket.dts = av_rescale_q_rnd(avPacket.dts, in_stream->time_base, out_stream->time_base,
                                        (AVRounding) (AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
        avPacket.duration = av_rescale_q(avPacket.duration, in_stream->time_base,
                                         out_stream->time_base);
        avPacket.pos = -1;

        if (avPacket.stream_index == videoIndex) {
            LOGI("Send %8d video frames to output URL", frame_index);
            frame_index++;
        }
        if (av_interleaved_write_frame(out_fmt, &avPacket) < 0) {
            LOGE("Error write frame");
            break;
        }
        av_packet_unref(&avPacket);
    }
    //9.收尾工作
    av_write_trailer(out_fmt);

    end_line:

    //10.关闭
    avformat_close_input(&in_fmt);
    if (out_fmt && !(ofmt->flags & AVFMT_NOFILE)) {
        avio_close(out_fmt->pb);
    }
    avformat_free_context(out_fmt);
    return 0;
}

/**
 * 停止推流
 */
void MP4Stream::stop_publish() {
    exit_flag = 0;
}

