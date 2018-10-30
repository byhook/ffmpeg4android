package com.onzhou.ffmpeg.hello;

/**
 * @anchor: andy
 * @date: 2018-10-29
 * @description:
 */
public class FFmpegHello {

    static {
        System.loadLibrary("avcodec");
        System.loadLibrary("avdevice");
        System.loadLibrary("avfilter");
        System.loadLibrary("avformat");
        System.loadLibrary("avutil");
        System.loadLibrary("postproc");
        System.loadLibrary("swresample");
        System.loadLibrary("swscale");

        System.loadLibrary("ffmpeg-hello");
    }

    public native String urlprotocolinfo();

    public native String avformatinfo();

    public native String avcodecinfo();

    public native String avfilterinfo();

    public native String configurationinfo();

}
