
/**
 * Applies exponential smoothing to a float value.
 * y(n) = lambda*y(n-1) + (1.0-lambda)*y(n)
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
  SmoothFloat& operator=(const float& other){
    setValue(other);
    return *this;
  }
  operator float(){
    return value;
  }
};

