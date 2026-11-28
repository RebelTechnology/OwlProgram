#include "TestPatch.hpp"
#include "WavTestData.h"
#include "../GenSource/genlib.cpp"
#include <stdio.h>
#include <unistd.h>

/*
 * Unit test for gen~ data loaded from a WAV resource (genlib.cpp): a data
 * object named after a resource is filled from the WAV file, interleaved;
 * a declared size that differs keeps the overlapping frames and reports
 * the mismatch with debugMessage(); plain data is still zeroed on resize.
 * The native test harness loads resources from files and prints debug
 * messages to stdout, which the test captures.
 */
class GenDataResourceTestPatch : public TestPatch {
public:
  static void writeFile(const char* name, const uint8_t* data, size_t len){
    FILE* f = fopen(name, "wb");
    fwrite(data, 1, len, f);
    fclose(f);
  }
  // run fn with stdout redirected; return whether its output contains text
  template<typename F>
  static bool printed(const char* text, F fn){
    fflush(stdout);
    int saved = dup(1);
    FILE* tmp = tmpfile();
    dup2(fileno(tmp), 1);
    fn();
    fflush(stdout);
    dup2(saved, 1);
    close(saved);
    char out[1024] = {};
    rewind(tmp);
    fread(out, 1, sizeof(out)-1, tmp);
    fclose(tmp);
    return strstr(out, text) != NULL;
  }
  GenDataResourceTestPatch(){
    const char* name = "Build/Test/gendata.wav";
    // stereo 16-bit PCM, 4 frames
    const int16_t pcm[] = { 1000, -1000, 2000, -2000, 3000, -3000, 4000, -4000 };
    uint8_t buf[44 + sizeof(pcm)];
    writeFile(name, buf, makeWav(buf, 1, 2, 16, pcm, sizeof(pcm)));
    {
      TEST("data filled from resource");
      Data data;
      CHECK(!printed("WAV", [&]{ data.reset(name, 4, 2); })); // sizes match: no message
      CHECK_EQUAL((int)data.dim, 4);
      CHECK_EQUAL((int)data.channels, 2);
      for(int i=0; i<4; ++i){
	CHECK_CLOSE(data.read(i, 0), pcm[2*i]/32768.0f, 1e-6);
	CHECK_CLOSE(data.read(i, 1), pcm[2*i+1]/32768.0f, 1e-6);
      }
      data.reset(name, 4, 2); // gen~ reset: resource data is kept
      CHECK_CLOSE(data.read(3, 1), pcm[7]/32768.0f, 1e-6);
    }
    {
      TEST("declared longer than resource");
      Data data;
      CHECK(printed("data frames / WAV frames 6 4", [&]{ data.reset(name, 6, 2); }));
      CHECK_EQUAL((int)data.dim, 6);
      for(int i=0; i<4; ++i)
	CHECK_CLOSE(data.read(i, 0), pcm[2*i]/32768.0f, 1e-6);
      CHECK_EQUAL(data.read(4, 0), 0);
      CHECK_EQUAL(data.read(5, 1), 0);
    }
    {
      TEST("declared mono");
      Data data;
      CHECK(printed("data channels / WAV channels 1 2", [&]{ data.reset(name, 4, 1); }));
      CHECK_EQUAL((int)data.channels, 1);
      for(int i=0; i<4; ++i)
	CHECK_CLOSE(data.read(i, 0), pcm[2*i]/32768.0f, 1e-6); // left channel kept
    }
    {
      TEST("plain data is zeroed");
      Data data;
      data.reset("no-such-resource", 8, 1);
      CHECK_EQUAL((int)data.dim, 8);
      data.write(0.5f, 3, 0);
      CHECK_EQUAL(data.read(3, 0), 0.5f);
      data.reset("no-such-resource", 8, 1); // gen~ reset clears plain data
      CHECK_EQUAL(data.read(3, 0), 0);
    }
    remove(name);
  }
};
