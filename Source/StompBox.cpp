#include <cstddef>
#include <string.h>
#include "StompBox.h"
#include "owlcontrol.h"
#include "device.h"
#include "ProgramVector.h"
#include "PatchProcessor.h"
#include "basicmaths.h"

AudioBuffer::~AudioBuffer(){}

PatchProcessor* getInitialisingPatchProcessor();

Patch::Patch() : processor(getInitialisingPatchProcessor()){
}

Patch::~Patch(){}

void Patch::registerParameter(PatchParameterId pid, const char* name, const char* description){
  if(getProgramVector()->registerPatchParameter != NULL)
    getProgramVector()->registerPatchParameter(pid, name);
}

double Patch::getSampleRate(){
  return getProgramVector()->audio_samplingrate;
}

int Patch::getBlockSize(){
  return getProgramVector()->audio_blocksize;
}

float Patch::getParameterValue(PatchParameterId pid){
  return processor->getParameterValue(pid);
  // if(pid < getProgramVector()->parameters_size)
  //   return getProgramVector()->parameters[pid]/4096.0f;
  // return 0.0;
}

AudioBuffer* Patch::createMemoryBuffer(int channels, int samples){
  return processor->createMemoryBuffer(channels, samples);
  // MemoryBuffer* buf = new ManagedMemoryBuffer(channels, size);
  // if(buf == NULL)
  //   return NULL;
  // buffers[bufferCount++] = buf;
  // buf->clear();
  // return buf;
}

void Patch::setButton(PatchButtonId bid, bool pressed){
  // processor->setButton(bid, pressed);
  if(pressed)
    getProgramVector()->buttons |= 1<<bid;
  else
    getProgramVector()->buttons &= ~(1<<bid);
}

bool Patch::isButtonPressed(PatchButtonId bid){
  // return processor->isButtonPressed(bid);
  return getProgramVector()->buttons & (1<<bid);
}

float ComplexFloatArray::mag(const int i){
  float result;
  arm_cmplx_mag_f32((float*)&(data[i]), &result,1);
  return result;
}

void ComplexFloatArray::getMagnitudeValues(FloatArray& dest){
  arm_cmplx_mag_f32((float*)data, (float*)dest, sz);
}

float ComplexFloatArray::mag2(const int i){
  float result;
  arm_cmplx_mag_squared_f32((float*)&(data[i]), &result, 1);
  return result;
}

void ComplexFloatArray::getMagnitudeSquaredValues(FloatArray& dest){
  arm_cmplx_mag_squared_f32((float*)data, (float*)dest, sz);
}

void ComplexFloatArray::complexDotProduct(ComplexFloatArray& operand2, ComplexFloat& result){
  arm_cmplx_dot_prod_f32 ( (float*)data, (float*)operand2, sz, &(result.re), &(result.im) );
}

void ComplexFloatArray::complexByComplexMultiplication(ComplexFloatArray& operand2, ComplexFloatArray& result){
  int minSize=min(sz,operand2.getSize()); //TODO: shall we take this out and allow it to segfault?
  arm_cmplx_mult_cmplx_f32 ( (float*)data, (float*)operand2, (float*)result, minSize );  
}

void ComplexFloatArray::getComplexConjugateValues(ComplexFloatArray& buf){
  int minSize= min(sz,buf.getSize()); //TODO: shall we take this out and allow it to segfault?
  arm_cmplx_conj_f32( (float*)data, (float*)buf, minSize );  
}
void ComplexFloatArray::complexByRealMultiplication(FloatArray& operand2, ComplexFloatArray& result){
  int minSize= min(sz,operand2.getSize()); //TODO: shall we take this out and allow it to segfault?
  arm_cmplx_mult_real_f32 ( (float*)data, (float*)operand2, (float*)result, minSize );  
}

int ComplexFloatArray::getMaxMagnitudeIndex(){ //this is probably slower than getMagnitudeSquaredValues() and getMaxIndex() on it
  float maxMag=-1;
  int maxInd=-1;
  for(int n=0; n<sz; n++){
    float magnitude=mag2(n); //uses mag2 which is faster
    if(magnitude>maxMag){
      maxMag=magnitude;
      maxInd=n;
    }
  }
  return maxInd;
}

float ComplexFloatArray::getMaxMagnitudeValue(){ //this is probably slower than getMagnitudeSquaredValues() and getMaxValue() on it
  float maxMag=-1;
  for(int n=0; n<sz; n++){
    float mag=this->mag2(n);
    if(mag>maxMag){
      maxMag=mag;
    }
  }
  maxMag=sqrtf(maxMag);
  return maxMag;
}

void ComplexFloatArray::getRealValues(FloatArray& buf){
  for(int n=0; n<sz; n++){
    buf[n]=data[n].re;
  }
}

void ComplexFloatArray::getImaginaryValues(FloatArray& buf){
  for(int n=0; n<sz; n++){
    buf[n]=data[n].im;
  }
}

void FloatArray::getMin(float* value, long unsigned int* index){
  arm_min_f32((float *)data, sz, value, index);
}

float FloatArray::getMinValue(){
  float value;
  long unsigned int index;
  getMin(&value, &index);
  return value;
}

int FloatArray::getMinIndex(){
  float value;
  long unsigned int index;
  getMin(&value, &index);
  return index;
}

void FloatArray::getMax(float* value, long unsigned int* index){
  arm_max_f32((float *)data, sz, value, index);
}

float FloatArray::getMaxValue(){
  float value;
  long unsigned int index;
  getMax(&value, &index);
  return value;
}

int FloatArray::getMaxIndex(){
  float value;
  long unsigned int index;
  getMax(&value, &index);
  return index;
}

void FloatArray::rectify(FloatArray& destination){ //this is actually "copy data with rectifify"
  int minSize= min(sz,destination.getSize()); //TODO: shall we take this out and allow it to segfault?
  arm_abs_f32( (float*)data, (float*)destination, sz);
}

void FloatArray::rectify(){//in place //TODO: rewrite this so that it calls the overloaded method
  rectify(*this);
}

void FloatArray::reverse(FloatArray& destination){ //this is actually "copy data with reverse"
  for(int n=0; n<sz; n++){
    destination[n]=data[sz-n-1];
  }
}

void FloatArray::reverse(){//in place //TODO: rewrite this so that it calls the overloaded method
  reverse(*this);
}

float FloatArray::getRms(){
  float result;
  arm_rms_f32 ((float *)data, sz, &result);
  return result;
}

float FloatArray::getMean(){
  float result;
  arm_mean_f32 ((float *)data, sz, &result);
  return result;
}

float FloatArray::getPower(){
  float result;
  arm_power_f32 ((float *)data, sz, &result);
  return result;
}

float FloatArray::getStandardDeviation(){
  float result;
  arm_std_f32 ((float *)data, sz, &result);
  return result;
}

float FloatArray::getVariance(){
  float result;
  arm_var_f32 ((float *)data, sz, &result);
  return result;
}

void FloatArray::scale(float factor){
  arm_scale_f32 ( (float*)data, factor, (float*)data, sz);
}

#define DWT_CYCCNT ((volatile unsigned int *)0xE0001004)

float Patch::getElapsedBlockTime(){
  return (*DWT_CYCCNT)/getBlockSize()/3500.0;
}

int Patch::getElapsedCycles(){
  return *DWT_CYCCNT;
}

