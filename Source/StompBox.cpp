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
#ifdef ARM_CORTEX
  arm_cmplx_mag_f32((float*)&(data[i]), &result,1);
#else
  result=sqrtf(mag2(i));
#endif
  return result;
}
void ComplexFloatArray::getMagnitudeValues(FloatArray& dest){
#ifdef ARM_CORTEX
  arm_cmplx_mag_f32((float*)data, (float*)dest, sz);
#else
  for(int i=0; i<sz; i++){
    dest[i]=mag(i);
  }
#endif
}
float ComplexFloatArray::mag2(const int i){
  float result;
#ifdef ARM_CORTEX
  arm_cmplx_mag_squared_f32((float*)&(data[i]), &result, 1);
#else
  float re=data[i].re;
  float im=data[i].im;
  result=re*re+im*im;
#endif  
  return result;
}
void ComplexFloatArray::getMagnitudeSquaredValues(FloatArray& dest){
#ifdef ARM_CORTEX
  arm_cmplx_mag_squared_f32((float*)data, (float*)dest, sz);
#else
  for(int i=0; i<sz; i++){
    dest[i]=mag2(i);
  }
#endif  
}
void ComplexFloatArray::complexDotProduct(ComplexFloatArray& operand2, ComplexFloat& result){
#ifdef ARM_CORTEX
  arm_cmplx_dot_prod_f32 ( (float*)data, (float*)operand2, sz, &(result.re), &(result.im) );
#else
  float *pSrcA=(float*)data;
  float *pSrcB=(float*)operand2;
  float realResult=0;    
  float imagResult=0;    
  for(int n=0; n<sz; n++) {    
      realResult += pSrcA[(2*n)+0]*pSrcB[(2*n)+0] - pSrcA[(2*n)+1]*pSrcB[(2*n)+1];    
      imagResult += pSrcA[(2*n)+0]*pSrcB[(2*n)+1] + pSrcA[(2*n)+1]*pSrcB[(2*n)+0];    
  }
  result.re=realResult;
  result.im=imagResult;
#endif  
};
void ComplexFloatArray::complexByComplexMultiplication(ComplexFloatArray& operand2, ComplexFloatArray& result){
  int minSize=min(sz,operand2.getSize()); //TODO: shall we take this out and allow it to segfault?
#ifdef ARM_CORTEX
  arm_cmplx_mult_cmplx_f32 ( (float*)data, (float*)operand2, (float*)result, minSize );  
#else
  float *pSrcA=(float*)data;
  float *pSrcB=(float*)operand2;
  float *pDst=(float*)result;
  for(int n=0; n<minSize; n++) {        
    pDst[(2*n)+0] = pSrcA[(2*n)+0] * pSrcB[(2*n)+0] - pSrcA[(2*n)+1] * pSrcB[(2*n)+1];        
    pDst[(2*n)+1] = pSrcA[(2*n)+0] * pSrcB[(2*n)+1] + pSrcA[(2*n)+1] * pSrcB[(2*n)+0];        
  }        
#endif  
};
void ComplexFloatArray::getComplexConjugateValues(ComplexFloatArray& buf){
  int minSize= min(sz,buf.getSize()); //TODO: shall we take this out and allow it to segfault?
#ifdef ARM_CORTEX
  arm_cmplx_conj_f32( (float*)data, (float*)buf, minSize );  
#else
  float *pSrc=(float*)data;
  float *pDst=(float *)buf;
  for(int n=0; n<sz; n++) {        
    pDst[(2*n)+0] = pSrc[(2*n)+0];     // real part        
    pDst[(2*n)+1] = -pSrc[(2*n)+1];    // imag part        
}   
#endif  
}
void ComplexFloatArray::complexByRealMultiplication(FloatArray& operand2, ComplexFloatArray& result){
  int minSize= min(sz,operand2.getSize()); //TODO: shall we take this out and allow it to segfault?
#ifdef ARM_CORTEX
  arm_cmplx_mult_real_f32 ( (float*)data, (float*)operand2, (float*)result, minSize );  
#else
  float *pSrcCmplx=(float*)data;
  float *pSrcReal=(float*)operand2;
  float *pCmplxDst=(float*)result;
  for(int n=0; n<sz; n++) {        
      pCmplxDst[(2*n)+0] = pSrcCmplx[(2*n)+0] * pSrcReal[n];        
      pCmplxDst[(2*n)+1] = pSrcCmplx[(2*n)+1] * pSrcReal[n];        
  }        
#endif
};
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
};
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
};
void ComplexFloatArray::getRealValues(FloatArray& buf){
  for(int n=0; n<sz; n++){
    buf[n]=data[n].re;
  }
};
void ComplexFloatArray::getImaginaryValues(FloatArray& buf){
  for(int n=0; n<sz; n++){
    buf[n]=data[n].im;
  }
};

void FloatArray::getMin(float* value, long unsigned int* index){
#ifdef ARM_CORTEX  
  arm_min_f32((float *)data, sz, value, index);
#else
  *value=data[0];
  *index=0;
  for(int n=1; n<sz; n++){
    float currentValue=data[n];
    if(currentValue<*value){
      *value=currentValue;
      *index=n;
    }
  }
#endif
};
float FloatArray::getMinValue(){
  float value;
  long unsigned int index;
  getMin(&value, &index);
  return value;
};
int FloatArray::getMinIndex(){
  float value;
  long unsigned int index;
  getMin(&value, &index);
  return index;
};
void FloatArray::getMax(float* value, long unsigned int* index){
#ifdef ARM_CORTEX  
  arm_max_f32((float *)data, sz, value, index);
#else
  *value=data[0];
  *index=0;
  for(int n=1; n<sz; n++){
    float currentValue=data[n];
    if(currentValue>*value){
      *value=currentValue;
      *index=n;
    }
  }
#endif
};
float FloatArray::getMaxValue(){
  float value;
  long unsigned int index;
  getMax(&value, &index);
  return value;
};
int FloatArray::getMaxIndex(){
  float value;
  long unsigned int index;
  getMax(&value, &index);
  return index;
};
void FloatArray::rectify(FloatArray& destination){ //this is actually "copy data with rectifify"
  int minSize= min(sz,destination.getSize()); //TODO: shall we take this out and allow it to segfault?
#ifdef ARM_CORTEX  
  arm_abs_f32( (float*)data, (float*)destination, sz);
#else
  for(int n=0; n<minSize; n++){
    destination[n]=abs(data[n]);
  }
#endif  
};
void FloatArray::rectify(){//in place
  rectify(*this);
};
void FloatArray::reverse(FloatArray& destination){ //this is actually "copy data with reverse"
  if(destination==*this){ //make sure it is not called "in-place"
    reverse();
    return;
  }
  for(int n=0; n<sz; n++){
    destination[n]=data[sz-n-1];
  }
}
void FloatArray::reverse(){//in place
  for(int n=0; n<sz/2; n++){
    float temp=data[n];
    data[n]=data[sz-n-1];
    data[sz-n-1]=temp;
  }
}
float FloatArray::getRms(){
  float result;
#ifdef ARM_CORTEX  
  arm_rms_f32 ((float *)data, sz, &result);
#else
  result=0;
  float *pSrc=(float *)data;
  for(int n=0; n<sz; n++){
    result+=pSrc[n]*pSrc[n];
  }
  result=sqrtf(result/sz);
#endif
  return result;
};
float FloatArray::getMean(){
  float result;
#ifdef ARM_CORTEX  
  arm_mean_f32 ((float *)data, sz, &result);
#else
  result=0;
  for(int n=0; n<sz; n++){
    result+=data[n];
  }
  result=result/sz;
#endif
  return result;
};
float FloatArray::getPower(){
  float result;
#ifdef ARM_CORTEX  
  arm_power_f32 ((float *)data, sz, &result);
#else
  result=0;
  float *pSrc=(float *)data;
  for(int n=0; n<sz; n++){
    result+=pSrc[n]*pSrc[n];
  }
#endif
  return result;
};
float FloatArray::getStandardDeviation(){
  float result;
#ifdef ARM_CORTEX  
  arm_std_f32 ((float *)data, sz, &result);
#else
  result=sqrtf(getVariance());
#endif
  return result;
};
float FloatArray::getVariance(){
  float result;
#ifdef ARM_CORTEX  
  arm_var_f32((float *)data, sz, &result);
#else
  float sumOfSquares=getPower();
  float sum=0;
  for(int n=0; n<sz; n++){
    sum+=data[n];
  }
  result=(sumOfSquares - sum*sum/sz) / (sz - 1);
#endif
  return result;
};
void FloatArray::scale(float factor){
#ifdef ARM_CORTEX  
  arm_scale_f32 ( (float*)data, factor, (float*)data, sz);
#else
  for(int n=0; n<sz; n++){
    data[n]*=factor;
  }
#endif
}


#define DWT_CYCCNT ((volatile unsigned int *)0xE0001004)

float Patch::getElapsedBlockTime(){
  return (*DWT_CYCCNT)/getBlockSize()/3500.0;
}

int Patch::getElapsedCycles(){
  return *DWT_CYCCNT;
}

