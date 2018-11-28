package com.onzhou.ffmpeg.streamer;

/**
 * @anchor: andy
 * @date: 2018-10-29
 * @description:
 */
public class NativeStreamer {

    static {
        System.loadLibrary("native-stream");
    }

    public native int startPublish(String mp4Path, String stream);

    public native void stopPublish();

}
