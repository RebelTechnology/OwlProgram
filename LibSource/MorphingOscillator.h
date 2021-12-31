#ifndef __MorphingOscillator_h
#define __MorphingOscillator_h

#include "Oscillator.h"

/**
 * The MorphingOscillator wraps a fixed number of Oscillators and crossfades 
 * between their outputs.
 * Only two oscillator outputs are computed at a time. When changing between
 * crossfading oscillators, their phases are synchronised.
 */
class MorphingOscillator : public Oscillator {
protected:
  Oscillator** osc;
  size_t osc_count;
  FloatArray buffer;
  Oscillator* lo = NULL;
  Oscillator* hi = NULL;
  float xf = 0;
public:
  MorphingOscillator(Oscillator** osc, size_t count, FloatArray buffer)
    : osc(osc), osc_count(count), buffer(buffer){}
  void setFrequency(float value){
    lo->setFrequency(value);
    hi->setFrequency(value);
  }
  float getFrequency(){
    return lo->getFrequency();
  }
  void setPhase(float value){
    lo->setPhase(value);
    hi->setPhase(value);
  }
  float getPhase(){
    return lo->getPhase();
  }
  void reset(){
    lo->reset();
    hi->reset();
  }
  float generate(){
    float l = lo->generate();
    float h = hi->generate();
    return l + (h - l) * xf;
  }
  float generate(float fm){
    float l = lo->generate(fm);
    float h = hi->generate(fm);
    return l + (h - l) * xf;
  }
 void generate(FloatArray output){
   lo->generate(output);
   output.multiply(1-xf);
   hi->generate(buffer);
   buffer.multiply(xf);
   output.add(buffer);
 }
 void generate(FloatArray output, FloatArray fm){
   lo->generate(output, fm);
   output.multiply(1-xf);
   hi->generate(buffer, fm);
   buffer.multiply(xf);
   output.add(buffer);
 }
  /**
   * Morph between all configured oscillators.
   * @param value a cross fade index between 0 and 1.
   */
  void morph(float value){
    value *= osc_count - 1;
    size_t idx = clamp((size_t)value, (size_t)0, osc_count - 2);
    Oscillator* newlo = osc[idx];
    Oscillator* newhi = osc[idx+1];
    xf = value - idx;
    if(lo != newlo){
      newlo->setPhase(getPhase());
      newlo->setFrequency(getFrequency());
      lo = newlo;
    }
    if(hi != newhi){
      newhi->setPhase(getPhase());
      newhi->setFrequency(getFrequency());
      hi = newhi;
    }    
  }
  size_t getNumberOfOscillators(){
    return osc_count;
  }
  Oscillator* getOscillator(size_t index){
    return osc[index];
  }
  void setOscillator(size_t index, Oscillator* oscillator){
    osc[index] = oscillator;
    lo = hi;
    hi = oscillator;
  }
  static MorphingOscillator* create(size_t oscillator_count, size_t blocksize){
    return new MorphingOscillator(new Oscillator*[oscillator_count], oscillator_count,
				  FloatArray::create(blocksize));
  }  
  static void destroy(MorphingOscillator* obj){
    FloatArray::destroy(obj->buffer);
    delete[] obj->osc;
    delete obj;
  }
};

#endif /* __MorphingOscillator_h */
