#include "Patch.h"
#include "WavetableOscillator.h"
#include "basicmaths.h"
#include <stdint.h>
#include "message.h"

WavetableOscillator* WavetableOscillator::create(int size) {
  FloatArray wave = FloatArray::create(size);
  for(int i=0; i<size; ++i)
    wave[i] = sin(2*M_PI*i/(size-1));    
  return new WavetableOscillator(wave);
}
static void destroy(WavetableOscillator* wavetableOscillator){
  FloatArray::destroy(wavetableOscillator->getTable());
  delete wavetableOscillator;
}

WavetableOscillator::WavetableOscillator(const FloatArray wavetable): 
  WavetableOscillator::WavetableOscillator(1, wavetable)
{}

WavetableOscillator::WavetableOscillator(unsigned int timeBase, const FloatArray wavetable): 
  wave(wavetable),
  acc(0.0), inc(0.1)
{
  setTimeBase(timeBase);
}

void WavetableOscillator::setFrequency(float newFreq){
  freq = newFreq;
  inc = freq * timeBaseOverFs;  
}

void WavetableOscillator::setTimeBase(unsigned int samples){
  timeBaseOverFs = samples / Patch::getSampleRate();
  setFrequency(freq);
}

float WavetableOscillator::getSample(float phase){
  uint32_t size = wave.getSize();
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

float WavetableOscillator::getNextSample(float fm){
  float s = getSample(acc);
  acc += inc + fm;
  while(acc > 1.0)
    acc -= 1.0;
  while(acc < -1.0)
    acc += 1.0;
  return s;
}

void WavetableOscillator::setTable(FloatArray table){
  wave = table;
}

FloatArray WavetableOscillator::getTable(){
  return wave;
}

