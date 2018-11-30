package com.onzhou.ffmpeg.swscale;

/**
 * @anchor: andy
 * @date: 2018-10-31
 * @description:
 */
public class NativeSwscale {

    static {
        System.loadLibrary("native-swscale");
    }

    public native int mp4ToYuv(String mp4Path, String yuvPath);

}
