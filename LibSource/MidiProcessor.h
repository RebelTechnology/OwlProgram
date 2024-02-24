#ifndef __MidiProcessor_h__
#define __MidiProcessor_h__

#include "MidiMessage.h"

/**
 * Base class for MIDI processors such as AbstractSynth.
 * Derived classes can overload the specific message handlers,
 * and/or the entry-point: process(MidiMessage msg)
 */
class MidiProcessor {
public:
  virtual ~MidiProcessor(){}
  virtual void noteOn(MidiMessage msg){}
  virtual void noteOff(MidiMessage msg){}
  virtual void sustain(MidiMessage msg){}
  virtual void modulate(MidiMessage msg){}
  virtual void pitchbend(MidiMessage msg){}
  virtual void controlChange(MidiMessage msg){}
  virtual void channelPressure(MidiMessage msg){}
  virtual void polyKeyPressure(MidiMessage msg){}
  virtual void process(MidiMessage msg) {
    if(msg.isNoteOn()) {
      noteOn(msg);
    }else if(msg.isNoteOff()) {
      noteOff(msg);
    }else if(msg.isPitchBend()) {
      pitchbend(msg);
    }else if(msg.isControlChange()) {
      controlChange(msg);
    }else if(msg.isChannelPressure()) {
      channelPressure(msg);
    }else if(msg.isPolyKeyPressure()) {
      polyKeyPressure(msg);
    }
  }
};

#endif // __MidiProcessor_h__
