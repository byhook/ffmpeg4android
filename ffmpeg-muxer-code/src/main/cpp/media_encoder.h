
#ifndef INCLUDE_MEDIA_ENCODER_H
#define INCLUDE_MEDIA_ENCODER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>

#ifdef __cplusplus
}
#endif

class MP4Encoder {

private:

    int frameIndex = 0;

    int videoIndex = -1;
    int audioIndex = -1;

    int audioStreamIndex = -1;
    int videoStreamIndex = -1;

    int64_t cur_pts_v = 0, cur_pts_a = 0;

    AVPacket avPacket;

    AVFormatContext *videoFormatCtx = NULL;
    AVFormatContext *audioFormatCtx = NULL;

public:

    AVFormatContext *outFormatCtx = NULL;

    int NewStream(AVFormatContext *avFormatContext, int &inputIndex, AVMediaType mediaType);

    void WritePTS(AVPacket *avPacket, AVStream *inputStream);

    int Transform(const char *videoPath, const char *audioPath, const char *outputPath);

};


#endif

