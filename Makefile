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

CXXFLAGS = -fno-rtti -fno-exceptions -std=c++11 $(CFLAGS) 

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

OBJS =  $(C_SRC:%.c=Build/%.o) $(CPP_SRC:%.cpp=Build/%.o)

# object files
OBJS += $(BUILD)/startup.o
OBJS += $(BUILD)/libnosys_gnu.o
OBJS += $(DSPLIB)/FastMathFunctions/arm_sin_f32.o
OBJS += $(DSPLIB)/FastMathFunctions/arm_cos_f32.o
OBJS += $(DSPLIB)/CommonTables/arm_common_tables.o
OBJS += $(DSPLIB)/CommonTables/arm_const_structs.o

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

OBJS += $(DSPLIB)/FilteringFunctions/arm_biquad_cascade_df1_init_f32.o
OBJS += $(DSPLIB)/FilteringFunctions/arm_biquad_cascade_df1_f32.o
OBJS += $(DSPLIB)/FilteringFunctions/arm_biquad_cascade_df2T_init_f32.o
OBJS += $(DSPLIB)/FilteringFunctions/arm_biquad_cascade_df2T_f32.o
OBJS += $(DSPLIB)/FilteringFunctions/arm_correlate_f32.o
OBJS += $(DSPLIB)/FilteringFunctions/arm_conv_f32.o
OBJS += $(DSPLIB)/FilteringFunctions/arm_conv_partial_f32.o
OBJS += $(DSPLIB)/FilteringFunctions/arm_lms_init_f32.o
OBJS += $(DSPLIB)/FilteringFunctions/arm_lms_f32.o
OBJS += $(DSPLIB)/FilteringFunctions/arm_lms_norm_f32.o
OBJS += $(DSPLIB)/FilteringFunctions/arm_lms_norm_init_f32.o
OBJS += $(DSPLIB)/FilteringFunctions/arm_fir_f32.o
OBJS += $(DSPLIB)/FilteringFunctions/arm_fir_init_f32.o
OBJS += $(DSPLIB)/FilteringFunctions/arm_fir_decimate_f32.o
OBJS += $(DSPLIB)/FilteringFunctions/arm_fir_decimate_init_f32.o
OBJS += $(DSPLIB)/FilteringFunctions/arm_fir_interpolate_f32.o
OBJS += $(DSPLIB)/FilteringFunctions/arm_fir_interpolate_init_f32.o

OBJS += $(DSPLIB)/SupportFunctions/arm_float_to_q31.o
OBJS += $(DSPLIB)/SupportFunctions/arm_q31_to_float.o
OBJS += $(DSPLIB)/SupportFunctions/arm_float_to_q15.o
OBJS += $(DSPLIB)/SupportFunctions/arm_q15_to_float.o
OBJS += $(DSPLIB)/SupportFunctions/arm_copy_f32.o
OBJS += $(DSPLIB)/SupportFunctions/arm_fill_f32.o

OBJS += $(DSPLIB)/BasicMathFunctions/arm_abs_f32.o
OBJS += $(DSPLIB)/BasicMathFunctions/arm_add_f32.o
OBJS += $(DSPLIB)/BasicMathFunctions/arm_dot_prod_f32.o
OBJS += $(DSPLIB)/BasicMathFunctions/arm_mult_f32.o
OBJS += $(DSPLIB)/BasicMathFunctions/arm_negate_f32.o
OBJS += $(DSPLIB)/BasicMathFunctions/arm_scale_f32.o
OBJS += $(DSPLIB)/BasicMathFunctions/arm_sub_f32.o

OBJS += $(DSPLIB)/StatisticsFunctions/arm_max_f32.o
OBJS += $(DSPLIB)/StatisticsFunctions/arm_mean_f32.o
OBJS += $(DSPLIB)/StatisticsFunctions/arm_min_f32.o
OBJS += $(DSPLIB)/StatisticsFunctions/arm_power_f32.o
OBJS += $(DSPLIB)/StatisticsFunctions/arm_rms_f32.o
OBJS += $(DSPLIB)/StatisticsFunctions/arm_std_f32.o
OBJS += $(DSPLIB)/StatisticsFunctions/arm_var_f32.o

# Heavy defines
CFLAGS += -D__unix__ -DHV_SIMD_NONE

LIBSOURCE = $(BUILDROOT)/LibSource
PATCHSOURCE = $(BUILDROOT)/PatchSource
TESTPATCHES = $(BUILDROOT)/TestPatches
CFLAGS += -I$(LIBSOURCE)
CFLAGS += -I$(PATCHSOURCE)
CFLAGS += -I$(TESTPATCHES)
CFLAGS += -I$(BUILD)
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

# include common make file
include $(BUILDROOT)/common.mk

.PHONY: prep clean realclean run store online docs

prep:
	@echo Building patch $(PATCHNAME)
	echo '.string "'$(PATCHNAME)'"' > $(BUILD)/progname.s
	echo "#include \"$(PATCHFILE)\"" > $(BUILD)/patch.h
	echo "REGISTER_PATCH($(PATCHCLASS), \"$(PATCHNAME)\", $(PATCHIN), $(PATCHOUT));" > $(BUILD)/patch.cpp

# Build executable 
$(BUILD)/patch.elf : $(PATCH_OBJS) $(OBJS) $(LDSCRIPT) 
	$(LD) $(LDFLAGS) -o $@ $(PATCH_OBJS) $(OBJS) $(LDLIBS)

$(BUILD)/patch.as : $(PATCH_OBJS) $(OBJS) $(LDSCRIPT)
	$(LD) $(LDFLAGS) -o $@ $(PATCH_OBJS) $(OBJS) $(LDLIBS)

$(BUILD)/patch.map : $(PATCH_OBJS) $(OBJS) $(LDSCRIPT)
	$(LD) $(LDFLAGS) -Wl,-Map=Build/patch.map $(OBJS) $(PATCH_OBJS) $(LDLIBS)

$(BUILD)/%.syx : $(BUILD)/%.bin
	$(FIRMWARESENDER) -q -in $< -save $@

$(BUILD)/%Patch.hpp: PatchSource/%.dsp
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
	echo "$(ONLINE_INCLUDES)" > $(BUILD)/patch.h
	echo "$(ONLINE_REGISTER)" > $(BUILD)/patch.cpp
	echo '.string "OnlineCompiler"' > $(BUILD)/progname.s
	make $(BUILD)/patch.syx
	cp $(BUILD)/patch.syx $(BUILD)/online.syx

web: $(PATCH_OBJS) $(OBJS)
	emcc -g -ISource -IPatchSource -ILibSource -IBuild -ITestPatches Source/PatchProgram.cpp -s EXPORTED_FUNCTIONS="['_WEB_setup','_WEB_setParameter','_WEB_processBlock','_WEB_getPatchName','_WEB_getParameterName','_WEB_getMessage']" Source/PatchProcessor.cpp Source/web.cpp Source/operators.cpp Source/message.cpp LibSource/StompBox.cpp -o Build/patch.js
