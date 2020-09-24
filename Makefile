dist/ffmpeg-webtools.js:
	mkdir -p dist && \
	emcc -L/opt/ffmpeg/lib -I/opt/ffmpeg/include/ src/main.c \
	-s EXPORTED_FUNCTIONS='["_version", "_openfile"]' \
	-s EXTRA_EXPORTED_RUNTIME_METHODS="[FS, cwrap, ccall, setValue, writeAsciiToMemory]" \
	-s INITIAL_MEMORY=268435456 \
	-lavcodec -lavformat -lavfilter -lavdevice -lswresample -lswscale -lavutil -lm -lx264 -pthread \
	-o $@