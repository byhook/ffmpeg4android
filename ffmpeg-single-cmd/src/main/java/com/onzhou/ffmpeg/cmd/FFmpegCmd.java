package com.onzhou.ffmpeg.cmd;

/**
 * @anchor: andy
 * @date: 2018-10-30
 * @description:
 */
public class FFmpegCmd {

    static {
        System.loadLibrary("ffmpeg");
        System.loadLibrary("ffmpeg-cmd");
    }

    public native int exec(int argc, String[] argv);

}
