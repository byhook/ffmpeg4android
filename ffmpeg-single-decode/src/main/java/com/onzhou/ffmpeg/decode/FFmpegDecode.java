package com.onzhou.ffmpeg.decode;

/**
 * @anchor: andy
 * @date: 2018-10-30
 * @description:
 */
public class FFmpegDecode {

    static {
        System.loadLibrary("ffmpeg");
        System.loadLibrary("ffmpeg-decode");
    }

    public native int decode(String input, String output);

}
