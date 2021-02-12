#include "WavetableOscillator.h"
#include "basicmaths.h"
#include <stdint.h>

WavetableOscillator* WavetableOscillator::create(size_t size) {
  FloatArray wave = FloatArray::create(size);
  for(size_t i=0; i<size; ++i)
    wave[i] = sin(2*M_PI*i/(size-1));    
  return new WavetableOscillator(wave);
}

void WavetableOscillator::destroy(WavetableOscillator* osc){
  FloatArray::destroy(osc->wave);
  delete osc;
}

WavetableOscillator::WavetableOscillator(const FloatArray wavetable): 
  wave(wavetable),
  acc(0.0), inc(0.1)
{}

void WavetableOscillator::setFrequency(float freq){
  inc = freq;
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
