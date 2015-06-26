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

CFLAGS += -DARM_CORTEX
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

C_SRC   = basicmaths.c # myalloc.c eepromcontrol.c errorhandlers.c gpio.c 
CPP_SRC = main.cpp operators.cpp message.cpp
OWL_SRC = StompBox.cpp PatchProcessor.cpp
SOLO_SRC = SoloProgram.cpp
MULTI_SRC = PatchController.cpp PatchRegistry.cpp MultiProgram.cpp

OBJS =  $(C_SRC:%.c=Build/%.o) $(CPP_SRC:%.cpp=Build/%.o)

# OBJS += Libraries/OwlPatches/retuner.o
# OBJS += Libraries/OwlPatches/Retune/zita-resampler/resampler.o
# OBJS += Libraries/OwlPatches/Retune/zita-resampler/resampler-table.o
# OBJS += Libraries/kiss_fft130/kiss_fft.o

SOLO_OBJS = $(OWL_SRC:%.cpp=Build/%.o) $(SOLO_SRC:%.cpp=Build/%.o)
MULTI_OBJS = $(OWL_SRC:%.cpp=Build/%.o) $(MULTI_SRC:%.cpp=Build/%.o)
BLINKY_OBJS = $(BUILD)/BlinkyProgram.o 

# object files
OBJS += $(BUILD)/stm32f4xx_flash.o
# OBJS += $(PERIPH) 
OBJS += $(BUILD)/startup.o # no system_hse.o: clocks and ram set by loader
#OBJS += $(USB_DEVICE) $(USB_OTG)
OBJS += $(SYSCALLS)
#OBJS += $(BUILD)/misc.o
OBJS += # $(BUILD)/stm32f4xx_gpio.o $(BUILD)/stm32f4xx_rcc.o
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

OBJS += $(DSPLIB)/SupportFunctions/arm_float_to_q31.o
OBJS += $(DSPLIB)/SupportFunctions/arm_q31_to_float.o
OBJS += $(DSPLIB)/SupportFunctions/arm_float_to_q15.o
OBJS += $(DSPLIB)/SupportFunctions/arm_q15_to_float.o

# include common make file
include $(TEMPLATEROOT)/Makefile.f4

all: multi

# Heavy 
HEAVY_SRC = HeavyProgram.cpp
HEAVY_OBJS = $(OWL_SRC:%.cpp=Build/%.o) $(HEAVY_SRC:%.cpp=Build/%.o)
HEAVY_FILES = $(wildcard HeavySource/*.c)
HEAVY_OBJS += $(addprefix Build/, $(notdir $(HEAVY_FILES:.c=.o)))
vpath %.c $(TEMPLATEROOT)/HeavySource
CFLAGS += -D__unix__ -DHV_SIMD_NONE

$(BUILD)/heavy.elf : $(HEAVY_OBJS) $(OBJS) $(LDSCRIPT)
	$(LD) $(LDFLAGS) -o $@ $(HEAVY_OBJS) $(OBJS) $(LDLIBS)

heavy:  $(BUILD)/heavy.bin
	$(FIRMWARESENDER) -in  $< -out "OWL FS"

# Build executable 
$(BUILD)/solo.elf : $(SOLO_OBJS) $(OBJS) $(LDSCRIPT) Source/progname.s
	$(LD) $(LDFLAGS) -o $@ $(SOLO_OBJS) $(OBJS) $(LDLIBS)
# $(BUILD)/solo.elf : $(SOLO_OBJS) $(OBJS) $(HEAVY_OBJS) $(LDSCRIPT)
# 	$(LD) $(LDFLAGS) -o $@ $(SOLO_OBJS) $(OBJS) $(HEAVY_OBJS) $(LDLIBS)

$(BUILD)/solo.as : $(SOLO_OBJS) $(OBJS) $(LDSCRIPT)
	$(LD) $(LDFLAGS) -o $@ $(SOLO_OBJS) $(OBJS) $(LDLIBS)

$(BUILD)/solo.map : $(SOLO_OBJS) $(OBJS) $(LDSCRIPT)
	$(LD) $(LDFLAGS) -Wl,-Map=Build/solo.map $(OBJS) $(SOLO_OBJS) $(LDLIBS)

$(BUILD)/multi.elf : $(MULTI_OBJS) $(OBJS) $(LDSCRIPT)
	$(LD) $(LDFLAGS) -o $@ $(MULTI_OBJS) $(OBJS) $(LDLIBS)

$(BUILD)/blinky.elf : $(BLINKY_OBJS) $(OBJS) $(LDSCRIPT)
	$(LD) $(LDFLAGS) -o $@ $(BLINKY_OBJS) $(OBJS) $(LDLIBS)

$(BUILD)/%.syx : $(BUILD)/%.bin
	$(FIRMWARESENDER) -q -in $< -save $@

solo : $(BUILD)/solo.bin $(BUILD)/solo.map
	$(FIRMWARESENDER) -in  $< -out "OWL FS"

multi: $(BUILD)/multi.bin
	$(FIRMWARESENDER) -in  $< -out "OWL FS"

blinky: $(BUILD)/blinky.bin
	$(FIRMWARESENDER) -in  $< -out "OWL FS"

$(BUILD)/program.a : $(BUILD)/program.a($(SOLO_OBJS) $(OBJS) OwlProgram.o)
	$(RANLIB) $(BUILD)/program.a
