#ifndef __RampOscillator_h
#define __RampOscillator_h

#include "Oscillator.h"

enum Waveshape { RAMP_SHAPE, POLYBLEP_RAMP_SHAPE, SQUARE_SHAPE, POLYBLEP_SQUARE_SHAPE, TRIANGLE_SHAPE, POLYBLEP_TRIANGLE_SHAPE };

/**
 * Ramp oscillator generates rising output values from -1 to 1.
 */
template<Waveshape shape=RAMP_SHAPE>
class RampOscillator : public Oscillator {
protected:
  float mul;
  float phase;
  float incr;
  float getSample();
public:
  RampOscillator(float sr) : phase(0.0f), incr(0.0f) {
  }
  RampOscillator(float freq, float sr) : phase(0.0f){
    setSampleRate(sr);
    setFrequency(freq);
  }
  void reset(){
    phase = -1;
  }
  void setSampleRate(float sr){
    mul = 2.0f/sr;
  }
  float getSampleRate(){
    return 2.0f/mul;
  }
  void setFrequency(float freq){
    incr = freq*mul;
  }
  float getFrequency(){
    return incr/mul;
  }
  void setPhase(float ph){
    phase = ph/M_PI - 1.0f; // internal phase is -1 to 1
  }
  float getPhase(){
    // return phase 0 to 2*pi
    return phase*M_PI+M_PI;
  }
  float generate(){
    float sample = getSample();
    phase += incr;
    if(phase >= 1.0f)
      phase -= 2.0f;
    return sample;
  }
  void generate(FloatArray output){
    size_t len = output.getSize();
    float* dest = output;
    while(phase + incr*len >= 1.0f){
      float remain = 1.0f - phase;
      size_t steps = (size_t)(remain/incr);
      for(size_t i=0; i<steps; ++i){
	*dest++ = getSample();
	phase += incr;
      }
      phase -= 2.0f;
      len -= steps;
    }
    for(size_t i=0; i<len; ++i){
      *dest++ = getSample();
      phase += incr;
    }
  }
  float generate(float fm){
    float sample = phase;
    phase += incr * (1 + fm);
    if(phase >= 1.0f)
      phase -= 2.0f;
    return sample;
  }  
  void generate(FloatArray output, FloatArray fm){
    for(size_t i=0; i<output.getSize(); ++i){
      output[i] = phase;
      phase += incr * (1 + fm[i]);
      if(phase >= 1)
	phase -= 2;
    }
  }
  static RampOscillator* create(float sr){
    return new RampOscillator(sr);
  }
  static RampOscillator* create(float freq, float sr){
    return new RampOscillator(freq, sr);
  }
  static void destroy(RampOscillator* osc){
    delete osc;
  }
};

template<>
float RampOscillator<RAMP_SHAPE>::getSample(){
  return phase;
}

template<>
float RampOscillator<POLYBLEP_RAMP_SHAPE>::getSample(){
  return phase - polyblep(incr, phase);
}
typedef RampOscillator<POLYBLEP_RAMP_SHAPE> AntialisedRampOscillator;

template<>
float RampOscillator<TRIANGLE_SHAPE>::getSample(){
  return -2 * (fabsf(phase) - 0.5);
}
typedef RampOscillator<TRIANGLE_SHAPE> TriangleOscillator;

template<>
float RampOscillator<SQUARE_SHAPE>::getSample(){
  return phase < 0 ? 1 : -1;
}
typedef RampOscillator<SQUARE_SHAPE> SquareOscillator;

template<>
float RampOscillator<POLYBLEP_SQUARE_SHAPE>::getSample(){
  float sample = phase < 0 ? 1 : -1;
  sample += polyblep(incr, phase);
  sample -= polyblep(incr, fmodf(phase + 1, 2));
  return sample;
}
typedef RampOscillator<POLYBLEP_SQUARE_SHAPE> AntialiasedSquareOscillator;

// template<Waveshape shape=SQUARE_SHAPE>
// class SquareOscillator : public RampOscillator<RAMP_SHAPE> {
// protected:
//   float pw = 0;
// public:
//   /**
//    * set pulse width to a value between 0 and 1
//    */
//   void setPulseWidth(float value){
//     pw = value*2 - 1; // scale to [-1,1]
//   }
// };

// template<>
// float SquareOscillator<SQUARE_SHAPE>::getSample(){
//   return phase < pw ? 1 : -1;
// }

// template<>
// float SquareOscillator<POLYBLEP_SQUARE_SHAPE>::getSample(){
//   float sample = phase < pw ? 1 : -1;
//   sample += polyblep(incr, phase);
//   sample -= polyblep(incr, fmod(phase + 1 + pw, 2));
//   return sample;
// }
// typedef SquareOscillator<POLYBLEP_SQUARE_SHAPE> AntialiasedSquareOscillator;

// class AntialiasedTriangleOscillator : public AntialiasedSquareOscillator {
// protected:
//   float previousSample = 0;
// };

// float AntialiasedTriangleOscillator::getSample(){
//   float sample = AntialiasedSquareOscillator::getSample();
//     // Leaky integrator: y[n] = A * x[n] + (1 - A) * y[n-1] = A * (x[n] - y[n-1]) + y[n-1]
//     float lambda = incr*0.5;
//     sample = lambda * (sample - previousSample) + previousSample;
//     previousSample = sample;
//     return sample;
// }

#endif /* __RampOscillator_h */
