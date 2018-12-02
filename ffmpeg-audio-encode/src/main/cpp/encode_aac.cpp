
#include "encode_aac.h"
#include "logger.h"

int AACEncoder::EncodeFrame(AVCodecContext *pCodecCtx, AVFrame *audioFrame) {
    int ret = avcodec_send_frame(pCodecCtx, audioFrame);
    if (ret < 0) {
        LOGE("Could't send frame");
        return -1;
    }
    while (avcodec_receive_packet(pCodecCtx, &audioPacket) == 0) {
        audioPacket.stream_index = audioStream->index;
        ret = av_interleaved_write_frame(pFormatCtx, &audioPacket);
        av_packet_unref(&audioPacket);
    }
    return ret;
}

int AACEncoder::EncodeStart(const char *aacPath) {
    //1.注册所有组件
    av_register_all();
    //2.获取输出文件的上下文环境
    avformat_alloc_output_context2(&pFormatCtx, NULL, NULL, aacPath);
    fmt = pFormatCtx->oformat;
    //3.打开输出文件
    if (avio_open(&pFormatCtx->pb, aacPath, AVIO_FLAG_READ_WRITE) < 0) {
        LOGE("Could't open output file");
        return -1;
    }
    //4.新建音频流
    audioStream = avformat_new_stream(pFormatCtx, NULL);
    if (audioStream == NULL) {
        return -1;
    }
    //5.寻找编码器并打开编码器
    pCodec = avcodec_find_encoder(fmt->audio_codec);
    if (pCodec == NULL) {
        LOGE("Could not find encoder");
        return -1;
    }

    //6.分配编码器并设置参数
    pCodecCtx = audioStream->codec;
    pCodecCtx->strict_std_compliance = FF_COMPLIANCE_EXPERIMENTAL;
    pCodecCtx->codec_id = fmt->audio_codec;
    pCodecCtx->codec_type = AVMEDIA_TYPE_AUDIO;
    pCodecCtx->sample_fmt = AV_SAMPLE_FMT_FLTP;
    pCodecCtx->sample_rate = 44100;
    pCodecCtx->channel_layout = AV_CH_LAYOUT_STEREO;
    pCodecCtx->channels = av_get_channel_layout_nb_channels(pCodecCtx->channel_layout);
    pCodecCtx->bit_rate = 96000;
    if (pFormatCtx->oformat->flags & AVFMT_GLOBALHEADER) {
        pCodecCtx->flags |= CODEC_FLAG_GLOBAL_HEADER;
    }

    //7.打开音频编码器
    int result = avcodec_open2(pCodecCtx, pCodec, NULL);
    if (result < 0) {
        LOGE("Could't open encoder");
        return -1;
    }

    audioFrame = av_frame_alloc();
    audioFrame->nb_samples = pCodecCtx->frame_size;
    audioFrame->format = pCodecCtx->sample_fmt;

    bufferSize = av_samples_get_buffer_size(NULL, pCodecCtx->channels, pCodecCtx->frame_size,
                                            pCodecCtx->sample_fmt, 1);
    audioBuffer = (uint8_t *) av_malloc(bufferSize);
    avcodec_fill_audio_frame(audioFrame, pCodecCtx->channels, pCodecCtx->sample_fmt,
                             (const uint8_t *) audioBuffer, bufferSize, 1);


    //8.写文件头
    avformat_write_header(pFormatCtx, NULL);
    av_new_packet(&audioPacket, bufferSize);

    //9.用于音频转码
    swr = swr_alloc();
    av_opt_set_channel_layout(swr, "in_channel_layout", AV_CH_LAYOUT_STEREO, 0);
    av_opt_set_channel_layout(swr, "out_channel_layout", AV_CH_LAYOUT_STEREO, 0);
    av_opt_set_int(swr, "in_sample_rate", 44100, 0);
    av_opt_set_int(swr, "out_sample_rate", 44100, 0);
    av_opt_set_sample_fmt(swr, "in_sample_fmt", AV_SAMPLE_FMT_S16, 0);
    av_opt_set_sample_fmt(swr, "out_sample_fmt", AV_SAMPLE_FMT_FLTP, 0);
    swr_init(swr);

    return 0;
}

int AACEncoder::EncodeBuffer(const unsigned char *pcmBuffer, int len) {

    uint8_t *outs[2];
    outs[0] = new uint8_t[len];
    outs[1] = new uint8_t[len];
    int count = swr_convert(swr, (uint8_t **) &outs, len * 4, &pcmBuffer, len / 4);
    audioFrame->data[0] = outs[0];
    audioFrame->data[1] = outs[1];

    if (count >= 0) {
        EncodeFrame(pCodecCtx, audioFrame);
    } else {
        char errorMessage[1024] = {0};
        av_strerror(len, errorMessage, sizeof(errorMessage));
        LOGE("error message: %s", errorMessage);
    }

    delete outs[0];
    delete outs[1];
    return 0;
}

int AACEncoder::EncodeStop() {

    EncodeFrame(pCodecCtx, NULL);
    //10.写文件尾
    av_write_trailer(pFormatCtx);

    //释放
    avcodec_close(pCodecCtx);
    av_free(audioFrame);
    av_free(audioBuffer);
    avio_close(pFormatCtx->pb);
    avformat_free_context(pFormatCtx);
    return 0;
}


