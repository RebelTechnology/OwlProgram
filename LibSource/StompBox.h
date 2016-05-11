#ifndef __StompBox_h__
#define __StompBox_h__

#include "basicmaths.h"
#include "FloatArray.h"
#include "PatchParameter.h"
#include "SmoothValue.h"

enum PatchParameterId {
  PARAMETER_A,
  PARAMETER_B,
  PARAMETER_C,
  PARAMETER_D,
  PARAMETER_E,
  PARAMETER_F
};

enum PatchButtonId {
  BYPASS_BUTTON = 0,
  PUSHBUTTON,
  GREEN_BUTTON,
  RED_BUTTON
};

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
  /* Get a float or int parameter with optional smoothing, hysteresis and exponentiation.
   * The parameter value will be scaled to the range given by min and max.
   * Lambda specifies smoothing factor 0 to 1.0, or 0.0 for no smoothing (default).
   * Delta specifies hysteresis, or stiffness, as the absolute value change required to update the parameter, or 0.0 for no hysteresis (default).
   * Skew specifies exponentiation: < 1.0 for logarithmic, > 1.0 for exponential, or 1.0 for linear scaling (default).
   */
  FloatParameter getFloatParameter(const char* name, float min, float max, float defaultValue=0.0f, float lambda=0.0f, float delta=0.0, float skew=LIN);
  IntParameter getIntParameter(const char* name, int min, int max, int defaultValue=0, float lambda=0.0f, float delta=0.0, float skew=LIN);
  void registerParameter(PatchParameterId pid, const char* name);
  float getParameterValue(PatchParameterId pid);
  void setParameterValue(PatchParameterId pid, float value);
  bool isButtonPressed(PatchButtonId bid);
  int getSamplesSinceButtonPressed(PatchButtonId bid);
  void setButton(PatchButtonId bid, bool pressed);
  int getBlockSize();
  double getSampleRate();
  AudioBuffer* createMemoryBuffer(int channels, int samples);
  float getElapsedBlockTime();
  int getElapsedCycles();
  virtual void processAudio(AudioBuffer& output) = 0;
  /** constant skew values for exponential, linear and logarithmic parameters */
  static const float EXP;
  static const float LIN;
  static const float LOG;
};

#endif // __StompBox_h__
