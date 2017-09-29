#include "fastlog.h"
/*
  O. Vinyals, G. Friedland, and N. Mirghafori
  ICSI Technical Report TR-07-002
  http://www.icsi.berkeley.edu/cgi-bin/pubs/publication.pl?ID=002209
*/

#if 0
/* Creates the ICSILog lookup table. Must be called
   once before any call to icsi_log().
   n is the number of bits to be taken from the mantissa
   (0<=n<=23)
   lookup_table is a pointer to a floating point array of 2^n positions.
*/
void fill_icsi_log_table(float* lookup_table, uint32_t precision){
  float numlog;
  int32_t *const exp_ptr = ((int32_t*)&numlog);
  int32_t x = *exp_ptr; //x is the float treated as an integer
  x = 0x3F800000; //set the exponent to 0 so numlog=1.0
  *exp_ptr = x;
  int32_t incr = 1 << (23 - precision); //amount to increase the mantissa
  int32_t size = 1 << precision;
  int32_t i;
  for(i=0;i<size;++i){
    lookup_table[i] = log2(numlog); //save the log value
    x += incr;
    *exp_ptr = x; //update the float value
  }
}
#endif

float icsi_log(float arg, 
		    const float* lookup_table,
		    const unsigned int precision){
  if(arg <= 0) 
    return 0;
  int *const exp_ptr = ((int*)&arg);
  int x = *exp_ptr; //x is treated as integer
  const int log_2 = ((x >> 23) & 255) - 127;//exponent
  x &= 0x7FFFFF; //mantissa
  x = x >> (23-precision); //quantize mantissa
  arg = lookup_table[x]; //lookup precomputed arg
  arg = ((arg + log_2)* 0.69314718);
  return arg; //natural logarithm
}
