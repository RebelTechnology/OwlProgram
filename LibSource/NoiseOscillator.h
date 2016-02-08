#ifndef NOISE_OSCILLATOR_HPP
#define NOISE_OSCILLATOR_HPP

#include <stdint.h>
#include "Oscillator.h"
#include "basicmaths.h"

class WhiteNoiseOscillator : public Oscillator {
 public:
  /* returns white noise in the range -0.5 to 0.5 */
  virtual float getNextSample(){
#ifdef ARM_CORTEX
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
  enum
  {
    NumPinkBins  = 16,
    NumPinkBins1 = NumPinkBins-1
  };

  PinkNoiseOscillator()
  {
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

    return (white() + m_pink)*0.125f; 
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
};

#ifdef 0 // ARM_CORTEX
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
