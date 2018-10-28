#!/bin/bash

ARCH=$1

source config.sh $ARCH
LIBS_DIR=$(cd `dirname $0`; pwd)/libs/libfdk-aac
echo "LIBS_DIR="$LIBS_DIR

cd fdk-aac-0.1.6

PLATFORM=$ANDROID_NDK_ROOT/platforms/$AOSP_API/$AOSP_ARCH
TOOLCHAIN=$ANDROID_NDK_ROOT/toolchains/$TOOLCHAIN_BASE-$AOSP_TOOLCHAIN_SUFFIX/prebuilt/linux-x86_64
CROSS_COMPILE=$TOOLCHAIN/bin/$TOOLNAME_BASE-
SYSROOT=$ANDROID_NDK_ROOT/platforms/$AOSP_API/$AOSP_ARCH

PREFIX=$LIBS_DIR/$AOSP_ABI

CFLAGS=""

FLAGS="--enable-static  --host=$HOST --target=android --disable-asm "

export CXX="${CROSS_COMPILE}g++ --sysroot=${SYSROOT}"
export LDFLAGS=" -L$SYSROOT/usr/lib  $CFLAGS "
export CXXFLAGS=$CFLAGS
export CFLAGS=$CFLAGS
export CC="${CROSS_COMPILE}gcc --sysroot=${SYSROOT}"
export AR="${CROSS_COMPILE}ar"
export LD="${CROSS_COMPILE}ld"
export AS="${CROSS_COMPILE}gcc"


./configure $FLAGS \
--enable-pic \
--enable-strip \
--prefix=$PREFIX

$ADDITIONAL_CONFIGURE_FLAG

make clean
make -j8
make install

cd ..

