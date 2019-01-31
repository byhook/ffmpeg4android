package com.onzhou.opensles.play;

import java.io.DataInputStream;
import java.io.File;
import java.io.FileInputStream;

/**
 * @anchor: andy
 * @date: 2019-01-31
 * @description:
 */
public interface IAudioPlay {

    void initPlayer(String path, int sampleRate, int channels, int bitRate);

    /**
     * 启动播放
     */
    void startPlay();

    /**
     * 停止播放
     */
    void stopPlay();

}
