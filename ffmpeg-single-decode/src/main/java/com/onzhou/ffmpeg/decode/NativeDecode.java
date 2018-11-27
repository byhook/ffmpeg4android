package com.onzhou.ffmpeg.decode;

/**
 * @anchor: andy
 * @date: 2018-10-30
 * @description:
 */
public class NativeDecode {

    static {
        System.loadLibrary("native-decode");
    }

    public native void decodeMP4(String input, String output);

}
