#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#define PRECISION 11

/**
 * Initialize powFast lookup table.
 *
 * @pTable     length must be 2 ^ precision
 * @precision  number of mantissa bits used, >= 0 and <= 18
 */
const float _2p23 = 8388608.0f;

void powFastSetTable
(
   unsigned int* const pTable,
   const unsigned int  precision
)
{
   /* step along table elements and x-axis positions */
   float zeroToOne = 1.0f / ((float)(1 << precision) * 2.0f);        /* A */
   int   i;                                                          /* B */
   for( i = 0;  i < (1 << precision);  ++i )                         /* C */
   {
      /* make y-axis value for table element */
      const float f = ((float)powf( 2.0f, zeroToOne ) - 1.0f) * _2p23;
      pTable[i] = (unsigned int)( f < _2p23 ? f : (_2p23 - 1.0f) );
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
   unsigned int* const pTable,
   const unsigned int  precision
)
{
   /* build float bits */
   const int i = (int)( (val * (_2p23 * ilog2)) + (127.0f * _2p23) );

   /* replace mantissa with lookup */
   const int it = (i & 0xFF800000) | pTable[(i & 0x7FFFFF) >>        /* E */
      (23 - precision)];                                             /* F */

   /* convert bits to float */
   return *(const float*)( &it );
}

int main(int argc, char** argv) {
  uint32_t precision = PRECISION;
  unsigned int* lookup_table;
  int table_size = (1 << precision);
  lookup_table = (unsigned int*)malloc(sizeof(unsigned int)*table_size);
  powFastSetTable(lookup_table, PRECISION);
  // fill_icsi_log_table(lookup_table, table_size);
  printf("/* pow table, precision %d, size %d */\n", precision, table_size);
  printf("const int icsi_table[%d] = {\n", table_size);
  int32_t i;
  for(i=0; i<table_size; ++i)
    printf("%d, ", lookup_table[i]);
  printf(" };\n");
  return 0;
}
