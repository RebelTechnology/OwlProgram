#ifndef __MonophonicProcessor_h__
#define __MonophonicProcessor_h__

template<class SynthVoice>
class MonophonicMidiProcessor : public MidiProcessor {
protected:
  SynthVoice* voice;
  uint8_t notes[16];
  uint8_t lastNote = 0;
public:
  MonophonicMidiProcessor(SynthVoice* voice) : voice(voice) {}
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
 void controlChange(MidiMessage msg){
   if(msg.getControllerNumber() == MIDI_CC_MODULATION)
     setParameter(1, msg.getControllerValue()/127.0f);
   else if(msg.getControllerNumber() == MIDI_ALL_NOTES_OFF)
      allNotesOff();
  }
  void pitchbend(MidiMessage msg){
    voice->pitchbend(msg);
  }
  SynthVoice* getVoice(){
    return voice;
  }
  void setParameter(uint8_t parameter_id, float value){
    voice->setParameter(parameter_id, value);
  }
};

template<class SynthVoice>
class MonophonicSignalGenerator : public MonophonicMidiProcessor<SynthVoice>, public SignalGenerator {
public:
  MonophonicSignalGenerator(SynthVoice* voice) : MonophonicMidiProcessor<SynthVoice>(voice) {}
  float generate(){
    return this->voice->generate();
  }
  void generate(FloatArray output){
    this->voice->generate(output);
  }
};

template<class SynthVoice>
class MonophonicMultiSignalGenerator : public MonophonicMidiProcessor<SynthVoice>, public MultiSignalGenerator {
public:
  MonophonicMultiSignalGenerator(SynthVoice* voice) : MonophonicMidiProcessor<SynthVoice>(voice) {}
  void generate(AudioBuffer& output){
    this->voice->generate(output);
  }  
};

#endif // __MonophonicProcessor_h__
