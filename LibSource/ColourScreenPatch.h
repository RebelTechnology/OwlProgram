#ifndef __ColourScreenPatch_h__
#define __ColourScreenPatch_h__

#include "Patch.h"
#include "ScreenBuffer.h"

#define BLACK    0x0000
#define BLUE     0x001F
#define RED      0xF800
#define GREEN    0x07E0
#define CYAN     0x07FF
#define MAGENTA  0xF81F
#define YELLOW   0xFFE0  
#define WHITE    0xFFFF

typedef uint16_t Colour;
typedef ScreenBuffer<uint16_t, BLACK, WHITE> ColourScreenBuffer;

/**
 * Abstract base class for patches that use a colour screen.
 * Colour format is 16 bits RGB565: RRRRR GGGGGG BBBBB
 * Compatible OLED drivers: SSD1331, SEPS1114A
 */
class ColourScreenPatch : public Patch {
public:
  ColourScreenPatch();
  virtual ~ColourScreenPatch();
  uint16_t getScreenWidth();
  uint16_t getScreenHeight();
  virtual void processScreen(ColourScreenBuffer& screen) = 0;
};

#endif // __ColourScreenPatch_h__
