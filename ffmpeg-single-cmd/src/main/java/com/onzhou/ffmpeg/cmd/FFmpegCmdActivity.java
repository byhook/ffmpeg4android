package com.onzhou.ffmpeg.cmd;

import android.os.Bundle;
import android.support.annotation.Nullable;
import android.view.View;

import com.onzhou.ffmpeg.base.AbsBaseActivity;

import io.reactivex.schedulers.Schedulers;

/**
 * @anchor: andy
 * @date: 2018-10-30
 * @description:
 */
public class FFmpegCmdActivity extends AbsBaseActivity {

    private FFmpegCmd fFmpegCmd;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_ffmpeg_cmd);
    }

    public void onCmdClick(View view) {
        if (fFmpegCmd == null) {
            fFmpegCmd = new FFmpegCmd();
        }
        Schedulers.newThread().scheduleDirect(new Runnable() {
            @Override
            public void run() {
                String cmdline = "ffmpeg -i /sdcard/Android/data/com.onzhou.ffmpeg.cmd/files/input.mp4 /sdcard/Android/data/com.onzhou.ffmpeg.cmd/files/output.mkv";
                String[] argv = cmdline.split(" ");
                Integer argc = argv.length;
                fFmpegCmd.exec(argc, argv);
            }
        });
    }
}
