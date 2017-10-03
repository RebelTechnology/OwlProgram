/**

   http://www.hxa.name/articles/content/fast-pow-adjustable_hxa7241_2007.html

Copyright (c) 2007, Harrison Ainsworth / HXA7241.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
The name of the author may not be used to endorse or promote products derived from this software without specific prior written permission.
THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**/
#include "fastpow.h"
#include <math.h>

static const float _2p23 = 8388608.0f;

void powFastSetTable
(
   uint32_t* const pTable,
   const uint32_t  precision
)
{
   /* step along table elements and x-axis positions */
   float zeroToOne = 1.0f / ((float)(1 << precision) * 2.0f);        /* A */
   int32_t i;                                                        /* B */
   for( i = 0;  i < (1 << precision);  ++i )                         /* C */
   {
      /* make y-axis value for table element */
      const float f = ((float)powf( 2.0f, zeroToOne ) - 1.0f) * _2p23;
      pTable[i] = (uint32_t)( f < _2p23 ? f : (_2p23 - 1.0f) );
      zeroToOne += 1.0f / (float)(1 << precision);
   }                                                                 /* D */
}

/**
 * Get pow (fast!).
 *
 * @val        power to raise radix to
 * @ilog2      one over log, to required radix, of two
 * @pTable     length must be 2 ^ precision
 * @precision  number of mantissa bits used, >= 0 and <= 18
 */
float powFastLookup
(
   const float         val,
   const float         ilog2,
   const uint32_t* pTable,
   const uint32_t  precision
)
{
   /* build float bits */
   const int32_t i = (int32_t)( (val * (_2p23 * ilog2)) + (127.0f * _2p23) );

   /* replace mantissa with lookup */
   const int32_t it = (i & 0xFF800000) | pTable[(i & 0x7FFFFF) >>        /* E */
      (23 - precision)];                                             /* F */

   /* convert bits to float */
   return *(const float*)( &it );
}
