# check=skip=FromPlatformFlagConstDisallowed
# OwlProgram build image — v2 (live server toolchain)
# Revision of the v1 Dockerfile: updated DSP library era — ARM GCC 9-2020-q2,
# FAUST 2.30.5, owl.cpp new/delete. See git history for the v1->v2 delta.
#
# This image reproduces the toolchain actually running on the production online
# compiler host (bitnami@ip-172-31-2-241:/opt/OwlProgram) that serves
# https://www.rebeltech.org/patch-library/patches/latest/ — captured
# 2026-10-23 (see baseline/LIVE_SERVER.md).
#
# It differs from the pinned "last stable" baseline (./Dockerfile) in three
# deliberate ways, each matching what the live host runs rather than what the
# repo commits:
#
#   1. ARM GCC 9-2020-q2-update  (NOT apt 9-2019-q4). The live host unpacks the
#      ARM-published toolchain into Tools/gcc-arm-none-eabi-9-2020-q2-update/
#      and uncomments compile.mk's `TOOLROOT ?= Tools/.../bin/`.
#   2. FAUST 2.30.5  (NOT apt 2.20.2). Built from source (LLVM 11). The live
#      host's `faust --version` is 2.30.5 (2021, LLVM 11.0.1).
#   3. FaustSource/owl.cpp patched to `new mydsp()` / `delete fDSP` — required
#      because FAUST 2.30.x stopped emitting the static mydsp::create()/destroy()
#      factory. The live host carries this as an uncommitted local edit.
#
# Build it on a feature/dsp-updates worktree (the live-server branch):
#   docker build -f Dockerfile.live -t owlprogram:live <dsp-updates-worktree>
#
# NOTE: the FAUST-from-source layer is slow (~15-25 min). Everything else is fast.

# The pinned toolchain (emsdk 2.0.10, and in v2 the ARM-published GCC) ships
# ONLY x86_64 binaries, so we pin the platform to linux/amd64. On an arm64 host
# (Apple Silicon) this builds under Docker Desktop's emulation; without the pin,
# emsdk fails with "No tool or SDK found by name 'sdk-releases-upstream-...'"
# because the 2.0.10 tag has no arm64 SDK bundle.
FROM --platform=linux/amd64 ubuntu:20.04

# Avoid interactive tzdata prompts during apt installs.
ENV DEBIAN_FRONTEND=noninteractive

# Target OWL platform the baked-in libraries are built for. Override at build
# time, e.g. `docker build --build-arg PLATFORM=OWL3 -t owlprogram:owl3-v2 .`.
# The libs (libowlprg.a/libdaisysp.a) are platform-specific (Cortex-M4 vs M7),
# so each image is pinned to ONE platform; build a separate image per target.
ARG PLATFORM=OWL2

# ---- Toolchain + build prerequisites -------------------------------------
# Same base set as ./Dockerfile, PLUS the FAUST-2.30.5 build deps:
#   cmake  -> FAUST build system (cmake-driven)
# We build FAUST with the `light` backend set, which has LLVM_BACKEND=OFF, so
# NO llvm/clang dev packages are needed — the faust CLI we need (CPP backend)
# does not embed LLVM. We do NOT install apt `faust` — we build 2.30.5 below.
RUN apt-get update && apt-get install -y --no-install-recommends \
        build-essential \
        make \
        cmake \
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
        curl \
        wget \
        unzip \
        bzip2 \
        xz-utils \
        zlib1g-dev \
        libfreetype6-dev \
        libx11-dev \
        libxext-dev \
        libxinerama-dev \
        libxrandr-dev \
        libxcursor-dev \
        libgl1-mesa-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /owl

# ---- ARM GCC 9-2020-q2-update (the live server's cross toolchain) ----------
# Unpacked under /opt and ALSO linked into the source tree at
# Tools/gcc-arm-none-eabi-9-2020-q2-update/ (where the live host keeps it and
# where the uncommented compile.mk TOOLROOT points). Fetched from ARM's
# published archive (developer.arm.com -> armkeil blob, verified reachable).
RUN wget -q -O /tmp/arm.tar.bz2 \
      "https://developer.arm.com/-/media/Files/downloads/gnu-rm/9-2020q2/gcc-arm-none-eabi-9-2020-q2-update-x86_64-linux.tar.bz2" \
 && mkdir -p /opt/arm \
 && tar -xjf /tmp/arm.tar.bz2 -C /opt/arm \
 && rm /tmp/arm.tar.bz2 \
 && ls /opt/arm
# the archive unpacks to /opt/arm/gcc-arm-none-eabi-9-2020-q2-update
ENV ARMGCC=/opt/arm/gcc-arm-none-eabi-9-2020-q2-update
ENV PATH=$ARMGCC/bin:$PATH

# ---- FAUST 2.30.5 from source ----------------------------------------------
# No prebuilt Linux binary exists for 2.30.5, so we build the `faust` CLI from
# the source tarball (which bundles the stdfaust libraries). The top Makefile's
# `compiler` target hardcodes BACKENDS=regular.cmake (which pulls LLVM), so we
# invoke cmake DIRECTLY with the `light` backend set (CPP_BACKEND on,
# LLVM_BACKEND off → no LLVM needed) and the `regular` target with
# INCLUDE_EXECUTABLE=on (produces the faust CLI). HTTP/OSC/static/dynamic are
# turned off — OwlProgram only needs the compiler binary. Builds in well under
# a minute.
RUN wget -q -O /tmp/faust.tar.gz \
      "https://github.com/grame-cncm/faust/releases/download/2.30.5/faust-2.30.5.tar.gz" \
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

# ---- SOUL compiler 1.0.82 (same as baseline) ------------------------------
RUN curl -sSLf -o /tmp/soul.zip \
        https://github.com/soul-lang/SOUL/releases/download/1.0.82/binaries-linux-combined.zip \
 && unzip -o /tmp/soul.zip -d /tmp/soul \
 && install -m 0755 /tmp/soul/linux/x64/soul /usr/local/bin/soul \
 && rm -rf /tmp/soul /tmp/soul.zip \
 && soul --version 2>&1 | head -1

# ---- Emscripten (emsdk) 2.0.10 (same as baseline, for make web/OwlServer) --
# Base image is pinned to linux/amd64 (see FROM) — emsdk 2.0.10 has no arm64
# SDK bundle, so the build must be x86_64.
RUN git clone --depth 1 --branch 2.0.10 \
      https://github.com/emscripten-core/emsdk.git /opt/emsdk \
 && cd /opt/emsdk \
 && ./emsdk install 2.0.10 \
 && ./emsdk activate 2.0.10
ENV EMSDK=/opt/emsdk \
    PATH=/opt/emsdk:/opt/emsdk/upstream/emscripten:/opt/emsdk/node/12.18.1_64bit/bin:$PATH

# ---- FirmwareSender (same as baseline) ------------------------------------
RUN git clone --depth 1 https://github.com/pingdynasty/FirmwareSender.git /opt/FirmwareSender \
 && make -C /opt/FirmwareSender/Builds/Linux CONFIG=Release \
 && install -m 0755 \
        "$(find /opt/FirmwareSender/Builds/Linux/build -type f -name FirmwareSender | head -1)" \
        /usr/local/bin/FirmwareSender \
 && FirmwareSender --help >/dev/null 2>&1; echo "FirmwareSender installed"

# ---- Source + live-server local edits --------------------------------------
# IMPORTANT: this copies the build context as-is, INCLUDING git submodules.
# The DaisySP submodule (Libraries/DaisySP) MUST be checked out in the context
# before building — run `git submodule update --init --recursive` first.
# If DaisySP is empty, `make libs` fails to find LibSource sources (the daisysp
# source list collapses the C++ vpath). The portable clone populates submodules
# at generation time, but switching branches locally needs another
# `git submodule update`. (DaisySP is slated for removal in a future revision.)
COPY . /owl

# Live-host edit #1: owl.cpp FAUST-2.30.x API migration (new/delete instead of
# the removed static create()/destroy() factory). Applied idempotently with sed
# so the image reproduces the live host even if the worktree is the committed
# (unpatched) tree.
RUN sed -i 's@fDSP = mydsp::create();@fDSP = new mydsp(); // mydsp::create();@' FaustSource/owl.cpp \
 && sed -i 's@mydsp::destroy(fDSP);@delete fDSP; // mydsp::destroy(fDSP);@' FaustSource/owl.cpp \
 && grep -n 'new mydsp()\|delete fDSP' FaustSource/owl.cpp

# Live-host edit #2: put the 9-2020-q2 toolchain where compile.mk's TOOLROOT
# expects it, and uncomment that TOOLROOT line so the build uses it.
RUN mkdir -p Tools \
 && ln -sfn "$ARMGCC" Tools/gcc-arm-none-eabi-9-2020-q2-update \
 && sed -i 's@^# *TOOLROOT ?= Tools/gcc-arm-none-eabi-9-2020-q2-update/bin/@TOOLROOT ?= Tools/gcc-arm-none-eabi-9-2020-q2-update/bin/@' compile.mk \
 && grep -n '^TOOLROOT' compile.mk

# Wire SOUL + FirmwareSender into Tools/ (same as baseline).
RUN ln -sf /usr/local/bin/soul /owl/Tools/soul \
 && ln -sf /usr/local/bin/FirmwareSender /owl/Tools/FirmwareSender

# hvcc deps (non-fatal).
RUN pip3 install --no-cache-dir -r Tools/hvcc/requirements.txt || true

# Pre-build the patch libraries for the target platform so a later
# `make PATCHNAME=X patch` links immediately (no separate `make libs` needed).
# `make libs` needs only PLATFORM. The archives are platform-specific and live
# at non-platform-keyed paths, so PLATFORM is fixed per image.
RUN make PLATFORM=${PLATFORM} libs

# Provenance.
RUN echo "=== LIVE-SERVER toolchain versions (PLATFORM=${PLATFORM}) ===" \
 && arm-none-eabi-gcc --version | head -1 \
 && gcc --version | head -1 \
 && (faust --version 2>&1 | head -1) \
 && (soul --version 2>&1 | head -1) \
 && (echo "FirmwareSender: $(ls -1 /usr/local/bin/FirmwareSender)") \
 && echo "TOOLROOT in compile.mk: $(grep '^TOOLROOT' compile.mk)"

CMD ["make", "help"]
