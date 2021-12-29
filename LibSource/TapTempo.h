#ifndef __TapTempo_h__
#define __TapTempo_h__

class TapTempo {
protected:
  const float sr;
  const size_t min_limit;
  const size_t max_limit;
  size_t samples;
  size_t trig;
  bool ison;
public:
  TapTempo(float sr, size_t limit) : 
    sr(sr), min_limit(16), max_limit(limit), samples(limit/2), trig(limit), ison(false) {
  }
  TapTempo(float sr, size_t min, size_t max) : 
    sr(sr), min_limit(min), max_limit(max), samples(max/2), trig(max), ison(false) {
  }
  bool isOn(){
    return ison;
  }
  void trigger(bool on){
    trigger(on, 0);
  }
  void trigger(bool on, int delay){
    if(on && !ison){
      if(trig > min_limit && trig < max_limit){
	samples = trig + delay;
      }
      trig = 0;
    }
    ison = on;
  }
  void clock(){
    if(trig < min_limit)
      trig++;
  }
  void clock(size_t steps){
    trig += steps;
    if(trig > max_limit)
      trig = max_limit;
  }
  /**
   * Get tap tempo in BPM.
   */
  float getBeatsPerMinute(){
    return samplePeriodToBpm(samples, sr);
  }
  /**
   * Set tap tempo in BPM.
   */
  void setBeatsPerMinute(float bpm){
    samples = bpmToSamplePeriod(bpm, sr);
  }
  /**
   * Get tap tempo frequency.
   */
  float getFrequency(){
    return sr/samples;
  }
  /**
   * Set tap tempo frequency.
   */
  void setFrequency(float freq){
    samples = sr/freq;
  }
  /**
   * Get tap tempo period in samples.
   */
  size_t getPeriodInSamples(){
    return samples;
  }
  /**
   * Set tap tempo period in samples.
   */
  void setPeriodInSamples(size_t value){
    samples = value;
  }
  static float bpmToFrequency(float bpm){
    return bpm/60;
  }
  static float frequencyToBpm(float freq){
    return freq*60;
  }
  static float frequencyToSamplePeriod(float hz, float sr){
    return sr/hz;
  }
  static float bpmToSamplePeriod(float bpm, float sr){
    return sr/bpmToFrequency(bpm);
  }
  static float samplePeriodToBpm(float samples, float sr){
    return frequencyToBpm(sr/samples);
  }
  static TapTempo* create(float sr, size_t max_limit){
    return new TapTempo(sr, max_limit);
  }
  static TapTempo* create(float sr, size_t min, size_t max){
    return new TapTempo(sr, min, max);
  }
  static void destroy(TapTempo* obj){
    delete obj;
  }    
};

class AdjustableTapTempo : public TapTempo {
protected:
  float speed;
  float range = 4;
  float scale(float s){
    // s = exp2f(range*s - range*0.5f);
    return s*s;
    return s;
    // s = s*s*s*range;
  }
public:
  AdjustableTapTempo(float sr, size_t min, size_t max) :
    TapTempo(sr, min, max), speed(scale(0.5f)) {}
  [[deprecated("use resetSpeed() instead.")]]  
  void resetAdjustment(uint16_t s){
    resetSpeed(s/4096.0f);
  }
  void resetSpeed(float s){
    speed = scale(s);
  }
  void setRange(float value){
    range = value*0.75f;
  }
  [[deprecated("use adjustSpeed() instead.")]]  
  void adjust(uint16_t s){
    adjustSpeed(s/4096.0f);
  }
  /**
   * Adjust the tap tempo period.
   * @param speed should be in the range [0, 1]
   * which corresponds to the configured adjustable range:
   * from bpm/(range/2) to bpm*(range/2)
   * A value of 0.5 corresponds to unity.
   */
  void adjustSpeed(float s){
    s = scale(s);
    float mul = 1 + (speed - s)*range;
    int32_t updated = clamp((size_t)(samples*mul), min_limit, max_limit);
    if(abs((int32_t)samples - updated) > 16){
      samples = updated;
      speed = s;
    }
  }
  static AdjustableTapTempo* create(float sr, size_t limit){
    return new AdjustableTapTempo(sr, 16, limit);
  }
  static AdjustableTapTempo* create(float sr, size_t min, size_t max){
    return new AdjustableTapTempo(sr, min, max);
  }
  static void destroy(AdjustableTapTempo* obj){
    delete obj;
  }    
};

#endif /* __TapTempo_h__ */
