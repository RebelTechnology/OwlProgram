#ifndef __PatchParameter_h__
#define __PatchParameter_h__

#define NO_PID -1

template<typename T>
class PatchParameter {
private:
  int pid;
  T value;
public:
  PatchParameter() : pid(NO_PID){}
  PatchParameter(int parameterId) : pid(parameterId){}
  PatchParameter<T>& operator=( const PatchParameter<T>& other );
  void update(T newValue){
    value = newValue;
  }
  operator T(){
    return value;
  }
};

typedef PatchParameter<float> FloatParameter;
typedef PatchParameter<int> IntParameter;
//FloatParameter spread = requestParameter("Spread", 0.0, 1.0);
//IntParameter spread = requestParameter("Spread", 0, 10, 5, 0.9);

#endif /* __PatchParameter_h__ */
