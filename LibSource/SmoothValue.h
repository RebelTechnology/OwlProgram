#ifndef __SmoothValue_h__
#define __SmoothValue_h__

/**
 * Applies exponential smoothing to a scalar value.
 * y(n) = lambda*y(n-1) + (1.0-lambda)*y(n) for floats
 * y(n) = (lambda*y(n-1) + y(n))/(lambda+1) for ints
 * Smoothing is applied when the value is written to, not when it is read.
 * todo: adjust for sampling frequency / blocksize (time delay of the response (mean) is 1/α data points)
 * https://en.wikipedia.org/wiki/Exponential_smoothing
 */
template<typename T>
class SmoothValue {
private:
  T value;
public:
  T lambda;
  SmoothValue();
  SmoothValue(T lambda);
  SmoothValue(T lambda, T initialValue);
  SmoothValue(const SmoothValue<T>& other){
    // copy constructor: not needed?
    value = other.value;
    lambda = other.lambda;
  }
  T normal(T lambda, float samplerate);
  void update(T newValue);
  T getValue(){
    return value;
  }
  SmoothValue<T>& operator=(const T& other){
    update(other);
    return *this;
  }
  SmoothValue<T>& operator+=(const T& other){
    update(value+other);
    return *this;
  }
  SmoothValue<T>& operator-=(const T& other){
    update(value-other);
    return *this;
  }
  SmoothValue<T>& operator*=(const T& other){
    update(value*other);
    return *this;
  }
  SmoothValue<T>& operator/=(const T& other){
    update(value/other);
    return *this;
  }
  operator T(){
    return value;
  }
};

typedef SmoothValue<float> SmoothFloat;
typedef SmoothValue<int> SmoothInt;


/**
 * Applies simple hysteresis to a scalar.
 * Only updates the value if the absolute difference is greater than delta
 */
template<typename T>
class StiffValue {
  // simple hysteresis
private:
  T delta;
  T value;
public:
  StiffValue(){}
  StiffValue(T d)
   : delta(d){}
  StiffValue(T d, T initialValue)
   : delta(d), value(initialValue) {}
  void update(T newValue){
    if(abs(value-newValue) > delta)
      value = newValue;
  }
};

typedef StiffValue<float> StiffFloat;
typedef StiffValue<int> StiffInt;

#endif /* __SmoothValue_h__ */
