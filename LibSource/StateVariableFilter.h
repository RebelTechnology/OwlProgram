#ifndef __StateVariableFilter_h__
#define __StateVariableFilter_h__

#include "FloatArray.h"
#include "SignalProcessor.h"

#define SVF_COMPUTE_BOUNDED

/**
 * State Variable Filter based on Andy Simper's code and analysis:
 * @ref http://www.cytomic.com/files/dsp/SvfLinearTrapOptimised2.pdf
 * This is a so-called Zero Delay Feedback, trapezoidal integration filter
 * and is suited to audio rate modulation of filter coefficients.
 *
 * Andy provides two versions of the algorithm: bounded form, and with unbounded 'g' term.
 * Define SVF_COMPUTE_BOUNDED to use the bounded form (default).
 */
class AbstractStateVariableFilter {
protected:
  float kvalue(float fc, float q){
#ifdef SVF_COMPUTE_BOUNDED
    const float g = tanf(pioversr*fc);
#else
    g = tanf(pioversr*fc);
#endif
    const float k = 1.0f / q;
    a1 = 1.0f/(1.0f + g * (g + k));
    a2 = g * a1;
    a3 = g * a2;
    return k;
  }
public:
  AbstractStateVariableFilter(float sr): pioversr(M_PI/sr) {}

  void setCutoff(float fc, float q){
    const float k = kvalue(fc, q);
    m0 = k;
  }

  void setLowPass(float fc, float q){
    kvalue(fc, q);
    m0 = 0;
    m1 = 0;
    m2 = 1.0f;
  }

  void setHighPass(float fc, float q){
    m0 = 1.0f;
    m1 = -kvalue(fc, q);
    m2 = -1.0f;
  }

  void setBandPass(float fc, float q){
    kvalue(fc, q);
    m0 = 0.0f;
    m1 = 1.0f;
    m2 = 0.0f;
  }

  void setNotch(float fc, float q){
    const float k = kvalue(fc, q);
    m0 = 1.0f;
    m1 = -k;
    m2 = 0.0f;
  }

  void setPeak(float fc, float q){
    const float k = kvalue(fc, q);
    m0 = 1.0f;
    m1 = -k;
    m2 = -2.0f;
  }
  
  void setBell(float fc, float q, float gain){
    const float k = kvalue(fc, q);
    const float A = exp10f(gain/40.);
    m0 = 1.0f;
    m1 = k * (A * A - 1.);
    m2 = 0.0f;
  }
  
  void setLowShelf(float fc, float q, float gain){
    const float k = kvalue(fc, q);
    const float A = exp10f(gain/40.);
    m0 = 1.0f;
    m1 = k * (A - 1.0f);
    m2 = (A * A - 1.0f);
  }

  void setHighShelf(float fc, float q, float gain){
    const float k = kvalue(fc, q);
    const float A = exp10f(gain/40.);
    m0 = A * A;
    m1 = k * (1.0f - A) * A;
    m2 = (1.0f - A * A);
  }

  void setAllPass(float fc, float q){
    const float k = kvalue(fc, q);
    m0 = 1.0f;
    m1 = 2.0f * k;
    m2 = 0.0f;
  }
protected:
  const float pioversr;
  // coefficients
#ifndef SVF_COMPUTE_BOUNDED
  float g = 0.0f;
#endif
  float a1 = 0.0f;
  float a2 = 0.0f;
  float a3 = 0.0f;
  float m0 = 0.0f;
  float m1 = 0.0f;
  float m2 = 0.0f;
};

class StateVariableFilter : public AbstractStateVariableFilter, SignalProcessor {
public:
  StateVariableFilter(float sr): AbstractStateVariableFilter(sr) {}

  float process(float v0){
#ifdef SVF_COMPUTE_BOUNDED
    float v3 = v0 - ic2eq;
    float v1 = a1 * ic1eq + a2*v3;
    float v2 = ic2eq + a2 * ic1eq + a3 * v3;
#else
    float v1 = a1 * ic1eq + a2 * (v0 - ic2eq);
    float v2 = ic2eq + g * v1;
#endif
    ic1eq = 2. * v1 - ic1eq;
    ic2eq = 2. * v2 - ic2eq;
    return m0 * v0 + m1 * v1 + m2 * v2;
  }

  void process(FloatArray input, FloatArray output){
    size_t nFrames = input.getSize();
    float v0, v1, v2, v3;
    for(size_t s = 0; s < nFrames; s++){
      v0 = input[s];
#ifdef SVF_COMPUTE_BOUNDED
      v3 = v0 - ic2eq;
      v1 = a1 * ic1eq + a2*v3;
      v2 = ic2eq + a2 * ic1eq + a3 * v3;
#else
      v1 = a1 * ic1eq + a2 * (v0 - ic2eq);
      v2 = ic2eq + g * v1;
#endif
      ic1eq = 2. * v1 - ic1eq;
      ic2eq = 2. * v2 - ic2eq;
      output[s] = m0 * v0 + m1 * v1 + m2 * v2;
    }
  }

  void processLowPass(FloatArray in, FloatArray fc, float q, FloatArray out){
    for(size_t i = 0; i < in.getSize(); i++){
      setLowPass(fc[i], q);
      out[i] = process(in[i]);
    }
  }

  void processHighPass(FloatArray in, FloatArray fc, float q, FloatArray out){
    for(size_t i = 0; i < in.getSize(); i++){
      setHighPass(fc[i], q);
      out[i] = process(in[i]);
    }
  }

  void processBandPass(FloatArray in, FloatArray fc, float q, FloatArray out){
    for(size_t i = 0; i < in.getSize(); i++){
      setBandPass(fc[i], q);
      out[i] = process(in[i]);
    }
  }

  /**
   * Simultaneously process lowpass, bandpass and highpass filtered output
   */
  void processLowBandHighPass(FloatArray in, float fc, float q, FloatArray low, FloatArray band, FloatArray high){
    size_t nFrames = in.getSize();
    float v0, v1, v2, v3;
    setCutoff(fc, q);
    for(size_t s = 0; s < nFrames; s++){
      v0 = in[s];
#ifdef SVF_COMPUTE_BOUNDED
      v3 = v0 - ic2eq;
      v1 = a1 * ic1eq + a2*v3;
      v2 = ic2eq + a2 * ic1eq + a3 * v3;
#else
      v1 = a1 * ic1eq + a2 * (v0 - ic2eq);
      v2 = ic2eq + g * v1;
#endif
      ic1eq = 2. * v1 - ic1eq;
      ic2eq = 2. * v2 - ic2eq;
      low[s] = v2;
      band[s] = v1;
      high[s] = v0 - m0*v1 - v2;
      // notch = low + high = v0 - k*v1;
      // peak = low - high = v0 - k*v1 - 2*v2;
      // all = low + high - k*band = v0 - 2*k*v1;
    }
  }

  void processLowBandHighPass(FloatArray in, FloatArray fc, float q,
			      FloatArray low, FloatArray band, FloatArray high){
    size_t nFrames = in.getSize();
    float v0, v1, v2, v3;
    for(size_t s = 0; s < nFrames; s++){
      setCutoff(fc[s], q);
      v0 = in[s];
#ifdef SVF_COMPUTE_BOUNDED
      v3 = v0 - ic2eq;
      v1 = a1 * ic1eq + a2*v3;
      v2 = ic2eq + a2 * ic1eq + a3 * v3;
#else
      v1 = a1 * ic1eq + a2 * (v0 - ic2eq);
      v2 = ic2eq + g * v1;
#endif
      ic1eq = 2. * v1 - ic1eq;
      ic2eq = 2. * v2 - ic2eq;
      low[s] = v2;
      band[s] = v1;
      high[s] = v0 - m0*v1 - v2;
      // notch = low + high = v0 - k*v1;
      // peak = low - high = v0 - k*v1 - 2*v2;
      // all = low + high - k*band = v0 - 2*k*v1;
    }
  }

  void reset() {
    ic1eq = 0.0f;
    ic2eq = 0.0f;
  }

  static StateVariableFilter* create(float sr){
    return new StateVariableFilter(sr);
  }

  static void destroy(StateVariableFilter* svf){
    delete svf;
  }
  
private:
  // state
  float ic1eq = 0.0f;
  float ic2eq = 0.0f;
};


class MultiStateVariableFilter : public AbstractStateVariableFilter, MultiSignalProcessor {
protected:
  size_t mChannels;
  float* mState;
  static constexpr size_t STATE_VARIABLES_PER_CHANNEL = 2;
public:
  MultiStateVariableFilter(float sr) :
    AbstractStateVariableFilter(sr), 
    mChannels(0), mState(NULL) {}

  MultiStateVariableFilter(float sr, size_t channels, float* state) :
    AbstractStateVariableFilter(sr), mChannels(channels), mState(state) {}

  void process(AudioBuffer &input, AudioBuffer &output){
    size_t len = min((int)mChannels, min(input.getChannels(), output.getChannels()));
    float* state = mState;
    for(size_t ch=0; ch<len; ++ch){
      FloatArray in = input.getSamples(ch);
      FloatArray out = output.getSamples(ch);
      size_t nFrames = in.getSize();
      float v0, v1, v2, v3;
      float ic1eq = state[0];
      float ic2eq = state[1];
      for(size_t s = 0; s < nFrames; s++){
	v0 = in[s];
#ifdef SVF_COMPUTE_BOUNDED
	v3 = v0 - ic2eq;
	v1 = a1 * ic1eq + a2*v3;
	v2 = ic2eq + a2 * ic1eq + a3 * v3;
#else
	v1 = a1 * ic1eq + a2 * (v0 - ic2eq);
	v2 = ic2eq + g * v1;
#endif
	ic1eq = 2. * v1 - ic1eq;
	ic2eq = 2. * v2 - ic2eq;
	out[s] = m0 * v0 + m1 * v1 + m2 * v2;
      }
      state[0] = ic1eq;
      state[1] = ic2eq;
      state += STATE_VARIABLES_PER_CHANNEL;
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


class StereoStateVariableFilter : public MultiStateVariableFilter {
public:
  StereoStateVariableFilter(float sr, float* state) :
    MultiStateVariableFilter(sr, 2, state) {}
  static StereoStateVariableFilter* create(float sr){
    return new StereoStateVariableFilter(sr, new float[STATE_VARIABLES_PER_CHANNEL]);
  }

  static void destroy(StereoStateVariableFilter* svf){
    MultiStateVariableFilter::destroy(svf);
  }

};

#endif // __StateVariableFilter_h__
