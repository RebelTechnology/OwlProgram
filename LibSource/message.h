#ifndef __MESSAGE_H
#define __MESSAGE_H

#include <stdint.h>

/* #ifdef DDEBUG */
/* #define DEBUG_ASSERT(cond, msg) if(!(cond)){debugAssertFailed(msg, __FILE__, __LINE__);} */
/* #else */
/* #define DEBUG_ASSERT(x, y) */
/* #endif */
/* #define ASSERT_TRUE(cond) if(!(cond)){debugAssertFailed("Assertion failed", __PRETTY_FUNCTION__, __LINE__);} */
/* #define ASSERT_FALSE(cond) if(cond){debugAssertFailed("Assertion failed", __PRETTY_FUNCTION__, __LINE__);} */
/* #define ASSERT(cond, msg) if(!(cond)){debugAssertFailed(msg, __PRETTY_FUNCTION__, __LINE__);} */

#define ASSERT(cond, msg) if(!(cond)){assert_failed(msg, __PRETTY_FUNCTION__, __LINE__);}

#ifndef CHECKSUM_ERROR_STATUS
#define CHECKSUM_ERROR_STATUS      -10
#endif
#ifndef OUT_OF_MEMORY_ERROR_STATUS
#define OUT_OF_MEMORY_ERROR_STATUS -20
#endif
#ifndef CONFIGURATION_ERROR_STATUS
#define CONFIGURATION_ERROR_STATUS -30
#endif
#ifndef PROGRAM_ERROR_STATUS
#define PROGRAM_ERROR_STATUS -40
#endif

#ifdef __cplusplus
 extern "C" {
#endif

   char* msg_itoa(int val, int base);
   char* msg_ftoa(float val, int base);

   void debugMessage(const char* msg);
   void error(int8_t code, const char* reason);
   void assert_failed(const char* msg, const char* location, int line);

#ifdef __cplusplus
}

void debugMessage(const char* msg, int);
void debugMessage(const char* msg, int, int);
void debugMessage(const char* msg, int, int, int);
void debugMessage(const char* msg, float);
void debugMessage(const char* msg, float, float);
void debugMessage(const char* msg, float, float, float);
void assert_failed(uint8_t* location, uint32_t line);

#endif

#endif /* __MESSAG£_H */
