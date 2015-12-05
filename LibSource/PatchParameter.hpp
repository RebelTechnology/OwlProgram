#include "SmoothValue.hpp"

class PatchParameter {
  virtual void update(uint16_t value);
};

template<type T, type V>
class GenericParameter : public PatchParameter {
private:
  PatchParameterId pid;
  T min, max;
  V value;
public:
  PatchParameter(){}
  PatchParameter(PatchParameterId pid, T min, T max, V initialValue);
  // called when requestParameter() returns to patch constructor
  PatchParameter(const PatchParameter<T>& other);
  void update(uint16_t value);
  /*
  void update(T newValue){
    value = newValue;
  } */
  operator V(){
    return value;
  }
};

template<type T, type V>
PatchParameter<T, V> Patch::requestParameter(const char* name, T min, T max, V defaultValue = V()){
  int pid = 0;
  if(nextPid < 5){
    pid = nextPid++;
    registerParameter(pid, name);
  }
  return PatchParameter<T, V>(PatchParameterId(pid), min, max, defaultValue);
}
template<type T>
PatchParameter<T, SmoothValue<T>> Patch::requestParameter(const char* name, T min, T max, T defaultValue, T lambda){
  requestParameter(name, min, max, SmoothValue<T>(defaultValue, lambda));
}

typedef IntParameter PatchParameter<int, int>;
typedef StiffIntParameter PatchParameter<int, StiffInt>;
typedef SmoothIntParameter PatchParameter<int, SmoothInt>;
typedef FloatParameter PatchParameter<float, SmoothFloat>;
//FloatParameter spread = requestParameter("Spread", 0.0, 1.0, 0.5);
//SmoothFloatParameter spread = requestParameter("Spread", 0.0, 1.0, SmoothFloat(0.9, 0.5));
//StiffFloatParameter spread = requestParameter("Spread", 0.0, 1.0);
//SmoothFloatParameter spread = requestParameter("Spread", 0.0, 1.0, 0.5, 0.9);

template<>
IntParameter Patch::requestParameter(const char* name, int min, int max, int lambda=5){
  // will need copy constructor for SmoothValue (default/trivial ok)
  if(nextPid < 5)
    return IntParameter(PatchParameterId(nextPid++), min, max, SmoothInt(lambda, 0));
  return IntParameter();
}

FloatParameter Patch::requestParameter(const char* name, float min, float max, float lambda=0.8){
  if(nextPid < 5)
    return FloatParameter(PatchParameterId(nextPid++), min, max, SmoothFloat(lambda, 0));
  return FloatParameter();
}

template<type T, type V>
GenericParameter<T,V>::GenericParameter<T,V>(PatchParameterId p, T mn, T mx, T v)
: pid(p), min(mn), max(mx), value(v) {}

template<type T, type V>
GenericParameter(const GenericParameter<T,V>& other){
  if(pid != NO_PID)
    getCurrentPatchProcessor()->setPatchParameter(pid, this);
}

template<V>
void GenericParameter<int,V>::update(uint16_t newValue){
  value = (newValue*(max-min)+min)/4095;
}

template<V>
void GenericParameter<float,V>::update(uint16_t value){
  value = newValue/4096.0f;
  // should work for <float, float>, <float, SmoothFloat>, and <float, StiffFloat>
}
