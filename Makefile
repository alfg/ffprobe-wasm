all: dist/ffprobe.js

dist/ffprobe.js:
	mkdir -p dist
	cp /opt/ffprobe-dist/* dist/
