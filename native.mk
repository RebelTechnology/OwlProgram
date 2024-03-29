
ifeq ($(CONFIG),Debug)
CPPFLAGS    ?= -g -O0 -Wall -Wcpp -Wunused-function -DDEBUG -DUSE_FULL_ASSERT
endif

ifeq ($(CONFIG),Release)
CPPFLAGS    ?= -Os -ffast-math
endif

BUILDROOT   ?= .
BUILD       ?= $(BUILDROOT)/Build
PATCHSOURCE ?= $(BUILDROOT)/PatchSource
SOURCE       = $(BUILDROOT)/Source
LIBSOURCE    = $(BUILDROOT)/LibSource
BUILDSOURCE  = $(BUILD)/Source
TESTPATCHES  = $(BUILDROOT)/TestPatches
DSPINC       = $(BUILDROOT)/Libraries/CMSIS/DSP/Include
DSPLIB       = $(BUILDROOT)/Libraries/CMSIS/DSP/Source
DAISYSP      = $(BUILDROOT)/Libraries/DaisySP/Source
CPPFLAGS    += -I$(SOURCE)
CPPFLAGS    += -I$(PATCHSOURCE)
CPPFLAGS    += -I$(LIBSOURCE)
CPPFLAGS    += -I$(BUILDSOURCE)
CPPFLAGS    += -I$(TESTPATCHES)
CPPFLAGS    += -I$(DSPINC)
CPPFLAGS    += -I$(DAISYSP)
CPPFLAGS    += -I$(DAISYSP)/Control
CPPFLAGS    += -I$(DAISYSP)/Drums
CPPFLAGS    += -I$(DAISYSP)/Dynamics
CPPFLAGS    += -I$(DAISYSP)/Effects
CPPFLAGS    += -I$(DAISYSP)/Filters
CPPFLAGS    += -I$(DAISYSP)/Noise
CPPFLAGS    += -I$(DAISYSP)/PhysicalModeling
CPPFLAGS    += -I$(DAISYSP)/Synthesis
CPPFLAGS    += -I$(DAISYSP)/Utility
CPPFLAGS    += -ILibraries -ILibraries/KissFFT
CPPFLAGS    += -D__PROGRAM_START=1 # prevent compilation of __cmsis_start function
CPPFLAGS    += "-DPATCHNAME=\"$(PATCHNAME)\""
CXXFLAGS     = -std=gnu++17
LDLIBS       = -lm
LDFLAGS      = -Wl,--gc-sections

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

# Uncomment lines below to build in 32-bit mode
# include $(BUILDROOT)/libs.mk
# CPPFLAGS += -ILibraries/CMSIS/Include
# CPPFLAGS += -DARM_MATH_CM0
# CPPFLAGS += -fno-builtin -ffreestanding -m32
# C_SRC += $(DSPLIB)/TransformFunctions/arm_bitreversal.c

include $(BUILDROOT)/sources.mk
C_SRC   += Libraries/KissFFT/kiss_fft.c
C_SRC   += $(wildcard $(BUILDSOURCE)/*.c)
CPP_SRC += $(wildcard $(BUILDSOURCE)/*.cpp)
C_SRC   += $(wildcard $(PATCHSOURCE)/*.c)
CPP_SRC += $(wildcard $(PATCHSOURCE)/*.cpp)
CPP_SRC += $(wildcard $(DAISYSP)/*/*.cpp)
ifdef MAXIMILIAN
CPP_SRC := $(filter-out $(PATCHSOURCE)/$(MAXIMILIAN).cpp, $(CPP_SRC))
endif

# Set up search path
OBJS = $(addprefix $(BUILD)/Test/,$(notdir $(C_SRC:.c=.o)))
OBJS += $(addprefix $(BUILD)/Test/,$(notdir $(CPP_SRC:.cpp=.o)))

vpath %.c $(sort $(dir $(C_SRC)))
vpath %.cpp $(sort $(dir $(CPP_SRC)))
vpath %.cpp $(SOURCE) $(LIBSOURCE) $(BUILDSOURCE)
vpath %.c $(SOURCE) $(LIBSOURCE) $(BUILDSOURCE)

.PHONY: native run test

native: $(TESTPATCHES)/PatchRun.cpp $(DEPS) $(OBJS)
	@$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(TESTPATCHES)/PatchRun.cpp -I$(BUILD) $(OBJS) -o $(BUILD)/Test/patch
	echo Built $(PATCHNAME) native executable in $(BUILD)/Test/patch

run: native
	@$(BUILD)/Test/patch

grind: native
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes $(BUILD)/Test/patch

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
