
#ifndef ENCODE_H264_H
#define ENCODE_H264_H

#include "video_publish.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libavutil/avutil.h>
#include <libavutil/time.h>

#ifdef __cplusplus
}
#endif

class H264Publisher : public VideoPublisher {

private:

    const char *outputPath;

    int width;

    int height;

    AVPacket avPacket;
    AVFormatContext *out_fmt = NULL;
    AVStream *pStream = NULL;
    AVCodecContext *pCodecCtx = NULL;
    AVCodec *pCodec = NULL;
    uint8_t *pFrameBuffer = NULL;
    AVFrame *pFrame = NULL;
    //AVFrame PTS
    int index = 0;

    int fps = 15;

    int EncodeFrame(AVCodecContext *pCodecCtx, AVFrame *pFrame, AVPacket *pPkt);

public:

    void InitPublish(const char *outputPath, int width, int height);

    void StartPublish();

    void EncodeBuffer(unsigned char *nv21Buffer);

    void StopPublish();

};

#endif //ENCODE_H264_H
