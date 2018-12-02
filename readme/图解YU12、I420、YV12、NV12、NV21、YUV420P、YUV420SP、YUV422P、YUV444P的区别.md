
## 概述

`YUV模型`是根据一个亮度`(Y分量)`和两个色度`(UV分量)`来定义颜色空间，常见的YUV格式有`YUY2、YUYV、YVYU、UYVY、AYUV、Y41P、Y411、Y211、IF09、IYUV、YV12、YVU9、YUV411、YUV420`等，其中比较常见的`YUV420`分为两种：`YUV420P和YUV420SP`。

我们在android平台下使用相机默认图像格式是`NV21`属于`YUV420SP`格式

![](https://github.com/byhook/ffmpeg4android/blob/master/readme/images/20181120103325455.png)

## YUV采样

```java
YUV 4:4:4采样，每一个Y对应一组UV分量,一个YUV占8+8+8 = 24bits 3个字节。
YUV 4:2:2采样，每两个Y共用一组UV分量,一个YUV占8+4+4 = 16bits 2个字节。
YUV 4:2:0采样，每四个Y共用一组UV分量,一个YUV占8+2+2 = 12bits 1.5个字节。
```

我们最常见的`YUV420P和YUV420SP`都是基于`4:2:0`采样的，所以如果图片的宽为`width`，高为`heigth`，在内存中占的空间为`width * height * 3 / 2`，其中前`width * height`的空间存放`Y分量`，接着`width * height / 4`存放`U分量`，最后`width * height / 4`存放`V分量`。

## YUV420P(YU12和YV12)格式

`YUV420P`又叫`plane平面模式`，`Y , U , V`分别在不同平面，也就是有三个平面，它是`YUV标准格式4：2：0`，主要分为：`YU12和YV12`

![](https://github.com/byhook/ffmpeg4android/blob/master/readme/images/20181119225805899.png)

![](https://github.com/byhook/ffmpeg4android/blob/master/readme/images/2018111923013322.png)

- YU12格式

> 在`android平台下`也叫作`I420格式`，首先是所有`Y值`，然后是所有`U值`，最后是所有`V值`。

![](https://github.com/byhook/ffmpeg4android/blob/master/readme/images/2018112012345555.png)

`YU12：亮度(行×列) + U(行×列/4) + V(行×列/4)`

- YV12格式

>`YV12格式`与`YU12`基本相同，首先是所有`Y值`，然后是所有`V值`，最后是所有`U值`。只要注意从适当的位置提取`U和V值`，`YU12和YV12`都可以使用相同的算法进行处理。

![](https://github.com/byhook/ffmpeg4android/blob/master/readme/images/20181120123516554.png)

`YV12：亮度Y(行×列) + V(行×列/4) + U(行×列/4)`

```java
YU12: YYYYYYYY UUVV    =>    YUV420P
YV12: YYYYYYYY VVUU    =>    YUV420P
```
YUV模型是根据一个亮度(Y分量)和两个色度(UV分量)来定义颜色空间，常见的YUV格式有YUY2、YUYV、YVYU、UYVY、AYUV、Y41P、Y411、Y211、IF09、IYUV、YV12、YVU9、YUV411、YUV420等，其中比较常见的YUV420分为两种：YUV420P和YUV420SP。

我们在android平台下使用相机默认图像格式是NV21属于YUV420SP格式## YUV420SP(NV21和NV12)格式

`YUV420SP`格式的图像阵列，首先是所有`Y值`，然后是`UV`或者`VU`交替存储，`NV12和NV21属于YUV420SP`格式，是一种`two-plane模式`，即`Y和UV分为两个plane`，但是`UV(CbCr)`为交错存储，而不是分为三个平面。

![](https://github.com/byhook/ffmpeg4android/blob/master/readme/images/201811192258324.png)

- NV21格式

>android手机从摄像头采集的预览数据一般都是NV21，存储顺序是先存Y，再VU交替存储，`NV21`存储顺序是先存`Y值`，再`VU`交替存储：`YYYYVUVUVU`，以 `4 X 4` 图片为例子，占用内存为 `4 X 4 X 3 / 2 = 24` 个字节

![](https://github.com/byhook/ffmpeg4android/blob/master/readme/images/20181119235116552.png)

- NV12格式

>NV12与NV21类似，也属于`YUV420SP`格式，`NV12`存储顺序是先存`Y值`，再`UV`交替存储：`YYYYUVUVUV`，以 `4 X 4` 图片为例子，占用内存为 `4 X 4 X 3 / 2 = 24` 个字节

![](https://github.com/byhook/ffmpeg4android/blob/master/readme/images/20181119235210475.png)

`注意：在DVD中，色度信号被存储成Cb和Cr(C代表颜色，b代表蓝色，r代表红色)`

```java
NV12: YYYYYYYY UVUV    =>YUV420SP
NV21: YYYYYYYY VUVU    =>YUV420SP
```

## YUV和RGB转换

```java
Y      =  (0.257 * R) + (0.504 * G) + (0.098 * B) + 16
Cr = V =  (0.439 * R) - (0.368 * G) - (0.071 * B) + 128
Cb = U = -(0.148 * R) - (0.291 * G) + (0.439 * B) + 128

B = 1.164(Y - 16) + 2.018(U - 128)
G = 1.164(Y - 16) - 0.813(V - 128) - 0.391(U - 128)
R = 1.164(Y - 16) + 1.596(V - 128)
```

## 分离YUV420P

下面基于实例来理解`Y，U，V分量的作用`

![](https://github.com/byhook/ffmpeg4android/blob/master/readme/images/20181120132051472.jpg)

先使用`ffmpeg`将指定的图片转为`yuv420p`格式

```java
ffmpeg -i input.jpg -s 510x510 -pix_fmt yuv420p input.yuv
```

- `分离YUV分量`

笔者使用的`Clion`直接运行下面这段代码，分离出所需的文件

```java
void split_yuv420(char *inputPath, int width, int height) {

    FILE *fp_yuv = fopen(inputPath, "rb+");

    FILE *fp_y = fopen("output_420_y.y", "wb+");
    FILE *fp_u = fopen("output_420_u.y", "wb+");
    FILE *fp_v = fopen("output_420_v.y", "wb+");

    unsigned char *data = (unsigned char *) malloc(width * height * 3 / 2);

    fread(data, 1, width * height * 3 / 2, fp_yuv);
    //Y
    fwrite(data, 1, width * height, fp_y);
    //U
    fwrite(data + width * height, 1, width * height / 4, fp_u);
    //V
    fwrite(data + width * height * 5 / 4, 1, width * height / 4, fp_v);

    //释放资源
    free(data);

    fclose(fp_yuv);
    fclose(fp_y);
    fclose(fp_u);
    fclose(fp_v);
}
```

笔者使用的是`ubuntu系统`，因此运行`yuvplayer.exe`文件，需要提前安装好`wine`：`sudo apt install wine`，运行`yuvplayer`之后，需要先设置`像素格式为Y`，否则你看到的图像可能会有问题

先看`output_420_y.y`文件：(分辨率设置为510x510)

![](https://github.com/byhook/ffmpeg4android/blob/master/readme/images/20181120145851501.png)

`output_420_u.y`显示如下：(分辨率设置为255x255)

![](https://github.com/byhook/ffmpeg4android/blob/master/readme/images/20181120150020705.png)

`output_420_v.y`显示如下：(分辨率设置为255x255)

![](https://github.com/byhook/ffmpeg4android/blob/master/readme/images/20181120150043110.png)

- 生成灰度图

>上面的例子实际上已经生成了一个`灰度图`了，但是只保留了`Y分量`，你如果直接用`ffplay工具`查看会有问题，下面的函数将会生成一个标准的`YUV文件`并且保留`Y分量`，你可能会有疑问，为什么`U分量和V分量`要写入`0x80`，其实你可以参考上面的`YUV转RGB的公式`，YUV数据是无法直接显示的，最终需要转成RGB显示，因此我这里是只需要保留`Y分量`，忽略`UV`分量的影响，因此根据上面的公式，我在`Y和U分量中都写入128`就是十六进制的`0x80`

- 保留Y分量(生成灰度图)

```java
void yuv420p_y(char *inputPath, char *outputPath, int width, int height) {

    FILE *inFile = fopen(inputPath, "rb+");
    FILE *outFile = fopen(outputPath, "wb+");

    unsigned char *data = (unsigned char *) malloc(width * height * 3 / 2);

    fread(data, 1, width * height * 3 / 2, inFile);

    //Y分量
    fwrite(data, 1, width * height, outFile);

    unsigned char *buffer = (unsigned char *) malloc(width * height / 4);
    memset(buffer, 0x80, width * height / 4);
    //U分量
    fwrite(buffer, 1, width * height / 4, outFile);
    //V分量
    fwrite(buffer, 1, width * height / 4, outFile);

    free(buffer);
    free(data);
    fclose(inFile);
    fclose(outFile);
}
```

```java
int main() {

    yuv420p_y("/home/byhook/media/input.yuv", "/home/byhook/media/output.yuv", 510, 510);

    return 0;
}
```

使用`ffplay`来播放`yuv格式`的文件：

```java
ffplay -f rawvideo -video_size 510x510 output.yuv
```

要注意这里的分辨率不能错

![](https://github.com/byhook/ffmpeg4android/blob/master/readme/images/20181120131959565.png)

## 分离YUV422P

`YUV422P`基于`YUV 4:2:2`采样，每两个Y共用一组UV分量,一个YUV占8+4+4 = 16bits 2个字节。分离代码如下：

```java
void split_yuv422(char *inputPath, int width, int height) {

    FILE *fp_yuv = fopen(inputPath, "rb+");

    FILE *fp_y = fopen("output_422_y.y", "wb+");
    FILE *fp_u = fopen("output_422_u.y", "wb+");
    FILE *fp_v = fopen("output_422_v.y", "wb+");

    unsigned char *data = (unsigned char *) malloc(width * height * 2);

    fread(data, 1, width * height * 2, fp_yuv);
    //Y
    fwrite(data, 1, width * height, fp_y);
    //U
    fwrite(data + width * height, 1, width * height / 2, fp_u);
    //V
    fwrite(data + width * height * 3 / 2, 1, width * height / 2, fp_v);

    //释放资源
    free(data);

    fclose(fp_yuv);
    fclose(fp_y);
    fclose(fp_u);
    fclose(fp_v);
}
```

## 分离YUV444P

`YUV444P`基于`YUV 4:4:4`采样，每一个Y对应一组UV分量,一个YUV占8+8+8 = 24bits 3个字节。分离代码如下：

```java
void split_yuv444(char *inputPath, int width, int height) {

    FILE *fp_yuv = fopen(inputPath, "rb+");

    FILE *fp_y = fopen("output_444_y.y", "wb+");
    FILE *fp_u = fopen("output_444_u.y", "wb+");
    FILE *fp_v = fopen("output_444_v.y", "wb+");

    unsigned char *data = (unsigned char *) malloc(width * height * 3);

    fread(data, 1, width * height * 3, fp_yuv);
    //Y
    fwrite(data, 1, width * height, fp_y);
    //U
    fwrite(data + width * height, 1, width * height, fp_u);
    //V
    fwrite(data + width * height * 2, 1, width * height, fp_v);

    //释放资源
    free(data);

    fclose(fp_yuv);
    fclose(fp_y);
    fclose(fp_u);
    fclose(fp_v);
}
```

参考：

https://blog.csdn.net/leixiaohua1020/article/details/50534150

https://en.wikipedia.org/wiki/YUV
