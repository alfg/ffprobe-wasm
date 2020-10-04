dist/ffprobe-web-analyzer.js:
	mkdir -p dist && \
	emcc --bind \
	-O3 \
	-L/opt/ffmpeg/lib \
	-I/opt/ffmpeg/include/ \
 	-s EXTRA_EXPORTED_RUNTIME_METHODS="[FS, cwrap, ccall, getValue, setValue, writeAsciiToMemory]" \
 	-s INITIAL_MEMORY=268435456 \
	-pthread \
 	-lavcodec -lavformat -lavfilter -lavdevice -lswresample -lswscale -lavutil -lm -lx264 -pthread \
	-o www/public/ffprobe-web-analyzer.js \
	src/main.cpp