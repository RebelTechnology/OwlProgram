#ifndef __HeavyPatch_hpp__
#define __HeavyPatch_hpp__

#include "StompBox.h"
#include "basicmaths.h"
#include "Heavy_owl.h"

#define HV_OWL_PARAM_A "Channel-A"
#define HV_OWL_PARAM_B "Channel-B"
#define HV_OWL_PARAM_C "Channel-C"
#define HV_OWL_PARAM_D "Channel-D"
#define HV_OWL_PARAM_E "Channel-E"
#define HV_OWL_PARAM_PUSH "Channel-Push"

extern "C" {
  static bool isButtonPressed(PatchButtonId bid){
    return getProgramVector()->buttons & (1<<bid);
  }
  static void setButton(PatchButtonId bid, bool pressed){
    if(pressed)
      getProgramVector()->buttons |= 1<<bid;
    else
      getProgramVector()->buttons &= ~(1<<bid);
  }
  static void printHook(double timestampMs, const char *printLabel, const char *msgString, void *userData) {
    char buf[64];
    char* dst = buf;
    int len = strnlen(printLabel, 48);
    dst = stpncpy(dst, printLabel, 63);
    dst = stpcpy(dst, " ");
    dst = stpncpy(dst, msgString, 63-len);
    debugMessage(buf);
  }
  static void sendHook(double timestampMs, // in milliseconds
		       const char *receiverName,
		       const HvMessage *const m,
		       void *userData) {
    if(strcmp(receiverName, HV_OWL_PARAM_PUSH) == 0){
      bool pressed;
      if(hv_msg_getNumElements(m) > 0 && hv_msg_isFloat(m, 0))
	pressed = hv_msg_getFloat(m, 0) > 0.5;
      else
	pressed = !isButtonPressed(PUSHBUTTON);
      setButton(PUSHBUTTON, pressed);
    }
  }
}

class HeavyPatch : public Patch {
private:
  bool pushbutton;
  unsigned int receiverHash[6];

public:
  HeavyPatch() {
    registerParameter(PARAMETER_E, HV_OWL_PARAM_A);
    registerParameter(PARAMETER_E, HV_OWL_PARAM_B);
    registerParameter(PARAMETER_E, HV_OWL_PARAM_C);
    registerParameter(PARAMETER_E, HV_OWL_PARAM_D);
    registerParameter(PARAMETER_E, HV_OWL_PARAM_E);
    receiverHash[0] = hv_stringToHash(HV_OWL_PARAM_A);
    receiverHash[1] = hv_stringToHash(HV_OWL_PARAM_B);
    receiverHash[2] = hv_stringToHash(HV_OWL_PARAM_C);
    receiverHash[3] = hv_stringToHash(HV_OWL_PARAM_D);
    receiverHash[4] = hv_stringToHash(HV_OWL_PARAM_E);
    receiverHash[5] = hv_stringToHash(HV_OWL_PARAM_PUSH);
    context = hv_owl_new(getSampleRate());
    hv_setPrintHook(context, &printHook);
    hv_setSendHook(context, sendHook);
  }
  
  ~HeavyPatch() {
    hv_owl_free(context);
  }
  
  void processAudio(AudioBuffer &buffer) {
    float paramA = getParameterValue(PARAMETER_A);
    float paramB = getParameterValue(PARAMETER_B);
    float paramC = getParameterValue(PARAMETER_C);
    float paramD = getParameterValue(PARAMETER_D);
    float paramE = getParameterValue(PARAMETER_E);
    if(isButtonPressed(PUSHBUTTON) != pushbutton){
      pushbutton = isButtonPressed(PUSHBUTTON);
      hv_sendFloatToReceiver(context, receiverHash[5], pushbutton ? 1.0 : 0.0);
    }
    hv_sendFloatToReceiver(context, receiverHash[0], paramA);
    hv_sendFloatToReceiver(context, receiverHash[1], paramB);
    hv_sendFloatToReceiver(context, receiverHash[2], paramC);
    hv_sendFloatToReceiver(context, receiverHash[3], paramD);
    hv_sendFloatToReceiver(context, receiverHash[4], paramE);
    float* outputs[] = {buffer.getSamples(LEFT_CHANNEL), buffer.getSamples(RIGHT_CHANNEL) };    
    hv_owl_process(context, outputs, outputs, getBlockSize());		     
  }
  
private:
  Hv_owl *context;
};

#endif // __HeavyPatch_hpp__
