#ifndef __VariableShapeOscillator_h
#define __VariableShapeOscillator_h

#include "Oscillator.h"

/**
 * Oscillator with a waveshape that is variable from saw to square, with adjustable pulse width
 */
class VariableShapeOscillator : public OscillatorTemplate<VariableShapeOscillator> {
protected:
  float waveshape = 0;
  float pw = 0.5f;
public:
  static constexpr float begin_phase = 0;
  static constexpr float end_phase = 1;
  /**
   * Set pulse width to a value between 0 (for saw) and 1 (for square)
   */
  void setWaveshape(float value){
    waveshape = value;
  }
  /**
   * Set pulse width to a value between 0 and 1
   */
  void setPulseWidth(float value){
    pw = value;
  }
  float getSample(){
    float square = phase < pw ? 0 : 1;
    float saw = phase;
    float sample = saw + (square - saw) * waveshape;
    return sample * 2 - 1;
  }
};
  
#endif /* __VariableShapeOscillator_h */
