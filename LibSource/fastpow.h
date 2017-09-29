#ifndef __fastpow_h__
#define __fastpow_h__

#include <stdint.h>

#ifdef __cplusplus
 extern "C" {
#endif

   void powFastSetTable(uint32_t* const pTable, const uint32_t  precision);
   float powFastLookup(const float val, const float ilog2, const uint32_t* pTable, const uint32_t precision);

#ifdef __cplusplus
}
#endif

#endif // __basicmaths_h__
