#include "Patch.h"
#include "ScreenBuffer.h"

typedef uint16_t Colour;
typedef ScreenBuffer<uint16_t> ColourScreenBuffer;

class ColourScreenPatch : public Patch {
public:
  ColourScreenPatch();
  virtual ~ColourScreenPatch();
  uint16_t getScreenWidth();
  uint16_t getScreenHeight();
  virtual void processScreen(ColourScreenBuffer& screen);
};
