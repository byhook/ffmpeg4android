package com.onzhou.lame.encode;

/**
 * @anchor: andy
 * @date: 18-11-10
 */

public class LameEncoder {

    static {
        System.loadLibrary("lame-encode");
    }

    public native void encode(String pcmPath, String mp3Path, int sampleRate, int channels, int bitRate);

    public native void destroy();

}
