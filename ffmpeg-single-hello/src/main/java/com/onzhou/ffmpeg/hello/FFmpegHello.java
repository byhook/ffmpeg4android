package com.onzhou.ffmpeg.hello;

/**
 * @anchor: andy
 * @date: 2018-10-29
 * @description: 描述
 * Protocol: FFmpeg类库支持的协议
 * AVFormat: FFmpeg类库支持的封装格式
 * AVCodec: FFmpeg类库支持的编解码器
 * AVFilter: FFmpeg类库支持的滤镜
 * Configure: FFmpeg类库的配置信息
 */
public class FFmpegHello {

    static {
        System.loadLibrary("ffmpeg");
        System.loadLibrary("ffmpeg-hello");
    }

    public native String urlprotocolinfo();

    public native String avformatinfo();

    public native String avcodecinfo();

    public native String avfilterinfo();

    public native String configurationinfo();

}
