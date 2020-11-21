#include <cstddef>
#include <string.h>
#include "device.h"
#include "ColourScreenPatch.h"
#include "ProgramVector.h"
#include "PatchProcessor.h"
#include "ServiceCall.h"

PatchProcessor* getInitialisingPatchProcessor();

static void onDrawCallback(uint8_t* pixels, uint16_t width, uint16_t height){
  ColourScreenPatch* patch = (ColourScreenPatch*)getInitialisingPatchProcessor()->patch;
  if(patch != NULL){
    ColourScreenBuffer screen(width, height);
    screen.setBuffer(pixels);
    patch->processScreen(screen);
  }
}

ColourScreenPatch::ColourScreenPatch(){
  void* drawArgs[] = {(void*)SYSTEM_FUNCTION_DRAW, (void*)&onDrawCallback};
  getProgramVector()->serviceCall(OWL_SERVICE_REGISTER_CALLBACK, drawArgs, 2);
}

ColourScreenPatch::~ColourScreenPatch(){}

template<>
Colour ColourScreenBuffer::getPixel(unsigned int x, unsigned int y){
  if(x >= width || y >= height)
    return 0;
  return pixels[y*width+x];
}

template<>
void ColourScreenBuffer::setPixel(unsigned int x, unsigned int y, Colour c){
  if(x < width && y < height)
    pixels[y*width+x] = c;
}

template<>
void ColourScreenBuffer::invertPixel(unsigned int x, unsigned int y){
  if(x < width && y < height)
    pixels[y*width+x] ^= WHITE;
}

template<>
void ColourScreenBuffer::fade(uint16_t steps){
  for(int i=0; i<height*width; ++i)
    pixels[i] = 
      (((pixels[i] & RED) >> steps) & RED) | 
      (((pixels[i] & GREEN) >> steps) & GREEN) |
      (((pixels[i] & BLUE) >> steps) & BLUE);
}

template<>
void ColourScreenBuffer::fill(Colour c) {
  for(int i=0; i<height*width; ++i)
    pixels[i] = c;
}

