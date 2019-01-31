package com.onzhou.opensles.recorder;

/**
 * @anchor: andy
 * @date: 2019-01-30
 * @description:
 */
public class AudioRecorder {

    static {
        System.loadLibrary("native-recorder");
    }

    public native void startRecord(int sampleRate, int channels, int bitRate, String pcmPath);

    public native void stopRecord();

}
