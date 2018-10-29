package com.onzhou.ffmpeg;

/**
 * @anchor: andy
 * @date: 2018-10-29
 * @description:
 */
public class NowStreamer {

    static {
        System.loadLibrary("ffmpeg");
        System.loadLibrary("now-ffmpeg");
    }

    public native String getAvFormatInfo();


}
