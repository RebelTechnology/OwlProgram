BUILDROOT ?= .

C_SRC   = Source/heap_5.c
# C_SRC   += sbrk.c

SOURCE       = $(BUILDROOT)/Source
LIBSOURCE    = $(BUILDROOT)/LibSource
GENSOURCE    = $(BUILD)/Source
TESTPATCHES  = $(BUILDROOT)/TestPatches
DAISYSP      = $(BUILDROOT)/Libraries/DaisySP/Source
CPPFLAGS += -I$(PATCHSOURCE)
CPPFLAGS += -I$(LIBSOURCE)
CPPFLAGS += -I$(GENSOURCE)
CPPFLAGS += -I$(TESTPATCHES)
CPPFLAGS += -I$(DAISYSP)
CPPFLAGS += -I$(DAISYSP)/Control
CPPFLAGS += -I$(DAISYSP)/Drums
CPPFLAGS += -I$(DAISYSP)/Dynamics
CPPFLAGS += -I$(DAISYSP)/Effects
CPPFLAGS += -I$(DAISYSP)/Filters
CPPFLAGS += -I$(DAISYSP)/Noise
CPPFLAGS += -I$(DAISYSP)/PhysicalModeling
CPPFLAGS += -I$(DAISYSP)/Synthesis
CPPFLAGS += -I$(DAISYSP)/Utility

PATCH_S_SRC = $(SOURCE)/startup.s
PATCH_C_SRC    = $(wildcard $(PATCHSOURCE)/*.c)
PATCH_CPP_SRC += $(wildcard $(PATCHSOURCE)/*.cpp)
PATCH_CPP_SRC += PatchProgram.cpp
ifdef GEN
PATCH_C_SRC   += $(wildcard $(GENSOURCE)/*.c)
PATCH_CPP_SRC += $(wildcard $(GENSOURCE)/*.cpp)
endif
ifdef MAXIMILIAN
PATCH_CPP_SRC := $(filter-out $(PATCHSOURCE)/$(MAXIMILIAN).cpp, $(PATCH_CPP_SRC))
endif
PATCH_OBJS += $(addprefix $(BUILD)/, $(notdir $(PATCH_S_SRC:.s=.o)))
PATCH_OBJS += $(addprefix $(BUILD)/, $(notdir $(PATCH_C_SRC:.c=.o)))
PATCH_OBJS += $(addprefix $(BUILD)/, $(notdir $(PATCH_CPP_SRC:.cpp=.o)))
DAISYSP_CPP_SRC = $(wildcard $(DAISYSP)/*/*.cpp)
DAISYSP_OBJS = $(addprefix $(BUILD)/, $(notdir $(DAISYSP_CPP_SRC:.cpp=.o)))

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

CXXFLAGS = -fno-rtti
CXXFLAGS += -fno-exceptions

ifdef HEAVY
CPPFLAGS    += -D__unix__ -DHV_SIMD_NONE
endif

# include common make file
include $(BUILDROOT)/libs.mk
include $(BUILDROOT)/sources.mk
include $(BUILDROOT)/common.mk

CPP_SRC += LibSource/Patch.cpp
CPP_SRC += LibSource/ComplexShortArray.cpp
CPP_SRC += LibSource/ShortFastFourierTransform.cpp 
CPP_SRC += Source/PatchProcessor.cpp
CPP_SRC += Source/main.cpp Source/operators.cpp Source/message.cpp
CPP_SRC += Source/system_tables.cpp

# Set up search path
OBJS = $(addprefix $(BUILD)/,$(notdir $(C_SRC:.c=.o)))
OBJS += $(addprefix $(BUILD)/,$(notdir $(CPP_SRC:.cpp=.o)))
OBJS += $(addprefix $(BUILD)/,$(notdir $(S_SRC:.S=.o)))
vpath %.c $(sort $(dir $(C_SRC)))
vpath %.cpp $(sort $(dir $(CPP_SRC)))
vpath %.s $(sort $(dir $(S_SRC)))
vpath %.S $(sort $(dir $(S_SRC)))
vpath %.cpp $(sort $(dir $(DAISYSP_CPP_SRC)))

# Set up search path
vpath %.cpp $(SOURCE)
vpath %.c $(SOURCE)
vpath %.s $(SOURCE)
vpath %.cpp $(PATCHSOURCE)
vpath %.c $(PATCHSOURCE)
vpath %.s $(PATCHSOURCE)
vpath %.cpp $(GENSOURCE)
vpath %.c $(GENSOURCE)
vpath %.s $(GENSOURCE)

vpath %.c Libraries/syscalls

.PHONY: libs as map compile

$(BUILD)/ShortPatchProgram.o: $(SOURCE)/ShortPatchProgram.cpp $(DEPS)
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) -I$(BUILD) $(SOURCE)/ShortPatchProgram.cpp -o $@
	$(CXX) -MM -MT"$@" $(CPPFLAGS) $(CXXFLAGS) -I$(BUILD) $(SOURCE)/ShortPatchProgram.cpp > $(@:.o=.d)

$(BUILD)/PatchProgram.o: $(SOURCE)/PatchProgram.cpp $(DEPS)
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) -I$(BUILD) $(SOURCE)/PatchProgram.cpp -o $@
	$(CXX) -MM -MT"$@" $(CPPFLAGS) $(CXXFLAGS) -I$(BUILD) $(SOURCE)/PatchProgram.cpp > $(@:.o=.d)

Libraries/libowlprg.a: $(OBJS)
	$(AR) rcs $@ $^

Libraries/libdaisysp.a: $(DAISYSP_OBJS)
	@$(AR) rcs $@ $^

$(BUILD)/$(TARGET).elf: $(PATCH_OBJS) $(LDSCRIPT)
	@$(LD) $(LDFLAGS) -o $@ $(PATCH_OBJS) $(LDLIBS) Libraries/libdaisysp.a Libraries/libowlprg.a

libs: Libraries/libowlprg.a Libraries/libdaisysp.a

as: $(BUILD)/$(TARGET).elf
	@$(OBJDUMP) -S $< > $(BUILD)/$(TARGET).s

map: $(PATCH_OBJS) $(LDSCRIPT)
	@$(LD) $(LDFLAGS) -Wl,-Map=$(BUILD)/$(TARGET).map $(PATCH_OBJS) $(LDLIBS) Libraries/libdaisysp.a Libraries/libowlprg.a

compile: $(BUILD)/$(TARGET).bin
