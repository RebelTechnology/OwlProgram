#ifndef __KissFftPatch_hpp__
#define __KissFftPatch_hpp__

#include "../kiss_fft130/kiss_fft.h"
#include "StompBox.h"

class Fft{
  kiss_fft_cfg cfgFft;
  kiss_fft_cfg cfgIfft;
  bool initialised;
  kiss_fft_cpx* tdBuffer;
  kiss_fft_cpx* fdBuffer;
  int length;
  float *windowTable;
  int windowType;
  public:
    enum windowDirections {kForwardPositive, kForwardNegative, kBackwardPositive, kBackwardNegative};
    enum windowTypes {
      kHanningWindow, //uses table lookup
      kTriangularWindow, //is generated algorithmically
      kRectangularWindow //"no window"
    };
    Fft(){
      tdBuffer=NULL;
      fdBuffer=NULL;
      windowTable=NULL;
      initialised=false;
    }
    ~Fft(){
      free(tdBuffer);
      free(fdBuffer);
      free(windowTable);
    }
    void init(int aLength, int aWindowType){
      free(tdBuffer);
      free(fdBuffer);
      free(windowTable);
      length=aLength;
      windowType=aWindowType;
      cfgFft = kiss_fft_alloc(length,0,0,0);
      cfgIfft = kiss_fft_alloc(length,1,0,0);
      tdBuffer = (kiss_fft_cpx*)malloc(length*sizeof(kiss_fft_cpx)*2); 
      fdBuffer = (kiss_fft_cpx*)malloc(length*sizeof(kiss_fft_cpx)*2); 
      if(tdBuffer==NULL || fdBuffer==NULL)
        return;
      switch(windowType){
        case kHanningWindow:{ // {} tolimit the scope of the tableLength variable
          //we generate the lookuptable, assuming length is a multiple of 2 
          float tableLength=length/2; //length of the lookup table: being the Hann window symmetric, we store only half of it
          windowTable = (float *)malloc(tableLength*sizeof(float));
          if(windowTable==NULL)
            return;
          for(int n=0; n<tableLength; n++){
            windowTable[n]=0.5*(1-cosf(n/(length-1)*2*M_PI));
          }
          break;
        }
        case kTriangularWindow:
          break;
        case kRectangularWindow:
          break;
      }
      initialised=true;
    }
    int fft(float *input){
      if(!initialised)
        return -1;
      setImag(tdBuffer,0.0f,length); //initialise the imaginary part of the buffer to zero
      applyWindow(tdBuffer, input, windowType); 
      kiss_fft(cfgFft, tdBuffer, fdBuffer);
      return 1;
    }
    int ifft(float *output){
      if(!initialised)
        return -1;
      kiss_fft(cfgIfft, fdBuffer, tdBuffer);
      getReal(tdBuffer, output, length, 1.0/length); //only return the real part, scaled by length
      return 1;
    }
    void applyWindow(kiss_fft_cpx* cpx, float * input, int windowType){ //applies a window to a real signal and stores it in the real part of cpx
      switch(windowType){
        case kHanningWindow:
          setRealWithWindow(cpx, input, length/2, windowTable, kForwardPositive);
          setRealWithWindow(cpx, input+length/2, length/2, windowTable, kBackwardPositive);
          break;
        case kTriangularWindow:
          setRealWithRamp(cpx, input, length/2, 0, 1);
          setRealWithRamp(cpx, input+length/2, length/2, 1, 0);
          break;
        case kRectangularWindow:
          setReal(cpx, input, length); //copy on the real part the input signal
          break;
      }
    }
    kiss_fft_cpx* getFdBuffer(){
      return fdBuffer;
    }
    kiss_fft_cpx* getTdBuffer(){
      return tdBuffer;
    }
    static void getReal(kiss_fft_cpx *cpx, float *real, int length){
      for(int i=0; i<length; i++){
        real[i]=cpx[i].r;
      }
    }
    static void getReal(kiss_fft_cpx *cpx, float *real, int length, float scale){
      for(int i=0; i<length; i++){
        real[i]=cpx[i].r*scale;
      }
    }
    static void setReal(kiss_fft_cpx *cpx, float *real, int length){
      for(int i=0; i<length; i++){
        cpx[i].r=real[i];
      }
    }
    static void setReal(kiss_fft_cpx *cpx, float real, int length){
      for(int i=0; i<length; i++){
        cpx[i].r=real;
      }
    }
    static void setRealWithRamp(kiss_fft_cpx *cpx, float *real, int length, float rampStart, float rampEnd){
      int rampStep=(rampStart-rampEnd)/length;
      float ramp=rampStart;
      for(int i=0; i<length; i++){
        cpx[i].r=real[i]*ramp;
        ramp=ramp+rampStep;
      }
    }
    static void setRealWithWindow(kiss_fft_cpx *cpx, float *real, int length, float *window, int mode){
      switch(mode){
        case kForwardPositive:
          for(int i=0; i<length; i++){
            cpx[i].r=real[i]*window[i];
          }
          break;
        case kBackwardPositive:
          for(int i=0; i<length; i++){
            cpx[i].r=real[i]*window[length-1-i];
          }
          break;
      }
    }
    static void setImag(kiss_fft_cpx *cpx, float *imag, int length){
      for(int i=0; i<length; i++){
        cpx[i].i=imag[i];
      }
    }
    static void setImag(kiss_fft_cpx *cpx, float imag, int length){
      for(int i=0; i<length; i++){
        cpx[i].i=imag;
      }
    }
    static void setRealRange(kiss_fft_cpx *cpx, float real, int start, int end){
      for(int i=start; i<end; i++){
        cpx[i].r=real;
      }
    }
    static void setImagRange(kiss_fft_cpx *cpx, float imag, int start, int end){
      for(int i=start; i<end; i++){
        cpx[i].i=imag;
      }
    }
    static void setPhase(kiss_fft_cpx *cpx, float phase){
      float ab=sqrtf((cpx->r*cpx->r)+(cpx->i*cpx->i));
      cpx->r=ab*cosf(phase);
      cpx->i=ab*sinf(phase);
    }
};
class KissFftPatch : public Patch {
private:
  Fft transform;
public:
  KissFftPatch(){
    init();
  }
  void init(){
    registerParameter(PARAMETER_A, "GAIN");
    registerParameter(PARAMETER_B, "TYPE"); // <=0.5: robotization, >0.5: whisperization
    registerParameter(PARAMETER_C, "N_BINS"); // How many bins we transform 
    int size=getBlockSize();
    transform.init(size,Fft::kRectangularWindow);
  }
  void processAudio(AudioBuffer &buffer){
    float gain = getParameterValue(PARAMETER_A);
    float type = getParameterValue(PARAMETER_B);
    float nBins = getParameterValue(PARAMETER_C);
    int size = buffer.getSize();
    float* left = buffer.getSamples(0);
    float* right=buffer.getSamples(1);
    transform.fft(left);
    kiss_fft_cpx* cpx=transform.getFdBuffer();
    int end=size/2*nBins;
    for(int n=1; n<end; n++){ //we do not need to get all the way to Nyquist: probably the content above 10kHz is not going to make much difference for these effects
      float ab=sqrtf((cpx[n].r*cpx[n].r)+(cpx[n].i*cpx[n].i));
      if(type>0.5){//whisperization: randomize phase
        float phase=rand()/(float)RAND_MAX*2*M_PI;
        cpx[n].r=ab*cosf(phase);
        cpx[n].i=ab*sinf(phase);
        cpx[size-n].r=cpx[n].r;
        cpx[size-n].i=cpx[n].i;
      }
      if(type<=0.5){//robotization
        cpx[n].r=ab;
        cpx[size-n].r=ab;
      }
    }
    if(type<=0.5) //robotization: set phase to zero
      Fft::setImag(cpx, 0.0, size);
    transform.ifft(left);
    for(int i=0; i<size;++i){
      //to protect your ears: first clip
      left[i]=left[i]>1? 1 : left[i];
      left[i]=left[i]<-1? -11 : left[i];
      //then apply gain
      left[i]*=gain;
      right[i]=left[i];
    }
  }
};











#endif // __KissFftPatch_hpp__
