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

class GenPatch : public Patch {
private:
  int8_t parameterindices[GEN_OWL_PARAM_COUNT];
  int8_t buttonindex;
  CommonState *context;
  int numParams;
public:
  GenPatch() {
    memset(parameterindices, -1, GEN_OWL_PARAM_COUNT);
    buttonindex = -1;
    context = (CommonState *)gen::create(getSampleRate(), 0);
    numParams = gen::num_params();
    for(int i = 0; i < numParams; i++){
      if(strcmp(GEN_OWL_PARAM_A, gen::getparametername(context, i)) == 0)
	parameterindices[0] = i;
      else if(strcmp(GEN_OWL_PARAM_B, gen::getparametername(context, i)) == 0)
	parameterindices[1] = i;
      else if(strcmp(GEN_OWL_PARAM_C, gen::getparametername(context, i)) == 0)
	parameterindices[2] = i;
      else if(strcmp(GEN_OWL_PARAM_D, gen::getparametername(context, i)) == 0)
	parameterindices[3] = i;
      else if(strcmp(GEN_OWL_PARAM_E, gen::getparametername(context, i)) == 0)
	parameterindices[4] = i;
      else if(strcmp(GEN_OWL_PARAM_F, gen::getparametername(context, i)) == 0)
	parameterindices[5] = i;
      else if(strcmp(GEN_OWL_PARAM_G, gen::getparametername(context, i)) == 0)
	parameterindices[6] = i;
      else if(strcmp(GEN_OWL_PARAM_H, gen::getparametername(context, i)) == 0)
	parameterindices[7] = i;
      else if(strcmp(GEN_OWL_PARAM_PUSH, gen::getparametername(context, i)) == 0)
	buttonindex = i;
    }
    for(int i=0; i<GEN_OWL_PARAM_COUNT; ++i){
      if(parameterindices[i] != -1){
	registerParameter((PatchParameterId)i, gen::getparametername(context, parameterindices[i]));
      }
    }
  }

  ~GenPatch() {
    gen::destroy(context);
  }

  t_param scaleParameter(CommonState *context, int index, t_param value)
  {
    if(gen::getparameterhasminmax(context, index) == 0)
      return value;
    t_param min = gen::getparametermin(context, index);
    t_param max = gen::getparametermax(context, index);
    return value * (max-min) + min;
  }

  void buttonChanged(PatchButtonId bid, uint16_t value, uint16_t samples){
    if(bid == PUSHBUTTON)
      if(buttonindex != -1)
	gen::setparameter(context, buttonindex, scaleParameter(context, buttonindex, value), NULL);
  }

  void processAudio(AudioBuffer &buffer) {
    for(int i=0; i<GEN_OWL_PARAM_COUNT; ++i){
      int8_t index = parameterindices[i];
      if(index != -1)
	gen::setparameter(context, index, scaleParameter(context, index, getParameterValue((PatchParameterId)i)), NULL);
    }

    float* outputs[] = {buffer.getSamples(0), buffer.getSamples(1) };
    gen::perform(context, outputs, 2, outputs, 2, buffer.getSize());
  }
};

#endif // __GenPatch_hpp__
