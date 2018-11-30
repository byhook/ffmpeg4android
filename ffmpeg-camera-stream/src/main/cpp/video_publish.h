
#ifndef ANDROID_VIDEO_PUBLISHER_H
#define ANDROID_VIDEO_PUBLISHER_H


class VideoPublisher {

protected:

    bool transform = false;

public:

    virtual void InitPublish(const char *mp4Path, int width, int height) = 0;

    virtual void StartPublish() = 0;

    virtual void EncodeBuffer(unsigned char *nv21Buffer) = 0;

    virtual void StopPublish() = 0;

    bool isTransform();

};

#endif //ANDROID_VIDEO_ENCODER_H
