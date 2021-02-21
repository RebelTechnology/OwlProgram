#ifndef NOISE_OSCILLATOR_HPP
#define NOISE_OSCILLATOR_HPP

#include "NoiseGenerator.h"

#warning "NoiseOscillator.h is deprecated, use NoiseGenerator.h instead"

typedef WhiteNoiseGenerator WhiteNoiseOscillator;
typedef PinkNoiseGenerator PinkNoiseOscillator;
typedef BrownNoiseGenerator BrownNoiseOscillator;
typedef GaussianNoiseGenerator GaussianNoiseOscillator;

#endif /* NOISE_OSCILLATOR_HPP */
