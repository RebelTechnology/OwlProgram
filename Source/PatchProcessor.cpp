#include "PatchProcessor.h"
#include "MemoryBuffer.hpp"
#include "device.h"
#include <string.h>
#include "ProgramVector.h"
#include "SmoothValue.h"

PatchProcessor::PatchProcessor() 
  : patch(NULL), bufferCount(0), parameterCount(0) {
  for(int i=0; i<NOF_ADC_VALUES; ++i)
    parameters[i] = NULL;
}

PatchProcessor::~PatchProcessor(){
  clear();
}

void PatchProcessor::clear(){
  for(int i=0; i<bufferCount; ++i)
    delete buffers[i];
  bufferCount = 0;
  delete patch;
  patch = NULL;
  index = -1;
  // memset(parameterNames, 0, sizeof(parameterNames));
}

void PatchProcessor::setPatch(Patch* p){
  patch = p;
}

AudioBuffer* PatchProcessor::createMemoryBuffer(int channels, int size){
  MemoryBuffer* buf = new ManagedMemoryBuffer(channels, size);
  if(buf == NULL)
    return NULL;
  buffers[bufferCount++] = buf;
  buf->clear();
  return buf;
}

void PatchProcessor::setParameterValues(uint16_t *params){
  for(int i=0; i<parameterCount; ++i)
    parameters[i]->update(params[i]);
}

template<typename T, typename V>
class LinearParameterUpdater : public ParameterUpdater {
private:
  PatchParameter<T>* parameter;
  T minimum;
  T maximum;
  V value;
public:
  LinearParameterUpdater(T min, T max, V initialValue)
    : parameter(NULL), minimum(min), maximum(max), value(initialValue) {}
  void update(uint16_t newValue){
    value = (newValue*(maximum-minimum))/4096+minimum;
    if(parameter != NULL)
      parameter->update((T)value);
  }
  void setParameter(PatchParameter<T>* p){
    parameter = p;
  }
};

// void setSkew(float mid){
//   if (maximum > minimum)
//     skew = log (0.5) / log ((mid - minimum) / (maximum - minimum));
// }

template<typename T, typename V>
class ExponentialParameterUpdater : public ParameterUpdater {
private:
  PatchParameter<T>* parameter;
  float skew;
  T minimum;
  T maximum;
  V value;
public:
  ExponentialParameterUpdater(float skw, T min, T max, V initialValue)
    : parameter(NULL), skew(skw), minimum(min), maximum(max), value(initialValue) {
    //    ASSERT(skew > 0.0 && skew <= 2.0, "Invalid exponential skew value");
    ASSERT(skew > 0.0, "Invalid exponential skew value");
  }
  void update(uint16_t newValue){
    float v = newValue/4096.0f;
    v = expf(logf(v)/skew);
    value = v*(maximum-minimum)+minimum;
    if(parameter != NULL)
      parameter->update((T)value);
  }
  void setParameter(PatchParameter<T>* p){
    parameter = p;
  }
};

double PatchProcessor::getSampleRate(){
  return getProgramVector()->audio_samplingrate;
}

int PatchProcessor::getBlockSize(){
  return getProgramVector()->audio_blocksize;
}

template<typename T>
PatchParameter<T> PatchProcessor::getParameter(const char* name, T min, T max, T defaultValue, float lambda, float delta, float skew){
  // todo: lambda and delta: if non-zero, smooth/stiff
  int pid = 0;
  int blocksize = getBlockSize();
  if(parameterCount < 5){
    pid = parameterCount++;
    if(getProgramVector()->registerPatchParameter != NULL)
      getProgramVector()->registerPatchParameter(pid, name);
    if(parameters[pid] != NULL)
      delete parameters[pid];
    ParameterUpdater* updater = NULL;
    T l = SmoothValue<T>::normal(lambda, blocksize);
    T d = StiffValue<T>::normal(delta);
    if(skew == 1.0){
      if(lambda == 0.0 && delta == 0.0){
	updater = new LinearParameterUpdater<T, T>(min, max, defaultValue);
      }else if(delta == 0.0){
	updater = new LinearParameterUpdater<T, SmoothValue<T> >(min, max, SmoothValue<T>(l, defaultValue));
      }else if(lambda == 0.0){      
	updater = new LinearParameterUpdater<T, StiffValue<T> >(min, max, StiffValue<T>(d, defaultValue));
      }else{
	updater = new LinearParameterUpdater<T, SmoothStiffValue<T> >(min, max, SmoothStiffValue<T>(l, d, defaultValue));
      }
    }else{
      if(lambda == 0.0 && delta == 0.0){
	updater = new ExponentialParameterUpdater<T, T>(skew, min, max, defaultValue);
      }else if(delta == 0.0){
	updater = new ExponentialParameterUpdater<T, SmoothValue<T> >(skew, min, max, SmoothValue<T>(l, defaultValue));
      }else if(lambda == 0.0){      
	updater = new ExponentialParameterUpdater<T, StiffValue<T> >(skew, min, max, StiffValue<T>(d, defaultValue));
      }else{
	updater = new ExponentialParameterUpdater<T, SmoothStiffValue<T> >(skew, min, max, SmoothStiffValue<T>(l, d, defaultValue));
      }
    }
    parameters[pid] = updater;
  }
  PatchParameter<T> pp(pid);
  return pp;
}

// explicit instantiation
template PatchParameter<float> PatchProcessor::getParameter(const char* name, float min, float max, float defaultValue, float lambda, float delta, float skew);
template PatchParameter<int> PatchProcessor::getParameter(const char* name, int min, int max, int defaultValue, float lambda, float delta, float skew);
  
#if 0
#define SMOOTH_HYSTERESIS
#define SMOOTH_FACTOR 3
void PatchProcessor::setParameterValues(uint16_t *params){
  /* Implements an exponential moving average (leaky integrator) to smooth ADC values
   * y(n) = (1-alpha)*y(n-1) + alpha*y(n)
   * with alpha=0.5, fs=48k, bs=128, then w0 ~= 18hz
   */
  if(getProgramVector()->hardware_version == OWL_MODULAR_HARDWARE){
    for(int i=0; i<NOF_ADC_VALUES; ++i)
#ifdef SMOOTH_HYSTERESIS
      if(abs(params[i]-parameterValues[i]) > 7)
#endif
    {  // invert parameter values for OWL Modular
      if(i<4)
        parameterValues[i] = (parameterValues[i]*SMOOTH_FACTOR + 4095 - params[i])/(SMOOTH_FACTOR+1);
      else
        parameterValues[i] = (parameterValues[i]*SMOOTH_FACTOR + params[i])/(SMOOTH_FACTOR+1);
    }
  }else{
    for(int i=0; i<NOF_ADC_VALUES; ++i)
#ifdef SMOOTH_HYSTERESIS
      if(abs(params[i]-parameterValues[i]) > 7)
#endif
	// 16 = half a midi step (4096/128=32)  
	parameterValues[i] = (parameterValues[i]*SMOOTH_FACTOR + params[i])/(SMOOTH_FACTOR+1);
  }
  // for(int i=NOF_ADC_VALUES; i<NOF_PARAMETERS; ++i)
  //   // todo!
}
#endif

void PatchProcessor::setPatchParameter(int pid, FloatParameter* param){
  if(pid < parameterCount && parameters[pid] != NULL)
    parameters[pid]->setParameter(param);
}

void PatchProcessor::setPatchParameter(int pid, IntParameter* param){
  if(pid < parameterCount && parameters[pid] != NULL)
    parameters[pid]->setParameter(param);
}
