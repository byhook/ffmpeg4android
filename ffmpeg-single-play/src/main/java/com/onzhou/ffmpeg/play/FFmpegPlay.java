package com.onzhou.ffmpeg.play;

import android.view.Surface;

/**
 * @anchor: andy
 * @date: 2018-10-31
 * @description:
 */
public class FFmpegPlay {

    static {
        System.loadLibrary("ffmpeg");
        System.loadLibrary("ffmpeg-play");
    }

    public native int playVideo(String videoPath, Object surface);

}
