package com.onzhou.audio.record;

/**
 * @anchor: andy
 * @date: 18-12-1
 */

public interface IAudioRecorder {


    interface RECORD_STATE {
        int STATE_RECORDING = 0;
        int STATE_SUCCESS = 1;
        int STATE_ERROR = 2;
    }

    /**
     * 初始化
     *
     * @param filePath
     */
    void initRecorder(String filePath);

    /**
     * 开始录制音频
     */
    int recordStart();

    /**
     * 停止录制音频
     */
    void recordStop();

}
