#include "basicmaths.h"
#include <stdint.h>
#include <string.h>
#include "fastpow.h"
#include "fastlog.h"

#ifdef ARM_CORTEX
/* The realloc() function changes the size of the memory block pointed  to */
/* by ptr to size bytes.  The contents will be unchanged in the range from */
/* the start of the region up to the minimum of the old and new sizes.  If */
/* the  new size is larger than the old size, the added memory will not be */
/* initialized.  If ptr is NULL, then  the  call  is  equivalent  to  mal‐ */
/* loc(size), for all values of size; if size is equal to zero, and ptr is */
/* not NULL, then the call is equivalent  to  free(ptr).   Unless  ptr  is */
/* NULL,  it  must have been returned by an earlier call to malloc(), cal‐ */
/* loc(), or realloc().  If the area pointed to was moved, a free(ptr)  is */
/* done. */
void* pvPortRealloc(void *ptr, size_t new_size) {
  if(ptr == NULL) 
    return pvPortMalloc(new_size);
  size_t old_size = vPortGetSizeBlock(ptr);
  if(new_size == 0){
    vPortFree(ptr);
    return NULL;
  }
  if(new_size <= old_size)
    return ptr;
  void* p = pvPortMalloc(new_size);
  if(p == NULL)
    return p;
  memcpy(p, ptr, old_size);
  vPortFree(ptr);
  return p;
}

/* The calloc() function allocates memory for an array of  nmemb  elements */
/* of  size bytes each and returns a pointer to the allocated memory. */
/* The memory is set to zero. */
void *pvPortCalloc(size_t nmemb, size_t size){						  
  size_t xWantedSize = nmemb*size;
  void* ptr = pvPortMalloc(xWantedSize);
  if(ptr != NULL)
    memset(ptr, 0, xWantedSize);
  return ptr;
}
#endif

// todo: see
// http://www.hxa.name/articles/content/fast-pow-adjustable_hxa7241_2007.html
// http://www.finesse.demon.co.uk/steven/sqrt.html
// http://www.keil.com/forum/7934/
// http://processors.wiki.ti.com/index.php/ARM_compiler_optimizations

  /* void *_sbrk(intptr_t increment){} */

static uint32_t r32seed = 33641;

void arm_srand32(uint32_t s){
  r32seed = s;
}

/**
 * Generate an unsigned 32bit pseudo-random number using xorshifter algorithm. Aka xorshifter32.
 * "Anyone who considers arithmetical methods of producing random digits is, of course, in a state of sin." 
 * -- John von Neumann.
*/
uint32_t arm_rand32(){
  r32seed ^= r32seed << 13;
  r32seed ^= r32seed >> 17;
  r32seed ^= r32seed << 5;
  return r32seed;
}

float randf(){
  return arm_rand32()*(1/4294967296.0f);
}

float arm_sqrtf(float in){
  float out;
#ifdef ARM_CORTEX
  arm_sqrt_f32(in, &out);
#else
  out=sqrtf(in);
#endif
  return out;
}

/* Fast arctan2
 * from http://dspguru.com/dsp/tricks/fixed-point-atan2-with-self-normalization
 */
float fast_atan2f(float y, float x){
  const float coeff_1 = M_PI/4;
  const float coeff_2 = 3*M_PI/4;
  float abs_y = fabs(y)+1e-10; // kludge to prevent 0/0 condition
  float r, angle;
  if (x>=0){
    r = (x - abs_y) / (x + abs_y);
    angle = coeff_1 - coeff_1 * r;
  }else{
    r = (x + abs_y) / (abs_y - x);
    angle = coeff_2 - coeff_1 * r;
  }
  if(y < 0)
    return(-angle); // negate if in quad III or IV
  else
    return(angle);
}

/* static const float* log_table = fast_log_table; */
/* static uint32_t log_precision = fast_log_precision; */
/* static const uint32_t* pow_table = fast_pow_table; */
/* static uint32_t pow_precision = fast_pow_precision; */

static const float* log_table;
static uint32_t log_precision;
static const uint32_t* pow_table;
static uint32_t pow_precision;

#define M_LOG210 3.32192809488736

float fast_powf(float x, float y){
  return powFastLookup(y, logf(x)*M_LOG2E, pow_table, pow_precision);
}
    
float fast_expf(float x){
  return powFastLookup(x, M_LOG2E, pow_table, pow_precision);
}

float fast_exp2f(float x){
  return powFastLookup(x, 1, pow_table, pow_precision);
}

float fast_exp10f(float x){
  return powFastLookup(x, M_LOG210, pow_table, pow_precision);
}

float fast_logf(float x){
  return icsi_log(x, log_table, log_precision);
}

float fast_log10f(float x){
  /* log10 (x) equals log (x) / log (10). */
  return icsi_log(x, log_table, log_precision) / M_LN10;
}

float fast_log2f(float x){
  /* log2 (x) equals log (x) / log (2). */
  return icsi_log(x, log_table, log_precision) / M_LN2;
}

void fast_pow_set_table(const uint32_t* table, int size){
  pow_table = table;
  pow_precision = fast_log2i(size);
}

void fast_log_set_table(const float* table, int size){
  log_table = table;
  log_precision = fast_log2i(size);
}

float fast_fmodf(float x, float y) {
  float a = x/y;
  return (a-(int)a)*y;
}

uint32_t fast_log2i(uint32_t x){
  return 31 - __builtin_clz (x); /* clz returns the number of leading 0's */
}
