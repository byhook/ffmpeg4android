package com.onzhou.mediacodec.main;

import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v4.os.AsyncTaskCompat;
import android.view.View;

import com.onzhou.ffmpeg.base.AbsBaseActivity;
import com.onzhou.ffmpeg.task.AssertReleaseTask;
import com.onzhou.mediacodec.audio.AudioHWEncoder;
import com.onzhou.mediacodec.audio.OnEncodeListener;
import com.onzhou.mediacodec.audio.R;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

/**
 * @anchor: andy
 * @date: 18-12-9
 */

public class MediaCodecActivity extends AbsBaseActivity implements OnEncodeListener, AssertReleaseTask.ReleaseCallback {


    private FileOutputStream outputStream = null;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_mediacodec_audio);
    }

    @Override
    protected void onPostCreate(@Nullable Bundle savedInstanceState) {
        super.onPostCreate(savedInstanceState);
        AssertReleaseTask videoReleaseTask = new AssertReleaseTask(this, "input.pcm", this);
        AsyncTaskCompat.executeParallel(videoReleaseTask);
    }

    @Override
    public void onReleaseSuccess(String filePath) {
        findViewById(R.id.btn_audio_encode).setEnabled(true);
    }

    public void onMediaEncodeClick(View view) {
        view.setEnabled(false);
        encode();
    }

    private void encode() {
        AudioHWEncoder audioEncoder = null;
        FileInputStream inputStream = null;
        try {
            audioEncoder = new AudioHWEncoder(this, 44100, 2, 128 * 1024);
            File pcmFile = new File(getExternalFilesDir(null), "input.pcm");
            File aacFile = new File(getExternalFilesDir(null), "output.aac");
            inputStream = new FileInputStream(pcmFile);
            outputStream = new FileOutputStream(aacFile);
            int bufferSize = 1024 * 256;
            byte[] buffer = new byte[bufferSize];
            int encodeBufferSize = 1024 * 10;
            byte[] encodeBuffer = new byte[encodeBufferSize];
            int len = -1;
            while ((len = inputStream.read(buffer)) > 0) {
                int offset = 0;
                while (offset < len) {
                    int encodeBufferLength = Math.min(len - offset, encodeBufferSize);
                    System.arraycopy(buffer, offset, encodeBuffer, 0, encodeBufferLength);
                    audioEncoder.fireAudio(encodeBuffer, encodeBufferLength);
                    offset += encodeBufferLength;
                }
            }
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            if (null != inputStream) {
                try {
                    inputStream.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
            if (null != outputStream) {
                try {
                    outputStream.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
        audioEncoder.stop();
    }

    @Override
    public void onEncodeAACBuffer(byte[] data) {
        try {
            outputStream.write(data);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }


}
