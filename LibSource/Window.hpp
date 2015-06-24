/*
 * Window provides static methods to generate and apply window functions:
 * rectangular, hann, hanning, hamming, triangular
 *   or
 * window(WindowType type, float* window, int size);
 * a static method to apply a window to a signal (nothing but pairwise multiplication)
 * applyWindow(float *signal, float *window)
 * and a static method to apply a triangular window 
 * (as computing a triangular window is very cheap, 
 * this solution can be handy as it requires less memory (no window array required))
 * applyTriangularWindow(float *signal, int size)
 */
class Window : public FloatArray {
public:
  typedef enum WindowType {
    HammingWindow,
    HannWindow,
    HanningWindow,
    TriangularWindow,
    RectangularWindow // no window
  } WindowType;
  Window(){}
  Window(float* win, int size) : FloatArray(win, size) {}
  void apply(float *signalIn){
    Window::applyWindow(signalIn, getData(), getSize());
  }
  void apply(float *signalIn, float *signalOut){
    Window::applyWindow(signalIn, getData(), signalOut, getSize());
  }
  void invert(){
    float* data = getData();
    for(int n=0; n<getSize(); n++)
      data[n] = 1/data[n];
  }
  static Window create(WindowType type, int size){
    Window win(new float[size], size);
    win.window(type, win, size);
    return win;
  }
  static void window(WindowType type, float *window, int size){
    switch(type){
    case HannWindow:
    case HanningWindow:
      hann(window, size);
      break;
    case HammingWindow:
      hamming(window, size);
      break;
    case TriangularWindow:
      triangular(window, size);
      break;
    case RectangularWindow:
    default:
      rectangular(window, size);
      break;
    }
  }
  static void rectangular(float *window, int size){
    for(int n=0; n<size; n++)
      window[n] = 1;
  }
  static void hann(float *window, int size){
    for(int n=0; n<size; n++)
      window[n] = 0.5*(1-cosf((float)n/(size-1)*2*M_PI));
  }
  static void hamming(float *window, int size){
    for(int n=0; n<size; n++)
      window[n] = 0.54-0.46*cosf((float)n/(size-1)*2*M_PI);
  }
  static void triangular(float *window, int size){
    float rampStep = 1/(size/2.0f);
    float ramp = 0;
    for(int n=0; n<size/2.0f; n++){
      window[n] = ramp;
      ramp = ramp+rampStep;
    }
    rampStep = -rampStep;
    for(int n=size/2.0f; n<size; n++){
      window[n] = ramp;
      ramp = ramp+rampStep;
    }
  }
  static void applyTriangularWindow(float *signal, int size){
    applyTriangularWindow(signal, signal, size);
  }
  static void applyTriangularWindow(float *signalIn, float *signalOut, int size){
    float rampStep = 1/(size/2);
    float ramp = 0;
    for(int n=0; n<size/2; n++){
      signalOut[n] = signalIn[n]*ramp;
      ramp = ramp+rampStep;
    }
    rampStep = -rampStep;
    for(int n=size/2; n<size; n++){
      signalOut[n] = signalIn[n]*ramp;
      ramp = ramp+rampStep;
    }
  }
  static void applyWindow(float *signal, float *window, int size){
    applyWindow(signal, window, signal, size);
  }
  static void applyWindow(float *signalIn, float *window, float *signalOut, int size){
    for(int n=0; n<size; n++)
      signalOut[n] = window[n]*signalIn[n];
  }
};
