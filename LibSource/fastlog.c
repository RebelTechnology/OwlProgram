/*  Lookup table generator for the ICSI logarithm function
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
    /* step along table elements and x-axis positions
      (start with extra half increment, so the steps intersect at their midpoints.) */
  float oneToTwo = 1.0f + (1.0f / (float)( 1 <<(precision + 1) ));
  int32_t i;
  for(i = 0; i < (1 << precision); ++i){
    // make y-axis value for table element
    lookup_table[i] = logf(oneToTwo) / 0.69314718055995f;
    oneToTwo += 1.0f / (float)( 1 << precision );
  }
}

float icsi_log(float arg, const float* lookup_table, const uint32_t precision){
  /* get access to float bits */
  register const int32_t* const pVal = (const int32_t*)(&arg);
  /* extract exponent and mantissa (quantized) */
  register const int32_t exp = ((*pVal >> 23) & 255) - 127;
  register const int32_t man = (*pVal & 0x7FFFFF) >> (23 - precision);
  /* exponent plus lookup refinement */
  return ((float)(exp) + lookup_table[man]) * 0.69314718055995f;
}
