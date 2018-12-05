package com.onzhou.common.avcapture.impl;

import android.graphics.ImageFormat;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.opengl.GLES11Ext;
import android.view.SurfaceHolder;

import com.onzhou.common.avcapture.IVideoCapture;

import java.io.IOException;
import java.util.Comparator;
import java.util.LinkedList;
import java.util.List;
import java.util.Queue;

/**
 * @anchor: andy
 * @date: 18-12-4
 */

public class VideoCapture implements IVideoCapture, Comparator<Camera.Size>, Camera.PreviewCallback {

    private static final String TAG = "VideoCapture";

    /**
     * 支持的相机数量
     */
    private int mCameraNum = 0;

    /**
     * 缓冲区长度
     */
    private int mBufferLength;

    /**
     * 时间间隔
     * 下面几个参数主要用来控制帧率
     */
    private int mDuration;
    private long mSumTime;
    private long mPrevTime;
    private long mCurrentTime;

    /**
     * 同步锁
     */
    private Object lock = new Object();

    /**
     * 是否已开启相机
     */
    private volatile boolean mOpenCamera = false;

    /**
     * 是否已开启视频捕获
     */
    private volatile boolean mStartVideoCapture = false;

    /**
     * 视频数据队列
     */
    private Queue<byte[]> mVideoQueue = new LinkedList();

    /**
     * 预览数据默认输出NV21格式
     */
    private byte[] nv21Buffer = null;

    /**
     * 当前相机实例
     */
    private Camera mCamera = null;

    /**
     * 相机类型
     * 默认前置相机
     */
    private int mCameraType = CameraDeviceType.CAMERA_FACING_FRONT;

    /**
     * 预览的宽高
     */
    private int mPreviewWidth, mPreviewHeight;

    /**
     * 帧率
     */
    private int mFrameRate = 15;

    /**
     * 预览的数据输出格式
     * 默认NV21格式
     */
    private int mPreviewFormat = ImageFormat.NV21;

    private SurfaceHolder mSurfaceHolder = null;

    private Camera openCamera(int cameraType) {
        Camera.CameraInfo cameraInfo = new Camera.CameraInfo();
        mCameraNum = Camera.getNumberOfCameras();
        for (int i = 0; i < mCameraNum; i++) {
            //获取相机信息
            Camera.getCameraInfo(i, cameraInfo);
            if (cameraInfo.facing == cameraType) {
                try {
                    return Camera.open(i);
                } catch (Exception e) {
                    e.printStackTrace();
                    mCamera = null;
                }
            }
        }
        return null;
    }

    @Override
    public void setCameraType(int cameraType) {
        mCameraType = cameraType;
    }

    @Override
    public int enablePreview(boolean enable) {
        if (enable) {
            if (mOpenCamera) {
                //已经开启相机了
                return ReturnCode.RET_ERROR_OCCUPIED;
            }
            mCamera = openCamera(mCameraType);
            if (mCamera == null) {
                return ReturnCode.RET_ERROR_UNKNOWN;
            }
            mOpenCamera = true;
            //设置相机默认旋转角度
            setCameraDisplayOrientation(mCameraType, mCamera);
            if (mSurfaceHolder != null) {
                try {
                    mCamera.setPreviewDisplay(mSurfaceHolder);
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
            //设置相机参数
            int result = setCameraParameter(mCamera);
            if (result >= 0) {
                mCamera.startPreview();
                return ReturnCode.RET_SUCCESS;
            } else {
                closeCamera();
                return result;
            }
        } else {
            closeCamera();
            return ReturnCode.RET_SUCCESS;
        }
    }

    @Override
    public void setPreviewSize(int width, int height) {
        mPreviewWidth = width;
        mPreviewHeight = height;
    }

    @Override
    public void setFrameRate(int fps) {
        if (fps > 0) {
            mFrameRate = fps;
        }
    }

    @Override
    public void setPreviewFormat(int pixelFormat) {
        mPreviewFormat = pixelFormat;
    }

    /**
     * 设置相机相关的参数
     *
     * @param camera
     * @return
     */
    private int setCameraParameter(Camera camera) {
        Camera.Parameters params = camera.getParameters();
        List supportedPreviewSizes = params.getSupportedPreviewSizes();
        for (int index = 0; index < supportedPreviewSizes.size(); index++) {
            Camera.Size size = (Camera.Size) supportedPreviewSizes.get(index);
            if ((size.width == mPreviewWidth) && (size.height == mPreviewHeight)) {
                params.setPreviewFormat(mPreviewFormat);
                int format = params.getPreviewFormat();
                //计算缓冲区大小
                mBufferLength = (mPreviewWidth * mPreviewHeight * ImageFormat.getBitsPerPixel(format) / 8);

                params.setPreviewSize(mPreviewWidth, mPreviewHeight);

                mSumTime = 0L;
                mDuration = (1000 / mFrameRate);

                camera.setParameters(params);
                nv21Buffer = new byte[mBufferLength];
                return ReturnCode.RET_SUCCESS;
            }
        }
        return ReturnCode.RET_ERROR_SIZE;
    }

    @Override
    public boolean isOpenCamera() {
        return mOpenCamera;
    }

    @Override
    public void closeCamera() {
        if (mOpenCamera) {
            //停止采集
            stopVideoCapture();
            //释放相机
            mCamera.release();
            nv21Buffer = null;
            mCamera = null;
            mSurfaceHolder = null;
            mOpenCamera = false;
        }
    }

    @Override
    public void setSurfaceHolder(SurfaceHolder surfaceHolder) {
        mSurfaceHolder = surfaceHolder;
    }

    @Override
    public int startVideoCapture() {
        mVideoQueue.clear();
        if (!mOpenCamera) {
            return ReturnCode.RET_CAPTURE_NO_START;
        }
        if ((lock == null) || (mVideoQueue == null)) {
            //已经释放关闭了
            return ReturnCode.RET_ERROR_BUFFER;
        }
        if (mStartVideoCapture) {
            //已经开始了
            return ReturnCode.RET_SUCCESS;
        }
        mCamera.addCallbackBuffer(nv21Buffer);
        mCamera.setPreviewCallbackWithBuffer(this);
        mStartVideoCapture = true;
        mPrevTime = System.currentTimeMillis();
        return ReturnCode.RET_SUCCESS;
    }

    @Override
    public boolean isOpenVideoCapture() {
        return mStartVideoCapture;
    }

    @Override
    public void stopVideoCapture() {
        if (mStartVideoCapture) {
            mCamera.setPreviewCallback(null);
            mCamera.stopPreview();
            mStartVideoCapture = false;
        }
    }

    @Override
    public int getVideoBuffer(byte[] byteBuffer, int length) {
        if (!mStartVideoCapture)
            return ReturnCode.RET_CAPTURE_NO_START;
        if ((byteBuffer == null) || (length < mBufferLength)) {
            return ReturnCode.RET_ERROR_PARAM;
        }
        synchronized (lock) {
            byte[] frameBuffer = mVideoQueue.poll();
            if (frameBuffer == null) {
                return ReturnCode.RET_NO_VIDEO_DATA;
            }
            System.arraycopy(frameBuffer, 0, byteBuffer, 0, mBufferLength);
        }
        return ReturnCode.RET_SUCCESS;
    }

    @Override
    public int compare(Camera.Size lhs, Camera.Size rhs) {
        if (lhs.width == rhs.width) {
            if (lhs.height < rhs.height)
                return -1;
            if (lhs.height == rhs.height) {
                return 0;
            }
            return 1;
        }
        if (lhs.width < rhs.width) {
            return -1;
        }
        return 1;
    }

    /**
     * 控制帧率
     *
     * @param data
     * @param camera
     */
    @Override
    public void onPreviewFrame(byte[] data, Camera camera) {
        synchronized (lock) {
            if (mVideoQueue.size() == 2) {
                mVideoQueue.poll();
            }
            mCurrentTime = System.currentTimeMillis();
            mSumTime = (mSumTime + mCurrentTime - mPrevTime);
            if (mSumTime > mDuration) {
                mVideoQueue.offer(data);
                mSumTime %= mDuration;
            }
            mPrevTime = mCurrentTime;
        }
        //通过内存复用来提高预览的效率
        mCamera.addCallbackBuffer(nv21Buffer);
    }

    /**
     * 前置摄像头要转换270度
     * 后置摄像头转换90度
     *
     * @param cameraId
     * @param camera
     */
    private void setCameraDisplayOrientation(int cameraId, Camera camera) {
        Camera.CameraInfo info = new Camera.CameraInfo();
        Camera.getCameraInfo(cameraId, info);
        int degrees = 0;
        int displayDegree;
        if (info.facing == Camera.CameraInfo.CAMERA_FACING_FRONT) {
            displayDegree = (info.orientation + degrees) % 360;
            displayDegree = (360 - displayDegree) % 360;
        } else {
            displayDegree = (info.orientation - degrees + 360) % 360;
        }
        camera.setDisplayOrientation(displayDegree);
    }
}
