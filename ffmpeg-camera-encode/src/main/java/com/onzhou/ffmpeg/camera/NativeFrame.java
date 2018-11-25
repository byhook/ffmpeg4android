package com.onzhou.ffmpeg.camera;

/**
 * @anchor: andy
 * @date: 18-11-3
 */

public class NativeFrame {

    static {
        System.loadLibrary("native-frame");
    }

    public native void onPreviewFrame(byte[] yuvData, int width, int height);

}
