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
  float tune;
public:
  float offset;
  float multiplier;
  VoltsPerOctave(bool input = true);
  VoltsPerOctave(float offset, float multiplier);
  // Tune the frequency converter to octaves up or down.
  // For semitones divide by 12. For cents divide by 1200.
  void setTune(float octaves){
    tune = octaves;
  }
  float getFrequency(float sample){
    return voltsToHertz(sampleToVolts(sample)+tune);
  }
  float sampleToVolts(float sample){
    return (sample-offset) * multiplier;
  }
  float voltsToHertz(float volts){
    return 440.f * exp2f(volts);
  }
  float voltsToSample(float volts){
    return volts / multiplier + offset;
  }
  float hertzToVolts(float hertz){
    return log2f(hertz/440.0f);
  }
  void getFrequency(FloatArray samples, FloatArray output);
  void getFrequency(FloatArray samples);
};

#endif /* __VoltsPerOctave_hpp__ */
