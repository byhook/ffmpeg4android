
## 音频概述

一般来说音频的裸数据格式就是脉冲编码调制`(PCM)`数据。描述一段P`CM数据`一般需要以下几个概念：`量化格式(sampleFormat)`、`采样率(sampleRate)`、`声道数(channel)`。以CD的音质为例：量化格式为16比特(2字节)，采样率为44100，声道数为2，这些信息就描述了CD的音质。而对于声音格式，还有一个概念用来描述它的大小，称为数据比特率，即1秒时间内的比特数目，它用于衡量音频数据单位时间内的容量大小。

上述的`裸数据`要在网络中实时在线传播的话，那么这个数据量比较大，所以必须对其进行压缩编码，常见的由`WAV、AAC、MP3、Ogg`等

## 开始配置交叉编译脚本

还是直接复用之前的环境配置脚本`config.sh`

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

编写交叉编译脚本：`build_lame.sh`

>可以根据自己需要决定是否只需要动态库或者静态库，笔者两个都输出来了。

```java
#!/bin/bash

ARCH=$1

source config.sh $ARCH
LIBS_DIR=$(cd `dirname $0`; pwd)/libs/liblame
echo "LIBS_DIR="$LIBS_DIR

cd lame-3.100

PLATFORM=$ANDROID_NDK_ROOT/platforms/$AOSP_API/$AOSP_ARCH
TOOLCHAIN=$ANDROID_NDK_ROOT/toolchains/$TOOLCHAIN_BASE-$AOSP_TOOLCHAIN_SUFFIX/prebuilt/linux-x86_64
CROSS_COMPILE=$TOOLCHAIN/bin/$TOOLNAME_BASE-
SYSROOT=$ANDROID_NDK_ROOT/platforms/$AOSP_API/$AOSP_ARCH

PREFIX=$LIBS_DIR/$AOSP_ABI

CFLAGS=""

FLAGS="--enable-static --enable-shared --host=$HOST --target=android"

export CXX="${CROSS_COMPILE}g++ --sysroot=${SYSROOT}"
export LDFLAGS=" -L$SYSROOT/usr/lib  $CFLAGS "
export CXXFLAGS=$CFLAGS
export CFLAGS=$CFLAGS
export CC="${CROSS_COMPILE}gcc --sysroot=${SYSROOT}"
export AR="${CROSS_COMPILE}ar"
export LD="${CROSS_COMPILE}ld"
export AS="${CROSS_COMPILE}gcc"


./configure $FLAGS \
--disable-frontend \
--prefix=$PREFIX

$ADDITIONAL_CONFIGURE_FLAG

make clean
make -j8
make install

cd ..

```

`开始编译命令`

```java
//编译单个平台
bash build_lame.sh armeabi-v7a
//编译全平台
bash build_lame_all
```

`输出如下：`

![](https://github.com/byhook/ffmpeg4android/blob/master/readme/images/20181110192125957.png)

## 工程实践

基于之前的[项目工程](https://github.com/byhook/ffmpeg4android)

![](https://github.com/byhook/ffmpeg4android/blob/master/readme/images/20181110194320271.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2J5aG9vaw==,size_16,color_FFFFFF,t_70)

新建`CMakeLists.txt`文件

```java

cmake_minimum_required(VERSION 3.4.1)


add_library(lame-encode
           SHARED
           src/main/cpp/lame_encode.cpp
           src/main/cpp/mp3_encode.cpp)

find_library(log-lib
            log)

find_library(android-lib
            android)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
set(CMAKE_VERBOSE_MAKEFILE on)

add_library(mp3-lame
           SHARED
           IMPORTED)

set_target_properties(mp3-lame
                    PROPERTIES IMPORTED_LOCATION
                    ${CMAKE_SOURCE_DIR}/libs/${ANDROID_ABI}/libmp3lame.so
                    )

#头文件
include_directories(${CMAKE_SOURCE_DIR}/libs/${ANDROID_ABI}/include)

target_link_libraries(lame-encode mp3-lame ${log-lib} ${android-lib})

```

新建`LameEncoder.java`文件

```java
public class LameEncoder {

    static {
        System.loadLibrary("lame-encode");
    }

    public native void encode(String pcmPath, String mp3Path, int sampleRate, int channels, int bitRate);

    public native void destroy();

}
```
新建MP3的编码实现类：`mp3_encode.cpp`

```java


#include "mp3_encode.h"


Mp3Encoder::Mp3Encoder() {
    pcmFile = NULL;
    mp3File = NULL;
}

Mp3Encoder::~Mp3Encoder() {

}

int Mp3Encoder::Init(const char *pcmPath, const char *mp3Path,
                     int sampleRate,
                     int channels,
                     int bitRate) {
    int result = -1;
    pcmFile = fopen(pcmPath, "rb");
    if (NULL != pcmFile) {
        mp3File = fopen(mp3Path, "wb");
        if (NULL != mp3File) {
            lameClient = lame_init();
            //设置采样率
            lame_set_in_samplerate(lameClient, sampleRate);
            lame_set_out_samplerate(lameClient, sampleRate);

            lame_set_num_channels(lameClient, channels);
            lame_set_brate(lameClient, bitRate / 1000);

            lame_set_quality(lameClient, 2);
            //初始化参数
            lame_init_params(lameClient);
            result = 0;
        }
    }
    return result;
}

void Mp3Encoder::Encode() {
    int bufferSize = 1024 * 256;
    short *buffer = new short[bufferSize / 2];
    short *leftBuffer = new short[bufferSize / 4];
    short *rightBuffer = new short[bufferSize / 4];
    unsigned char *mp3_buffer = new unsigned char[bufferSize];
    size_t readBufferSize = 0;
    while ((readBufferSize = fread(buffer, 2, bufferSize / 2, pcmFile)) > 0) {
        for (int i = 0; i < readBufferSize; i++) {
            if (i % 2 == 0) {
                leftBuffer[i / 2] = buffer[i];
            } else {
                rightBuffer[i / 2] = buffer[i];
            }
        }
        size_t wroteSize = lame_encode_buffer(lameClient, leftBuffer, rightBuffer,
                                              (int) (readBufferSize / 2), mp3_buffer, bufferSize);
        fwrite(mp3_buffer, 1, wroteSize, mp3File);
    }
    delete[] buffer;
    delete[] leftBuffer;
    delete[] rightBuffer;
    delete[] mp3_buffer;
}

void Mp3Encoder::Release() {
    if (pcmFile) {
        fclose(pcmFile);
    }
    if (mp3File) {
        fclose(mp3File);
        lame_close(lameClient);
    }
}
```

新建JNI的接口实现类：`lame_encode.cpp`

```java
#include <jni.h>
#include <stdio.h>
#include "lame_encode.h"
#include "mp3_encode.h"

/**
 * 动态注册
 */
JNINativeMethod methods[] = {
        {"encode",  "(Ljava/lang/String;Ljava/lang/String;III)V", (void *) encode},
        {"destroy", "()V",                                        (void *) destroy}
};

/**
 * 动态注册
 * @param env
 * @return
 */
jint registerNativeMethod(JNIEnv *env) {
    jclass cl = env->FindClass("com/onzhou/audio/lame/LameEncoder");
    if ((env->RegisterNatives(cl, methods, sizeof(methods) / sizeof(methods[0]))) < 0) {
        return -1;
    }
    return 0;
}

/**
 * 加载默认回调
 * @param vm
 * @param reserved
 * @return
 */
jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env = NULL;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }
    //注册方法
    if (registerNativeMethod(env) != JNI_OK) {
        return -1;
    }
    return JNI_VERSION_1_6;
}


Mp3Encoder *encoder = NULL;

JNIEXPORT void JNICALL encode(JNIEnv *env, jobject obj, jstring pcmFilePath, jstring mp3FilePath, jint sampleRate,
       jint channels,
       jint bitRate) {
    const char *pcmPath = env->GetStringUTFChars(pcmFilePath, NULL);
    const char *mp3Path = env->GetStringUTFChars(mp3FilePath, NULL);
    encoder = new Mp3Encoder();
    encoder->Init(pcmPath, mp3Path, sampleRate, channels, bitRate);
    //开始编码
    encoder->Encode();

    env->ReleaseStringUTFChars(pcmFilePath, mp3Path);
    env->ReleaseStringUTFChars(mp3FilePath, pcmPath);
}

JNIEXPORT void JNICALL destroy(JNIEnv * env, jobject obj) {
    if (NULL != encoder) {
        encoder->Release();
        encoder = NULL;
    }
}
```

在启动的`LameEncodeActivity`中`点击按钮实现`功能如下：

```java
Schedulers.newThread().scheduleDirect(new Runnable() {
            @Override
            public void run() {
                File pcmFile = new File(getExternalFilesDir(null), "input.pcm");
                File mp3File = new File(getExternalFilesDir(null), "output.mp3");
                mLameEncoder = new LameEncoder();
                mLameEncoder.encode(pcmFile.getAbsolutePath(), mp3File.getAbsolutePath(), 44100, 2, 128);
            }
        });
```

输出目录

![](https://github.com/byhook/ffmpeg4android/blob/master/readme/images/20181110193208761.png)

播放正常，表示编码正常。

![](https://github.com/byhook/ffmpeg4android/blob/master/readme/images/20181110193017411.png)


原文地址：

https://blog.csdn.net/byhook/article/details/83926620
