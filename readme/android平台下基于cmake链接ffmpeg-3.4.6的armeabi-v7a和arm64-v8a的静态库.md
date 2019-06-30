
## 概述
之前记录过`ffmpeg`相关的移植内容，最近有人问我，关于64位的问题，笔者今天抽空重新编译了一下**`ffmpeg-3.4.6`**版本的32位和64位的静态库，基于cmake来进行编译链接，中间有一些坑，记录一下。

## 环境配置

**系统：`ubuntu 16.04`**
**ndk版本：`android-ndk-r15c`**
**fdk-aac版本：`0.1.6`**
**x264版本：`最新的stable版本`**

**固定的配置脚本：`config.sh`，根据你自己的环境变量，变更一下NDK的路径**

```java

#NDK路径
export ANDROID_NDK_ROOT=/home/byhook/android/android-ndk-r15c
export AOSP_TOOLCHAIN_SUFFIX=4.9

export AOSP_API="android-21"

#架构
if [ "$#" -lt 1 ]; then
	THE_ARCH=armv7
else
	THE_ARCH=$(tr [A-Z] [a-z] <<< "$1")
fi

#根据不同架构配置环境变量
case "$THE_ARCH" in
  arm|armv5|armv6|armv7|armeabi)
	TOOLCHAIN_BASE="arm-linux-androideabi"
	TOOLNAME_BASE="arm-linux-androideabi"
	AOSP_ABI="armeabi"
	AOSP_ARCH="arm"
	HOST="arm-linux-androideabi"
	AOSP_FLAGS="-march=armv5te -mtune=xscale -mthumb -msoft-float -funwind-tables -fexceptions -frtti"
	FF_EXTRA_CFLAGS="-O3 -fpic -fasm -Wno-psabi -fno-short-enums -fno-strict-aliasing -finline-limit=300 -mfloat-abi=softfp -mfpu=vfp -marm -march=armv6 "
	FF_CFLAGS="-O3 -Wall -pipe -ffast-math -fstrict-aliasing -Werror=strict-aliasing -Wno-psabi -Wa,--noexecstack -DANDROID  "
	;;
  armv7a|armeabi-v7a)
	TOOLCHAIN_BASE="arm-linux-androideabi"
	TOOLNAME_BASE="arm-linux-androideabi"
	AOSP_ABI="armeabi-v7a"
	AOSP_ARCH="arm"
	HOST="arm-linux-androideabi"
	AOSP_FLAGS="-march=armv7-a -mthumb -mfpu=vfpv3-d16 -mfloat-abi=softfp -Wl,--fix-cortex-a8 -funwind-tables -fexceptions -frtti"
	FF_EXTRA_CFLAGS="-DANDROID -fPIC -ffunction-sections -funwind-tables -fstack-protector -march=armv7-a -mfloat-abi=softfp -mfpu=vfpv3-d16 -fomit-frame-pointer -fstrict-aliasing -funswitch-loops -finline-limit=300 "
	FF_CFLAGS="-O3 -Wall -pipe -ffast-math -fstrict-aliasing -Werror=strict-aliasing -Wno-psabi -Wa,--noexecstack -DANDROID  "
	;;
  hard|armv7a-hard|armeabi-v7a-hard)
	TOOLCHAIN_BASE="arm-linux-androideabi"
	TOOLNAME_BASE="arm-linux-androideabi"
	AOSP_ABI="armeabi-v7a"
	AOSP_ARCH="arm"
	HOST="arm-linux"
	AOSP_FLAGS="-mhard-float -D_NDK_MATH_NO_SOFTFP=1 -march=armv7-a -mfpu=vfpv3-d16 -mfloat-abi=softfp -Wl,--fix-cortex-a8 -funwind-tables -fexceptions -frtti -Wl,--no-warn-mismatch -Wl,-lm_hard"
	FF_EXTRA_CFLAGS="-DANDROID -fPIC -ffunction-sections -funwind-tables -fstack-protector -march=armv7-a -mfloat-abi=softfp -mfpu=vfpv3-d16 -fomit-frame-pointer -fstrict-aliasing -funswitch-loops -finline-limit=300 "
	FF_CFLAGS="-O3 -Wall -pipe -ffast-math -fstrict-aliasing -Werror=strict-aliasing -Wno-psabi -Wa,--noexecstack -DANDROID  "
	;;
  neon|armv7a-neon)
	TOOLCHAIN_BASE="arm-linux-androideabi"
	TOOLNAME_BASE="arm-linux-androideabi"
	AOSP_ABI="armeabi-v7a"
	AOSP_ARCH="arm"
	HOST="arm-linux"
	AOSP_FLAGS="-march=armv7-a -mfpu=vfpv3-d16 -mfloat-abi=softfp -Wl,--fix-cortex-a8 -funwind-tables -fexceptions -frtti"
	FF_EXTRA_CFLAGS="-DANDROID -fPIC -ffunction-sections -funwind-tables -fstack-protector -march=armv7-a -mfloat-abi=softfp -mfpu=vfpv3-d16 -fomit-frame-pointer -fstrict-aliasing -funswitch-loops -finline-limit=300 "
	FF_CFLAGS="-O3 -Wall -pipe -ffast-math -fstrict-aliasing -Werror=strict-aliasing -Wno-psabi -Wa,--noexecstack -DANDROID  "
	;;
  armv8|armv8a|aarch64|arm64|arm64-v8a)
	TOOLCHAIN_BASE="aarch64-linux-android"
	TOOLNAME_BASE="aarch64-linux-android"
	AOSP_ABI="arm64-v8a"
	AOSP_ARCH="arm64"
	HOST="aarch64-linux"
	AOSP_FLAGS="-funwind-tables -fexceptions -frtti"
	FF_EXTRA_CFLAGS=""
	FF_CFLAGS="-O3 -Wall -pipe -ffast-math -fstrict-aliasing -Werror=strict-aliasing -Wno-psabi -Wa,--noexecstack -DANDROID  "
	;;
  mips|mipsel)
	TOOLCHAIN_BASE="mipsel-linux-android"
	TOOLNAME_BASE="mipsel-linux-android"
	AOSP_ABI="mips"
	AOSP_ARCH="mips"
	HOST="mipsel-linux"
	AOSP_FLAGS="-funwind-tables -fexceptions -frtti"
	;;
  mips64|mipsel64|mips64el)
	TOOLCHAIN_BASE="mips64el-linux-android"
	TOOLNAME_BASE="mips64el-linux-android"
	AOSP_ABI="mips64"
	AOSP_ARCH="mips64"
	HOST="mips64el-linux-android"
	AOSP_FLAGS="-funwind-tables -fexceptions -frtti"
	;;
  x86)
	TOOLCHAIN_BASE="x86"
	TOOLNAME_BASE="i686-linux-android"
	AOSP_ABI="x86"
	AOSP_ARCH="x86"
	HOST="i686-linux"
	AOSP_FLAGS="-march=i686 -mtune=intel -mssse3 -mfpmath=sse -funwind-tables -fexceptions -frtti"
	FF_EXTRA_CFLAGS="-O3 -DANDROID -Dipv6mr_interface=ipv6mr_ifindex -fasm -Wno-psabi -fno-short-enums -fno-strict-aliasing -fomit-frame-pointer -march=k8 "
	FF_CFLAGS="-O3 -Wall -pipe -ffast-math -fstrict-aliasing -Werror=strict-aliasing -Wno-psabi -Wa,--noexecstack -DANDROID  "
	;;
  x86_64|x64)
	TOOLCHAIN_BASE="x86_64"
	TOOLNAME_BASE="x86_64-linux-android"
	AOSP_ABI="x86_64"
	AOSP_ARCH="x86_64"
	HOST="x86_64-linux"
	AOSP_FLAGS="-march=x86-64 -msse4.2 -mpopcnt -mtune=intel -funwind-tables -fexceptions -frtti"
	FF_EXTRA_CFLAGS="-O3 -DANDROID -Dipv6mr_interface=ipv6mr_ifindex -fasm -Wno-psabi -fno-short-enums -fno-strict-aliasing -fomit-frame-pointer -march=k8 "
        FF_CFLAGS="-O3 -Wall -pipe -ffast-math -fstrict-aliasing -Werror=strict-aliasing -Wno-psabi -Wa,--noexecstack -DANDROID  "
	;;
  *)
	echo "ERROR: Unknown architecture $1"
	[ "$0" = "$BASH_SOURCE" ] && exit 1 || return 1
	;;
esac

echo "TOOLCHAIN_BASE="$TOOLCHAIN_BASE
echo "TOOLNAME_BASE="$TOOLNAME_BASE
echo "AOSP_ABI="$AOSP_ABI
echo "AOSP_ARCH="$AOSP_ARCH
echo "AOSP_FLAGS="$AOSP_FLAGS
echo "HOST="$HOST

```

**脚本与源码目录同级：**

![](https://github.com/byhook/ffmpeg4android/blob/master/readme/images/2019063023370224.png)

## 安装NASM和YASM

直接看官方的文档，保证这两个已经成功安装，并且版本有效

https://trac.ffmpeg.org/wiki/CompilationGuide/Ubuntu

## 编译fdk-aac0.1.6

**`注意这里不建议使用最新的2.0.0版本，不然编译ffmpeg-3.4.6会报错`**

**脚本build_fdkaac.sh内容如下：**

```java
#!/bin/bash

ARCH=$1

source config.sh $ARCH
LIBS_DIR=$(cd `dirname $0`; pwd)/libs/libfdk-aac
echo "LIBS_DIR="$LIBS_DIR

cd fdk-aac-0.1.6


PREFIX=$LIBS_DIR/$AOSP_ABI
TOOLCHAIN=$ANDROID_NDK_ROOT/toolchains/$TOOLCHAIN_BASE-$AOSP_TOOLCHAIN_SUFFIX/prebuilt/linux-x86_64
SYSROOT=$ANDROID_NDK_ROOT/platforms/$AOSP_API/arch-$AOSP_ARCH
CROSS_PREFIX=$TOOLCHAIN/bin/$TOOLNAME_BASE-

CFLAGS=""

FLAGS="--enable-static  --host=$HOST --target=android --disable-asm "

export CXX="${CROSS_PREFIX}g++ --sysroot=${SYSROOT}"
export LDFLAGS=" -L$SYSROOT/usr/lib  $CFLAGS "
export CXXFLAGS=$CFLAGS
export CFLAGS=$CFLAGS
export CC="${CROSS_PREFIX}gcc --sysroot=${SYSROOT}"
export AR="${CROSS_PREFIX}ar"
export LD="${CROSS_PREFIX}ld"
export AS="${CROSS_PREFIX}gcc"


./configure $FLAGS \
--enable-pic \
--enable-strip \
--prefix=$PREFIX

$ADDITIONAL_CONFIGURE_FLAG

make clean
make -j8
make install

cd ..
```

**编译全平台build_fdkaac_all.sh内容如下：**

```java
for arch in armeabi-v7a arm64-v8a x86 x86_64
do
    bash build_fdkaac.sh $arch
done
```

## 编译x264

**脚本build_x264.sh内容如下：**
```java
#!/bin/bash

ARCH=$1

source config.sh $ARCH
LIBS_DIR=$(cd `dirname $0`; pwd)/libs/libx264
echo "LIBS_DIR="$LIBS_DIR

cd x264

PREFIX=$LIBS_DIR/$AOSP_ABI
TOOLCHAIN=$ANDROID_NDK_ROOT/toolchains/$TOOLCHAIN_BASE-$AOSP_TOOLCHAIN_SUFFIX/prebuilt/linux-x86_64
SYSROOT=$ANDROID_NDK_ROOT/platforms/$AOSP_API/arch-$AOSP_ARCH
CROSS_PREFIX=$TOOLCHAIN/bin/$TOOLNAME_BASE-


./configure --prefix=$PREFIX \
--disable-shared \
--enable-static \
--enable-pic \
--enable-neon \
--disable-cli \
--host=$HOST \
--cross-prefix=$CROSS_PREFIX \
--sysroot=$SYSROOT \
--extra-cflags="-Os -fpic" \
--extra-ldflags=""

make clean
make -j4
make install

cd ..
```

**全平台脚本build_x264_all.sh内容如下：**

```java
for arch in armeabi-v7a arm64-v8a x86 x86_64
do
    bash build_x264.sh $arch
done
```

## 编译ffmpeg-3.4.6

**脚本build_ffmpeg_with_x264_fdkaac.sh内容如下：**

```java
#!/bin/bash

ARCH=$1

source config.sh $ARCH
NOW_DIR=$(cd `dirname $0`; pwd)
LIBS_DIR=$NOW_DIR/libs
echo "LIBS_DIR="$LIBS_DIR

cd ffmpeg-3.4.6

PREFIX=$LIBS_DIR/ffmpeg-x264-fdkaac-merge/$AOSP_ABI
TOOLCHAIN=$ANDROID_NDK_ROOT/toolchains/$TOOLCHAIN_BASE-$AOSP_TOOLCHAIN_SUFFIX/prebuilt/linux-x86_64
SYSROOT=$ANDROID_NDK_ROOT/platforms/$AOSP_API/arch-$AOSP_ARCH
CROSS_PREFIX=$TOOLCHAIN/bin/$TOOLNAME_BASE-


FDK_INCLUDE=$LIBS_DIR/libfdk-aac/$AOSP_ABI/include
FDK_LIB=$LIBS_DIR/libfdk-aac/$AOSP_ABI/lib
X264_INCLUDE=$LIBS_DIR/libx264/$AOSP_ABI/include
X264_LIB=$LIBS_DIR/libx264/$AOSP_ABI/lib

echo $SYSROOT
echo $CROSS_PREFIX
echo $FDK_LIB
echo $X264_LIB

./configure \
--prefix=$PREFIX \
--enable-cross-compile \
--disable-runtime-cpudetect \
--disable-asm \
--arch=$AOSP_ARCH \
--target-os=android \
--cc=$TOOLCHAIN/bin/$TOOLNAME_BASE-gcc \
--cross-prefix=$CROSS_PREFIX \
--disable-stripping \
--nm=$TOOLCHAIN/bin/$TOOLNAME_BASE-nm \
--sysroot=$SYSROOT \
--extra-cflags="-I$X264_INCLUDE -I$FDK_INCLUDE " \
--extra-ldflags="-L$X264_LIB -L$FDK_LIB" \
--enable-gpl \
--enable-nonfree \
--disable-shared \
--enable-static \
--enable-version3 \
--enable-pthreads \
--enable-small \
--disable-vda \
--disable-iconv \
--enable-libx264 \
--enable-neon \
--enable-yasm \
--enable-libfdk_aac \
--enable-encoder=libx264 \
--enable-encoder=mpeg4 \
--enable-encoder=libfdk_aac \
--enable-encoder=mjpeg \
--enable-encoder=png \
--enable-nonfree \
--enable-muxers \
--enable-muxer=mov \
--enable-muxer=mp4 \
--enable-muxer=h264 \
--enable-muxer=avi \
--enable-decoder=aac \
--enable-decoder=aac_latm \
--enable-decoder=h264 \
--enable-decoder=mpeg4 \
--enable-decoder=mjpeg \
--enable-decoder=png \
--enable-demuxer=image2 \
--enable-demuxer=h264 \
--enable-demuxer=aac \
--enable-demuxer=avi \
--enable-demuxer=mpc \
--enable-demuxer=mpegts \
--enable-demuxer=mov \
--enable-parser=aac \
--enable-parser=ac3 \
--enable-parser=h264 \
--enable-protocols \
--enable-zlib \
--enable-avfilter \
--enable-postproc \
--enable-avdevice \
--enable-avresample \
--disable-outdevs \
--disable-ffprobe \
--disable-ffplay \
--disable-ffmpeg \
--disable-ffserver \
--disable-debug \
--disable-ffprobe \
--disable-ffplay \
--disable-ffmpeg \
--disable-symver \
--disable-stripping \
--extra-cflags="$FF_EXTRA_CFLAGS  $FF_CFLAGS" \
--extra-ldflags="  "

make clean
make -j4
make install

cd ..
```

**全平台脚本build_ffmpeg_with_x264_fdkaac_all.sh内容如下：**

```java
for arch in armeabi-v7a arm64-v8a
do
    bash build_ffmpeg_with_x264_fdkaac.sh $arch
done
```

## 链接静态库

**考虑到现在的ndk开发，基本都是基于`android-studio`的`cmake`来完成，而基于动态库的实现比较简单，之前博客也有，这里记录一下，基于静态库来编译链接最终的`so库`**

**这里以`ffmpeg-stream-mp4`工程为例，CMakeLists.txt内容如下**

```java

cmake_minimum_required(VERSION 3.4.1)


add_library(native-stream
           SHARED
           src/main/cpp/native_stream.cpp
           src/main/cpp/mp4_stream.cpp)

#获取上级目录
get_filename_component(PARENT_DIR ${CMAKE_SOURCE_DIR} PATH)
set(LIBRARY_DIR ${PARENT_DIR}/common-static)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
set(CMAKE_VERBOSE_MAKEFILE on)

add_library(avcodec
           STATIC
           IMPORTED)

set_target_properties(avcodec
                    PROPERTIES IMPORTED_LOCATION
                    ${LIBRARY_DIR}/libs/${ANDROID_ABI}/lib/libavcodec.a
                    )

add_library(avdevice
           STATIC
           IMPORTED)

set_target_properties(avdevice
                    PROPERTIES IMPORTED_LOCATION
                    ${LIBRARY_DIR}/libs/${ANDROID_ABI}/lib/libavdevice.a
                    )

add_library(avfilter
           STATIC
           IMPORTED)

set_target_properties(avfilter
                    PROPERTIES IMPORTED_LOCATION
                    ${LIBRARY_DIR}/libs/${ANDROID_ABI}/lib/libavfilter.a
                    )

add_library(avformat
           STATIC
           IMPORTED)

set_target_properties(avformat
                    PROPERTIES IMPORTED_LOCATION
                    ${LIBRARY_DIR}/libs/${ANDROID_ABI}/lib/libavformat.a
                    )



add_library(avresample
           STATIC
           IMPORTED)

set_target_properties(avresample
                    PROPERTIES IMPORTED_LOCATION
                    ${LIBRARY_DIR}/libs/${ANDROID_ABI}/lib/libavresample.a
                    )

add_library(avutil
           STATIC
           IMPORTED)

set_target_properties(avutil
                    PROPERTIES IMPORTED_LOCATION
                    ${LIBRARY_DIR}/libs/${ANDROID_ABI}/lib/libavutil.a
                    )

add_library(postproc
           STATIC
           IMPORTED)

set_target_properties(postproc
                    PROPERTIES IMPORTED_LOCATION
                    ${LIBRARY_DIR}/libs/${ANDROID_ABI}/lib/libpostproc.a
                    )

add_library(swresample
           STATIC
           IMPORTED)

set_target_properties(swresample
                    PROPERTIES IMPORTED_LOCATION
                    ${LIBRARY_DIR}/libs/${ANDROID_ABI}/lib/libswresample.a
                    )

add_library(swscale
           STATIC
           IMPORTED)

set_target_properties(swscale
                    PROPERTIES IMPORTED_LOCATION
                    ${LIBRARY_DIR}/libs/${ANDROID_ABI}/lib/libswscale.a
                    )

add_library(fdk-aac
           STATIC
           IMPORTED)

set_target_properties(fdk-aac
                    PROPERTIES IMPORTED_LOCATION
                    ${LIBRARY_DIR}/libs/${ANDROID_ABI}/lib/libfdk-aac.a
                    )


add_library(x264
           STATIC
           IMPORTED)

set_target_properties(x264
                    PROPERTIES IMPORTED_LOCATION
                    ${LIBRARY_DIR}/libs/${ANDROID_ABI}/lib/libx264.a
                    )

#头文件
include_directories(${LIBRARY_DIR}/libs/${ANDROID_ABI}/include)

#静态库一定要注意下面的链接顺序，否则会报错
target_link_libraries(native-stream
                    avformat
                    avdevice
                    avfilter
                    avcodec
                    avutil
                    postproc
                    swresample
                    avresample
                    swscale
                    x264
                    fdk-aac
                    z
                    c
                    m
                    dl
                    log)

```

**`这里要注意：静态库链接的顺序很重要！！！`**

**build.gradle中的配置**

```java
externalNativeBuild {
      ndk {
          ldLibs "rt"
          abiFilters 'armeabi-v7a','arm64-v8a'
      }
      cmake {
          cppFlags "-frtti -fexceptions"
          arguments '-DANDROID_PLATFORM=android-21',
                    '-DANDROID_TOOLCHAIN=clang'
      }
}
```


#### 项目地址`ffmpeg-stream-mp4`
https://github.com/byhook/ffmpeg4android

#### 参考
https://trac.ffmpeg.org/wiki/CompilationGuide/Ubuntu