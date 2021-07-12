#ifndef __MonophonicProcessor_h__
#define __MonophonicProcessor_h__

#include "MidiProcessor.h"
#include "SignalGenerator.h"
#include "VoiceAllocator.h"

template<class SynthVoice>
class MonophonicProcessor : public VoiceAllocator<SynthVoice, 1> {
  typedef VoiceAllocator<SynthVoice, 1> Allocator;
protected:
  uint8_t notes[16];
  uint8_t lastNote = 0;
public:
  MonophonicProcessor() {}
  virtual void noteOn(MidiMessage msg){
    if(lastNote < 16)
      notes[lastNote++] = msg.getNote();
    Allocator::voice[0]->noteOn(msg);
  }
  virtual void noteOff(MidiMessage msg){
    uint8_t note = msg.getNote();
    int i;
    for(i = 0; i < lastNote; ++i) {
      if(notes[i] == note)
	break;
    }
    if(lastNote > 1) {
      lastNote--;
      while (i < lastNote) {
	notes[i] = notes[i + 1];
	i++;
      }
      Allocator::voice[0]->setNote(notes[lastNote - 1]);
    }else{
      if(!Allocator::dosustain)
	Allocator::voice[0]->gate(false);
      lastNote = 0;
    }
  }
  void allNotesOn() {
    Allocator::voice[0]->gate(true);
  }
  void allNotesOff() {
    Allocator::voice[0]->gate(false);
    lastNote = 0;
  }
  virtual void sustainOff(){
    if(lastNote == 0)
      Allocator::voice[0]->gate(false);
  }
  // void sustain(MidiMessage msg){} // todo
  void modulate(MidiMessage msg){
    Allocator::voice[0]->modulate(msg);
  }
  void pitchbend(MidiMessage msg){
    Allocator::voice[0]->pitchbend(msg);
  }
};

#endif // __MonophonicProcessor_h__
