#include "PatchProcessor.h"
#include "MemoryBuffer.hpp"
#include "device.h"
#include "main.h"
#include <string.h>
#include "ProgramVector.h"
#include "SmoothValue.h"

PatchProcessor::PatchProcessor() 
  : patch(NULL), bufferCount(0), parameterCount(0) {
  for(int i=0; i<MAX_NUMBER_OF_PARAMETERS; ++i)
    parameters[i] = NULL;
}

PatchProcessor::~PatchProcessor(){
  clear();
}

void PatchProcessor::clear(){
  for(int i=0; i<bufferCount; ++i){
    delete buffers[i];
    buffers[i] = NULL;
  }
  bufferCount = 0;
  for(int i=0; i<parameterCount; ++i){
    delete parameters[i];
    parameters[i] = NULL;
  }
  parameterCount = 0;
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
  if(getProgramVector()->hardware_version == OWL_MODULAR_HARDWARE){
    for(int i=0; i<4 && i<parameterCount; ++i)
      parameters[i]->update(4095 - params[i]);
    for(int i=4; i<parameterCount; ++i)
      parameters[i]->update(params[i]);
  }else{
    for(int i=0; i<parameterCount; ++i)
      parameters[i]->update(params[i]);
  }
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

void PatchProcessor::setDefaultValue(int pid, float value){
  doSetPatchParameter(pid, value*4095);
}

void PatchProcessor::setDefaultValue(int pid, int value){
  doSetPatchParameter(pid, value);
}

template<typename T>
PatchParameter<T> PatchProcessor::getParameter(const char* name, T min, T max, T defaultValue, float lambda, float delta, float skew){
  int pid = 0;
  int blocksize = getBlockSize();
  if(parameterCount < MAX_NUMBER_OF_PARAMETERS && 
     parameterCount < getProgramVector()->parameters_size){
    pid = parameterCount++;
    if(getProgramVector()->registerPatchParameter != NULL)
      getProgramVector()->registerPatchParameter(pid, name);
    setDefaultValue(pid, defaultValue);
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

void PatchProcessor::setPatchParameter(int pid, FloatParameter* param){
  if(pid < parameterCount && parameters[pid] != NULL)
    parameters[pid]->setParameter(param);
}

void PatchProcessor::setPatchParameter(int pid, IntParameter* param){
  if(pid < parameterCount && parameters[pid] != NULL)
    parameters[pid]->setParameter(param);
}
