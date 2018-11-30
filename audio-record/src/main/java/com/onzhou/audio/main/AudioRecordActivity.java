package com.onzhou.audio.main;

import android.Manifest;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v4.app.ActivityCompat;
import android.view.View;
import android.widget.Button;

import com.onzhou.audio.record.AudioRecordRecorder;
import com.onzhou.audio.record.R;
import com.onzhou.ffmpeg.base.AbsBaseActivity;

import java.io.File;

/**
 * @anchor: andy
 * @date: 18-11-10
 */

public class AudioRecordActivity extends AbsBaseActivity {

    private static final int PERMISSION_CODE = 1000;

    private Button mBtnStart, mBtnStop;

    private AudioRecordRecorder audioRecorder;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_audio_record);
    }

    @Override
    protected void onPostCreate(@Nullable Bundle savedInstanceState) {
        super.onPostCreate(savedInstanceState);
        applyPermission();
    }

    private void applyPermission() {
        if (ActivityCompat.checkSelfPermission(this, Manifest.permission.RECORD_AUDIO) != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.RECORD_AUDIO}, PERMISSION_CODE);
        } else {
            setupView();
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (requestCode == PERMISSION_CODE && grantResults != null && grantResults.length > 0) {
            if (grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                setupView();
            }
        }
    }

    private void setupView() {
        mBtnStart = (Button) findViewById(R.id.btn_record_start);
        mBtnStop = (Button) findViewById(R.id.btn_record_stop);
        mBtnStart.setEnabled(true);
    }

    public void onRecordStart(View view) {
        if (audioRecorder == null) {
            audioRecorder = new AudioRecordRecorder();
        }
        File audioFile = new File(getExternalFilesDir(null), "output.pcm");
        audioRecorder.initRecorder(audioFile.getAbsolutePath(), AudioRecordRecorder.SAMPLE_RATE, AudioRecordRecorder.CHANNEL, AudioRecordRecorder.AUDIO_FORMAT);
        audioRecorder.recordStart();
        //更新按钮状态
        mBtnStart.setEnabled(false);
        mBtnStop.setEnabled(true);
    }

    public void onRecordStop(View view) {
        if (audioRecorder != null) {
            audioRecorder.recordStop();
            audioRecorder = null;
        }
        //更新按钮状态
        mBtnStart.setEnabled(true);
        mBtnStop.setEnabled(false);
    }

}
