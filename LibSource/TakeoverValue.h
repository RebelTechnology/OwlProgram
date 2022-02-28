#ifndef __TakeoverValue_h__
#define __TakeoverValue_h__

#include "SimpleValue.h"

/**
 * TakeoverValue allows parameters to implement Takeover logic.
 * Takeover logic, also known as 'Match' or 'Soft Takeover', allows for 
 * controls to be multiplexed while avoiding sudden jumps in settings.
 * Other common control modes are 'Relative' and 'Jump-to' or 'Skip'.
 * 
 * A TakeoverValue can be locked by calling `setLocked(true)`.
 * Further updates are then prevented, until either the TakeoverValue
 * is manually unlocked (i.e. with `setLocked(false)`), or the new value
 * falls within a given threshold of the previously set value.
 * 
 */
template<typename T, typename Value = SimpleValue<T>>
class TakeoverValue : public Value {
protected:
  bool locked = false;
  T threshold = Value::DEFAULT_DELTA;
public:
  TakeoverValue() {}
  TakeoverValue(T value): Value(value) {}
  TakeoverValue(const TakeoverValue<T>& other): Value(other), locked(other.locked), threshold(other.threshold) {}
  TakeoverValue(T threshold, T init): Value(init), threshold(threshold) {}
  void setLocked(bool value){
    locked = value;
  }
  bool isLocked(){
    return locked;
  }
  void update(T x){
    if(locked && abs(Value::get() - x) <= threshold)
      locked = false;
    if(!locked)
      Value::update(x);
  }
  operator T(){
    return Value::get();
  }  
  TakeoverValue<T, Value>& operator=(const T& x){
    update(x);
    return *this;
  }
  TakeoverValue<T, Value>& operator+=(const T& other){
    update(Value::get() + other);
    return *this;
  }
  TakeoverValue<T, Value>& operator-=(const T& other){
    update(Value::get() - other);
    return *this;
  }
  TakeoverValue<T, Value>& operator*=(const T& other){
    update(Value::get() * other);
    return *this;
  }
  TakeoverValue<T, Value>& operator/=(const T& other){
    update(Value::get() / other);
    return *this;
  }
};

template class TakeoverValue<int>;
template class TakeoverValue<float>;
typedef TakeoverValue<float>  TakeoverFloat;
typedef TakeoverValue<int>  TakeoverInt;

#endif /* __TakeoverValue_h__ */
