# `FFProbe Web Analyzer`
> A Web-based FFProbe. Powered by FFmpeg, Vue and Web Assembly! 🦀.

https://alfg.github.io/ffprobe-web-analyzer/

⚠️ Currently functional, but a work-in-progress! Check back for updates!

## Development
`ffprobe-web-analyzer` uses [emscripten](https://emscripten.org/) to compile [FFmpeg](https://ffmpeg.org)'s [libav](https://ffmpeg.org/doxygen/4.1/index.html) to [Web Assembly](https://webassembly.org/) via [Docker](https://www.docker.com/).

Emscripten is also used to create and compile the Wasm bindings to be imported by the browser.

### Requirements
* `nodejs` - https://nodejs.org/en/download/
* `docker` - https://docs.docker.com/desktop/

### Setup 
* Clone project and build the Wasm module via Docker:
```
docker-compose run ffprobe-web-analyzer make
```

This will build the WASM module and place it into the `/dist` folder.

* Install and run web:
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
Deploys to [Github Pages](https://pages.github.com/)
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