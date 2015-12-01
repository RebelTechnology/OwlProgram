#ifndef __SmoothFloat_hpp__
#define __SmoothFloat_hpp__

/**
 * Applies exponential smoothing to a float value.
 * y(n) = lambda*y(n-1) + (1.0-lambda)*y(n)
 * Smoothing is applied when the value is written to, not when it is read.
 */
class SmoothFloat {
private:
  float value;
public:
  float lambda;
  SmoothFloat(float l=0.9, float initialValue=0.0)
    : lambda(l), value(initialValue){}
  void setValue(float newValue){
    value = value*lambda + newValue*(1.0-lambda);
  }
  float getValue(){
    return value;
  }
  // set without smoothing
  void resetValue(float newValue){
    value = newValue;
  }
  SmoothFloat& operator=(const float& other){
    setValue(other);
    return *this;
  }
  operator float(){
    return value;
  }
};

#endif /* __SmoothFloat_hpp__ */
