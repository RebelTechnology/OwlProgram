/* Copyright 2014 Section6. All Rights Reserved. */

#include "Utils_unix.h"

#if HV_UNIX

hv_size_t hv_max_ui(hv_size_t x, hv_size_t y) {
  return (x >= y) ? x : y;
}

hv_size_t hv_min_ui(hv_size_t x, hv_size_t y) {
  return (x <= y) ? x : y;
}

int hv_max_i(int x, int y) {
  return (x >= y) ? x : y;
}

int hv_min_i(int x, int y) {
  return (x <= y) ? x : y;
}

hv_uint32_t hv_min_max_log2(hv_uint32_t x) {
  // finds ceil(log2(x))
  // http://stackoverflow.com/questions/2589096/find-most-significant-bit-left-most-that-is-set-in-a-bit-array
  return (hv_uint32_t) ((8 * sizeof(unsigned int)) - __builtin_clz(x - 1));
}

#endif // HV_UNIX
