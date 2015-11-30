#ifndef __VoltsPerOctave_hpp__
#define __VoltsPerOctave_hpp__

#include "FloatArray.h"
#include "basicmaths.h"

/*
 * Converts samples into volts and frequencies, following 1V/octave tuning.
 * Tuned to 440Hz. Change base note with setTune(float octave).
 */
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
  float sampleToVolts(float sample){
    return (sample-offset) * multiplier;
  }
  float voltsToHerz(float volts){
    return 440.f * powf(2, volts);
  }
  void getFrequency(FloatArray samples, FloatArray output);
};

#endif /* __VoltsPerOctave_hpp__ */
