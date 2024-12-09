#ifndef __SimpleValue_h__
#define __SimpleValue_h__

/**
 * Base class for a value that can be set, updated and retrieved (get).
 * Used to selectively add smoothing, hysteresis and ranging to floats and ints.
 * For absolute value updates use the `=` assignment operator, or `update()`.
 * For relative value updates use the `+=` and/or `-=` assignment operators.
 * For value scaling use the `*=` and/or `/=` assignment operators.
 */
template <typename T>
class SimpleValue {
protected:
  T value;
public:
  SimpleValue() : value(0) {}
  SimpleValue(T value) : value(value) {}
  SimpleValue(const SimpleValue<T>& other) : value(other.value) {}
  T get(){
    return value;
  }
  void reset(T x){
    value = x;
  }
  void update(T x){
    value = x;    
  }
  SimpleValue<T>& operator=(const T& other){
    update(other);
    return *this;
  }
  SimpleValue<T>& operator+=(const T& other){
    update(value + other);
    return *this;
  }
  SimpleValue<T>& operator-=(const T& other){
    update(value - other);
    return *this;
  }
  SimpleValue<T>& operator*=(const T& other){
    update(value * other);
    return *this;
  }
  SimpleValue<T>& operator/=(const T& other){
    update(value / other);
    return *this;
  }
  operator T(){
    return get();
  }
  static const T DEFAULT_LAMBDA;
  static const T DEFAULT_DELTA;
  static const T DEFAULT_EXP_C;
  static const T DEFAULT_EXP_K;
  static const T DEFAULT_LOG_C;
  static const T DEFAULT_LOG_K;
};

// prevent -Wundefined-var-template warnings
template<> const float SimpleValue<float>::DEFAULT_LAMBDA;
template<> const float SimpleValue<float>::DEFAULT_DELTA;
template<> const float SimpleValue<float>::DEFAULT_EXP_C;
template<> const float SimpleValue<float>::DEFAULT_EXP_K;
template<> const float SimpleValue<float>::DEFAULT_LOG_C;
template<> const float SimpleValue<float>::DEFAULT_LOG_K;
template<> const int SimpleValue<int>::DEFAULT_LAMBDA;
template<> const int SimpleValue<int>::DEFAULT_DELTA;
template<> const int SimpleValue<int>::DEFAULT_EXP_C;
template<> const int SimpleValue<int>::DEFAULT_EXP_K;
template<> const int SimpleValue<int>::DEFAULT_LOG_C;
template<> const int SimpleValue<int>::DEFAULT_LOG_K;

#endif /* __SimpleValue_h__ */
