
#include "message.h"
#include "basicmaths.h"
#include "SharedMemory.h"
#include <string.h>

static char buffer[64];

const char hexnumerals[] = "0123456789abcdef";

char* itoa(int val, int base){
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

char* ftoa(float val, int base){
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
  strncpy(buffer, msg, 64);
  getSharedMemory()->message = buffer;
}

void debugMessage(const char* msg, int a){
  char* p = buffer;
  p = stpncpy(p, msg, 48);
  p = stpcpy(p, itoa(a, 10));
  getSharedMemory()->message = buffer;
}

void debugMessage(const char* msg, int a, int b){
  char* p = buffer;
  p = stpncpy(p, msg, 32);
  p = stpcpy(p, itoa(a, 10));
  p = stpcpy(p, (const char*)" ");
  p = stpcpy(p, itoa(b, 10));
  getSharedMemory()->message = buffer;
}

void debugMessage(const char* msg, float a){
  char* p = buffer;
  p = stpncpy(p, msg, 48);
  p = stpcpy(p, ftoa(a, 10));
  getSharedMemory()->message = buffer;
}

void debugMessage(const char* msg, float a, float b){
  char* p = buffer;
  p = stpncpy(p, msg, 32);
  p = stpcpy(p, ftoa(a, 10));
  p = stpcpy(p, (const char*)" ");
  p = stpcpy(p, ftoa(b, 10));
  getSharedMemory()->message = buffer;
}

void assert_failed(const char* msg, const char* location, int line){
  char* p = buffer;
  p = stpncpy(p, msg, 32);
  p = stpcpy(p, (const char*)" in ");
  p = stpncpy(p, location, 32);
  p = stpcpy(p, (const char*)" line ");
  p = stpcpy(p, itoa(line, 10));
  getSharedMemory()->message = buffer;
  if(getSharedMemory()->programStatus != NULL)
    getSharedMemory()->programStatus(AUDIO_ERROR_STATUS);
}

void assert_failed(uint8_t* location, uint32_t line){
  assert_failed("Assertion Failed", (const char*)location, line);
}
