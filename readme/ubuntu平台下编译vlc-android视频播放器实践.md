
## 概述

视频播放器的方案其实有很多种，常规的比如`ffmpeg`，或者`ijkplayer(基于ffmpeg)`，或者使用系统自带的`MediaPlayer`都可以实现基本的播放功能，但是如果要支持多种格式的话，技术选型可能要作多考虑，之前的博客写了部分关于`ffmpeg`的移植相关的内容，最近准备实现一个简单的播放器，因为这个`vlc-android`是开源的，因此，今天记录一下这个编译过程


## 环境准备

操作系统：`ubuntu 16.05`
ndk版本 ：[android-ndk-r14b](https://dl.google.com/android/repository/android-ndk-r14b-linux-x86_64.zip)
vlc-android版本：`3.0.13`

下面的脚本`build-env.sh`二话不说，先运行一遍，保证自己的电脑里编译环境是齐全的：

```java
sudo apt-get update
sudo apt-get install autoconf make m4 gawk mawk a.k.a g++
sudo apt-get install automake autopoint cmake ant build-essential libtool patch
sudo apt-get install pkg-config protobuf-compiler ragel subversion unzip git wget
sudo apt-get install liba52-0.7.4-dev libxcb-composite0-dev libxcb-xv0-dev libxcb-randr0-dev libasound2-dev
sudo apt-get install libprotobuf-c-dev libprotobuf-c1 libprotobuf-dev protobuf-c-compiler
sudo dpkg --add-architecture i386
sudo apt-get install zlib1g:i386 libstdc++6:i386 libc6:i386
sudo apt install libtool-bin
```

## 检查protoc版本

你可以先看看自己本机的`protoc`版本

```java
#查看版本
protoc --version
```

如果版本低于`3.0.0版本`，需要升级一下`protoc版本`，笔者使用的是最新的`3.6.1版本`，官方文档：https://github.com/protocolbuffers/protobuf/blob/master/src/README.md

```java
git clone https://github.com/protocolbuffers/protobuf.git
cd protobuf
git submodule update --init --recursive
./autogen.sh

./configure
make
make check
sudo make install
sudo ldconfig
```

## 开始编译

最新的代码地址：https://code.videolan.org/videolan/vlc-android

编译过程比较简单，主要是前期依赖的一些环境配置好就可以了，笔者是基于`3.0.13版本进行编译的`，你也可以看看它的`tag列表`，根据自己的需要选择对应的版本

```java
git clone git@code.videolan.org:videolan/vlc-android.git
git checkout 3.0.13
```

先编写配置脚本`config.sh`：

```java
export ANDROID_SDK=/media/byhook/backup/android-sdk-linux
export ANDROID_NDK=/media/byhook/backup/android/android-ndk-r14b
export PATH=$PATH:$ANDROID_SDK/platform-tools:$ANDROID_SDK/tools
```

配置编译library库的脚本`build-libs.sh`：

```java
source config.sh
sh compile.sh -l -a armeabi-v7a -r
```

配置编译apk文件的脚本`build-apk.sh`：

```java
source config.sh
sh compile.sh -a armeabi-v7a
```

注意：`在这个过程中可能会下载需要的依赖和部分源码，备好翻墙梯子，确保能访问google`

如果编译`library库`，最终会输出：

![](https://github.com/byhook/ffmpeg4android/blob/master/readme/images/20181121110046640.png)

如果编译`apk文件`，最终会输出：

![](https://github.com/byhook/ffmpeg4android/blob/master/readme/images/20181121110055162.png)

运行apk文件如图：

![](https://github.com/byhook/ffmpeg4android/blob/master/readme/images/20181121110342679.png)

整个开源项目功能已经非常完善了，包括各种字幕的加载，部分流协议的支持，还有各种均衡器等等，基本可以说是开箱即用

参考：

https://wiki.videolan.org/Compile_VLC/

原文地址：

https://blog.csdn.net/byhook/article/details/84256849
