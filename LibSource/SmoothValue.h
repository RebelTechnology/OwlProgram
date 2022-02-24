#ifndef __SmoothValue_h__
#define __SmoothValue_h__

#include "SimpleValue.h"

/**
 * Applies exponential smoothing to a scalar value.
 * y(n) = lambda*y(n-1) + (1.0-lambda)*y(n) for floats
 * y(n) = (lambda*y(n-1) + y(n))/(lambda+1) for ints
 * Smoothing is applied when the value is written to, not when it is read.
 * todo: adjust for sampling frequency / blocksize (time delay of the response (mean) is 1/α data points)
 * https://en.wikipedia.org/wiki/Exponential_smoothing
 */
template<typename T, typename Value = SimpleValue<T>>
class SmoothValue : public Value {
public:
  T lambda = Value::DEFAULT_LAMBDA;
  SmoothValue() {}
  SmoothValue(T value) : Value(value) {}
  SmoothValue(T lambda, T init) : Value(init), lambda(lambda) {}
  SmoothValue(const SmoothValue<T, Value>& other) : Value(other), lambda(other.lambda) {}
  void update(T x){
    Value::update(Value::get()*lambda + x*(1.0f - lambda));
  }
  SmoothValue<T, Value>& operator=(const T& x){
    update(x);
    return *this;
  }
  SmoothValue<T, Value>& operator+=(const T& other){
    update(Value::get() + other);
    return *this;
  }
  SmoothValue<T, Value>& operator-=(const T& other){
    update(Value::get() - other);
    return *this;
  }
  SmoothValue<T, Value>& operator*=(const T& other){
    update(Value::get() * other);
    return *this;
  }
  SmoothValue<T, Value>& operator/=(const T& other){
    update(Value::get() / other);
    return *this;
  }
  operator T(){
    return Value::get();
  }
  static T normal(float lambda, int blocksize);
};

template class SmoothValue<int>;
template class SmoothValue<float>;
typedef SmoothValue<float> SmoothFloat;
typedef SmoothValue<int> SmoothInt;

/**
 * Applies simple hysteresis to a scalar.
 * Only updates the value if the absolute difference is greater than delta
 */
template<typename T, typename Value = SimpleValue<T>>
class StiffValue : public Value {
public:
  T delta = Value::DEFAULT_DELTA;
  StiffValue() {}
  StiffValue(T value) : Value(value) {}
  StiffValue(T delta, T init) : Value(init), delta(delta) {}
  StiffValue(const StiffValue<T, Value>& other) : Value(other), delta(other.delta) {}
  void update(T newValue){
    if(abs(Value::get() - newValue) > delta)
      Value::update(newValue);
  }
  StiffValue<T, Value>& operator=(const T& other){
    update(other);
    return *this;
  }
  StiffValue<T, Value>& operator+=(const T& other){
    update(Value::get() + other);
    return *this;
  }
  StiffValue<T, Value>& operator-=(const T& other){
    update(Value::get() - other);
    return *this;
  }
  StiffValue<T, Value>& operator*=(const T& other){
    update(Value::get() * other);
    return *this;
  }
  StiffValue<T, Value>& operator/=(const T& other){
    update(Value::get() / other);
    return *this;
  }
  operator T(){
    return Value::get();
  }
  static T normal(float delta);
};

template class StiffValue<int>;
template class StiffValue<float>;
typedef StiffValue<float> StiffFloat;
typedef StiffValue<int> StiffInt;
typedef SmoothValue<float, StiffFloat> SmoothStiffFloat;
typedef SmoothValue<int, StiffInt> SmoothStiffInt;

template<class T>
using SmoothStiffValue = SmoothValue<T, StiffValue<T>>;

/**
 * Linearly scaled value. Scales an input on the range [0, 1] linearly to the range [min, max].
 * Minimum may be less than maximum for inverted scale.
 */
template<typename T, typename Value = SimpleValue<T>>
class LinearValue : public Value {
protected:
  T scale;
  T adjust;
  T offset;
public:
  LinearValue(T value = 0): Value(value), scale(1), adjust(0), offset(0) {}
  LinearValue(const LinearValue<T, Value>& other):
    Value(other), scale(other.scale), adjust(other.adjust), offset(other.offset) {}
  LinearValue(T minimum, T maximum, T init): Value(init) {
    setRange(minimum, maximum);
  }
  void setRange(T minimum, T maximum){
    scale = maximum - minimum;
    adjust = 0;
    offset = minimum;
  }
  void setRange(T from_lo, T from_hi, T to_lo, T to_hi){
    // return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    //                       * scale + 
    scale = (to_hi - to_lo) / (from_hi - from_lo);
    offset = to_lo;
    adjust = from_lo;
    // todo: verify!
  }
  /** 
   * Get the input value
   */
  T getControl(){
    return (Value::get() - offset) / scale + adjust;
  }
  void update(T x){
    Value::update((x - adjust) * scale + offset);
  }  
  LinearValue<T, Value>& operator=(const T& x){
    update(x);
    return *this;
  }
  LinearValue<T, Value>& operator+=(const T& other){
    update(Value::get() + other);
    return *this;
  }
  LinearValue<T, Value>& operator-=(const T& other){
    update(Value::get() - other);
    return *this;
  }
  LinearValue<T, Value>& operator*=(const T& other){
    update(Value::get() * other);
    return *this;
  }
  LinearValue<T, Value>& operator/=(const T& other){
    update(Value::get() / other);
    return *this;
  }
  operator T(){
    return Value::get();
  }
};

template class LinearValue<int>;
template class LinearValue<float>;
typedef LinearValue<float>  LinearFloat;
typedef LinearValue<int>  LinearInt;
typedef SmoothValue<float, LinearFloat> SmoothLinearFloat;
typedef SmoothValue<int, LinearInt> SmoothLinearInt;

/**
 * Exponentially scaled value. Scales an input on the range [0, 1] exponentially to the range [min, max].
 * Minimum may be less than maximum for inverted scale.
 * Note that configured minimum and maximum values must be greater than 0.
 *
 * Ref: https://www.desmos.com/calculator/wehcvtggk7
 */
template<typename T, typename Value = SimpleValue<T>>
class ExponentialValue : public Value {
protected:
  T c = Value::DEFAULT_EXP_C;
  T k = Value::DEFAULT_EXP_K;
public:
  /** Default range scales [0, 1] to [0.5, 2] */
  ExponentialValue() {}
  ExponentialValue(T value): Value(value) {}
  ExponentialValue(const ExponentialValue<T>& other): Value(other), c(other.c), k(other.k) {}
  ExponentialValue(T minimum, T maximum, T init): Value(init) {
    setRange(minimum, maximum);
  }
  void setRange(T minimum, T maximum){
    ASSERT(minimum > 0 && maximum > 0, "Exponential min/max must be greater than 0");
    c = minimum;
    k = logf(maximum/minimum);
  }
  /**
   * Get the input value
   */
  T getControl(){
    return logf(Value::get()/c)/k;
  }
  void update(T x){
    Value::update(c * expf(k * x));
  }
  operator T(){
    return Value::get();
  }  
  ExponentialValue<T, Value>& operator=(const T& x){
    update(x);
    return *this;
  }
  ExponentialValue<T, Value>& operator+=(const T& other){
    update(Value::get() + other);
    return *this;
  }
  ExponentialValue<T, Value>& operator-=(const T& other){
    update(Value::get() - other);
    return *this;
  }
  ExponentialValue<T, Value>& operator*=(const T& other){
    update(Value::get() * other);
    return *this;
  }
  ExponentialValue<T, Value>& operator/=(const T& other){
    update(Value::get() / other);
    return *this;
  }
};

template class ExponentialValue<int>;
template class ExponentialValue<float>;
typedef ExponentialValue<int>  ExponentialInt;
typedef ExponentialValue<float>  ExponentialFloat;
typedef SmoothValue<int, ExponentialInt> SmoothExponentialInt;
typedef SmoothValue<float, ExponentialFloat> SmoothExponentialFloat;

/**
 * Logarithmically scaled value. Scales an input on the range [0, 1] logarithmically to the range [min, max].
 * Minimum may be less than maximum for inverted scale.
 * Note: coefficients scale exponentially with range and may explode!
 *
 * Ref: https://www.desmos.com/calculator/xljgqopbhy
 */
template<typename T, typename Value = SimpleValue<T>>
class LogarithmicValue : public Value {
protected:
  T c = Value::DEFAULT_LOG_C;
  T k = Value::DEFAULT_LOG_K;
public:
  /** Default range scales [0, 1] to [0.5, 2] */
  LogarithmicValue() {}
  LogarithmicValue(T value): Value(value) {}
  LogarithmicValue(const LogarithmicValue<T>& other): Value(other), c(other.c), k(other.k) {}
  LogarithmicValue(T a, T b, T init): Value(init) {
    setRange(a, b);
  }
  void setRange(T a, T b){
    c = expf(a);
    k = expf(b) - c;
  }
  void update(T x){
    Value::update(logf(k * x + c));
  }
  T getControl(){
    return (expf(Value::get()) - c) / k;
  }
  /* value cast operator */
  operator T(){
    return Value::get();
  }  
  /* assignment operator */
  LogarithmicValue<T, Value>& operator=(const T& x){
    update(x);
    return *this;
  }
  LogarithmicValue<T, Value>& operator+=(const T& other){
    update(Value::get() + other);
    return *this;
  }
  LogarithmicValue<T, Value>& operator-=(const T& other){
    update(Value::get() - other);
    return *this;
  }
  LogarithmicValue<T, Value>& operator*=(const T& other){
    update(Value::get() * other);
    return *this;
  }
  LogarithmicValue<T, Value>& operator/=(const T& other){
    update(Value::get() / other);
    return *this;
  }
};

template class LogarithmicValue<float>;
template class LogarithmicValue<int>;
typedef LogarithmicValue<float> LogarithmicFloat;
typedef LogarithmicValue<int> LogarithmicInt;

#endif /* __SmoothValue_h__ */
