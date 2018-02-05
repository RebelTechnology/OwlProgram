#ifndef __HeavyPatch_hpp__
#define __HeavyPatch_hpp__

#include "Patch.h"
#include "basicmaths.h"
#include "Heavy_owl.hpp"

#define HV_OWL_PARAM_A "Channel-A"
#define HV_OWL_PARAM_B "Channel-B"
#define HV_OWL_PARAM_C "Channel-C"
#define HV_OWL_PARAM_D "Channel-D"
#define HV_OWL_PARAM_E "Channel-E"
#define HV_OWL_PARAM_F "Channel-F"
#define HV_OWL_PARAM_G "Channel-G"
#define HV_OWL_PARAM_H "Channel-H"
#define HV_OWL_PARAM_PUSH "Channel-Push"
#define HV_OWL_PARAM_NOTEOUT "__hv_noteout"
#define HEAVY_MESSAGE_POOL_SIZE  4 // in kB (default 10kB)
#define HEAVY_MESSAGE_IN_QUEUE_SIZE 1 // in kB (default 2kB)
#define HEAVY_MESSAGE_OUT_QUEUE_SIZE 0 // in kB (default 0kB)

extern "C" {
  volatile bool _msgLock = false;
  static bool isButtonPressed(PatchButtonId bid){
    return getProgramVector()->buttons & (1<<bid);
  }
  static void setButton(PatchButtonId bid, bool pressed){
    if(pressed)
      getProgramVector()->buttons |= 1<<bid;
    else
      getProgramVector()->buttons &= ~(1<<bid);
  }
  static void printHook(HeavyContextInterface* ctxt, 
			const char *printLabel, 
			const char *msgString, 
			const HvMessage *m) {
    char buf[64];
    char* dst = buf;
    int len = strnlen(printLabel, 48);
    dst = stpncpy(dst, printLabel, 63);
    dst = stpcpy(dst, " ");
    dst = stpncpy(dst, msgString, 63-len);
    debugMessage(buf);
  }
  static void sendHook(HeavyContextInterface* ctxt,
		       const char *receiverName,
		       uint32_t sendHash,
		       const HvMessage *m){
    if(strcmp(receiverName, HV_OWL_PARAM_PUSH) == 0){
      bool pressed;
      if(hv_msg_getNumElements(m) > 0 && hv_msg_isFloat(m, 0))
	pressed = hv_msg_getFloat(m, 0) > 0.5;
      else
	pressed = !isButtonPressed(PUSHBUTTON);
      setButton(PUSHBUTTON, pressed);
    }else if(strcmp(receiverName, HV_OWL_PARAM_NOTEOUT) == 0){
      uint8_t note = hv_msg_getFloat(m, 0)*128;
      uint16_t velocity = hv_msg_getFloat(m, 1)*4096;
      setButton((PatchButtonId)(MIDI_NOTE_BUTTON+note), velocity);
    }
  }
}

class HeavyPatch : public Patch {
private:

  unsigned int receiverHash[9];
  HvMessage* notein;
public:
  HeavyPatch() {
    registerParameter(PARAMETER_A, HV_OWL_PARAM_A);
    registerParameter(PARAMETER_B, HV_OWL_PARAM_B);
    registerParameter(PARAMETER_C, HV_OWL_PARAM_C);
    registerParameter(PARAMETER_D, HV_OWL_PARAM_D);
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
    context = new Heavy_owl(getSampleRate(), 
			    HEAVY_MESSAGE_POOL_SIZE, 
			    HEAVY_MESSAGE_IN_QUEUE_SIZE, 
			    HEAVY_MESSAGE_OUT_QUEUE_SIZE);
    context->setPrintHook(&printHook);
    context->setSendHook(sendHook);

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
    delete context;
    free(notein);
  }
  
  void buttonChanged(PatchButtonId bid, uint16_t value, uint16_t samples){
    if(_msgLock)
      return;
    if(bid == PUSHBUTTON){
      context->sendFloatToReceiver(receiverHash[8], value ? 1.0 : 0.0);
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
      context->sendMessageToReceiver(hash, ms, notein);
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
    _msgLock = true;
    context->sendFloatToReceiver(receiverHash[0], paramA);
    context->sendFloatToReceiver(receiverHash[1], paramB);
    context->sendFloatToReceiver(receiverHash[2], paramC);
    context->sendFloatToReceiver(receiverHash[3], paramD);
    context->sendFloatToReceiver(receiverHash[4], paramE);
    context->sendFloatToReceiver(receiverHash[5], paramF);
    context->sendFloatToReceiver(receiverHash[6], paramG);
    context->sendFloatToReceiver(receiverHash[7], paramH);
    _msgLock = false;
    float* outputs[] = {buffer.getSamples(LEFT_CHANNEL), buffer.getSamples(RIGHT_CHANNEL)};
    context->process(outputs, outputs, getBlockSize());
  }
  
private:
  HeavyContext* context;
};

#endif // __HeavyPatch_hpp__
