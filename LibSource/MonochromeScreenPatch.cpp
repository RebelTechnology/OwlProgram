#include <cstddef>
#include <string.h>
#include "device.h"
#include "MonochromeScreenPatch.h"
#include "ProgramVector.h"
#include "PatchProcessor.h"
#include "ServiceCall.h"

#include "font.c" // todo load as resource

PatchProcessor* getInitialisingPatchProcessor();

void onDrawCallback(uint8_t* pixels, uint16_t width, uint16_t height){
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

void drawMessage(MonochromeScreenBuffer& screen){
  ProgramVector* pv = getProgramVector();
  if(pv->message != NULL){
    screen.setTextSize(1);
    screen.setTextWrap(true);
    screen.print(0, 26, pv->message);
  }    
}
void drawTitle(const char* title, MonochromeScreenBuffer& screen){
  // draw title
  screen.setTextSize(2);
  screen.print(0, 16, title);
}

void MonochromeScreenPatch::processScreen(MonochromeScreenBuffer& screen){
  // screen.clear();
  const char* title = getInitialisingPatchProcessor()->getPatchName();
  drawTitle(title, screen);
  drawMessage(screen);
  // const char title[] = "KickBox";
  // screen.setTextSize(2);
  // screen.print(0, 16, title);
}


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

// Draw a character
template<typename Colour>
void ScreenBuffer<Colour>::drawChar(uint16_t x, uint16_t y, unsigned char ch,
                            Colour c, Colour bg, uint8_t size) {
  // if((x >= width)            || // Clip right
  //    (y >= height)           || // Clip bottom
  //    ((x + 6 * size - 1) < 0) || // Clip left
  //    ((y + 8 * size - 1) < 0))   // Clip top
  //   return;
  y -= 8 * size; // set origin to bottom left
  for(int8_t i=0; i<6; i++ ) {
    uint8_t line;
    if (i == 5) 
      line = 0x0;
    else 
      line = font[(ch*5)+i];
    for (int8_t j = 0; j<8; j++) {
      if (line & 0x1) {
        if (size == 1) // default size
          setPixel(x+i, y+j, c);
        else {  // big size
          fillRectangle(x+(i*size), y+(j*size), size, size, c);
        } 
      } else if (bg != c) {
        if (size == 1) // default size
          setPixel(x+i, y+j, bg);
        else {  // big size
          fillRectangle(x+i*size, y+j*size, size, size, bg);
        }
      }
      line >>= 1;
    }
  }
}

// Draw a character rotated 90 degrees
template<typename Colour>
void ScreenBuffer<Colour>::drawRotatedChar(uint16_t x, uint16_t y, unsigned char ch,
                                   Colour c, Colour bg, uint8_t size) {
  // if((x >= width)            || // Clip right
  //    (y >= height)           || // Clip bottom
  //    ((x + 8 * size - 1) < 0) || // Clip left
  //    ((y + 6 * size - 1) < 0))   // Clip top
  //   return;
  // for (int8_t i=5; i>=0; i-- ) {
  for (int8_t i=0; i<6; i++ ) {
    uint8_t line;
    if (i == 5)
      line = 0x0;
    else 
      line = font[(ch*5)+i];
    // for (int8_t j = 0; j<8; j++) {
    for (int8_t j = 7; j>=0; j--) {
      if (line & 0x1) {
        if (size == 1) // default size
          setPixel(y+i, x+j, c);
        else {  // big size
          // fillRectangle(x+(i*size), y+(j*size), size, size, c);
          fillRectangle(y+(j*size), x+(i*size), size, size, c);
        } 
      } else if (bg != c) {
        if (size == 1) // default size
          setPixel(y+i, x+j, bg);
        else {  // big size
          // fillRectangle(x+i*size, y+j*size, size, size, bg);
          fillRectangle(y+j*size, x+i*size, size, size, bg);
        }
      }
      line >>= 1;
    }
  }
}
