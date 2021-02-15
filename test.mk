BUILDROOT ?= .

# C_SRC   += basicmaths.c fastpow.c fastlog.c # sbrk.c heap_5.c 
# CPP_SRC = operators.cpp system_tables.cpp # message.cpp main.cpp 
# # CPP_SRC += PatchProcessor.cpp Patch.cpp 
# CPP_SRC += FloatArray.cpp ComplexFloatArray.cpp FastFourierTransform.cpp 
# CPP_SRC += ShortArray.cpp # ComplexShortArray.cpp ShortFastFourierTransform.cpp 
# CPP_SRC += Envelope.cpp VoltsPerOctave.cpp Window.cpp
# CPP_SRC += WavetableOscillator.cpp PolyBlepOscillator.cpp
# CPP_SRC += SmoothValue.cpp # PatchParameter.cpp
# CPP_SRC += MonochromeScreenPatch.cpp ColourScreenPatch.cpp
# CPP_SRC += Resource.cpp
# C_SRC += font.c

BUILD       ?= $(BUILDROOT)/Build

SOURCE       = $(BUILDROOT)/Source
LIBSOURCE    = $(BUILDROOT)/LibSource
GENSOURCE    = $(BUILD)/Source
TESTPATCHES  = $(BUILDROOT)/TestPatches
DSPLIB       = Libraries/CMSIS/DSP_Lib/Source
CPPFLAGS  = -g -Wall -m32
CPPFLAGS += -I$(SOURCE)
CPPFLAGS += -I$(PATCHSOURCE)
CPPFLAGS += -I$(LIBSOURCE)
CPPFLAGS += -I$(GENSOURCE)
CPPFLAGS += -I$(TESTPATCHES)
CPPFLAGS += -ILibraries -ILibraries/KissFFT
CPPFLAGS += -ILibraries/CMSIS/Include
CPPFLAGS +=  -DARM_MATH_CM0
CPPFLAGS +=  -fno-builtin -ffreestanding

# Tools
CC=$(TOOLROOT)gcc
LD=$(TOOLROOT)gcc
AR=$(TOOLROOT)ar
AS=$(TOOLROOT)as
NM=$(TOOLROOT)nm
CXX=$(TOOLROOT)g++
GDB=$(TOOLROOT)gdb
SIZE=$(TOOLROOT)size
RANLIB=$(TOOLROOT)ranlib
OBJCOPY=$(TOOLROOT)objcopy
OBJDUMP=$(TOOLROOT)objdump

LDLIBS   = -lm
LDFLAGS  = -Wl,--gc-sections

CXXFLAGS = -std=c++11

# object files
# OBJS  = $(C_SRC:%.c=$(BUILD)/Test/%.o) $(CPP_SRC:%.cpp=$(BUILD)/Test/%.o)

include $(BUILDROOT)/libs.mk
include $(BUILDROOT)/sources.mk
C_SRC += Libraries/KissFFT/kiss_fft.c
C_SRC += $(DSPLIB)/TransformFunctions/arm_bitreversal.c

# Set up search path
OBJS = $(addprefix $(BUILD)/Test/,$(notdir $(C_SRC:.c=.o)))
vpath %.c $(sort $(dir $(C_SRC)))
OBJS += $(addprefix $(BUILD)/Test/,$(notdir $(CPP_SRC:.cpp=.o)))
vpath %.cpp $(sort $(dir $(CPP_SRC)))

vpath %.cpp $(SOURCE) $(LIBSOURCE) $(GENSOURCE)
vpath %.c $(SOURCE) $(LIBSOURCE) $(GENSOURCE)

.PHONY: perform test

perform: $(TESTPATCHES)/PatchRun.cpp $(DEPS) $(OBJS)
	@$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(TESTPATCHES)/PatchRun.cpp -I$(BUILD) $(OBJS) -o $(BUILD)/Test/patch
	@$(BUILD)/Test/patch


test: $(TESTPATCHES)/PatchTest.cpp $(DEPS) $(OBJS)
	@$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(TESTPATCHES)/PatchTest.cpp -I$(BUILD) $(OBJS) -o $(BUILD)/Test/$@
	@$(BUILD)/Test/$@

# compile and generate dependency info
$(BUILD)/Test/%.o: %.c
	@$(CC) -c $(CPPFLAGS) $(CFLAGS) $< -o $@
	@$(CC) -MM -MT"$@" $(CPPFLAGS) $(CFLAGS) $< > $(@:.o=.d)

$(BUILD)/Test/%.o: %.cpp
	@$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@
	@$(CXX) -MM -MT"$@" $(CPPFLAGS) $(CXXFLAGS) $< > $(@:.o=.d)

-include $(OBJS:.o=.d) $(SOLO_OBJS:.o=.d) $(MULTI_OBJS:.o=.d)
