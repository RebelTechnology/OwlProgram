#ifndef __VoltsPerOctave_hpp__
#define __VoltsPerOctave_hpp__

#include "FloatArray.h"
#include "basicmaths.h"

class VoltsPerOctave {
private:
  float offset;
  float multiplier;
  float tune;
public:
  VoltsPerOctave();
  VoltsPerOctave(float offset, float multiplier);
  // Tune the frequency converter to octaves up or down.
  // For semitones divide by 12. For cents divide by 1200.
  void setTune(float octaves){
    tune = octaves;
  }
  float getFrequency(float sample){
    return voltsToHerz(sampleToVolts(sample)+tune);
  }
  void getFrequency(FloatArray samples, FloatArray output);
  float sampleToVolts(float sample){
    return (sample-offset) * multiplier;
  }
  float voltsToHerz(float volts){
    return 440.f * powf(2, volts);
  }
};

#endif /* __VoltsPerOctave_hpp__ */
