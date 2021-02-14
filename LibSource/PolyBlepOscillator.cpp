#include "PolyBlepOscillator.h"
#include "basicmaths.h"

PolyBlepOscillator::PolyBlepOscillator(float sr):
  mul(1.0/sr),
  frequency(0), 
  shape(0.5), 
  pw(0.5) {
  osc.Init();
}

PolyBlepOscillator::PolyBlepOscillator(float freq, float sr):
  mul(1.0/sr),
  shape(0.5), 
  pw(0.5) {
  setFrequency(freq);
  osc.Init();
}

void PolyBlepOscillator::setSampleRate(float sr){
  mul = 1.0/sr;
}

void PolyBlepOscillator::setFrequency(float freq){
  frequency = mul*freq;
}

void PolyBlepOscillator::setShape(float value){
  shape = value;
}

void PolyBlepOscillator::setPulseWidth(float value){
  pw = value;
}

float PolyBlepOscillator::generate(){
  float sample;
  osc.Render<true>(frequency, pw, shape, &sample, 1);
  return sample;
}

float PolyBlepOscillator::generate(float fm){
  float sample;
  osc.Render<true>(frequency+fm, pw, shape, &sample, 1);
  return sample;
}

void PolyBlepOscillator::generate(FloatArray output){
  osc.Render<true>(frequency, pw, shape, output, output.getSize());
}

void PolyBlepOscillator::generate(FloatArray output, FloatArray fm){
  fm.add(frequency); // add our base frequency
  osc.Render<true>(fm, pw, shape, output, output.getSize());
}

void PolyBlepOscillator::getSamples(FloatArray output, FloatArray freqs){
  freqs.multiply(mul); // normalise frequencies
  osc.Render<true>(freqs, pw, shape, output, output.getSize());
}

PolyBlepOscillator* PolyBlepOscillator::create(float sr){
  return new PolyBlepOscillator(sr);
}

PolyBlepOscillator* PolyBlepOscillator::create(float freq, float sr){
  return new PolyBlepOscillator(freq, sr);
}

void PolyBlepOscillator::destroy(PolyBlepOscillator* osc){
  delete osc;
}
