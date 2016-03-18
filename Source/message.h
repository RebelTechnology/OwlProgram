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


#ifdef __cplusplus
 extern "C" {
#endif

   char* ftoa(float val, int base);
   char* itoa(int val, int base);
   void debugMessage(const char* msg);
   void error(int8_t code, const char* reason);
   void assert_failed(const char* msg, const char* location, int line);

#ifdef __cplusplus
}

void debugMessage(const char* msg, int);
void debugMessage(const char* msg, int, int, int);
void debugMessage(const char* msg, float);
void debugMessage(const char* msg, float, float);
void debugMessage(const char* msg, float, float, float);
void assert_failed(uint8_t* location, uint32_t line);

#endif

#endif /* __MESSAG£_H */
