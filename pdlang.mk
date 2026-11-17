# pdlang.mk — compile a Pure Data patch with pdlang (https://github.com/pingdynasty/pdlang).
#   make PDLANG=<patch> [patch|native|test|run|web]
# compiles $(PATCHSOURCE)/<patch>.pd for the build's target, wrapped by PdlangSource/PdlangPatch.cpp.
# Options:
#   PDLANG_BIN    compiler path (default: pdlang on PATH)
#   PDLANGMODE    -fcompat mode: pdlang (default), libpd or hvcc
#   PDLANGMATH    -fmath: host (default: the maths come from PdlangPatch.cpp, OWL's fast kernels on the device)
#                 or libm
#   PDLANGPRECISION -fprecision: single (default) or double (the pd modes only)
#   PDLANGARGS    extra pdlang options, e.g. --noise-seed=1 --random-seed=1,2 or -I <dir>
# Included by compile.mk, native.mk and web.mk, which set:
#   PDLANGOBJ     the object to build
#   PDLANGTARGET  the pdlang --target triple (empty: the host)

# The top-level Makefile exports these, so unset ones arrive here as empty strings: test for empty, not ?=
ifeq ($(PDLANGFILE),)
PDLANGFILE   = $(PDLANG).pd
endif
ifeq ($(PDLANG_BIN),)
PDLANG_BIN   = pdlang
endif
ifeq ($(PDLANGMODE),)
PDLANGMODE   = pdlang
endif
ifeq ($(PDLANGMATH),)
PDLANGMATH   = host
endif
ifeq ($(PDLANGPRECISION),)
PDLANGPRECISION = single
endif

ifeq ($(shell command -v $(PDLANG_BIN) 2>/dev/null),)
$(error pdlang compiler not found: '$(PDLANG_BIN)'. Build it (cargo build --release in the pdlang checkout) and set PDLANG_BIN to the binary, or put it on the PATH)
endif

PDLANGFLAGS  = -c -O2 -fcompat=$(PDLANGMODE) -fmath=$(PDLANGMATH) -fprecision=$(PDLANGPRECISION) -I $(PATCHSOURCE)
ifneq ($(PDLANGTARGET),)
PDLANGFLAGS += --target=$(PDLANGTARGET)
endif

$(PDLANGOBJ): $(PATCHSOURCE)/$(PDLANGFILE) .FORCE
	@mkdir -p $(dir $@)
	@$(PDLANG_BIN) $(PDLANGFLAGS) $(PDLANGARGS) $< -o $@

.FORCE:

pdlang: $(PDLANGOBJ)
