BUILDROOT ?= .

ifndef CONFIG
ifndef TEST
  CONFIG=Release
else
  CONFIG=Debug
endif
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
else ifdef TEST
PATCHNAME   ?= $(TEST)
PATCHCLASS  ?= $(PATCHNAME)Patch
PATCHFILE   ?= $(PATCHNAME)Patch.hpp
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

DEPS += $(BUILD)/registerpatch.cpp $(BUILD)/registerpatch.h $(BUILD)/Source/startup.s 

all: patch

.PHONY: .FORCE clean realclean run store docs help

.FORCE:
	@echo Building patch $(PATCHNAME)
	@mkdir -p $(BUILD)/Source

$(BUILD)/registerpatch.cpp: .FORCE
	@echo "REGISTER_PATCH($(PATCHCLASS), \"$(PATCHNAME)\", $(PATCHIN), $(PATCHOUT));" > $@

$(BUILD)/registerpatch.h: .FORCE
	@echo "#include \"$(PATCHFILE)\"" > $@

$(BUILD)/Source/startup.s: .FORCE
	@echo '.string "'$(PATCHNAME)'"' > $(BUILD)/Source/progname.s

$(BUILD)/%.syx: $(BUILD)/%.bin
	@$(FIRMWARESENDER) -q -in $< -save $@

patch: $(DEPS) ## build patch binary
	@$(MAKE) -s -f compile.mk compile

web: $(DEPS) ## build Javascript patch
	@$(MAKE) -s -f web.mk web
	@echo Built Web Audio $(PATCHNAME) in $(BUILD)/web/$(TARGET).js

minify: $(DEPS)
	@$(MAKE) -s -f web.mk minify

faust: .FORCE
	@$(MAKE) -s -f faust.mk faust

heavy: .FORCE
	@$(MAKE) -s -f heavy.mk heavy

sysex: patch $(BUILD)/$(TARGET).syx ## package patch binary as MIDI sysex
	@echo Built sysex $(PATCHNAME) in $(BUILD)/$(TARGET).syx

run: patch ## upload patch to attached OWL via MIDI
	@echo Sending patch $(PATCHNAME) to $(OWLDEVICE) to run
	@$(FIRMWARESENDER) -q -in $(BUILD)/$(TARGET).bin -out $(OWLDEVICE) -run

store: patch ## upload and save patch to attached OWL
	@echo Sending patch $(PATCHNAME) to $(OWLDEVICE) to store in slot $(SLOT)
	@$(FIRMWARESENDER) -q -in $(BUILD)/$(TARGET).bin -out $(OWLDEVICE) -store $(SLOT)

docs: ## generate HTML documentation
	@doxygen Doxyfile

clean: ## remove generated patch files
	@rm -rf $(BUILD)/*

realclean: clean ## remove all library object files
	@find Libraries/ -name '*.o' -delete

size: patch ## show binary size metrics and large object summary
	@$(MAKE) -s -f common.mk size

map: patch ## build map file (Build/patch.map)
	@$(MAKE) -s -f compile.mk map
	@echo Built $(PATCHNAME) map in $(BUILD)/$(TARGET).map

as: patch ## build assembly file (Build/patch.s)
	@$(MAKE) -s -f compile.mk as
	@echo Built $(PATCHNAME) assembly in $(BUILD)/$(TARGET).s

test: $(DEPS) ## run test patch
	@$(MAKE) -s -f test.mk test

help: ## show this help
	@echo 'Usage: make [target] ...'
	@echo 'Targets:'
	@fgrep -h "##" $(MAKEFILE_LIST) | fgrep -v fgrep | sed -e  's/^\(.*\): .*##\(.*\)/\1:#\2/' | column -t -c 2 -s '#'
