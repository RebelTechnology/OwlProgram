# Code Paths
DSPLIB=Libraries/CMSIS/DSP_Lib/Source

# Tool path
TOOLROOT ?= Tools/gcc-arm-none-eabi-9-2020-q2-update/bin/

CMSIS ?= Libraries/CMSIS/Include/

# Tools
CC=$(TOOLROOT)arm-none-eabi-gcc
LD=$(TOOLROOT)arm-none-eabi-gcc
AR=$(TOOLROOT)arm-none-eabi-gcc-ar
AS=$(TOOLROOT)arm-none-eabi-as
NM=$(TOOLROOT)arm-none-eabi-nm
CXX=$(TOOLROOT)arm-none-eabi-g++
GDB=$(TOOLROOT)arm-none-eabi-gdb
SIZE=$(TOOLROOT)arm-none-eabi-size
RANLIB=$(TOOLROOT)arm-none-eabi-ranlib
OBJCOPY=$(TOOLROOT)arm-none-eabi-objcopy
OBJDUMP=$(TOOLROOT)arm-none-eabi-objdump

# Compilation Flags
ARCH_FLAGS = -fsingle-precision-constant -mthumb
ARCH_FLAGS += -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16
# ARCH_FLAGS += -mcpu=cortex-m7 -mfloat-abi=hard -mfpu=fpv5-sp-d16
# ARCH_FLAGS += -mcpu=cortex-m0 -mfloat-abi=soft -msoft-float
DEF_FLAGS = -DSTM32F4XX -DARM_MATH_CM4 -D__FPU_PRESENT -D__FPU_USED=1
# DEF_FLAGS = -DSTM32F745xx -DARM_MATH_CM7 -D__FPU_PRESENT -D__FPU_USED=1

INC_FLAGS = -I$(BUILDROOT)/Libraries -I$(DEVICE) -I$(CMSIS) -I$(PERIPH_FILE)/inc -I$(SOURCE)
INC_FLAGS += -I$(DEVICE)/Include -I$(CMSIS)
INC_FLAGS += -I$(USB_DEVICE_FILE)/Core/inc -I$(USB_DEVICE_FILE)/Class/cdc/inc
INC_FLAGS += -I$(USB_OTG_FILE)/inc
CPPFLAGS += $(ARCH_FLAGS) $(INC_FLAGS) $(DEF_FLAGS)
CFLAGS += -std=gnu11
CXXFLAGS += -std=gnu++14
LDFLAGS += -T$(LDSCRIPT) $(ARCH_FLAGS)

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

$(BUILD)/%.map: $(BUILD)/%.elf
	@$(OBJDUMP) -S $< > $@

size:
	$(NM) --print-size --size-sort $(BUILD)/$(TARGET).elf | tail -n 20
	$(SIZE) $(BUILD)/$(TARGET).elf
	ls -s $(BUILD)/$(TARGET).bin

# pull in dependencies
-include $(OBJS:.o=.d) $(SOLO_OBJS:.o=.d) $(MULTI_OBJS:.o=.d)
