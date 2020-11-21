#include "Patch.h"
#include "MonochromeScreenBuffer.h"

class MonochromeScreenPatch : public Patch {
public:
  MonochromeScreenPatch();
  virtual ~MonochromeScreenPatch();
  uint16_t getScreenWidth();
  uint16_t getScreenHeight();
  virtual void processScreen(MonochromeScreenBuffer& screen);
};
