/**
 * gcc LibSource/fastlog.c Tools/MakeLogTable.c -lm -o log
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../LibSource/fastlog.h"

#define DEFAULT_PRECISION 14

int main(int argc, char** argv) {
  uint32_t precision = DEFAULT_PRECISION;
  if(argc > 1)
    precision = atoi(argv[1]);
  if(precision > 18)
    errx(1, "Invalid precision %d, must be [0,18]", precision);
  float* lookup_table;
  int table_size = (1 << precision);
  lookup_table = (float*)malloc(sizeof(float)*table_size);
  fill_icsi_log_table(lookup_table, precision);
  printf("/* fast log lookup table, precision %d, size %d */\n", precision, table_size);
  printf("const uint32_t fast_log_precision = %d;\n", precision);
  printf("const uint32_t fast_log_table_size = %d;\n", table_size);
  printf("const float fast_log_table[%d] = {\n", table_size);
  int32_t i;
  for(i=0; i<table_size; ++i)
    printf("%e, ", lookup_table[i]);
  printf(" };\n");
  free(lookup_table);
  return 0;
}
