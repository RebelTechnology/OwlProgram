#ifndef __fastlog_h__
#define __fastlog_h__

#include <stdint.h>

#ifdef __cplusplus
 extern "C" {
#endif

   void fill_icsi_log_table(float* lookup_table, const uint32_t precision);
   float icsi_log(const float val, const float* lookup_table, const uint32_t precision);

#ifdef __cplusplus
}
#endif

#endif // __basicmaths_h__
