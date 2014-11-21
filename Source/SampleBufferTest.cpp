/*
g++ -Wall -g -I/opt/local/include -L/opt/local/lib -o SampleBufferTest  SampleBufferTest.cpp -lboost_unit_test_framework  && ./SampleBufferTest
*/

#include <stdint.h>
typedef int16_t q15_t;
typedef int32_t q31_t;
#define AUDIO_BITDEPTH 16
#define AUDIO_CHANNELS 2
#include "SampleBuffer.hpp"

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Test
#include <boost/test/unit_test.hpp>


// tolerance for test comparison in percentage units
#define TEST_PRECISION 0.00001f

BOOST_AUTO_TEST_CASE(universeInOrder){
    BOOST_CHECK(2+2 == 4);
}

BOOST_AUTO_TEST_CASE(testSplit){
  SampleBuffer buffer;
  buffer.setSize(128);
  float samples[256];
  for(int i=0; i<256; ++i)
    samples[i] = i/128.0 - 1.0f;
  int16_t input[512];
  for(int i=0; i<256; ++i){
#if AUDIO_BITDEPTH == 24
#ifdef AUDIO_BIGEND
//     // big endian
    input[i*2] = (((int32_t)(samples[i]*2147483648.0f)) >> 16) & 0xffff; // MSB
    input[i*2+1] = ((int32_t)(samples[i]*2147483648.0f)) & 0xffff; // LSB
#else
    // little endian
    input[i*2] = ((int32_t)(samples[i]*2147483648.0f)) & 0xffff; // LSB
    input[i*2+1] = (((int32_t)(samples[i]*2147483648.0f)) >> 16) & 0xffff; // MSB
#endif
#else /* AUDIO_BITDEPTH == 24 */
    input[i] = ((int16_t)(samples[i]*32768.0f));
#endif
  }
//     input[i] = ((int32_t)(samples[i]*8388608.0f))<<8;
  buffer.split(input, 256);
  for(int i=0; i<128; ++i){
    BOOST_CHECK_CLOSE(buffer.getSamples(0)[i], samples[i*2], TEST_PRECISION);
    BOOST_CHECK_CLOSE(buffer.getSamples(1)[i], samples[i*2+1], TEST_PRECISION);
  }
}

// BOOST_AUTO_TEST_CASE(testComb){
//   SampleBuffer<128> buffer;
//   float samples[128];
//   for(int i=0; i<128; ++i)
//     samples[i] = i/64.0 - 1.0f;

//   for(int i=0; i<128; ++i){
//     buffer.getSamples(0)[i] = samples[i];
//     buffer.getSamples(1)[i] = samples[i];
//   }
//   int16_t output[512];
//   buffer.comb(output);
//   for(int i=0; i<512; ++i){
//   }

//   for(int i=0; i<256; ++i){
//     input[i*2] = (((int32_t)(samples[i]*2147483648.0f)) >> 16) & 0xffff;
//     input[i*2+1] = ((int32_t)(samples[i]*2147483648.0f)) & 0xffff;
//   }
// //     input[i] = ((int32_t)(samples[i]*8388608.0f))<<8;
//   buffer.split(input);
//   for(int i=0; i<128; ++i){
//     BOOST_CHECK_CLOSE(buffer.getSamples(0)[i], samples[i*2], TEST_PRECISION);
//     BOOST_CHECK_CLOSE(buffer.getSamples(1)[i], samples[i*2+1], TEST_PRECISION);
//   }
// }

BOOST_AUTO_TEST_CASE(testCombAndSplit){
  SampleBuffer buffer;
  buffer.setSize(128);
  float samples[128];
  int16_t output[512];

  for(int i=0; i<128; ++i)
    samples[i] = i/64.0 - 1.0f;

  for(int i=0; i<128; ++i){
    buffer.getSamples(0)[i] = samples[i];
    buffer.getSamples(1)[i] = samples[i];
  }

  buffer.comb(output);
  buffer.split(output, 256);

  for(int i=0; i<128; ++i){
    BOOST_CHECK_CLOSE(buffer.getSamples(0)[i], samples[i], TEST_PRECISION);
    BOOST_CHECK_CLOSE(buffer.getSamples(1)[i], samples[i], TEST_PRECISION);
  }
}

BOOST_AUTO_TEST_CASE(testMultiplyAndCombSplit){
  SampleBuffer buffer;
  buffer.setSize(128);
  float samples[128];
  int16_t output[512];

  for(int i=0; i<128; ++i)
    samples[i] = i/64.0 - 1.0f;

  for(int i=0; i<buffer.getSize(); ++i){
    buffer.getSamples(0)[i] = samples[i];
    buffer.getSamples(1)[i] = samples[i];
  }
  buffer.comb(output);
  buffer.split(output, 256);

  for(int i=0; i<buffer.getSize(); ++i){
//     buffer.getSamples(0)[i] = samples[i] * 0.9;
//     buffer.getSamples(1)[i] = samples[i] * 0.1;
    buffer.getSamples(0)[i] *= 0.9;
    buffer.getSamples(1)[i] *= 0.1;
  }

  for(int i=0; i<128; ++i){
    BOOST_CHECK_CLOSE(buffer.getSamples(0)[i], samples[i]*0.9, TEST_PRECISION);
    BOOST_CHECK_CLOSE(buffer.getSamples(1)[i], samples[i]*0.1, TEST_PRECISION);
  }
  // buffer.comb(output);
  // buffer.split(output, 256);
  // for(int i=0; i<128; ++i){
  //   BOOST_CHECK_CLOSE(buffer.getSamples(0)[i], samples[i]*0.9, TEST_PRECISION);
  //   BOOST_CHECK_CLOSE(buffer.getSamples(1)[i], samples[i]*0.1, TEST_PRECISION);
  // }
}

BOOST_AUTO_TEST_CASE(testCombAndSplitAndComb){
  SampleBuffer buffer;
  buffer.setSize(128);
  float samples[128];
  int16_t output[512];
  int16_t input[512];

  for(int i=0; i<128; ++i)
    samples[i] = i/64.0 - 1.0f;

  for(int i=0; i<128; ++i){
    buffer.getSamples(0)[i] = samples[i];
    buffer.getSamples(1)[i] = samples[i];
  }

  buffer.comb(output);
  buffer.split(output, 256);
  buffer.comb(input);

  for(int i=0; i<256; ++i)
    BOOST_CHECK_EQUAL(input[i], output[i]);
}

// BOOST_AUTO_TEST_CASE(testSplitAndComb){
//   SampleBuffer<128> buffer;
//   int32_t input[256];
//   int32_t output[256];
// //   input[0] = -1;
// //   input[1] = 2;
//   for(int i=0; i<256; ++i)
//     input[i] = (831828-i)<<8;
// //     input[i] = input[i-2]*input[i-1];
//   buffer.split((int16_t*)input);
//   buffer.comb((int16_t*)output);
//   for(int i=0; i<256; ++i)
//     BOOST_CHECK_EQUAL(input[i], output[i]);    
// }
  

AudioBuffer::~AudioBuffer(){}
