#ifndef __DryWetProcessor_h__
#define __DryWetProcessor_h__

#include <utility>
#include "SignalProcessor.h"

template<class Processor>
class DryWetSignalProcessor : public Processor {
protected:
  FloatArray mix_buffer;
  float mix_amount;
public:
  template <typename... Args>
  DryWetSignalProcessor(FloatArray buffer, Args&&... args) :
    Processor(std::forward<Args>(args)...), mix_buffer(buffer), mix_amount(0) {}
  void setMix(float amount){
    mix_amount = amount;
  }
  float getMix(){
    return mix_amount;
  }
  void process(FloatArray input, FloatArray output){
    mix_buffer.copyFrom(input);
    Processor::process(input, output);
    output.multiply(mix_amount);
    mix_buffer.multiply(1-mix_amount);
    output.add(mix_buffer);
  }
  static void destroy(DryWetSignalProcessor<Processor>* obj){
    FloatArray::destroy(obj->mix_buffer);
    Processor::destroy(obj);
  }
};

template<class Processor>
class DryWetMultiSignalProcessor : public Processor {
protected:
  AudioBuffer* mix_buffer;
  float mix_amount;
public:
  template <typename... Args>
  DryWetMultiSignalProcessor(AudioBuffer* buffer, Args&&... args) :
    Processor(std::forward<Args>(args)...), mix_buffer(buffer), mix_amount(0) {}
  void setMix(float amount){
    mix_amount = amount;
  }
  float getMix(){
    return mix_amount;
  }
  void process(AudioBuffer& input, AudioBuffer& output){
    mix_buffer->copyFrom(input);
    Processor::process(input, output);
    output.multiply(mix_amount);
    mix_buffer->multiply(1-mix_amount);
    output.add(*mix_buffer);
  }
  static void destroy(DryWetMultiSignalProcessor<Processor>* obj){
    AudioBuffer::destroy(obj->mix_buffer);
    Processor::destroy(obj);
  }
};

#endif // __DryWetProcessor_h__
