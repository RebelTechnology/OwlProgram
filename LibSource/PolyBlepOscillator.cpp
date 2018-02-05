#include "PolyBlepOscillator.h"
#include "basicmaths.h"

PolyBlepOscillator::PolyBlepOscillator(float sr):
  multiplier(1.0/sr),
  frequency(440.0*multiplier), 
  shape(0.5), 
  pw(0.5) {
  osc.Init();
}

void PolyBlepOscillator::setSampleRate(float value){
  multiplier = 1.0/value;
}

void PolyBlepOscillator::setFrequency(float value){
  frequency = value*multiplier;
}

void PolyBlepOscillator::setShape(float value){
  shape = value;
}

void PolyBlepOscillator::setPulseWidth(float value){
  pw = value;
}

float PolyBlepOscillator::getNextSample(){
  float sample;
  osc.Render<true>(frequency, pw, shape, &sample, 1);
  return sample;
}

void PolyBlepOscillator::getSamples(FloatArray output){
  osc.Render<true>(frequency, pw, shape, output, output.getSize());
}

void PolyBlepOscillator::getSamples(FloatArray output, FloatArray frequency){
  frequency.multiply(multiplier);
  osc.Render<true>(frequency, pw, shape, output, output.getSize());
}

PolyBlepOscillator* PolyBlepOscillator::create(float sr){
  return new PolyBlepOscillator(sr);
}

void PolyBlepOscillator::destroy(PolyBlepOscillator* osc){
  delete osc;
}
