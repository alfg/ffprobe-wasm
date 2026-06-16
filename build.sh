#!/bin/sh

set -eu

image=ffprobe-wasm
container=ffprobe-wasm-container

rm -rf dist
mkdir -p dist
docker build -t "$image" .
docker create --name "$container" "$image"
trap 'docker rm -f "$container" >/dev/null 2>&1 || true' EXIT
docker cp "$container:/build/dist/." dist/
