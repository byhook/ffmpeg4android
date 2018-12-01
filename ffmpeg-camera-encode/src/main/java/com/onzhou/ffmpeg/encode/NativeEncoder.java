package com.onzhou.ffmpeg.encode;

/**
 * @anchor: andy
 * @date: 18-11-3
 */

public class NativeEncoder {

    static {
        System.loadLibrary("native-encode");
    }

    public native void encodeMP4Start(String mp4Path, int width, int height);

    public native void encodeMP4Stop();

    public native void onPreviewFrame(byte[] yuvData, int width, int height);

    public native void encodeJPEG(String jpegPath, int width, int height);

}
