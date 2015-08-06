#ifndef __SuperSaw_h__
#define __SuperSaw_h__


/**
 * This class is a SuperSaw wave oscillator using multiple free-running non-bandlimited sawtooth oscillators (7 by default).
 *
 * Controls for mix and detune are provided.
 * 
 * Note: these controls are linear, unlike in the Roland JP-8000.
 * 
 * Main reference is <a href="https://www.nada.kth.se/utbildning/grukth/exjobb/rapportlistor/2010/rapporter10/szabo_adam_10131.pdf">https://www.nada.kth.se/utbildning/grukth/exjobb/rapportlistor/2010/rapporter10/szabo_adam_10131.pdf</a>
 */

class SuperSaw{
private:
  enum lastCalls {
    kNone,
    kSetDetune,
    kSetFrequency,
    kUpdatePhaseIncrements,
  };
  int lastCall=kNone;
  float samplePeriod;
  float detune;
  FloatArray frequencyRatios;
  FloatArray phaseIncrements;
  FloatArray amplitudes;
  FloatArray phases;
  BiquadFilter *filter=NULL;
  float frequency;
  int numberOfOscillators;
  bool filterBypass;
void init(){
    frequency=0;
    numberOfOscillators=7;
    filterBypass=false;
    allocate();
    setDetune(0);
    setMix(0);
  }
  
  void destroy(){
    FloatArray::destroy(frequencyRatios);
    FloatArray::destroy(phaseIncrements);
    FloatArray::destroy(amplitudes);
    FloatArray::destroy(phases);
    if(filter!=NULL){
      BiquadFilter::destroy(filter);
      filter=NULL;
    }
  }
  
  void allocate(){
    destroy();
    int numArrays=4;
    frequencyRatios=FloatArray::create(numberOfOscillators);
    frequencyRatios.clear();
    phaseIncrements=FloatArray::create(numberOfOscillators);
    phaseIncrements.clear();
    amplitudes=FloatArray::create(numberOfOscillators);
    amplitudes.clear();
    phases=FloatArray::create(numberOfOscillators);
    phases.clear();
    float filterStages=2;
    filter=BiquadFilter::create(filterStages);
  }
  
  void setFilter(float aFrequency){
    float cutoff=aFrequency*samplePeriod*0.5;
    filter->setHighPass(cutoff, FilterStage::BUTTERWORTH_Q);
  }
  
  void updatePhaseIncrements(){
    lastCall=kUpdatePhaseIncrements;
    for(int n=0; n<numberOfOscillators; n++){
      phaseIncrements[n]=frequencyRatios[n]*frequency*samplePeriod * 2;// the *2 multiplier takes into account that the oscillator range is -1 to 1
    }
  }
public:
  /**
   * Default constructor.
   * @remarks Before using the oscillator you will need to initialize the sample rate with setSampleRate().
  */
  SuperSaw(){
    init();
  }
  /**
   * Constructor.
   * @param[in] aSampleRate the sample rate of the system
  */
  SuperSaw(float aSampleRate){
    setSampleRate(aSampleRate);
    init();
  }
  /** Destructor
  */
  ~SuperSaw(){
    destroy();
  }

  /**
    * Set the sample rate
    * @param[in] aSampleRate the sampling rate of the system
  */ 
  void setSampleRate(float aSampleRate){
    samplePeriod=1/aSampleRate;
  }

  /**
    * Set the number of oscillators
    * @param[in] aNumberOfOscillators the number of oscillators.
    * @remark This method allocates memory. You should avoid calling it while processing audio.
  */ 
  void setNumOscillators(int aNumberOfOscillators){
    if(numberOfOscillators==aNumberOfOscillators){ //nothing to do
      return;
    }
    numberOfOscillators=aNumberOfOscillators;
    allocate();
  }

  /**
    * Set the frequency.
    * @param[in] aFrequency The frequency of the oscillator.
  */ 
  void setFrequency(float aFrequency){ 
    lastCall=kSetFrequency;
    frequency=aFrequency;
    setFilter(aFrequency);
  }

  /**
    * Set the mix.
    * Set the mix parametem that is the mix between the oscillator at the base frequency and the detuned oscillators.
    * @param[in] aMix the mix parameter. Values will be clipped in the range 0-1
  */ 
  void setMix(float aMix){
    if(aMix>1) {
      aMix=1;
    } else if (aMix<0) {
      aMix=0;
    }
    float norm=1.0f/(numberOfOscillators-1);
    float detunedAmplitude=aMix*norm;
    for(int n=0; n<numberOfOscillators; n++){
      amplitudes[n]=detunedAmplitude;
    }
    amplitudes[numberOfOscillators/2]=1-aMix;
  }

  /**
    * Set the detune.
    * @param[in] aDetune the detune parameter.
  */ 
  void setDetune(float aDetune){
    lastCall=kSetDetune;
    for(int n=0; n<numberOfOscillators; n++){
      float spread=(n-(int)(numberOfOscillators/2))/(float)numberOfOscillators; //this will be ==0 for the middle value of n, that is the middle oscillator will always be in tune
      float frequencyRatio=1+aDetune*spread; 
      frequencyRatios[n]=1/frequencyRatio;
    }
  }

  /**
    * Get the oscillator samples
    * @param[out] output The array where the output is to be written
  */
  void getSamples(FloatArray& output){
    getSamples((float*)output, output.getSize());
  }

  /*
  * Bypasses the built-in high pass filter.
  * Sets the state of the built-in high pass filter tuned at the fundamental frequency, which was used
  * in the original Roland design to attenuate aliasing below the fundamental frequency.
  * @param bypass If set to *true* the filter will be bypassed, if set to *false* the filter will be applied (default).
  */
  void setFilterBypass(bool bypass){
    filterBypass=bypass;
  }
  /**
    * Get the oscillator samples
    * @param[out] output Pointer to a location in memory where the output is to be written
    * @param[in] size Number of samples to be written.
  */
  void getSamples(float* output, int size){
    if(lastCall!=kUpdatePhaseIncrements){ //if the frequencies have not been updated after setDetune or setFrequency
      updatePhaseIncrements();  //this ensures that updatePhaseIncrements gets called at most once when frequency and/or detune are set 
                                //and regardless of the order in which they have been called.
    }
    for (int i=0; i<size; i++){
      output[i]=0;
      for(int n=0; n<numberOfOscillators; n++){
        output[i]+=phases[n]*amplitudes[n];
        phases[n]+=phaseIncrements[n];
        if(phases[n]>=1)
          phases[n]-=2; // 2 reflects the fact that the oscillator range is -1 to 1
      }
    }
    if(filterBypass==false){
      // filter->process(output, output, size); //highpass filter
    }
  }
};
#endif // __SuperSaw_h__
