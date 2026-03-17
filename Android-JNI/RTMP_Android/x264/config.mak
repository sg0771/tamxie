SRCPATH=.
prefix=arm-lib
exec_prefix=${prefix}
bindir=${exec_prefix}/bin
libdir=${exec_prefix}/lib
includedir=${prefix}/include
SYS_ARCH=ARM
SYS=LINUX
CC=/android-ndk-r14b/toolchains/arm-linux-androideabi-4.9/prebuilt/windows-x86_64/bin/arm-linux-androideabi-gcc
CFLAGS=-Wno-maybe-uninitialized -Wshadow -O2 -ffast-math  -Wall -I. -I$(SRCPATH) --sysroot=/android-ndk-r14b/platforms/android-17/arch-arm -Os -fpic -march=armv7-a -mfpu=neon -mfloat-abi=softfp -std=gnu99 -D_GNU_SOURCE -fPIC -fomit-frame-pointer -fno-tree-vectorize
COMPILER=GNU
COMPILER_STYLE=GNU
DEPMM=-MM -g0
DEPMT=-MT
LD=/android-ndk-r14b/toolchains/arm-linux-androideabi-4.9/prebuilt/windows-x86_64/bin/arm-linux-androideabi-gcc -o 
LDFLAGS= --sysroot=/android-ndk-r14b/platforms/android-17/arch-arm  -lm  -s -ldl
LIBX264=libx264.a
AR=/android-ndk-r14b/toolchains/arm-linux-androideabi-4.9/prebuilt/windows-x86_64/bin/arm-linux-androideabi-gcc-ar rc 
RANLIB=/android-ndk-r14b/toolchains/arm-linux-androideabi-4.9/prebuilt/windows-x86_64/bin/arm-linux-androideabi-gcc-ranlib
STRIP=/android-ndk-r14b/toolchains/arm-linux-androideabi-4.9/prebuilt/windows-x86_64/bin/arm-linux-androideabi-strip
INSTALL=install
AS=/android-ndk-r14b/toolchains/arm-linux-androideabi-4.9/prebuilt/windows-x86_64/bin/arm-linux-androideabi-gcc
ASFLAGS= -I. -I$(SRCPATH) -c -DSTACK_ALIGNMENT=4 -DPIC -DHIGH_BIT_DEPTH=0 -DBIT_DEPTH=8
RC=
RCFLAGS=
EXE=
HAVE_GETOPT_LONG=1
DEVNULL=/dev/null
PROF_GEN_CC=-fprofile-generate
PROF_GEN_LD=-fprofile-generate
PROF_USE_CC=-fprofile-use
PROF_USE_LD=-fprofile-use
HAVE_OPENCL=yes
default: cli
install: install-cli
SOSUFFIX=so
SONAME=libx264.so.148
SOFLAGS=-shared -Wl,-soname,$(SONAME)  -Wl,-Bsymbolic
default: lib-shared
install: install-lib-shared
LDFLAGSCLI = -ldl 
CLI_LIBX264 = $(LIBX264)
