#ifndef __StompBox_h__
#define __StompBox_h__

#include <string>
class PatchProcessor;

enum PatchParameterId {
  PARAMETER_A,
  PARAMETER_B,
  PARAMETER_C,
  PARAMETER_D,
  PARAMETER_E,
  PARAMETER_F
};

enum PatchButtonId {
  BYPASS_BUTTON,
  PUSHBUTTON,
  GREEN_BUTTON,
  RED_BUTTON
};

class FloatArray {
private:
  float* data;
  int sz;
public:
 FloatArray(float* d, int s) :
   data(d), sz(s) {}
  int size(){
    return sz;
  }
  float getMinValue();
  float getMaxValue();
  int getMinIndex();
  int getMaxIndex();
  float getPeakValue();
  int getPeakIndex();
  float getDb();
  void reverse();
  void rectify();
  FloatArray subarray(int offset, int length);
  float& operator [](const int index){
    return data[index];
  }
  operator float*() {
    return data;
  }
};

class AudioBuffer {
public:
  virtual ~AudioBuffer();
  virtual FloatArray getSamples(int channel) = 0;
  // virtual float* getSamples(int channel) = 0;
  virtual int getChannels() = 0;
  virtual int getSize() = 0;
  virtual void clear() = 0;
};

struct ComplexFloat {
  float re;
  float im;
};

class ComplexFloatArray {
private:
  ComplexFloat* data;
  int sz;
public:
  ComplexFloatArray(ComplexFloat* d, int s) :
    data(d), sz(s) {}
  float re(const int i){
    return data[i].re;
  }
  float im(const int i){
    return data[i].im;
  }
  float mag(const int i);
  int size(){
    return sz;
  }
  float getPeakMagnitudeValue();
  int getPeakMagnitudeIndex();
  ComplexFloatArray subarray(int offset, int length);
  void getMagnitudeValues(FloatArray& buf);
  void getRealValues(FloatArray& buf);
  void getImaginaryValues(FloatArray& buf);
  ComplexFloat& operator [](const int i){
    return data[i];
  }
  operator ComplexFloat*() {
    return data;
  }
};

class Patch {
public:
  Patch();
  virtual ~Patch();
  void registerParameter(PatchParameterId pid, const char* name, const char* description = "");
  float getParameterValue(PatchParameterId pid);
  bool isButtonPressed(PatchButtonId bid);
  void setButton(PatchButtonId bid, bool pressed);
  int getBlockSize();
  double getSampleRate();
  AudioBuffer* createMemoryBuffer(int channels, int samples);
  FloatArray createFloatArray(int size);
  ComplexFloatArray createComplexFloatArray(int size);
  float getElapsedBlockTime();
  int getElapsedCycles();
public:
  virtual void processAudio(AudioBuffer& output) = 0;
private:
  PatchProcessor* processor;
  /* uint8_t bufferCount; */
  /* uint16_t parameterValues[NOF_ADC_VALUES]; */
  /* AudioBuffer* buffers[MAX_BUFFERS_PER_PATCH]; */
};

#endif // __StompBox_h__
