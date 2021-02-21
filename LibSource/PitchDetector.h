#ifndef __PitchDetector_h__
#define __PitchDetector_h__

#include "FloatArray.h"
#include "ComplexFloatArray.h"
#include "FastFourierTransform.h"
#include "BiquadFilter.h"
#include "Window.h"

class FourierPitchDetector{
private:
  FastFourierTransform fft;
  float samplingRate;
  float minBin;
  float maxBin;
  float binSize;
  float frequency;
  ComplexFloatArray fd;
  FloatArray magnitudes;
  FloatArray timeDomain;
  size_t writePointer;
  Window window;
public:
  FourierPitchDetector(){
    
  };
  FourierPitchDetector(int fftSize, float aSamplingRate){
    init(fftSize, aSamplingRate);
  };
  ~FourierPitchDetector(){
    ComplexFloatArray::destroy(fd);
    FloatArray::destroy(magnitudes);
    FloatArray::destroy(timeDomain);
    Window::destroy(window);
  }
  void init(int fftSize, float aSamplingRate){
    samplingRate=aSamplingRate;
    frequency=0;
    writePointer=0;
    fft.init(fftSize);
    binSize=samplingRate/fftSize;
    fd=ComplexFloatArray::create(fftSize);
    magnitudes=FloatArray::create(fftSize);
    timeDomain=FloatArray::create(fftSize);
    window=Window::create(Window::HannWindow, fftSize);
  }
  size_t getSize(){
    return fft.getSize();
  }
  void setSamplingRate(float asamplingRate){
    samplingRate=asamplingRate;
  }
  void setMinFrequency(float aMinFrequency){
    minBin=(int)(aMinFrequency/binSize);
  }
  void setMaxFrequency(float aMaxFrequency){
    maxBin=(int)(aMaxFrequency/binSize+1);
  }
  int process(FloatArray input){ //return values: 0 no fft performed, 1 fft performed
    ASSERT(input.getSize()<=fd.getSize(), "wrong size");
    if(input.getSize()==fft.getSize()){ //if the two sizes match, no need to copy the input into the local timeDomain FloatArray
      input.multiply(window, timeDomain);
      fft.fft(timeDomain, fd);
      return 1;
    }
    // otherwise keep filling the input buffer TODO: could multiply by the window while copying
    size_t samplesToCopy=min(input.getSize(),timeDomain.getSize()-writePointer);
    timeDomain.insert(input, 0, writePointer, samplesToCopy);
    writePointer+=samplesToCopy;
    if(writePointer==fft.getSize()){ // if it is full, reset pointer and do fft 
      writePointer=0;
      timeDomain.multiply(window);
      fft.fft(timeDomain, fd);
      return 1;
    }
    return 0;
  }
  float computeFrequency(){// this could have been implemented into process().
    //The reason why it is in a separate method is to allow to distribute the computational load across different audio blocks
    ComplexFloatArray fdsub=fd.subArray((int)minBin, (int)maxBin-(int)minBin);
    FloatArray magnitudesSub=magnitudes.subArray((int)minBin, (int)maxBin-(int)minBin);
    fdsub.getMagnitudeSquaredValues(magnitudesSub);
    size_t maxIndex=magnitudesSub.getMaxIndex();
    //do quadratic interpolation https://ccrma.stanford.edu/~jos/sasp/Quadratic_Interpolation_Spectral_Peaks.html
    //this single iteration method gives the following accuracy using as input a sinewave in the range 100Hz-200Hz
    //fftSize=512; max(abs(error))=15Hz well, with this fftSize, the binsize is 93.75 Hz, with a 100Hz input
                                    //the peak is in the first quarter of the second bin and the estimate cannot be precise!
                                    // performance improve drastically above 140Hz (error <2Hz)
    //fftSize=1024; max(abs(error))=0.7495Hz
    //fftSize=2048; max(abs(error))=0.37531Hz
    //fftSize=4096; max(abs(error))=0.18746Hz
    if(maxIndex==0||maxIndex==magnitudesSub.getSize()-1) { //value out of range
      frequency=0;  //TODO: what to do if value is out of range?
      return getFrequency();
    }
    //note that we are working on the values in Bel (10*dB).
    //Using the logarithmic scale gives better accuracy of the peak estimate
    //in this application
    float alpha=log10(magnitudesSub[maxIndex-1]);
    float beta=log10(magnitudesSub[maxIndex]);
    float gamma=log10(magnitudesSub[maxIndex+1]);
    float p=0.5*(alpha-gamma)/(alpha-2*beta+gamma);
    // ASSERT(p>=-0.5 && p<=0.5, "Wrong range for p"); 
    int bin=maxIndex+minBin;
    frequency=(bin+p)*binSize; 
    return getFrequency();
  }
  float getFrequency(){
    return frequency;
  }
};

class ZeroCrossingPitchDetector{
private:
  float samplingRate;
  int numLowPassStages;
  int numHighPassStages;
  BiquadFilter *filter;
  FloatArray counts;
  FloatArray filterOutput;
  const static int POINTS_AVERAGE = 10;
public:
  ZeroCrossingPitchDetector(float aSamplingRate, int blocksize, int aNumLowPassStages=1, int aNumHighPassStages=1) :
    samplingRate(aSamplingRate),
    numLowPassStages(aNumLowPassStages),
    numHighPassStages(aNumHighPassStages) {
    // RAII constructor
    filterOutput = FloatArray::create(blocksize);
    counts = FloatArray::create(POINTS_AVERAGE); //number of zcc to be averaged
    filter = BiquadFilter::create(aSamplingRate, numLowPassStages+numHighPassStages);
    setLowPassCutoff(0.03);
    setHighPassCutoff(0.001);
  }
  ~ZeroCrossingPitchDetector(){
    FloatArray::destroy(counts);
    FloatArray::destroy(filterOutput);
    BiquadFilter::destroy(filter);
  }
  void setSamplingRate(float aSamplingRate){
    samplingRate = aSamplingRate;
  }
  void setLowPassCutoff(float fc){
    if(numLowPassStages<1)
      return;
    filter->setLowPass(fc, FilterStage::BUTTERWORTH_Q);
  };
  void setHighPassCutoff(float fc){
    if(numHighPassStages<1)
      return;
    filter->setHighPass(fc, FilterStage::BUTTERWORTH_Q);
  };
  void process(FloatArray input){
    ASSERT(input.getSize()<=filterOutput.getSize(), "wrong size");
    static float lastValue=0;
    static size_t countsPointer=0;
    static float count;
    filter->process(input, filterOutput);
    // filterOutput.copyTo(input);
    for(size_t n=0; n<input.getSize(); n++){
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
    float mean = counts.getMean();
    if(mean > 0.0) // avoid divide by zero
      return samplingRate/mean;
    return 0.0;
  }
  BiquadFilter* getFilter(){
    return filter;
  }
};

#endif /* __PitchDetector_h__ */
