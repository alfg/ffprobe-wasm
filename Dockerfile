FROM emscripten/emsdk:3.1.15 as build

ARG FFMPEG_VERSION=4.3.1
ARG X264_VERSION=20170226-2245-stable
ARG LAME_VERSION=3.100 

ARG PREFIX=/opt/ffmpeg
ARG MAKEFLAGS="-j4"

RUN apt-get update && apt-get install -y autoconf libtool build-essential

# libx264
RUN cd /tmp && \
  wget https://download.videolan.org/pub/videolan/x264/snapshots/x264-snapshot-${X264_VERSION}.tar.bz2 && \
  tar xvfj x264-snapshot-${X264_VERSION}.tar.bz2

RUN cd /tmp/x264-snapshot-${X264_VERSION} && \
  emconfigure ./configure \
  --prefix=${PREFIX} \
  --host=i686-gnu \
  --enable-static \
  --disable-cli \
  --disable-asm \
  --extra-cflags="-s USE_PTHREADS=1"

RUN cd /tmp/x264-snapshot-${X264_VERSION} && \
  emmake make && emmake make install 

# libmp3lame
RUN cd /tmp && \
  wget -O lame-${LAME_VERSION}.tar.gz https://sourceforge.net/projects/lame/files/lame/${LAME_VERSION}/lame-${LAME_VERSION}.tar.gz/download && \
  tar zxf lame-${LAME_VERSION}.tar.gz

RUN cd /tmp/lame-${LAME_VERSION} && \
  emconfigure ./configure \
  --prefix=${PREFIX} \
  --host=i686-gnu \
  --enable-static \
  --disable-frontend

RUN cd /tmp/lame-${LAME_VERSION} && \
  emmake make && emmake make install 

# Get ffmpeg source.
RUN cd /tmp/ && \
  wget http://ffmpeg.org/releases/ffmpeg-${FFMPEG_VERSION}.tar.gz && \
  tar zxf ffmpeg-${FFMPEG_VERSION}.tar.gz && rm ffmpeg-${FFMPEG_VERSION}.tar.gz

ARG CFLAGS="-s USE_PTHREADS=1 -O3 -I${PREFIX}/include"
ARG LDFLAGS="$CFLAGS -L${PREFIX}/lib -s INITIAL_MEMORY=33554432"

# Compile ffmpeg.
RUN cd /tmp/ffmpeg-${FFMPEG_VERSION} && \
  emconfigure ./configure \
  --prefix=${PREFIX} \
  --target-os=none \
  --arch=x86_32 \
  --enable-cross-compile \
  --disable-debug \
  --disable-x86asm \
  --disable-inline-asm \
  --disable-stripping \
  --disable-programs \
  --disable-doc \
  --disable-all \
  --enable-avcodec \
  --enable-avformat \
  --enable-avfilter \
  --enable-avdevice \
  --enable-avutil \
  --enable-swresample \
  --enable-postproc \
  --enable-swscale \
  --enable-protocol=file \
  --enable-decoder=h264,aac,pcm_s16le,mp3 \
  --enable-demuxer=mov,matroska,mp3 \
  --enable-muxer=mp4 \
  --enable-gpl \
  --enable-libx264 \
  --enable-libmp3lame \
  --extra-cflags="$CFLAGS" \
  --extra-cxxflags="$CFLAGS" \
  --extra-ldflags="$LDFLAGS" \
  --nm="llvm-nm -g" \
  --ar=emar \
  --as=llvm-as \
  --ranlib=llvm-ranlib \
  --cc=emcc \
  --cxx=em++ \
  --objcc=emcc \
  --dep-cc=emcc

RUN cd /tmp/ffmpeg-${FFMPEG_VERSION} && \
  emmake make -j4 && \
  emmake make install


COPY ./src/ffprobe-wasm-wrapper.cpp /build/src/ffprobe-wasm-wrapper.cpp
COPY ./Makefile /build/Makefile

WORKDIR /build

RUN make