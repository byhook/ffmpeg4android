
//

#ifndef FFMPEG4ANDROID_RECORD_BUFFER_H
#define FFMPEG4ANDROID_RECORD_BUFFER_H


class RecordBuffer {
public:
    short **buffer;
    int index = -1;
public:
    RecordBuffer(int buffersize);

    ~RecordBuffer();

    //即将录入PCM数据的buffer
    short *getRecordBuffer();

    //当前录制好的PCM数据的buffer,可以写入文件
    short *getNowBuffer();
};

#endif //FFMPEG4ANDROID_RECORD_BUFFER_H
