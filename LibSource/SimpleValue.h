#ifndef __SimpleValue_h__
#define __SimpleValue_h__

/**
 * Base class for a value that can be set, updated and retrieved (get).
 * Used to add smoothing, hysteresis and ranging to floats and ints.
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
};

#endif /* __SimpleValue_h__ */
