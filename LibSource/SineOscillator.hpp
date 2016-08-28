#ifndef SINEOSCILLATOR_HPP
#define SINEOSCILLATOR_HPP

#include "Oscillator.h"

class SineOscillator : public Oscillator {
private:
  const float fs;
  float phase;
  float incr;
  float freq;
  float timeBaseOverFs;
public:
  SineOscillator() : SineOscillator(1) {}
  SineOscillator(unsigned int timeBase)
    : fs(Patch::getSampleRate()), phase(0.0f), incr(1.0f)
  {
    setTimeBase(timeBase);
  }
  void setFrequency(float newFreq){
    freq = newFreq;
    incr = freq * timeBaseOverFs;
  }
  void setPhase(float ph){
    phase = ph;
    while(phase >= 1)
      phase -= 1;
  }
  void reset(){
    phase = 0.0f;
  }
  float getPhase(){
    return phase;
  }
  float getNextSample(){
    float sample = sinf(2 * M_PI * phase);
    phase += incr;
    if(phase >= 1) 
      phase -= 1;
    return sample;
  }
  float getNextSample(float fm){
    float sample = sinf(2 * M_PI * phase);
    phase += incr + fm;
    // if fm is very large, then phase could go above 2
    while(phase >= 1)
      phase -= 1;
    // a if -fm > incr, then the phase could go negative and we need to
    // account for that.
    while(phase < 1)
      phase += 1;
    return sample;
  }
  void setTimeBase(unsigned int samples){
    timeBaseOverFs = samples / fs;
    setFrequency(freq);
  }
};
#endif /* SINEOSCILLATOR_HPP */
