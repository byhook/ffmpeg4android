
## 概述

`RGB色彩模式`是工业界的一种颜色标准，是通过对红、绿、蓝三个颜色通道的变化以及它们相互之间的叠加来得到各式各样的颜色的，RGB即是代表红、绿、蓝三个通道的颜色，这个标准几乎包括了人类视力所能感知的所有颜色，是目前运用最广的颜色系统之一。

## RGB16格式

`RGB16`数据格式主要有二种：`RGB565`和`RGB555`。

- **RGB565**

>每个像素用16比特位表示，占2个字节，RGB分量分别使用5位、6位、5位。

![](https://github.com/byhook/blog/blob/master/ndk/images/20181121131550287.png)

```java
//获取高字节的5个bit
R = color & 0xF800;
//获取中间6个bit
G = color & 0x07E0;
//获取低字节5个bit
B = color & 0x001F;
```

- **RGB555**

>每个像素用16比特位表示，占2个字节，RGB分量都使用5位(最高位不用)。

![](https://github.com/byhook/blog/blob/master/ndk/images/20181121131759287.png)

```java
//获取高字节的5个bit
R = color & 0x7C00;
//获取中间5个bit
G = color & 0x03E0;
//获取低字节5个bit
B = color & 0x001F;
```

## RGB24格式

`RGB24图像`每个像素用`8比特位`表示，占`1个字节`，注意：`在内存中RGB各分量的排列顺序为：BGR BGR BGR ......`。

![](https://github.com/byhook/blog/blob/master/ndk/images/20181121131934533.png)

## RGB32格式

`RGB32图像`每个像素用`32比特位`表示，占`4个字节`，`R，G，B`分量分别用8个bit表示，存储顺序为`B，G，R`，最后`8个字节`保留。注意：`在内存中RGB各分量的排列顺序为：BGRA BGRA BGRA ......`。

- **ARGB32**

>本质就是带`alpha通道`的`RGB24`，与`RGB32`的区别在与，保留的`8个bit`用来表示透明，也就是`alpha`的值。

在内存中的分量排列顺序如下：

![](https://github.com/byhook/blog/blob/master/ndk/images/20181121132055320.png)

```java
R = color & 0x0000FF00;
G = color & 0x00FF0000;
B = color & 0xFF000000;
A = color & 0x000000FF;
```

## android平台下的RGB格式

我们平时在`android平台下`处理`Bitmap`的时候，下面的几个参数应该接触的比较多：

- **Bitmap.Config.ALPHA_8**：

>每个像素用8比特位表示，占1个字节，只有透明度，没有颜色。

- **Bitmap.Config.RGB_565**：

>每个像素用16比特位表示，占2个字节，RGB分量分别使用5位、6位、5位，上面的图已经有作说明。

- **Bitmap.Config.ARGB_4444**：

>每个像素用16比特位表示，占2个字节，由4个4位组成，ARGB分量都是4位。

- **Bitmap.Config.ARGB_8888**：

>每个像素用32比特位表示，占4个字节，由4个8位组成，ARGB分量都是8位。

注意：`java默认使用大端字节序，c/c++默认使用小端字节序`，`android`平台下`Bitmap.config.ARGB_8888`的Bitmap默认是`大端字节序`，当需要把这个图片内存数据给`小端语言`使用的时候，就需要把`大端字节序`转换为`小端字节序`。例如：`java层`的`ARGB_8888`传递给`jni层`使用时，需要把`java层的ARGB_8888`的内存数据转换为`BGRA8888`。

## 分离RGB24像素数据中的R、G、B分量

先准备一张色彩图：

![](https://github.com/byhook/blog/blob/master/ndk/images/20181122105229292.jpeg)

使用`ffmpeg命令`将其转换成`rgb格式`

```java
ffmpeg -i 375x375.jpeg -s 375x375 -pix_fmt rgb24 rgb24.rgb
```

```java
void split_rgb24(char *inputPath, int width, int height) {
    FILE *fp_rgb = fopen(inputPath, "rb+");
    FILE *fp_r = fopen("output_r", "wb+");
    FILE *fp_g = fopen("output_g", "wb+");
    FILE *fp_b = fopen("output_b", "wb+");

    unsigned char *data = (unsigned char *) malloc(width * height * 3);

    fread(data, 1, width * height * 3, fp_rgb);
    int index = 0;
    for (index = 0; index < width * height * 3; index = index + 3) {
        //R
        fwrite(data + index, 1, 1, fp_r);
        //G
        fwrite(data + index + 1, 1, 1, fp_g);
        //B
        fwrite(data + index + 2, 1, 1, fp_b);
    }

    cout << index << endl;

    free(data);
    fclose(fp_rgb);
    fclose(fp_r);
    fclose(fp_g);
    fclose(fp_b);
}
```

```java
int main() {

    split_rgb24("/home/byhook/media/rgb24.rgb", 375, 375);

    return 0;
}
```

最终我们可以通过雷神提供的工具，来查看分离的`R,G,B三个分量的文件`：

- **R分量图像**

![](https://github.com/byhook/blog/blob/master/ndk/images/20181122105621834.png)

- **G分量图像**

![](https://github.com/byhook/blog/blob/master/ndk/images/2018112210563446.png)

- **B分量图像**

![](https://github.com/byhook/blog/blob/master/ndk/images/20181122105641914.png)


原文地址：

https://blog.csdn.net/byhook/article/details/84262330

参考：

https://baike.baidu.com/item/RGB

https://blog.csdn.net/leixiaohua1020/article/details/50534150

https://en.wikipedia.org/wiki/RGBA_color_space
