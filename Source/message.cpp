
#include "message.h"
#include "SharedMemory.h"
#include <string.h>

static char buffer[64];

char* itoa(int val, int base){
  static char buf[13] = {0};
  bool neg = false;
  if(val == 0){
    buf[11] = '0';
    return &buf[11];
  }else if(val < 0){
    neg = true;
    val *= -1;
  }
  int i=11;
  for(; val && i ; --i, val /= base)
    buf[i] = "0123456789abcdef"[val % base];
  if(neg)
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
