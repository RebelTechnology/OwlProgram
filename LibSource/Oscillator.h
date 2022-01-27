#ifndef __Oscillator_h
#define __Oscillator_h

#include "basicmaths.h"
#include "FloatArray.h"
#include "SignalGenerator.h"

/**
 * An Oscillator is a SignalGenerator that operates at a given frequency
 * and that can be frequency modulated.
 */
class Oscillator : public SignalGenerator {
public:
  Oscillator(){}
  virtual ~Oscillator(){}
  using SignalGenerator::generate;
  /**
   * Set oscillator sample rate
   */
  virtual void setSampleRate(float value){}
  /**
   * Set oscillator frequency in Hertz
   */
  virtual void setFrequency(float value) = 0;
  /**
   * Get oscillator frequency in Hertz
   */
  virtual float getFrequency() = 0;
  /**
   * Set current oscillator phase in radians
   * @param phase a value between 0 and 2*pi
   */
  virtual void setPhase(float phase) = 0;
  /**
   * Get current oscillator phase in radians
   * @return a value between 0 and 2*pi
   */
  virtual float getPhase() = 0;
  /**
   * Reset oscillator (typically resets phase)
   */
  virtual void reset() = 0;
  /**
   * Produce a sample with frequency modulation.
   */
  virtual float generate(float fm) = 0;
  /**
   * Produce a block of samples with frequency modulation.
   */
  virtual void generate(FloatArray output, FloatArray fm){
    for(size_t i=0; i<output.getSize(); ++i)
      output[i] = generate(fm[i]);
  }
  [[deprecated("use generate() instead.")]]
  float getNextSample(){ return generate(); }
  [[deprecated("use generate() instead.")]]
  float getNextSample(float fm){ return generate(fm); }
  [[deprecated("use generate() instead.")]]
  void getSamples(FloatArray output){
    generate(output);
  }
  [[deprecated("use generate() instead.")]]
  void getSamples(FloatArray output, FloatArray fm){
    generate(output, fm);
  }
};

template<class T, class BaseOscillator=Oscillator, typename Sample=float>
class OscillatorTemplate : public BaseOscillator {
protected:
  float mul = 1;  // Prevent having NaN as default getFrequency result
  float phase = 0;
  float incr = 0;
public:
  void setSampleRate(float sr){
    float freq = getFrequency();
    mul = (T::end_phase - T::begin_phase)/sr;
    setFrequency(freq);
  }
  float getSampleRate(){
    return (T::end_phase - T::begin_phase)/mul;
  }
  void setFrequency(float freq){
    incr = freq*mul;
  }
  float getFrequency(){
    return incr/mul;
  }
  void setPhase(float ph){
    phase = (T::end_phase - T::begin_phase)*ph/(2*M_PI) + T::begin_phase;
  }
  float getPhase(){
    // return phase 0 to 2*pi
    return (phase - T::begin_phase)*2*M_PI/(T::end_phase - T::begin_phase);
  }
  void reset(){
    phase = T::begin_phase;
  }
  Sample generate() {
    Sample sample = static_cast<T*>(this)->getSample();
    phase += incr;
    if(phase >= T::end_phase)
      phase -= (T::end_phase - T::begin_phase);
    return sample;
  }
  Sample generate(float fm) {
    Sample sample = static_cast<T*>(this)->getSample();
    // phase += incr * (1 + fm);
    phase += incr  + (T::end_phase - T::begin_phase)*fm;
    if(phase >= T::end_phase)
      phase -= (T::end_phase - T::begin_phase);
    return sample;
  }  
  using BaseOscillator::generate;
  static T* create(float sr){
    T* obj = new T();
    obj->setSampleRate(sr);
    return obj;
  }
  static void destroy(T* osc){
    delete osc;
  }
protected:
  /** 
   * Calculate poly blep antialiasing compensation on normalised (to range [0, 1])
   * phase and phase increment (angular rate) values.
   */
  static float polyblep(float t, float dt){
    // PolyBLEP by various
    // http://research.spa.aalto.fi/publications/papers/smc2010-phaseshaping/
    // https://www.kvraudio.com/forum/viewtopic.php?t=375517
    // http://www.martin-finke.de/blog/articles/audio-plugins-018-polyblep-oscillator/
    // https://www.metafunction.co.uk/post/all-about-digital-oscillators-part-2-blits-bleps
    if(t < dt){
      t /= dt;
      return t+t - t*t - 1;
    }else if(t + dt > 1){
      t = (t - 1) / dt;
      return t*t + t+t + 1;
    }
    return 0;
  }
};

template<class Osc>
class PhaseShiftOscillator : public Osc {
protected:
  float phaseshift;
public:
  template <typename... Args>
  PhaseShiftOscillator(float phaseshift, Args&&... args) :
    Osc(std::forward<Args>(args)...), phaseshift(phaseshift) {}
  void setPhase(float phase){
    Osc::setPhase(phase +  phaseshift);
  }
  float getPhase(){
    return Osc::getPhase() - phaseshift;
  }
  void reset(){
    Osc::setPhase(phaseshift);
  }
  /**
   * @param phaseshift oscillator phase shift in radians
   */
  template <typename... Args>
  static PhaseShiftOscillator<Osc>* create(float phaseshift, Args&&... args){
    return new PhaseShiftOscillator<Osc>(phaseshift, std::forward<Args>(args)...);
  }    
  static void destroy(PhaseShiftOscillator<Osc>* obj){
    Osc::destroy(obj);
  }
};

#endif /* __Oscillator_h */
