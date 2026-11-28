#include "TestPatch.hpp"
#include "WavFile.h"
#include "WavTestData.h"

/*
 * Unit test for WavFile: header parsing, per-channel read and interleaved
 * read of all channels (read(channel, output, true) / createFloatArray()).
 */
class WavFileTestPatch : public TestPatch {
public:
  WavFileTestPatch(){
    // stereo 16-bit PCM, 3 frames: L = 1000, 2000, 3000; R = -1000, -2000, -3000
    const int16_t pcm[] = { 1000, -1000, 2000, -2000, 3000, -3000 };
    uint8_t buf[44 + sizeof(pcm)];
    size_t len = makeWav(buf, 1, 2, 16, pcm, sizeof(pcm));
    {
      TEST("header");
      WavFile wav(buf, len);
      CHECK(wav.isValid());
      CHECK_EQUAL(wav.getNumberOfChannels(), 2);
      CHECK_EQUAL(wav.getNumberOfSamples(), 3);
    }
    {
      TEST("read one channel");
      WavFile wav(buf, len);
      FloatArray left = wav.createFloatArray(0);
      FloatArray right = wav.createFloatArray(1);
      CHECK_EQUAL(left.getSize(), 3);
      for(int i=0; i<3; ++i){
	CHECK_CLOSE(left[i], (i+1)*1000/32768.0f, 1e-6);
	CHECK_CLOSE(right[i], -(i+1)*1000/32768.0f, 1e-6);
      }
      FloatArray::destroy(left);
      FloatArray::destroy(right);
    }
    {
      TEST("read all channels interleaved");
      WavFile wav(buf, len);
      FloatArray all = wav.createFloatArray();
      CHECK_EQUAL(all.getSize(), 6);
      for(int i=0; i<6; ++i)
	CHECK_CLOSE(all[i], pcm[i]/32768.0f, 1e-6);
      FloatArray::destroy(all);
    }
    {
      TEST("read all into a short buffer");
      WavFile wav(buf, len);
      FloatArray out = FloatArray::create(4);
      wav.read(0, out, true);
      for(int i=0; i<4; ++i)
	CHECK_CLOSE(out[i], pcm[i]/32768.0f, 1e-6);
      FloatArray::destroy(out);
    }
    {
      TEST("mono float");
      const float f[] = { 0.25f, -0.5f, 1.0f, 0.0f };
      uint8_t fbuf[44 + sizeof(f)];
      size_t flen = makeWav(fbuf, 3, 1, 32, f, sizeof(f));
      WavFile wav(fbuf, flen);
      CHECK(wav.isValid());
      FloatArray all = wav.createFloatArray();
      CHECK_EQUAL(all.getSize(), 4);
      for(int i=0; i<4; ++i)
	CHECK_EQUAL(all[i], f[i]);
      FloatArray::destroy(all);
    }
  }
};
