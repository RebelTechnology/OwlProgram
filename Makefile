TEMPLATEROOT = .

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

PATCHNAME ?= "OWL Patch"
SLOT ?= 0

CFLAGS += -DEXTERNAL_SRAM
CFLAGS += -nostdlib -nostartfiles -fno-builtin -ffreestanding
CFLAGS += -mtune=cortex-m4
CFLAGS += -fpic
CFLAGS += -fpie
CFLAGS += -fdata-sections 
CFLAGS += -ffunction-sections
# CFLAGS +=  -munaligned-access
CFLAGS +=  -mno-unaligned-access
# CFLAGS += –mlong-calls

# CFLAGS += -mpic-data-is-text-relative
CFLAGS += -fno-omit-frame-pointer
CFLAGS += -flto

CXXFLAGS = -fno-rtti -fno-exceptions -std=c++11 $(CFLAGS) 

LDFLAGS  = -Wl,--gc-sections
LDFLAGS += -fpic
LDFLAGS += -fpie
LDFLAGS += -flto

LDLIBS   = -lm
LDSCRIPT = Source/flash.ld
FIRMWARESENDER = Tools/FirmwareSender -s 240

C_SRC   = basicmaths.c
CPP_SRC = main.cpp operators.cpp message.cpp StompBox.cpp PatchProcessor.cpp 
CPP_SRC += FloatArray.cpp ComplexFloatArray.cpp
CPP_SRC += PatchProgram.cpp

OBJS =  $(C_SRC:%.c=Build/%.o) $(CPP_SRC:%.cpp=Build/%.o)

# object files
OBJS += $(BUILD)/stm32f4xx_flash.o
OBJS += $(BUILD)/startup.o
OBJS += $(SYSCALLS)
OBJS += $(DSPLIB)/FastMathFunctions/arm_sin_f32.o
OBJS += $(DSPLIB)/FastMathFunctions/arm_cos_f32.o
OBJS += $(DSPLIB)/CommonTables/arm_common_tables.o

OBJS += $(DSPLIB)/ComplexMathFunctions/arm_cmplx_conj_f32.o
OBJS += $(DSPLIB)/ComplexMathFunctions/arm_cmplx_dot_prod_f32.o
OBJS += $(DSPLIB)/ComplexMathFunctions/arm_cmplx_mag_f32.o
OBJS += $(DSPLIB)/ComplexMathFunctions/arm_cmplx_mag_squared_f32.o
OBJS += $(DSPLIB)/ComplexMathFunctions/arm_cmplx_mult_cmplx_f32.o
OBJS += $(DSPLIB)/ComplexMathFunctions/arm_cmplx_mult_real_f32.o

OBJS += $(DSPLIB)/TransformFunctions/arm_cfft_f32.o
OBJS += $(DSPLIB)/TransformFunctions/arm_cfft_radix8_f32.o
OBJS += $(DSPLIB)/TransformFunctions/arm_bitreversal2.o
OBJS += $(DSPLIB)/TransformFunctions/arm_rfft_fast_f32.o
OBJS += $(DSPLIB)/TransformFunctions/arm_rfft_fast_init_f32.o
OBJS += $(DSPLIB)/CommonTables/arm_const_structs.o

OBJS += $(DSPLIB)/FilteringFunctions/arm_biquad_cascade_df1_init_f32.o
OBJS += $(DSPLIB)/FilteringFunctions/arm_biquad_cascade_df1_f32.o
OBJS += $(DSPLIB)/FilteringFunctions/arm_biquad_cascade_df2T_init_f32.o
OBJS += $(DSPLIB)/FilteringFunctions/arm_biquad_cascade_df2T_f32.o

OBJS += $(DSPLIB)/SupportFunctions/arm_float_to_q31.o
OBJS += $(DSPLIB)/SupportFunctions/arm_q31_to_float.o
OBJS += $(DSPLIB)/SupportFunctions/arm_float_to_q15.o
OBJS += $(DSPLIB)/SupportFunctions/arm_q15_to_float.o

OBJS += $(DSPLIB)/BasicMathFunctions/arm_scale_f32.o

# include common make file
include $(TEMPLATEROOT)/Makefile.f4

# Heavy 
CFLAGS += -D__unix__ -DHV_SIMD_NONE

PATCHSOURCE = $(TEMPLATEROOT)/PatchSource
LIBSOURCE = $(TEMPLATEROOT)/LibSource
CFLAGS += -ILibSource
CFLAGS += -I$(BUILD)
CFLAGS += -ILibraries/OwlPatches
PATCH_C_SRC = $(wildcard $(PATCHSOURCE)/*.c) 
PATCH_CPP_SRC += $(wildcard $(PATCHSOURCE)/*.cpp)
PATCH_OBJS += $(addprefix Build/, $(notdir $(PATCH_C_SRC:.c=.o)))
PATCH_OBJS += $(addprefix Build/, $(notdir $(PATCH_CPP_SRC:.cpp=.o)))
vpath %.cpp $(LIBSOURCE)
vpath %.c $(LIBSOURCE)
vpath %.s $(LIBSOURCE)
vpath %.cpp $(PATCHSOURCE)
vpath %.c $(PATCHSOURCE)
vpath %.s $(PATCHSOURCE)

all: patch

.PHONY: prep clean upload store

$(BUILD)/progname.s:
	echo '.string "'$(PATCHNAME)'"' > $@

$(BUILD)/patch.h:
	echo '#include "'$(PATCHNAME)'Patch.hpp"' > $@

$(BUILD)/patch.cpp:
	echo 'REGISTER_PATCH('$(PATCHNAME)'Patch, "'$(PATCHNAME)'", 2, 2);' > $@

prep: $(BUILD)/patch.h $(BUILD)/patch.cpp $(BUILD)/progname.s
	echo Building patch $(PATCHNAME)

# Build executable 
$(BUILD)/patch.elf : prep $(PATCH_OBJS) $(OBJS) $(LDSCRIPT) 
	$(LD) $(LDFLAGS) -o $@ $(PATCH_OBJS) $(OBJS) $(LDLIBS)

$(BUILD)/patch.as : $(PATCH_OBJS) $(OBJS) $(LDSCRIPT)
	$(LD) $(LDFLAGS) -o $@ $(PATCH_OBJS) $(OBJS) $(LDLIBS)

$(BUILD)/patch.map : $(PATCH_OBJS) $(OBJS) $(LDSCRIPT)
	$(LD) $(LDFLAGS) -Wl,-Map=Build/patch.map $(OBJS) $(PATCH_OBJS) $(LDLIBS)

$(BUILD)/%.syx : $(BUILD)/%.bin
	$(FIRMWARESENDER) -q -in $< -save $@

patch: $(BUILD)/patch.bin

sysex: $(BUILD)/%.syx

upload: $(BUILD)/patch.bin
	$(FIRMWARESENDER) -in  $< -out "OWL FS"

store: $(BUILD)/patch.bin
	$(FIRMWARESENDER) -in  $< -out "OWL FS" -store $(SLOT)
