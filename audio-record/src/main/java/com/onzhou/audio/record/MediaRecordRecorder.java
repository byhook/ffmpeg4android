package com.onzhou.audio.record;

import android.media.MediaRecorder;
import android.os.Environment;

import java.io.File;
import java.io.IOException;

/**
 * @anchor: andy
 * @date: 18-12-1
 * @description: 基于MediaRecorder录制音频数据, 支持wav, amr等格式。
 */

public class MediaRecordRecorder implements IAudioRecorder {

    /**
     * 基于MediaRecorder录制音频
     */
    private MediaRecorder mMediaRecorder;

    /**
     * 是否正在录制
     */
    private boolean isRecord = false;

    @Override
    public void initRecorder(String filePath) {
        //实例化MediaRecorder对象
        mMediaRecorder = new MediaRecorder();

        //从麦克风采集声音数据
        mMediaRecorder.setAudioSource(MediaRecorder.AudioSource.MIC);
        //设置输出格式为MP4
        mMediaRecorder.setOutputFormat(MediaRecorder.OutputFormat.MPEG_4);
        //设置采样频率44100 频率越高,音质越好,文件越大
        mMediaRecorder.setAudioSamplingRate(44100);
        //设置声音数据编码格式,音频通用格式是AAC
        mMediaRecorder.setAudioEncoder(MediaRecorder.AudioEncoder.AAC);
        //设置编码频率
        mMediaRecorder.setAudioEncodingBitRate(96000);
        //设置输出文件
        mMediaRecorder.setOutputFile(filePath);
    }

    @Override
    public int recordStart() {
        //判断是否有外部存储设备sdcard
        if (isRecord) {
            return RECORD_STATE.STATE_RECORDING;
        } else {
            if (mMediaRecorder != null) {
                try {
                    mMediaRecorder.prepare();
                    mMediaRecorder.start();
                    // 让录制状态为true
                    isRecord = true;
                    return RECORD_STATE.STATE_SUCCESS;
                } catch (IOException e) {
                    e.printStackTrace();

                }
            }
            return RECORD_STATE.STATE_ERROR;
        }
    }

    @Override
    public void recordStop() {
        if (mMediaRecorder != null) {
            isRecord = false;
            mMediaRecorder.stop();
            mMediaRecorder.release();
            mMediaRecorder = null;
        }
    }

}
