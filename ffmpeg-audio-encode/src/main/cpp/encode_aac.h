
//

#ifndef NATIVE_ENCODE_AAC_H
#define NATIVE_ENCODE_AAC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#include <libavutil/opt.h>

#ifdef __cplusplus
}
#endif

class AACEncoder {

private:

    int index = 0;
    int bufferSize = 0;

    AVFormatContext *pFormatCtx = NULL;
    AVOutputFormat *fmt = NULL;
    AVStream *audioStream = NULL;
    AVCodecContext *pCodecCtx = NULL;
    AVCodec *pCodec = NULL;

    uint8_t *audioBuffer = NULL;
    AVFrame *audioFrame = NULL;
    AVPacket audioPacket;

    SwrContext *swr;

    int EncodeFrame(AVCodecContext *pCodecCtx, AVFrame *pFrame);

public:

    int EncodeStart(const char *aacPath);

    int EncodeBuffer(const unsigned char *pcmBuffer, int length);

    int EncodeStop();

};


#endif
