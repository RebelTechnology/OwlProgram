BUILDROOT ?= .

ifndef CONFIG
  CONFIG=Release
endif

DEPS = .FORCE

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
PATCHFILE   ?= FaustPatch.hpp
DEPS        += faust
else ifdef HEAVY
# options for Heavy PD compilation
PATCHNAME   ?= $(HEAVY)
PATCHCLASS  ?= HeavyPatch
PATCHFILE   ?= HeavyPatch.hpp
DEPS        += heavy
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

export BUILD BUILDROOT
export PATCHNAME PATCHCLASS PATCHSOURCE 
export PATCHFILE PATCHIN PATCHOUT
export HEAVYTOKEN HEAVY
export LDSCRIPT CPPFLAGS EMCCFLAGS ASFLAGS

DEPS += $(BUILD)/patch.cpp $(BUILD)/patch.h $(BUILD)/Source/startup.s 

all: patch

.PHONY: .FORCE clean realclean run store online docs

.FORCE:
	@echo Building patch $(PATCHNAME)
	@mkdir -p $(BUILD)/Source

$(BUILD)/patch.cpp:
	@echo "REGISTER_PATCH($(PATCHCLASS), \"$(PATCHNAME)\", $(PATCHIN), $(PATCHOUT));" > $@

$(BUILD)/patch.h:
	@echo "#include \"$(PATCHFILE)\"" > $@

$(BUILD)/Source/startup.s:
	@echo '.string "'$(PATCHNAME)'"' > $(BUILD)/Source/progname.s

$(BUILD)/%.syx: $(BUILD)/%.bin
	@$(FIRMWARESENDER) -q -in $< -save $@

size: patch
	@$(SIZE) $(BUILD)/patch.elf
	@ls -s --block-size=1 $(BUILD)/patch.bin

patch: $(DEPS)
	@$(MAKE) -s -f compile.mk compile

map: $(DEPS)
	@$(MAKE) -s -f compile.mk map

web: $(DEPS)
	@$(MAKE) -s -f web.mk web
	@echo Built Web Audio $(PATCHNAME) in $(BUILD)/web/patch.js

minify: $(DEPS)
	@$(MAKE) -s -f web.mk minify

faust:
	@$(MAKE) -s -f faust.mk faust

heavy:
	@$(MAKE) -s -f heavy.mk HEAVY=$(HEAVY) heavy

sysex: patch $(BUILD)/patch.syx
	@echo Built sysex $(PATCHNAME) in $(BUILD)/patch.syx

run: patch
	@echo Sending patch $(PATCHNAME) to $(OWLDEVICE) to run
	@$(FIRMWARESENDER) -q -in $(BUILD)/patch.bin -out $(OWLDEVICE) -run

store: patch
	@echo Sending patch $(PATCHNAME) to $(OWLDEVICE) to store in slot $(SLOT)
	@$(FIRMWARESENDER) -q -in $(BUILD)/patch.bin -out $(OWLDEVICE) -store $(SLOT)

docs:
	@doxygen Doxyfile

clean:
	rm -rf $(BUILD)/*

realclean: clean
	@find Libraries/ -name '*.o' -delete

