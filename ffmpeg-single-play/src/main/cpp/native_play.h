
#include <android/native_window.h>
#include <android/native_window_jni.h>


#ifndef INCLUDE_NATIVE_PLAYER_H
#define INCLUDE_NATIVE_PLAYER_H
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

class NativePlayer {

private:

    int width = 0;
    int height = 0;
    int bufferSize = 0;
    int videoIndex = -1;

    AVPacket *vPacket = NULL;
    AVFrame *vFrame = NULL, *pFrameRGBA = NULL;
    AVCodecContext *vCodecCtx = NULL;
    SwsContext *sws_ctx = NULL;
    AVFormatContext *pFormatCtx = NULL;
    uint8_t *out_buffer = NULL;
    ANativeWindow_Buffer windowBuffer;

    AVCodec *vCodec = NULL;

public:

    int PlayVideo(const char *input_str, ANativeWindow *nativeWindow);

};


#endif
