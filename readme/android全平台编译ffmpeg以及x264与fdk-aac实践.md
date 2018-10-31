
### 目录

- [编译环境](#编译环境)
- [单独编译完整功能ffmpeg库](#单独编译完整功能ffmpeg库)
- [开始编译x264库](#开始编译x264库)
- [开始编译fdk-aac库](#开始编译fdk-aac库)
- [ffmpeg混合编译x264和fdk-aar库](#ffmpeg混合编译x264和fdk-aar库)

#### 编译环境

`操作系统：ubuntu 16.05`

[android-ndk-r10e](https://dl.google.com/android/repository/android-ndk-r10e-linux-x86_64.zip)

`注意 这里如果使用高版本比如android-ndk-r16b 编译会报错`

[ffmpeg-3.3.8版本](http://ffmpeg.org/releases/ffmpeg-3.3.8.tar.gz)

[fdk-aac-0.1.6版本](https://downloads.sourceforge.net/opencore-amr/fdk-aac-0.1.6.tar.gz)

#### 单独编译完整功能ffmpeg库

- 第一步：编写`config.sh`环境配置文件

```java
#NDK路径
export ANDROID_NDK_ROOT=/home/byhook/android/android-ndk-r10e

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
	AOSP_ARCH="arch-arm"
	HOST="arm-linux-androideabi"
	AOSP_FLAGS="-march=armv5te -mtune=xscale -mthumb -msoft-float -funwind-tables -fexceptions -frtti"
	FF_EXTRA_CFLAGS="-O3 -fpic -fasm -Wno-psabi -fno-short-enums -fno-strict-aliasing -finline-limit=300 -mfloat-abi=softfp -mfpu=vfp -marm -march=armv6 "
	FF_CFLAGS="-O3 -Wall -pipe -ffast-math -fstrict-aliasing -Werror=strict-aliasing -Wno-psabi -Wa,--noexecstack -DANDROID  "
	;;
  armv7a|armeabi-v7a)
	TOOLCHAIN_BASE="arm-linux-androideabi"
	TOOLNAME_BASE="arm-linux-androideabi"
	AOSP_ABI="armeabi-v7a"
	AOSP_ARCH="arch-arm"
	HOST="arm-linux-androideabi"
	AOSP_FLAGS="-march=armv7-a -mthumb -mfpu=vfpv3-d16 -mfloat-abi=softfp -Wl,--fix-cortex-a8 -funwind-tables -fexceptions -frtti"
	FF_EXTRA_CFLAGS="-DANDROID -fPIC -ffunction-sections -funwind-tables -fstack-protector -march=armv7-a -mfloat-abi=softfp -mfpu=vfpv3-d16 -fomit-frame-pointer -fstrict-aliasing -funswitch-loops -finline-limit=300 "
	FF_CFLAGS="-O3 -Wall -pipe -ffast-math -fstrict-aliasing -Werror=strict-aliasing -Wno-psabi -Wa,--noexecstack -DANDROID  "
	;;
  hard|armv7a-hard|armeabi-v7a-hard)
	TOOLCHAIN_BASE="arm-linux-androideabi"
	TOOLNAME_BASE="arm-linux-androideabi"
	AOSP_ABI="armeabi-v7a"
	AOSP_ARCH="arch-arm"
	HOST="arm-linux-androideabi"
	AOSP_FLAGS="-mhard-float -D_NDK_MATH_NO_SOFTFP=1 -march=armv7-a -mfpu=vfpv3-d16 -mfloat-abi=softfp -Wl,--fix-cortex-a8 -funwind-tables -fexceptions -frtti -Wl,--no-warn-mismatch -Wl,-lm_hard"
	FF_EXTRA_CFLAGS="-DANDROID -fPIC -ffunction-sections -funwind-tables -fstack-protector -march=armv7-a -mfloat-abi=softfp -mfpu=vfpv3-d16 -fomit-frame-pointer -fstrict-aliasing -funswitch-loops -finline-limit=300 "
	FF_CFLAGS="-O3 -Wall -pipe -ffast-math -fstrict-aliasing -Werror=strict-aliasing -Wno-psabi -Wa,--noexecstack -DANDROID  "
	;;
  neon|armv7a-neon)
	TOOLCHAIN_BASE="arm-linux-androideabi"
	TOOLNAME_BASE="arm-linux-androideabi"
	AOSP_ABI="armeabi-v7a"
	AOSP_ARCH="arch-arm"
	HOST="arm-linux-androideabi"
	AOSP_FLAGS="-march=armv7-a -mfpu=vfpv3-d16 -mfloat-abi=softfp -Wl,--fix-cortex-a8 -funwind-tables -fexceptions -frtti"
	FF_EXTRA_CFLAGS="-DANDROID -fPIC -ffunction-sections -funwind-tables -fstack-protector -march=armv7-a -mfloat-abi=softfp -mfpu=vfpv3-d16 -fomit-frame-pointer -fstrict-aliasing -funswitch-loops -finline-limit=300 "
	FF_CFLAGS="-O3 -Wall -pipe -ffast-math -fstrict-aliasing -Werror=strict-aliasing -Wno-psabi -Wa,--noexecstack -DANDROID  "
	;;
  armv8|armv8a|aarch64|arm64|arm64-v8a)
	TOOLCHAIN_BASE="aarch64-linux-android"
	TOOLNAME_BASE="aarch64-linux-android"
	AOSP_ABI="arm64-v8a"
	AOSP_ARCH="arch-arm64"
	HOST="aarch64-linux"
	AOSP_FLAGS="-funwind-tables -fexceptions -frtti"
	FF_EXTRA_CFLAGS=""
	FF_CFLAGS="-O3 -Wall -pipe -ffast-math -fstrict-aliasing -Werror=strict-aliasing -Wno-psabi -Wa,--noexecstack -DANDROID  "
	;;
  mips|mipsel)
	TOOLCHAIN_BASE="mipsel-linux-android"
	TOOLNAME_BASE="mipsel-linux-android"
	AOSP_ABI="mips"
	AOSP_ARCH="arch-mips"
	HOST="mipsel-linux"
	AOSP_FLAGS="-funwind-tables -fexceptions -frtti"
	;;
  mips64|mipsel64|mips64el)
	TOOLCHAIN_BASE="mips64el-linux-android"
	TOOLNAME_BASE="mips64el-linux-android"
	AOSP_ABI="mips64"
	AOSP_ARCH="arch-mips64"
	HOST="mipsel64-linux"
	AOSP_FLAGS="-funwind-tables -fexceptions -frtti"
	;;
  x86)
	TOOLCHAIN_BASE="x86"
	TOOLNAME_BASE="i686-linux-android"
	AOSP_ABI="x86"
	AOSP_ARCH="arch-x86"
	HOST="i686-linux"
	AOSP_FLAGS="-march=i686 -mtune=intel -mssse3 -mfpmath=sse -funwind-tables -fexceptions -frtti"
	FF_EXTRA_CFLAGS="-O3 -DANDROID -Dipv6mr_interface=ipv6mr_ifindex -fasm -Wno-psabi -fno-short-enums -fno-strict-aliasing -fomit-frame-pointer -march=k8 "
	FF_CFLAGS="-O3 -Wall -pipe -ffast-math -fstrict-aliasing -Werror=strict-aliasing -Wno-psabi -Wa,--noexecstack -DANDROID  "
	;;
  x86_64|x64)
	TOOLCHAIN_BASE="x86_64"
	TOOLNAME_BASE="x86_64-linux-android"
	AOSP_ABI="x86_64"
	AOSP_ARCH="arch-x86_64"
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
- 第二步：编写`build_ffmpeg.sh`文件

```java
#!/bin/bash

ARCH=$1

source config.sh $ARCH
LIBS_DIR=$(cd `dirname $0`; pwd)/libs/lib-ffmpeg
echo "LIBS_DIR="$LIBS_DIR

cd ffmpeg-3.3.8

PLATFORM=$ANDROID_NDK_ROOT/platforms/$AOSP_API/$AOSP_ARCH
TOOLCHAIN=$ANDROID_NDK_ROOT/toolchains/$TOOLCHAIN_BASE-$AOSP_TOOLCHAIN_SUFFIX/prebuilt/linux-x86_64

PREFIX=$LIBS_DIR/$AOSP_ABI

./configure \
--prefix=$PREFIX \
--enable-cross-compile \
--disable-runtime-cpudetect \
--disable-asm \
--arch=$AOSP_ABI \
--target-os=android \
--cc=$TOOLCHAIN/bin/$TOOLNAME_BASE-gcc \
--cross-prefix=$TOOLCHAIN/bin/$TOOLNAME_BASE- \
--disable-stripping \
--nm=$TOOLCHAIN/bin/$TOOLNAME_BASE-nm \
--sysroot=$PLATFORM \
--enable-gpl \
--enable-shared \
--disable-static \
--enable-small \
--disable-outdevs \
--disable-ffprobe \
--disable-ffplay \
--disable-ffmpeg \
--disable-ffserver \
--disable-debug \
--disable-ffprobe \
--disable-ffplay \
--disable-ffmpeg \
--disable-postproc \
--disable-avdevice \
--disable-symver \
--disable-stripping \
--extra-cflags="$FF_EXTRA_CFLAGS  $FF_CFLAGS" \
--extra-ldflags="  "

make clean
make -j8
make install

cd ..
```

文件目录结构如下：

```java
——ffmpeg-3.3.8
——config.sh
——build_ffmpeg.sh
——build_ffmpeg_all.sh
```

`编译指定架构的版本可以通过如下命令：`

```java
bash build_ffmpeg.sh armeabi-v7a
bash build_ffmpeg.sh x86_64
......
```

`编译全平台支持的版本命令：`

```java
bash build_ffmpeg_all.sh
```

其中`build_ffmpeg_all.sh`脚本内容如下：
```java
for arch in armeabi armeabi-v7a arm64-v8a x86 x86_64
do
    bash build_ffmpeg.sh $arch
done
```

`输出结果：`

![](https://github.com/byhook/ffmpeg4android/blob/master/screenshots/20181028223843689.png)

#### 开始编译x264库

先获取最新的`x264库`

```java
git clone http://git.videolan.org/git/x264.git
```

`脚本文件目录结构如下：`

```java
——x264
——build_x264.sh
——build_all_x264.sh
```

`开始编译：`

```java
#编译指定平台
bash build_x264.sh armeabi-v7a
#编译全平台
bash build_all_x264.sh
```

稍等片刻即可生成相关的文件
![](https://github.com/byhook/ffmpeg4android/blob/master/screenshots/20181028222816153.png)

#### 开始编译fdk-aac库

先下载 [fdk-aac-0.1.6版本](https://downloads.sourceforge.net/opencore-amr/fdk-aac-0.1.6.tar.gz)

`目录结构如下：`
```java
——fdk-aac-0.1.6
——build_fdkaac.sh
——build_all_fdkaac.sh
```
`开始编译：`

```java
#编译指定平台
bash build_fdkaac.sh armeabi-v7a
#编译全平台
bash build_all_fdkaac.sh
```

`输出如下：`
![](https://github.com/byhook/ffmpeg4android/blob/master/screenshots/20181028223303504.png)

#### ffmpeg混合编译x264和fdk-aar库

`目录结构如下：`

![](https://github.com/byhook/ffmpeg4android/blob/master/screenshots/2018102823133512.png)

```java
——fdk-aac-0.1.6
——x264
——ffmpeg-3.3.8
——libs
——build_ffmpeg_with_x264_fdkaac.sh
——build_ffmpeg_with_x264_fdkaac_all.sh
```

`开始编译：`

```java
#编译指定平台
bash build_ffmpeg_with_x264_fdkaac.sh armeabi-v7a
#编译全平台
bash build_ffmpeg_with_x264_fdkaac_all.sh
```

`混合输出结果：`

![](https://github.com/byhook/ffmpeg4android/blob/master/screenshots/20181028224153651.png)

`全部脚本地址：`
https://github.com/byhook/ffmpeg4android

参考：
https://blog.csdn.net/leixiaohua1020/article/details/47008825
https://blog.csdn.net/panda1234lee/article/details/53099203
https://trac.ffmpeg.org/wiki/CompilationGuide/Ubuntu
