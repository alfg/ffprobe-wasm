mkdir -p dist
docker build -t ffprobe-wasm .
docker create -ti --name ffprobe-wasm-container ffprobe-wasm
docker cp ffprobe-wasm-container:/build/dist/ www
docker rm -fv ffprobe-wasm-container