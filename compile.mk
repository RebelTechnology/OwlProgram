BUILDROOT ?= .

C_SRC   = basicmaths.c
CPP_SRC = main.cpp operators.cpp message.cpp StompBox.cpp PatchProcessor.cpp
CPP_SRC += FloatArray.cpp ComplexFloatArray.cpp FastFourierTransform.cpp 
CPP_SRC += Envelope.cpp VoltsPerOctave.cpp
CPP_SRC += WavetableOscillator.cpp PolyBlepOscillator.cpp
CPP_SRC += PatchProgram.cpp

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

CXXFLAGS = -fno-rtti -fno-exceptions -std=c++11

ifdef HEAVY
CPPFLAGS    += -D__unix__ -DHV_SIMD_NONE
endif

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

$(BUILD)/PatchProgram.o: $(SOURCE)/PatchProgram.cpp $(DEPS)
	@$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) -I$(BUILD) $(SOURCE)/PatchProgram.cpp -o $@
	@$(CXX) -MM -MT"$@" $(CPPFLAGS) $(CXXFLAGS) -I$(BUILD) $(SOURCE)/PatchProgram.cpp > $(@:.o=.d)

$(BUILD)/patch.elf: $(PATCH_OBJS) $(OBJS) $(LDSCRIPT)
	@$(LD) $(LDFLAGS) -o $@ $(PATCH_OBJS) $(OBJS) $(LDLIBS)

as: $(PATCH_OBJS) $(OBJS) $(LDSCRIPT)
	@$(LD) $(LDFLAGS) -o $@ $(PATCH_OBJS) $(OBJS) $(LDLIBS)

map: $(PATCH_OBJS) $(OBJS) $(LDSCRIPT)
	@$(LD) $(LDFLAGS) -Wl,-Map=$(BUILD)/patch.map $(OBJS) $(PATCH_OBJS) $(LDLIBS)

compile: $(BUILD)/patch.bin
