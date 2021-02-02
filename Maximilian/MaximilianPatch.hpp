#ifndef __MaximilianPatch_hpp__
#define __MaximilianPatch_hpp__

#include "Patch.h"
#include "maximilian.h"

#define MAX_MAXIPARAMS 20
static maxiParam* maxiParameters[MAX_MAXIPARAMS];
static unsigned int maxiParameterCount = 0;
maxiParam::maxiParam() : value(0.5), minValue(0.0), maxValue(1.0),
			 name(NULL), pid(maxiParameterCount){
  if(maxiParameterCount < MAX_MAXIPARAMS)
    maxiParameters[maxiParameterCount++] = this;
}

void maxiParam::update(double v){
#ifdef ARM_CORTEX
  const static double lambda = 0.9; // factor for exponential smoothing
#else
  const static double lambda = 0.5; // factor for exponential smoothing
#endif
  value = value*lambda + scale(v)*(1-lambda);
}

class MaximilianPatch : public Patch {
private:
#include "maximilian-patch.h"
  float output[2];
public:
  MaximilianPatch(){
    maxiSettings::sampleRate = getSampleRate();
    maxiSettings::channels = 2;
    maxiSettings::bufferSize = getBlockSize();
    setup();
    char name[] = "A";
    for(size_t i=0; i<maxiParameterCount; ++i){
      if(maxiParameters[i]->name == NULL){
	name[0] = 'A'+maxiParameters[i]->pid;
	registerParameter((PatchParameterId)(maxiParameters[i]->pid), name);
      }else{
	registerParameter((PatchParameterId)(maxiParameters[i]->pid), maxiParameters[i]->name);
      }
      float min = maxiParameters[i]->minValue;
      float max = maxiParameters[i]->maxValue;
      float value = (maxiParameters[i]->value - min)/(max-min);
      setParameterValue((PatchParameterId)(maxiParameters[i]->pid), value);
    }
  }
  void processAudio(AudioBuffer &buffer) {
    float* left = buffer.getSamples(LEFT_CHANNEL);
    float* right = buffer.getSamples(RIGHT_CHANNEL);
    for(size_t i=0; i<maxiParameterCount; ++i)
      maxiParameters[i]->update(getParameterValue((PatchParameterId)(maxiParameters[i]->pid)));
    for(size_t i=0; i<buffer.getSize(); ++i){
      play(output);
      left[i] = output[0];
      right[i] = output[1];
    }
  }
};

#endif   // __MaximilianPatch_hpp__
