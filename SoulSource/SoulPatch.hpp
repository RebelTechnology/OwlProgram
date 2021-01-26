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

#include "soul.hpp"

namespace std {
  void __throw_bad_alloc (void) { error(PROGRAM_ERROR_STATUS, "bad alloc"); while(1); }
  void __throw_bad_function_call() { error(PROGRAM_ERROR_STATUS, "bad func"); while(1); }
  void __throw_length_error (const char *) { error(PROGRAM_ERROR_STATUS, "bad len"); while(1); }
}

#define MAX_MIDI_MESSAGES 32
class SoulPatch : public Patch {
private:
  SOULPATCH soulpatch;
  std::vector<SOULPATCH::Parameter> params;
  AudioBuffer* outputBuffer;
  SOULPATCH::MIDIMessage midiBuffer[32];
  size_t numMIDIMessages = 0;
public:
  SoulPatch(){
    soulpatch.init(getSampleRate(), 0);
    params = soulpatch.createParameterList();
    for(size_t i=0; i<params.size(); ++i){
      registerParameter(PatchParameterId(PARAMETER_A+i), params[i].properties.name);
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
      params[i].setValue(value * (max-min) + min);
    }
    for(size_t i=0; i<ctx.inputChannels.size(); ++i)
      ctx.inputChannels[i] = buffer.getSamples(i);
    for(size_t i=0; i<ctx.outputChannels.size(); ++i){
      outputBuffer->clear();
      ctx.outputChannels[i] = outputBuffer->getSamples(i);
      // ctx.outputChannels[i] = buffer.getSamples(i); // in-place
    }
    // debugMessage("xruns", (int)soulpatch.getNumXRuns());
    ctx.numFrames = buffer.getSize();
    ctx.incomingMIDI.messages = &midiBuffer[0];
    // ctx.incomingMIDI.numMessages = 0;
    ctx.incomingMIDI.numMessages = numMIDIMessages;
    soulpatch.render(ctx);
    numMIDIMessages = 0;
    for(size_t i=0; i<ctx.outputChannels.size(); ++i){
      buffer.getSamples(i).copyFrom(outputBuffer->getSamples(i));
    }
  }
};

#endif // __SoulPatch_hpp__
