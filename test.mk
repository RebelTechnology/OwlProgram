BUILDROOT ?= .

C_SRC   = basicmaths.c
C_SRC   += kiss_fft.c
C_SRC   += fastpow.c fastlog.c
CPP_SRC += FloatArray.cpp
CPP_SRC += ShortArray.cpp
CPP_SRC += Envelope.cpp VoltsPerOctave.cpp Window.cpp
CPP_SRC += WavetableOscillator.cpp PolyBlepOscillator.cpp
CPP_SRC += SmoothValue.cpp # PatchParameter.cpp
CPP_SRC += system_tables.cpp

BUILD       ?= $(BUILDROOT)/Build

SOURCE       = $(BUILDROOT)/Source
LIBSOURCE    = $(BUILDROOT)/LibSource
GENSOURCE    = $(BUILD)/Source
TESTPATCHES  = $(BUILDROOT)/TestPatches
CPPFLAGS  = -g -Wall -m32
CPPFLAGS += -I$(SOURCE)
CPPFLAGS += -I$(LIBSOURCE)
CPPFLAGS += -I$(GENSOURCE)
CPPFLAGS += -I$(TESTPATCHES)
CPPFLAGS += -ILibraries -ILibraries/KissFFT
CPPFLAGS += -ILibraries/CMSIS/Include
CPPFLAGS +=  -DARM_MATH_CM0
CPPFLAGS +=  -fno-builtin -ffreestanding

# Tools
# TOOLROOT=i686-pc-cygwin-
# TOOLROOT=x86_64-pc-cygwin-
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
OBJS  = $(C_SRC:%.c=$(BUILD)/%.o) $(CPP_SRC:%.cpp=$(BUILD)/%.o)

# Set up search path
vpath %.cpp $(SOURCE)
vpath %.c $(SOURCE)
vpath %.c Libraries/KissFFT/
vpath %.s $(SOURCE)
vpath %.cpp $(LIBSOURCE)
vpath %.c $(LIBSOURCE)
vpath %.s $(LIBSOURCE)
vpath %.cpp $(GENSOURCE)
vpath %.c $(GENSOURCE)
vpath %.s $(GENSOURCE)

DSPLIB=Libraries/CMSIS/DSP_Lib/Source
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
OBJS += $(DSPLIB)/TransformFunctions/arm_bitreversal.o
OBJS += $(DSPLIB)/TransformFunctions/arm_rfft_fast_f32.o
OBJS += $(DSPLIB)/TransformFunctions/arm_rfft_init_q15.o
OBJS += $(DSPLIB)/TransformFunctions/arm_rfft_q15.o
OBJS += $(DSPLIB)/TransformFunctions/arm_cfft_q15.o
OBJS += $(DSPLIB)/TransformFunctions/arm_cfft_radix4_q15.o

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
OBJS += $(DSPLIB)/SupportFunctions/arm_q15_to_q31.o
OBJS += $(DSPLIB)/SupportFunctions/arm_q31_to_q15.o

OBJS += $(DSPLIB)/BasicMathFunctions/arm_abs_f32.o
OBJS += $(DSPLIB)/BasicMathFunctions/arm_add_f32.o
OBJS += $(DSPLIB)/BasicMathFunctions/arm_dot_prod_f32.o
OBJS += $(DSPLIB)/BasicMathFunctions/arm_mult_f32.o
OBJS += $(DSPLIB)/BasicMathFunctions/arm_negate_f32.o
OBJS += $(DSPLIB)/BasicMathFunctions/arm_scale_f32.o
OBJS += $(DSPLIB)/BasicMathFunctions/arm_sub_f32.o
OBJS += $(DSPLIB)/BasicMathFunctions/arm_shift_q15.o

OBJS += $(DSPLIB)/StatisticsFunctions/arm_max_f32.o
OBJS += $(DSPLIB)/StatisticsFunctions/arm_mean_f32.o
OBJS += $(DSPLIB)/StatisticsFunctions/arm_min_f32.o
OBJS += $(DSPLIB)/StatisticsFunctions/arm_power_f32.o
OBJS += $(DSPLIB)/StatisticsFunctions/arm_rms_f32.o
OBJS += $(DSPLIB)/StatisticsFunctions/arm_std_f32.o
OBJS += $(DSPLIB)/StatisticsFunctions/arm_var_f32.o

OBJS += $(DSPLIB)/FastMathFunctions/arm_sin_q15.o
OBJS += $(DSPLIB)/FastMathFunctions/arm_cos_q15.o
OBJS += $(DSPLIB)/FastMathFunctions/arm_sqrt_q15.o
OBJS += $(DSPLIB)/ComplexMathFunctions/arm_cmplx_conj_q15.o
OBJS += $(DSPLIB)/ComplexMathFunctions/arm_cmplx_dot_prod_q15.o
OBJS += $(DSPLIB)/ComplexMathFunctions/arm_cmplx_mag_q15.o
OBJS += $(DSPLIB)/ComplexMathFunctions/arm_cmplx_mag_squared_q15.o
OBJS += $(DSPLIB)/ComplexMathFunctions/arm_cmplx_mult_cmplx_q15.o
OBJS += $(DSPLIB)/ComplexMathFunctions/arm_cmplx_mult_real_q15.o
OBJS += $(DSPLIB)/FilteringFunctions/arm_correlate_q15.o
OBJS += $(DSPLIB)/FilteringFunctions/arm_conv_q15.o
OBJS += $(DSPLIB)/FilteringFunctions/arm_conv_partial_q15.o
OBJS += $(DSPLIB)/FilteringFunctions/arm_lms_init_q15.o
OBJS += $(DSPLIB)/FilteringFunctions/arm_lms_q15.o
OBJS += $(DSPLIB)/FilteringFunctions/arm_lms_norm_q15.o
OBJS += $(DSPLIB)/FilteringFunctions/arm_lms_norm_init_q15.o
OBJS += $(DSPLIB)/FilteringFunctions/arm_fir_q15.o
OBJS += $(DSPLIB)/FilteringFunctions/arm_fir_init_q15.o
OBJS += $(DSPLIB)/FilteringFunctions/arm_fir_decimate_q15.o
OBJS += $(DSPLIB)/FilteringFunctions/arm_fir_decimate_init_q15.o
OBJS += $(DSPLIB)/FilteringFunctions/arm_fir_interpolate_q15.o
OBJS += $(DSPLIB)/FilteringFunctions/arm_fir_interpolate_init_q15.o
OBJS += $(DSPLIB)/SupportFunctions/arm_copy_q15.o
OBJS += $(DSPLIB)/SupportFunctions/arm_fill_q15.o
OBJS += $(DSPLIB)/BasicMathFunctions/arm_abs_q15.o
OBJS += $(DSPLIB)/BasicMathFunctions/arm_add_q15.o
OBJS += $(DSPLIB)/BasicMathFunctions/arm_dot_prod_q15.o
OBJS += $(DSPLIB)/BasicMathFunctions/arm_mult_q15.o
OBJS += $(DSPLIB)/BasicMathFunctions/arm_negate_q15.o
OBJS += $(DSPLIB)/BasicMathFunctions/arm_scale_q15.o
OBJS += $(DSPLIB)/BasicMathFunctions/arm_sub_q15.o
OBJS += $(DSPLIB)/StatisticsFunctions/arm_max_q15.o
OBJS += $(DSPLIB)/StatisticsFunctions/arm_mean_q15.o
OBJS += $(DSPLIB)/StatisticsFunctions/arm_min_q15.o
OBJS += $(DSPLIB)/StatisticsFunctions/arm_power_q15.o
OBJS += $(DSPLIB)/StatisticsFunctions/arm_rms_q15.o
OBJS += $(DSPLIB)/StatisticsFunctions/arm_std_q15.o
OBJS += $(DSPLIB)/StatisticsFunctions/arm_var_q15.o

OBJS += $(DSPLIB)/BasicMathFunctions/arm_add_q31.o
# include $(BUILDROOT)/libs.mk

test: $(TESTPATCHES)/PatchTest.cpp $(DEPS) $(OBJS)
	@$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(TESTPATCHES)/PatchTest.cpp -I$(BUILD) $(OBJS) -o $(BUILD)/$@
	@$(BUILD)/$@

# compile and generate dependency info
$(BUILD)/%.o: %.c
	@$(CC) -c $(CPPFLAGS) $(CFLAGS) $< -o $@
	@$(CC) -MM -MT"$@" $(CPPFLAGS) $(CFLAGS) $< > $(@:.o=.d)

$(BUILD)/%.o: %.cpp
	@$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@
	@$(CXX) -MM -MT"$@" $(CPPFLAGS) $(CXXFLAGS) $< > $(@:.o=.d)

-include $(OBJS:.o=.d) $(SOLO_OBJS:.o=.d) $(MULTI_OBJS:.o=.d)
