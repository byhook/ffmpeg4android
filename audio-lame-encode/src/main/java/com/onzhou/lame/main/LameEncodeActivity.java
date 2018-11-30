package com.onzhou.lame.main;

import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v4.os.AsyncTaskCompat;
import android.view.View;
import android.widget.Button;

import com.onzhou.lame.encode.LameEncoder;
import com.onzhou.audio.lame.R;
import com.onzhou.ffmpeg.base.AbsBaseActivity;
import com.onzhou.ffmpeg.task.AssertReleaseTask;

import java.io.File;

import io.reactivex.schedulers.Schedulers;

/**
 * @anchor: andy
 * @date: 18-11-10
 */

public class LameEncodeActivity extends AbsBaseActivity implements AssertReleaseTask.ReleaseCallback {

    private Button mBtnEncode;

    private LameEncoder mLameEncoder;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_lame_encode);

        mBtnEncode = (Button) findViewById(R.id.btn_encode);
    }

    public void onEncodeClick(View view) {
        mBtnEncode.setEnabled(false);
        Schedulers.newThread().scheduleDirect(new Runnable() {
            @Override
            public void run() {
                File pcmFile = new File(getExternalFilesDir(null), "input.pcm");
                File mp3File = new File(getExternalFilesDir(null), "output.mp3");
                mLameEncoder = new LameEncoder();
                mLameEncoder.encode(pcmFile.getAbsolutePath(), mp3File.getAbsolutePath(), 44100, 2, 128);
            }
        });
    }

    @Override
    protected void onPostCreate(@Nullable Bundle savedInstanceState) {
        super.onPostCreate(savedInstanceState);
        AssertReleaseTask videoReleaseTask = new AssertReleaseTask(this, "input.pcm", this);
        AsyncTaskCompat.executeParallel(videoReleaseTask);
    }

    @Override
    public void onReleaseSuccess(String filePath) {
        mBtnEncode.setEnabled(true);
    }
}
