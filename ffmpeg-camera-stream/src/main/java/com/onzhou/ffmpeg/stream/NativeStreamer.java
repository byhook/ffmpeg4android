package com.onzhou.ffmpeg.stream;

/**
 * @anchor: andy
 * @date: 18-11-3
 */

public class NativeStreamer {

    static {
        System.loadLibrary("native-stream");
    }

    public native void startPublish(String stream, int width, int height);

    public native void stopPublish();

    public native void onPreviewFrame(byte[] yuvData, int width, int height);

}
