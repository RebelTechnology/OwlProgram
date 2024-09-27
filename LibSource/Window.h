#ifndef __WINDOW_H__
#define __WINDOW_H__

#include "basicmaths.h"
#include "SignalProcessor.h"
#include "FloatArray.h"

/*
 * Window provides static methods to generate and apply window functions:
 * rectangular, hann, hamming, triangular, sine, gauss, blackman, blackman_harris,
 * nuttall, blackman_nuttall, bohman, flattop, lanczos, parzen, welch
 *   or
 * window(WindowType type, float* window, int size);
 * a static method to apply a window to a signal (nothing but pairwise multiplication)
 * applyWindow(float *signal, float *window)
 * and a static method to apply a triangular window 
 * (as computing a triangular window is very cheap, 
 * this solution can be handy as it requires less memory (no window array required))
 * applyTriangularWindow(float *signal, int size)
 * 
 * Gauss and Tukey windows accept an extra parameter that modifies their shape
 */
class Window : public FloatArray, SignalProcessor {
private:
  size_t index = 0;
public:
  typedef enum WindowType {
    HammingWindow,
    HannWindow,
    SineWindow,
    BartlettHannWindow,
    GaussWindow,
    TukeyWindow,
    BlackmanWindow,
    BlackmanHarrisWindow,
    NuttallWindow,
    BlackmanNuttallWindow,
    BohmanWindow,
    FlatTopWindow,
    LanczosWindow,
    ParzenWindow,
    WelchWindow,
    BartlettWindow,
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
  float process(float input){
    float value = input*getData()[index++];
    if(index >= getSize())
      index = 0;
    return value;
  }
  void process(FloatArray input, FloatArray output){
    Window::applyWindow(input, getData(), output, getSize());
  }
  static Window create(WindowType type, int size){
    Window win(new float[size], size);
    win.window(type, win, size);
    return win;
  }
  static Window create(int size){
    return Window(new float[size], size);
  }
  static void destroy(Window win) {
    delete[] win.getData();
  }
  static void window(WindowType type, float *window, int size){
    switch(type){
    case HannWindow:
      hann(window, size);
      break;
    case HammingWindow:
      hamming(window, size);
      break;
    case SineWindow:
      sine(window, size);
      break;
    case BartlettHannWindow:
      bartlett_hann(window, size);
      break;
    case GaussWindow:
      gauss(window, size);
      break;
    case TukeyWindow:
      tukey(window, size);
      break;
    case BlackmanWindow:
      blackman(window, size);
      break;
    case BlackmanHarrisWindow:
      blackman_harris(window, size);
      break;
    case NuttallWindow:
      nuttall(window, size);
      break;
    case BlackmanNuttallWindow:
      blackman_nuttall(window, size);
      break;
    case BohmanWindow:
      bohman(window, size);
      break;
    case FlatTopWindow:
      flat_top(window, size);
      break;
    case LanczosWindow:
      lanczos(window, size);
      break;
    case ParzenWindow:
      parzen(window, size);
      break;
    case WelchWindow:
      welch(window, size);
      break;
    case TriangularWindow:
    case BartlettWindow:
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
  static void sine(float* window, int size) {
    for(int n=0; n<size; n++)
      window[n] = sinf(M_PI * n / (size - 1));
  }
  static void bartlett_hann(float* window, int size) {
    for(int n=0; n<size; n++)
      window[n] = 0.62 - 0.48 * std::abs((float)n / (size - 1) - 0.5)
        + 0.38 * cosf(M_PI * 2 * (float(n) / (size - 1) - 0.5));
  }
  static void gauss(float *window, int size, float q = 0.5){
    float a = (size - 1) / 2;
    for(int n=0; n<size; n++) {
      float t = (n - a) / (q * a);
      window[n] = expf(-t * t / 2);
    }
  }
  static void tukey(float *window, int size, float q = 0.5){
    float a = q * 0.5 * (size - 1);
    for(int n=0; n<size; n++) {
      if (n <= a) {
        window[n] =  0.5 * (1.0 + cosf(M_PI * (float(n) /a - 1.0)));
      }
      else if (n >= 1.0 - a) {
        window[n] = 0.5 * (1.0 + cosf(M_PI * (float(n) / a - 1.0)));
      }
      else {
        window[n] = 1;
      }
    }
  }
  static void blackman(float *window, int size){
    for(int n=0; n<size; n++) {
      float a = M_PI * 2 * n / (size - 1);
      window[n] = 0.42 - 0.5 * cosf(a) + 0.08 * cosf(a * 2);
    }
  }
  static void blackman_harris(float *window, int size){
    for(int n=0; n<size; n++)
      window[n] = 0.35875 - (0.48829 * cosf((2 * M_PI * n)/(size - 1))) +
        (0.14128 * cosf((4 * M_PI * n) / (size - 1))) - (0.01168 * cosf((4 * M_PI * n) / (size - 1)));
  }
  static void nuttall(float *window, int size){
    for(int n=0; n<size; n++) {
      float x = M_PI * 2 * n / (size - 1);
      window[n] = 0.355768 - 0.487396 * cosf(x * 2) + 0.144232 * cosf(x * 4) - 0.012604 * cosf(x * 6);
    }
  }
  static void blackman_nuttall(float *window, int size){
    for(int n=0; n<size; n++){
      float x = float(n) / (size - 1) * M_PI;
      window[n] = 0.3635819 - 0.4891775 * cosf(x * 2) + 0.1365995 * cosf(x * 4) - 0.0106411 * cosf(x * 6);
    }
  }
  static void bohman(float* window, int size) {
    for(int n=0; n<size; n++) {
      float x = float(n) / (size - 1) * 2 - 1;
      window[n] = (1.0 - std::abs(x)) * cosf(M_PI * std::abs(x)) + M_1_PI * sinf(M_PI * std::abs(x));
    }
  }
  static void flat_top(float* window, int size) {
    for(int n=0; n<size; n++) {
      float x = float(n) / (size - 1) * M_PI;
      window[n] = 0.21557895 - 0.41663158 * cosf(x * 2)
        + 0.277263158 * cosf(x * 4) - 0.083578947 * cosf(x * 6)
        + 0.006947368 * cosf(x * 8);
    }
  }
  static void parzen(float* window, int size) {
    for(int n=0; n<size; n++) {
      int n1 = n - size / 2;
      if (std::abs(n1) < size / 4) {
        float a = n1 * 2.0 / size;
        window[n] = 1.0 - 7.0 * a * a * (1.0 - std::abs(n1) * 2.0 / size);
      }
      else {
        float a = (1.0 - std::abs(n1) * 2.0 / size);
        window[n] = 2.0 * a * a * a;
      }
    }
  }
  static void welch(float* window, int size) {
    for(int n=0; n<size; n++) {
      float a = float(size - 1) / 2;
      float b = (float(n) - a) / a;
      window[n] = 1.0 - a * a;
    }
  }
  static void lanczos(float* window, int size) {
    for(int n=0; n<size; n++)
      window[n] = sinf(M_PI * 2 * n / (size - 1) - M_PI) / (M_PI * 2 * n / (size - 1) - M_PI);
  }
  static void triangular(float *window, int size){
    float rampStep = 1/(size/2.0f);
    float ramp = 0;
    int n=0;
    for(; n<size/2; n++){
      window[n] = ramp;
      ramp = ramp+rampStep;
    }
    rampStep = -rampStep;
    for(; n<size; n++){
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
  static void applyWindow(float *signalIn, float *window, float *signalOut, int size);
};

#endif /* __WINDOW_H__ */
