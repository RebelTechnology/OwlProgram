TEMPLATEROOT = .

CFLAGS = -g 
# CFLAGS   = -O2 
# CFLAGS   = -O1
CFLAGS  += -Wall -Wcpp -DUSE_FULL_ASSERT -D__FPU_PRESENT=1 -D__FPU_USED=1
CFLAGS  += -DEXTERNAL_SRAM
CXXFLAGS = -fno-rtti -fno-exceptions -std=c++11 $(CFLAGS) 
ASFLAGS  = -g
LDLIBS   = -lm
# LDSCRIPT = Source/sram.ld
# LDSCRIPT = Source/sram-in-flash.ld
LDSCRIPT = Source/flash.ld

C_SRC   = errorhandlers.c gpio.c eepromcontrol.c basicmaths.c myalloc.c
CPP_SRC = main.cpp
CPP_SRC += OwlProgram.cpp StompBox.cpp PatchController.cpp operators.cpp
CPP_SRC += ApplicationSettings.cpp PatchProcessor.cpp PatchRegistry.cpp

OBJS =  $(C_SRC:%.c=Build/%.o)  $(CPP_SRC:%.cpp=Build/%.o)

# object files
OBJS += $(BUILD)/stm32f4xx_flash.o
# OBJS += $(PERIPH) 
OBJS += $(BUILD)/startup.o # no system_hse.o: clocks and ram set by loader
# OBJS += $(USB_DEVICE) $(USB_OTG)
OBJS += $(SYSCALLS)
OBJS += $(BUILD)/misc.o
OBJS += $(BUILD)/stm32f4xx_gpio.o $(BUILD)/stm32f4xx_rcc.o 
OBJS += $(DSPLIB)/FastMathFunctions/arm_sin_f32.o
OBJS += $(DSPLIB)/FastMathFunctions/arm_cos_f32.o
# OBJS += $(DSPLIB)/SupportFunctions/arm_float_to_q31.o
# OBJS += $(DSPLIB)/SupportFunctions/arm_q31_to_float.o
# OBJS += $(DSPLIB)/SupportFunctions/arm_float_to_q15.o
# OBJS += $(DSPLIB)/SupportFunctions/arm_q15_to_float.o

# include common make file
include $(TEMPLATEROOT)/Makefile.f4
