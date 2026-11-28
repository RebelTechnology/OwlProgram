#ifndef __WavTestData_h__
#define __WavTestData_h__

#include <string.h>
#include <stdint.h>

// Build a minimal RIFF/WAVE file in memory: 44-byte header plus interleaved data.
// Returns the total size in bytes; out must hold 44 + data bytes.
static size_t makeWav(uint8_t* out, uint16_t format, uint16_t channels, uint16_t bps,
		      const void* data, uint32_t databytes){
  uint32_t u32; uint16_t u16;
  memcpy(out, "RIFF", 4);
  u32 = 36 + databytes; memcpy(out+4, &u32, 4);
  memcpy(out+8, "WAVEfmt ", 8);
  u32 = 16; memcpy(out+16, &u32, 4);
  memcpy(out+20, &format, 2);
  memcpy(out+22, &channels, 2);
  u32 = 48000; memcpy(out+24, &u32, 4);
  u32 = 48000*channels*(bps/8); memcpy(out+28, &u32, 4);
  u16 = channels*(bps/8); memcpy(out+32, &u16, 2);
  memcpy(out+34, &bps, 2);
  memcpy(out+36, "data", 4);
  memcpy(out+40, &databytes, 4);
  memcpy(out+44, data, databytes);
  return 44 + databytes;
}

#endif // __WavTestData_h__
