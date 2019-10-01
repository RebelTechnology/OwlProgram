#ifndef __HeavyPatch_hpp__
#define __HeavyPatch_hpp__

#include "Patch.h"
#include "basicmaths.h"
#include "HvHeavy.h"
#include "Heavy_owl.hpp"
#include "Heavy_owl_constants.h"

#define HV_HASH_CHANNEL_PUSH 0x3cf4c9df
/*
#define HV_HASH_CHANNEL_A 0xc440c54f //Channel-A
//#define HV_HASH_CHANNEL_A 0x7bcbaf78 //ChannelA
#define HV_HASH_CHANNEL_B 0xb762bb42
#define HV_HASH_CHANNEL_C 0x27d89cd5
#define HV_HASH_CHANNEL_D 0x217d22f5
#define HV_HASH_CHANNEL_E 0x7746db0a
#define HV_HASH_CHANNEL_F 0xd3c05ccb
#define HV_HASH_CHANNEL_G 0xba16b531
#define HV_HASH_CHANNEL_H 0xfbc0c5a
*/
#define HV_HASH_BUTTON_A 0xfbc73385
#define HV_HASH_BUTTON_B 0x8c39a047
#define HV_HASH_BUTTON_C 0x69b62624
#define HV_HASH_BUTTON_D 0x55d2d3a6
#define HV_HASH_BUTTON_E 0x1444edb
#define HV_HASH_BUTTON_F 0xe8f2587f

#define HV_HASH_TR_F 0xc3bc4805
#define HV_HASH_CV_F 0xf8424b3c
#define HV_HASH_CV_G 0xf1c46a96

#define HV_HASH_NOTEIN 0x67e37ca3
#define HV_HASH_CTLIN 0x41be0f9c
#define HV_HASH_BENDIN 0x3083f0f7
#define HV_HASH_TOUCHIN 0x553925bd
#define HV_HASH_PGMIN 0x2e1ea03d

#define HV_HASH_NOTEOUT 0xd1d4ac2
#define HV_HASH_CTLOUT 0xe5e2a040
#define HV_HASH_BENDOUT 0xe8458013
#define HV_HASH_TOUCHOUT 0x476d4387
#define HV_HASH_PGMOUT 0x8753e39e

#define HV_EXTENDED_PARAMETERS
#define HV_OWL_PARAM_A "Channel-A"
#define HV_OWL_PARAM_B "Channel-B"
#define HV_OWL_PARAM_C "Channel-C"
#define HV_OWL_PARAM_D "Channel-D"
#define HV_OWL_PARAM_E "Channel-E"
#define HV_OWL_PARAM_F "Channel-F"
#define HV_OWL_PARAM_G "Channel-G"
#define HV_OWL_PARAM_H "Channel-H"

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
		       const HvMessage *m);
}

class HeavyPatch : public Patch {
public:
  HeavyPatch() {
//     registerParameter(PARAMETER_A, HV_OWL_PARAM_A);
//     registerParameter(PARAMETER_B, HV_OWL_PARAM_B);
//     registerParameter(PARAMETER_C, HV_OWL_PARAM_C);
//     registerParameter(PARAMETER_D, HV_OWL_PARAM_D);
//     registerParameter(PARAMETER_E, HV_OWL_PARAM_E);
// #ifdef HV_EXTENDED_PARAMETERS
//     registerParameter(PARAMETER_F, HV_OWL_PARAM_F);
//     registerParameter(PARAMETER_G, HV_OWL_PARAM_G);
//     registerParameter(PARAMETER_H, HV_OWL_PARAM_H);
// #endif
    context = new Heavy_owl(getSampleRate(), 
			    HEAVY_MESSAGE_POOL_SIZE, 
			    HEAVY_MESSAGE_IN_QUEUE_SIZE, 
			    HEAVY_MESSAGE_OUT_QUEUE_SIZE);
    context->setUserData(this);
    context->setPrintHook(&printHook);
    context->setSendHook(&sendHook);

#ifdef HV_NAME_CHANNEL_A
    registerParameter(PARAMETER_A, HV_NAME_CHANNEL_A);
#endif
#ifdef HV_NAME_CHANNEL_B
    registerParameter(PARAMETER_B, HV_NAME_CHANNEL_B);
#endif
#ifdef HV_NAME_CHANNEL_C
    registerParameter(PARAMETER_C, HV_NAME_CHANNEL_C);
#endif
#ifdef HV_NAME_CHANNEL_D
    registerParameter(PARAMETER_D, HV_NAME_CHANNEL_D);
#endif
#ifdef HV_NAME_CHANNEL_E
    registerParameter(PARAMETER_E, HV_NAME_CHANNEL_E);
#endif
#ifdef HV_NAME_CHANNEL_F
    registerParameter(PARAMETER_F, HV_NAME_CHANNEL_F);
#endif
#ifdef HV_NAME_CHANNEL_G
    registerParameter(PARAMETER_G, HV_NAME_CHANNEL_G);
#endif
#ifdef HV_NAME_CHANNEL_H
    registerParameter(PARAMETER_H, HV_NAME_CHANNEL_H);
#endif
#ifdef HV_NAME_CHANNEL_AA
    registerParameter(PARAMETER_AA, HV_NAME_CHANNEL_AA);
#endif
#ifdef HV_NAME_CHANNEL_AB
    registerParameter(PARAMETER_AB, HV_NAME_CHANNEL_AB);
#endif
#ifdef HV_NAME_CHANNEL_AC
    registerParameter(PARAMETER_AC, HV_NAME_CHANNEL_AC);
#endif
#ifdef HV_NAME_CHANNEL_AD
    registerParameter(PARAMETER_AD, HV_NAME_CHANNEL_AD);
#endif
#ifdef HV_NAME_CHANNEL_AE
    registerParameter(PARAMETER_AE, HV_NAME_CHANNEL_AE);
#endif
#ifdef HV_NAME_CHANNEL_AF
    registerParameter(PARAMETER_AF, HV_NAME_CHANNEL_AF);
#endif
#ifdef HV_NAME_CHANNEL_AG
    registerParameter(PARAMETER_AG, HV_NAME_CHANNEL_AG);
#endif
#ifdef HV_NAME_CHANNEL_AH
    registerParameter(PARAMETER_AH, HV_NAME_CHANNEL_AH);
#endif
#ifdef HV_NAME_CHANNEL_BA
    registerParameter(PARAMETER_BA, HV_NAME_CHANNEL_BA);
#endif
#ifdef HV_NAME_CHANNEL_BB
    registerParameter(PARAMETER_BB, HV_NAME_CHANNEL_BB);
#endif
#ifdef HV_NAME_CHANNEL_BC
    registerParameter(PARAMETER_BC, HV_NAME_CHANNEL_BC);
#endif
#ifdef HV_NAME_CHANNEL_BD
    registerParameter(PARAMETER_BD, HV_NAME_CHANNEL_BD);
#endif

    /*
    HvParameterInfo pinfo;
    int pcount = hv_getParameterInfo(context, 0, &pinfo);
    if (pcount > 0) {
      for (int i=0;;) {

        if (i==0) {
          registerParameter(PARAMETER_A, *pinfo->name);
        } else if (i==1){
          registerParameter(PARAMETER_B, *pinfo->name);
        }
        i++;
        if (i > pcount) break;
        hv_getParameterInfo(context, i, &pinfo);
      } 
    }
    */
  }
  
  ~HeavyPatch() {
    delete context;
  }

  uint16_t getButtonValue(PatchButtonId bid, const HvMessage *m){
    if(hv_msg_getNumElements(m) > 0 && hv_msg_isFloat(m, 0))
      return hv_msg_getFloat(m, 0) > 0.5 ? 4095 : 0;
    else
      return isButtonPressed(bid) ? 0 : 4095; // toggle
  }

  void sendCallback(uint32_t sendHash, const HvMessage *m){
    switch(sendHash){
    case HV_HASH_BUTTON_A:
    case HV_HASH_CHANNEL_PUSH:
      setButton(PUSHBUTTON, getButtonValue(PUSHBUTTON, m));
      break;
    // case HV_HASH_BUTTON_B:
    //   setButton(BUTTON_B, getButtonValue(BUTTON_B, m));
    //   break;
    // case HV_HASH_BUTTON_C:
    //   setButton(BUTTON_C, getButtonValue(BUTTON_C, m));
    //   break;
    // case HV_HASH_BUTTON_D:
    //   setButton(BUTTON_D, getButtonValue(BUTTON_D, m));
    //   break;
    // case HV_HASH_BUTTON_F:
    case HV_HASH_TR_F:  // Wizard trigger F out
      setButton(BUTTON_F, getButtonValue(BUTTON_F, m));
      break;
    // case HV_HASH_CHANNEL_F:
    case HV_HASH_CV_F: // Wizard CV F out
      setParameterValue(PARAMETER_F, hv_msg_getFloat(m, 0));
      break;
    // case HV_HASH_CHANNEL_G:
    case HV_HASH_CV_G: // Wizard CV G out
      setParameterValue(PARAMETER_G, hv_msg_getFloat(m, 0));
      break;
    case HV_HASH_NOTEOUT:
      {
      uint8_t note = hv_msg_getFloat(m, 0);
      uint8_t velocity = hv_msg_getFloat(m, 1);
      uint8_t ch = hv_msg_getFloat(m, 2);
      // debugMessage("noteout", note, velocity, ch);
      sendMidi(MidiMessage::note(ch, note, velocity));
      }
      break;
    case HV_HASH_CTLOUT:
      {
	uint8_t value = hv_msg_getFloat(m, 0);
	uint8_t cc = hv_msg_getFloat(m, 1);
	uint8_t ch = hv_msg_getFloat(m, 2);
	// debugMessage("ctlout", value, cc, ch);
	sendMidi(MidiMessage::cc(ch, cc, value));
      }
      break;
    case HV_HASH_BENDOUT:
      {
	uint16_t value = hv_msg_getFloat(m, 0);
	uint8_t ch = hv_msg_getFloat(m, 1);
      // debugMessage("bendout", value, ch);
	sendMidi(MidiMessage::pb(ch, value));
      }
      break;
    case HV_HASH_TOUCHOUT:
      sendMidi(MidiMessage::cp((uint8_t)hv_msg_getFloat(m, 1), (uint8_t)hv_msg_getFloat(m, 0)));
      break;
    case HV_HASH_PGMOUT:
      sendMidi(MidiMessage::pc((uint8_t)hv_msg_getFloat(m, 1), (uint8_t)hv_msg_getFloat(m, 0)));
      break;
#ifdef HV_HASH_SEND_CHANNEL_A
    case HV_HASH_SEND_CHANNEL_A:
      setParameterValue(PARAMETER_A, hv_msg_getFloat(m, 0));
      break;
#endif
#ifdef HV_HASH_SEND_CHANNEL_B
    case HV_HASH_SEND_CHANNEL_B:
      setParameterValue(PARAMETER_B, hv_msg_getFloat(m, 0));
      break;
#endif
#ifdef HV_HASH_SEND_CHANNEL_C
    case HV_HASH_SEND_CHANNEL_C:
      setParameterValue(PARAMETER_C, hv_msg_getFloat(m, 0));
      break;
#endif
#ifdef HV_HASH_SEND_CHANNEL_D
    case HV_HASH_SEND_CHANNEL_D:
      setParameterValue(PARAMETER_D, hv_msg_getFloat(m, 0));
      break;
#endif
#ifdef HV_HASH_SEND_CHANNEL_E
    case HV_HASH_SEND_CHANNEL_E:
      setParameterValue(PARAMETER_E, hv_msg_getFloat(m, 0));
      break;
#endif
#ifdef HV_HASH_SEND_CHANNEL_F
    case HV_HASH_SEND_CHANNEL_F:
      setParameterValue(PARAMETER_F, hv_msg_getFloat(m, 0));
      break;
#endif
#ifdef HV_HASH_SEND_CHANNEL_G
    case HV_HASH_SEND_CHANNEL_G:
      setParameterValue(PARAMETER_G, hv_msg_getFloat(m, 0));
      break;
#endif
#ifdef HV_HASH_SEND_CHANNEL_H
    case HV_HASH_SEND_CHANNEL_H:
      setParameterValue(PARAMETER_H, hv_msg_getFloat(m, 0));
      break;
#endif
#ifdef HV_HASH_SEND_CHANNEL_AA
    case HV_HASH_SEND_CHANNEL_AA:
      setParameterValue(PARAMETER_AA, hv_msg_getFloat(m, 0));
      break;
#endif
#ifdef HV_HASH_SEND_CHANNEL_AB
    case HV_HASH_SEND_CHANNEL_AB:
      setParameterValue(PARAMETER_AB, hv_msg_getFloat(m, 0));
      break;
#endif
#ifdef HV_HASH_SEND_CHANNEL_AC
    case HV_HASH_SEND_CHANNEL_AC:
      setParameterValue(PARAMETER_AC, hv_msg_getFloat(m, 0));
      break;
#endif
#ifdef HV_HASH_SEND_CHANNEL_AD
    case HV_HASH_SEND_CHANNEL_AD:
      setParameterValue(PARAMETER_AD, hv_msg_getFloat(m, 0));
      break;
#endif
#ifdef HV_HASH_SEND_CHANNEL_AE
    case HV_HASH_SEND_CHANNEL_AE:
      setParameterValue(PARAMETER_AE, hv_msg_getFloat(m, 0));
      break;
#endif
#ifdef HV_HASH_SEND_CHANNEL_AF
    case HV_HASH_SEND_CHANNEL_AF:
      setParameterValue(PARAMETER_AF, hv_msg_getFloat(m, 0));
      break;
#endif
#ifdef HV_HASH_SEND_CHANNEL_AG
    case HV_HASH_SEND_CHANNEL_AG:
      setParameterValue(PARAMETER_AG, hv_msg_getFloat(m, 0));
      break;
#endif
#ifdef HV_HASH_SEND_CHANNEL_AH
    case HV_HASH_SEND_CHANNEL_AH:
      setParameterValue(PARAMETER_AH, hv_msg_getFloat(m, 0));
      break;
#endif
#ifdef HV_HASH_SEND_CHANNEL_BA
    case HV_HASH_SEND_CHANNEL_BA:
      setParameterValue(PARAMETER_BA, hv_msg_getFloat(m, 0));
      break;
#endif
#ifdef HV_HASH_SEND_CHANNEL_BB
    case HV_HASH_SEND_CHANNEL_BB:
      setParameterValue(PARAMETER_BB, hv_msg_getFloat(m, 0));
      break;
#endif
#ifdef HV_HASH_SEND_CHANNEL_BC
    case HV_HASH_SEND_CHANNEL_BC:
      setParameterValue(PARAMETER_BC, hv_msg_getFloat(m, 0));
      break;
#endif
#ifdef HV_HASH_SEND_CHANNEL_BD
    case HV_HASH_SEND_CHANNEL_BD:
      setParameterValue(PARAMETER_BD, hv_msg_getFloat(m, 0));
      break;
#endif
    }
  }

  void processMidi(MidiMessage msg){
    // sendMessageToReceiverV parses format and loops over args, see HeavyContext.cpp
    switch(msg.getStatus()){
    case CONTROL_CHANGE:
      context->sendMessageToReceiverV
	(HV_HASH_CTLIN, 0, "fff",
	 (float)msg.getControllerValue(), // value
	 (float)msg.getControllerNumber(), // controller number
	 (float)msg.getChannel());
      break;
    case NOTE_ON:
      context->sendMessageToReceiverV
	(HV_HASH_NOTEIN, 0, "fff",
	 (float)msg.getNote(), // pitch
	 (float)msg.getVelocity(), // velocity
	 (float)msg.getChannel());
      break;
    case NOTE_OFF:
      context->sendMessageToReceiverV
	(HV_HASH_NOTEIN, 0, "fff",
	 (float)msg.getNote(), // pitch
	 0.0f, // velocity
	 (float)msg.getChannel());
      break;
    case CHANNEL_PRESSURE:
      context->sendMessageToReceiverV
	(HV_HASH_TOUCHIN, 0, "ff",
	 (float)msg.getChannelPressure(),
	 (float)msg.getChannel());
      break;
    case PITCH_BEND_CHANGE:
      context->sendMessageToReceiverV
	(HV_HASH_BENDIN, 0, "ff",	 
	 (float)msg.getPitchBend(),
	 (float)msg.getChannel());
      break;
    case PROGRAM_CHANGE:
      context->sendMessageToReceiverV
	(HV_HASH_PGMIN, 0, "ff",	 
	 (float)msg.getProgramChange(),
	 (float)msg.getChannel());
      break;
    }
  }

  void buttonChanged(PatchButtonId bid, uint16_t value, uint16_t samples){
    if(_msgLock)
      return;
    switch(bid){
    case PUSHBUTTON:
      context->sendFloatToReceiver(HV_HASH_CHANNEL_PUSH, value ? 1.0 : 0.0);
      break;
    case BUTTON_A:
      context->sendFloatToReceiver(HV_HASH_BUTTON_A, value ? 1.0 : 0.0);
      break;
    case BUTTON_B:
      context->sendFloatToReceiver(HV_HASH_BUTTON_B, value ? 1.0 : 0.0);
      break;
    case BUTTON_C:
      context->sendFloatToReceiver(HV_HASH_BUTTON_C, value ? 1.0 : 0.0);
      break;
    case BUTTON_D:
      context->sendFloatToReceiver(HV_HASH_BUTTON_D, value ? 1.0 : 0.0);
      break;
    }
  }

  void processAudio(AudioBuffer &buffer) {
    _msgLock = true;
#ifdef HV_HASH_RECV_CHANNEL_A
    context->sendFloatToReceiver(HV_HASH_RECV_CHANNEL_A, getParameterValue(PARAMETER_A));
#endif
#ifdef HV_HASH_RECV_CHANNEL_B
    context->sendFloatToReceiver(HV_HASH_RECV_CHANNEL_B, getParameterValue(PARAMETER_B));
#endif
#ifdef HV_HASH_RECV_CHANNEL_C
    context->sendFloatToReceiver(HV_HASH_RECV_CHANNEL_C, getParameterValue(PARAMETER_C));
#endif
#ifdef HV_HASH_RECV_CHANNEL_D
    context->sendFloatToReceiver(HV_HASH_RECV_CHANNEL_D, getParameterValue(PARAMETER_D));
#endif
#ifdef HV_HASH_RECV_CHANNEL_E
    context->sendFloatToReceiver(HV_HASH_RECV_CHANNEL_E, getParameterValue(PARAMETER_E));
#endif
#ifdef HV_EXTENDED_PARAMETERS
#ifdef HV_HASH_RECV_CHANNEL_F
    context->sendFloatToReceiver(HV_HASH_RECV_CHANNEL_F, getParameterValue(PARAMETER_F));
#endif
#ifdef HV_HASH_RECV_CHANNEL_G
    context->sendFloatToReceiver(HV_HASH_RECV_CHANNEL_G, getParameterValue(PARAMETER_G));
#endif
#ifdef HV_HASH_RECV_CHANNEL_H
    context->sendFloatToReceiver(HV_HASH_RECV_CHANNEL_H, getParameterValue(PARAMETER_H));
#endif
#ifdef HV_HASH_RECV_CHANNEL_AA
    context->sendFloatToReceiver(HV_HASH_RECV_CHANNEL_AA, getParameterValue(PARAMETER_AA));
#endif
#ifdef HV_HASH_RECV_CHANNEL_AB
    context->sendFloatToReceiver(HV_HASH_RECV_CHANNEL_AB, getParameterValue(PARAMETER_AB));
#endif
#ifdef HV_HASH_RECV_CHANNEL_AC
    context->sendFloatToReceiver(HV_HASH_RECV_CHANNEL_AC, getParameterValue(PARAMETER_AC));
#endif
#ifdef HV_HASH_RECV_CHANNEL_AD
    context->sendFloatToReceiver(HV_HASH_RECV_CHANNEL_AD, getParameterValue(PARAMETER_AD));
#endif
#ifdef HV_HASH_RECV_CHANNEL_AE
    context->sendFloatToReceiver(HV_HASH_RECV_CHANNEL_AE, getParameterValue(PARAMETER_AE));
#endif
#ifdef HV_HASH_RECV_CHANNEL_AF
    context->sendFloatToReceiver(HV_HASH_RECV_CHANNEL_AF, getParameterValue(PARAMETER_AF));
#endif
#ifdef HV_HASH_RECV_CHANNEL_AG
    context->sendFloatToReceiver(HV_HASH_RECV_CHANNEL_AG, getParameterValue(PARAMETER_AG));
#endif
#ifdef HV_HASH_RECV_CHANNEL_AH
    context->sendFloatToReceiver(HV_HASH_RECV_CHANNEL_AH, getParameterValue(PARAMETER_AH));
#endif
#ifdef HV_HASH_RECV_CHANNEL_BA
    context->sendFloatToReceiver(HV_HASH_RECV_CHANNEL_BA, getParameterValue(PARAMETER_BA));
#endif
#ifdef HV_HASH_RECV_CHANNEL_BB
    context->sendFloatToReceiver(HV_HASH_RECV_CHANNEL_BB, getParameterValue(PARAMETER_BB));
#endif
#ifdef HV_HASH_RECV_CHANNEL_BC
    context->sendFloatToReceiver(HV_HASH_RECV_CHANNEL_BC, getParameterValue(PARAMETER_BC));
#endif
#ifdef HV_HASH_RECV_CHANNEL_BD
    context->sendFloatToReceiver(HV_HASH_RECV_CHANNEL_BD, getParameterValue(PARAMETER_BD));
#endif
#endif //EXTENDED PARAMETERS
    _msgLock = false;
    float* outputs[] = {buffer.getSamples(LEFT_CHANNEL), buffer.getSamples(RIGHT_CHANNEL)};
    context->process(outputs, outputs, getBlockSize());
  }
  
private:
  HeavyContext* context;
};

// extern "C" {
static void sendHook(HeavyContextInterface* ctxt,
		     const char *receiverName,
		     uint32_t sendHash,
		     const HvMessage *m){
  HeavyPatch* patch = (HeavyPatch*)ctxt->getUserData();
  patch->sendCallback(sendHash, m);
}
// }
#endif // __HeavyPatch_hpp__
