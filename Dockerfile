FROM emscripten/emsdk:3.1.15 AS build

ARG FFMPEG_VERSION=8.0.1
ARG PREFIX=/opt/ffmpeg
ARG MAKEFLAGS="-j4"

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    ca-certificates \
    xz-utils \
    && rm -rf /var/lib/apt/lists/*

RUN cd /tmp && \
    wget --tries=5 --timeout=30 --retry-connrefused \
      -O ffmpeg-${FFMPEG_VERSION}.tar.xz \
      https://ffmpeg.org/releases/ffmpeg-${FFMPEG_VERSION}.tar.xz && \
    echo "05ee0b03119b45c0bdb4df654b96802e909e0a752f72e4fe3794f487229e5a41  ffmpeg-${FFMPEG_VERSION}.tar.xz" | sha256sum -c - && \
    tar xf ffmpeg-${FFMPEG_VERSION}.tar.xz && \
    rm ffmpeg-${FFMPEG_VERSION}.tar.xz

ARG CFLAGS="-Oz -flto -DNDEBUG -I${PREFIX}/include"
ARG LDFLAGS="-Oz -flto -L${PREFIX}/lib -sINITIAL_MEMORY=33554432 -sMALLOC=emmalloc"
ARG FFPROBE_LDEXEFLAGS="-Oz -flto --bind -lc++ -lc++abi -sMODULARIZE=1 -sEXPORT_NAME=createFFprobe -sINVOKE_RUN=0 -sEXIT_RUNTIME=0 -sALLOW_MEMORY_GROWTH=1 -sINITIAL_MEMORY=33554432 -sMALLOC=emmalloc -sEXPORTED_RUNTIME_METHODS=FS,callMain -lworkerfs.js"

RUN cd /tmp/ffmpeg-${FFMPEG_VERSION} && \
    emconfigure ./configure \
    --prefix=${PREFIX} \
    --target-os=none \
    --arch=x86_32 \
    --enable-cross-compile \
    --disable-debug \
    --enable-small \
    --disable-x86asm \
    --disable-inline-asm \
    --disable-stripping \
    --disable-autodetect \
    --disable-network \
    --disable-runtime-cpudetect \
    --disable-pthreads \
    --disable-w32threads \
    --disable-os2threads \
    --disable-iconv \
    --progs-suffix=.js \
    --disable-programs \
    --disable-doc \
    --disable-all \
    --enable-ffprobe \
    --enable-avcodec \
    --enable-avformat \
    --enable-avutil \
    --enable-protocol=file \
    --enable-decoder=h264,aac,pcm_s16le,mp3,vp8,vp9,opus,vorbis,flv \
    --enable-parser=h264,aac,mpegaudio,vp8,vp9,opus,vorbis \
    --enable-demuxer=mov,matroska,mp3,flv \
    --extra-cflags="$CFLAGS" \
    --extra-cxxflags="$CFLAGS" \
    --extra-ldflags="$LDFLAGS" \
    --extra-ldexeflags="$FFPROBE_LDEXEFLAGS" \
    --nm="llvm-nm -g" \
    --ar=emar \
    --as=llvm-as \
    --ranlib=llvm-ranlib \
    --cc=emcc \
    --cxx=em++ \
    --objcc=emcc \
    --dep-cc=emcc

COPY ./src/ffprobe-wasm-wrapper.cpp /tmp/ffprobe-wasm-wrapper.cpp

RUN cd /tmp/ffmpeg-${FFMPEG_VERSION} && \
    cp /tmp/ffprobe-wasm-wrapper.cpp fftools/ffprobe-wasm-wrapper.cpp && \
    em++ -c -Oz -flto -DNDEBUG -std=c++11 -I. \
      fftools/ffprobe-wasm-wrapper.cpp -o fftools/ffprobe-wasm-wrapper.o && \
    printf '\nOBJS-ffprobe += fftools/ffprobe-wasm-wrapper.o\n' >> fftools/Makefile && \
    emmake make && \
    mkdir -p /opt/ffprobe-dist && \
    cp ffprobe.js ffprobe.wasm /opt/ffprobe-dist/

COPY ./Makefile /build/Makefile

WORKDIR /build

RUN make
