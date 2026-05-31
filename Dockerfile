FROM --platform=linux/arm64 alpine:edge

RUN apk add --no-cache \
    build-base \
    cmake \
    pkgconf \
    sdl2-dev \
    sdl2_image-dev \
    sdl2_mixer-dev \
    sdl2_ttf-dev \
    libwebp-dev \
    tiff-dev \
    libjxl-dev \
    libavif-dev \
    freetype-dev \
    harfbuzz-dev \
    flac-dev \
    libvorbis-dev \
    opusfile-dev \
    mpg123-dev

WORKDIR /workspace