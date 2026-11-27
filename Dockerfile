# check=skip=FromPlatformFlagConstDisallowed
# SOUL only ships x86_64 Linux binaries.
FROM --platform=linux/amd64 ubuntu:24.04
ENV DEBIAN_FRONTEND=noninteractive

# Libraries are platform-specific: build one image per target, e.g.
# `docker build --build-arg PLATFORM=OWL3 .`
ARG PLATFORM=OWL2

RUN apt-get update && apt-get install -y --no-install-recommends \
        build-essential \
        make \
        cmake \
        git \
        ca-certificates \
        python3 \
        python3-pip \
        python3-venv \
        pipx \
        libasound2-dev \
        libcurl4-openssl-dev \
        pkg-config \
        curl \
        wget \
        unzip \
        bzip2 \
        xz-utils \
        zlib1g-dev \
        libfreetype-dev \
        libx11-dev \
        libxext-dev \
        libxinerama-dev \
        libxrandr-dev \
        libxcursor-dev \
        libgl1-mesa-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /owl

RUN wget -q -O /tmp/arm.tar.xz \
      "https://developer.arm.com/-/media/Files/downloads/gnu/13.3.rel1/binrel/arm-gnu-toolchain-13.3.rel1-x86_64-arm-none-eabi.tar.xz" \
 && mkdir -p /opt/arm \
 && tar -xJf /tmp/arm.tar.xz -C /opt/arm --strip-components=1 \
 && rm /tmp/arm.tar.xz
ENV ARMGCC=/opt/arm
ENV PATH=$ARMGCC/bin:$PATH

RUN wget -q -O /tmp/faust.tar.gz \
      "https://github.com/grame-cncm/faust/releases/download/2.88.0/faust-2.88.0.tar.gz" \
 && mkdir -p /opt/faust-src \
 && tar -xzf /tmp/faust.tar.gz -C /opt/faust-src --strip-components=1 \
 && rm /tmp/faust.tar.gz \
 && mkdir -p /opt/faust-src/build/faustdir \
 && cd /opt/faust-src/build/faustdir \
 && cmake -C ../backends/light.cmake -C ../targets/regular.cmake \
      -DCMAKE_BUILD_TYPE=Release -DINCLUDE_HTTP=off -DINCLUDE_OSC=off \
      -DINCLUDE_STATIC=off -DINCLUDE_DYNAMIC=off -DINCLUDE_EXECUTABLE=on \
      -DWORKLET=off -DLIBSDIR=lib -G 'Unix Makefiles' .. 2>&1 | tail -5 \
 && make -j"$(nproc)" 2>&1 | tail -6 \
 && make install 2>&1 | tail -4 \
 && faust --version | head -1

# SOUL is discontinued upstream (successor: Cmajor); 1.0.82 is the final release.
RUN curl -sSLf -o /tmp/soul.zip \
        https://github.com/soul-lang/SOUL/releases/download/1.0.82/binaries-linux-combined.zip \
 && unzip -o /tmp/soul.zip -d /tmp/soul \
 && install -m 0755 /tmp/soul/linux/x64/soul /usr/local/bin/soul \
 && rm -rf /tmp/soul /tmp/soul.zip \
 && soul --version 2>&1 | head -1

RUN git clone --depth 1 --branch 7.0.1 \
      https://github.com/emscripten-core/emsdk.git /opt/emsdk \
 && cd /opt/emsdk \
 && ./emsdk install 7.0.1 \
 && ./emsdk activate 7.0.1
ENV EMSDK=/opt/emsdk
ENV PATH=/opt/emsdk:/opt/emsdk/upstream/emscripten:$PATH

RUN pipx install hvcc==0.17.3 \
 && /root/.local/bin/hvcc --version 2>&1 | head -1 || pip3 install --break-system-packages hvcc==0.17.3
ENV PATH=/root/.local/bin:$PATH

RUN git clone --depth 1 https://github.com/pingdynasty/FirmwareSender.git /opt/FirmwareSender \
 && make -C /opt/FirmwareSender/Builds/Linux CONFIG=Release \
 && install -m 0755 \
        "$(find /opt/FirmwareSender/Builds/Linux/build -type f -name FirmwareSender | head -1)" \
        /usr/local/bin/FirmwareSender \
 && echo "FirmwareSender installed"

# Requires the DaisySP submodule: run `git submodule update --init` first.
COPY . /owl

RUN ln -sf /usr/local/bin/soul /owl/Tools/soul \
 && ln -sf /usr/local/bin/FirmwareSender /owl/Tools/FirmwareSender

RUN make PLATFORM=${PLATFORM} libs

RUN echo "=== toolchain versions (PLATFORM=${PLATFORM}) ===" \
 && arm-none-eabi-gcc --version | head -1 \
 && gcc --version | head -1 \
 && (faust --version 2>&1 | head -1) \
 && (soul --version 2>&1 | head -1) \
 && (emcc --version 2>&1 | head -1) \
 && (hvcc --version 2>&1 | head -1 || echo "hvcc via pipx") \
 && echo "base: ubuntu 24.04"

CMD ["make", "help"]
