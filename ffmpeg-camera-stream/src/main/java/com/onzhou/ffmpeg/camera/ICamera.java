package com.onzhou.ffmpeg.camera;

import android.view.SurfaceView;

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

}
