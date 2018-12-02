
## 概述
`libyuv`是Google开源的实现各种`YUV与RGB`之间相互`转换、旋转、缩放`的库。它是跨平台的，可在`Windows、Linux、Mac、Android`等操作系统，`x86、x64、arm`架构上进行编译运行，支持`SSE、AVX、NEON等SIMD`指令加速。
## 环境配置
操作系统：`ubuntu 16.05`
ndk版本：`android-ndk-r16b`

## 开始编译
`第一步：`先克隆出最新的`libyuv版本`
```java
git clone git@github.com:lemenkov/libyuv.git
```
`第二步：`重命名`libyuv`为`jni`，编辑`Android.mk文件`，注释掉`JPEG相关的配置`

![](https://github.com/byhook/blog/blob/master/ndk/images/20181125101510668.png)

![](https://github.com/byhook/blog/blob/master/ndk/images/20181125101545833.png)

`第三步：`新建`Application.mk文件`

```java
APP_ABI := armeabi-v7a x86 x86_64
APP_PLATFORM := android-16
APP_STL := stlport_static  
APP_CPPFLAGS += -fno-rtti  
```

开始编译`ndk-build`

![](https://github.com/byhook/blog/blob/master/ndk/images/20181125101859154.png)

## 工程实践

还是基于之前的[graphic4android工程](https://github.com/byhook/graphic4android)，新建`native-libyuv`工程，配置可以参考之前的配置：

```java
enum Type {
    TYPE_YUV420P_TO_RGB24 = 0,
    TYPE_NV12_TO_RGB24 = 1,
    TYPE_NV21_TO_RGB24 = 2
};

/**
 * I420就是YUV420P
 * @param yuvData
 * @param rgb24
 * @param width
 * @param height
 */
void I420_TO_RGB24(unsigned char *yuvData, unsigned char *rgb24, int width, int height) {

    unsigned char *ybase = yuvData;
    unsigned char *ubase = &yuvData[width * height];
    unsigned char *vbase = &yuvData[width * height * 5 / 4];
    //YUV420P转RGB24
    libyuv::I420ToRGB24(ybase, width, ubase, width / 2, vbase, width / 2,
                        rgb24,
                        width * 3, width, height);

}

/**
 * NV12属于YUV420SP
 * @param yuvData
 * @param rgb24
 * @param width
 * @param height
 */
void NV12_TO_RGB24(unsigned char *yuvData, unsigned char *rgb24, int width, int height) {

    unsigned char *ybase = yuvData;
    unsigned char *uvbase = &yuvData[width * height];
    //NV12转RGB24
    libyuv::NV12ToRGB24(ybase, width, uvbase, width,
                        rgb24,
                        width * 3, width, height);

}

/**
 * NV21属于YUV420SP
 * @param yuvData
 * @param rgb24
 * @param width
 * @param height
 */
void NV21_TO_RGB24(unsigned char *yuvData, unsigned char *rgb24, int width, int height) {

    unsigned char *ybase = yuvData;
    unsigned char *vubase = &yuvData[width * height];
    //NV21转RGB24
    libyuv::NV21ToRGB24(ybase, width, vubase, width,
                        rgb24,
                        width * 3, width, height);

}

void drawYUV(const char *path, int type, int width, int height, ANativeWindow_Buffer buffer) {
    FILE *file = fopen(path, "rb");

    int frameSize = width * height * 3 / 2;

    unsigned char *yuvData = new unsigned char[frameSize];

    fread(yuvData, 1, frameSize, file);

    unsigned char *rgb24 = new unsigned char[width * height * 3];

    //YUV转RGB24
    switch (type) {
        case TYPE_YUV420P_TO_RGB24:
            //YUV420P转RGB24
            I420_TO_RGB24(yuvData, rgb24, width, height);
            break;
        case TYPE_NV12_TO_RGB24:
            //YUV420SP转RGB24
            NV12_TO_RGB24(yuvData, rgb24, width, height);
            break;
        case TYPE_NV21_TO_RGB24:
            //YUV420SP转RGB24
            NV21_TO_RGB24(yuvData, rgb24, width, height);
            break;
    }

    uint32_t *line = (uint32_t *) buffer.bits;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = y * width + x;
            line[x] = rgb24[index * 3] << 16
                      | rgb24[index * 3 + 1] << 8
                      | rgb24[index * 3 + 2];
        }
        line = line + buffer.stride;
    }

    //释放内存
    delete[] yuvData;
    delete[] rgb24;

    //关闭文件句柄
    fclose(file);
}

void yuv2rgb(JNIEnv *env, jobject obj, jstring yuvPath, jint type, jint width, jint height,
             jobject surface) {

    const char *path = env->GetStringUTFChars(yuvPath, 0);

    //获取目标surface
    ANativeWindow *window = ANativeWindow_fromSurface(env, surface);
    if (NULL == window) {
        ThrowException(env, "java/lang/RuntimeException", "unable to get native window");
        return;
    }
    //默认的是RGB_565
    int32_t result = ANativeWindow_setBuffersGeometry(window, 0, 0, WINDOW_FORMAT_RGBA_8888);
    if (result < 0) {
        ThrowException(env, "java/lang/RuntimeException", "unable to set buffers geometry");
        //释放窗口
        ANativeWindow_release(window);
        window = NULL;
        return;
    }
    ANativeWindow_acquire(window);

    ANativeWindow_Buffer buffer;
    //锁定窗口的绘图表面
    if (ANativeWindow_lock(window, &buffer, NULL) < 0) {
        ThrowException(env, "java/lang/RuntimeException", "unable to lock native window");
        //释放窗口
        ANativeWindow_release(window);
        window = NULL;
        return;
    }

    //绘制YUV420P
    drawYUV(path, type, width, height, buffer);

    //解锁窗口的绘图表面
    if (ANativeWindow_unlockAndPost(window) < 0) {
        ThrowException(env, "java/lang/RuntimeException",
                       "unable to unlock and post to native window");
    }

    env->ReleaseStringUTFChars(yuvPath, path);
    //释放
    ANativeWindow_release(window);
}

```

直接调用`libyuv库里的函数即可完成转换过程`

![](https://github.com/byhook/blog/blob/master/ndk/images/20181125125310654.png)
