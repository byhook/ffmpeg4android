package cn.byhook.ffmpegwithx264;

/**
 * 作者: andy
 * 时间: 2016-07-21
 * 描述:
 * 修订:
 */
public class FFmpegPlayer {

    static {
        System.loadLibrary("FFmpegPlayer");
        System.loadLibrary("ffmpeg");
    }

    public native String getMessage();
}
