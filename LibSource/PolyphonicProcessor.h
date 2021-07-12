#ifndef __PolyphonicProcessor_h__
#define __PolyphonicProcessor_h__

#include "MidiProcessor.h"
#include "SignalGenerator.h"
#include "VoiceAllocator.h"

/**
 * Supports both Polyphonic Key Pressure and Channel Pressure Aftertouch.
 */
template<class SynthVoice, int VOICES>
class PolyphonicProcessor : public VoiceAllocator<SynthVoice, VOICES> {
  typedef VoiceAllocator<SynthVoice, VOICES> Allocator;
protected:
  static const uint16_t TAKEN = 0xffff;
  uint8_t notes[VOICES];
  uint16_t allocation[VOICES];
  uint16_t allocated;
protected:
  void take(uint8_t ch, MidiMessage msg){
    release(ch);
    notes[ch] = msg.getNote();
    allocation[ch] = TAKEN;
    Allocator::voice[ch]->noteOn(msg);
  }
  void release(uint8_t ch){
    allocation[ch] = ++allocated;
    if(!Allocator::dosustain)
      Allocator::voice[ch]->gate(false);
  }
public:
  PolyphonicProcessor() : allocated(0) {}
  virtual ~PolyphonicProcessor(){};
  size_t getNumberOfTakenVoices(){
    size_t active = 0;
    for(int i=0; i<VOICES; ++i){
      if(allocation[i] == TAKEN)
	active++;
    }
    return active;
  }
  void noteOn(MidiMessage msg){
    uint8_t note = msg.getNote();
    uint16_t minval = USHRT_MAX;
    uint8_t minidx = 0;
    // take oldest free voice, to allow voices to ring out
    for(int i=0; i<VOICES; ++i){
      if(notes[i] == note){
	minidx = i;
	break;
      }
      if(allocation[i] < minval){
	minidx = i;
	minval = allocation[i];
      }
    }
    // take oldest voice
    take(minidx, msg);
  }
  void noteOff(MidiMessage msg){
    uint8_t note = msg.getNote();
    for(int i=0; i<VOICES; ++i)
      // if(notes[i] == note && allocation[i] == TAKEN)
      if(notes[i] == note)
	release(i);
  }
  void pitchbend(MidiMessage msg){
    for(int i=0; i<VOICES; ++i)
      Allocator::voice[i]->pitchbend(msg);
  }
  void modulate(MidiMessage msg){
    for(int i=0; i<VOICES; ++i)
      Allocator::voice[i]->modulate(msg);
  }
  void channelPressure(MidiMessage msg){
    // route channel pressure to all voices
    for(int i=0; i<VOICES; ++i)
      Allocator::voice[i]->setPressure(msg.getChannelPressure()/128.0f);
  }
  void polyKeyPressure(MidiMessage msg){
    // route poly key pressure to the right voice
    uint8_t note = msg.getNote();
    for(int i=0; i<VOICES; ++i)
      if(notes[i] == note)
	Allocator::voice[i]->setPressure(msg.getPolyKeyPressure()/128.0f);
  }
  void sustainOff(){
    // gate off any sustained (but not active) voices
    for(int i=0; i<VOICES; ++i){
      if(allocation[i] != TAKEN)
	Allocator::voice[i]->gate(false);
    }
  }
};

#endif /* __PolyphonicProcessor_h__ */
