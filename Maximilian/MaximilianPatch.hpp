#ifndef __MaximilianPatch_hpp__
#define __MaximilianPatch_hpp__

#include "Patch.h"
#include "maximilian.h"

static maxiParam* maxiParameters[8];
static unsigned int maxiParameterCount = 0;
maxiParam::maxiParam() : value(0.0), minValue(0.0), maxValue(1.0),
			 name(NULL), pid(maxiParameterCount++){
  if(pid < 8)
    maxiParameters[pid] = this;
}

void maxiParam::update(double v){
  const static double lambda = 0.9; // factor for exponential smoothing
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
    maxiParameterCount = std::min(7u, maxiParameterCount);
    for(int i=0; i<maxiParameterCount; ++i){
      if(maxiParameters[i]->name == NULL){
	name[0] = 'A'+maxiParameters[i]->pid;
	registerParameter((PatchParameterId)(maxiParameters[i]->pid), name);
      }else{
	registerParameter((PatchParameterId)(maxiParameters[i]->pid), maxiParameters[i]->name);
      }
    }
  }
  void processAudio(AudioBuffer &buffer) {
    float* left = buffer.getSamples(LEFT_CHANNEL);
    float* right = buffer.getSamples(RIGHT_CHANNEL);
    for(int i=0; i<maxiParameterCount; ++i)
      maxiParameters[i]->update(getParameterValue((PatchParameterId)(maxiParameters[i]->pid)));
    for(int i=0; i<buffer.getSize(); ++i){
      play(output);
      left[i] = output[0];
      right[i] = output[1];
    }
  }
};

#endif   // __MaximilianPatch_hpp__
