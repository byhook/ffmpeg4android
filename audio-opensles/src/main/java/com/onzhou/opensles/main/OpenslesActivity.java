package com.onzhou.opensles.main;

import android.Manifest;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v4.app.ActivityCompat;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;

import com.onzhou.opensles.R;
import com.onzhou.opensles.play.AudioTrackPlayer;
import com.onzhou.opensles.play.IAudioPlay;
import com.onzhou.opensles.recorder.AudioRecorder;

import java.io.File;

/**
 * @anchor: andy
 * @date: 2019-01-30
 * @description:
 */
public class OpenslesActivity extends AppCompatActivity {

    private static final int PERMISSION_CODE = 1000;

    private CheckBox mChkAudioRecord, mChkAudioPlay;

    private AudioRecorder audioRecorder = new AudioRecorder();

    private IAudioPlay mAudioPlayer;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_opensles_record);
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
        mChkAudioRecord = (CheckBox) findViewById(R.id.chk_audio_record);
        mChkAudioPlay = (CheckBox) findViewById(R.id.chk_audio_play);
        mChkAudioPlay.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if (isChecked) {
                    buttonView.setText("停止播放");
                    startPlay();
                    mChkAudioRecord.setEnabled(false);
                } else {
                    buttonView.setText("开始播放");
                    stopPlay();
                    mChkAudioRecord.setEnabled(true);
                }
            }
        });


        mChkAudioRecord.setEnabled(true);
        mChkAudioRecord.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if (isChecked) {
                    buttonView.setText("停止录制");
                    startRecord();
                    //禁止播放
                    mChkAudioPlay.setEnabled(false);
                } else {
                    buttonView.setText("开始录制");
                    stopRecord();
                    //可以播放
                    mChkAudioPlay.setEnabled(true);
                }
            }
        });

    }

    public void startRecord() {
        File outFile = new File(getExternalFilesDir(null), "output.pcm");
        audioRecorder.startRecord(44100, 2, 16, outFile.getAbsolutePath());
    }

    public void stopRecord() {
        audioRecorder.stopRecord();
    }

    public void startPlay() {
        if (mAudioPlayer == null) {
            mAudioPlayer = new AudioTrackPlayer();
            File outFile = new File(getExternalFilesDir(null), "output.pcm");
            mAudioPlayer.initPlayer(outFile.getAbsolutePath(), 44100, 2, 16);
        }
        mAudioPlayer.startPlay();
    }

    public void stopPlay() {
        if (mAudioPlayer != null) {
            mAudioPlayer.stopPlay();
        }
    }
}
