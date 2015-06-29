#ifndef __BiquadFilter_h__
#define __BiquadFilter_h__

#include "FloatArray.h"

/** 
 * Cascaded Biquad Filter.
 * Implemented using CMSIS DSP Library, Direct Form 2 Transposed.
 * Each cascaded stage implements a second order filter.
 */
class BiquadFilter {
private:
#ifdef ARM_CORTEX
  // arm_biquad_casd_df1_inst_f32 df1;
  arm_biquad_cascade_df2T_instance_f32 df2;
#endif /* ARM_CORTEX */

  float* coefficients; // stages*5
  float* state; // stages*4 for df1, stages*2 for df2
  int stages;
  /*
   * The coefficients are stored in the array <code>coefficients</code> in the following order:
   * <pre>
   *     {b10, b11, b12, a11, a12, b20, b21, b22, a21, a22, ...}
   * </pre>
   * where <code>b1x</code> and <code>a1x</code> are the coefficients for the first stage,
   * <code>b2x</code> and <code>a2x</code> are the coefficients for the second stage,
   * and so on.  The <code>coeffs</code> array must contain a total of <code>5*stages</code> values.   
   */
protected:
  void init(){
#ifdef ARM_CORTEX
    // arm_biquad_cascade_df1_init_f32(&df1, stages, coefficients, state);
    arm_biquad_cascade_df2T_init_f32(&df2, stages, coefficients, state);
#endif /* ARM_CORTEX */
  }
public:
  BiquadFilter(float* coefs, float* ste, int sgs) :
    coefficients(coefs), state(ste), stages(sgs) {
    init();
  }

  /* perform in-place processing */
  void process(float* buf, int size){
#ifdef ARM_CORTEX
    // arm_biquad_cascade_df1_f32(&df1, buf, buf, size);
    arm_biquad_cascade_df2T_f32(&df2, buf, buf, size);
#endif /* ARM_CORTEX */
  }

  /* process into output, leaving input intact */
  void process(float* input, float* output, int size){
#ifdef ARM_CORTEX
    // arm_biquad_cascade_df1_f32(&df1, input, output, size);
    arm_biquad_cascade_df2T_f32(&df2, input, output, size);
#endif /* ARM_CORTEX */
  }

  void process(FloatArray in){
    process(in, in.getSize());
  }

  void process(FloatArray in, FloatArray out){
    ASSERT(out.getSize() >= in.getSize(), "output array must be at least as long as input");
    process(in, out, in.getSize());
  }

  /* process a single sample and return the result */
  float process(float input){
    float output;
    process(&input, &output, 1);
    return output;
  }

  void copyCoefficients(){
    coefficients[3] = - coefficients[3];
    coefficients[4] = - coefficients[4];
    for(int i=1; i<stages; ++i){
      coefficients[0+i*5] = coefficients[0];
      coefficients[1+i*5] = coefficients[1];
      coefficients[2+i*5] = coefficients[2];
      coefficients[3+i*5] = coefficients[3];
      coefficients[4+i*5] = coefficients[4];
    }
  }

  void setLowPass(float fc, float q){
    float omega = M_PI*fc/2;
    float K = tanf(omega);
    float norm = 1 / (1 + K / q + K * K);
    coefficients[0] = K * K * norm;
    coefficients[1] = 2 * coefficients[0];
    coefficients[2] = coefficients[0];
    coefficients[3] = 2 * (K * K - 1) * norm;
    coefficients[4] = (1 - K / q + K * K) * norm;
    copyCoefficients();
  }

  void setHighPass(float fc, float q){
    float omega = M_PI*fc/2;
    float K = tanf(omega);
    float norm = 1 / (1 + K / q + K * K);
    coefficients[0] = 1 * norm;
    coefficients[1] = -2 * coefficients[0];
    coefficients[2] = coefficients[0];
    coefficients[3] = 2 * (K * K - 1) * norm;
    coefficients[4] = (1 - K / q + K * K) * norm;
    copyCoefficients();
  }

  void setBandPass(float fc, float q){
    float omega = M_PI*fc/2;
    float K = tanf(omega);
    float norm = 1 / (1 + K / q + K * K);
    coefficients[0] = K / q * norm;
    coefficients[1] = 0;
    coefficients[2] = -coefficients[0];
    coefficients[3] = 2 * (K * K - 1) * norm;
    coefficients[4] = (1 - K / q + K * K) * norm;
    copyCoefficients();
  }

  void setNotch(float fc, float q){
    float omega = M_PI*fc/2;
    float K = tanf(omega);
    float norm = 1 / (1 + K / q + K * K);
    coefficients[0] = (1 + K * K) * norm;
    coefficients[1] = 2 * (K * K - 1) * norm;
    coefficients[2] = coefficients[0];
    coefficients[3] = coefficients[1];
    coefficients[4] = (1 - K / q + K * K) * norm;
    copyCoefficients();
  }

  void setPeak(float fc, float q, float gain){
    float omega = M_PI*fc/2;
    float K = tanf(omega);
    float V = abs(gain-0.5)*60 + 1; // Gain
    float norm;
    if (gain >= 0.5) {
      norm = 1 / (1 + 1/q * K + K * K);
      coefficients[0] = (1 + V/q * K + K * K) * norm;
      coefficients[1] = 2 * (K * K - 1) * norm;
      coefficients[2] = (1 - V/q * K + K * K) * norm;
      coefficients[3] = coefficients[1];
      coefficients[4] = (1 - 1/q * K + K * K) * norm;
    }
    else {
      norm = 1 / (1 + V/q * K + K * K);
      coefficients[0] = (1 + 1/q * K + K * K) * norm;
      coefficients[1] = 2 * (K * K - 1) * norm;
      coefficients[2] = (1 - 1/q * K + K * K) * norm;
      coefficients[3] = coefficients[1];
      coefficients[4] = (1 - V/q * K + K * K) * norm;
    }
    copyCoefficients();
  }

  void setLowShelf(float fc, float gain){
    float omega = M_PI*fc/2;
    float K = tanf(omega);
    float V = abs(gain-0.5)*60 + 1; // Gain
    float norm;
    if(gain >= 0.5) {
      norm = 1 / (1 + M_SQRT2 * K + K * K);
      coefficients[0] = (1 + sqrtf(2*V) * K + V * K * K) * norm;
      coefficients[1] = 2 * (V * K * K - 1) * norm;
      coefficients[2] = (1 - sqrtf(2*V) * K + V * K * K) * norm;
      coefficients[3] = 2 * (K * K - 1) * norm;
      coefficients[4] = (1 - M_SQRT2 * K + K * K) * norm;
    } else {
      norm = 1 / (1 + sqrtf(2*V) * K + V * K * K);
      coefficients[0] = (1 + M_SQRT2 * K + K * K) * norm;
      coefficients[1] = 2 * (K * K - 1) * norm;
      coefficients[2] = (1 - M_SQRT2 * K + K * K) * norm;
      coefficients[3] = 2 * (V * K * K - 1) * norm;
      coefficients[4] = (1 - sqrtf(2*V) * K + V * K * K) * norm;
    }
    copyCoefficients();
  }

  void setHighShelf(float fc, float gain){
    float omega = M_PI*fc/2;
    float K = tanf(omega);
    float V = abs(gain-0.5)*60 + 1; // Gain
    float norm;
    if(gain >= 0.5) {
      norm = 1 / (1 + M_SQRT2 * K + K * K);
      coefficients[0] = (V + sqrtf(2*V) * K + K * K) * norm;
      coefficients[1] = 2 * (K * K - V) * norm;
      coefficients[2] = (V - sqrtf(2*V) * K + K * K) * norm;
      coefficients[3] = 2 * (K * K - 1) * norm;
      coefficients[4] = (1 - M_SQRT2 * K + K * K) * norm;
    } else {
      norm = 1 / (V + sqrtf(2*V) * K + K * K);
      coefficients[0] = (1 + M_SQRT2 * K + K * K) * norm;
      coefficients[1] = 2 * (K * K - 1) * norm;
      coefficients[2] = (1 - M_SQRT2 * K + K * K) * norm;
      coefficients[3] = 2 * (K * K - V) * norm;
      coefficients[4] = (V - sqrtf(2*V) * K + K * K) * norm;
    }
    copyCoefficients();
  }

  static BiquadFilter* create(int stages){
    return new BiquadFilter(new float[stages*5], new float[stages*2], stages);
    // for df1: state requires stages*4
    // return new BiquadFilter(new float[stages*5], new float[stages*4], stages);
  }

  static void destroy(BiquadFilter* filter){
    delete filter->coefficients;
    delete filter->state;
    delete filter;
  }
};

#endif // __BiquadFilter_h__
