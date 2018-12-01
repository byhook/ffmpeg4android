
#ifndef FFMPEG4ANDROID_JPEG_ENCODE_H
#define FFMPEG4ANDROID_JPEG_ENCODE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>

#ifdef __cplusplus
}
#endif


class JPEGEncoder {

private:

    int width = 0;
    int height = 0;

    int bufferSize = 0;

    char jpegPath[256] = {0};

    AVFormatContext *pFormatCtx = NULL;
    AVStream *pStream = NULL;
    AVCodecContext *pCodecCtx = NULL;
    AVCodec *pCodec = NULL;
    SwsContext *sws_ctx = NULL;
    uint8_t *out_buffer = NULL;
    AVFrame *pFrame = NULL;
    AVPacket avPacket;

    bool transform = false;

public:

    JPEGEncoder(const char *jpegPath, int width, int height);
    ~JPEGEncoder();

    int EncodeJPEG(unsigned char *nv21Buffer);


    bool isTransform();

};


#endif //FFMPEG4ANDROID_JPEG_ENCODE_H
