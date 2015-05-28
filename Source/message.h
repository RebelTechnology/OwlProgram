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
#define ASSERT(cond, msg) if(!(cond)){debugAssertFailed(msg, __PRETTY_FUNCTION__, __LINE__);}

void debugMessage(const char* msg);
void debugMessage(const char* msg, int);
void debugMessage(const char* msg, int, int);
void debugMessage(const char* msg, float);
void debugMessage(const char* msg, float, float);
void debugAssertFailed(const char* msg, const char* location, int line);

#endif /* __MESSAG£_H */
