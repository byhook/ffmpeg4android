package com.onzhou.ffmpeg.camera;

import android.graphics.SurfaceTexture;
import android.view.SurfaceView;

import com.onzhou.ffmpeg.encode.NativeEncoder;

/**
 * @anchor: andy
 * @date: 2018-11-12
 * @description:
 */
public interface ICamera {

    void setPreviewView(SurfaceView surfaceView);

    void onDestroy();

    void encodeStart(String outputPath);

    void encodeStop();

    void encodeJPEG(String jpegPath);

}
