#ifndef __TestPatch_hpp__
#define __TestPatch_hpp__

#include "Patch.h"

#define TEST(name) do{ errormessage = (char*)("Test " name " failed line"); }while(0);
#define CHECK(cond) if(!cond){ fail(errormessage, __LINE__); }else{ pass(); }
#define CHECK_EQUAL(a, b) if(a != b){ fail(errormessage, __LINE__); }else{ pass(); }
// #define CHECK_CLOSE(a, b) if(abs(a-b)>DEFAULT_TOLERANCE){ fail(errormessage, __LINE__); }else{ pass(); }
#define CHECK_CLOSE(a, b, c) if(abs(a-b)>c){ fail(errormessage, __LINE__); }else{ pass(); }

/** Abstract base class for tests */
class TestPatch : public Patch {
public:
  const float DEFAULT_TOLERANCE = 0.0000001;
  const char* DEFAULT_MESSAGE = (char*)"Tests passed/failed";
protected:
  bool success;
  int passed;
  int failed;
  char* errormessage;
  // void CHECK(bool condition, const char* message){
  //   assert(condition, message);
  // }
  // void CHECK_CLOSE(float first, float second, const char *message, 
  // 		   float tolerance=DEFAULT_TOLERANCE){
  //   bool cond = abs(second-first)<tolerance;
  //   assert(cond, message);
  // }
// template<type T>
//   void CHECK_EQUAL(T a, T b, const char* message){
//     assert(a == b, message);
//   }
  void pass(){
    passed++;
  }
  void fail(const char* message, int line){
    if(success)
      debugMessage(message, line);
    success = false;
    failed++;
  }
public:
  TestPatch(): success(true), passed(0), failed(0), errormessage((char*)DEFAULT_MESSAGE){
  }

  virtual void processAudio(AudioBuffer &buffer){
    float *sig=buffer.getSamples(0);
    if(!success){
      for(int n=0; n<getBlockSize(); n++){
        sig[n]+=0.05*rand()/(float)RAND_MAX;
      }
    }else{
      static float phase=0;
      float inc=2*M_PI/200.0f;
      for(int n=0; n<getBlockSize(); n++){
        sig[n]+=0.2*sinf(phase);
        phase+=inc;
        phase= phase>2*M_PI ? phase-2*M_PI : phase;
      }
    }
    if(success)
      debugMessage("Tests passed", passed);
  }
};

#endif // __TestPatch_hpp__
