/*
class Window
provides static methods to generate windows:
  rectangular, hann, hanning, hamming, triangular (float *window, int size)
or
  window(WindowType kWindowType, float* window, int size);
a static method to apply a window to a signal (nothing but pairwise multiplication)
  applyWindow(float *signal, float *window)
and a static method to apply a triangular window (as computing a triangular window is very cheap, 
this solution can be handy as it requires less memory (no window array required))
  applyTriangularWindow(float *signal, int size)
*/
class Window{
  public:
     typedef enum WindowType{
      kHammingWindow,
      kHannWindow,
      kHanningWindow,
      kTriangularWindow,
      kRectangularWindow //"no window"
    } WindowType;
    static void window(WindowType windowType, float *window, int size){
      switch(windowType){
        case kHannWindow:
        case kHanningWindow:
          hann(window, size);
          break;
        case kHammingWindow:
          hamming(window, size);
          break;
        case kTriangularWindow:
          triangular(window, size);
          break;
        case kRectangularWindow:
        default:
          rectangular(window, size);
          break;
      }
    }
    static void rectangular(float *window, int size){
      for(int n=0; n<size; n++){
        window[n]=1;
      }
    };
    static void hanning(float *window, int size){hann(window, size);};
    static void hann(float *window, int size){
      for(int n=0; n<size; n++){
        window[n]=0.5*(1-cosf((float)n/(size-1)*2*M_PI));
      }
    };
    static void hamming(float *window, int size){
      for(int n=0; n<size; n++){
        window[n]=0.54-0.46*cosf((float)n/(size-1)*2*M_PI);
      };
    }
    static void triangular(float *window, int size){
      float rampStep=1/(size/2.0f);
      float ramp=0;
      for(int n=0; n<size/2.0f; n++){
        window[n]=ramp;
        ramp=ramp+rampStep;
      }
      rampStep=-rampStep;
      for(int n=size/2.0f; n<size; n++){
        window[n]=ramp;
        ramp=ramp+rampStep;
      }
    };
    static void applyTriangularWindow(float *signal, int size){
      applyTriangularWindow(signal, signal, size);
    }
    static void applyTriangularWindow(float *signalIn, float *signalOut, int size){
      float rampStep=1/(size/2);
      float ramp=0;
      for(int n=0; n<size/2; n++){
        signalOut[n]=signalIn[n]*ramp;
        ramp=ramp+rampStep;
      }
      rampStep=-rampStep;
      for(int n=size/2; n<size; n++){
        signalOut[n]=signalIn[n]*ramp;
        ramp=ramp+rampStep;
      }
    }
    static void applyWindow(float *signal, float *window, int size){
      applyWindow(signal,window,signal,size);
    }
    static void applyWindow(float *signalIn, float *window, float *signalOut, int size){
      for(int n=0; n<size; n++)
        signalOut[n]=window[n]*signalIn[n];
    }
};

class FastFourierTransform {
private:
  arm_rfft_fast_instance_f32 instance;
public:
  void init(int len){
    void* args[] = {(void*)&instance, (void*)&len};
    getProgramVector()->serviceCall(OWL_SERVICE_ARM_RFFT_FAST_INIT_F32, args, 2);
    // arm_rfft_fast_init_f32(&instance, len);
// Supported FFT Lengths are 32, 64, 128, 256, 512, 1024, 2048, 4096.
  }
  void fft(float* in, ComplexFloatArray& out){
    arm_rfft_fast_f32(&instance, in, (float *)out, 0);
  }
  void ifft(ComplexFloatArray& in, float* out){
   arm_rfft_fast_f32(&instance, (float *)in, out, 1);
  }
};

