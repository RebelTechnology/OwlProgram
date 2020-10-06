#ifndef __HeavyPatch_hpp__
#define __HeavyPatch_hpp__

#include "Patch.h"
#include "basicmaths.h"
#include "HvHeavy.h"
#include "Heavy_owl.hpp"
#include "Heavy_owl_constants.h"

#define BUTTON_Push PUSHBUTTON
#define BUTTON_B1 BUTTON_A
#define BUTTON_B2 BUTTON_B
#define BUTTON_B3 BUTTON_C
#define BUTTON_B4 BUTTON_D
#define BUTTON_B5 BUTTON_E
#define BUTTON_B6 BUTTON_F
#define BUTTON_B7 BUTTON_G
#define BUTTON_B8 BUTTON_H

#define HV_HASH_MIDIIN 0x149631be
#define HV_HASH_NOTEIN 0x67e37ca3
#define HV_HASH_CTLIN 0x41be0f9c
#define HV_HASH_BENDIN 0x3083f0f7
#define HV_HASH_TOUCHIN 0x553925bd
#define HV_HASH_PGMIN 0x2e1ea03d

#define HV_HASH_MIDIOUT 0x6511de55
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
    {% for param, name, typ, namehash, minvalue, maxvalue, defvalue, button in jdata if button == False %}
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
    {% if '__hv_midiout' in midiout|map(attribute=0) %}
    if(sendHash == HV_HASH_MIDIOUT){
      if(!hv_msg_hasFormat(m, "ff"))
	return;
      static MidiMessage msg;
      static uint8_t pos = 1;
      msg.data[pos++] = hv_msg_getFloat(m, 0);
      if(pos > msg.getLength()){
	uint8_t port = min(15, (int)hv_msg_getFloat(m, 1));
	msg.data[0] = (port << 4) | (msg.getStatus() >> 4);
	sendMidi(msg);
	pos = 1;
      }
    }
    {% endif %}
    switch(sendHash){
      {% for name, namehash in midiout %}
      // {{ name }} {{ namehash }}
      {% if name == '__hv_noteout' %}
    case HV_HASH_NOTEOUT:
      {
	if(!hv_msg_hasFormat(m, "fff"))
	  return;
	uint8_t note = min(127, hv_msg_getFloat(m, 0));
	uint8_t velocity = min(127, hv_msg_getFloat(m, 1));
	uint8_t ch = min(15, hv_msg_getFloat(m, 2));
	debugMessage("noteout", note, velocity, ch);
	sendMidi(MidiMessage::note(ch, note, velocity));
      }
      break;
      {% elif name == '__hv_ctlout' %}
    case HV_HASH_CTLOUT:
      {
	uint8_t value = min(127, hv_msg_getFloat(m, 0));
	uint8_t cc = min(127, hv_msg_getFloat(m, 1));
	uint8_t ch = min(15, hv_msg_getFloat(m, 1));
	// debugMessage("ctlout", value, cc, ch);
	sendMidi(MidiMessage::cc(ch, cc, value));
      }
      break;
      {% elif name == '__hv_bendout' %}
    case HV_HASH_BENDOUT:
      // [bendout] takes values from -8192 to 8191 - this won't change.
      {
	int16_t value = max(-8192, min(8191, hv_msg_getFloat(m, 0)));
	uint8_t ch = min(15, hv_msg_getFloat(m, 1));
      // debugMessage("bendout", value, ch);
	sendMidi(MidiMessage::pb(ch, value));
      }
      break;
      {% elif name == '__hv_touchout' %}
    case HV_HASH_TOUCHOUT:
      sendMidi(MidiMessage::cp((uint8_t)hv_msg_getFloat(m, 1), (uint8_t)hv_msg_getFloat(m, 0)));
      break;
      {% elif name == '__hv_pgmout' %}
    case HV_HASH_PGMOUT:
      sendMidi(MidiMessage::pc((uint8_t)hv_msg_getFloat(m, 1), (uint8_t)hv_msg_getFloat(m, 0)));
      break;
      {% endif %}
      {% endfor %}      
      {% for param, name, typ, namehash, minvalue, maxvalue, defvalue, button in jdata if typ == 'SEND'%}
      {% if button == True %}
    // Button {{name}}
    case HV_HASH_{{typ}}_CHANNEL_{{param}}:
      setButton(BUTTON_{{param}}, (hv_msg_getFloat(m, 0)-HV_MIN_CHANNEL_{{param}})/
		(HV_MAX_CHANNEL_{{param}}-HV_MIN_CHANNEL_{{param}}) > 0.5);
      {% else %}
    // Parameter {{name}}
    case HV_HASH_{{typ}}_CHANNEL_{{param}}:
      setParameterValue(PARAMETER_{{param}}, (hv_msg_getFloat(m, 0)-HV_MIN_CHANNEL_{{param}})/
  		        (HV_MAX_CHANNEL_{{param}}-HV_MIN_CHANNEL_{{param}}));
      {% endif %}
      break;
      {% endfor %}
    default:
      break;
    }
  }

  void processMidi(MidiMessage msg){
    // sendMessageToReceiverV parses format and loops over args, see HeavyContext.cpp
    {% if '__hv_midiin' in midiin|map(attribute=0) %}
      // __hv_midiin 0x149631be
    for(int i=1; i<4; ++i)
      context->sendMessageToReceiverV
	(HV_HASH_MIDIIN, i, "ff",
	 (float)msg.data[i],
	 (float)msg.getPort());
      {% endif %}
    switch(msg.getStatus()){
      {% for name, namehash in midiin %}
      // {{ name }} {{ namehash }}
      {% if name == '__hv_notein' %}
    case NOTE_OFF:
      msg.data[3] = 0; // ensure velocity is zero for note off
      // deliberate fall-through
    case NOTE_ON:
      context->sendMessageToReceiverV
	(HV_HASH_NOTEIN, 0, "fff",
	 (float)msg.getNote(), // pitch
	 (float)msg.getVelocity(), // velocity
	 (float)msg.getChannel());
      break;
      {% elif name == '__hv_ctlin' %}
    case CONTROL_CHANGE:
      context->sendMessageToReceiverV
	(HV_HASH_CTLIN, 0, "fff",
         (float)msg.getControllerValue(), // value
	 (float)msg.getControllerNumber(), // controller number
	 (float)msg.getChannel());
      break;    
      {% elif name == '__hv_touchin' %}
    case CHANNEL_PRESSURE:
      context->sendMessageToReceiverV
	( {{ namehash }}, 0, "ff",
	 (float)msg.getChannelPressure(),
	 (float)msg.getChannel());
      break;      
      {% elif name == '__hv_bendin' %}
    case PITCH_BEND_CHANGE:
      context->sendMessageToReceiverV
	(HV_HASH_BENDIN, 0, "ff",
	 (float)msg.getPitchBend() + 8192,
	 (float)msg.getChannel());
      break;
      {% elif name == '__hv_pgm' %}
    case PROGRAM_CHANGE:
      context->sendMessageToReceiverV
	(HV_HASH_PGMIN, 0, "ff",
	 (float)msg.getProgramChange(),
	 (float)msg.getChannel());
      break;
      {% endif %}
      {% endfor %}      
    default:
      break;
    }
  }

  void buttonChanged(PatchButtonId bid, uint16_t value, uint16_t samples){
    if(_msgLock)
      return;
    switch(bid){
    {% for param, name, typ, namehash, minvalue, maxvalue, defvalue, button in jdata if typ == 'RECV' and button == True %}
    // {{name}}
    case BUTTON_{{param}}:
      context->sendFloatToReceiver(HV_HASH_{{typ}}_CHANNEL_{{param}}, isButtonPressed(BUTTON_{{param}})*
				 (HV_MAX_CHANNEL_{{param}}-HV_MIN_CHANNEL_{{param}})+HV_MIN_CHANNEL_{{param}});
      break;
    {% endfor %}
    default:
      break;
    }
  }

  void processAudio(AudioBuffer &buffer) {
    _msgLock = true;
    {% for param, name, typ, namehash, minvalue, maxvalue, defvalue, button in jdata if typ == 'RECV' and button == False %}
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
