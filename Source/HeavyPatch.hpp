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
#define HV_OWL_PARAM_CTLOUT "__hv_ctlout"
#define HV_OWL_PARAM_BENDOUT "__hv_bendout"
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
    // todo: check if sendHash is same as receiverHash and use that instead of strcmp
    Patch* patch = (Patch*)ctxt->getUserData();
    if(strcmp(receiverName, HV_OWL_PARAM_PUSH) == 0){
      bool pressed;
      if(hv_msg_getNumElements(m) > 0 && hv_msg_isFloat(m, 0))
	pressed = hv_msg_getFloat(m, 0) > 0.5;
      else
	pressed = !isButtonPressed(PUSHBUTTON);
      setButton(PUSHBUTTON, pressed);
    }else if(strcmp(receiverName, HV_OWL_PARAM_NOTEOUT) == 0){      
      uint8_t note = hv_msg_getFloat(m, 0);
      uint8_t velocity = hv_msg_getFloat(m, 1);
      uint8_t ch = hv_msg_getFloat(m, 2);
      patch->sendMidi(MidiMessage::note(ch, note, velocity));
    }else if(strcmp(receiverName, HV_OWL_PARAM_CTLOUT) == 0){
      uint8_t value = hv_msg_getFloat(m, 0);
      uint8_t cc = hv_msg_getFloat(m, 1);
      uint8_t ch = hv_msg_getFloat(m, 2);
      patch->sendMidi(MidiMessage::cc(ch, cc, value));
    }else if(strcmp(receiverName, HV_OWL_PARAM_BENDOUT) == 0){
      uint16_t value = hv_msg_getFloat(m, 0);
      uint8_t ch = hv_msg_getFloat(m, 1);
      patch->sendMidi(MidiMessage::pb(ch, value));
    }
  }
}

class HeavyPatch : public Patch {
private:

  unsigned int receiverHash[9];
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
    context->setUserData(this);
    context->setPrintHook(printHook);
    context->setSendHook(sendHook);
  }
  
  ~HeavyPatch() {
    delete context;
  }

  void processMidi(MidiMessage msg){
    // sendMessageToReceiverV parses format and loops over args, see HeavyContext.cpp
    switch(msg.getStatus()){
    case CONTROL_CHANGE:
      context->sendMessageToReceiverV
	(0x41BE0F9C, // __hv_ctlin
	 0, "fff",
	 (float)msg.getControllerValue(), // value
	 (float)msg.getControllerNumber(), // controller number
	 (float)msg.getChannel());
      break;
    case NOTE_ON:
      context->sendMessageToReceiverV
	(0x67E37CA3, // __hv_notein
	 0, "fff",
	 (float)msg.getNote(), // pitch
	 (float)msg.getVelocity(), // velocity
	 (float)msg.getChannel());
      break;
    case NOTE_OFF:
      context->sendMessageToReceiverV
	(0x67E37CA3, // __hv_notein
	 0, "fff",
	 (float)msg.getNote(), // pitch
	 0.0f, // velocity
	 (float)msg.getChannel());
      break;
    case CHANNEL_PRESSURE:
      context->sendMessageToReceiverV
	(0x553925BD, // __hv_touchin
	 0, "ff",
	 (float)msg.getChannelPressure(),
	 (float)msg.getChannel());
      break;
    case PITCH_BEND_CHANGE:
      context->sendMessageToReceiverV
	(0x3083F0F7, // __hv_bendin
	 0, "ff",
	 (float)msg.getPitchBend(),
	 (float)msg.getChannel());
      break;
    case PROGRAM_CHANGE:
      context->sendMessageToReceiverV
	(0x2E1EA03D, // __hv_pgmin,
	 0, "ff",
	 (float)msg.getProgramChange(),
	 (float)msg.getChannel());
      break;
    }
  }

  void buttonChanged(PatchButtonId bid, uint16_t value, uint16_t samples){
    if(_msgLock)
      return;
    if(bid == PUSHBUTTON){
      context->sendFloatToReceiver(receiverHash[8], value ? 1.0 : 0.0);
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
