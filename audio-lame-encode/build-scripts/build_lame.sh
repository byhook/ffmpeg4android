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

