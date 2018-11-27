
#ifndef MP4_DECODER_H
#define MP4_DECODER_H

#include "video_decode.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/log.h"
#include <libavutil/imgutils.h>

#ifdef __cplusplus
}
#endif

class MP4Decoder : public VideoDecoder {

private:

    AVFormatContext *pFormatCtx = nullptr;
    AVCodecContext *pCodecCtx = nullptr;
    AVCodec *pCodec = nullptr;
    AVFrame *pFrame, *pFrameYUV = nullptr;
    AVPacket *pAvPacket = nullptr;
    SwsContext *pSwsContext = nullptr;

    FILE *yuv_file = nullptr;

    int DecodePacket(AVCodecContext *pCodecCtx, AVPacket *pAvPacket, AVFrame *pFrame);

public:

    int InitDecoder(const char *mp4Path);

    int DecodeFile(const char *yuvPath);

};

#endif //FFMPEG4ANDROID_FFMPEG_DECODE_H
