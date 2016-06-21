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

void PolyBlepOscillator::getSamples(FloatArray samples){
  osc.Render<true>(frequency, pw, shape, samples, samples.getSize());
}
