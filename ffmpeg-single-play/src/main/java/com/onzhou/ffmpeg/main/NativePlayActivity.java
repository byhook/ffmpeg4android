package com.onzhou.ffmpeg.main;

import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v4.os.AsyncTaskCompat;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.TextView;

import com.onzhou.ffmpeg.base.AbsBaseActivity;
import com.onzhou.ffmpeg.player.NativePlayer;
import com.onzhou.ffmpeg.play.R;
import com.onzhou.ffmpeg.task.AssertReleaseTask;

import java.io.File;

import io.reactivex.schedulers.Schedulers;

/**
 * @anchor: andy
 * @date: 2018-10-31
 * @description:
 */
public class NativePlayActivity extends AbsBaseActivity implements AssertReleaseTask.ReleaseCallback {

    private NativePlayer fFmpegPlay;

    private TextView btnPlay;

    private SurfaceView surfaceView;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_ffmpeg_play);
    }

    @Override
    protected void onPostCreate(@Nullable Bundle savedInstanceState) {
        super.onPostCreate(savedInstanceState);
        setupVideo();
    }

    private void setupVideo() {
        btnPlay = (TextView) findViewById(R.id.video_play_btn_start);
        surfaceView = (SurfaceView) findViewById(R.id.video_play_surface_view);
        surfaceView.getHolder().addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(SurfaceHolder holder) {
                //创建成功之后释放视频文件
                releaseVideo();
            }

            @Override
            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {

            }

            @Override
            public void surfaceDestroyed(SurfaceHolder holder) {

            }
        });
    }

    private void releaseVideo() {
        AssertReleaseTask videoReleaseTask = new AssertReleaseTask(this, "input.mp4", this);
        AsyncTaskCompat.executeParallel(videoReleaseTask);
    }

    public void onPlayClick(View view) {
        if (fFmpegPlay == null) {
            fFmpegPlay = new NativePlayer();
        }
        btnPlay.setEnabled(false);
        final File videoFile = getExternalFilesDir(null);
        Schedulers.newThread().scheduleDirect(new Runnable() {
            @Override
            public void run() {
                fFmpegPlay.playVideo(videoFile.getAbsolutePath() + "/input.mp4", surfaceView.getHolder().getSurface());
            }
        });
    }

    @Override
    public void onReleaseSuccess(String filePath) {
        btnPlay.setEnabled(true);
    }
}
