#include "AdsrEnvelope.h"

template<>
float AdsrEnvelope<true>::increment(float level, float amount){
  return level + amount;
}

template<>
float AdsrEnvelope<true>::decrement(float level, float amount){
  return level + amount;
}

template<>
float AdsrEnvelope<false>::increment(float level, float amount){
  return level + (1.01-level)*amount; // aim slightly higher to ensure we reach 1.0
}

template<>
float AdsrEnvelope<false>::decrement(float level, float amount){
  return level + level * amount;
}

template<>
float AdsrEnvelope<true>::calculateIncrement(float startValue, float endValue, float time){
  return (endValue-startValue)/(sampleRate*time+1);
}

template<>
float AdsrEnvelope<false>::calculateIncrement(float startValue, float endValue, float time) {
  // Ref: Christian Schoenebeck http://www.musicdsp.org/showone.php?id=189
  return (logf(endValue) - logf(startValue)) / (time*sampleRate+10);
}

template<>
const float AdsrEnvelope<true>::MINLEVEL = 0;

template<>
const float AdsrEnvelope<false>::MINLEVEL = 0.00001; // -100dB
