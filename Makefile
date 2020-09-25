# dist/ffmpeg-webtools.js:
# 	mkdir -p dist && \
# 	emcc -L/opt/ffmpeg/lib -I/opt/ffmpeg/include/ src/main.c \
# 	-s EXPORTED_FUNCTIONS='["_c_avformat_version", "_openfile", "_addOne"]' \
# 	-s EXTRA_EXPORTED_RUNTIME_METHODS="[FS, cwrap, ccall, getValue, setValue, writeAsciiToMemory]" \
# 	-s INITIAL_MEMORY=268435456 \
# 	-lavcodec -lavformat -lavfilter -lavdevice -lswresample -lswscale -lavutil -lm -lx264 -pthread \
# 	-o www/public/ffmpeg-webtools.js

dist/ffmpeg-webtools.js:
	mkdir -p dist && \
	emcc  \
	--bind \
	-L/opt/ffmpeg/lib \
	-I/opt/ffmpeg/include/ \
 	-s EXTRA_EXPORTED_RUNTIME_METHODS="[FS, cwrap, ccall, getValue, setValue, writeAsciiToMemory]" \
 	-s INITIAL_MEMORY=268435456 \
	-pthread \
 	-lavcodec -lavformat -lavfilter -lavdevice -lswresample -lswscale -lavutil -lm -lx264 -pthread \
	-o www/public/ffmpeg-webtools.js \
	src/main.cpp