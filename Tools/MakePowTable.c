/**
 * gcc LibSource/fastpow.c Tools/MakePowTable.c -lm -o pow
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "../LibSource/fastpow.h"
#include <err.h>

#define DEFAULT_PRECISION 11

int main(int argc, char** argv) {
  uint32_t precision = DEFAULT_PRECISION;
  if(argc > 1)
    precision = atoi(argv[1]);
  if(precision > 18)
    errx(1, "Invalid precision %d, must be [0,18]", precision);
  unsigned int* lookup_table;
  int table_size = (1 << precision);
  lookup_table = (unsigned int*)malloc(sizeof(unsigned int)*table_size);
  powFastSetTable(lookup_table, precision);
  printf("/* fast pow lookup table, precision %u, size %u */\n", precision, table_size);
  printf("const uint32_t fast_pow_precision = %u;\n", precision);
  printf("const uint32_t fast_pow_table_size = %u;\n", table_size);
  printf("const uint32_t fast_pow_table[%u] = {\n", table_size);
  int32_t i;
  for(i=0; i<table_size; ++i)
    printf("%u, ", lookup_table[i]);
  printf(" };\n");
  return 0;
}
