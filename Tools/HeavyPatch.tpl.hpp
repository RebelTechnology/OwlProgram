#ifndef __HeavyPatch_hpp__
#define __HeavyPatch_hpp__

#include "Patch.h"
#include "basicmaths.h"
#include "HvHeavy.h"
#include "Heavy_owl.hpp"
#include "Heavy_owl_constants.h"

#define HV_HASH_CHANNEL_PUSH 0x3cf4c9df
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
  static void owlPrintHook(HeavyContextInterface* ctxt,
			   const char *printLabel,
			   const char *msgString,
			   const HvMessage *m) {
    char buf[64];
    char* dst = buf;
    int len = strnlen(printLabel, 48);
    dst = stpncpy(dst, printLabel, len);
    dst = stpcpy(dst, " ");
    dst = stpncpy(dst, msgString, 63-len);
    debugMessage(buf);
  }
  static void owlSendHook(HeavyContextInterface* ctxt,
			  const char *receiverName,
			  uint32_t sendHash,
			  const HvMessage *m);
}

class HeavyPatch : public Patch {
public:
  HeavyPatch() {
    context = new Heavy_owl(getSampleRate(),
			    HEAVY_MESSAGE_POOL_SIZE,
			    HEAVY_MESSAGE_IN_QUEUE_SIZE,
			    HEAVY_MESSAGE_OUT_QUEUE_SIZE);
    context->setUserData(this);
    context->setPrintHook(&owlPrintHook);
    context->setSendHook(&owlSendHook);
    {% for param, name, typ, namehash, minvalue, maxvalue, defvalue in jdata %}
    // {{name}}
    registerParameter(PARAMETER_{{param}}, HV_NAME_CHANNEL_{{param}});
    setParameterValue(PARAMETER_{{param}}, HV_DEFAULT_CHANNEL_{{param}});
    {% endfor %}
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
    {% for param, name, typ, namehash, minvalue, maxvalue, defvalue in jdata if typ == 'SEND'%}
    // {{name}}
    case HV_HASH_{{typ}}_CHANNEL_{{param}}:
      setParameterValue(PARAMETER_{{param}}, (hv_msg_getFloat(m, 0)-HV_MIN_CHANNEL_{{param}})/
			(HV_MAX_CHANNEL_{{param}}-HV_MIN_CHANNEL_{{param}}));
      break;
    {% endfor %}
    default:
      break;
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
    {% for param, name, typ, namehash, minvalue, maxvalue, defvalue in jdata if typ == 'RECV' %}
    // {{name}}
      context->sendFloatToReceiver(HV_HASH_{{typ}}_CHANNEL_{{param}}, getParameterValue(PARAMETER_{{param}})*
				 (HV_MAX_CHANNEL_{{param}}-HV_MIN_CHANNEL_{{param}})+HV_MIN_CHANNEL_{{param}});
    {% endfor %}

    _msgLock = false;
    float* outputs[] = {buffer.getSamples(LEFT_CHANNEL), buffer.getSamples(RIGHT_CHANNEL)};
    context->process(outputs, outputs, getBlockSize());
  }

private:
  HeavyContext* context;
};

static void owlSendHook(HeavyContextInterface* ctxt,
			const char *receiverName,
			uint32_t sendHash,
			const HvMessage *m){
  HeavyPatch* patch = (HeavyPatch*)ctxt->getUserData();
  patch->sendCallback(sendHash, m);
}

#endif // __HeavyPatch_hpp__
