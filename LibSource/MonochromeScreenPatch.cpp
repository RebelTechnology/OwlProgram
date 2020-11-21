#include <cstddef>
#include <string.h>
#include "device.h"
#include "MonochromeScreenPatch.h"
#include "ProgramVector.h"
#include "PatchProcessor.h"
#include "ServiceCall.h"

#include "font.c" // todo load as resource

PatchProcessor* getInitialisingPatchProcessor();

static void onDrawCallback(uint8_t* pixels, uint16_t width, uint16_t height){
  MonochromeScreenPatch* patch = (MonochromeScreenPatch*)getInitialisingPatchProcessor()->patch;
  if(patch != NULL){
    MonochromeScreenBuffer screen(width, height);
    screen.setBuffer(pixels);
    patch->processScreen(screen);
  }
}

MonochromeScreenPatch::MonochromeScreenPatch(){
  void* drawArgs[] = {(void*)SYSTEM_FUNCTION_DRAW, (void*)&onDrawCallback};
  getProgramVector()->serviceCall(OWL_SERVICE_REGISTER_CALLBACK, drawArgs, 2);
}

MonochromeScreenPatch::~MonochromeScreenPatch(){}

template<>
Colour MonochromeScreenBuffer::getPixel(unsigned int x, unsigned int y){
  if(x >= width || y >= height)
    return 0;
  uint8_t  ucByteOffset = 0;
  uint16_t usiArrayLoc = 0;
  // Determine array location
  usiArrayLoc = (y/8)+(x*8);
  // Determine byte offset
  ucByteOffset = y-((uint8_t)(y/8)*8);
  // Return bit state from buffer
  return pixels[usiArrayLoc] & (1 << ucByteOffset);
  // return pixels[y*width+x];
}

template<>
void MonochromeScreenBuffer::setPixel(unsigned int x, unsigned int y, Colour c){
  if(x < width && y < height){
    uint8_t  ucByteOffset = 0;
    uint16_t usiArrayLoc = 0;
    // Determine array location
    usiArrayLoc = (y/8)+(x*8);
    // Determine byte offset
    ucByteOffset = y-((uint8_t)(y/8)*8);		
    // Set pixel in buffer
    if(c == BLACK)
      pixels[usiArrayLoc] &= ~(1 << ucByteOffset);
    else
      pixels[usiArrayLoc] |= (1 << ucByteOffset);
  }
}

template<>
void MonochromeScreenBuffer::invertPixel(unsigned int x, unsigned int y){
  if(x < width && y < height){
    uint8_t  ucByteOffset = 0;
    uint16_t usiArrayLoc = 0;
    // Determine array location
    usiArrayLoc = (y/8)+(x*8);
    // Determine byte offset
    ucByteOffset = y-((uint8_t)(y/8)*8);
    uint8_t pixel = (1 << ucByteOffset);
    // Set pixel in buffer
    if(pixels[usiArrayLoc] & pixel)
      pixels[usiArrayLoc] &= ~pixel;
    else
      pixels[usiArrayLoc] |= pixel;
  }
}

template<>
void MonochromeScreenBuffer::fade(uint16_t steps){
  // for(unsigned int i=0; i<height*width; ++i)
  //   pixels[i] = 
  //     (((pixels[i] & RED) >> steps) & RED) | 
  //     (((pixels[i] & GREEN) >> steps) & GREEN) |
  //     (((pixels[i] & BLUE) >> steps) & BLUE);
  // todo!

  // todo: update contrast setting
}

template<>
void MonochromeScreenBuffer::fill(Colour c) {
  memset(pixels, c, height*width/8);
  // for(unsigned int i=0; i<height*width; ++i)
  //   pixels[i] = c;
}
