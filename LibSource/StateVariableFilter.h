#ifndef __StateVariableFilter_h__
#define __StateVariableFilter_h__

/**
 * State Variable Filter based on Andy Simper's code:
 * @ref http://www.cytomic.com/files/dsp/SvfLinearTrapOptimised2.pdf
 */

class StateVariableFilter {
public:

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

  void setLowPass(float freq, float q, float sampleRate){
    setLowPass(freq/sampleRate, q);
  }

  void setLowPass(float fnorm, float q){
    const float w = tanf(M_PI * fnorm);
    const float g = w;
    const float k = 1. / q;
    m_a1 = 1./(1. + g * (g + k));
    m_a2 = g * m_a1;
    m_a3 = g * m_a2;
    m_m0 = 0;
    m_m1 = 0;
    m_m2 = 1.;
  }

  void setHighPass(float freq, float q, float sampleRate){
    setHighPass(freq/sampleRate, q);
  }
  void setHighPass(float fnorm, float q){
    const float w = tanf(M_PI * fnorm);
    const float g = w;
    const float k = 1. / q;
    m_a1 = 1./(1. + g * (g + k));
    m_a2 = g * m_a1;
    m_a3 = g * m_a2;
    m_m0 = 1.;
    m_m1 = -k;
    m_m2 = -1.;
  }

  void setBandPass(float freq, float q, float sampleRate){
    setBandPass(freq/sampleRate, q);
  }
  void setBandPass(float fnorm, float q){
    const float w = tanf(M_PI * fnorm);
    const float g = w;
    const float k = 1. / q;
    m_a1 = 1./(1. + g * (g + k));
    m_a2 = g * m_a1;
    m_a3 = g * m_a2;
    m_m0 = 0.;
    m_m1 = 1.;
    m_m2 = 0.;
  }

  void setNotch(float freq, float q, float sampleRate){
    setNotch(freq/sampleRate, q);
  }
  void setNotch(float fnorm, float q){
    const float w = tanf(M_PI * fnorm);
    const float g = w;
    const float k = 1. / q;
    m_a1 = 1./(1. + g * (g + k));
    m_a2 = g * m_a1;
    m_a3 = g * m_a2;
    m_m0 = 1.;
    m_m1 = -k;
    m_m2 = 0.;
  }

  void setPeak(float freq, float q, float sampleRate){
    setPeak(freq/sampleRate, q);
  }
  void setPeak(float fnorm, float q){
    const float w = tanf(M_PI * fnorm);
    const float g = w;
    const float k = 1. / q;
    m_a1 = 1./(1. + g * (g + k));
    m_a2 = g * m_a1;
    m_a3 = g * m_a2;
    m_m0 = 1.;
    m_m1 = -k;
    m_m2 = -2.;
  }
  
  void setBell(float freq, float q, float gain, float sampleRate){
    setBell(freq/sampleRate, q, gain);
  }
  void setBell(float fnorm, float q, float gain){
    const float w = tanf(M_PI * fnorm);
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
  
  void setLowShelf(float freq, float q, float gain, float sampleRate){
    setLowShelf(freq/sampleRate, q, gain);
  }
  void setLowShelf(float fnorm, float q, float gain){
    const float w = tanf(M_PI * fnorm);
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

  void setHighShelf(float freq, float q, float gain, float sampleRate){
    setHighShelf(freq/sampleRate, q, gain);
  }
  void setHighShelf(float fnorm, float q, float gain){
    const float w = tanf(M_PI * fnorm);
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

private:
  // state
  float mV1 = 0.;
  float mV2 = 0.;
  float mV3 = 0.;
  float mIc1eq = 0.;
  float mIc2eq = 0.;
  // coefficients
  float m_a1 = 0.;
  float m_a2 = 0.;
  float m_a3 = 0.;
  float m_m0 = 0.;
  float m_m1 = 0.;
  float m_m2 = 0.;
};

#endif // __StateVariableFilter_h__
