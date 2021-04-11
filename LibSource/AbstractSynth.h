#ifndef __AbstractSynth_h__
#define __AbstractSynth_h__

#include "SignalGenerator.h"
#include "MidiProcessor.h"
#include "VelocityCurve.h"

class AbstractSynth : public MidiProcessor, public SignalGenerator, public VelocityCurve {
protected:
  uint8_t note = 69;
  float pb = 0;
  float pb_range = 2;
public:
  virtual ~AbstractSynth(){}
  // getters and setters
  /**
   * Set note in whole semitones
   */
  uint8_t getNote(){
    return note;
  }
  /**
   * Set note in whole semitones and update frequency
   */
  void setNote(uint8_t note){
    this->note = note;
    setFrequency(noteToFrequency(note+pb));
  }
  /**
   * Get pitch bend amount in semitones
   */
  float getPitchBend(){
    return pb;
  }
  /**
   * Set pitch bend amount in semitones and update frequency
   */
  void setPitchBend(float pb){
    this->pb = pb;
    setFrequency(noteToFrequency(note+pb));
  }
  float getPitchBendRange(){
    return pb_range;
  }
  void setPitchBendRange(float range){
    this->pb_range = range;
  }  
  float getFrequency(){
    return noteToFrequency(note+pb);
  }
  // pure abstract methods that must be implemented by a derived class
  virtual void setFrequency(float freq) = 0;
  virtual void setGain(float gain) = 0;
  virtual void gate(bool state) = 0;
  // MIDI handlers
  virtual void noteOn(MidiMessage msg){
    note = msg.getNote();
    setFrequency(noteToFrequency(note+pb));
    setGain(velocityToGain(msg.getVelocity()));
    gate(true);
  }
  virtual void noteOff(MidiMessage msg){
    gate(false);
  }
  virtual void controlChange(MidiMessage msg){
    if(msg.getControllerNumber() == MIDI_ALL_NOTES_OFF)
      allNotesOff();
  }
  virtual void allNotesOff(){
    gate(false);
  }
  virtual void pitchbend(MidiMessage msg){
    setPitchBend(pb_range*msg.getPitchBend()/8192.0f);
  }
  // static utility methods
  static inline float frequencyToNote(float freq){
    return 12 * log2f(freq / 440) + 69;
  }
  static inline float noteToFrequency(float note){
    return 440 * exp2f((note - 69) / 12);
  }
};
  
template<class SynthVoice, int VOICES>
class PolyphonicSynth : public AbstractSynth {
private:
  static const uint16_t TAKEN = 0xffff;
  SynthVoice* voice[VOICES];
  uint8_t notes[VOICES];
  uint16_t allocation[VOICES];
  uint16_t allocated;
  FloatArray buffer;
protected:
  void take(uint8_t ch, MidiMessage msg){
    release(ch);
    notes[ch] = note;
    allocation[ch] = TAKEN;
    voice[ch]->noteOn(msg);
  }
  void release(uint8_t ch){
    // notes[ch] = EMPTY;
    allocation[ch] = ++allocated;
    voice[ch]->setGate(false);
  }
public:
  PolyphonicSynth(float sr, int bs) : allocated(0) {
    for(int i=0; i<VOICES; ++i){
      voice[i] = SynthVoice::create(sr);
      notes[i] = 69; // middle A, 440Hz
      allocation[i] = 0;
    }
    buffer = FloatArray::create(bs);
  }
  ~PolyphonicSynth(){
    for(int i=0; i<VOICES; ++i)
      SynthVoice::destroy(voice[i]);
    FloatArray::destroy(buffer);
  }
  void allNotesOff(){
    for(int i=0; i<VOICES; ++i)
      release(i, 0);
    allocated = 0;
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
    // debugMessage("idx/note/vel", minidx, note, velocity);
  }
  void noteOff(MidiMessage msg){
    uint8_t note = msg.getNote();
    for(int i=0; i<VOICES; ++i)
      if(notes[i] == note)
	release(i);
  }
  void generate(FloatArray output){
    voice[0]->getSamples(output);
    for(int i=1; i<VOICES; ++i){
      voice[i]->getSamples(buffer);
      output.add(buffer);
    }
  }
  void setFrequency(float freq){
    for(int i=0; i<VOICES; ++i)
      voice[i]->setFrequency(freq);
  }
  void setGain(float gain){
    for(int i=0; i<VOICES; ++i)
      voice[i]->setGain(gain);
  }
  void gate(bool state){
    for(int i=0; i<VOICES; ++i)
      voice[i]->gate(state);
  }
  void setPitchBend(float pb){
    for(int i=0; i<VOICES; ++i)
      voice[i]->setPitchBend(pb);
  }
  SynthVoice* getVoice(size_t index){
    if(index < VOICES)
      return voice[index];
    return NULL;
  }
};

template<class SynthVoice>
class MonophonicSynth : public AbstractSynth {
private:
  SynthVoice* voice;
  uint8_t notes[16];
  uint8_t lastNote = 0;
public:
  MonophonicSynth(SynthVoice* voice) : voice(voice) {}
  virtual void noteOn(MidiMessage msg){
    if(lastNote < 16)
      notes[lastNote++] = msg.getNote();
    voice->noteOn(msg);
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
      voice->setNote(notes[lastNote - 1]);
    }else{
      voice->gate(false);
      lastNote = 0;
    }
  }
  void allNotesOff() {
    voice->gate(false);
    lastNote = 0;
  }
  float generate(){
    return voice->generate();
  }
  void generate(FloatArray output){
    voice->generate(output);
  }
  void setFrequency(float freq){
    voice->setFrequency(freq);
  }
  void setGain(float gain){
    voice->setGain(gain);
  }
  void gate(bool state){
    voice->gate(state);
  }
  void setPitchBend(float pb){
    voice->setPitchBend(pb);
    debugMessage("pb", pb);
  }
  // void pitchbend(MidiMessage msg){
  //   voice->pitchbend(msg);
  // }
  SynthVoice* getVoice(){
    return voice;
  }
};

#endif // __AbstractSynth_h__
