#include "SimpleValue.h"
#include "math.h"

#define QINT12_FACTOR ((float)(1<<12))

template<> const float SimpleValue<float>::DEFAULT_LAMBDA = 0;
template<> const float SimpleValue<float>::DEFAULT_DELTA = 0.02;
template<> const float SimpleValue<float>::DEFAULT_EXP_C = 1;
template<> const float SimpleValue<float>::DEFAULT_EXP_K = 2*M_LN2;
template<> const float SimpleValue<float>::DEFAULT_LOG_C = 1.6487212707;
template<> const float SimpleValue<float>::DEFAULT_LOG_K = 5.74033482823*M_LN2;
template<> const int SimpleValue<int>::DEFAULT_LAMBDA = (int)(0.02*QINT12_FACTOR);
template<> const int SimpleValue<int>::DEFAULT_DELTA = (int)(0.02*QINT12_FACTOR);
template<> const int SimpleValue<int>::DEFAULT_EXP_C = (int)(1*QINT12_FACTOR);
template<> const int SimpleValue<int>::DEFAULT_EXP_K = (int)(2*M_LN2*QINT12_FACTOR);
template<> const int SimpleValue<int>::DEFAULT_LOG_C = (int)(1.6487212707*QINT12_FACTOR);
template<> const int SimpleValue<int>::DEFAULT_LOG_K = (int)(5.74033482823*M_LN2*QINT12_FACTOR);
