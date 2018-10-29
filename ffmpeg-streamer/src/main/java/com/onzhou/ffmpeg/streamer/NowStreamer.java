package com.onzhou.ffmpeg.streamer;

/**
 * @anchor: andy
 * @date: 2018-10-29
 * @description:
 */
public class NowStreamer {

    static {
        System.loadLibrary("ffmpeg");
        System.loadLibrary("now-streamer");
    }

    public native int startPublish(String input, String output);

}
