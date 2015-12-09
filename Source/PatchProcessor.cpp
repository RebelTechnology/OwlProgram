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

float PatchProcessor::getParameterValue(PatchParameterId pid){
  if(pid < NOF_ADC_VALUES && parameters[pid] != NULL)
    return parameters[pid]->getValue();
  else
    return 0.0f;
}

void PatchProcessor::setParameterValues(uint16_t *params){
  for(int i=0; i<parameterCount; ++i)
    parameters[i]->update(params[i]);
}

template<typename T, typename V>
class TemplateParameterUpdater : public ParameterUpdater {
private:
  PatchParameter<T>* parameter;
  T minimum;
  T maximum;
  V value;
public:
  TemplateParameterUpdater(T min, T max, V initialValue)
    : parameter(NULL), minimum(min), maximum(max), value(initialValue) {}
  void update(uint16_t newValue){
    value = (newValue*(maximum-minimum)+minimum)/4095;
    if(parameter != NULL)
      parameter->update((T)value);
  }
  void setParameter(PatchParameter<T>* p){
    parameter = p;
  }
};

template<typename T>
PatchParameter<T> PatchProcessor::getParameter(const char* name, T min, T max, T defaultValue, PatchParameterScale scale, float lambda, float delta){
  // todo: lambda and delta: if non-zero, smooth/stiff
  int pid = 0;
  if(parameterCount < 5){
    pid = parameterCount++;
    if(getProgramVector()->registerPatchParameter != NULL)
      getProgramVector()->registerPatchParameter(pid, name);
    if(parameters[pid] != NULL)
      delete parameters[pid];
    ParameterUpdater* updater = NULL;
    if(lambda == 0.0 && delta == 0.0){
      updater = new TemplateParameterUpdater<T, T>(min, max, defaultValue);
    }else if(lambda == 0.0){      
      updater = new TemplateParameterUpdater<T, StiffValue<T>>(min, max, StiffValue<T>(delta, defaultValue));
    }else if(delta == 0.0){
      updater = new TemplateParameterUpdater<T, SmoothValue<T>>(min, max, SmoothValue<T>(lambda, defaultValue));
    }else{
      updater = new TemplateParameterUpdater<T, SmoothStiffValue<T>>(min, max, SmoothStiffValue<T>(lambda, delta, defaultValue));
    }
    parameters[pid] = updater;
  }
  PatchParameter<T> pp(pid);
  return pp;
}

// explicit instantiation
template PatchParameter<float> PatchProcessor::getParameter(const char* name, float min, float max, float defaultValue, PatchParameterScale scale, float lambda, float delta);
template PatchParameter<int> PatchProcessor::getParameter(const char* name, int min, int max, int defaultValue, PatchParameterScale scale, float lambda, float delta);
template class TemplateParameterUpdater<int, int>;
template class TemplateParameterUpdater<float, float>;
template class TemplateParameterUpdater<int, SmoothValue<int>>;
template class TemplateParameterUpdater<float, SmoothValue<float>>;
template class TemplateParameterUpdater<int, StiffValue<int>>;
template class TemplateParameterUpdater<float, StiffValue<float>>;
template class TemplateParameterUpdater<int, SmoothStiffValue<int>>;
template class TemplateParameterUpdater<float, SmoothStiffValue<float>>;
  
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
        parameterValues[i] = (parameterValues[i]*SMOOTH_FACTOR - params[i])/(SMOOTH_FACTOR+1);
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
