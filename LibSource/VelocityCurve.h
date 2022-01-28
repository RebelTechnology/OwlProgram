#ifndef __VelocityCurve_h__
#define __VelocityCurve_h__

#include "basicmaths.h"

/**
 * General MIDI DLS Level 1 Specification:
 * The MIDI Note Velocity value is converted to attenuation in dB by the Concave Transform
 * attenuation = 20*log10(127^2/velocity^2)
 * so gain = velocity^2/127^2
 * Adjustable for dynamic range.
 * ref: https://www.cs.cmu.edu/~rbd/papers/velocity-icmc2006.pdf
 */
class VelocityCurve {
private:
  // defaults for 72dB range: r=10^(72/20), b=127/(126*sqrt(r)) - 1/126, m=(1-b)/127
  float b = 0.00803821f;
  float m = 0.00781072f;
public:
  void setDynamicRange(float db){
    float r = exp10f(db/20);
    b = 127/(126*sqrtf(r)) - 1.0f/126;
    m = (1-b)/127;
  }
  float velocityToGain(float v){
    /* a = peak RMS value, v is velocity */
    /* a = (m*v+b)^2 */
    float root = m*v+b;
    return root*root;
  }
};

#endif // __VelocityCurve_h__
