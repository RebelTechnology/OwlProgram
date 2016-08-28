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

WavetableOscillator::WavetableOscillator(): 
  //TODO: once #55 is fixed, this can be set to call the overloaded constructor
  //WavetableOscillator::WavetableOscillator(1,FloatArray::create(0))
  acc(0), inc(1)
{
  setTimeBase(1);
}

WavetableOscillator::WavetableOscillator(const FloatArray wavetable): 
  WavetableOscillator::WavetableOscillator(1, wavetable)
{}

WavetableOscillator::WavetableOscillator(unsigned int timeBase, const FloatArray wavetable): 
  acc(0), inc(1)
{
  setTimeBase(timeBase);
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
  intSize = wave.getSize() << fracBits;
}

FloatArray WavetableOscillator::getTable(){
  return wave;
}

void SmoothWavetableOscillator::setTable(const FloatArray wavetable){
  wave = wavetable;
  unsigned int size = wave.getSize() - 1;
  // check size is a multiple of two smaller than 2^(32-fracBits)
  bool powerOf2 = false;
  for(unsigned int n = 1; n < 32 - fracBits; ++n){
    if(1 << n == size)
      powerOf2 = true;
  }
  ASSERT(powerOf2, "SmoothWavetableOscillator: the table must have size 2^n + 1\n");
  ASSERT(wave[size] == wave[0], "SmoothWavetableOscillator: The last value of the table must be the same as the first\n");
  intSize = size << fracBits;
}

float SmoothWavetableOscillator::getCurrentSample(){
  unsigned int index = acc >> fracBits;
  float frac = (acc & ((1 << fracBits) - 1)) / (float)(1 << fracBits);
  return interpolate(wave[index], wave[index + 1], frac);
}

float SmoothWavetableOscillator4::getCurrentSample(){
  unsigned int index = acc >> fracBits;
  float frac = (acc & ((1 << fracBits) - 1)) / (float)(1 << fracBits);
  return interpolate4(&wave[index - 1], frac);
}

float SmoothWavetableOscillator4::interpolate4(float* w, float frac){
  float a = w[0];
  float b = w[1];
  float c = w[2];
  float d = w[3];
  float cminusb = c - b;
  // borrowed this verbatim from tabosc4~ in PureData/src/d_array.c
  float out = b + frac * (
      cminusb - 0.1666667f * (1.-frac) * (
          (d - a - 3.0f * cminusb) * frac + (d + 2.0f*a - 3.0f*b)
      )
  );
  return out;
}
