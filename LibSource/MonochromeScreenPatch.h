#include "Patch.h"
#include "ScreenBuffer.h"

typedef uint8_t Colour;
typedef ScreenBuffer<uint8_t> MonochromeScreenBuffer;

class MonochromeScreenPatch : public Patch {
public:
  MonochromeScreenPatch();
  virtual ~MonochromeScreenPatch();
  uint16_t getScreenWidth();
  uint16_t getScreenHeight();
  virtual void processScreen(MonochromeScreenBuffer& screen);
};
