/* Copyright 2014 Section6. All Rights Reserved. */

#include <stdio.h>
#include <stdlib.h>

#include "Heavy_heavy.h"

void printHook(double timestamp, const char *name, const char *s, void *c) {
  printf("[@ %.3f] %s: %s\n", timestamp, name, s);
}

int main(int argc, const char *argv[]) {
  Hv_heavy *context = hv_heavy_new(0, 0, 44100.0);
  hv_setPrintHook(context, &printHook);
  hv_heavy_process(context, NULL, NULL, HV_N_SIMD);
  hv_heavy_free(context);
  return 0;
}
