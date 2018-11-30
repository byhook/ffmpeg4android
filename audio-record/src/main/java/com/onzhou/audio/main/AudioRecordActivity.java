package com.onzhou.audio.main;

import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v4.os.AsyncTaskCompat;
import android.view.View;
import android.widget.Button;

import com.onzhou.audio.record.R;
import com.onzhou.ffmpeg.base.AbsBaseActivity;

import io.reactivex.schedulers.Schedulers;

/**
 * @anchor: andy
 * @date: 18-11-10
 */

public class AudioRecordActivity extends AbsBaseActivity {

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_audio_record);
    }

    public void onRecordStart(View view) {
        Schedulers.newThread().scheduleDirect(new Runnable() {
            @Override
            public void run() {

            }
        });
    }

    public void onRecordStop(View view) {

    }

}
