#ifndef __SoulPatch_hpp__
#define __SoulPatch_hpp__

#include "Patch.h"

#undef min
#undef max
#undef sin
#undef cos
#undef exp
#undef sqrt
#undef pow
#undef log
#undef log10

#define SOUL_CPP_ASSERT(x)
#define SOUL_INTRINSICS owl

#include <limits>
namespace owl {
  inline float pow(float x, float y){ return fast_powf(x, y); }
  inline float exp(float x){ return fast_expf(x); }
  inline float log(float x){ return fast_logf(x); }
  inline float log10(float x){ return fast_log10f(x); }
#ifdef ARM_CORTEX
  inline float sqrt(float x){ return arm_sqrtf(x); }
  inline float sin(float x){ return arm_sin_f32(x); }
  inline float cos(float x){ return arm_cos_f32(x); }
#else
  inline float sqrt(float x){ return std::sqrt(x); }
  inline float sin(float x){ return std::sin(x); }
  inline float cos(float x){ return std::cos(x); }
#endif
  inline float tan(float x){ return std::tan(x); }
  inline float sinh(float x){ return std::sinh(x); }
  inline float cosh(float x){ return std::cosh(x); }
  inline float tanh(float x){ return std::tanh(x); }
  inline float asinh(float x){ return std::asinh(x); }
  inline float acosh(float x){ return std::acosh(x); }
  inline float atanh(float x){ return std::atanh(x); }
  inline float asin(float x){ return std::asin(x); }
  inline float acos(float x){ return std::acos(x); }
  inline float atan(float x){ return std::atan(x); }
  inline float atan2(float x, float y){ return fast_atan2f(x, y); }
  inline float isnan(float x){ return std::isnan(x); }
  inline float isinf(float x){ return std::isinf(x); }
  inline float floor(float x){ return std::floor(x); }
  inline float fmod(float x, float y){ return std::fmod(x, y); }
}
#define double float
#include "soul.hpp"

#define MAX_MIDI_MESSAGES 32
class SoulPatch : public Patch {
private:
  SOULPATCH soulpatch;
  SOULPATCH::ParameterList params;
  AudioBuffer* outputBuffer;
  SOULPATCH::MIDIMessage midiBuffer[32];
  size_t numMIDIMessages = 0;
public:
  SoulPatch(){
    soulpatch.init(getSampleRate(), 0);
    params = soulpatch.createParameterList();
    for(size_t i=0; i<params.size(); ++i){
      registerParameter(PatchParameterId(PARAMETER_A+i), params[i].properties.name);
      float min = params[i].properties.minValue;
      float max = params[i].properties.maxValue;
      float value = (params[i].properties.initialValue - min)/(max-min);
      setParameterValue(PatchParameterId(PARAMETER_A+i), value);
    }
    ASSERT(getNumberOfChannels() >= SOULPATCH::numAudioInputChannels, "Too many input channels in SOUL patch");
    ASSERT(getNumberOfChannels() >= SOULPATCH::numAudioOutputChannels, "Too many output channels in SOUL patch");
    outputBuffer = AudioBuffer::create(SOULPATCH::numAudioOutputChannels, getBlockSize());
  }
  void processMidi(MidiMessage msg){
    if(numMIDIMessages < MAX_MIDI_MESSAGES){
      midiBuffer[numMIDIMessages] = {0, msg.data[1], msg.data[2], msg.data[3]};
      numMIDIMessages++;
    }
  }
  void processAudio(AudioBuffer &buffer){
    SOULPATCH::RenderContext<float> ctx;
    for(size_t i=0; i<params.size(); ++i){
      float min = params[i].properties.minValue;
      float max = params[i].properties.maxValue;
      float value = getParameterValue(PatchParameterId(PARAMETER_A+i));
      SOULPATCH::Parameter& param = (SOULPATCH::Parameter&)params[i]; // uh oh, discarding const
      param.setValue(value * (max-min) + min);
    }
    for(size_t i=0; i<ctx.inputChannels.size(); ++i)
      ctx.inputChannels[i] = buffer.getSamples(i);
    for(size_t i=0; i<ctx.outputChannels.size(); ++i){
      FloatArray out = outputBuffer->getSamples(i);
      out.clear();
      ctx.outputChannels[i] = out;
      // ctx.outputChannels[i] = buffer.getSamples(i); // in-place
    }
    // debugMessage("xruns", (int)soulpatch.getNumXRuns());
    ctx.numFrames = buffer.getSize();
    ctx.incomingMIDI.messages = &midiBuffer[0];
    ctx.incomingMIDI.numMessages = numMIDIMessages;
    soulpatch.render(ctx);
    numMIDIMessages = 0;
    for(size_t i=0; i<ctx.outputChannels.size(); ++i)
      buffer.getSamples(i).copyFrom(outputBuffer->getSamples(i));
  }
};

#endif // __SoulPatch_hpp__
