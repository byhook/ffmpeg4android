//
// Created by byhook on 18-11-27.
//

#ifndef VIDEO_DECODER_H
#define VIDEO_DECODER_H

class VideoDecoder {

public:

    virtual int InitDecoder(const char *videoPath) = 0;

    virtual int DecodeFile(const char *yuvPath) = 0;

};

#endif //FFMPEG4ANDROID_FFMPEG_DECODE_H
