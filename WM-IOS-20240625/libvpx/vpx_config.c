/* Copyright (c) 2011 The WebM project authors. All Rights Reserved. */
/*  */
/* Use of this source code is governed by a BSD-style license */
/* that can be found in the LICENSE file in the root of the source */
/* tree. An additional intellectual property rights grant can be found */
/* in the file PATENTS.  All contributing project authors may */
/* be found in the AUTHORS file in the root of the source tree. */
#include "vpx/vpx_codec.h"
static const char* const cfg = "--target=armv7-android-gcc --sdk-path=/c/soft/android-ndk-r14b --prefix=/usr/local/android/ --disable-examples --disable-install-docs --disable-unit-tests --extra-cflags=-mfloat-abi=softfp -mfpu=neon  --disable-debug --disable-debug-libs --disable-shared --enable-static";
const char *vpx_codec_build_config(void) {return cfg;}
