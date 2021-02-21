#ifndef __Patch_h__
#define __Patch_h__

#include "device.h"
#include "basicmaths.h"
#include "Resource.h"
#include "FloatArray.h"
#include "PatchParameter.h"
#include "SmoothValue.h"
#include "OpenWareMidiControl.h"

#ifdef USE_MIDI_CALLBACK
#include "MidiMessage.h"
#endif /* USE_MIDI_CALLBACK */

enum PatchChannelId {
  LEFT_CHANNEL = 0,
  RIGHT_CHANNEL = 1
};

class AudioBuffer {
public:
  virtual ~AudioBuffer();
  virtual FloatArray getSamples(int channel) = 0;
  virtual int getChannels() = 0;
  virtual int getSize() = 0;
  virtual void clear() = 0;
  static AudioBuffer* create(int channels, int samples);
};

class Patch {
public:
  Patch();
  virtual ~Patch();
  /** constant skew values for exponential, linear and logarithmic parameters */
  static const float EXP;
  static const float LIN;
  static const float LOG;
  static const uint16_t ON;
  static const uint16_t OFF;
  /* Get a float or int parameter with optional smoothing, hysteresis and exponentiation.
   * The parameter value will be scaled to the range given by min and max.
   * Lambda specifies smoothing factor 0 to 1.0, or 0.0 for no smoothing (default).
   * Delta specifies hysteresis, or stiffness, as the absolute value change required to update the parameter, or 0.0 for no hysteresis (default).
   * Skew specifies exponentiation: > 1.0 for logarithmic, < 1.0 for exponential, or 1.0 for linear scaling (default).
   */
  FloatParameter getParameter(const char* name, float defaultValue);
  FloatParameter getFloatParameter(const char* name, float min, float max, float defaultValue=0.0f, float lambda=0.0f, float delta=0.0, float skew=LIN);
  IntParameter getIntParameter(const char* name, int min, int max, int defaultValue=0, float lambda=0.0f, float delta=0.0, float skew=LIN);
  void registerParameter(PatchParameterId pid, const char* name);
  [[deprecated]]
  void registerParameter(PatchParameterId pid, const char* name, const char* desc){
    registerParameter(pid, name);
  }    
  float getParameterValue(PatchParameterId pid);
  void setParameterValue(PatchParameterId pid, float value);
  bool isButtonPressed(PatchButtonId bid);
  [[deprecated]]
  int getSamplesSinceButtonPressed(PatchButtonId bid);
  void setButton(PatchButtonId bid, uint16_t value, uint16_t samples=0);
  int getBlockSize();
  int getNumberOfChannels();
  float getSampleRate();
  AudioBuffer* createMemoryBuffer(int channels, int samples);
  float getElapsedBlockTime();
  int getElapsedCycles();
  [[deprecated]]
  virtual void encoderChanged(PatchParameterId pid, int16_t delta, uint16_t samples){};
  virtual void buttonChanged(PatchButtonId bid, uint16_t value, uint16_t samples){}
  /* virtual void parameterChanged(PatchParameterId pid, float value, int samples){} */
  virtual void processAudio(AudioBuffer& audio) = 0;
#ifdef USE_MIDI_CALLBACK
  virtual void processMidi(MidiMessage msg);
  virtual void sendMidi(MidiMessage msg);
#endif /* USE_MIDI_CALLBACK */
  /**
   * Get a resource (such as a stored FloatArray) from the firmware.
   * If the resource does not exist, this raises an error.
   * If the resource exists but is not memory mapped, this will allocate new memory.
   */
  Resource* getResource(const char* name);
};

#endif // __Patch_h__
