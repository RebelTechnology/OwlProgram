#include "WavetableOscillator.h"
#include "basicmaths.h"
#include <stdint.h>

WavetableOscillator* WavetableOscillator::create(float sr, size_t size) {
  FloatArray wave = FloatArray::create(size);
  for(size_t i=0; i<size; ++i)
    wave[i] = sinf(2*M_PI*i/(size-1));    
  return new WavetableOscillator(sr, wave);
}

void WavetableOscillator::destroy(WavetableOscillator* osc){
  FloatArray::destroy(osc->wave);
  delete osc;
}

WavetableOscillator::WavetableOscillator(float sr, const FloatArray wavetable):
  mul(1.0/sr),
  wave(wavetable),
  acc(0.0), inc(0.1)
{}
 
void WavetableOscillator::setSampleRate(float sr){
  mul = 1.0/sr;
}

void WavetableOscillator::setFrequency(float freq){
  inc = freq*mul;
}

float WavetableOscillator::getSample(float phase){
  uint32_t size = wave.getSize();
  uint32_t index = phase*(size-1);
  index = min(index, size-1);
  return wave[index];
}

float WavetableOscillator::generate(){
  float s = getSample(acc);
  acc += inc;
  if(acc >= 1.0)
    acc -= 1.0;
  return s;
}

float WavetableOscillator::generate(float fm){
  float s = getSample(acc);
  acc += inc + fm;
  if(acc > 1.0)
    acc -= 1.0;
  return s;
}

void WavetableOscillator::generate(FloatArray samples){
  for(size_t i=0; i<samples.getSize();++i)
    samples[i] = generate();
}
