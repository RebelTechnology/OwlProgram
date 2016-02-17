#ifndef __HeavyPatch_hpp__
#define __HeavyPatch_hpp__

#include "StompBox.h"
#include "Heavy_owl.h"

extern "C" {
  static bool isButtonPressed(PatchButtonId bid){
    getProgramVector()->buttons & (1<<bid);
  }
  static void setButton(PatchButtonId bid, bool pressed){
    if(pressed)
      getProgramVector()->buttons |= 1<<bid;
    else
      getProgramVector()->buttons &= ~(1<<bid);
  }
  static void printHook(double timestampMs, const char *printLabel, const char *msgString, void *userData) {
    // printf("[@ %.3fms] %s: %s\n", timestampMs, printLabel, msgString);
    char buf[64];
    char* p = buf;
    int len = strnlen(printLabel, 48);
    stpncpy(buf, printLabel, 63);
    stpncpy(buf, msgString, 63-len);
    debugMessage(buf);
  }
  static void sendHook(double timestampMs, // in milliseconds
		       const char *receiverName,
		       const HvMessage *const m,
		       void *userData) {
    // if(strcmp(receiverName, "Channel-Buttons") == 0){
    //   // first 
    //   int count = hv_msg_getNumElements(m);
    //   float value;
    //   PatchButtonId bid;
    //   // hv_msg_getType(m, 0);
    //   if(count > 0)
    // 	value = hv_msg_getFloat(m, 0);
    //   if(led > 0.75)
    // 	bid = GREEN_BUTTON;
    //   else if(led > 0.25)
    // 	bid = RED_BUTTON;
    //   else
    // 	bid = PUSHBUTTON;
    //   bool pressed;
    //   if(count > 1)
    // 	value = hv_msg_getFloat(m, 1) > 0.5;
    //   else // else toggle
    // 	pressed = !isButtonPressed(bid);
    //   setButton(bid, pressed);
    // }else 
      if(strcmp(receiverName, "Channel-Push") == 0){
      bool pressed;
      if(hv_msg_getNumElements(m) > 0)
	pressed = hv_msg_isBang(m, 0) || hv_msg_getFloat(m, 0) > 0.5;
      else
	pressed = !isButtonPressed(PUSHBUTTON);
      setButton(PUSHBUTTON, pressed);
    }
  }
}

class HeavyPatch : public Patch {
private:
  bool pushbutton;
public:
  HeavyPatch() {
    registerParameter(PARAMETER_A, "Channel-A");
    registerParameter(PARAMETER_B, "Channel-B");
    registerParameter(PARAMETER_C, "Channel-C");
    registerParameter(PARAMETER_D, "Channel-D");
    registerParameter(PARAMETER_E, "Channel-E");    
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
      hv_vscheduleMessageForReceiver(context, "Channel-Push", 0.0, "f", pushbutton ? 1.0 : 0.0);
    }
    // Note: The third parameter is the timestamp at which to execute the message,
    // but in this case it simply means to execute it immediately. "f" says that
    // the message contains one element and its type is float. paramA is then the
    // value.
    hv_vscheduleMessageForReceiver(context, "Channel-A", 0.0, "f", paramA);
    hv_vscheduleMessageForReceiver(context, "Channel-B", 0.0, "f", paramB);
    hv_vscheduleMessageForReceiver(context, "Channel-C", 0.0, "f", paramC);
    hv_vscheduleMessageForReceiver(context, "Channel-D", 0.0, "f", paramD);
    hv_vscheduleMessageForReceiver(context, "Channel-E", 0.0, "f", paramE);
    float* outputs[] = {buffer.getSamples(0), buffer.getSamples(1) };    
    hv_owl_process(context, outputs, outputs, getBlockSize());		     
  }
  
private:
  Hv_owl *context;
};

#endif // __HeavyPatch_hpp__
