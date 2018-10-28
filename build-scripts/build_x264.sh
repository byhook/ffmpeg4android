#!/bin/bash

ARCH=$1

source config.sh $ARCH
LIBS_DIR=$(cd `dirname $0`; pwd)/libs/libx264
echo "LIBS_DIR="$LIBS_DIR

cd x264

PLATFORM=$ANDROID_NDK_ROOT/platforms/$AOSP_API/$AOSP_ARCH
TOOLCHAIN=$ANDROID_NDK_ROOT/toolchains/$TOOLCHAIN_BASE-$AOSP_TOOLCHAIN_SUFFIX/prebuilt/linux-x86_64

PREFIX=$LIBS_DIR/$AOSP_ABI

./configure --prefix=$PREFIX \
--disable-shared \
--enable-static \
--enable-pic \
--disable-asm \
--disable-cli \
--host=$HOST \
--cross-prefix=$TOOLCHAIN/bin/$TOOLNAME_BASE- \
--sysroot=$PLATFORM \
--extra-cflags="-Os -fpic" \
--extra-ldflags=""

make clean
make -j8
make install

cd ..

