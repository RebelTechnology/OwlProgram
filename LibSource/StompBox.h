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
  FloatParameter getParameter(const char* name, float min, float max, float defaultValue=0.0f, float lambda=0.0f, float delta=0.0, float skew=1.0);
  IntParameter getParameter(const char* name, int min, int max, int defaultValue=0, float lambda=0.0f, float delta=0.0, float skew=1.0);
  void registerParameter(PatchParameterId pid, const char* name);
  float getParameterValue(PatchParameterId pid);
  bool isButtonPressed(PatchButtonId bid);
  int getSamplesSinceButtonPressed(PatchButtonId bid);
  void setButton(PatchButtonId bid, bool pressed);
  int getBlockSize();
  double getSampleRate();
  AudioBuffer* createMemoryBuffer(int channels, int samples);
  float getElapsedBlockTime();
  int getElapsedCycles();
  virtual void processAudio(AudioBuffer& output) = 0;
};

#endif // __StompBox_h__
