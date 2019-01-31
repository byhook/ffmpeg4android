package com.onzhou.opensles.play;

import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioRecord;
import android.media.AudioTrack;
import android.util.Log;

import java.io.DataInputStream;
import java.io.FileInputStream;

public class AudioTrackPlayer implements IAudioPlay {

    public static final String TAG = "AudioTrackPlayer";

    private AudioTrack audioTrack;

    private DataInputStream mAudioStream;

    private Thread recordThread;

    private int bufferSize;

    /**
     * pcm路径
     */
    private String mPcmPath;

    @Override
    public void initPlayer(String path, int sampleRate, int channels, int bitRate) {
        this.mPcmPath = path;
        bufferSize = AudioTrack.getMinBufferSize(sampleRate, AudioFormat.CHANNEL_OUT_STEREO, AudioFormat.ENCODING_PCM_16BIT);
    }

    /**
     * 销毁线程方法
     */
    private void destroyThread() {
        try {
            if (null != recordThread && Thread.State.RUNNABLE == recordThread.getState()) {
                try {
                    Thread.sleep(500);
                    recordThread.interrupt();
                } catch (Exception e) {
                    recordThread = null;
                }
            }
            recordThread = null;
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            recordThread = null;
        }
    }

    /**
     * 启动播放线程
     */
    private void startThread() {
        destroyThread();
        if (audioTrack == null) {
            audioTrack = new AudioTrack(AudioManager.STREAM_MUSIC, 44100, AudioFormat.CHANNEL_OUT_STEREO, AudioFormat.ENCODING_PCM_16BIT, bufferSize * 2, AudioTrack.MODE_STREAM);
        }
        if (recordThread == null) {
            recordThread = new Thread(recordRunnable);
            recordThread.start();
        }
    }

    /**
     * 播放线程
     */
    Runnable recordRunnable = new Runnable() {
        @Override
        public void run() {
            try {
                android.os.Process.setThreadPriority(android.os.Process.THREAD_PRIORITY_URGENT_AUDIO);
                byte[] tempBuffer = new byte[bufferSize];
                int readCount = 0;
                audioTrack.play();
                mAudioStream = new DataInputStream(new FileInputStream(mPcmPath));
                while (mAudioStream.available() > 0) {
                    readCount = mAudioStream.read(tempBuffer);
                    if (readCount == AudioTrack.ERROR_INVALID_OPERATION || readCount == AudioTrack.ERROR_BAD_VALUE) {
                        continue;
                    }
                    if (readCount != 0 && readCount != -1) {
                        audioTrack.write(tempBuffer, 0, readCount);
                    }
                    Log.d(TAG, "Play: " + readCount);
                }
                stopPlay();
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    };

    /**
     * 启动播放
     */
    @Override
    public void startPlay() {
        try {
            startThread();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /**
     * 停止播放
     */
    public void stopPlay() {
        try {
            destroyThread();
            if (audioTrack != null) {
                if (audioTrack.getState() == AudioRecord.STATE_INITIALIZED) {
                    audioTrack.stop();
                }
                audioTrack.release();
                audioTrack = null;
            }
            if (mAudioStream != null) {
                mAudioStream.close();
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

}