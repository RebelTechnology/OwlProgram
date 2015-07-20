#ifndef __PitchDetectors_hpp__
#define __PitchDetectors_hpp__

#include "FloatArray.h"

class ZeroCrossingPitchDetector{
private:
  BiquadFilter *filter;
  int numLowPassStages;
  int numHighPassStages;
  FloatArray counts;
  FloatArray filterOutput;
  float samplingRate;
public:
  ZeroCrossingPitchDetector(): samplingRate(48000), numLowPassStages(1), numHighPassStages(0){
    init(0);
  };
  ZeroCrossingPitchDetector(float aSamplingRate, int aMaxBlocksize):numLowPassStages(1), numHighPassStages(0){
    samplingRate=aSamplingRate;
    init(aMaxBlocksize);
  };
  ZeroCrossingPitchDetector(float aSamplingRate, int aMaxBlocksize, int aNumLowPassStages, int aNumHighPassStages){
    samplingRate=aSamplingRate;
    init(aMaxBlocksize);
  }
  ~ZeroCrossingPitchDetector(){
    FloatArray::destroy(counts);
    FloatArray::destroy(filterOutput);
    BiquadFilter::destroy(filter);
  }
  void init(int aMaxBlocksize){
    setMaxBlocksize(aMaxBlocksize);
    counts=FloatArray::create(10); //number of zcc to be averaged
    counts.setAll(0);
    filter=BiquadFilter::create(numLowPassStages+numHighPassStages);
    setLowPassCutoff(0.03);
    setHighPassCutoff(0.001);
  };
  void setSamplingRate(float aSamplingRate){
    samplingRate=aSamplingRate;
  }
  void setMaxBlocksize(int aMaxBlocksize){
    FloatArray::destroy(filterOutput);
    filterOutput=FloatArray::create(aMaxBlocksize);
  }
  void setLowPassCutoff(float fc){
    if(numLowPassStages<1)
      return;
    FilterStage stage0=filter->getFilterStage(0);
    stage0.setLowPass(fc/samplingRate, FilterStage::BUTTERWORTH_Q);
    for(int n=1; n<numLowPassStages; n++){
      FilterStage stage=filter->getFilterStage(n);
      stage.setCoefficients(FloatArray(stage0.getCoefficients(), 5));
    }
  };
  void setHighPassCutoff(float fc){
    if(numHighPassStages<1)
      return;
    FilterStage stage0=filter->getFilterStage(numLowPassStages);
    stage0.setHighPass(fc/samplingRate, FilterStage::BUTTERWORTH_Q);
    for(int n=numLowPassStages+1; n<numHighPassStages+numLowPassStages; n++){
      FilterStage stage=filter->getFilterStage(n);
      stage.setCoefficients(stage0.getCoefficients());
    }
  };
  void process(FloatArray input){
    ASSERT(input.getSize()<=filterOutput.getSize(), "wrong size");
    static float lastValue=0;
    static int countsPointer=0;
    static float period=0;
    static float count;
    filter->process(input, filterOutput);
    // filterOutput.copyTo(input);
    for(int n=0; n<input.getSize(); n++){
      float currentValue=filterOutput[n];
      if(currentValue>0 && lastValue<=0){
        /*
         counts[countsPointer]=count; //Could use nearest neighbour, but
         count=0;
        */
         //linear interpolation gives a better estimate of the zero crossing time: 
        float offset=(-lastValue)/(lastValue+currentValue);
        counts[countsPointer]=count+offset;
        count=-offset;
        countsPointer++;
        if(countsPointer==counts.getSize()) //use counts as a circular buffer
          countsPointer=0;
      }
      lastValue=currentValue;
      count++;
    }
  }

  float getFrequency(){
    return samplingRate/counts.getMean();
  }
  BiquadFilter* getFilter(){
    return filter;
  }
};

#endif /* __PitchDetectors_hpp__ */