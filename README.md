# `FFProbe Wasm`
> A Web-based FFProbe. Powered by FFmpeg, Vue and Web Assembly!

https://ffprobe-wasm.netlify.app/

## Development
`ffprobe-wasm` uses [Emscripten](https://emscripten.org/) to compile FFmpeg 8.0.1's libraries and the real `ffprobe` program to [WebAssembly](https://webassembly.org/) via [Docker](https://www.docker.com/).

Emscripten is also used to create and compile the Wasm bindings to be imported by the browser.

## Build size

The default build is optimized for download size:

* `-Oz` and link-time optimization are enabled.
* FFmpeg networking, threading, runtime CPU detection, iconv, and automatic
  dependency detection are disabled.
* The legacy bindings and raw `ffprobe` API share one Wasm module.
* Only MP4/MOV, Matroska/WebM, MP3, and FLV demuxers plus H.264, VP8, VP9,
  AAC, Vorbis, PCM S16LE, MP3, and FLV decoders are included. Opus parsing is
  enabled, but Opus decoding is left out to avoid pulling in `swresample`.

Current uncompressed artifacts are approximately:

| Artifact | Size |
| --- | ---: |
| `ffprobe.wasm` | 1.88 MiB |

With gzip enabled, the Wasm transfer is approximately 789 KiB.

Serve `.wasm` and `.js` with Brotli or gzip enabled. Adding formats or codecs
to the Docker build increases these sizes.

## Raw ffprobe output

The worker accepts `get_raw_info` and returns the full output produced by:

```sh
ffprobe -hide_banner -loglevel fatal \
  -show_format -show_streams -show_chapters \
  -show_programs -show_stream_groups -show_private_data \
  -print_format json input
```

Pass `json` to receive a parsed object, or `string` to receive the original JSON text:

```js
const worker = new Worker('ffprobe-worker.js');

worker.onmessage = ({ data }) => {
  console.log(data);
};

worker.postMessage(['get_raw_info', file, 'json']);
worker.postMessage(['get_raw_info', file, 'string']);
```

Create a separate worker, or wait for each response, when requesting both forms at once.

### Requirements
* `nodejs` - https://nodejs.org/en/download/
* `docker` - https://docs.docker.com/desktop/

### Setup 
* Clone project and build the Wasm module via Docker:
```
docker-compose run ffprobe-wasm make
```

This will build the Wasm module and place it into the `/dist` directory.

Verify both the FFmpeg 8 wrapper and raw ffprobe JSON with:

```sh
node test/raw-output.cjs /path/to/input.mp4
```

* Copy the JS and Wasm modules into `www/public/`:
```
cp -a dist/. www/public/
```

* Install and run the web application:
```
cd www
npm install
npm run serve
```

* Load `http://localhost:8080/` in the web browser.

### Compiles and minifies for production
```
npm run build
```

### Deploy
Builds and deploys to `gh-pages` branch.

The default single-threaded build does not require `SharedArrayBuffer` or
cross-origin isolation headers.
```
npm run deploy
```

### Resources
* https://ffmpeg.org/doxygen/8.0/index.html
* https://emscripten.org/
* https://vuejs.org
* https://bootstrap-vue.org

## License
MIT
