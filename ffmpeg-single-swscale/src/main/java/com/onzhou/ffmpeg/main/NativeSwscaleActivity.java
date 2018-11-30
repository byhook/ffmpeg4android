package com.onzhou.ffmpeg.main;

import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v4.os.AsyncTaskCompat;
import android.view.View;
import android.widget.Button;

import com.onzhou.ffmpeg.base.AbsBaseActivity;
import com.onzhou.ffmpeg.swscale.NativeSwscale;
import com.onzhou.ffmpeg.swscale.R;
import com.onzhou.ffmpeg.task.AssertReleaseTask;

import java.io.File;

import io.reactivex.schedulers.Schedulers;

/**
 * @anchor: andy
 * @date: 2018-10-31
 * @description:
 */
public class NativeSwscaleActivity extends AbsBaseActivity implements AssertReleaseTask.ReleaseCallback {

    private Button mBtnTransform;

    private NativeSwscale mNativeSwscale;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_native_swscale);
    }

    @Override
    protected void onPostCreate(@Nullable Bundle savedInstanceState) {
        super.onPostCreate(savedInstanceState);
        mBtnTransform = (Button) findViewById(R.id.btn_transform);
        setupVideo();
    }

    public void setupVideo() {
        AssertReleaseTask videoReleaseTask = new AssertReleaseTask(this, "input.mp4", this);
        AsyncTaskCompat.executeParallel(videoReleaseTask);
    }

    @Override
    public void onReleaseSuccess(String filePath) {
        mNativeSwscale = new NativeSwscale();
        mBtnTransform.setEnabled(true);
    }

    public void onSWScaleClick(View view) {
        mBtnTransform.setEnabled(false);
        Schedulers.newThread().scheduleDirect(new Runnable() {
            @Override
            public void run() {
                File mp4File = new File(getExternalFilesDir(null), "input.mp4");
                File yuvFile = new File(getExternalFilesDir(null), "output.yuv");
                mNativeSwscale.mp4ToYuv(mp4File.getAbsolutePath(), yuvFile.getAbsolutePath());
            }
        });
    }

}
