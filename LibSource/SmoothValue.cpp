#include "SmoothValue.h"

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

template class SmoothValue<int>;
template class SmoothValue<float>;
