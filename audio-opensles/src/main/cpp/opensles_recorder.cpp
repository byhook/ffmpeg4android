
//
#include "logger.h"
#include "record_buffer.h"
#include "opensles_recorder.h"
#include <SLES/OpenSLES_Android.h>
#include <stdio.h>

//录制大小设为4096
#define RECORDER_FRAMES (2048)
static unsigned recorderSize = RECORDER_FRAMES * 2;

static void RecordCallback(SLAndroidSimpleBufferQueueItf bufferQueue, void *context) {

    LOGI("录制大小: %d", recorderSize);

    OpenSLESRecorder *recorder = (OpenSLESRecorder *) context;

    if (NULL != recorder->recordBuffer) {
        fwrite(recorder->recordBuffer->getNowBuffer(), 1, recorderSize, recorder->pcmFile);
    }

    if (recorder->finished) {
        (*recorder->recorderRecorder)->SetRecordState(recorder->recorderRecorder,
                                                      SL_RECORDSTATE_STOPPED);
        //刷新缓冲区后，关闭流
        fclose(recorder->pcmFile);
        //释放内存
        delete recorder->recordBuffer;
        recorder->recordBuffer = NULL;
        LOGI("停止录音");
    } else {
        (*bufferQueue)->Enqueue(bufferQueue, recorder->recordBuffer->getRecordBuffer(),
                                recorderSize);
    }
}

void OpenSLESRecorder::StartRecord(const char *pcmPath, int sampleRate, int channels, int bitRate) {

    //打开输出文件
    pcmFile = fopen(pcmPath, "w");

    recordBuffer = new RecordBuffer(RECORDER_FRAMES * 2);

    //1. 调用全局方法创建一个引擎对象(OpenSL ES唯一入口)
    SLresult result = slCreateEngine(&engineObject, 0, NULL, 0, NULL, NULL);
    if (SL_RESULT_SUCCESS != result) {
        return;
    }
    //2. 实例化这个对象
    result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    if (SL_RESULT_SUCCESS != result) {
        return;
    }
    //3. 从这个对象里面获取引擎接口
    result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);
    if (SL_RESULT_SUCCESS != result) {
        return;
    }
    //4. 设置IO设备(麦克风)
    SLDataLocator_IODevice ioDevice = {
            SL_DATALOCATOR_IODEVICE,         //类型
            SL_IODEVICE_AUDIOINPUT,          //device类型 选择了音频输入类型
            SL_DEFAULTDEVICEID_AUDIOINPUT,   //deviceID
            NULL                             //device实例
    };
    SLDataSource dataSource = {
            &ioDevice,                      //SLDataLocator_IODevice配置输入
            NULL                             //输入格式,采集的并不需要
    };

    //5. 设置输出buffer队列
    SLDataLocator_AndroidSimpleBufferQueue buffer_queue = {
            SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,    //类型 这里只能是这个常量
            2                                           //buffer的数量
    };
    //6. 设置输出数据的格式
    SLDataFormat_PCM pcmFormat = {
            SL_DATAFORMAT_PCM,                             //输出PCM格式的数据
            (SLuint32) channels,                                  //输出的声道数量
            SL_SAMPLINGRATE_44_1,                          //输出的采样频率，这里是44100Hz
            SL_PCMSAMPLEFORMAT_FIXED_16,                   //输出的采样格式，这里是16bit
            SL_PCMSAMPLEFORMAT_FIXED_16,                   //一般来说，跟随上一个参数
            SL_SPEAKER_FRONT_LEFT |
            SL_SPEAKER_FRONT_RIGHT,  //双声道配置，如果单声道可以用 SL_SPEAKER_FRONT_CENTER
            SL_BYTEORDER_LITTLEENDIAN                      //PCM数据的大小端排列
    };
    SLDataSink audioSink = {
            &buffer_queue,                   //SLDataFormat_PCM配置输出
            &pcmFormat                      //输出数据格式
    };

    SLAndroidSimpleBufferQueueItf recorderBufferQueue; //Buffer接口

    //7. 创建录制的对象
    const SLInterfaceID id[1] = {SL_IID_ANDROIDSIMPLEBUFFERQUEUE};
    const SLboolean req[1] = {SL_BOOLEAN_TRUE};
    result = (*engineEngine)->CreateAudioRecorder(engineEngine,        //引擎接口
                                                  &recorderObject,   //录制对象地址，用于传出对象
                                                  &dataSource,          //输入配置
                                                  &audioSink,         //输出配置
                                                  1,                  //支持的接口数量
                                                  id,                 //具体的要支持的接口
                                                  req                 //具体的要支持的接口是开放的还是关闭的
    );
    if (SL_RESULT_SUCCESS != result) {
        return;
    }
    //8. 实例化这个录制对象
    result = (*recorderObject)->Realize(recorderObject, SL_BOOLEAN_FALSE);
    if (SL_RESULT_SUCCESS != result) {
        return;
    }
    //9. 获取录制接口
    (*recorderObject)->GetInterface(recorderObject, SL_IID_RECORD, &recorderRecorder);
    //10. 获取Buffer接口
    (*recorderObject)->GetInterface(recorderObject, SL_IID_ANDROIDSIMPLEBUFFERQUEUE,
                                    &recorderBufferQueue);

    finished = false;

    result = (*recorderBufferQueue)->Enqueue(recorderBufferQueue, recordBuffer->getRecordBuffer(),
                                             recorderSize);
    if (SL_RESULT_SUCCESS != result) {
        return;
    }
    result = (*recorderBufferQueue)->RegisterCallback(recorderBufferQueue, RecordCallback,
                                                      this);
    if (SL_RESULT_SUCCESS != result) {
        return;
    }
    //11. 开始录音
    (*recorderRecorder)->SetRecordState(recorderRecorder, SL_RECORDSTATE_RECORDING);

}

void OpenSLESRecorder::StopRecord() {
    if (NULL != recorderRecorder) {
        finished = true;
    }
}