#ifndef __FastFourierTestPatch_hpp__
#define __FastFourierTestPatch_hpp__

#include "StompBox.h"

class FastFourierTransform {
private:
  arm_rfft_fast_instance_f32 instance;

public:
  void init(int len){
    arm_rfft_fast_init_f32(&instance, len);
// Supported FFT Lengths are 32, 64, 128, 256, 512, 1024, 2048, 4096.
  }
  void fft(float* in, float* out){
    arm_rfft_fast_f32(&instance, in, out, 0);
  }
  void ifft(float* in, float* out){
    arm_rfft_fast_f32(&instance, in, out, 1);
  }
};

class FastFourierTestPatch : public Patch {
private:
  FastFourierTransform transform;
  float *fd;
  AudioBuffer *tdBuffersIn;
  AudioBuffer *tdBuffersOut;
  int fftSize;
  int numBuffers;
  int countIn;
  int nextInBuffer;
  int nextOutBuffer;
  int inPointers[8];
  int outPointers[8];
  float windowGain;
  float *window;
  void resetPointers(){
    countIn=0;
    for(int n=0; n<numBuffers; n++){
      inPointers[n]=-1;
      outPointers[n]=-1;
    }
    inPointers[0]=0; //activate the first buffer, to make sure we start storing samples immediately
    nextInBuffer=1;
    nextOutBuffer=0;
  }
public:
  FastFourierTestPatch(){
    fftSize=512;
    transform.init(fftSize);
    numBuffers=8;
    tdBuffersIn=createMemoryBuffer(numBuffers, fftSize);
    tdBuffersOut=createMemoryBuffer(numBuffers, fftSize);
    window=createMemoryBuffer(1, fftSize)->getSamples(0);
    for(int n=0; n<fftSize/2; n++){
      window[n]=n/(fftSize/2.0);
    }
    windowGain=2; //gain to compensate for the window's gain loss
    for(int n=fftSize/2; n<fftSize; n++){
      window[n]=(fftSize-n)/(fftSize/2.0); //triangular window
      // window[n]=1;//rectangular window
    }
    fd=createMemoryBuffer(1, fftSize)->getSamples(0);
    registerParameter(PARAMETER_A, "Gain");
    registerParameter(PARAMETER_B, "Mode");
    registerParameter(PARAMETER_C, "nBins");
    registerParameter(PARAMETER_D, "Step");
  }
  void processAudio(AudioBuffer &buffer){
    float gain = getParameterValue(PARAMETER_A);
    float mode = getParameterValue(PARAMETER_B);
    float nBins = getParameterValue(PARAMETER_C);
    static int previousStep=0;
    int step = (int)(getParameterValue(PARAMETER_D)*1000+0.5)+fftSize/(float)numBuffers; //set a minimum value to avoid buffer underruns
    if(step!=previousStep){ //prevent weird things when step is changed. ADC noise should be covered by the *1000 and rounding above
      resetPointers();   
    }
    previousStep=step;
    float* left = buffer.getSamples(0);
    float* right = buffer.getSamples(1);
    int size=getBlockSize();
    for(int n=0; n<size; n++){
      float out=0;
      for(int k=0; k<numBuffers; k++){ //overlap and add loop!
        if(nextInBuffer==k && countIn==step){ //start a new in buffer in the current block
          nextInBuffer=nextInBuffer+1>=numBuffers?0:nextInBuffer+1;//increment the nextInBuffer
          countIn=0;
          inPointers[k]=0; //activate the current buffer
        } else if(inPointers[k]==fftSize){//buffer full, let's do the transform
          inPointers[k]=-1;//deactivate the current buffer;
          transform.fft(tdBuffersIn->getSamples(k), fd);
          for(int j=0; j<fftSize/2*nBins; j++){
            float ab=sqrtf(fd[j]*fd[j]+fd[j+1]*fd[j+1]); //modulus
            if(mode<0.33){//robotization
              fd[j]=ab;
              fd[j+1]=0;
            }
            if(mode >=0.33 && mode<0.66){ //whisperization1: random phase
              float phase=rand()/((float)RAND_MAX)*2*M_PI;
              fd[j]=ab*cosf(phase);
              fd[j+1]=ab*sinf(phase);
            }
            if(mode >= 0.66){ //whisperization2: pseudorandom modulus
              float phase=atan2(fd[j],fd[j+1]);
              ab*=rand()/((float)RAND_MAX);
              fd[j]=ab*cosf(phase);
              fd[j+1]=ab*sinf(phase);
            }
          }
          transform.ifft(fd, tdBuffersOut->getSamples(nextOutBuffer)); //save the ifft in the outBuffer
          outPointers[nextOutBuffer]=0; //activate the buffer
          nextOutBuffer=nextOutBuffer+1>=numBuffers?0:nextOutBuffer+1; //increment the next buffer
        }
        if(inPointers[k]!=-1){ //if the buffer is active, keep filling it
          tdBuffersIn->getSamples(k)[inPointers[k]]=left[n]*window[inPointers[k]];
          inPointers[k]++;
        }
        if (outPointers[k]==fftSize){ //end of the buffer, disable the buffer
          outPointers[k]=-1;
        }
        if(outPointers[k]!=-1){ //if the buffer is active, read from it 
          out+=tdBuffersOut->getSamples(k)[outPointers[k]]*window[outPointers[k]];
          outPointers[k]++;
        }
      }
      left[n]=left[n]*(1-gain)+out*windowGain*gain;
      right[n]=left[n];
      countIn++;
    }
  }
};

#endif // __FastFourierTestPatch_hpp__
