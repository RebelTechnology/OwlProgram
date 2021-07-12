#ifndef __MidiPolyphonicExpressionProcessor_h__
#define __MidiPolyphonicExpressionProcessor_h__

#include "MidiProcessor.h"
#include "SignalGenerator.h"
#include "VoiceAllocator.h"

/**
 * Implementation of MPE (MIDI Polyphonic Expression) message processing.
 * We are controlling an AbstractSynth with:
 * X: Pitch Bend: setFrequency()
 * Y: CC74 or CC1 : setModulation()
 * Z: Channel Pressure : setPressure()
 */
template<class SynthVoice, int VOICES>
class MidiPolyphonicExpressionProcessor : public VoiceAllocator<SynthVoice, VOICES> {
// routes five per-note messages (Note On, Note Off, Pitch Bend, CC74 and Channel Pressure/Aftertouch)
// to the voice assigned to each channel
  typedef VoiceAllocator<SynthVoice, VOICES> Allocator;
protected:
  static const uint16_t TAKEN = 0xffff;
  uint8_t notes[VOICES];
  uint16_t allocation[VOICES];
  uint16_t allocated;
  float pressure[VOICES];
  float modulation[VOICES];
  uint8_t master_channel = 1;
  // One channel (usually Channel 1) is used for global messages
  // The global channel is set by the "MPE Zone." When the MPE Zone is the Lower Zone, Channel 1 is used for global messages. When the MPE Zone is Upper Zone is, Channel 16 is used for global messages.
  float zone_pitchbend = 0;
  float zone_pressure = 0;
  float zone_modulation = 0;
  float note_pitchbend_range = 48;
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
  MidiPolyphonicExpressionProcessor() {}
  virtual ~MidiPolyphonicExpressionProcessor(){};

  uint8_t findFreeVoice(MidiMessage msg){
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
    return minidx;
  }

  void releaseVoicesForNote(MidiMessage msg){
    uint8_t note = msg.getNote();
    for(int i=0; i<VOICES; ++i)
      if(notes[i] == note)
	release(i);
  }

  void noteOn(MidiMessage msg){
    if(isMasterChannel(msg)){ // Zone message
      uint8_t idx = findFreeVoice(msg);
      take(idx, msg);
    }else{
      uint8_t idx = getNoteChannel(msg);
      take(idx, msg);
    }
  }

  void noteOff(MidiMessage msg){
    if(isMasterChannel(msg)){ // Zone message
      releaseVoicesForNote(msg);
    }else{
      uint8_t ch = getNoteChannel(msg);
      if(ch < VOICES)
	release(ch);
    }
  }
  void controlChange(MidiMessage msg){
    switch(msg.getControllerNumber()){
    case MIDI_CC_MODULATION: // handle modulation same as CC74 for non-MPE compatibility
    case MIDI_CC_FREQ_CUTOFF: {
      // All MPE receivers are required to respond to CC #74 at the Zone and Note level. If a device receives CC #74
      // on both a Master Channel and Member Channel, it must combine such data meaningfully and separately for
      // each sounding note.
      float value = Allocator::mod_range * msg.getControllerValue();
      if(isMasterChannel(msg)){ // Zone message
	for(int i=0; i<VOICES; ++i)
	  Allocator::voice[i]->setModulation(value + modulation[i]);
	zone_modulation = value;
      }else{
	uint8_t ch = getNoteChannel(msg);
	if(ch < VOICES){
	  Allocator::voice[ch]->setModulation(zone_modulation + value);
	  modulation[ch] = value;
	}
      }
      break;
    }
    default:
      Allocator::controlChange(msg);
      break;
    }
  }
  void rpn(uint16_t id, uint8_t msb, uint8_t lsb, MidiMessage msg){
    switch(id){
    case MIDI_RPN_PITCH_BEND_RANGE: {
      float semitones = msb + lsb/100.0f; // semitones and cents
	if(isMasterChannel(msg)){
	  Allocator::setPitchBendRange(semitones);
	}else{
	  note_pitchbend_range = semitones;
	}
	break;
    }
    case MIDI_RPN_MPE_CONFIGURATION: {
	uint8_t n = msg.getChannel();
	uint8_t mm = msg.getControllerValue();
	// mm=0: MPE is Off (No Channels)
	// mm=1 to F: Assigns that number of MIDI Channels to the Zone
	if(mm != 0){
	  if(n == 0x0) // n=0: Lower Zone Master Channel
	    master_channel = 1;
	  else if(n == 0xf) // n=F: Upper Zone Master Channel
	    master_channel = 0xf;
	  // All other channel values are invalid and should be ignored
	}
	// Each Zone is activated with its own message, which can be sent in either order.
	// Sending an MCM with the number of Member Channels set to zero deactivates that Zone.
	break;
    }
    default:
      Allocator::rpn(id, msb, lsb, msg);
      break;
    }
  }
  bool isMasterChannel(MidiMessage msg){
    return msg.getChannel()+1 == master_channel;
  }
  uint8_t getNoteChannel(MidiMessage msg){
    // The Lower Zone is controlled by Master Channel 1,
    // with Member Channels assigned sequentially from Channel 2 upwards.
    if(master_channel == 1)
      return (msg.getChannel()-1) % VOICES;
    else
      return (14-msg.getChannel()) % VOICES;
    // The Upper Zone is controlled by Master Channel 16,
    // with Member Channels assigned sequentially from Channel 15 downwards.
  }
  void pitchbend(MidiMessage msg){
    // Pitch Bend is both a Zone Message and a Note Level Message. If an MPE synthesizer receives Pitch Bend (for example) on both a Master and a Member Channel, it must combine the data meaningfully. The same is true for Channel Pressure. 
    if(isMasterChannel(msg)){ // Zone message
      float value = Allocator::getPitchBendRange()*msg.getPitchBend()/8192.0f;
      float delta = value - zone_pitchbend;
      for(int i=0; i<VOICES; ++i)
	Allocator::voice[i]->setPitchBend(Allocator::voice[i]->getPitchBend()+delta);
      zone_pitchbend = value;
    }else{
      float value = note_pitchbend_range*msg.getPitchBend()/8192.0f;
      uint8_t ch = getNoteChannel(msg);
      if(ch < VOICES){ // Note level message
	Allocator::voice[ch]->setPitchBend(zone_pitchbend + value);
      }
    }
  }
  void channelPressure(MidiMessage msg){
    // All MPE receivers are required to respond to Channel Pressure at the Note and Zone level.
    // If a device receives Channel Pressure on both a Master Channel and Member Channel it must
    // combine such data meaningfully and separately for each sounding note.
    float value = msg.getChannelPressure()/128.0f;
    if(isMasterChannel(msg)){ // Zone message
      for(int i=0; i<VOICES; ++i)
	Allocator::voice[i]->setPressure(value + pressure[i]);
      zone_pressure = value;
    }else{
      uint8_t ch = getNoteChannel(msg);
      if(ch < VOICES){ // Note level message
	Allocator::voice[ch]->setPressure(zone_pressure + value);
	pressure[ch] = value;
      }
    }
  }
  void polyKeyPressure(MidiMessage msg){
    // Polyphonic Key Pressure may be used with notes on the Master Channel, but not on other Channels
    if(isMasterChannel(msg)){
      uint8_t note = msg.getNote();
      for(int i=0; i<VOICES; ++i)
	if(Allocator::voice[i]->getNote() == note)
	  Allocator::voice[i]->setPressure(msg.getPolyKeyPressure()/128.0f);
    }
  }
  void sustainOff(){
    // gate off any sustained (but not active) voices
    for(int i=0; i<VOICES; ++i){
      if(allocation[i] != TAKEN)
	Allocator::voice[i]->gate(false);
    }
  }
};

#endif /* __MidiPolyphonicExpressionProcessor_h__ */
