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

template<>
SmoothFloat::SmoothValue()
  : lambda(0.9), value(0.0){}

template<>
SmoothFloat::SmoothValue(float l)
  : lambda(l), value(0.0){}

template<>
SmoothFloat::SmoothValue(float l, float initialValue)
  : lambda(l), value(initialValue){}

template<>
SmoothInt::SmoothValue(int divider) : lambda(divider) {
// lambda = 1 - 1/divider
// divider 4:0.75, 5:0.8, 6:0.833, 7:0.857, 8:0.875, 9:0.888, 10:0.9 et c
}

template<>
SmoothInt::SmoothValue(int divider, int initialValue) 
  : lambda(divider), value(initialValue) {}

template<>
void SmoothFloat::update(float newValue){
  value = value*lambda + newValue*(1.0f - lambda);
}

template<>
void SmoothInt::update(int newValue){
  value = (value*lambda + newValue)/(lambda+1);
}

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
