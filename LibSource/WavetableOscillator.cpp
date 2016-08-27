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
  acc(0), inc(1)
{
  setTimeBase(timeBase);
  intSize = wave.getSize() << fracBits;
}

void WavetableOscillator::setFrequency(float newFreq){
  freq = newFreq;
  inc = freq * timeBaseOverFs * intSize;
}

void WavetableOscillator::setTimeBase(unsigned int samples){
  timeBaseOverFs = samples / Patch::getSampleRate();
  setFrequency(freq);
}

float WavetableOscillator::getCurrentSample(){
  int index = acc >> fracBits; 
  return wave[index];
}

// NOTE: phase is normalized between 0 and 1
float WavetableOscillator::getSample(float phase){
  int index = phase * intSize;
  index = index >> fracBits;
  return wave[index];
}

float WavetableOscillator::getNextSample(){
  float s = getCurrentSample();
  acc += inc;
  if(acc >= intSize)
    acc -= intSize;
  return s;
}

float WavetableOscillator::getNextSample(float fm){
  float s = getCurrentSample();
  acc += inc + fm * intSize;
  while(acc >= intSize)
    acc -= intSize;
  while(acc < 0)
    acc += intSize;
  return s;
}

void WavetableOscillator::setTable(FloatArray table){
  wave = table;
}

FloatArray WavetableOscillator::getTable(){
  return wave;
}

#if 0
float SmoothWavetableOscillator::getSample(float phase){
  uint32_t size = wave.getSize();
  float index = phase * size;
  uint32_t fix = (int)index;
  float frac = index - fix;
  float value;
  if(index >= size){
    index -= size;
  }
  if(index == size - 1){
    value = interpolate(wave[size - 1], wave[0], frac);
  } else {
    value = interpolate(wave[index], wave[index + 1], frac);
  }
  return value;
}
#endif
