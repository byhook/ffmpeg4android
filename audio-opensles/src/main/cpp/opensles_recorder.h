
//

#ifndef FFMPEG4ANDROID_NATIVE_OPENSLES_H
#define FFMPEG4ANDROID_NATIVE_OPENSLES_H

#include <string>
#include <stdio.h>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include "record_buffer.h"

class OpenSLESRecorder {

private:

    SLObjectItf engineObject = nullptr;   //引擎对象
    SLEngineItf engineEngine = nullptr;   //引擎接口

    SLObjectItf recorderObject = nullptr;                         //录制对象，这个对象我们从里面获取了2个接口

public:

    SLRecordItf recorderRecorder = nullptr;                        //录制接口

    RecordBuffer *recordBuffer;

    FILE *pcmFile;

    bool finished;

    void StartRecord(const char *pcmPath, int sampleRate, int channels, int bitRate);

    void StopRecord();

};


#endif //FFMPEG4ANDROID_NATIVE_OPENSLES_H
