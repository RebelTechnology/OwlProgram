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
