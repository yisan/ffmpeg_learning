#!/bin/bash
make clean
export TMPDIR=/Users/ing/github/ffmp/ffmpeg-3.4.6/ffmpegtemp
# NDK的路径，根据自己的安装位置进行设置
export NDK=/Users/ing/android-ndk-r14b
export PLATFORM=$NDK/platforms/android-21/arch-arm/
export TOOLCHAIN=$NDK/toolchains/arm-linux-androideabi-4.9/prebuilt/darwin-x86_64
PREFIX=./android_single_so
function build_one
{
./configure \
--prefix=$PREFIX \
--target-os=android \
--cross-prefix=$TOOLCHAIN/bin/arm-linux-androideabi- \
--arch=arm \
--sysroot=$PLATFORM \
--extra-cflags="-I$PLATFORM/usr/include" \
--cc=$TOOLCHAIN/bin/arm-linux-androideabi-gcc \
--nm=$TOOLCHAIN/bin/arm-linux-androideabi-nm \
--disable-shared \
--disable-ffmpeg \
--disable-ffplay \
--disable-ffprobe \
--disable-ffserver \
--disable-doc \
--disable-symver \
--enable-small \
--enable-gpl \
--enable-asm \
--enable-jni \
--enable-mediacodec \
--enable-decoder=h264_mediacodec \
--enable-hwaccel=h264_mediacodec \
--enable-decoder=hevc_mediacodec \
--enable-decoder=mpeg4_mediacodec \
--enable-decoder=vp8_mediacodec \
--enable-decoder=vp9_mediacodec \
--enable-nonfree \
--enable-version3 \
--extra-cflags="-Os -fpic $ADDI_CFLAGS" \
--extra-ldflags="$ADDI_LDFLAGS" \
$ADDITIONAL_CONFIGURE_FLAG
make clean
make j8
make install
$TOOLCHAIN/bin/arm-linux-androideabi-ld \
-rpath-link=$PLATFORM/usr/lib \
-L$PLATFORM/usr/lib \
-L$PREFIX/lib \
-soname libffmpeg.so -shared -nostdlib -Bsymbolic --whole-archive --no-undefined -o \
$PREFIX/libffmpeg.so \
libavcodec/libavcodec.a \
libavfilter/libavfilter.a \
libswresample/libswresample.a \
libavformat/libavformat.a \
libavutil/libavutil.a \
libswscale/libswscale.a \
libavdevice/libavdevice.a \
libpostproc/libpostproc.a \
-lc -lm -lz -ldl -llog --dynamic-linker=/system/bin/linker \
$TOOLCHAIN/lib/gcc/arm-linux-androideabi/4.9.x/libgcc.a
}
# arm v7vfp
CPU=arm
OPTIMIZE_CFLAGS="-mfloat-abi=softfp -mfpu=vfp -marm -march=$CPU "
ADDI_CFLAGS="-marm"
echo '开始编译...'
build_one
echo '编译结束...'
