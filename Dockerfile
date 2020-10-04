FROM emscripten/emsdk as build

ARG FFMPEG_VERSION=4.3.1
ARG X264_VERSION=20170226-2245-stable

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

# Get ffmpeg source.
RUN cd /tmp/ && \
  wget http://ffmpeg.org/releases/ffmpeg-${FFMPEG_VERSION}.tar.gz && \
  tar zxf ffmpeg-${FFMPEG_VERSION}.tar.gz && rm ffmpeg-${FFMPEG_VERSION}.tar.gz

ARG CFLAGS="-s USE_PTHREADS -O3 -I${PREFIX}/include"
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
  --enable-decoder=h264,aac,pcm_s16le \
  --enable-demuxer=mov \
  --enable-muxer=mp4 \
  --enable-gpl \
  --enable-libx264 \
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

# RUN	cd /build && \
#   emcc -L/opt/ffmpeg/lib -I/opt/ffmpeg/include/ src/main.c \
# 	-s EXPORTED_FUNCTIONS='["_version", "_openfile"]' \
# 	-s EXTRA_EXPORTED_RUNTIME_METHODS="[FS, cwrap, ccall, setValue, writeAsciiToMemory]" \
# 	-s INITIAL_MEMORY=268435456 \
# 	-lavcodec -lavformat -lavfilter -lavdevice -lswresample -lswscale -lavutil -lm -lx264 -pthread \
# 	-o ffprobe-web-analyzer.js

WORKDIR /build
# COPY ./src/main.c /build/src/main.c
COPY ./Makefile /build/Makefile
# RUN make