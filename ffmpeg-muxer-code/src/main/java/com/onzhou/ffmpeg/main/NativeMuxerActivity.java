package com.onzhou.ffmpeg.main;

import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v4.os.AsyncTaskCompat;
import android.view.View;
import android.widget.Button;

import com.onzhou.ffmpeg.base.AbsBaseActivity;
import com.onzhou.ffmpeg.muxer.NativeMuxer;
import com.onzhou.ffmpeg.muxer.R;
import com.onzhou.ffmpeg.task.AssertReleaseTask;

import java.io.File;

import io.reactivex.Scheduler;
import io.reactivex.schedulers.Schedulers;

/**
 * @anchor: andy
 * @date: 18-11-25
 */

public class NativeMuxerActivity extends AbsBaseActivity implements AssertReleaseTask.ReleaseCallback {

    private NativeMuxer nativeMuxer;

    private Button mBtnMix;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_muxer_layer);
    }

    @Override
    protected void onPostCreate(@Nullable Bundle savedInstanceState) {
        super.onPostCreate(savedInstanceState);
        this.mBtnMix = (Button) findViewById(R.id.btn_mix_video_audio);
        AssertReleaseTask videoReleaseTask = new AssertReleaseTask(this, new String[]{"input.h264", "input.aac"}, this);
        AsyncTaskCompat.executeParallel(videoReleaseTask);
    }

    public void onMixClick(View view) {
        mBtnMix.setEnabled(false);
        if (nativeMuxer == null) {
            nativeMuxer = new NativeMuxer();
        }
        Schedulers.newThread().scheduleDirect(new Runnable() {
            @Override
            public void run() {
                File h264File = new File(getExternalFilesDir(null), "input.h264");
                File mp3File = new File(getExternalFilesDir(null), "input.aac");
                File outputFile = new File(getExternalFilesDir(null), "output.mp4");
                nativeMuxer.muxerVideoAudio(h264File.getAbsolutePath(), mp3File.getAbsolutePath(), outputFile.getAbsolutePath());
            }
        });
    }

    @Override
    public void onReleaseSuccess(String filePath) {
        mBtnMix.setEnabled(true);
    }
}

