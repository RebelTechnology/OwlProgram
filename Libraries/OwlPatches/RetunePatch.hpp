#ifndef __RetunePatch_h__
#define __RetunePatch_h__

#include "retuner.h"
#include "StompBox.h"

extern "C" int _gettimeofday(struct timeval *tv, struct timezone *tz){ return 0; }

class RetunePatch : public Patch {
private:
  Retuner retuner;
  uint16_t notemask = 0xFFF;
  /*
    param    min,   max,  ini
    R_TUNE 400.0, 480.0, 440.0
    R_BIAS   0.0,   1.0,   0.5
    R_FILT   0.50,  0.02,  0.1
    R_CORR   0.0,   1.0,   1.0
    R_OFFS  -2.0,   2.0,   0.0
  */
  float tune = 440.0;
  float bias = 0.5;
  float filt = 0.1;
  float corr = 1.0;
  float offs = 0.0;

public:
  RetunePatch() {
    float *fourPointers[4];
    AudioBuffer *retunerBuffers=createMemoryBuffer(4, 4096);
    for(int n=0; n<4; n++)
      fourPointers[n]=retunerBuffers->getSamples(n);
    retuner.init(getSampleRate(),fourPointers); 
    registerParameter(PARAMETER_A, "Notes");    
    registerParameter(PARAMETER_B, "Tune");    
    registerParameter(PARAMETER_C, "Bias");
    registerParameter(PARAMETER_D, "Corr");
  }

  volatile unsigned int *DWT_CYCCNT = (volatile unsigned int *)0xE0001004; //address of the register
  int dwtmax = 0;
  void processAudio(AudioBuffer &buffer){
    notemask = (uint16_t)(getParameterValue(PARAMETER_A)*0xfff);
    tune = getParameterValue(PARAMETER_B)*440.0+220.0;
    bias = getParameterValue(PARAMETER_C);
    corr = getParameterValue(PARAMETER_D);
    int size = buffer.getSize();
    retuner.set_notemask(notemask);
    retuner.set_refpitch(tune);
    retuner.set_notebias(bias);
    retuner.set_corrfilt(filt);
    retuner.set_corrgain(corr);
    retuner.set_corroffs(offs);

    float* input = buffer.getSamples(0);
    float output[size];
    // memset(output, 0, size*sizeof(float));
    retuner.process(size, input, output);
    memcpy(input, output, size*sizeof(float));
    if(*DWT_CYCCNT > dwtmax){
      dwtmax = *DWT_CYCCNT;
      debugMessage("dwt", dwtmax/getBlockSize());
    }
  }
};

#endif // __RetunePatch_h__
