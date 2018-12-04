package com.onzhou.ffmpeg.muxer;

/**
 * @anchor: andy
 * @date: 18-11-3
 */

public class NativeMuxer {

    static {
        System.loadLibrary("native-encode");
    }

    public native void muxerVideoAudio(String h264Path, String audioPath, String outputPath);


}
