#ifndef __AbstractSynth_h__
#define __AbstractSynth_h__

#include <climits>
#include "Synth.h"
#include "MidiProcessor.h"
#include "VelocityCurve.h"

class AbstractSynth : public Synth, public MidiProcessor, public VelocityCurve {
protected:
  uint8_t note = 60; // C4
  float pb = 0;
  float pb_range = 2/8192.0f;
  float mod_range = 0.5/127.0f;
  float tuning = 440;
public:
  virtual ~AbstractSynth(){}
  /**
   * Set frequency in Hertz for middle A (defaults to Stuttgart pitch, A440, 440 Hz)
   */
  void setTuning(float value){
    tuning = value;
  }
  float getTuning(){
    return tuning;
  }
  /**
   * Set note in whole semitones
   */
  uint8_t getNote(){
    return note;
  }
  /**
   * Set note in whole semitones and update frequency
   */
  virtual void setNote(uint8_t note){
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
  virtual void setPitchBend(float pb){
    this->pb = pb;
    setFrequency(noteToFrequency(note+pb));
  }
  /**
   * Get pitch bend range in semitones
   */
  float getPitchBendRange(){
    return pb_range;
  }
  /**
   * Set pitch bend range in semitones.
   * Does not update the frequency; effective from next pitch bend change
   */
  void setPitchBendRange(float range){
    this->pb_range = range/8192.0f;
  }
  /**
   * Set modulation depth range, from 0 to 1.0
   */
  void setModulationDepthRange(float range){
    mod_range = range / 127.0f;
  }
  // MIDI handlers
  virtual void noteOn(MidiMessage msg) {
    setNote(msg.getNote());
    setFrequency(noteToFrequency(note+pb));
    setGain(velocityToGain(msg.getVelocity()));
    gate(true);
  }
  virtual void noteOff(MidiMessage msg) {
    gate(false);
  }
  virtual void controlChange(MidiMessage msg) {
    if(msg.getControllerNumber() == MIDI_CC_MODULATION)
      setModulation(msg.getControllerValue()/128.0f);
    else if(msg.getControllerNumber() == MIDI_ALL_NOTES_OFF)
      allNotesOff();
  }
  virtual void channelPressure(MidiMessage msg) {
    setPressure(msg.getChannelPressure()/128.0f);
  }
  virtual void polyKeyPressure(MidiMessage msg) {
    setPressure(msg.getPolyKeyPressure()/128.0f);
  }
  virtual void modulate(MidiMessage msg) {
    setModulation(mod_range * msg.getControllerValue());
  }
  virtual void pitchbend(MidiMessage msg) {
    setPitchBend(pb_range * msg.getPitchBend());
  }
  virtual void allNotesOff(){
    gate(false);
  }
  // default implementations do nothing
  virtual void setModulation(float modulation){} 
  virtual void setPressure(float pressure){}
  // static utility methods
  inline float frequencyToNote(float freq){
    return 12 * log2f(freq / tuning) + 69;
  }
  inline float noteToFrequency(float note){
    return tuning * exp2f((note - 69) / 12);
  }
};

#endif // __AbstractSynth_h__
