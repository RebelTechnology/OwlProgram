#include <cstddef>
#include <string.h>
#include "device.h"
#include "ColourScreenPatch.h"
#include "ProgramVector.h"
#include "PatchProcessor.h"
#include "ServiceCall.h"

#include "font.c" // todo load as resource

PatchProcessor* getInitialisingPatchProcessor();

void onDrawCallback(uint8_t* pixels, uint16_t width, uint16_t height){
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

void drawMessage(ColourScreenBuffer& screen){
  ProgramVector* pv = getProgramVector();
  if(pv->message != NULL){
    screen.setTextSize(1);
    screen.setTextWrap(true);
    screen.print(0, 26, pv->message);
  }    
}
void drawTitle(const char* title, ColourScreenBuffer& screen){
  // draw title
  screen.setTextSize(2);
  screen.print(0, 16, title);
}

void ColourScreenPatch::processScreen(ColourScreenBuffer& screen){
  // screen.clear();
  const char* title = getInitialisingPatchProcessor()->getPatchName();
  drawTitle(title, screen);
  drawMessage(screen);
  // const char title[] = "KickBox";
  // screen.setTextSize(2);
  // screen.print(0, 16, title);
}


Colour ScreenBuffer::getPixel(unsigned int x, unsigned int y){
  if(x >= width || y >= height)
    return 0;
  return pixels[y*width+x];
}

void ScreenBuffer::setPixel(unsigned int x, unsigned int y, Colour c){
  if(x < width && y < height)
    pixels[y*width+x] = c;
}

void ScreenBuffer::invertPixel(unsigned int x, unsigned int y){
  if(x < width && y < height)
    pixels[y*width+x] ^= WHITE;
}

void ScreenBuffer::fade(uint16_t steps){
  for(int i=0; i<height*width; ++i)
    pixels[i] = 
      (((pixels[i] & RED) >> steps) & RED) | 
      (((pixels[i] & GREEN) >> steps) & GREEN) |
      (((pixels[i] & BLUE) >> steps) & BLUE);
}

void ScreenBuffer::fill(Colour c) {
  for(int i=0; i<height*width; ++i)
    pixels[i] = c;
}

