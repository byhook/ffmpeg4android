//
// Created by byhook on 18-11-26.
//

#ifndef FFMPEG4ANDROID_ENCODE_MP4_H
#define FFMPEG4ANDROID_ENCODE_MP4_H

#include "video_encoder.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>

#ifdef __cplusplus
}
#endif

class MP4Encoder : public VideoEncoder {

private:

    const char *mp4Path;

    int width;

    int height;

    AVPacket avPacket;

    AVFormatContext *pFormatCtx = nullptr;

    AVOutputFormat *fmt = nullptr;

    AVStream *video_st = nullptr;

    AVCodecContext *pCodecCtx = nullptr;

    AVCodec *pCodec = nullptr;

    uint8_t *picture_buf = nullptr;

    AVFrame *picture = nullptr;

    //AVFrame PTS
    int index = 0;

    int FlushEncoder(AVFormatContext *fmt_ctx, unsigned int stream_index);

public:

    void InitEncoder(const char *mp4Path, int width, int height);

    void EncodeStart();

    void EncodeBuffer(unsigned char *nv21Buffer);

    void EncodeStop();

};

#endif //FFMPEG4ANDROID_ENCODE_MP4_H
