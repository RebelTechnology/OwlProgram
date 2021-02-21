#ifndef __StateVariableFilter_h__
#define __StateVariableFilter_h__

#include "FloatArray.h"
#include "SignalProcessor.h"

/**
 * State Variable Filter based on Andy Simper's code and analysis:
 * @ref http://www.cytomic.com/files/dsp/SvfLinearTrapOptimised2.pdf
 */
class AbstractStateVariableFilter {
public:
  AbstractStateVariableFilter(float sr): pioversr(M_PI/sr) {}

  void setLowPass(float fc, float q){
    const float w = tanf(pioversr*fc);
    const float g = w;
    const float k = 1. / q;
    m_a1 = 1./(1. + g * (g + k));
    m_a2 = g * m_a1;
    m_a3 = g * m_a2;
    m_m0 = 0;
    m_m1 = 0;
    m_m2 = 1.;
  }

  void setHighPass(float fc, float q){
    const float w = tanf(pioversr*fc);
    const float g = w;
    const float k = 1. / q;
    m_a1 = 1./(1. + g * (g + k));
    m_a2 = g * m_a1;
    m_a3 = g * m_a2;
    m_m0 = 1.;
    m_m1 = -k;
    m_m2 = -1.;
  }

  void setBandPass(float fc, float q){
    const float w = tanf(pioversr*fc);
    const float g = w;
    const float k = 1. / q;
    m_a1 = 1./(1. + g * (g + k));
    m_a2 = g * m_a1;
    m_a3 = g * m_a2;
    m_m0 = 0.;
    m_m1 = 1.;
    m_m2 = 0.;
  }

  void setNotch(float fc, float q){
    const float w = tanf(pioversr*fc);
    const float g = w;
    const float k = 1. / q;
    m_a1 = 1./(1. + g * (g + k));
    m_a2 = g * m_a1;
    m_a3 = g * m_a2;
    m_m0 = 1.;
    m_m1 = -k;
    m_m2 = 0.;
  }

  void setPeak(float fc, float q){
    const float w = tanf(pioversr*fc);
    const float g = w;
    const float k = 1. / q;
    m_a1 = 1./(1. + g * (g + k));
    m_a2 = g * m_a1;
    m_a3 = g * m_a2;
    m_m0 = 1.;
    m_m1 = -k;
    m_m2 = -2.;
  }
  
  void setBell(float fc, float q, float gain){
    const float w = tanf(pioversr*fc);
    const float A = exp10f(gain/40.);
    const float g = w;
    const float k = 1 / q;
    m_a1 = 1./(1. + g * (g + k));
    m_a2 = g * m_a1;
    m_a3 = g * m_a2;
    m_m0 = 1.;
    m_m1 = k * (A * A - 1.);
    m_m2 = 0.;
  }
  
  void setLowShelf(float fc, float q, float gain){
    const float w = tanf(pioversr*fc);
    const float A = exp10f(gain/40.);
    const float g = w / sqrtf(A);
    const float k = 1. / q;
    m_a1 = 1./(1. + g * (g + k));
    m_a2 = g * m_a1;
    m_a3 = g * m_a2;
    m_m0 = 1.;
    m_m1 = k * (A - 1.);
    m_m2 = (A * A - 1.);
  }

  void setHighShelf(float fc, float q, float gain){
    const float w = tanf(pioversr*fc);
    const float A = exp10f(gain/40.);
    const float g = w / sqrtf(A);
    const float k = 1. / q;
    m_a1 = 1./(1. + g * (g + k));
    m_a2 = g * m_a1;
    m_a3 = g * m_a2;
    m_m0 = A*A;
    m_m1 = k*(1. - A)*A;
    m_m2 = (1. - A*A);
  }
protected:
  float pioversr;
  // coefficients
  float m_a1 = 0.;
  float m_a2 = 0.;
  float m_a3 = 0.;
  float m_m0 = 0.;
  float m_m1 = 0.;
  float m_m2 = 0.;
};

class StateVariableFilter : public AbstractStateVariableFilter, SignalProcessor {
public:
  StateVariableFilter(float sr): AbstractStateVariableFilter(sr) {}

  float process(float v0){
    mV3 = v0 - mIc2eq;
    mV1 = m_a1 * mIc1eq + m_a2*mV3;
    mV2 = mIc2eq + m_a2 * mIc1eq + m_a3 * mV3;
    mIc1eq = 2. * mV1 - mIc1eq;
    mIc2eq = 2. * mV2 - mIc2eq;
    return m_m0 * v0 + m_m1 * mV1 + m_m2 * mV2;
  }

  void process(FloatArray input, FloatArray output){
    size_t nFrames = input.getSize();
    for(size_t s = 0; s < nFrames; s++){
      const float v0 = input[s];
      mV3 = v0 - mIc2eq;
      mV1 = m_a1 * mIc1eq + m_a2*mV3;
      mV2 = mIc2eq + m_a2 * mIc1eq + m_a3 * mV3;
      mIc1eq = 2. * mV1 - mIc1eq;
      mIc2eq = 2. * mV2 - mIc2eq;
      output[s] = m_m0 * v0 + m_m1 * mV1 + m_m2 * mV2;
    }
  }

  void reset() {
    mV1 = 0.;
    mV2 = 0.;
    mV3 = 0.;
    mIc1eq = 0.;
    mIc2eq = 0.;
  }

  static StateVariableFilter* create(float sr){
    return new StateVariableFilter(sr);
  }

  static void destroy(StateVariableFilter* svf){
    delete svf;
  }
  
private:
  // state
  float mV1 = 0.;
  float mV2 = 0.;
  float mV3 = 0.;
  float mIc1eq = 0.;
  float mIc2eq = 0.;
};

class MultiStateVariableFilter : public AbstractStateVariableFilter, MultiSignalProcessor {
private:
  size_t mChannels;
  // state
  float* mState;
  static constexpr size_t STATE_VARIABLES_PER_CHANNEL = 5;
public:
  MultiStateVariableFilter(float sr) :
    AbstractStateVariableFilter(sr), 
    mChannels(0), mState(NULL) {}

  MultiStateVariableFilter(float sr, size_t channels,
			   float* state) :
    AbstractStateVariableFilter(sr),
    mChannels(channels), mState(state) {}

  void process(AudioBuffer &input, AudioBuffer &output){
    size_t len = min((int)mChannels, min(input.getChannels(), output.getChannels()));
    float* state = mState;
    for(size_t ch=0; ch<len; ++ch){
      FloatArray in = input.getSamples(ch);
      FloatArray out = output.getSamples(ch);
      size_t nFrames = in.getSize();
      float mV1 = *state++;
      float mV2 = *state++;
      float mV3 = *state++;
      float mIc1eq = *state++;
      float mIc2eq = *state++;
      for(size_t s = 0; s < nFrames; s++){
	float v0 = in[s];
	mV3 = v0 - mIc2eq;
	mV1 = m_a1 * mIc1eq + m_a2*mV3;
	mV2 = mIc2eq + m_a2 * mIc1eq + m_a3 * mV3;
	mIc1eq = 2. * mV1 - mIc1eq;
	mIc2eq = 2. * mV2 - mIc2eq;
	out[s] = m_m0 * v0 + m_m1 * mV1 + m_m2 * mV2;
      }
    }
  }

  void reset() {
    memset(mState, 0, mChannels*STATE_VARIABLES_PER_CHANNEL*sizeof(float));
  }

  static MultiStateVariableFilter* create(float sr, size_t channels){
    return new MultiStateVariableFilter(sr, channels, new float[STATE_VARIABLES_PER_CHANNEL]);
  }

  static void destroy(MultiStateVariableFilter* svf){
    delete[] svf->mState;
    delete svf;
  }
};

#endif // __StateVariableFilter_h__
