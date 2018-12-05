package com.onzhou.common.main;

import android.Manifest;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v4.app.ActivityCompat;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import com.onzhou.common.avcapture.IVideoCapture;
import com.onzhou.common.avcapture.R;
import com.onzhou.common.avcapture.impl.VideoCapture;
import com.onzhou.ffmpeg.base.AbsBaseActivity;

/**
 * @anchor: andy
 * @date: 18-12-4
 */

public class CaptureActivity extends AbsBaseActivity implements SurfaceHolder.Callback {

    private static final int PERMISSION_CODE = 100;

    private SurfaceView mSurfaceView;

    private IVideoCapture mVideoCapture = new VideoCapture();

    private int mCameraType = IVideoCapture.CameraDeviceType.CAMERA_FACING_FRONT;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_capture_layer);
    }

    @Override
    protected void onPostCreate(@Nullable Bundle savedInstanceState) {
        super.onPostCreate(savedInstanceState);
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
        mSurfaceView = (SurfaceView) findViewById(R.id.capture_surface_view);
        mSurfaceView.getHolder().addCallback(this);
    }

    public boolean openCamera() {
        mVideoCapture.closeCamera();
        mVideoCapture.setFrameRate(25);
        mVideoCapture.setPreviewSize(1280, 640);
        mVideoCapture.setCameraType(IVideoCapture.CameraDeviceType.CAMERA_FACING_FRONT);
        mVideoCapture.setSurfaceHolder(mSurfaceView.getHolder());
        if (mVideoCapture.enablePreview(true) >= 0) {
            //开始捕获视频数据
            mVideoCapture.startVideoCapture();
            return true;
        }
        return false;
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        openCamera();
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {

    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {

    }
}
