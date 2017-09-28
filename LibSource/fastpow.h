#ifndef __fastpow_h__
#define __fastpow_h__

#ifdef __cplusplus
 extern "C" {
#endif

   void powFastSetTable(unsigned int* const pTable, const unsigned int  precision);
   float powFastLookup(const float val, const float ilog2, const unsigned int* pTable, const unsigned int precision);

#ifdef __cplusplus
}
#endif

#endif // __basicmaths_h__
