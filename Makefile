BUILDROOT ?= .

ifndef CONFIG
  CONFIG=Release
endif

DEPS = .FORCE
TARGET ?= patch

ifeq ($(CONFIG),Debug)
CPPFLAGS    ?= -g -Wall -Wcpp -Wunused-function -DDEBUG -DUSE_FULL_ASSERT
EMCCFLAGS   ?= -g
ASFLAGS      = -g
endif

ifeq ($(CONFIG),Release)
CPPFLAGS    ?= -O2 -specs=nano.specs -ffast-math
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

export BUILD BUILDROOT TARGET
export PATCHNAME PATCHCLASS PATCHSOURCE 
export PATCHFILE PATCHIN PATCHOUT
export HEAVYTOKEN HEAVY
export LDSCRIPT CPPFLAGS EMCCFLAGS ASFLAGS

DEPS += $(BUILD)/patch.cpp $(BUILD)/patch.h $(BUILD)/Source/startup.s 

all: patch

.PHONY: .FORCE clean realclean run store docs

.FORCE:
	@echo Building patch $(PATCHNAME)
	@mkdir -p $(BUILD)/Source

$(BUILD)/patch.cpp: .FORCE
	@echo "REGISTER_PATCH($(PATCHCLASS), \"$(PATCHNAME)\", $(PATCHIN), $(PATCHOUT));" > $@

$(BUILD)/patch.h: .FORCE
	@echo "#include \"$(PATCHFILE)\"" > $@

$(BUILD)/Source/startup.s: .FORCE
	@echo '.string "'$(PATCHNAME)'"' > $(BUILD)/Source/progname.s

$(BUILD)/%.syx: $(BUILD)/%.bin
	@$(FIRMWARESENDER) -q -in $< -save $@

patch: $(DEPS)
	@$(MAKE) -s -f compile.mk compile

size: patch
	@$(MAKE) -s -f common.mk size

map: patch
	@$(MAKE) -s -f compile.mk map
	@echo Built $(PATCHNAME) map in $(BUILD)/$(TARGET).map

as: patch
	@$(MAKE) -s -f compile.mk as
	@echo Built $(PATCHNAME) assembly in $(BUILD)/$(TARGET).s

web: $(DEPS)
	@$(MAKE) -s -f web.mk web
	@echo Built Web Audio $(PATCHNAME) in $(BUILD)/web/$(TARGET).js

minify: $(DEPS)
	@$(MAKE) -s -f web.mk minify

faust: .FORCE
	@$(MAKE) -s -f faust.mk faust

heavy: .FORCE
	@$(MAKE) -s -f heavy.mk heavy

sysex: patch $(BUILD)/$(TARGET).syx
	@echo Built sysex $(PATCHNAME) in $(BUILD)/$(TARGET).syx

run: patch
	@echo Sending patch $(PATCHNAME) to $(OWLDEVICE) to run
	@$(FIRMWARESENDER) -q -in $(BUILD)/$(TARGET).bin -out $(OWLDEVICE) -run

store: patch
	@echo Sending patch $(PATCHNAME) to $(OWLDEVICE) to store in slot $(SLOT)
	@$(FIRMWARESENDER) -q -in $(BUILD)/$(TARGET).bin -out $(OWLDEVICE) -store $(SLOT)

docs:
	@doxygen Doxyfile

clean:
	@rm -rf $(BUILD)/*

realclean: clean
	@find Libraries/ -name '*.o' -delete

