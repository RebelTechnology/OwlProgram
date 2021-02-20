// Copyright 2015 Olivier Gillet.
//
// Author: Olivier Gillet (ol.gillet@gmail.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
// 
// See http://creativecommons.org/licenses/MIT/ for more information.
//
// -----------------------------------------------------------------------------
//
// VA-style oscillator.

#ifndef STMLIB_DSP_VA_OSCILLATOR_H_
#define STMLIB_DSP_VA_OSCILLATOR_H_

#include <inttypes.h>
#include <stddef.h>

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);               \
  void operator=(const TypeName&)

namespace stmlib {

// The parameters (like frequency, PW) which are provided once per block
// can optionally be linearly interpolated from block to block.
template<bool enable_linear_interpolation>
class ControlRateSource {
 public:
  ControlRateSource(float value, float* state, size_t n) { }
  ~ControlRateSource() { }
  inline float Next() { return 0.0f; }
};

template<>
class ControlRateSource<true> {
 public:
  ControlRateSource(float value, float* state, size_t n) {
    state_ = state;
    value_ = *state;
    const float step = 1.0f / static_cast<float>(n);
    increment_ = (value - value_) * step;
  }
  
  ~ControlRateSource() {
    // Save state for next call.
    *state_ = value_;
  }

  inline float Next() {
    value_ += increment_;
    return value_;
  }

 private:
  float* state_;
  float value_;
  float increment_;
  
  DISALLOW_COPY_AND_ASSIGN(ControlRateSource);
};

// Cheaper, but more zipper noise.
template<>
class ControlRateSource<false> {
 public:
  ControlRateSource(float value, float* state, size_t n) {
    value_ = value;
  }
  
  ~ControlRateSource() { }

  inline float Next() const { return value_; }

 private:
  float value_;

  DISALLOW_COPY_AND_ASSIGN(ControlRateSource);
};

// For audio-rate modulated parameters which are available once per sample.
class AudioRateSource {
 public:
  AudioRateSource(float* source) : source_(source) { }
  inline float Next() { return *source_++; }
 private:
  float* source_;
};

class Oscillator {
 public:
  Oscillator() { }
  ~Oscillator() { }
  
  void Init() {
    master_phase_ = 0.0f;
    slave_phase_ = 0.0f;
    next_sample_ = 0.0f;
    pw_ = 0.5f;
    high_ = false;
    
    previous_master_frequency_ = 0.0f;
    previous_slave_frequency_ = 0.01f;
    previous_pw_ = 0.5f;
    previous_waveshape_ = 0.0f;
  }

  float getMasterPhase(){
    return master_phase_;
  }

  void setMasterPhase(float value){
    master_phase_ = value;
  }

  template<bool interpolate_parameters>
  void Render(
      float* frequency,
      float pw,
      float waveshape,
      float* out,
      size_t size) {
    ControlRateSource<false> master_frequency_source(
        0.0f, &previous_master_frequency_, size);
    AudioRateSource slave_frequency_source(
        frequency);
    ControlRateSource<interpolate_parameters> pw_source(
        pw, &previous_pw_, size);
    ControlRateSource<interpolate_parameters> waveshape_source(
        waveshape, &previous_waveshape_, size);

    Render<false>(
        master_frequency_source,
        slave_frequency_source,
        pw_source,
        waveshape_source,
        out,
        size);
  }
  
  template<bool interpolate_parameters>
  void Render(
      float frequency,
      float pw,
      float waveshape,
      float* out,
      size_t size) {
    ControlRateSource<interpolate_parameters> master_frequency_source(
        0.0f, &previous_master_frequency_, size);
    ControlRateSource<interpolate_parameters> slave_frequency_source(
        frequency, &previous_slave_frequency_, size);
    ControlRateSource<interpolate_parameters> pw_source(
        pw, &previous_pw_, size);
    ControlRateSource<interpolate_parameters> waveshape_source(
        waveshape, &previous_waveshape_, size);

    Render<false>(
        master_frequency_source,
        slave_frequency_source,
        pw_source,
        waveshape_source,
        out,
        size);
  }
  
  template<bool interpolate_parameters>
  void Render(
      float master_frequency,
      float frequency,
      float pw,
      float waveshape,
      float* out,
      size_t size) {
    ControlRateSource<interpolate_parameters> master_frequency_source(
        master_frequency, &previous_master_frequency_, size);
    ControlRateSource<interpolate_parameters> slave_frequency_source(
        frequency, &previous_slave_frequency_, size);
    ControlRateSource<interpolate_parameters> pw_source(
        pw, &previous_pw_, size);
    ControlRateSource<interpolate_parameters> waveshape_source(
        waveshape, &previous_waveshape_, size);

    Render<true>(
        master_frequency_source,
        slave_frequency_source,
        pw_source,
        waveshape_source,
        out,
        size);
  }

 private:
  inline float ComputeNaiveSample(
      float phase,
      float pw,
      float waveshape) const {
    float square = phase < pw ? 0.0f : 1.0f;
    float saw = phase;
    return saw + (square - saw) * waveshape;
  }

  static inline float ThisBlepSample(float t) {
    return 0.5f * t * t;
  }
  static inline float NextBlepSample(float t) {
    t = 1.0f - t;
    return -0.5f * t * t;
  }
  
  template<
      bool slaved,
      typename MasterFrequencySource,
      typename SlaveFrequencySource,
      typename PWSource,
      typename WaveshapeSource>
  void Render(
      MasterFrequencySource& master_frequency_source,
      SlaveFrequencySource& slave_frequency_source,
      PWSource& pw_source,
      WaveshapeSource& waveshape_source,
      float* out,
      size_t size) {
    float next_sample = next_sample_;
    while (size--) {
      bool reset = false;
      bool transition_during_reset = false;
      float reset_time = 0.0f;

      float this_sample = next_sample;
      next_sample = 0.0f;
      
      float slave_frequency = slave_frequency_source.Next();
      float pw = pw_source.Next();
      float waveshape = waveshape_source.Next();
      
      if (slaved) {
        float master_frequency = master_frequency_source.Next();
        master_phase_ += master_frequency;
        if (master_phase_ >= 1.0f) {
          master_phase_ -= 1.0f;
          reset_time = master_phase_ / master_frequency;
        
          float slave_phase_at_reset = slave_phase_ + \
              (1.0f - reset_time) * slave_frequency;
          reset = true;
          if (slave_phase_at_reset >= 1.0f) {
            slave_phase_at_reset -= 1.0f;
            transition_during_reset = true;
          }
          if (!high_ && slave_phase_at_reset >= pw) {
            transition_during_reset = true;
          }
          float value = ComputeNaiveSample(slave_phase_at_reset, pw, waveshape);
          this_sample -= value * ThisBlepSample(reset_time);
          next_sample -= value * NextBlepSample(reset_time);
        }
      }

      slave_phase_ += slave_frequency;
      while (transition_during_reset || !reset) {
        if (!high_) {
          if (slave_phase_ < pw) {
            break;
          }
          float t = (slave_phase_ - pw) / (pw_ - pw + slave_frequency);
          this_sample += waveshape * ThisBlepSample(t);
          next_sample += waveshape * NextBlepSample(t);
          high_ = true;
        }
        
        if (high_) {
          if (slave_phase_ < 1.0f) {
            break;
          }
          slave_phase_ -= 1.0f;
          float t = slave_phase_ / slave_frequency;
          this_sample -= ThisBlepSample(t);
          next_sample -= NextBlepSample(t);
          high_ = false;
        }
      }
      
      if (slaved && reset) {
        slave_phase_ = reset_time * slave_frequency;
        high_ = false;
      }
      
      next_sample += ComputeNaiveSample(slave_phase_, pw, waveshape);
      pw_ = pw;

      *out++ = (2.0f * this_sample - 1.0f);
    }
    next_sample_ = next_sample;
  }

  // Oscillator state.
  float master_phase_;
  float slave_phase_;
  float next_sample_;
  float pw_;
  bool high_;
  
  // For interpolation of control-rate parameters.
  float previous_master_frequency_;
  float previous_slave_frequency_;
  float previous_pw_;
  float previous_waveshape_;
};

}  // namespace stmlib

#endif  // STMLIB_DSP_VA_OSCILLATOR_H_
