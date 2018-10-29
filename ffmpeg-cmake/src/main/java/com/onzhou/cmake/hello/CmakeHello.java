package com.onzhou.cmake.hello;

/**
 * @anchor: andy
 * @date: 2018-10-29
 * @description:
 */
public class CmakeHello {

    static {
        System.loadLibrary("avcodec");
        System.loadLibrary("avdevice");
        System.loadLibrary("avfilter");
        System.loadLibrary("avformat");
        System.loadLibrary("avutil");
        System.loadLibrary("postproc");
        System.loadLibrary("swresample");
        System.loadLibrary("swscale");

        System.loadLibrary("ffmpeg-cmake");
    }

    public native String urlprotocolinfo();

    public native String avformatinfo();

    public native String avcodecinfo();

    public native String avfilterinfo();

    public native String configurationinfo();

}
