BUILDROOT ?= .

ifndef CONFIG
  CONFIG=Release
endif

ifeq ($(CONFIG),Debug)
CPPFLAGS     = -g -Wall -Wcpp -Wunused-function -DDEBUG -DUSE_FULL_ASSERT
EMCCFLAGS   ?= -g
ASFLAGS      = -g
endif

ifeq ($(CONFIG),Release)
CPPFLAGS     = -O2
EMCCFLAGS   ?= -Oz # optimise for size
endif

ifdef FAUST
# options for FAUST compilation
PATCHNAME   ?= $(FAUST)
PATCHCLASS  ?= FaustPatch
PATCHFILE   ?= $(PATCHNAME)Patch.hpp
DEPS        = faust
else ifdef HEAVY
# options for Heavy PD compilation
PATCHNAME   ?= $(HEAVY)
PATCHCLASS  ?= HeavyPatch
PATCHFILE   ?= HeavyPatch.hpp
DEPS        = heavy
else
# options for C++ compilation
PATCHNAME   ?= "Template"
PATCHCLASS  ?= $(PATCHNAME)Patch
PATCHFILE   ?= $(PATCHNAME)Patch.hpp
endif

PATCHIN     ?= 2
PATCHOUT    ?= 2
SLOT        ?= 0
OWLDEVICE   ?= "OWL-MIDI"
BUILD       ?= $(BUILDROOT)/Build
LDSCRIPT    ?= $(BUILDROOT)/Source/flash.ld
PATCHSOURCE ?= $(BUILDROOT)/PatchSource
FIRMWARESENDER = Tools/FirmwareSender

export BUILD
export BUILDROOT
export PATCHNAME 
export PATCHCLASS 
export PATCHSOURCE
export PATCHFILE
export PATCHIN
export PATCHOUT
export HEAVYTOKEN
export HEAVY
export LDSCRIPT

all: patch

.PHONY: .FORCE clean realclean run store online docs

$(BUILD)/%.syx: $(BUILD)/%.bin
	@$(FIRMWARESENDER) -q -in $< -save $@

size: $(BUILD)/patch.elf $(BUILD)/patch.bin
	@$(SIZE) $(BUILD)/patch.elf
	@ls -s --block-size=1 $(BUILD)/patch.bin

patch: $(DEPS)
	mkdir -p $(BUILD)/Source
	$(MAKE) -f compile.mk compile

faust:
	$(MAKE) -f faust.mk faust

heavy:
	$(MAKE) -f heavy.mk HEAVY=$(HEAVY) heavy

web:
	$(MAKE) -f web.mk web

online:
	$(MAKE) -f web.mk online

# patch: $(BUILD)/patch.bin

sysex: patch $(BUILD)/patch.syx

run: patch
	$(FIRMWARESENDER) -in $(BUILD)/patch.bin -out $(OWLDEVICE) -run

store: patch
	$(FIRMWARESENDER) -in $(BUILD)/patch.bin -out $(OWLDEVICE) -store $(SLOT)

docs:
	@doxygen Doxyfile

clean:
	@rm -rf $(BUILD)/*

realclean: clean
	@find Libraries/ -name '*.o' -delete

