/**
 * gcc Tools/MakeLogTable.c -lm -o log
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PRECISION 14

/* Creates the ICSILog lookup table.
 */
static void fill_icsi_log_table(float* lookup_table, uint32_t precision){
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

int main(int argc, char** argv) {
  uint32_t precision = PRECISION;
  float* lookup_table;
  int table_size = (1 << precision);
  lookup_table = (float*)malloc(sizeof(float)*table_size);
  fill_icsi_log_table(lookup_table, precision);
  printf("/* fast log lookup table, precision %d, size %d */\n", precision, table_size);
  printf("const unsigned int fast_log_precision = %d;\n", precision);
  printf("const unsigned int fast_log_table_size = %d;\n", table_size);
  printf("const float fast_log_table[%d] = {\n", table_size);
  int32_t i;
  for(i=0; i<table_size; ++i)
    printf("%e, ", lookup_table[i]);
  printf(" };\n");
  free(lookup_table);
  return 0;
}
