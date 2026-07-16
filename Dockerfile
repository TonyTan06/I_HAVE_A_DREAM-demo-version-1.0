FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    ca-certificates \
    cmake \
    fluxbox \
    git \
    libasound2-dev \
    libgl1-mesa-dev \
    libglu1-mesa-dev \
    libxcursor-dev \
    libxi-dev \
    libxinerama-dev \
    libxkbcommon-dev \
    libxrandr-dev \
    libx11-dev \
    mesa-utils \
    novnc \
    pkg-config \
    python3 \
    websockify \
    x11vnc \
    xvfb \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY . .

RUN cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Release \
    -DENABLE_TESTS=OFF \
    && cmake --build build --config Release --parallel

EXPOSE 6080

ENV DISPLAY=:99
ENV LIBGL_ALWAYS_SOFTWARE=1
ENV MESA_LOADER_DRIVER_OVERRIDE=llvmpipe
ENV SCREEN_WIDTH=1280
ENV SCREEN_HEIGHT=720

CMD ["./docker/run-game.sh"]
