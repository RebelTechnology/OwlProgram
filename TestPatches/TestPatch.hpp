#ifndef __TestPatch_hpp__
#define __TestPatch_hpp__

#include <float.h>
#include "Patch.h"

#define TEST(name) do{ errormessage = (char*)("Test " name " failed line"); }while(0)
#define CHECK(cond) if(!(cond)){ fail("CHECK(" #cond ") fail", __LINE__); }else{ pass(); }
// #define CHECK_EQUAL(a, b) if((a) != (b)){ fail("CHECK_EQUAL(" #a ", " #b ") fail", __LINE__); }else{ pass(); }
// #define CHECK_CLOSE(a, b, c) if(abs((a)-(b))>(c)){ fail("CHECK_CLOSE(" #a ", " #b ", " #c ") fail", __LINE__); }else{ pass(); }
#define CHECK_RELATIVE(a, b, c) if(abs(((a)-(b))/(a))>(c)){ fail("CHECK_RELATIVE(" #a ", " #b ", " #c ") fail", __LINE__); }else{ pass(); }
#define REQUIRE(cond) if(!(cond)){ assert_failed("REQUIRE(" #cond ") fail", "", __LINE__); }else{ pass(); }

/* alternatively use templates to check different typed values */
#define CHECK_EQUAL(a, b) check_equal(a, b, __LINE__)
#define CHECK_CLOSE(a, b, c) check_close(a, b, c, __LINE__)

/** Abstract base class for tests */
class TestPatch : public Patch {
public:
  const float DEFAULT_TOLERANCE = 2*FLT_EPSILON;
  const char* DEFAULT_MESSAGE = (char*)"Tests passed/failed";
  bool success;
  int passed;
  int failed;
  char* errormessage;
  void pass(){
    passed++;
  }
  void fail(const char* condition, int line){
    debugMessage(condition);
    debugMessage(errormessage, line);
    success = false;
    failed++;
  }
  bool check_equal(float a, float b, int line){
    if(a != b){
      debugMessage("CHECK_EQUAL", a, b);
      debugMessage(errormessage, line);
      success = false;
      failed++;
      return false;
    }else{
      pass();
      return true;
    }
  }
  bool check_close(float a, float b, float c, int line){
    if(abs((a)-(b))>(c)){
      debugMessage("CHECK_CLOSE", a, b, c);
      debugMessage(errormessage, line);
      success = false;
      failed++;
      return false;
    }else{
      pass();
      return true;
    }
  }
  TestPatch(): success(true), passed(0), failed(0), errormessage((char*)DEFAULT_MESSAGE){
  }

  void processAudio(AudioBuffer &buffer){
    FloatArray sig = buffer.getSamples(LEFT_CHANNEL);
    if(success){
      static float phase = 0;
      float inc = 2*M_PI/200.0f;
      for(int n=0; n<getBlockSize(); n++){
        sig[n] = 0.4*sinf(phase);
        phase += inc;
        phase = phase>2*M_PI ? phase-2*M_PI : phase;
      }
    }else{
      for(int n=0; n<getBlockSize(); n++){
        sig[n] += 0.2*randf();
      }
      // debugMessage("Tests failed");
      error(PROGRAM_ERROR_STATUS, "Tests failed");
    }
  }
};

#endif // __TestPatch_hpp__
