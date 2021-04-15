#ifndef __MidiProcessor_h__
#define __MidiProcessor_h__

#include "MidiMessage.h"

/**
 * Base class for MIDI processors such as @class AbstractSynth.
 * Derived classes can overload the specific message handlers,
 * and/or the entry-point: @method process(MidiMessage msg)
 */
class MidiProcessor {
public:
  virtual ~MidiProcessor(){}
  virtual void noteOn(MidiMessage msg){}
  virtual void noteOff(MidiMessage msg){}
  virtual void pitchbend(MidiMessage msg){}
  virtual void controlChange(MidiMessage msg){}
  virtual void channelPressure(MidiMessage msg){}
  virtual void process(MidiMessage msg) {
    if(msg.isNoteOn()) {
      if(msg.getVelocity())
	noteOn(msg);
      else
	noteOff(msg);
    }else if(msg.isNoteOff()) {
      noteOff(msg);
    }else if(msg.isPitchBend()) {
      pitchbend(msg);
    }else if(msg.isControlChange()) {
      controlChange(msg);
    }else if(msg.isChannelPressure()) {
      channelPressure(msg);
    }
  }  
};

#endif // __MidiProcessor_h__
