mkdir -p dist
docker build -t ffprobe-web-analyzer .
docker create -ti --name ffprobe-web-analyzer-container ffprobe-web-analyzer
docker cp ffprobe-web-analyzer-container:/build/dist/ .
docker rm -fv ffprobe-web-analyzer-container