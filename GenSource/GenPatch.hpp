#ifndef __GenPatch_hpp__
#define __GenPatch_hpp__

#include "Patch.h"
#include "gen.h"

#define GEN_OWL_PARAM_A "A"
#define GEN_OWL_PARAM_B "B"
#define GEN_OWL_PARAM_C "C"
#define GEN_OWL_PARAM_D "D"
#define GEN_OWL_PARAM_E "E"
#define GEN_OWL_PARAM_F "F"
#define GEN_OWL_PARAM_G "G"
#define GEN_OWL_PARAM_H "H"
#define GEN_OWL_PARAM_PUSH "Push"
#define GEN_OWL_PARAM_COUNT 8

class GenParameter {
public:
  PatchParameterId id;
  float min = 0.0;
  float max = 1.0;
  uint8_t index = -1;
  void configure(CommonState *context, PatchParameterId pid, const char* name){
    id = pid;
    int numParams = gen::num_params();
    for(int i = 0; i < numParams; i++){
      if(strcmp(name, gen::getparametername(context, i)) == 0){
	index = i;
	if(gen::getparameterhasminmax(context, index)){
	  min = gen::getparametermin(context, index);
	  max = gen::getparametermax(context, index);
	}
	break;
      }
    }
  }
  void update(CommonState *context, float value){
    if(index != -1){
      value = value * (max-min) + min;
      gen::setparameter(context, index, value, NULL);
    }
  }
};

class GenButton {
public:
  PatchButtonId id;
  float min = 0.0;
  float max = 1.0;
  uint8_t index = -1;
  void configure(CommonState *context, PatchButtonId bid, const char* name){
    id = bid;
    int numParams = gen::num_params();
    for(int i = 0; i < numParams; i++){
      if(strcmp(name, gen::getparametername(context, i)) == 0){
	index = i;
	if(gen::getparameterhasminmax(context, index)){
	  min = gen::getparametermin(context, index);
	  max = gen::getparametermax(context, index);
	}
	break;
      }
    }
  }
  void update(CommonState *context, float value){
    if(index != -1){
      value = value * (max-min) + min;
      gen::setparameter(context, index, value, NULL);
    }
  }
};

class GenPatch : public Patch {
private:
  CommonState *context;
  GenParameter params[GEN_OWL_PARAM_COUNT];
  GenButton pushbutton;
public:
  GenPatch() {
    context = (CommonState *)gen::create(getSampleRate(), 0);
    params[0].configure(context, PARAMETER_A, GEN_OWL_PARAM_A);
    params[1].configure(context, PARAMETER_B, GEN_OWL_PARAM_B);
    params[2].configure(context, PARAMETER_C, GEN_OWL_PARAM_C);
    params[3].configure(context, PARAMETER_D, GEN_OWL_PARAM_D);
    params[4].configure(context, PARAMETER_E, GEN_OWL_PARAM_E);
    params[5].configure(context, PARAMETER_F, GEN_OWL_PARAM_F);
    params[6].configure(context, PARAMETER_G, GEN_OWL_PARAM_G);
    params[7].configure(context, PARAMETER_H, GEN_OWL_PARAM_H);
    for(int i=0; i<GEN_OWL_PARAM_COUNT; ++i){
      if(params[i].index != -1)
	registerParameter(params[i].id, gen::getparametername(context, params[i].index));
    }
    pushbutton.configure(context, PUSHBUTTON, GEN_OWL_PARAM_PUSH);
  }

  ~GenPatch() {
    gen::destroy(context);
  }

  void buttonChanged(PatchButtonId bid, uint16_t value, uint16_t samples){
    if(bid == PUSHBUTTON)
      pushbutton.update(context, value/4095.0f);
  }

  void processAudio(AudioBuffer &buffer) {
    for(int i=0; i<GEN_OWL_PARAM_COUNT; ++i)
      params[i].update(context, getParameterValue((PatchParameterId)(PARAMETER_A+i)));
    float* outputs[] = {buffer.getSamples(0), buffer.getSamples(1) };
    gen::perform(context, outputs, 2, outputs, 2, buffer.getSize());
  }
};

#endif // __GenPatch_hpp__
