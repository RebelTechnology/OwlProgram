#include "TestPatch.hpp"
#include "TakeoverValue.h"

/*
 * Unit test for TakeoverValue, documented soft-takeover ("Match") logic: once
 * locked, updates are ignored until the incoming value falls within a threshold
 * of the previously set value, at which point it unlocks and resumes tracking.
 * Tests assert that documented behaviour: locking blocks updates, a far value
 * leaves it locked, a value within threshold unlocks and takes over, and manual
 * unlock resumes tracking immediately.
 */
class TakeoverValueTestPatch : public TestPatch {
public:
  TakeoverValueTestPatch(){
    {
      TEST("unlocked tracks updates");
      TakeoverValue<float> v(0.1f, 0.5f); // threshold 0.1, init 0.5
      CHECK_CLOSE((float)v, 0.5f, 0.0001);
      v = 0.8f; // unlocked by default
      CHECK_CLOSE((float)v, 0.8f, 0.0001);
    }
    {
      TEST("locked blocks far updates");
      TakeoverValue<float> v(0.1f, 0.5f);
      v.setLocked(true);
      CHECK(v.isLocked());
      v = 0.9f; // far from 0.5, beyond threshold 0.1
      CHECK_CLOSE((float)v, 0.5f, 0.0001); // value unchanged
      CHECK(v.isLocked());               // still locked
    }
    {
      TEST("value within threshold takes over");
      TakeoverValue<float> v(0.1f, 0.5f);
      v.setLocked(true);
      v = 0.55f; // within threshold 0.1 of 0.5 -> unlocks
      CHECK(!v.isLocked());
      // Subsequent updates now track normally.
      v = 0.9f;
      CHECK_CLOSE((float)v, 0.9f, 0.0001);
    }
    {
      TEST("manual unlock resumes tracking");
      TakeoverValue<float> v(0.1f, 0.2f);
      v.setLocked(true);
      v = 0.95f; // ignored while locked
      CHECK_CLOSE((float)v, 0.2f, 0.0001);
      v.setLocked(false);
      v = 0.95f; // now accepted
      CHECK_CLOSE((float)v, 0.95f, 0.0001);
    }
    {
      TEST("relative update honours lock");
      TakeoverValue<float> v(0.1f, 1.0f);
      v.setLocked(true);
      v += 1.0f; // target 2.0, far from 1.0 -> ignored, stays locked
      CHECK_CLOSE((float)v, 1.0f, 0.0001);
      CHECK(v.isLocked());
    }
  }
};
