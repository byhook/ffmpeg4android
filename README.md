之前记录过一篇编译FFmpeg的文章，没写完整
最近在做ijkplayer的二次开发，重新拾起FFmpeg，记录完。
记得之前的移植很头疼，网上很多帖子，都是抄来抄去，作者也没有去验证
笔者记录的目的，是完整的记录下编译过程，以及从中碰到的坑，避免以后再次花时间预研

下文编译过程为笔者亲自验证通过

1.下载x264解码库
http://www.videolan.org/developers/x264.html
或者
```
git clone http://git.videolan.org/git/x264.git
```
推荐使用git克隆下来，方便以后更新
编写脚本build_x264.sh
```
cd x264

export NDK=/workspace/android-ndk-r10e
export TOOLCHAIN=$NDK/toolchains/arm-linux-androideabi-4.9/prebuilt/linux-x86_64
export PLATFORM=$NDK/platforms/android-8/arch-arm
export PREFIX=../libx264


./configure \
    --prefix=$PREFIX \
    --enable-static \
    --enable-shared \
    --enable-pic \
    --disable-asm \
    --disable-cli \
    --host=arm-linux \
    --cross-prefix=$TOOLCHAIN/bin/arm-linux-androideabi- \
    --sysroot=$PLATFORM

make -j8
make install

cd ..
```
脚本中的目录根据自己机器中的实际情况填写

2.下载FFmpeg库
https://ffmpeg.org/
或者
```shell
git clone https://git.ffmpeg.org/ffmpeg.git
```
编写脚本build_with_x264.sh

```

sh build_x264.sh

export NDK=/workspace/android-ndk-r10e
export TOOLCHAIN=$NDK/toolchains/arm-linux-androideabi-4.9/prebuilt/linux-x86_64
export PLATFORM=$NDK/platforms/android-8/arch-arm
export SYSROOT=$TOOLCHAIN/sysroot/
export PREFIX=../liboutput #编译结果的目录 最终生成的编译结果

# 加入x264编译库
EXTRA_CFLAGS="-I./libx264/include" 
EXTRA_LDFLAGS="-L./libx264/lib"


./configure \
    --target-os=linux \
    --prefix=$PREFIX \
    --enable-cross-compile \
    --enable-runtime-cpudetect \
    --disable-asm \
    --disable-doc \
    --arch=arm \
    --cc=$TOOLCHAIN/bin/arm-linux-androideabi-gcc \
    --cross-prefix=$TOOLCHAIN/bin/arm-linux-androideabi- \
    --disable-stripping \
    --nm=$TOOLCHAIN/bin/arm-linux-androideabi-nm \
    --sysroot=$PLATFORM \
    --enable-gpl \
    --enable-static \
    --disable-shared \
    --enable-version3 \
    --enable-small \
    --disable-vda \
    --disable-iconv \
    --disable-encoders \
    --enable-libx264 \
    --enable-encoder=libx264 \
    --disable-muxers \
    --enable-muxer=mov \
    --enable-muxer=ipod \
    --enable-muxer=psp \
    --enable-muxer=mp4 \
    --enable-muxer=avi \
    --disable-decoders \
    --enable-decoder=aac \
    --enable-decoder=aac_latm \
    --enable-decoder=h264 \
    --enable-decoder=mpeg4 \
    --disable-demuxers \
    --enable-demuxer=h264 \
    --enable-demuxer=mov \
    --disable-parsers \
    --enable-parser=aac \
    --enable-parser=ac3 \
    --enable-parser=h264 \
    --disable-protocols \
    --enable-protocol=file \
    --enable-protocol=rtmp \
    --disable-bsfs \
    --enable-bsf=aac_adtstoasc \
    --enable-bsf=h264_mp4toannexb \
    --disable-indevs \
    --enable-zlib \
    --disable-outdevs \
    --disable-ffprobe \
    --disable-ffplay \
    --disable-ffmpeg \
    --disable-ffserver \
    --disable-debug \
    --extra-cflags=$EXTRA_CFLAGS \
    --extra-ldflags=$EXTRA_LDFLAGS


make clean 
make -j8
make install

# 这段解释见后文
$TOOLCHAIN/bin/arm-linux-androideabi-ld -rpath-link=$PLATFORM/usr/lib -L$PLATFORM/usr/lib -L$PREFIX/lib -soname libffmpeg.so -shared -nostdlib -Bsymbolic --whole-archive --no-undefined -o $PREFIX/libffmpeg.so \
    android-lib/lib/libx264.a \
    libavcodec/libavcodec.a \
    libavfilter/libavfilter.a \
    libswresample/libswresample.a \
    libavformat/libavformat.a \
    libavutil/libavutil.a \
    libswscale/libswscale.a \
    libpostproc/libpostproc.a \
    libavdevice/libavdevice.a \
    -lc -lm -lz -ldl -llog --dynamic-linker=/system/bin/linker $TOOLCHAIN/lib/gcc/arm-linux- androideabi/4.9/libgcc.a  
```

文件目录结构为
```
|—FFmpeg
  |—x264
  |—build_x264.sh
  |—build_with_x264.sh
```
编译输出

```
sh build_with_x264.sh
```
![这里写图片描述](http://img.blog.csdn.net/20160720234826042)


笔者也在Github上开了一个库，记录下编译过的源码

![这里写图片描述](http://img.blog.csdn.net/20160720203413711)



参考
http://zheteng.me/android/2016/05/25/build-ffmpeg-for-android-with-x264/

相关详解
http://blog.csdn.net/leixiaohua1020/article/details/44587465