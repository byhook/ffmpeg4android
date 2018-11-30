
#ifndef INCLUDE_NATIVE_SW_SCALE_H
#define INCLUDE_NATIVE_SW_SCALE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavfilter/avfilter.h"
#include "libavutil/log.h"
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>


#ifdef __cplusplus
}
#endif

class NativeSwscale {

private:

    int bufferSize = 0;

    FILE *outputFile = NULL;

    AVFormatContext *pFormatCtx = NULL;
    int videoIndex = -1;
    AVCodecContext *pCodecCtx = NULL;
    AVCodec *pCodec = NULL;
    AVFrame *pFrame = NULL, *pFrameYUV = NULL;
    AVPacket *vPacket = NULL;
    uint8_t *out_buffer = NULL;
    SwsContext *sws_ctx = NULL;
public:

    int Transform(const char *videoPath, const char *yuvPath);

};


#endif //INCLUDE_NATIVE_SW_SCALE_H
