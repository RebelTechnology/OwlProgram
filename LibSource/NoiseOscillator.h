#ifndef NOISE_OSCILLATOR_HPP
#define NOISE_OSCILLATOR_HPP

#include <stdint.h>
#include "Oscillator.h"
#include "basicmaths.h"

class WhiteNoiseOscillator : public Oscillator {
 public:
  /* returns white noise in the range -0.5 to 0.5 */
  virtual float getNextSample(){
#if 0 // #ifdef ARM_CORTEX
    // todo: fixme
    union {
      float f;
      uint32_t i;
    } x;
    x.i = arm_rand32();
    // brutally converting u32 to float by zeroing some
    // exponent bits in IEEE single precision float
    // x.i &= 0xbfffffff; // mask top exponent bit to get exponent values from 2^-1 to 2^-126 (-1 to 1)
    x.i &= 0x9fffffff; // mask top two exponent bits to get exponent values from 2^-2 to 2^-126 (-0.5 to 0.5)
    // mantissa holds a value from 1.0 to 1.999...
    // https://en.wikipedia.org/wiki/Single-precision_floating-point_format
    return x.f;
#else
    return (rand()/(RAND_MAX+1.0f)) * 2 - 1;
#endif
  }

  static WhiteNoiseOscillator* create(){
    return new WhiteNoiseOscillator();
  }
  static void destroy(WhiteNoiseOscillator* osc){
    delete osc;
  }
};

#ifdef ARM_CORTEX
#define CTZ(x) (__builtin_ctz(x))
#else
int inline CTZ(int num){
  int i=0;
  while (((num>>i)&1)==0 && i<sizeof(int)) i++;
  return i;
}
#endif

class PinkNoiseOscillator : public WhiteNoiseOscillator {
private:
  enum {
    NumPinkBins  = 16,
    NumPinkBins1 = NumPinkBins-1
  };
public:
  PinkNoiseOscillator(){
    m_count = 1;
    m_pink  = 0;
    for (int i=0; i<NumPinkBins; i++)
    {
      m_pinkStore[i] = 0.0f;
    }
  };

  /* returns pink noise random number in the range -0.5 to 0.5 */
  float getNextSample(){
    float prevr;
    float r;
    unsigned long k = CTZ(m_count);
    k = k & NumPinkBins1; 
    // get previous value of this octave 
    prevr = m_pinkStore[k]; 
    while (true){
      r = WhiteNoiseOscillator::getNextSample();
      // store new value 
      m_pinkStore[k] = r;
      r -= prevr;
      // update total 
      m_pink += r; 
      if (m_pink <-4.0f || m_pink > 4.0f) 
	m_pink -= r;
      else 
	break;
    }
    // update counter 
    m_count++; 
    return (WhiteNoiseOscillator::getNextSample() + m_pink)*0.125f; 
  }
  static PinkNoiseOscillator* create(){
    return new PinkNoiseOscillator();
  }
  static void destroy(PinkNoiseOscillator* osc){
    delete osc;
  }
private:
  unsigned long  m_count;
  float          m_pink;
  float          m_pinkStore[NumPinkBins];
};

/**
 * Oscillator that produces Brownian noise (aka red noise)
 */
class BrownNoiseOscillator : public WhiteNoiseOscillator {
private:
  float          m_brown;
public:
  BrownNoiseOscillator(){
    m_brown = 0.0f;
  }
  // returns brown noise random number in the range -0.5 to 0.5
  //
  float getNextSample() {
    while (true)
    {
      float  r = WhiteNoiseOscillator::getNextSample();
      m_brown += r;
      if (m_brown<-8.0f || m_brown>8.0f) 
	m_brown -= r;
      else 
	break;
    }
    return m_brown*0.0625f;
  }  
  static BrownNoiseOscillator* create(){
    return new BrownNoiseOscillator();
  }
  static void destroy(BrownNoiseOscillator* osc){
    delete osc;
  }
};

class GaussianNoiseOscillator : public Oscillator {
private:
  FloatArray noise; // whitenoise
  int phase;
public:
  GaussianNoiseOscillator(FloatArray ns) : noise(ns), phase(0) {}

  static void destroy(GaussianNoiseOscillator* gn){
    FloatArray::destroy(gn->noise);
    delete gn;
  }

  static GaussianNoiseOscillator* create(int size){
    GaussianNoiseOscillator* gn = new GaussianNoiseOscillator(FloatArray::create(size));
    // generate white gaussian noise:
    // from http://www.musicdsp.org/showone.php?id=168
    /* Setup constants */
    const static int q = 15;
    const static float c1 = (1 << q) - 1;
    const static float c2 = ((int)(c1 / 3)) + 1;
    const static float c3 = 1.f / c1;
    float max = 0;

    /* random number in range 0 - 1 not including 1 */
    float random = 0.f;
    for(int i = 0; i < gn->noise.getSize(); i++){
      random = ((float)rand() / (float)RAND_MAX);
      gn->noise[i] = (2.f * ((random * c2) + (random * c2) + (random * c2)) - 3.f * (c2 - 1.f)) * c3;	  
      if(fabs(gn->noise[i]) > max)
	max = fabs(gn->noise[i]);
    }
    for (int i = 0; i < gn->noise.getSize(); i++){
      //normalize the gain of our noise
      gn->noise[i] = gn->noise[i] / max;
    }

    return gn;
  }

  float getNextSample(){
    float sample = noise[phase];
    if(++phase == noise.getSize())
      phase = 0;
    return sample;
  }
};


#if 0 // ARM_CORTEX
class TrueNoiseOscillator : public Oscillator {
public:
  TrueNoiseOscillator() {
   RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG,ENABLE);
   RNG_Cmd(ENABLE);
  }
  float getNextSample(){
    while(RNG_GetFlagStatus(RNG_FLAG_DRDY) == RESET);
    uint32_t data = RNG_GetRandomNumber();
    return data/float(UINT32_MAX);
  }
};
#endif

#endif /* NOISE_OSCILLATOR_HPP */
