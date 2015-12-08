# Code Paths
DSPLIB=Libraries/CMSIS/DSP_Lib/Source

# Tool path
#TOOLROOT ?= Tools/gcc-arm-none-eabi-4_9-2015q3/bin

CMSIS ?= Libraries/CMSIS/Include/

# Tools
CC=arm-none-eabi-gcc
CXX=arm-none-eabi-g++
LD=arm-none-eabi-gcc
AR=arm-none-eabi-ar
AS=arm-none-eabi-as
RANLIB=arm-none-eabi-ranlib
GDB=arm-none-eabi-gdb
OBJCOPY=arm-none-eabi-objcopy
OBJDUMP=arm-none-eabi-objdump
SIZE=arm-none-eabi-size


# Compilation Flags
ARCH_FLAGS = -mcpu=cortex-m4 -mthumb
ARCH_FLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
# ARCH_FLAGS += -mfloat-abi=soft -msoft-float
ARCH_FLAGS += -fsingle-precision-constant
DEF_FLAGS = -DARM_MATH_CM4 -DSTM32F4XX -D__FPU_PRESENT -D__FPU_USED=1
# DEF_FLAGS = -DUSE_STDPERIPH_DRIVER -DARM_MATH_CM4 -DSTM32F4XX
INC_FLAGS = -I$(BUILDROOT)/Libraries -I$(DEVICE) -I$(CMSIS) -I$(PERIPH_FILE)/inc -I$(SOURCE)
INC_FLAGS += -I$(DEVICE)/Include -I$(CMSIS)
INC_FLAGS += -I$(USB_DEVICE_FILE)/Core/inc -I$(USB_DEVICE_FILE)/Class/cdc/inc
INC_FLAGS += -I$(USB_OTG_FILE)/inc
CPPFLAGS += $(ARCH_FLAGS) $(INC_FLAGS) $(DEF_FLAGS)
CFLAGS += -fno-builtin -std=c99
LDFLAGS += -T$(LDSCRIPT) $(ARCH_FLAGS)

# Build executable 
$(ELF) : $(OBJS) $(LDSCRIPT)
	@$(LD) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)

# compile and generate dependency info
$(BUILD)/%.o: %.c
	@$(CC) -c $(CPPFLAGS) $(CFLAGS) $< -o $@
	@$(CC) -MM -MT"$@" $(CPPFLAGS) $(CFLAGS) $< > $(@:.o=.d)

$(BUILD)/%.o: %.cpp
	@$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@
	@$(CXX) -MM -MT"$@" $(CPPFLAGS) $(CXXFLAGS) $< > $(@:.o=.d)

$(BUILD)/%.o: %.s
	@$(CC) -c $(CPPFLAGS) $(CFLAGS) $< -o $@

$(BUILD)/%.s: %.c
	@$(CC) -S $(CPPFLAGS) $(CFLAGS) $< -o $@

$(BUILD)/%.s: %.cpp
	@$(CXX) -S $(CPPFLAGS) $(CXXFLAGS) $< -o $@

$(BUILD)/%.bin: $(BUILD)/%.elf
	@$(OBJCOPY) -O binary $< $@

$(BUILD)/%.s: $(BUILD)/%.elf
	@$(OBJDUMP) -S $< > $@

clean:
	@rm -rf $(BUILD)/*

realclean: clean
	@find Libraries/ -name '*.o' -delete

# pull in dependencies
-include $(OBJS:.o=.d) $(SOLO_OBJS:.o=.d) $(MULTI_OBJS:.o=.d)
