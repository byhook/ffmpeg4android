package com.onzhou.common.avcapture;

/**
 * @anchor: andy
 * @date: 18-12-4
 */

public interface IAudioCapture {

    /**
     * 打开音频设备
     *
     * @param samplesRate
     * @param channels
     * @param audioFormat
     * @return
     */
    int openAudio(int samplesRate, int channels, int audioFormat);

    /**
     * 关闭音频
     */
    void closeAudio();

    /**
     * 音频是否已打开
     *
     * @return
     */
    boolean isOpenAudio();

    /**
     * 开始捕获音频
     *
     * @return
     */
    int startAudioCapture();

    /**
     * 停止捕获音频
     */
    void stopAudioCapture();

    /**
     * 是否正在捕获音频数据
     *
     * @return
     */
    boolean isStartAudioCapture();

    /**
     * 获取音频数据
     *
     * @param paramArrayOfByte
     * @param paramInt
     * @return
     */
    int getAudioBuffer(byte[] paramArrayOfByte, int paramInt);

    interface ReturnCode {
        int RET_SUCCESS = 0;

        int RET_ERROR_OCCUPIED = -11;
        int RET_ERROR_PARAM = -12;
        int RET_ERROR_BUFFER = -13;

        int RET_CAPTURE_NO_START = -21;
        int RET_NO_AUDIO_DATA = -23;
    }

}
