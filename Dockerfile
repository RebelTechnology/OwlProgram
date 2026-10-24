# OwlProgram — v3 MODERNIZATION image
#
# v3 brings every toolchain axis up to date on a modern base image. It descends
# from Dockerfile.live (v2 / current live server) but moves each dependency
# forward. See baseline/V3_SCOPE.md and baseline/V3_TRACKER.md.
#
# Axis status baked into THIS file (iterative — each axis is refined as it lands):
#   0 base image : ubuntu 20.04 -> 24.04
#   1 arm-gcc    : 9-2020-q2 -> Arm GNU 13.3.rel1  (LINKER SCRIPTS: see note)
#   4 emscripten : 2.0.10 -> 3.1.x                 (web.mk flags: see note)
#   5 hvcc       : vendored py2.7 fork -> pip hvcc (mainline, -g owl)
#   3 FAUST      : 2.30.5 -> 2.74.x                (owl.cpp wrapper: see note)
#   6 SOUL       : frozen at 1.0.82
#
# This is WORK IN PROGRESS: it establishes the modern base so each axis can be
# verified against the patch matrix. Build on the feature/v3-modernization tree.

# The ARM GNU cross-compiler (13.3.rel1) and SOUL binaries are downloaded as
# x86_64, so pin the platform to linux/amd64. On an arm64 host (Apple Silicon)
# this builds under Docker Desktop's emulation; the cross-compiler emits ARM
# Cortex-M code regardless of build-host arch.
FROM --platform=linux/amd64 ubuntu:24.04
ENV DEBIAN_FRONTEND=noninteractive

# Target OWL platform the baked-in libraries are built for. Override at build
# time, e.g. `docker build --build-arg PLATFORM=OWL3 -t owlprogram:owl3-v3 .`.
# The libs are platform-specific (Cortex-M4 vs M7), so each image is pinned to
# ONE platform; build a separate image per target.
ARG PLATFORM=OWL2

# ---- Base prerequisites (24.04: python3 is default, host gcc is 13) --------
# No python2 (axis 5 removes the py2.7 hvcc dependency). binutils/newlib come
# from the pinned arm-gcc tarball below, not apt, so only host build deps here.
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

# ---- Axis 1: ARM GNU Toolchain 13.3.rel1 -----------------------------------
# The modern Arm GNU Toolchain (GCC 13). NOTE: GCC >=12 changed freestanding
# section placement + ld orphan-section handling — OwlProgram's linker scripts
# (*.ld) need rework (issue #120). This layer installs the toolchain; the
# linker-script fixes land as source edits on feature/v3-modernization.
RUN wget -q -O /tmp/arm.tar.xz \
      "https://developer.arm.com/-/media/Files/downloads/gnu/13.3.rel1/binrel/arm-gnu-toolchain-13.3.rel1-x86_64-arm-none-eabi.tar.xz" \
 && mkdir -p /opt/arm \
 && tar -xJf /tmp/arm.tar.xz -C /opt/arm --strip-components=1 \
 && rm /tmp/arm.tar.xz
ENV ARMGCC=/opt/arm
ENV PATH=$ARMGCC/bin:$PATH

# ---- Axis 3: FAUST 2.74.x from source --------------------------------------
# Same light-backend direct-cmake path as v2 (CPP backend, no LLVM). NOTE: the
# generated-class API may differ from 2.30.5 — FaustSource/owl.cpp is re-audited
# on the branch (axis 3). Pin bumped from 2.30.5.
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

# ---- Axis 6: SOUL 1.0.82 (FROZEN — discontinued upstream, successor Cmajor) -
RUN curl -sSLf -o /tmp/soul.zip \
        https://github.com/soul-lang/SOUL/releases/download/1.0.82/binaries-linux-combined.zip \
 && unzip -o /tmp/soul.zip -d /tmp/soul \
 && install -m 0755 /tmp/soul/linux/x64/soul /usr/local/bin/soul \
 && rm -rf /tmp/soul /tmp/soul.zip \
 && soul --version 2>&1 | head -1

# ---- Axis 4: Emscripten 3.1.x ---------------------------------------------
# Big jump from 2.0.10. NOTE: web.mk emcc flags + JS glue need updating (STRICT,
# EXPORTED_RUNTIME_METHODS syntax, Node floor). emsdk ships its own clang/node.
RUN git clone https://github.com/emscripten-core/emsdk.git /opt/emsdk \
 && cd /opt/emsdk \
 && ./emsdk install 6.0.13 \
 && ./emsdk activate 6.0.13
ENV EMSDK=/opt/emsdk
ENV PATH=/opt/emsdk:/opt/emsdk/upstream/emscripten:$PATH

# ---- Axis 5: hvcc — mainline Wasted-Audio via pip (python3) ----------------
# Replaces the vendored Tools/hvcc py2.7 submodule + build_send_receive_constants.py.
# heavy.mk on the v3 branch calls `hvcc -g owl` (see owl-hvcc-migration/migrate).
RUN pipx install hvcc==0.17.3 \
 && /root/.local/bin/hvcc --version 2>&1 | head -1 || pip3 install --break-system-packages hvcc==0.17.3
ENV PATH=/root/.local/bin:$PATH

# ---- Axis 7: FirmwareSender (rebuilt on 24.04 / newer JUCE deps) -----------
RUN git clone --depth 1 https://github.com/pingdynasty/FirmwareSender.git /opt/FirmwareSender \
 && make -C /opt/FirmwareSender/Builds/Linux CONFIG=Release \
 && install -m 0755 \
        "$(find /opt/FirmwareSender/Builds/Linux/build -type f -name FirmwareSender | head -1)" \
        /usr/local/bin/FirmwareSender \
 && echo "FirmwareSender installed"

# ---- Source ----------------------------------------------------------------
COPY . /owl

# arm-gcc is on PATH via /opt/arm/bin, so compile.mk's default TOOLROOT (empty =>
# PATH lookup) works; no Tools/ symlink needed for v3. Keep the toolchain
# explicit for clarity.
RUN ln -sf /usr/local/bin/soul /owl/Tools/soul \
 && ln -sf /usr/local/bin/FirmwareSender /owl/Tools/FirmwareSender

# Pre-build the patch libraries for the target platform so a later
# `make PATCHNAME=X patch` links immediately (no separate `make libs` needed).
# `make libs` needs only PLATFORM; the archives are platform-specific and live
# at non-platform-keyed paths, so PLATFORM is fixed per image.
RUN make PLATFORM=${PLATFORM} libs

# Provenance.
RUN echo "=== v3 toolchain versions (PLATFORM=${PLATFORM}) ===" \
 && arm-none-eabi-gcc --version | head -1 \
 && gcc --version | head -1 \
 && (faust --version 2>&1 | head -1) \
 && (soul --version 2>&1 | head -1) \
 && (emcc --version 2>&1 | head -1) \
 && (hvcc --version 2>&1 | head -1 || echo "hvcc via pipx") \
 && echo "base: ubuntu 24.04"

CMD ["make", "help"]
