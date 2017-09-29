/*  Lookup table generator for the ICSI logarithm function
    Version 0.6 beta
    Build date: November 13th, 2007
    Copyright (C) 2007 International Computer Science Institute
    1947 Center Street. Suite 600
    Berkeley, CA 94704
    Contact information:
    Oriol Vinyals   vinyals@icsi.berkeley.edu
    Gerald Friedland    fractor@icsi.berkeley.edu
    Acknowledgements:
    Thanks to Harrison Ainsworth (hxa7241@gmail.com) for his idea that
    doubled the accuracy.
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/
#include "fastlog.h"
#include <math.h>

/* Creates the ICSILog lookup table. Must be called
   once before any call to icsi_log().
   n is the number of bits to be taken from the mantissa
   (0<=n<=23)
   lookup_table is a pointer to a floating point array of 2^n positions.
*/
void fill_icsi_log_table(float* lookup_table, const uint32_t precision){
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

float icsi_log(float arg, 
		    const float* lookup_table,
		    const uint32_t precision){
  if(arg <= 0) 
    return 0;
  int32_t *const exp_ptr = ((int32_t*)&arg);
  int32_t x = *exp_ptr; //x is treated as integer
  const int32_t log_2 = ((x >> 23) & 255) - 127;//exponent
  x &= 0x7FFFFF; //mantissa
  x = x >> (23-precision); //quantize mantissa
  arg = lookup_table[x]; //lookup precomputed arg
  arg = ((arg + log_2)* 0.69314718);
  return arg; //natural logarithm
}
