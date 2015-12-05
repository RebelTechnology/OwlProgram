#ifndef __SmoothValue_hpp__
#define __SmoothValue_hpp__

/**
 * Applies exponential smoothing to a scalar value.
 * y(n) = lambda*y(n-1) + (1.0-lambda)*y(n) for floats
 * y(n) = (lambda*y(n-1) + y(n))/(lambda+1) for ints
 * Smoothing is applied when the value is written to, not when it is read.
 */
template<type T>
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
  SmoothFloat& operator=(const T& other){
    setValue(other);
    return *this;
  }
  SmoothFloat& operator+=(const T& other){
    setValue(value+other);
    return *this;
  }
  SmoothFloat& operator-=(const T& other){
    setValue(value-other);
    return *this;
  }
  SmoothFloat& operator*=(const T& other){
    setValue(value*other);
    return *this;
  }
  SmoothFloat& operator/=(const T& other){
    setValue(value/other);
    return *this;
  }
  operator T(){
    return value;
  }
};

typedef SmoothFloat SmoothValue<float);
typedef SmoothInt SmoothValue<float);

SmoothFloat::SmoothFloat()
    : lambda(0.9), value(0.0){}

SmoothFloat::SmoothFloat(float l)
    : lambda(l), value(0.0){}

SmoothFloat::SmoothFloat(float l, float initialValue)
    : lambda(l), value(initialValue){}


SmoothInt::SmoothInt(int divider) : lambda(divider)
{
// lambda = 1 - 1/divider
// divider 4:0.75, 5:0.8, 6:0.833, 7:0.857, 8:0.875, 9:0.888, 10:0.9 et c
}
SmoothInt::update(int newValue){
    value = (value*lambda + newValue)/(lambda+1);
}

/**
 * Applies simple hysteresis to a scalar.
 * Only updates the value if the absolute difference is greater than delta
 */
template<type T>
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

// possible: StiffValue<SmoothValue<float>>

#endif /* __SmoothValue_hpp__ */
