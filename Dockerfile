# OwlProgram — reproducible build environment ("last stable state")
#
# Why these versions:
#   * Ubuntu 20.04 (focal) — period-appropriate userland for the OWL "last
#     stable" era (the README targets Ubuntu 18.04; focal is the closest LTS
#     that still carries the old ARM toolchain in apt and a C++17 host gcc).
#   * arm-none-eabi-gcc 9-2019-q4 (apt: 15:9-2019-q4) — the ARM cross compiler
#     from ~2019, i.e. the "right, now-outdated gcc from 4-5 years ago". This
#     PRE-DATES the GCC 13 freestanding-mode breakage reported in upstream
#     issue #120 ("Compiling anything fails on latest arm-none-eabi-gcc"),
#     which is exactly why a pinned old toolchain is required. The project's
#     own CI pins the neighbouring 10-2020-q4 release; both build green.
#   * host gcc/g++ 9.3.0 — for the native build + unit tests (native.mk needs
#     C++17; gcc 9 supports it).
#   * Python + hvcc requirements — for Pure Data (HEAVY=) patches.
#   * emscripten / emsdk 2.0.10 (the exact version the CI pins) — for the
#     JavaScript/WASM `make web` build. This is what the OwlServer online
#     compiler (https://github.com/RebelTechnology/OwlServer) uses to turn a
#     patch into a browser-runnable build, so it is installed by default here.
#
# This image compiles ARM patches, builds the web (emscripten) patch, and runs
# the native unit tests. FAUST/SOUL compilers are still optional — see the
# notes at the bottom.

FROM ubuntu:20.04

# Avoid interactive tzdata prompts during apt installs.
ENV DEBIAN_FRONTEND=noninteractive

# ---- Toolchain + build prerequisites -------------------------------------
# build-essential  -> host gcc/g++/make (native build, unit tests)
# gcc-arm-none-eabi -> ARM cross toolchain, 2019-q4 (the pinned old GCC)
# binutils-arm-none-eabi + libnewlib -> assembler/linker + freestanding libc
# python2 / python3 + pip -> Heavy (hvcc) PD compiler
# libasound2-dev libcurl4-openssl-dev pkg-config -> FirmwareSender build deps
RUN apt-get update && apt-get install -y --no-install-recommends \
        build-essential \
        make \
        gcc-arm-none-eabi=15:9-2019-q4-0ubuntu1 \
        binutils-arm-none-eabi \
        libnewlib-arm-none-eabi \
        libstdc++-arm-none-eabi-newlib \
        git \
        ca-certificates \
        python3 \
        python3-pip \
        python2 \
        libasound2-dev \
        libcurl4-openssl-dev \
        pkg-config \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /owl

# ---- Emscripten (emsdk) 2.0.10 — pinned to match OwlProgram CI --------------
# Installed before copying the source so edits to the patch don't invalidate
# this (slow) layer. emsdk 2.0.10 ships its own clang/node, no system node
# needed. We `git checkout 2.0.10` first: the emsdk git tip now requires
# Python 3.10+, but Ubuntu 20.04 ships Python 3.8, so the old tag is what runs
# on this base image. `make web` finds emcc via the PATH set below.
RUN git clone https://github.com/emscripten-core/emsdk.git /opt/emsdk \
 && cd /opt/emsdk \
 && git checkout 2.0.10 \
 && ./emsdk install 2.0.10 \
 && ./emsdk activate 2.0.10

# Put emcc et al. on PATH for every shell so plain `make web` / `make libs`
# find emcc without a manual `source emsdk_env.sh`.
ENV EMSDK=/opt/emsdk \
    PATH=/opt/emsdk:/opt/emsdk/upstream/emscripten:/opt/emsdk/node/12.18.1_64bit/bin:$PATH

# Copy the already-checked-out source (submodules included) into the image.
COPY . /owl

# Point the ARM Makefiles at the apt-installed cross toolchain. The apt
# package puts arm-none-eabi-* on PATH under /usr/bin, so TOOLROOT is /usr/bin/
# (trailing slash required by compile.mk: CC=$(TOOLROOT)arm-none-eabi-gcc).
ENV TOOLROOT=/usr/bin/

# Install the Heavy (hvcc) Python requirements so `make check` can build PD
# patches. hvcc of this era runs under Python; use pip3. Non-fatal if the
# pinned deps have drifted — C++/native builds don't need them.
RUN pip3 install --no-cache-dir -r Tools/hvcc/requirements.txt || true

# Record the toolchain versions in the image for provenance.
RUN echo "=== Toolchain versions ===" \
 && arm-none-eabi-gcc --version | head -1 \
 && gcc --version | head -1 \
 && make --version | head -1

# Default: show the build help. Override with e.g.
#   docker run --rm owlprogram:stable make PATCHNAME=Template patch
CMD ["make", "help"]

# ---------------------------------------------------------------------------
# Optional extras (not installed to keep the image lean and the build fast):
#
#  FAUST patches (make FAUST=...):  install faust + faust2owl.
#  SOUL  patches (make SOUL=...):   install the soul compiler.
#
#  FirmwareSender (make load/store/sysex to a real device):
#     git clone https://github.com/pingdynasty/FirmwareSender \
#       && make -C FirmwareSender/Builds/Linux \
#       && cp FirmwareSender/Builds/Linux/build/FirmwareSender Tools/
#     (device upload needs USB/MIDI passthrough into the container.)
#
# Already included: ARM cross build (patch), native build + unit tests, and
# the emscripten web build (make web / make libs) used by OwlServer.
# ---------------------------------------------------------------------------
