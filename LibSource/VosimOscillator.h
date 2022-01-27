#ifndef __VosimOscillator_h
#define __VosimOscillator_h

#include "Oscillator.h"
#include "PolyBlepOscillator.h"

/**
 * VOSIM-type oscillator based on Rob Hordijk's Nord Modular patch.
 * ref: https://electro-music.com/nm_classic/015_workshops/Clavia/NordModularWorkshops&Threads/WerkMap/WorkShops/Hordijk1999-2000/VOSIM.html
*/
class VosimOscillator : public OscillatorTemplate<VosimOscillator> {
private:
  SineOscillator* formant1;
  SineOscillator* formant2;
public:
  VosimOscillator(SineOscillator* osc2, SineOscillator* osc1) :
    formant1(osc1), formant2(osc2) {}
  static constexpr float begin_phase = 0;
  static constexpr float end_phase = 1;
  float waveshape = 0;
  float getSample(){
    // return 1 - phase;
    float square = phase < 0 ? 0.0f : 1.0f;
    float saw = phase;
    float sample = saw + (square - saw) * waveshape;
    return 1 - sample * (1 - waveshape*0.5);
    // return 1 - (1-waveshape)*phase + waveshape*sinf(phase*2*M_PI);
  }
  void setWaveshape(float value){
    waveshape = value*0.25;
  }    
  float getFormant1(){
    return formant1->getFrequency();
  }
  float getFormant2(){
    return formant1->getFrequency();
  }
  void setFormant1(float frequency){
    formant1->setFrequency(frequency);
  }
  void setFormant2(float frequency){
    formant2->setFrequency(frequency);
  }
  float generate(float fm){
    float sample = getSample();
    float s1 = formant1->generate();
    float s2 = formant2->generate();
    sample = (s1*s1 + s2*s2)*sample - 1;
    phase += incr * (1 + fm);
    if(phase >= end_phase){
      phase -= (end_phase - begin_phase);
#if 0
      float ph = getPhase();
      formant1->setPhase(ph);
      formant2->setPhase(ph);
#else
      formant1->reset();
      formant2->reset();
#endif
    }
    return sample;
  }
  float generate(){
    return generate(0);
  }
  void generate(FloatArray output){
    float* out = output.getData();
    size_t len = output.getSize();
    float sample, s1, s2;
    while(len--){
      sample = getSample() * (1 - 0.4*waveshape);
      s1 = formant1->generate();
      s2 = formant2->generate();
      *out++ = (s1*s1 + s2*s2)*sample;
      phase += incr;
      if(phase >= end_phase){
	phase -= (end_phase - begin_phase);
	formant1->reset();
	formant2->reset();
      }
    }
  }
  void generate(FloatArray output, FloatArray input){
    float* out = output.getData();
    float* fm = input.getData();
    size_t len = output.getSize();
    float sample, s1, s2;
    while(len--){
      sample = getSample() * (1 - 0.4*waveshape);
      s1 = formant1->generate();
      s2 = formant2->generate();
      *out++ = (s1*s1 + s2*s2)*sample;
      phase += incr * (1 + *fm++);
      if(phase >= end_phase){
	phase -= (end_phase - begin_phase);
	formant1->reset();
	formant2->reset();
      }
    }
  }
  static VosimOscillator* create(float sr){
    SineOscillator* f1 = SineOscillator::create(sr);
    SineOscillator* f2 = SineOscillator::create(sr);
    VosimOscillator* osc = new VosimOscillator(f1, f2);
    osc->setSampleRate(sr);
    return osc;
  }
  static void destroy(VosimOscillator* obj){
    SineOscillator::destroy(obj->formant1);
    SineOscillator::destroy(obj->formant2);
    delete obj;
  }
};

#endif  /* __VosimOscillator_h */
