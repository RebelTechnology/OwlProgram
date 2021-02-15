S_SRC += $(DSPLIB)/TransformFunctions/arm_bitreversal2.S

C_SRC += $(DSPLIB)/FastMathFunctions/arm_sin_f32.c
C_SRC += $(DSPLIB)/FastMathFunctions/arm_cos_f32.c
C_SRC += $(DSPLIB)/CommonTables/arm_common_tables.c
C_SRC += $(DSPLIB)/CommonTables/arm_const_structs.c

C_SRC += $(DSPLIB)/ComplexMathFunctions/arm_cmplx_conj_f32.c
C_SRC += $(DSPLIB)/ComplexMathFunctions/arm_cmplx_dot_prod_f32.c
C_SRC += $(DSPLIB)/ComplexMathFunctions/arm_cmplx_mag_f32.c
C_SRC += $(DSPLIB)/ComplexMathFunctions/arm_cmplx_mag_squared_f32.c
C_SRC += $(DSPLIB)/ComplexMathFunctions/arm_cmplx_mult_cmplx_f32.c
C_SRC += $(DSPLIB)/ComplexMathFunctions/arm_cmplx_mult_real_f32.c

C_SRC += $(DSPLIB)/TransformFunctions/arm_cfft_f32.c
C_SRC += $(DSPLIB)/TransformFunctions/arm_cfft_radix8_f32.c
C_SRC += $(DSPLIB)/TransformFunctions/arm_rfft_fast_f32.c
C_SRC += $(DSPLIB)/TransformFunctions/arm_rfft_init_q15.c
C_SRC += $(DSPLIB)/TransformFunctions/arm_rfft_q15.c
C_SRC += $(DSPLIB)/TransformFunctions/arm_cfft_q15.c
C_SRC += $(DSPLIB)/TransformFunctions/arm_cfft_radix4_q15.c

C_SRC += $(DSPLIB)/FilteringFunctions/arm_biquad_cascade_df1_init_f32.c
C_SRC += $(DSPLIB)/FilteringFunctions/arm_biquad_cascade_df1_f32.c
C_SRC += $(DSPLIB)/FilteringFunctions/arm_biquad_cascade_df2T_init_f32.c
C_SRC += $(DSPLIB)/FilteringFunctions/arm_biquad_cascade_df2T_f32.c
C_SRC += $(DSPLIB)/FilteringFunctions/arm_correlate_f32.c
C_SRC += $(DSPLIB)/FilteringFunctions/arm_conv_f32.c
C_SRC += $(DSPLIB)/FilteringFunctions/arm_conv_partial_f32.c
C_SRC += $(DSPLIB)/FilteringFunctions/arm_lms_init_f32.c
C_SRC += $(DSPLIB)/FilteringFunctions/arm_lms_f32.c
C_SRC += $(DSPLIB)/FilteringFunctions/arm_lms_norm_f32.c
C_SRC += $(DSPLIB)/FilteringFunctions/arm_lms_norm_init_f32.c
C_SRC += $(DSPLIB)/FilteringFunctions/arm_fir_f32.c
C_SRC += $(DSPLIB)/FilteringFunctions/arm_fir_init_f32.c
C_SRC += $(DSPLIB)/FilteringFunctions/arm_fir_decimate_f32.c
C_SRC += $(DSPLIB)/FilteringFunctions/arm_fir_decimate_init_f32.c
C_SRC += $(DSPLIB)/FilteringFunctions/arm_fir_interpolate_f32.c
C_SRC += $(DSPLIB)/FilteringFunctions/arm_fir_interpolate_init_f32.c

C_SRC += $(DSPLIB)/SupportFunctions/arm_float_to_q31.c
C_SRC += $(DSPLIB)/SupportFunctions/arm_q31_to_float.c
C_SRC += $(DSPLIB)/SupportFunctions/arm_float_to_q15.c
C_SRC += $(DSPLIB)/SupportFunctions/arm_q15_to_float.c
C_SRC += $(DSPLIB)/SupportFunctions/arm_copy_f32.c
C_SRC += $(DSPLIB)/SupportFunctions/arm_fill_f32.c
C_SRC += $(DSPLIB)/SupportFunctions/arm_q15_to_q31.c
C_SRC += $(DSPLIB)/SupportFunctions/arm_q31_to_q15.c

C_SRC += $(DSPLIB)/BasicMathFunctions/arm_abs_f32.c
C_SRC += $(DSPLIB)/BasicMathFunctions/arm_add_f32.c
C_SRC += $(DSPLIB)/BasicMathFunctions/arm_dot_prod_f32.c
C_SRC += $(DSPLIB)/BasicMathFunctions/arm_mult_f32.c
C_SRC += $(DSPLIB)/BasicMathFunctions/arm_negate_f32.c
C_SRC += $(DSPLIB)/BasicMathFunctions/arm_scale_f32.c
C_SRC += $(DSPLIB)/BasicMathFunctions/arm_sub_f32.c

C_SRC += $(DSPLIB)/StatisticsFunctions/arm_max_f32.c
C_SRC += $(DSPLIB)/StatisticsFunctions/arm_mean_f32.c
C_SRC += $(DSPLIB)/StatisticsFunctions/arm_min_f32.c
C_SRC += $(DSPLIB)/StatisticsFunctions/arm_power_f32.c
C_SRC += $(DSPLIB)/StatisticsFunctions/arm_rms_f32.c
C_SRC += $(DSPLIB)/StatisticsFunctions/arm_std_f32.c
C_SRC += $(DSPLIB)/StatisticsFunctions/arm_var_f32.c


C_SRC += $(DSPLIB)/FastMathFunctions/arm_sin_q15.c
C_SRC += $(DSPLIB)/FastMathFunctions/arm_cos_q15.c
C_SRC += $(DSPLIB)/FastMathFunctions/arm_sqrt_q15.c
C_SRC += $(DSPLIB)/ComplexMathFunctions/arm_cmplx_conj_q15.c
C_SRC += $(DSPLIB)/ComplexMathFunctions/arm_cmplx_dot_prod_q15.c
C_SRC += $(DSPLIB)/ComplexMathFunctions/arm_cmplx_mag_q15.c
C_SRC += $(DSPLIB)/ComplexMathFunctions/arm_cmplx_mag_squared_q15.c
C_SRC += $(DSPLIB)/ComplexMathFunctions/arm_cmplx_mult_cmplx_q15.c
C_SRC += $(DSPLIB)/ComplexMathFunctions/arm_cmplx_mult_real_q15.c
C_SRC += $(DSPLIB)/FilteringFunctions/arm_correlate_q15.c
C_SRC += $(DSPLIB)/FilteringFunctions/arm_conv_q15.c
C_SRC += $(DSPLIB)/FilteringFunctions/arm_conv_partial_q15.c
C_SRC += $(DSPLIB)/FilteringFunctions/arm_lms_init_q15.c
C_SRC += $(DSPLIB)/FilteringFunctions/arm_lms_q15.c
C_SRC += $(DSPLIB)/FilteringFunctions/arm_lms_norm_q15.c
C_SRC += $(DSPLIB)/FilteringFunctions/arm_lms_norm_init_q15.c
C_SRC += $(DSPLIB)/FilteringFunctions/arm_fir_q15.c
C_SRC += $(DSPLIB)/FilteringFunctions/arm_fir_init_q15.c
C_SRC += $(DSPLIB)/FilteringFunctions/arm_fir_decimate_q15.c
C_SRC += $(DSPLIB)/FilteringFunctions/arm_fir_decimate_init_q15.c
C_SRC += $(DSPLIB)/FilteringFunctions/arm_fir_interpolate_q15.c
C_SRC += $(DSPLIB)/FilteringFunctions/arm_fir_interpolate_init_q15.c
C_SRC += $(DSPLIB)/SupportFunctions/arm_copy_q15.c

C_SRC += $(DSPLIB)/SupportFunctions/arm_fill_q15.c
C_SRC += $(DSPLIB)/BasicMathFunctions/arm_abs_q15.c
C_SRC += $(DSPLIB)/BasicMathFunctions/arm_add_q15.c
C_SRC += $(DSPLIB)/BasicMathFunctions/arm_dot_prod_q15.c
C_SRC += $(DSPLIB)/BasicMathFunctions/arm_mult_q15.c
C_SRC += $(DSPLIB)/BasicMathFunctions/arm_negate_q15.c
C_SRC += $(DSPLIB)/BasicMathFunctions/arm_scale_q15.c
C_SRC += $(DSPLIB)/BasicMathFunctions/arm_sub_q15.c
C_SRC += $(DSPLIB)/BasicMathFunctions/arm_shift_q15.c

C_SRC += $(DSPLIB)/StatisticsFunctions/arm_max_q15.c
C_SRC += $(DSPLIB)/StatisticsFunctions/arm_mean_q15.c
C_SRC += $(DSPLIB)/StatisticsFunctions/arm_min_q15.c
C_SRC += $(DSPLIB)/StatisticsFunctions/arm_power_q15.c
C_SRC += $(DSPLIB)/StatisticsFunctions/arm_rms_q15.c
C_SRC += $(DSPLIB)/StatisticsFunctions/arm_std_q15.c
C_SRC += $(DSPLIB)/StatisticsFunctions/arm_var_q15.c

C_SRC += $(DSPLIB)/BasicMathFunctions/arm_add_q31.c

