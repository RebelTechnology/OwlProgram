#ifndef __MaximilianPatch_hpp__
#define __MaximilianPatch_hpp__

#include "StompBox.h"

#include "maximilian.h"

class MaximilianPatch : public Patch {
private:
// #include "maximilian_examples/1.TestTone.cpp"
// #include "maximilian_examples/2.TwoTones.cpp"
// #include "maximilian_examples/3.AM1.cpp"
// #include "maximilian_examples/4.AM2.cpp"
// #include "maximilian_examples/5.FM1.cpp"
// #include "maximilian_examples/6.FM2.cpp"
// #include "maximilian_examples/7.Counting1.cpp"
// #include "maximilian_examples/8.Counting2.cpp"
// #include "maximilian_examples/8.Counting3.cpp"
// #include "maximilian_examples/8.Counting4.cpp"
// #include "maximilian_examples/9.Envelopes.cpp"
// #include "maximilian_examples/10.Filters.cpp"
// #include "maximilian_examples/11.Mixing.cpp"
// #include "maximilian_examples/13.Advanced-Filters.cpp"
#include "maximilian_examples/14.monosynth.cpp" // 32%
// #include "maximilian_examples/15.polysynth.cpp" // 188%
// #include "maximilian_examples/16.Replicant.cpp" // 110%
// #include "maximilian_examples/17.Compressor.cpp" // missing maxiSample::load
// #include "maximilian_examples/19.Enveloping2.cpp" // missing maxiSample::load
// #include "maximilian_examples/20.FFT_example.cpp" // missing/failing FFT lib

public:
  MaximilianPatch(){
    setup();
  }
  void processAudio(AudioBuffer &buffer) {
    float* left = buffer.getSamples(LEFT_CHANNEL);
    float* right = buffer.getSamples(RIGHT_CHANNEL);
    double output[2];
    for(int i=0; i<buffer.getSize(); ++i){
      play(output);
      left[i] = output[0];
      right[i] = output[1];
    }
  }
};

#endif   // __MaximilianPatch_hpp__
