#ifndef __VoiceAllocator_h__
#define __VoiceAllocator_h__

template<class SynthVoice, int VOICES>
class VoiceAllocator : public MidiProcessor {
protected:
  SynthVoice* voice[VOICES];
  float pb_range = 2/8192.0f;
  float mod_range = 0.5/127.0f;
  bool dosustain = false;
  uint16_t rpn_id = 0;
  uint8_t rpn_msb = 0;
  uint8_t tuning_semitones = 69;
  static float noteToFrequency(float note){
    return 440 * exp2f((note - 69) / 12);
  }
  static float frequencyToNote(float freq){
    return 12 * log2f(freq / 440) + 69;
  }
public:
  float getPitchBendRange(){
    return voice[0]->getPitchBendRange();
  }
  /**
   * Set pitch bend range in semitones.
   * Does not update the frequency; effective from next pitch bend change
   */
  void setPitchBendRange(float range){
    for(int i=0; i<VOICES; ++i)
      voice[i]->setPitchBendRange(range);
  }
  /**
   * Set modulation depth range, from 0 to 1.0
   */
  void setModulationDepthRange(float range){
    for(int i=0; i<VOICES; ++i)
      voice[i]->setModulationDepthRange(range);
  }
  /**
   * Set the MIDI velocity dynamic range in dB, default 72dB
   */
  void setDynamicRange(float range){
    for(int i=0; i<VOICES; ++i)
      voice[i]->setDynamicRange(range);    
  }
  float getTuning(){
    return voice[0]->getTuning();
  }
  void setTuning(float frequency){
    for(int i=0; i<VOICES; ++i)
      voice[i]->setTuning(frequency);
  }
  void allNotesOff() {
    for(int i=0; i<VOICES; ++i)
      voice[i]->gate(false);      
  }
  // todo: unison note on/off
  void allNotesOn() {
    for(int i=0; i<VOICES; ++i)
      voice[i]->gate(true);
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
  void setParameter(uint8_t parameter_id, float value){
    for(int i=0; i<VOICES; ++i)
      voice[i]->setParameter(parameter_id, value);
  }
  void sustain(MidiMessage msg){
    setSustain(msg.getControllerValue() > 63);
  }
  bool getSustain(){
    return dosustain;
  }
  void setSustain(bool value){
    if(value != dosustain && !value)
      sustainOff();
    dosustain = value;
  }
  virtual void sustainOff(){}
  void rpn(uint16_t id, uint8_t msb, uint8_t lsb, MidiMessage msg){
    switch(id){
    case MIDI_RPN_PITCH_BEND_RANGE: {
      float semitones = msb + lsb/100.0f; // semitones and cents
      setPitchBendRange(semitones);
      break;
    }
    case MIDI_RPN_FINE_TUNING: {
      // The MSB and LSB together form a 14-bit value that sets the tuning in semitones,
      // with a +/- 1 semitone range, where 0x2000 is A440 tuning
      // MIDI note 69 is Middle A, 440Hz concert pitch
      float semitones = tuning_semitones + (((msb << 7) | lsb) - 8192) / 8192.0f;
      setTuning(noteToFrequency(semitones));
      break;
    }
    case MIDI_RPN_COARSE_TUNING: {
      // Setting the coarse tuning adjusts the tuning in semitones, where 0x40 is A440 tuning.
      // Setting coarse tuning resets fine tuning.
      // Only the coarse data entry / MSB is used.
      tuning_semitones = 69 + msb - 0x40;
      setTuning(noteToFrequency(tuning_semitones));
      break;
    }
    case MIDI_RPN_MODULATION_DEPTH_RANGE: {
      setModulationDepthRange(((msb << 7) | lsb) / 16384.0f);
      break;
    }
    }
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
    // case MIDI_CC_DATA_INCR: // Data Button Increment
    // case MIDI_CC_DATA_DECR: // Data Button Decrement
    case MIDI_CC_RPN_LSB: // RPN LSB comes first
      rpn_id = msg.getControllerValue();
      break;
    case MIDI_CC_RPN_MSB: // Then comes RPN MSB
      rpn_id |= msg.getControllerValue() << 7;
      if(rpn_id == MIDI_RPN_RESET){ // RPN reset
	rpn_id = 0;
	rpn_msb = 0;
      }
      break;
    case MIDI_CC_DATAENTRY_MSB: // Data Entry MSB
      rpn_msb = msg.getControllerValue();
      rpn(rpn_id, rpn_msb, 0, msg); // call rpn callback with zero LSB
      break;
    case MIDI_CC_DATAENTRY_LSB: // Data Entry LSB
      rpn(rpn_id, rpn_msb, msg.getControllerValue(), msg);
      break;
    }
  }
};

template<class Allocator, class SynthVoice, int VOICES>
class VoiceAllocatorSignalGenerator : public Allocator, public SignalGenerator {
  static_assert(std::is_base_of<VoiceAllocator<SynthVoice, VOICES>, Allocator>::value, "Allocator must derive from VoiceAllocator");
  typedef VoiceAllocatorSignalGenerator<Allocator, SynthVoice, VOICES> MyClass;
protected:
  FloatArray buffer;
public:
  template <typename... Args>
  VoiceAllocatorSignalGenerator(FloatArray buffer, Args&&... args) :
    Allocator(std::forward<Args>(args)...), buffer(buffer) {}
  virtual ~VoiceAllocatorSignalGenerator(){};
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
  static MyClass* create(size_t blocksize){
    FloatArray buffer = FloatArray::create(blocksize);    
    return new MyClass(buffer);
  }
  static void destroy(MyClass* obj){
    FloatArray::destroy(obj->buffer);
    delete obj;
  }
};

template<class Allocator, class SynthVoice, int VOICES>
class VoiceAllocatorSignalProcessor : public Allocator, public SignalProcessor {
  static_assert(std::is_base_of<VoiceAllocator<SynthVoice, VOICES>, Allocator>::value, "Allocator must derive from VoiceAllocator");
  typedef VoiceAllocatorSignalProcessor<Allocator, SynthVoice, VOICES> MyClass;
protected:
  FloatArray buffer;
public:
  template <typename... Args>
  VoiceAllocatorSignalProcessor(FloatArray buffer, Args&&... args) :
    Allocator(std::forward<Args>(args)...), buffer(buffer) {}
  virtual ~VoiceAllocatorSignalProcessor(){};
  using MidiProcessor::process;
  float process(float input){
    float sample = 0;
    for(int i=0; i<VOICES; ++i)
      sample += this->voice[i]->process(input);
    return sample;
  }
  void process(FloatArray input, FloatArray output){
    for(int i=0; i<VOICES; ++i){
      this->voice[i]->process(input, buffer);
      output.add(buffer);
    }
  }
  static MyClass* create(size_t blocksize){
    FloatArray buffer = FloatArray::create(blocksize);    
    return new MyClass(buffer);
  }
  static void destroy(MyClass* obj){
    FloatArray::destroy(obj->buffer);
    delete obj;
  }
};

template<class Allocator, class SynthVoice, int VOICES>
class VoiceAllocatorMultiSignalGenerator : public Allocator, public MultiSignalGenerator {
  static_assert(std::is_base_of<VoiceAllocator<SynthVoice, VOICES>, Allocator>::value, "Allocator must derive from VoiceAllocator");
  typedef VoiceAllocatorMultiSignalGenerator<Allocator, SynthVoice, VOICES> MyClass;
protected:
  AudioBuffer* buffer;
public:
  template <typename... Args>
  VoiceAllocatorMultiSignalGenerator(AudioBuffer* buffer, Args&&... args) :
    Allocator(std::forward<Args>(args)...), buffer(buffer) {}
  virtual ~VoiceAllocatorMultiSignalGenerator(){};
  void generate(AudioBuffer& output){
    this->voice[0]->generate(output);
    for(int i=1; i<VOICES; ++i){
      this->voice[i]->generate(*buffer);
      output.add(*buffer);
    }
  }  
  static MyClass* create(size_t channels, size_t blocksize){
    AudioBuffer* buffer = AudioBuffer::create(channels, blocksize);
    return new MyClass(buffer);
  }
  static void destroy(MyClass* obj){
    AudioBuffer::destroy(obj->buffer);
    delete obj;
  }
};

template<class Allocator, class SynthVoice, int VOICES>
class VoiceAllocatorMultiSignalProcessor : public Allocator, public MultiSignalProcessor {
  static_assert(std::is_base_of<VoiceAllocator<SynthVoice, VOICES>, Allocator>::value, "Allocator must derive from VoiceAllocator");
  typedef VoiceAllocatorMultiSignalProcessor<Allocator, SynthVoice, VOICES> MyClass;
protected:
  AudioBuffer* buffer;
public:
  template <typename... Args>
  VoiceAllocatorMultiSignalProcessor(AudioBuffer* buffer, Args&&... args) :
    Allocator(std::forward<Args>(args)...), buffer(buffer) {}
  virtual ~VoiceAllocatorMultiSignalProcessor(){};
  using MidiProcessor::process;
  void process(AudioBuffer& input, AudioBuffer& output){
    for(int i=0; i<VOICES; ++i){
      this->voice[i]->process(input, *buffer);
      output.add(*buffer);
    }
  }  
  static MyClass* create(size_t channels, size_t blocksize){
    AudioBuffer* buffer = AudioBuffer::create(channels, blocksize);
    return new MyClass(buffer);
  }
  static void destroy(MyClass* obj){
    AudioBuffer::destroy(obj->buffer);
    delete obj;
  }
};


#endif /* __VoiceAllocator_h__ */
