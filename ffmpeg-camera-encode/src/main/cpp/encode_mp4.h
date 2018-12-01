
#ifndef MP4_ENCODER_H
#define MP4_ENCODER_H

#include "encode_video.h"

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
    AVFormatContext *pFormatCtx = NULL;
    AVStream *pStream = NULL;
    AVCodecContext *pCodecCtx = NULL;
    AVCodec *pCodec = NULL;
    uint8_t *pFrameBuffer = NULL;
    AVFrame *pFrame = NULL;

    //AVFrame PTS
    int index = 0;

    int EncodeFrame(AVCodecContext *pCodecCtx, AVFrame *pFrame, AVPacket *pPkt);

public:

    void InitEncoder(const char *mp4Path, int width, int height);

    void EncodeStart();

    void EncodeBuffer(unsigned char *nv21Buffer);

    void EncodeStop();

};

#endif //FFMPEG4ANDROID_ENCODE_MP4_H
