package com.onzhou.common.avcapture;

import android.view.SurfaceHolder;

/**
 * @anchor: andy
 * @date: 18-12-4
 */

public interface IVideoCapture {

    /**
     * 是否开启预览
     *
     * @param enable
     * @return
     */
    int enablePreview(boolean enable);

    /**
     * 设置相机类型
     *
     * @param cameraType
     */
    void setCameraType(int cameraType);

    /**
     * 设置预览大小
     *
     * @param width
     * @param height
     */
    void setPreviewSize(int width, int height);

    /**
     * 设置帧率
     *
     * @param fps
     */
    void setFrameRate(int fps);

    /**
     * 预览数据输出的格式
     *
     * @param pixelFormat
     */
    void setPreviewFormat(int pixelFormat);

    /**
     * 相机是否已打开
     *
     * @return
     */
    boolean isOpenCamera();

    /**
     * 关闭相机释放资源
     */
    void closeCamera();

    /**
     * @param surfaceHolder
     */
    void setSurfaceHolder(SurfaceHolder surfaceHolder);

    /**
     * 开始相机视频捕获
     *
     * @return
     */
    int startVideoCapture();

    /**
     * 停止相机视频捕获
     */
    void stopVideoCapture();

    /**
     * 是否已经开启相机视频捕获
     *
     * @return
     */
    boolean isOpenVideoCapture();

    int getVideoBuffer(byte[] byteBuffer, int length);

    /**
     * 相机类型
     */
    interface CameraDeviceType {

        /**
         * 后置相机
         */
        int CAMERA_FACING_BACK = 0;

        /**
         * 前置相机
         */
        int CAMERA_FACING_FRONT = 1;
    }

    interface ReturnCode {
        /**
         * 打开设备成功
         */
        int RET_SUCCESS = 0;

        /**
         * 没有找到相机设备
         */
        int RET_NO_CAMERA = -11;

        /**
         * 不支持此操作
         */
        int RET_NOT_SUPPORTED = -12;

        int RET_CAPTURE_NO_START = -21;

        int RET_ERROR_PARAM = -22;

        int RET_NO_VIDEO_DATA = -23;


        int RET_ERROR_OCCUPIED = -31;
        int RET_ERROR_TYPE = -32;
        int RET_ERROR_SIZE = -33;
        int RET_ERROR_BUFFER = -34;
        int RET_ERROR_UNKNOWN = -35;
    }


}
