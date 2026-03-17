#!/bin/bash 

NDK=/android-ndk-r14b
SYSROOT=$NDK/platforms/android-17/arch-arm
TOOLCHAIN=$NDK/toolchains/arm-linux-androideabi-4.9/prebuilt/windows-x86_64
ADDI_CFLAGS="-march=armv7-a -mfpu=neon -mfloat-abi=softfp"

function build_one(){
./configure --prefix=arm-lib    --enable-shared    --enable-pic   --enable-strip \
    --cross-prefix=$TOOLCHAIN/bin/arm-linux-androideabi- \
    --host=arm-linux-androideabi \
    --sysroot=$SYSROOT \
    --extra-cflags="-Os -fpic $ADDI_CFLAGS" \
    --extra-ldflags="$ADDI_LDFLAGS" \
    $ADDITIONAL_CONFIGURE_FLAG
}

build_one
