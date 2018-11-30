package com.onzhou.ffmpeg.player;

/**
 * @anchor: andy
 * @date: 2018-10-31
 * @description:
 */
public class NativePlayer {

    static {
        System.loadLibrary("native-player");
    }

    public native int playVideo(String videoPath, Object surface);

}
