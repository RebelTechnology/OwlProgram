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
    incr = freq * 2 * M_PI * timeBaseOverFs;
    //debugMessage("incr: ", incr);
  }
  void setPhase(float ph){
    phase = ph;
    while(phase >= 2*M_PI)
      phase -= 2*M_PI;
  }
  void reset(){
    phase = 0.0f;
  }
  float getPhase(){
    return phase;
  }
  float getNextSample(){
    float sample = sinf(phase);
    phase += incr;
    if(phase >= 2*M_PI) 
      phase -= 2*M_PI;
    return sample;
  }
  float getNextSample(float fm){
    float sample = sinf(phase);
    phase += incr + fm;
    // if fm is very large, then phase could go above 4*M_PI
    while(phase >= 2*M_PI)
      phase -= 2*M_PI;
    // a if -fm > incr, then the phase could go negative and we need to
    // account for that.
    while(phase < -2*M_PI)
      phase += 2*M_PI;
    return sample;
  }
  void setTimeBase(unsigned int samples){
    timeBaseOverFs = samples / fs;
    setFrequency(freq);
  }
};
