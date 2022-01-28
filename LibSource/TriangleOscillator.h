#ifndef __TriangleOscillator_h
#define __TriangleOscillator_h

#include "Oscillator.h"

class TriangleOscillator : public OscillatorTemplate<TriangleOscillator> {
public:
  static constexpr float begin_phase = -1;
  static constexpr float end_phase = 1;
  TriangleOscillator(){}
  TriangleOscillator(float sr){
    setSampleRate(sr);
  }  
  float getSample(){
    return -2 * (fabsf(phase) - 0.5);
  }
};

class AntialiasedTriangleOscillator : public OscillatorTemplate<AntialiasedTriangleOscillator> {
protected:
  float previousSample = 0;
public:
  static constexpr float begin_phase = 0;
  static constexpr float end_phase = 1;
  float getSample(){
    float sample = phase < 0.5f ? 1 : -1; // naive square wave
    sample += polyblep(phase, incr);
    sample -= polyblep(fmod(phase + 0.5f, 1), incr); // polyblep square wave
    // Leaky integrator: y[n] = A * x[n] + (1 - A) * y[n-1] = A * (x[n] - y[n-1]) + y[n-1]
    float lambda = incr*2*M_PI;
    sample = lambda * (sample - previousSample) + previousSample;
    previousSample = sample;
    return sample;
  }
};
  
#endif /* __TriangleOscillator_h */
