#ifndef __Patch_h__
#define __Patch_h__

#include "device.h"
#include "basicmaths.h"
#include "FloatArray.h"
#include "PatchParameter.h"
#include "SmoothValue.h"
#ifdef USE_SCREEN
#include "ScreenBuffer.h"
#endif /* USE_SCREEN */

#ifdef USE_MIDI_CALLBACK
#include "MidiStatus.h"
class MidiMessage {
 public:
  uint8_t data[3];
  uint8_t getPort(){
    return (data[0] & 0xf0)>>4;
  }
  uint8_t getChannel(){
    return (data[1] & MIDI_CHANNEL_MASK);
  }
  uint8_t getStatus(){
    return (data[1] & MIDI_STATUS_MASK);
  }
  uint8_t getNote(){
    return data[2];
  }
  uint8_t getVelocity(){
    return data[3];
  }
  uint8_t getControllerNumber(){
    return data[2];
  }
  uint8_t getControllerValue(){
    return data[3];
  }
  int16_t getPitchBend(){
    int16_t pb = (data[2] | (data[3]<<7)) - 8192;
    return pb;
  }
  bool isNoteOn(){
    return (data[1] & MIDI_STATUS_MASK) == NOTE_ON && getVelocity() != 0;
  }
  bool isNoteOff(){
    return (data[1] & MIDI_STATUS_MASK) == NOTE_OFF || (isNoteOn() && getVelocity() == 0);
  }
  bool isControlChange(){
    return (data[1] & MIDI_STATUS_MASK) == CONTROL_CHANGE;
  }
  bool isProgramChange(){
    return (data[1] & MIDI_STATUS_MASK) == PROGRAM_CHANGE;
  }
  bool isPitchBend(){
    return (data[1] & MIDI_STATUS_MASK) == PITCH_BEND_CHANGE;
  }
};
#endif /* USE_MIDI_CALLBACK */

enum PatchParameterId {
  PARAMETER_A,
  PARAMETER_B,
  PARAMETER_C,
  PARAMETER_D,
  PARAMETER_E,
  PARAMETER_F,
  PARAMETER_G,
  PARAMETER_H,

  PARAMETER_AA,
  PARAMETER_AB,
  PARAMETER_AC,
  PARAMETER_AD,
  PARAMETER_AE,
  PARAMETER_AF,
  PARAMETER_AG,
  PARAMETER_AH,

  PARAMETER_BA,
  PARAMETER_BB,
  PARAMETER_BC,
  PARAMETER_BD,
  PARAMETER_BE,
  PARAMETER_BF,
  PARAMETER_BG,
  PARAMETER_BH,

  PARAMETER_CA,
  PARAMETER_CB,
  PARAMETER_CC,
  PARAMETER_CD,
  PARAMETER_CE,
  PARAMETER_CF,
  PARAMETER_CG,
  PARAMETER_CH,

  PARAMETER_DA,
  PARAMETER_DB,
  PARAMETER_DC,
  PARAMETER_DD,
  PARAMETER_DE,
  PARAMETER_DF,
  PARAMETER_DG,
  PARAMETER_DH,

  PARAMETER_FREQ,
  PARAMETER_GAIN
};

enum PatchButtonId {
  BYPASS_BUTTON,
  PUSHBUTTON,
  GREEN_BUTTON,
  RED_BUTTON,
  GATE_BUTTON,
  BUTTON_A,
  BUTTON_B,
  BUTTON_C,
  BUTTON_D,
  MIDI_NOTE_BUTTON = 0x80 // values over 127 are mapped to note numbers
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
  float getParameterValue(PatchParameterId pid);
  void setParameterValue(PatchParameterId pid, float value);
  bool isButtonPressed(PatchButtonId bid);
  /** @deprecated */
  int getSamplesSinceButtonPressed(PatchButtonId bid);
  void setButton(PatchButtonId bid, uint16_t value, uint16_t samples=0);
  int getBlockSize();
  float getSampleRate();
  AudioBuffer* createMemoryBuffer(int channels, int samples);
  float getElapsedBlockTime();
  int getElapsedCycles();
  virtual void encoderChanged(PatchParameterId pid, int16_t delta, uint16_t samples){};
  virtual void buttonChanged(PatchButtonId bid, uint16_t value, uint16_t samples){}
  /* virtual void parameterChanged(PatchParameterId pid, float value, int samples){} */
  virtual void processAudio(AudioBuffer& audio) = 0;
#ifdef USE_SCREEN
  uint16_t getScreenWidth();
  uint16_t getScreenHeight();
  virtual void processScreen(ScreenBuffer& screen);
#endif /* USE_SCREEN */
#ifdef USE_MIDI_CALLBACK
  virtual void processMidi(MidiMessage& msg);
#endif /* USE_MIDI_CALLBACK */
};

#endif // __Patch_h__
