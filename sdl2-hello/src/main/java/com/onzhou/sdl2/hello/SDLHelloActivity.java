package com.onzhou.sdl2.hello;

import android.Manifest;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v4.app.ActivityCompat;
import android.support.v4.os.AsyncTaskCompat;
import android.view.View;
import android.widget.TextView;

import com.onzhou.ffmpeg.base.AbsBaseActivity;
import com.onzhou.ffmpeg.task.AssertReleaseTask;

import org.libsdl.app.SDLActivity;

/**
 * @anchor: andy
 * @date: 2018-10-30
 * @description:
 */
public class SDLHelloActivity extends AbsBaseActivity implements AssertReleaseTask.ReleaseCallback {

    private TextView mBtnSDLShow;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_sdl_hello);
    }

    @Override
    protected void onPostCreate(@Nullable Bundle savedInstanceState) {
        super.onPostCreate(savedInstanceState);
        mBtnSDLShow = (TextView) findViewById(R.id.btn_sdl_hello);
        AssertReleaseTask videoReleaseTask = new AssertReleaseTask(this, "input.bmp", this);
        AsyncTaskCompat.executeParallel(videoReleaseTask);
    }

    public void onSDLClick(View view) {
        Intent intent = new Intent(this, SDLActivity.class);
        startActivity(intent);
    }

    @Override
    public void onReleaseSuccess(String filePath) {
        mBtnSDLShow.setEnabled(true);
    }
}
