#include "SimpleValue.h"
#include "math.h"

/**
 * The int constants are meant to provide reasonable defaults for use
 * with OWL parameters, which are generally 12 bit fixed point, i.e. Q12.
 */
#define Q12_FACTOR ((float)(1<<12))

template<> const float SimpleValue<float>::DEFAULT_LAMBDA = 0.9;
template<> const float SimpleValue<float>::DEFAULT_DELTA = 0.02;
template<> const float SimpleValue<float>::DEFAULT_EXP_C = 1;
template<> const float SimpleValue<float>::DEFAULT_EXP_K = 2*M_LN2;
template<> const float SimpleValue<float>::DEFAULT_LOG_C = 1.6487212707;
template<> const float SimpleValue<float>::DEFAULT_LOG_K = 5.74033482823*M_LN2;
template<> const int SimpleValue<int>::DEFAULT_LAMBDA = (int)(0.9*Q12_FACTOR);
template<> const int SimpleValue<int>::DEFAULT_DELTA = (int)(0.02*Q12_FACTOR);
template<> const int SimpleValue<int>::DEFAULT_EXP_C = (int)(1*Q12_FACTOR);
template<> const int SimpleValue<int>::DEFAULT_EXP_K = (int)(2*M_LN2*Q12_FACTOR);
template<> const int SimpleValue<int>::DEFAULT_LOG_C = (int)(1.6487212707*Q12_FACTOR);
template<> const int SimpleValue<int>::DEFAULT_LOG_K = (int)(5.74033482823*M_LN2*Q12_FACTOR);
