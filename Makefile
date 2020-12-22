dist/ffprobe-wasm.js:
	mkdir -p dist && \
	emcc --bind \
	-O3 \
	-L/opt/ffmpeg/lib \
	-I/opt/ffmpeg/include/ \
	-s EXTRA_EXPORTED_RUNTIME_METHODS="[FS, cwrap, ccall, getValue, setValue, writeAsciiToMemory]" \
	-s INITIAL_MEMORY=268435456 \
	-lavcodec -lavformat -lavfilter -lavdevice -lswresample -lswscale -lavutil -lm -lx264 \
	-pthread \
	-lworkerfs.js \
	-o dist/ffprobe-wasm.js \
	src/ffprobe-wasm-wrapper.cpp