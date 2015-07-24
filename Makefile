BUILDROOT ?= $(CURDIR)

ifndef CONFIG
  CONFIG=Release
endif

ifeq ($(CONFIG),Debug)
CPPFLAGS   = -g -Wall -Wcpp -Wunused-function -DDEBUG -DUSE_FULL_ASSERT
ASFLAGS    = -g
endif

ifeq ($(CONFIG),Release)
CPPFLAGS   = -O2
endif

DEPS       = $(BUILD)/patch.cpp $(BUILD)/patch.h

ifdef FAUST
# options for FAUST compilation
PATCHNAME   ?= $(FAUST)
PATCHCLASS  ?= $(PATCHNAME)Patch
PATCHFILE   ?= $(PATCHNAME)Patch.hpp
DEPS        += $(BUILD)/$(PATCHFILE)
else ifdef HEAVY
# options for Heavy PD compilation
PATCHNAME   ?= $(HEAVY)
PATCHCLASS  ?= HeavyPatch
PATCHFILE   ?= HeavyPatch.hpp
DEPS        += $(HEAVYDIR)/Heavy_owl.h
HEAVYFILE   ?= $(HEAVY).pd
HEAVYNAME   ?= owl
HEAVYDIR    ?= $(BUILD)/HeavySource
CPPFLAGS    += -I$(HEAVYDIR)
CPPFLAGS    += -D__unix__ -DHV_SIMD_NONE
vpath %.c $(HEAVYDIR)
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

CPPFLAGS += -DARM_CORTEX
CPPFLAGS += -DEXTERNAL_SRAM
CPPFLAGS += -nostdlib -nostartfiles -fno-builtin -ffreestanding
CPPFLAGS += -mtune=cortex-m4
CPPFLAGS += -fpic
CPPFLAGS += -fpie
CPPFLAGS += -fdata-sections 
CPPFLAGS += -ffunction-sections
# CPPFLAGS += -munaligned-access
CPPFLAGS +=  -mno-unaligned-access
# CPPFLAGS += -mlong-calls

# CPPFLAGS += -mpic-data-is-text-relative
CPPFLAGS += -fno-omit-frame-pointer
CPPFLAGS += -flto

LDFLAGS  = -Wl,--gc-sections
LDFLAGS += -fpic
LDFLAGS += -fpie
LDFLAGS += -flto

LDLIBS   ?= -lm
LDSCRIPT ?= Source/flash.ld
FIRMWARESENDER = Tools/FirmwareSender

C_SRC   = basicmaths.c
CPP_SRC = main.cpp operators.cpp message.cpp StompBox.cpp PatchProcessor.cpp 
CPP_SRC += FloatArray.cpp ComplexFloatArray.cpp
CPP_SRC += PatchProgram.cpp

SOURCE       = $(BUILDROOT)/Source
PATCHSOURCE ?= $(BUILDROOT)/PatchSource
LIBSOURCE    = $(BUILDROOT)/LibSource
TESTPATCHES  = $(BUILDROOT)/TestPatches
CPPFLAGS += -I$(LIBSOURCE)
CPPFLAGS += -I$(PATCHSOURCE)
CPPFLAGS += -I$(TESTPATCHES)
CPPFLAGS += -I$(BUILD)
CPPFLAGS += -IOwlPatches
PATCH_C_SRC = $(wildcard $(PATCHSOURCE)/*.c) 
PATCH_CPP_SRC += $(wildcard $(PATCHSOURCE)/*.cpp)
PATCH_OBJS += $(addprefix $(BUILD)/, $(notdir $(PATCH_C_SRC:.c=.o)))
PATCH_OBJS += $(addprefix $(BUILD)/, $(notdir $(PATCH_CPP_SRC:.cpp=.o)))

# Set up search path
vpath %.cpp $(SOURCE)
vpath %.c $(SOURCE)
vpath %.s $(SOURCE)
vpath %.cpp $(LIBSOURCE)
vpath %.c $(LIBSOURCE)
vpath %.s $(LIBSOURCE)
vpath %.cpp $(PATCHSOURCE)
vpath %.c $(PATCHSOURCE)
vpath %.s $(PATCHSOURCE)
vpath %.c Libraries/syscalls

# emscripten
EMCC       = emcc
EMCCFLAGS ?= -fno-rtti -fno-exceptions # -std=c++11 
EMCCFLAGS += -IOwlPatches -I$(SOURCE) -I$(PATCHSOURCE) -I$(LIBSOURCE) -I$(BUILD) -I$(TESTPATCHES)
EMCCFLAGS += -ILibraries/KissFFT
EMCCFLAGS += -s EXPORTED_FUNCTIONS="['_WEB_setup','_WEB_setParameter','_WEB_processBlock','_WEB_getPatchName','_WEB_getParameterName','_WEB_getMessage','_WEB_getStatus']"
EMCC_SRC   = $(SOURCE)/PatchProgram.cpp $(SOURCE)/PatchProcessor.cpp $(SOURCE)/operators.cpp $(SOURCE)/message.cpp 
EMCC_SRC  += WebSource/web.cpp 
EMCC_SRC  += $(LIBSOURCE)/basicmaths.c $(LIBSOURCE)/StompBox.cpp $(LIBSOURCE)/FloatArray.cpp $(LIBSOURCE)/ComplexFloatArray.cpp
EMCC_SRC  += $(PATCH_CPP_SRC) $(PATCH_C_SRC)
EMCC_SRC  += Libraries/KissFFT/kiss_fft.c

CXXFLAGS = -fno-rtti -fno-exceptions -std=c++11

# object files
OBJS  = $(C_SRC:%.c=$(BUILD)/%.o) $(CPP_SRC:%.cpp=$(BUILD)/%.o)
OBJS += $(BUILD)/startup.o
OBJS += $(BUILD)/libnosys_gnu.o

all: patch

# include common make file
include $(BUILDROOT)/libs.mk
include $(BUILDROOT)/common.mk

.PHONY: .FORCE clean realclean run store online docs

.FORCE:
	@echo Building patch $(PATCHNAME)

$(BUILD)/patch.cpp: .FORCE
	echo "REGISTER_PATCH($(PATCHCLASS), \"$(PATCHNAME)\", $(PATCHIN), $(PATCHOUT));" > $(BUILD)/patch.cpp

$(BUILD)/patch.h: .FORCE
	echo "#include \"$(PATCHFILE)\"" > $(BUILD)/patch.h

$(BUILD)/startup.o: .FORCE
	@echo '.string "'$(PATCHNAME)'"' > $(BUILD)/progname.s
	@$(CC) -c $(CPPFLAGS) $(CFLAGS) $(SOURCE)/startup.s -o $@

$(BUILD)/PatchProgram.o: $(SOURCE)/PatchProgram.cpp $(DEPS)
	@$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $(SOURCE)/PatchProgram.cpp -o $@
	@$(CXX) -MM -MT"$@" $(CPPFLAGS) $(CXXFLAGS) $(SOURCE)/PatchProgram.cpp > $(@:.o=.d)

$(BUILD)/patch.elf: $(PATCH_OBJS) $(OBJS) $(LDSCRIPT)
	@$(LD) $(LDFLAGS) -o $@ $(PATCH_OBJS) $(OBJS) $(LDLIBS)

as: $(PATCH_OBJS) $(OBJS) $(LDSCRIPT)
	@$(LD) $(LDFLAGS) -o $@ $(PATCH_OBJS) $(OBJS) $(LDLIBS)

map: $(PATCH_OBJS) $(OBJS) $(LDSCRIPT)
	@$(LD) $(LDFLAGS) -Wl,-Map=$(BUILD)/patch.map $(OBJS) $(PATCH_OBJS) $(LDLIBS)

$(BUILD)/%.syx: $(BUILD)/%.bin
	@$(FIRMWARESENDER) -q -in $< -save $@

$(BUILD)/%Patch.hpp: $(PATCHSOURCE)/%.dsp
	@cd $(BUILD) && faust2owl $<

size: $(BUILD)/patch.elf $(BUILD)/patch.bin
	@$(SIZE) $(BUILD)/patch.elf
	@ls -s --block-size=1 $(BUILD)/patch.bin

patch: $(BUILD)/patch.bin

sysex: patch $(BUILD)/patch.syx

run: patch
	$(FIRMWARESENDER) -in $(BUILD)/patch.bin -out $(OWLDEVICE) -run

store: patch
	$(FIRMWARESENDER) -in $(BUILD)/patch.bin -out $(OWLDEVICE) -store $(SLOT)

docs:
	@doxygen Doxyfile

online:
	@echo "$(ONLINE_INCLUDES)" > $(BUILD)/patch.h
	@echo "$(ONLINE_REGISTER)" > $(BUILD)/patch.cpp
	@echo '.string "OnlineCompiler"' > $(BUILD)/progname.s
	@$(MAKE) $(BUILD)/patch.syx
	@cp $(BUILD)/patch.syx $(BUILD)/online.syx

web: $(EMCC_SRC) $(DEPS)
	@$(EMCC) $(EMCCFLAGS) $(EMCC_SRC) -o $(BUILD)/patch.js

$(HEAVYDIR)/_main.pd: $(PATCHSOURCE)/$(HEAVYFILE)
	@mkdir -p $(BUILD)/HeavySource
	@cp -f $(PATCHSOURCE)/*.pd $(BUILD)/HeavySource
	@cp -f $< $@

$(HEAVYDIR)/Heavy_owl.h: $(HEAVYDIR)/_main.pd
	@python ./Tools/Heavy/uploader.py $(BUILD)/HeavySource -g c -n $(HEAVYNAME) -o $(HEAVYDIR)

heavy: $(HEAVYDIR)/Heavy_owl.h
	@$(eval HEAVY_SRC = $(wildcard $(HEAVYDIR)/*.c) )
	@$(eval PATCH_OBJS += $(addprefix $(HEAVYDIR)/, $(notdir $(HEAVY_SRC:.c=.o))))
	@make $(PATCH_OBJS)
