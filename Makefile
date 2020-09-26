dist/ffmpeg-webtools.js:
	mkdir -p dist && \
	emcc --bind \
	-L/opt/ffmpeg/lib \
	-I/opt/ffmpeg/include/ \
 	-s EXTRA_EXPORTED_RUNTIME_METHODS="[FS, cwrap, ccall, getValue, setValue, writeAsciiToMemory]" \
 	-s INITIAL_MEMORY=268435456 \
	-pthread \
 	-lavcodec -lavformat -lavfilter -lavdevice -lswresample -lswscale -lavutil -lm -lx264 -pthread \
	-o www/public/ffmpeg-webtools.js \
	src/main.cpp