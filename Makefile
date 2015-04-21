TEMPLATEROOT = .

ifndef CONFIG
  CONFIG=Debug
endif

ifeq ($(CONFIG),Debug)
CFLAGS   = -g -Wall -Wcpp -Wunused-function -DDEBUG -DUSE_FULL_ASSERT
ASFLAGS  = -g
endif

ifeq ($(CONFIG),Release)
CFLAGS   = -O2
endif

CFLAGS  += -DEXTERNAL_SRAM
CFLAGS  += -nostdlib -nostartfiles -ffreestanding
CFLAGS  += -mtune=cortex-m4
CFLAGS  += -fpic -fpie
CFLAGS  += -fno-builtin
# CFLAGS  += -fpic
# CFLAGS  += -mpic-data-is-text-relative
CFLAGS += -fdata-sections 
# CFLAGS += -ffunction-sections
# CFLAGS += -fno-omit-frame-pointer
# CFLAGS  += -flto
CXXFLAGS = -fno-rtti -fno-exceptions -std=c++11 $(CFLAGS) 

LDFLAGS = -fpic -fpie
# LDFLAGS = -fpic
# LDFLAGS = -flto
LDFLAGS = -Wl,--gc-sections
# ASFLAGS  = -g
LDLIBS   = -lm
LDSCRIPT = Source/flash.ld
FIRMWARESENDER = Tools/FirmwareSender

C_SRC   = errorhandlers.c gpio.c eepromcontrol.c basicmaths.c # myalloc.c
CPP_SRC = main.cpp operators.cpp
OWL_SRC = StompBox.cpp PatchProcessor.cpp
SOLO_SRC = SoloProgram.cpp
MULTI_SRC = PatchController.cpp PatchRegistry.cpp MultiProgram.cpp

OBJS =  $(C_SRC:%.c=Build/%.o) $(CPP_SRC:%.cpp=Build/%.o)

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
# OBJS += $(DSPLIB)/SupportFunctions/arm_float_to_q31.o
# OBJS += $(DSPLIB)/SupportFunctions/arm_q31_to_float.o
# OBJS += $(DSPLIB)/SupportFunctions/arm_float_to_q15.o
# OBJS += $(DSPLIB)/SupportFunctions/arm_q15_to_float.o

# include common make file
include $(TEMPLATEROOT)/Makefile.f4

all: multi

# Build executable 
$(BUILD)/solo.elf : $(SOLO_OBJS) $(OBJS) $(LDSCRIPT)
	$(LD) $(LDFLAGS) -o $@ $(SOLO_OBJS) $(OBJS) $(LDLIBS)

$(BUILD)/solo.as : $(SOLO_OBJS) $(OBJS) $(LDSCRIPT)
	$(LD) $(LDFLAGS) -o $@ $(SOLO_OBJS) $(OBJS) $(LDLIBS)

$(BUILD)/solo.map : $(SOLO_OBJS) $(OBJS) $(LDSCRIPT)
	$(LD) $(LDFLAGS) -o $@ $(SOLO_OBJS) $(OBJS) $(LDLIBS)

$(BUILD)/multi.elf : $(MULTI_OBJS) $(OBJS) $(LDSCRIPT)
	$(LD) $(LDFLAGS) -o $@ $(MULTI_OBJS) $(OBJS) $(LDLIBS)

$(BUILD)/blinky.elf : $(BLINKY_OBJS) $(OBJS) $(LDSCRIPT)
	$(LD) $(LDFLAGS) -o $@ $(BLINKY_OBJS) $(OBJS) $(LDLIBS)

$(BUILD)/%.syx : $(BUILD)/%.bin
	$(FIRMWARESENDER) -q -in $< -save $@

solo : $(BUILD)/solo.bin
	$(FIRMWARESENDER) -in  $< -out "OWL FS"

solomap : $(BUILD)/solo.elf
	$(LD) $(LDFLAGS) -Wl,-Map=Build/solo.map $(OBJS) $(SOLO_OBJS) $(LDLIBS)

multi: $(BUILD)/multi.bin
	$(FIRMWARESENDER) -in  $< -out "OWL FS"

blinky: $(BUILD)/blinky.bin
	$(FIRMWARESENDER) -in  $< -out "OWL FS"

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

$(BUILD)/program.a : $(BUILD)/program.a($(SOLO_OBJS) $(OBJS) OwlProgram.o)
	$(RANLIB) $(BUILD)/program.a
