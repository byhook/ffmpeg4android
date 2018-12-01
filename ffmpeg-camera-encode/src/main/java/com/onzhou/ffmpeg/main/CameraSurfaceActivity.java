package com.onzhou.ffmpeg.main;

import android.Manifest;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v4.app.ActivityCompat;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;

import com.onzhou.camera.encode.R;
import com.onzhou.ffmpeg.base.AbsBaseActivity;
import com.onzhou.ffmpeg.camera.CameraV1;
import com.onzhou.ffmpeg.core.AppCore;

import java.io.File;

/**
 * @anchor: andy
 * @date: 18-11-25
 */

public class CameraSurfaceActivity extends AbsBaseActivity {

    private static final int PERMISSION_CODE = 1000;

    private ViewGroup mRootLayer;

    private Button mBtnEncodeStartMP4, mBtnEncodeStopMP4;

    private SurfaceView mSurfaceView;

    private CameraV1 mCameraV1;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_native_camera);
        applyPermission();
    }

    private void applyPermission() {
        if (ActivityCompat.checkSelfPermission(this, Manifest.permission.CAMERA) != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.CAMERA}, PERMISSION_CODE);
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
        mRootLayer = (ViewGroup) findViewById(R.id.camera_root_layer);

        mBtnEncodeStartMP4 = (Button) findViewById(R.id.btn_encode_mp4_start);
        mBtnEncodeStopMP4 = (Button) findViewById(R.id.btn_encode_mp4_stop);

        mSurfaceView = new SurfaceView(this);
        mRootLayer.addView(mSurfaceView);
        mCameraV1 = new CameraV1();
        mCameraV1.setPreviewView(mSurfaceView);
    }

    public void onEncodeStart(View view) {
        mBtnEncodeStartMP4.setEnabled(false);
        mBtnEncodeStopMP4.setEnabled(true);

        //".h264"
        File outputFile = new File(getExternalFilesDir(null), System.currentTimeMillis() + ".mp4");
        mCameraV1.encodeStart(outputFile.getAbsolutePath());
    }

    public void onEncodeStop(View view) {
        mBtnEncodeStartMP4.setEnabled(true);
        mBtnEncodeStopMP4.setEnabled(false);
        mCameraV1.encodeStop();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (mCameraV1 != null) {
            mCameraV1.onDestroy();
            mCameraV1 = null;
        }
    }

    public void onEncodeJPEG(View view) {
        if (mCameraV1 != null) {
            File outputFile = new File(getExternalFilesDir(null), System.currentTimeMillis() + ".jpeg");
            mCameraV1.encodeJPEG(outputFile.getAbsolutePath());
        }
    }
}
