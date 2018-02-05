
#include "message.h"
#include "basicmaths.h"
#include "ProgramVector.h"
#include <string.h>

static char buffer[64];

const char hexnumerals[] = "0123456789abcdef";

char* msg_itoa(int val, int base){
  static char buf[13] = {0};
  int i = 11;
  unsigned int part = abs(val);
  do{
    buf[i--] = hexnumerals[part % base];
    part /= base;
  }while(part && i);
  if(val < 0)
    buf[i--] = '-';
  return &buf[i+1];
}

char* msg_ftoa(float val, int base){
  static char buf[16] = {0};
  int i = 14;
  // print 4 decimal points
  unsigned int part = abs((int)((val-int(val))*10000));
  do{
    buf[i--] = hexnumerals[part % base];
    part /= base;
  }while(i>10);
  buf[i--] = '.';
  part = abs(int(val));
  do{
    buf[i--] = hexnumerals[part % base];
    part /= base;
  }while(part && i);
  if(val < 0.0f)
    buf[i--] = '-';
  return &buf[i+1];
}

void debugMessage(const char* msg){
  strlcpy(buffer, msg, 64);
  getProgramVector()->message = buffer;
}

void debugMessage(const char* msg, int a){
  char* p = buffer;
  p = stpncpy(p, msg, 48);
  p = stpcpy(p, (const char*)" ");
  p = stpcpy(p, msg_itoa(a, 10));
  getProgramVector()->message = buffer;
}

void debugMessage(const char* msg, int a, int b){
  char* p = buffer;
  p = stpncpy(p, msg, 32);
  p = stpcpy(p, (const char*)" ");
  p = stpcpy(p, msg_itoa(a, 10));
  p = stpcpy(p, (const char*)" ");
  p = stpcpy(p, msg_itoa(b, 10));
  getProgramVector()->message = buffer;
}

void debugMessage(const char* msg, int a, int b, int c){
  char* p = buffer;
  p = stpncpy(p, msg, 32);
  p = stpcpy(p, (const char*)" ");
  p = stpcpy(p, msg_itoa(a, 10));
  p = stpcpy(p, (const char*)" ");
  p = stpcpy(p, msg_itoa(b, 10));
  p = stpcpy(p, (const char*)" ");
  p = stpcpy(p, msg_itoa(c, 10));
  getProgramVector()->message = buffer;
}

void debugMessage(const char* msg, float a){
  char* p = buffer;
  p = stpncpy(p, msg, 48);
  p = stpcpy(p, (const char*)" ");
  p = stpcpy(p, msg_ftoa(a, 10));
  getProgramVector()->message = buffer;
}

void debugMessage(const char* msg, float a, float b){
  char* p = buffer;
  p = stpncpy(p, msg, 32);
  p = stpcpy(p, (const char*)" ");
  p = stpcpy(p, msg_ftoa(a, 10));
  p = stpcpy(p, (const char*)" ");
  p = stpcpy(p, msg_ftoa(b, 10));
  getProgramVector()->message = buffer;
}

void debugMessage(const char* msg, float a, float b, float c){
  char* p = buffer;
  p = stpncpy(p, msg, 32);
  p = stpcpy(p, (const char*)" ");
  p = stpcpy(p, msg_ftoa(a, 10));
  p = stpcpy(p, (const char*)" ");
  p = stpcpy(p, msg_ftoa(b, 10));
  p = stpcpy(p, (const char*)" ");
  p = stpcpy(p, msg_ftoa(c, 10));
  getProgramVector()->message = buffer;
}

void error(int8_t code, const char* reason){
  ProgramVector* vec = getProgramVector();
  if(vec != NULL){
    vec->error = code;
    vec->message = (char*)reason;
    if(vec->programStatus != NULL)
      vec->programStatus(AUDIO_ERROR_STATUS);
  }
}

void assert_failed(const char* msg, const char* location, int line){
  char* p = buffer;
  p = stpncpy(p, msg, 32);
  p = stpcpy(p, (const char*)" in ");
  p = stpncpy(p, location, 32);
  p = stpcpy(p, (const char*)" line ");
  p = stpcpy(p, msg_itoa(line, 10));
  getProgramVector()->message = buffer;
  if(getProgramVector()->programStatus != NULL)
    getProgramVector()->programStatus(AUDIO_ERROR_STATUS);
}

void assert_failed(uint8_t* location, uint32_t line){
  assert_failed("Assertion Failed", (const char*)location, line);
}
