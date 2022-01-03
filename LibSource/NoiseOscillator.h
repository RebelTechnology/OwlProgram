#ifndef NOISE_OSCILLATOR_HPP
#define NOISE_OSCILLATOR_HPP

#include "Oscillator.h"

/**
 * The NoiseOscillator generates random values in the range [-1, 1] at
 * a given frequency.
 * It behaves like a white noise generator going into a sample and hold.
 */
class NoiseOscillator : public OscillatorTemplate<NoiseOscillator> {
protected:
  float sample = 0;
public:
  static constexpr float begin_phase = 0;
  static constexpr float end_phase = 1;
  NoiseOscillator(){}
  NoiseOscillator(float sr){
    setSampleRate(sr);
  }  
  float getSample(){
    return sample;
  }
  float generate() {
    phase += incr;
    if(phase >= 1){
      sample = randf()*2 - 1;
      phase -= 1;
    }
    return sample;
  }
  using OscillatorTemplate<NoiseOscillator>::generate;  
  void reset(){
    sample = randf()*2 - 1;
    phase = 0;
  }
  using SignalGenerator::generate;
};

#endif /* NOISE_OSCILLATOR_HPP */
