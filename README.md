# `FFProbe Wasm`
> A Web-based FFProbe. Powered by FFmpeg, Vue and Web Assembly!

https://ffprobe-wasm.netlify.app/

## Development
`ffprobe-wasm` uses [emscripten](https://emscripten.org/) to compile [FFmpeg](https://ffmpeg.org)'s [libav](https://ffmpeg.org/doxygen/4.1/index.html) to [Web Assembly](https://webassembly.org/) via [Docker](https://www.docker.com/).

Emscripten is also used to create and compile the Wasm bindings to be imported by the browser.

### Requirements
* `nodejs` - https://nodejs.org/en/download/
* `docker` - https://docs.docker.com/desktop/

### Setup 
* Clone project and build the Wasm module via Docker:
```
docker-compose run ffprobe-wasm make
```

This will build the Wasm module and place it into the `/dist` directory.

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

However, I am hosting on [Netlify](https://netlify.com) to enable [SharedArrayBuffer](https://caniuse.com/sharedarraybuffer) support via the [required CORS headers](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/SharedArrayBuffer).
```
npm run deploy
```

### Resources
* https://ffmpeg.org/doxygen/4.1/index.html
* https://emscripten.org/
* https://vuejs.org
* https://bootstrap-vue.org

## License
MIT
