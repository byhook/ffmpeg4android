package com.onzhou.ffmpeg.cmd;

import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v4.os.AsyncTaskCompat;
import android.view.View;
import android.widget.TextView;

import com.onzhou.ffmpeg.base.AbsBaseActivity;
import com.onzhou.ffmpeg.task.AssertReleaseTask;

import java.io.File;

import io.reactivex.schedulers.Schedulers;

/**
 * @anchor: andy
 * @date: 2018-10-30
 * @description:
 */
public class FFmpegCmdActivity extends AbsBaseActivity implements AssertReleaseTask.ReleaseCallback {

    private TextView btnTransfer;

    private FFmpegCmd fFmpegCmd;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_ffmpeg_cmd);
        btnTransfer = (TextView) findViewById(R.id.btn_transfer);
    }

    @Override
    protected void onPostCreate(@Nullable Bundle savedInstanceState) {
        super.onPostCreate(savedInstanceState);
        AssertReleaseTask videoReleaseTask = new AssertReleaseTask(this, "input.mp4", this);
        AsyncTaskCompat.executeParallel(videoReleaseTask);
    }


    public void onCmdClick(View view) {
        if (fFmpegCmd == null) {
            fFmpegCmd = new FFmpegCmd();
        }
        btnTransfer.setEnabled(false);
        File outputFile = new File(getExternalFilesDir(null), "output.mkv");
        if (outputFile.exists()) {
            outputFile.delete();
        }
        Schedulers.newThread().scheduleDirect(new Runnable() {
            @Override
            public void run() {
                String filePath = getExternalFilesDir(null).getAbsolutePath();
                String cmdline = String.format("ffmpeg -i %s/input.mp4 %s/output.mkv", filePath, filePath);
                String[] argv = cmdline.split(" ");
                Integer argc = argv.length;
                fFmpegCmd.exec(argc, argv);
            }
        });
    }

    @Override
    public void onReleaseSuccess(String filePath) {
        btnTransfer.setEnabled(true);
    }
}
