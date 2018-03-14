BUILDROOT ?= .

C_SRC   = basicmaths.c heap_5.c fastpow.c fastlog.c # sbrk.c
CPP_SRC = main.cpp operators.cpp message.cpp system_tables.cpp
CPP_SRC += Patch.cpp PatchProcessor.cpp
CPP_SRC += FloatArray.cpp ComplexFloatArray.cpp ComplexShortArray.cpp FastFourierTransform.cpp ShortFastFourierTransform.cpp 
CPP_SRC += ShortArray.cpp
CPP_SRC += Envelope.cpp VoltsPerOctave.cpp Window.cpp
CPP_SRC += WavetableOscillator.cpp PolyBlepOscillator.cpp
CPP_SRC += SmoothValue.cpp PatchParameter.cpp
CPP_SRC += PatchProgram.cpp 
# CPP_SRC += ShortPatchProgram.cpp 

SOURCE       = $(BUILDROOT)/Source
LIBSOURCE    = $(BUILDROOT)/LibSource
GENSOURCE    = $(BUILD)/Source
TESTPATCHES  = $(BUILDROOT)/TestPatches
CPPFLAGS += -I$(PATCHSOURCE)
CPPFLAGS += -I$(LIBSOURCE)
CPPFLAGS += -I$(GENSOURCE)
CPPFLAGS += -I$(TESTPATCHES)
PATCH_C_SRC    = $(wildcard $(PATCHSOURCE)/*.c)
PATCH_CPP_SRC += $(wildcard $(PATCHSOURCE)/*.cpp)
PATCH_C_SRC   += $(wildcard $(GENSOURCE)/*.c)
PATCH_CPP_SRC += $(wildcard $(GENSOURCE)/*.cpp)
PATCH_OBJS += $(addprefix $(BUILD)/, $(notdir $(PATCH_C_SRC:.c=.o)))
PATCH_OBJS += $(addprefix $(BUILD)/, $(notdir $(PATCH_CPP_SRC:.cpp=.o)))

CPPFLAGS += -DARM_CORTEX
CPPFLAGS += -DEXTERNAL_SRAM
CPPFLAGS += -nostdlib -nostartfiles -fno-builtin -ffreestanding
CPPFLAGS += -mtune=cortex-m4
# CPPFLAGS += -mtune=cortex-m7
CPPFLAGS += -fpic
CPPFLAGS += -fpie
CPPFLAGS += -fdata-sections
CPPFLAGS += -ffunction-sections
# CPPFLAGS += -munaligned-access
CPPFLAGS += -mno-unaligned-access
# CPPFLAGS += -mlong-calls

# CPPFLAGS += -mpic-data-is-text-relative
CPPFLAGS += -fno-omit-frame-pointer
CPPFLAGS += -flto

LDLIBS   = -lm
LDFLAGS  = -Wl,--gc-sections
LDFLAGS += -fpic
LDFLAGS += -fpie
LDFLAGS += -flto

CXXFLAGS = -fno-rtti -fno-exceptions -std=gnu++11

ifdef HEAVY
CPPFLAGS    += -D__unix__ -DHV_SIMD_NONE
else ifeq ($(PLATFORM),Alchemist)
CPPFLAGS    += -DOWL_MICROLAB
else ifeq ($(PLATFORM),Wizard)
CPPFLAGS    += -DOWL_MINILAB
else ifeq ($(PLATFORM),Prism)
CPPFLAGS    += -DOWL_PRISM
CPP_SRC     += ScreenBuffer.cpp ScreenBufferDevice.cpp
else ifeq ($(PLATFORM),Magus)
CPPFLAGS    += -DOWL_MAGUS
CPP_SRC     += ScreenBuffer.cpp ScreenBufferDevice.cpp
else ifeq ($(PLATFORM),Player)
CPPFLAGS    += -DOWL_PLAYER
CPP_SRC     += ScreenBuffer.cpp ScreenBufferDevice.cpp
else
CPPFLAGS    += -DOWL_CLASSIC
endif

CC=gcc
LD=gcc
AR=ar
AS=as
NM=nm
CXX=g++
GDB=gdb
SIZE=size
RANLIB=ranlib
OBJCOPY=objcopy
OBJDUMP=objdump

# object files
OBJS  = $(C_SRC:%.c=$(BUILD)/%.o) $(CPP_SRC:%.cpp=$(BUILD)/%.o)
OBJS += $(BUILD)/startup.o
OBJS += $(BUILD)/libnosys_gnu.o

# include common make file
include $(BUILDROOT)/libs.mk
include $(BUILDROOT)/common.mk

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
vpath %.cpp $(GENSOURCE)
vpath %.c $(GENSOURCE)
vpath %.s $(GENSOURCE)
vpath %.c Libraries/syscalls

$(BUILD)/ShortPatchProgram.o: $(SOURCE)/ShortPatchProgram.cpp $(DEPS)
	@$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) -I$(BUILD) $(SOURCE)/ShortPatchProgram.cpp -o $@
	@$(CXX) -MM -MT"$@" $(CPPFLAGS) $(CXXFLAGS) -I$(BUILD) $(SOURCE)/ShortPatchProgram.cpp > $(@:.o=.d)

$(BUILD)/PatchProgram.o: $(SOURCE)/PatchProgram.cpp $(DEPS)
	@$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) -I$(BUILD) $(SOURCE)/PatchProgram.cpp -o $@
	@$(CXX) -MM -MT"$@" $(CPPFLAGS) $(CXXFLAGS) -I$(BUILD) $(SOURCE)/PatchProgram.cpp > $(@:.o=.d)

$(BUILD)/$(TARGET).elf: $(PATCH_OBJS) $(OBJS) $(LDSCRIPT)
	@$(LD) $(LDFLAGS) -o $@ $(PATCH_OBJS) $(OBJS) $(LDLIBS)

as: $(BUILD)/$(TARGET).elf
	@$(OBJDUMP) -S $< > $(BUILD)/$(TARGET).s

map: $(PATCH_OBJS) $(OBJS) $(LDSCRIPT)
	@$(LD) $(LDFLAGS) -Wl,-Map=$(BUILD)/$(TARGET).map $(OBJS) $(PATCH_OBJS) $(LDLIBS)

compile: $(BUILD)/$(TARGET).bin
