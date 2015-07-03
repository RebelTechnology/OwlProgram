BUILDROOT = .

ifndef CONFIG
  CONFIG=Release
endif

ifeq ($(CONFIG),Debug)
CFLAGS   = -g -Wall -Wcpp -Wunused-function -DDEBUG -DUSE_FULL_ASSERT
ASFLAGS  = -g
endif

ifeq ($(CONFIG),Release)
CFLAGS   = -O2
endif

PATCHNAME   ?= "Template"
PATCHCLASS  ?= $(PATCHNAME)Patch
PATCHFILE   ?= $(PATCHNAME)Patch.hpp
PATCHIN     ?= 2
PATCHOUT    ?= 2
SLOT        ?= 0
OWLDEVICE   ?= "OWL-MIDI"
BUILD       ?= $(BUILDROOT)/Build

CFLAGS += -DARM_CORTEX
CFLAGS += -DEXTERNAL_SRAM
CFLAGS += -nostdlib -nostartfiles -fno-builtin -ffreestanding
CFLAGS += -mtune=cortex-m4
CFLAGS += -fpic
CFLAGS += -fpie
CFLAGS += -fdata-sections 
CFLAGS += -ffunction-sections
# CFLAGS += -munaligned-access
CFLAGS +=  -mno-unaligned-access
# CFLAGS += -mlong-calls

# CFLAGS += -mpic-data-is-text-relative
CFLAGS += -fno-omit-frame-pointer
CFLAGS += -flto

LDFLAGS  = -Wl,--gc-sections
LDFLAGS += -fpic
LDFLAGS += -fpie
LDFLAGS += -flto

LDLIBS   = -lm
LDSCRIPT = Source/flash.ld
FIRMWARESENDER = Tools/FirmwareSender

C_SRC   = basicmaths.c
CPP_SRC = main.cpp operators.cpp message.cpp StompBox.cpp PatchProcessor.cpp 
CPP_SRC += FloatArray.cpp ComplexFloatArray.cpp
CPP_SRC += PatchProgram.cpp

PATCHSOURCE ?= $(BUILDROOT)/PatchSource
LIBSOURCE    = $(BUILDROOT)/LibSource
TESTPATCHES  = $(BUILDROOT)/TestPatches
CFLAGS += -I$(LIBSOURCE)
CFLAGS += -I$(PATCHSOURCE)
CFLAGS += -I$(TESTPATCHES)
CFLAGS += -I$(BUILD)
CFLAGS += -IOwlPatches
PATCH_C_SRC = $(wildcard $(PATCHSOURCE)/*.c) 
PATCH_CPP_SRC += $(wildcard $(PATCHSOURCE)/*.cpp)
PATCH_OBJS += $(addprefix $(BUILD)/, $(notdir $(PATCH_C_SRC:.c=.o)))
PATCH_OBJS += $(addprefix $(BUILD)/, $(notdir $(PATCH_CPP_SRC:.cpp=.o)))
vpath %.cpp $(LIBSOURCE)
vpath %.c $(LIBSOURCE)
vpath %.s $(LIBSOURCE)
vpath %.cpp $(PATCHSOURCE)
vpath %.c $(PATCHSOURCE)
vpath %.s $(PATCHSOURCE)

# Heavy
HEAVYFILE ?= $(PATCHNAME).pd
HEAVYNAME  = owl
HEAVYDIR   = $(BUILD)/HeavySource
CFLAGS    += -I$(HEAVYDIR)
CFLAGS    += -D__unix__ -DHV_SIMD_NONE
vpath %.c $(HEAVYDIR)

# emscripten
EMCC       = emcc
EMCCFLAGS ?= -fno-rtti -fno-exceptions # -std=c++11 
EMCCFLAGS += -IOwlPatches -ISource -I$(PATCHSOURCE) -I$(LIBSOURCE) -I$(BUILD) -I$(TESTPATCHES)
EMCCFLAGS += -ILibraries/KissFFT
EMCCFLAGS += -s EXPORTED_FUNCTIONS="['_WEB_setup','_WEB_setParameter','_WEB_processBlock','_WEB_getPatchName','_WEB_getParameterName','_WEB_getMessage','_WEB_getStatus']"
EMCC_SRC   = Source/PatchProgram.cpp Source/PatchProcessor.cpp Source/operators.cpp Source/message.cpp 
EMCC_SRC  += WebSource/web.cpp 
EMCC_SRC  += $(LIBSOURCE)/basicmaths.c $(LIBSOURCE)/StompBox.cpp $(LIBSOURCE)/FloatArray.cpp $(LIBSOURCE)/ComplexFloatArray.cpp
EMCC_SRC  += $(PATCH_CPP_SRC) $(PATCH_C_SRC)
EMCC_SRC  += Libraries/KissFFT/kiss_fft.c

CXXFLAGS = -fno-rtti -fno-exceptions -std=c++11 $(CFLAGS) 

# object files
OBJS =  $(C_SRC:%.c=$(BUILD)/%.o) $(CPP_SRC:%.cpp=$(BUILD)/%.o)

all: patch

# include common make file
include $(BUILDROOT)/libs.mk
include $(BUILDROOT)/common.mk

.PHONY: clean realclean prep run store online docs

prep:
	@echo Building patch $(PATCHNAME)
	@echo '.string "'$(PATCHNAME)'"' > $(BUILD)/progname.s
	@echo "#include \"$(PATCHFILE)\"" > $(BUILD)/patch.h
	@echo "REGISTER_PATCH($(PATCHCLASS), \"$(PATCHNAME)\", $(PATCHIN), $(PATCHOUT));" > $(BUILD)/patch.cpp

# Build executable 
$(BUILD)/patch.elf : $(PATCH_OBJS) $(OBJS) $(LDSCRIPT) 
	$(LD) $(LDFLAGS) -o $@ $(PATCH_OBJS) $(OBJS) $(LDLIBS)

$(BUILD)/patch.as : $(PATCH_OBJS) $(OBJS) $(LDSCRIPT)
	$(LD) $(LDFLAGS) -o $@ $(PATCH_OBJS) $(OBJS) $(LDLIBS)

$(BUILD)/patch.map : $(PATCH_OBJS) $(OBJS) $(LDSCRIPT)
	$(LD) $(LDFLAGS) -Wl,-Map=$(BUILD)/patch.map $(OBJS) $(PATCH_OBJS) $(LDLIBS)

$(BUILD)/%.syx : $(BUILD)/%.bin
	$(FIRMWARESENDER) -q -in $< -save $@

$(BUILD)/%Patch.hpp: $(PATCHSOURCE)/%.dsp
	cd $(BUILD) && faust2owl ../$<

patch: prep $(BUILD)/patch.bin

sysex: prep $(BUILD)/patch.syx

run: prep $(BUILD)/patch.bin
	$(FIRMWARESENDER) -in $(BUILD)/patch.bin -out $(OWLDEVICE) -run

store: prep $(BUILD)/patch.bin
	$(FIRMWARESENDER) -in $(BUILD)/patch.bin -out $(OWLDEVICE) -store $(SLOT)

docs:
	doxygen Doxyfile

online:
	@echo "$(ONLINE_INCLUDES)" > $(BUILD)/patch.h
	@echo "$(ONLINE_REGISTER)" > $(BUILD)/patch.cpp
	@echo '.string "OnlineCompiler"' > $(BUILD)/progname.s
	@make $(BUILD)/patch.syx
	@cp $(BUILD)/patch.syx $(BUILD)/online.syx

web: prep $(PATCH_C_SRC) $(PATCH_CPP_SRC)
	$(EMCC) $(EMCCFLAGS) $(EMCC_SRC) -o $(BUILD)/patch.js

$(HEAVYDIR)/_main.pd: $(PATCHSOURCE)/$(HEAVYFILE)
	@mkdir -p $(BUILD)/HeavySource
	@cp -f $(PATCHSOURCE)/*.pd $(BUILD)/HeavySource
	@cp -f $< $@

$(HEAVYDIR)/Heavy_owl.h: $(BUILD)/HeavySource/_main.pd
	python ./Tools/Heavy/uploader.py $(BUILD)/HeavySource -g c -n $(HEAVYNAME) -o $(HEAVYDIR)

heavy: $(HEAVYDIR)/Heavy_owl.h
	$(eval HEAVY_SRC = $(wildcard $(HEAVYDIR)/*.c) )
	$(eval PATCH_OBJS += $(addprefix $(BUILD)/, $(notdir $(HEAVY_SRC:.c=.o))) )
	$(eval PATCHCLASS = HeavyPatch)
	$(eval PATCHFILE = HeavyPatch.hpp)
	make $(PATCH_OBJS)
