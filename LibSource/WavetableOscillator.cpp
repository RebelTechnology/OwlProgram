#include "WavetableOscillator.h"
#include "basicmaths.h"

WavetableOscillator::WavetableOscillator(float sr, int size) : 
  multiplier(1.0/sr), 
  wave(FloatArray::create(size)), 
  acc(0.0), inc(0.0){
  for(int i=0; i<size; ++i)
    wave[i] = sin(2*M_PI*i/(size-1));    
}

WavetableOscillator::WavetableOscillator(float sr, const FloatArray wavetable): 
  multiplier(1.0/sr),
  wave(wavetable),
  acc(0.0), inc(0.1){}

float WavetableOscillator::setFrequency(float freq){
  //    inc = max(0.0, min(0.5, freq*multiplier));
  inc = freq*multiplier;
}

float WavetableOscillator::getSample(float phase){
  int size = wave.getSize();
  uint32_t index = phase*(size-1);
  index = min(index, size-1);
  return wave[index];
}

float WavetableOscillator::getNextSample(){
  float s = getSample(acc);
  acc += inc;
  if(acc > 1.0)
    acc -= 1.0;
  return s;
}

void WavetableOscillator::getSamples(FloatArray samples){
  for(int i=0; i<samples.getSize();++i)
    samples[i] = getNextSample();
}
