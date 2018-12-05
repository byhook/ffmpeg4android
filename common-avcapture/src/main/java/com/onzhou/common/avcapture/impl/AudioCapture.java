package com.onzhou.common.avcapture.impl;

import android.media.AudioRecord;
import android.media.MediaRecorder;

import com.onzhou.common.avcapture.IAudioCapture;

import java.util.LinkedList;
import java.util.Queue;

/**
 * @anchor: andy
 * @date: 18-12-4
 */

public class AudioCapture implements IAudioCapture {

    /**
     * 单声道缓冲区
     */
    private static final int BUFFER_LENGTH_MONO = 4096;

    /**
     * 双声道缓冲区
     */
    private static final int BUFFER_LENGTH_STEREO = 8192;

    private int mBufferSize = 0;

    /**
     * 音频数据
     */
    private AudioData mAudioData = null;

    private byte[] mAudioBuffer = null;

    /**
     * 音频录制实例
     */
    private AudioRecord mAudioRecord = null;

    /**
     * 是否打开音频设备
     */
    private boolean mOpenAudioDevice = false;

    /**
     * 是否已经开始音频捕获
     */
    private boolean mStartAudioCapture = false;

    /**
     * 同步锁
     */
    private Object lock = new Object();

    /**
     * 音频录制线程
     */
    private AudioRecordThread mAudioRecordThread = null;

    /**
     * 音频源
     */
    private static final int AUDIO_SOURCE = MediaRecorder.AudioSource.MIC;

    /**
     * 音频数据队列
     */
    private Queue<AudioData> mAudioQueue = new LinkedList();

    @Override
    public int openAudio(int samplesRate, int channels, int audioFormat) {
        mAudioQueue.clear();
        if (channels >= 1 && channels <= 2) {
            int iChannelInfo = 0;
            if (mOpenAudioDevice) {
                return ReturnCode.RET_ERROR_OCCUPIED;
            }
            mBufferSize = AudioRecord.getMinBufferSize(samplesRate, channels, audioFormat);
            mAudioBuffer = new byte[mBufferSize];
            if ((lock == null) || (mAudioBuffer == null) ||
                    (mAudioQueue == null)) {
                return ReturnCode.RET_ERROR_BUFFER;
            }
            mAudioRecord = new AudioRecord(AUDIO_SOURCE, samplesRate, iChannelInfo, audioFormat, mBufferSize);
            if (mAudioRecord == null) {
                mAudioBuffer = null;
                return ReturnCode.RET_ERROR_PARAM;
            }
            mOpenAudioDevice = true;
            return ReturnCode.RET_SUCCESS;
        } else {
            return ReturnCode.RET_ERROR_PARAM;
        }
    }

    @Override
    public boolean isOpenAudio() {
        return mOpenAudioDevice;
    }

    @Override
    public void closeAudio() {
        if (mOpenAudioDevice) {
            stopAudioCapture();
            mAudioRecord.release();
            mAudioBuffer = null;
            mAudioRecord = null;
            mOpenAudioDevice = false;
        }
    }

    @Override
    public int startAudioCapture() {
        if ((mOpenAudioDevice) && (!mStartAudioCapture)) {
            mStartAudioCapture = true;
            mAudioRecordThread = new AudioRecordThread();
            mAudioRecord.startRecording();
            mAudioRecordThread.start();
        }
        return ReturnCode.RET_SUCCESS;
    }

    @Override
    public boolean isStartAudioCapture() {
        return mStartAudioCapture;
    }

    @Override
    public void stopAudioCapture() {
        if (mStartAudioCapture) {
            mAudioRecordThread.exit();
            mAudioRecord.stop();
        }
    }

    /**
     * 从缓冲区中获取音频数据
     *
     * @param audioBuffer
     * @param bufferLength
     * @return
     */
    @Override
    public int getAudioBuffer(byte[] audioBuffer, int bufferLength) {
        if (!mStartAudioCapture)
            return ReturnCode.RET_CAPTURE_NO_START;
        if ((audioBuffer == null) || (bufferLength < mBufferSize)) {
            return ReturnCode.RET_ERROR_PARAM;
        }
        synchronized (lock) {
            mAudioData = mAudioQueue.poll();
            if (mAudioData == null) {
                return ReturnCode.RET_NO_AUDIO_DATA;
            }
            System.arraycopy(mAudioData.audioBuffer, 0, audioBuffer, 0,
                    mAudioData.length);
        }
        return ReturnCode.RET_SUCCESS;
    }

    private class AudioData {

        /**
         * 音频数据
         */
        byte[] audioBuffer;

        /**
         * 长度
         */
        int length;
    }

    private class AudioRecordThread extends Thread {

        public void run() {
            while (mStartAudioCapture) {
                int readSize = mAudioRecord.read(mAudioBuffer, 0, mBufferSize);
                if (readSize > 0) {
                    synchronized (lock) {
                        mAudioData = new AudioData();
                        mAudioData.audioBuffer = mAudioBuffer;
                        mAudioData.length = readSize;
                        mAudioQueue.offer(mAudioData);
                    }
                }
            }
        }

        public void exit() {
            mStartAudioCapture = false;
        }
    }
}
