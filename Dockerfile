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
# This image compiles ARM patches, builds the web (emscripten) patch, builds
# FAUST and SOUL patches, runs the native unit tests, and bundles
# FirmwareSender for MIDI sysex packaging / device upload. See the usage notes
# at the bottom.

FROM ubuntu:20.04

# Avoid interactive tzdata prompts during apt installs.
ENV DEBIAN_FRONTEND=noninteractive

# ---- Toolchain + build prerequisites -------------------------------------
# build-essential  -> host gcc/g++/make (native build, unit tests)
# gcc-arm-none-eabi -> ARM cross toolchain, 2019-q4 (the pinned old GCC)
# binutils-arm-none-eabi + libnewlib -> assembler/linker + freestanding libc
# python2 / python3 + pip -> Heavy (hvcc) PD compiler
# libasound2-dev libcurl4-openssl-dev pkg-config -> FirmwareSender build deps
# libfreetype6-dev libx11-dev libxext-dev libxinerama-dev libxrandr-dev
# libxcursor-dev libgl1-mesa-dev -> JUCE module compile deps pulled in by
#   FirmwareSender's juce_audio_devices/juce_events modules (even though the
#   app itself is a headless ConsoleApp, the JUCE sources #include these).
# faust -> FAUST DSP compiler for `make FAUST=...` patches (focal ships
#          2.20.2, a 2019-era release that matches the "last stable" window).
#          OwlProgram's faust.mk calls the `faust` binary directly with its
#          own FaustSource/owl.cpp architecture file, so no faust2owl needed.
# curl unzip -> fetch + unpack the SOUL compiler release (binary distribution)
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
        faust \
        curl \
        unzip \
        libfreetype6-dev \
        libx11-dev \
        libxext-dev \
        libxinerama-dev \
        libxrandr-dev \
        libxcursor-dev \
        libgl1-mesa-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /owl

# ---- Emscripten (emsdk) 2.0.10 — pinned to match OwlProgram CI --------------
# Installed before copying the source so edits to the patch don't invalidate
# this (slow) layer. emsdk 2.0.10 ships its own clang/node, no system node
# needed. We `git checkout 2.0.10` first: the emsdk git tip now requires
# Python 3.10+, but Ubuntu 20.04 ships Python 3.8, so the old tag is what runs
# on this base image. `make web` finds emcc via the PATH set below.
#
# We install by the explicit SDK *hash* rather than the `2.0.10` version alias.
# The alias path resolves through emscripten-releases-tags and has intermittently
# failed with "No tool or SDK found by name 'sdk-releases-upstream-...'" when the
# release-tags lookup goes stale; installing the resolved SDK name directly skips
# that indirection. 37fc7647... is the release hash the 2.0.10 tag maps to.
RUN git clone https://github.com/emscripten-core/emsdk.git /opt/emsdk \
 && cd /opt/emsdk \
 && git checkout 2.0.10 \
 && EMSDK_HASH=sdk-releases-upstream-37fc7647c754ac9a28ad588c143b82286de0ef71-64bit \
 && ( ./emsdk install "$EMSDK_HASH" || ./emsdk install 2.0.10 ) \
 && ./emsdk activate 2.0.10

# Put emcc et al. on PATH for every shell so plain `make web` / `make libs`
# find emcc without a manual `source emsdk_env.sh`.
ENV EMSDK=/opt/emsdk \
    PATH=/opt/emsdk:/opt/emsdk/upstream/emscripten:/opt/emsdk/node/12.18.1_64bit/bin:$PATH

# ---- SOUL compiler 1.0.82 — for `make SOUL=...` patches --------------------
# ROLI/soul-lang shipped SOUL as a prebuilt binary (the compiler is not
# open-source-buildable). 1.0.82 is the last 1.0.x release (2020), matching
# the "last stable" era. The release zip (binaries-linux-combined.zip) carries
# per-arch CLIs under linux/{x64,arm32,arm64,bela}/soul — we must pick the x64
# one (a bare `find -name soul` can grab an ARM build that won't exec here).
# OwlProgram's soul.mk invokes it as `Tools/soul generate --cpp ...`, so we
# drop the binary at /usr/local/bin/soul AND symlink it into the source tree's
# Tools/ after COPY (see below). Network fetch is confined to this layer so it
# doesn't invalidate the source COPY layer.
RUN curl -sSLf -o /tmp/soul.zip \
        https://github.com/soul-lang/SOUL/releases/download/1.0.82/binaries-linux-combined.zip \
 && unzip -o /tmp/soul.zip -d /tmp/soul \
 && install -m 0755 /tmp/soul/linux/x64/soul /usr/local/bin/soul \
 && rm -rf /tmp/soul /tmp/soul.zip \
 && soul --version 2>&1 | head -1

# ---- FirmwareSender — for `make load/store/sysex` device upload ------------
# Built from pingdynasty/FirmwareSender (JUCE app). Needs ALSA + curl dev libs
# (installed above). Produces a CLI that packs a patch .bin into MIDI sysex and
# sends it to an attached OWL device over USB-MIDI. The OwlProgram Makefile
# expects it at Tools/FirmwareSender; we build to /usr/local/bin/FirmwareSender
# and symlink into Tools/ after COPY. Actual device upload also needs USB/MIDI
# passthrough into the container (e.g. `docker run --device=/dev/snd ...`).
RUN git clone --depth 1 https://github.com/pingdynasty/FirmwareSender.git /opt/FirmwareSender \
 && make -C /opt/FirmwareSender/Builds/Linux CONFIG=Release \
 && install -m 0755 \
        "$(find /opt/FirmwareSender/Builds/Linux/build -type f -name FirmwareSender | head -1)" \
        /usr/local/bin/FirmwareSender \
 && FirmwareSender --help >/dev/null 2>&1; echo "FirmwareSender installed"

# Copy the already-checked-out source (submodules included) into the image.
COPY . /owl

# Wire the SOUL + FirmwareSender binaries into the paths OwlProgram's Makefiles
# expect (Tools/soul and Tools/FirmwareSender), pointing at the versions
# installed above. Symlinks keep the single source of truth in /usr/local/bin.
RUN ln -sf /usr/local/bin/soul /owl/Tools/soul \
 && ln -sf /usr/local/bin/FirmwareSender /owl/Tools/FirmwareSender

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
 && make --version | head -1 \
 && (faust --version 2>&1 | head -1) \
 && (soul --version 2>&1 | head -1) \
 && (echo "FirmwareSender: $(ls -1 /usr/local/bin/FirmwareSender)")

# Default: show the build help. Override with e.g.
#   docker run --rm owlprogram:stable make PATCHNAME=Template patch
CMD ["make", "help"]

# ---------------------------------------------------------------------------
# What this image can build:
#
#   ARM patch (default OWL2/OWL3):   make PATCHNAME=Template patch
#   Native build + unit tests:       make test / make check
#   Web (emscripten, OwlServer):     make web / make libs
#   FAUST patch:                     make PATCHSOURCE=<dir> FAUST=<Name> patch
#   SOUL patch:                      make PATCHSOURCE=<dir> SOUL=<Name> patch
#   Heavy / Pure Data patch:         make HEAVY=<Name> patch
#
# Device upload with FirmwareSender (make load / store / sysex) needs the OWL
# connected over USB-MIDI AND passed into the container, e.g.:
#   docker run --rm -it --device=/dev/snd \
#     -v /dev/bus/usb:/dev/bus/usb owlprogram:stable \
#     make PATCHNAME=Template OWLDEVICE="OWL-*" load
#
# Installed compilers/tools:
#   * host gcc/g++ 9        (native build + unit tests)
#   * arm-none-eabi-gcc 9   (ARM cross build — the pinned old toolchain)
#   * emscripten 2.0.10     (web build used by OwlServer)
#   * faust 2.20.2          (FAUST DSP patches)
#   * soul 1.0.82           (SOUL patches — prebuilt ROLI binary)
#   * FirmwareSender        (MIDI sysex packaging + device upload)
#   * hvcc / Heavy          (Pure Data patches)
# ---------------------------------------------------------------------------
