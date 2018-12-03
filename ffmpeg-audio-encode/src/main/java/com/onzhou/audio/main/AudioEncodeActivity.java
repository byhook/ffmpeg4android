package com.onzhou.audio.main;

import android.Manifest;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v4.app.ActivityCompat;
import android.view.View;
import android.widget.Button;

import com.onzhou.audio.encode.AudioRecordRecorder;
import com.onzhou.audio.encode.IAudioRecorder;
import com.onzhou.audio.encode.MediaRecordRecorder;
import com.onzhou.audio.encode.NativeAudioEncoder;
import com.onzhou.audio.encode.R;
import com.onzhou.ffmpeg.base.AbsBaseActivity;

import java.io.File;

/**
 * @anchor: andy
 * @date: 18-12-2
 */

public class AudioEncodeActivity extends AbsBaseActivity {


    private static final int PERMISSION_CODE = 1000;

    private Button mBtnStart, mBtnStop;

    private IAudioRecorder audioRecorder;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_audio_encode);
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

    /**
     * 点击开始录制
     * @param view
     */
    public void onRecordStart(View view) {
        File targetDir = getExternalFilesDir(null);
        if (audioRecorder == null) {
            //audioRecorder = new MediaRecordRecorder(targetDir.getAbsolutePath()+ File.separator+"output.aac");
            //audioRecorder = new AudioRecordRecorder(targetDir.getAbsolutePath()+ File.separator+"output.pcm");
            audioRecorder = new NativeAudioEncoder(targetDir.getAbsolutePath()+ File.separator+"output.aac");
        }
        audioRecorder.initRecorder();
        audioRecorder.recordStart();
        //更新按钮状态
        mBtnStart.setEnabled(false);
        mBtnStop.setEnabled(true);
    }

    /**
     * 点击停止录制
     * @param view
     */
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
