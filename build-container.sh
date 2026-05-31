#!/bin/bash
set -e

docker build --platform linux/arm64 -t alpine-sdl-builder .

docker run --rm --platform linux/arm64 -u $(id -u):$(id -g) -v "$(pwd):/workspace" alpine-sdl-builder sh -c "
    mkdir -p build/alpine/temp build/alpine/output
    rm -rf build/alpine/temp/*
    cmake -S . -B build/alpine/temp -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=/workspace/build/alpine/output
    cmake --build build/alpine/temp -j\$(nproc)
"