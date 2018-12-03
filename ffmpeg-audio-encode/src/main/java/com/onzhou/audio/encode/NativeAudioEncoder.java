package com.onzhou.audio.encode;

/**
 * @anchor: andy
 * @date: 18-12-2
 */

public class NativeAudioEncoder implements IAudioRecorder, AudioRecordRecorder.OnAudioRecordListener {

    static {
        System.loadLibrary("native-encode");
    }

    public native void encodeAudioStart(String aacPath);

    public native void encodeAudioStop();

    public native void onAudioFrame(byte[] pcmData, int length);

    /**
     * 录制原始的音频数据
     */
    private AudioRecordRecorder audioRecorder;

    /**
     * 输出的aac文件路径
     */
    private String aacPath;

    public NativeAudioEncoder(String path) {
        this.aacPath = path;
        audioRecorder = new AudioRecordRecorder(null);
        audioRecorder.setOnAudioRecordListener(this);
    }

    @Override
    public void onAudioBuffer(byte[] buffer, int length) {
        onAudioFrame(buffer, length);
    }

    @Override
    public void initRecorder() {
        //初始化native层编码器
        encodeAudioStart(aacPath);
        //初始化音频录制
        if (audioRecorder != null) {
            audioRecorder.initRecorder();
        }
    }

    @Override
    public int recordStart() {
        if (audioRecorder != null) {
            return audioRecorder.recordStart();
        }
        return -1;
    }

    @Override
    public void recordStop() {
        //停止音频录制
        if (audioRecorder != null) {
            audioRecorder.recordStop();
        }
        //停止native层编码
        encodeAudioStop();
    }

}
