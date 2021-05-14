#ifndef __PolyphonicProcessor_h__
#define __PolyphonicProcessor_h__

#include "MidiProcessor.h"
#include "SignalGenerator.h"

/**
 * Supports both Polyphonic Key Pressure and Channel Pressure Aftertouch.
 */
template<class SynthVoice, int VOICES>
class PolyphonicMidiProcessor : public MidiProcessor {
protected:
  static const uint16_t TAKEN = 0xffff;
  SynthVoice* voice[VOICES];
  uint8_t notes[VOICES];
  uint16_t allocation[VOICES];
  uint16_t allocated;
  bool dosustain = false;
protected:
  void take(uint8_t ch, MidiMessage msg){
    release(ch);
    notes[ch] = msg.getNote();
    allocation[ch] = TAKEN;
    voice[ch]->noteOn(msg);
  }
  void release(uint8_t ch){
    allocation[ch] = ++allocated;
    if(!dosustain)
      voice[ch]->gate(false);
  }
public:
  PolyphonicMidiProcessor() : allocated(0) {}
  virtual ~PolyphonicMidiProcessor(){};
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
      if(notes[i] == note)
	release(i);
  }
  void controlChange(MidiMessage msg){
    switch(msg.getControllerNumber()){
    case MIDI_CC_MODULATION:
      modulate(msg);
      break;
    case MIDI_CC_SUSTAIN:
      sustain(msg);
      break;
    case MIDI_ALL_NOTES_OFF:
      allNotesOff();
      break;
    }
  }
  void pitchbend(MidiMessage msg){
    for(int i=0; i<VOICES; ++i)
      voice[i]->pitchbend(msg);
  }
  void modulate(MidiMessage msg){
    float value = msg.getControllerValue()/127.0f;
    for(int i=0; i<VOICES; ++i)
      voice[i]->setModulation(value);
  }
  void sustain(MidiMessage msg){
    setSustain(msg.getControllerValue() > 63);
  }
  // todo: unison note on/off
  void allNotesOn() {
    for(int i=0; i<VOICES; ++i)
      voice[i]->gate(true);
  }
  void allNotesOff() {
    for(int i=0; i<VOICES; ++i)
      voice[i]->gate(false);      
  }
  void setVoice(size_t index, SynthVoice* obj){
    if(index < VOICES)
      voice[index] = obj;
  }
  SynthVoice* getVoice(size_t index){
    if(index < VOICES)
      return voice[index];
    return NULL;
  }
  void channelPressure(MidiMessage msg){
    // route channel pressure to all voices
    for(int i=0; i<VOICES; ++i)
      voice[i]->setPressure(msg.getChannelPressure()/127.0f);
  }
  void polyKeyPressure(MidiMessage msg){
    // route poly key pressure to the right voice
    uint8_t note = msg.getNote();
    for(int i=0; i<VOICES; ++i)
      if(notes[i] == note)
	voice[i]->setPressure(msg.getPolyKeyPressure()/127.0f);
  }
  void setParameter(uint8_t parameter_id, float value){
    for(int i=0; i<VOICES; ++i)
      voice[i]->setParameter(parameter_id, value);
  }
  bool getSustain(){
    return dosustain;
  }
  void setSustain(bool value){
    if(!value && dosustain){
      // gate off any sustained (but not active) voices
      for(int i=0; i<VOICES; ++i){
	if(allocation[i] != TAKEN)
	  voice[i]->gate(false);
      }
    }
    dosustain = value;
  }
};

template<class SynthVoice, int VOICES>
class PolyphonicSignalGenerator : public PolyphonicMidiProcessor<SynthVoice, VOICES>, public SignalGenerator {
private:
  FloatArray buffer;
public:
  PolyphonicSignalGenerator(FloatArray buffer) : buffer(buffer) {}
  virtual ~PolyphonicSignalGenerator(){};
  float generate(){
    float sample = this->voice[0]->generate();
    for(int i=1; i<VOICES; ++i)
      sample += this->voice[i]->generate();
    return sample;
  }
  void generate(FloatArray output){
    this->voice[0]->generate(output);
    for(int i=1; i<VOICES; ++i){
      this->voice[i]->generate(buffer);
      output.add(buffer);
    }
  }
  static PolyphonicSignalGenerator<SynthVoice, VOICES>* create(size_t blocksize){
    FloatArray buffer = FloatArray::create(blocksize);    
    return new PolyphonicSignalGenerator<SynthVoice, VOICES>(buffer);
  }
  static void destroy(PolyphonicSignalGenerator<SynthVoice, VOICES>* obj){
    FloatArray::destroy(obj->buffer);
    delete obj;
  }
};

template<class SynthVoice, int VOICES>
class PolyphonicMultiSignalGenerator : public PolyphonicMidiProcessor<SynthVoice, VOICES>, public MultiSignalGenerator {
private:
  AudioBuffer* buffer;
public:
  PolyphonicMultiSignalGenerator(AudioBuffer* buffer) : buffer(buffer) {}
  virtual ~PolyphonicMultiSignalGenerator(){};
  static PolyphonicMultiSignalGenerator<SynthVoice, VOICES>* create(size_t channels, size_t blocksize){
    AudioBuffer* buffer = AudioBuffer::create(channels, blocksize);    
    return new PolyphonicMultiSignalGenerator<SynthVoice, VOICES>(buffer);
  }
  static void destroy(PolyphonicMultiSignalGenerator<SynthVoice, VOICES>* obj){
    AudioBuffer::destroy(obj->buffer);
    delete obj;
  }
  void generate(AudioBuffer& output){
    this->voice[0]->generate(output);
    for(int i=1; i<VOICES; ++i){
      this->voice[i]->generate(*buffer);
      output.add(*buffer);
    }
  }  
};

#endif /* __PolyphonicProcessor_h__ */
