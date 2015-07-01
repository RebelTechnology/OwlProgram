#ifndef __HeavyPatch_hpp__
#define __HeavyPatch_hpp__

#include "StompBox.h"
#include "Heavy_owl.h"

#define HEAVY_CHANNELS 2

class HeavyPatch : public Patch {
public:
  HeavyPatch() {
    registerParameter(PARAMETER_A, "Channel-A");
    registerParameter(PARAMETER_B, "Channel-B");
    registerParameter(PARAMETER_C, "Channel-C");
    registerParameter(PARAMETER_D, "Channel-D");
    
    context = hv_owl_new(getSampleRate());
  }
  
  ~HeavyPatch() {
    hv_owl_free(context);
  }
  
  void processAudio(AudioBuffer &buffer) {
    float paramA = getParameterValue(PARAMETER_A);
    float paramB = getParameterValue(PARAMETER_B);
    float paramC = getParameterValue(PARAMETER_C);
    float paramD = getParameterValue(PARAMETER_D);
    
    // Note: The 0.0 parameter is the timestamp at which to execute the message,
    // but in this case it simply means to execute it immediately. "f" says that
    // the message contains one element and its type is float. paramA is then the
    // value.
    hv_vscheduleMessageForReceiver(context, "Channel-A", 0.0, "f", paramA);
    hv_vscheduleMessageForReceiver(context, "Channel-B", 0.0, "f", paramB);
    hv_vscheduleMessageForReceiver(context, "Channel-C", 0.0, "f", paramC);
    hv_vscheduleMessageForReceiver(context, "Channel-D", 0.0, "f", paramD);

    // int nbSples = buffer.getSize()*buffer.getChannels();
    // int nbSples = buffer.getSize()*HEAVY_CHANNELS;
    // float* inputCopy = (float*)malloc(nbSples*sizeof(float));
    // memcpy(inputCopy, buffer.getSamples(0), nbSples*sizeof(float));

    // float** inputs = { &inputCopy, &inputCopy+getBlockSize()};
    float* outputs[] = {buffer.getSamples(0), buffer.getSamples(1) };
    
    hv_owl_process(context, outputs, outputs, getBlockSize());		     
  }
  
private:
  Hv_owl *context;
};

#endif // __HeavyPatch_hpp__
