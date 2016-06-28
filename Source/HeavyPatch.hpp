#ifndef __HeavyPatch_hpp__
#define __HeavyPatch_hpp__

#include "Patch.h"
#include "basicmaths.h"
#include "Heavy_owl.h"

#define HV_OWL_PARAM_A "Channel-A"
#define HV_OWL_PARAM_B "Channel-B"
#define HV_OWL_PARAM_C "Channel-C"
#define HV_OWL_PARAM_D "Channel-D"
#define HV_OWL_PARAM_E "Channel-E"
#define HV_OWL_PARAM_F "Channel-F"
#define HV_OWL_PARAM_G "Channel-G"
#define HV_OWL_PARAM_H "Channel-H"
#define HV_OWL_PARAM_PUSH "Channel-Push"
#define HEAVY_MESSAGE_POOL_SIZE  4 // in kB (default 10kB)
#define HEAVY_MESSAGE_QUEUE_SIZE 1 // in kB (default 2kB)

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
  unsigned int receiverHash[9];
  HvMessage* notein;
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
    receiverHash[5] = hv_stringToHash(HV_OWL_PARAM_F);
    receiverHash[6] = hv_stringToHash(HV_OWL_PARAM_G);
    receiverHash[7] = hv_stringToHash(HV_OWL_PARAM_H);
    receiverHash[8] = hv_stringToHash(HV_OWL_PARAM_PUSH);
    context = hv_owl_new_with_options(getSampleRate(), 
				      HEAVY_MESSAGE_POOL_SIZE, 
				      HEAVY_MESSAGE_QUEUE_SIZE);
    hv_setPrintHook(context, &printHook);
    hv_setSendHook(context, sendHook);

    // create note in message
    notein = (HvMessage*)malloc(hv_msg_getByteSize(5));
    hv_msg_init(notein, 5, 0.0);
    hv_msg_setFloat(notein, 0, 60.0); // note
    hv_msg_setFloat(notein, 1, 80.0); // velocity
    hv_msg_setFloat(notein, 2, 1.0f); // channel
    hv_msg_setFloat(notein, 3, 0x90); // command
    hv_msg_setFloat(notein, 4, 0.0f); // port
  }
  
  ~HeavyPatch() {
    hv_owl_free(context);
    free(notein);
  }
  
  void buttonChanged(PatchButtonId bid, uint16_t value, uint16_t samples){
    if(bid == PUSHBUTTON){
      hv_sendFloatToReceiver(context, receiverHash[8], value ? 1.0 : 0.0);
    }else if(bid >= MIDI_NOTE_BUTTON){
      // send message to notein object
      unsigned int hash = 0x67E37CA3; // __hv_notein
      float note = (float)(bid - MIDI_NOTE_BUTTON);
      float velocity = (float)(value>>5);
      // unsigned int hash = 0x41BE0F9C; // __hv_ctlin
      float ms = 1000.0f*(samples+getBlockSize())/getSampleRate(); // delay in milliseconds
      // float cmd = value ? 0x90 : 0x80;
      hv_msg_setFloat(notein, 0, note);
      hv_msg_setFloat(notein, 1, velocity);
      // notein expects: note, velocity, channel, command, port
      // not thread safe
      hv_scheduleMessageForReceiver(context, hash, ms, notein);
    }
  }

  void processAudio(AudioBuffer &buffer) {
    float paramA = getParameterValue(PARAMETER_A);
    float paramB = getParameterValue(PARAMETER_B);
    float paramC = getParameterValue(PARAMETER_C);
    float paramD = getParameterValue(PARAMETER_D);
    float paramE = getParameterValue(PARAMETER_E);
    float paramF = getParameterValue(PARAMETER_F);
    float paramG = getParameterValue(PARAMETER_G);
    float paramH = getParameterValue(PARAMETER_H);
    hv_sendFloatToReceiver(context, receiverHash[0], paramA);
    hv_sendFloatToReceiver(context, receiverHash[1], paramB);
    hv_sendFloatToReceiver(context, receiverHash[2], paramC);
    hv_sendFloatToReceiver(context, receiverHash[3], paramD);
    hv_sendFloatToReceiver(context, receiverHash[4], paramE);
    hv_sendFloatToReceiver(context, receiverHash[5], paramF);
    hv_sendFloatToReceiver(context, receiverHash[6], paramG);
    hv_sendFloatToReceiver(context, receiverHash[7], paramH);
    float* outputs[] = {buffer.getSamples(LEFT_CHANNEL), buffer.getSamples(RIGHT_CHANNEL) };    
    hv_owl_process(context, outputs, outputs, getBlockSize());		     
  }
  
private:
  Hv_owl *context;
};

#endif // __HeavyPatch_hpp__
