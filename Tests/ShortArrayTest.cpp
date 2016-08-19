/*
  g++ -ILibSource -ISource -I/opt/local/include -L/opt/local/lib -std=c++11 Tests/ShortArrayTest.cpp LibSource/ShortArray.cpp -std=gnu++11 -lboost_unit_test_framework -o ShortArrayTest && ./ShortArrayTest
*/
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Test
#include <boost/test/unit_test.hpp>
#include <inttypes.h>
#include "ShortArray.h"

extern "C"{
  void assert_failed(const char* msg, const char* location, int line){
    BOOST_CHECK_MESSAGE(false, msg);
  }
}

BOOST_AUTO_TEST_CASE(universeInOrder){
  BOOST_CHECK(2+2 == 4);
}

BOOST_AUTO_TEST_CASE(testDefaultConstructor){
  ShortArray empty;
  BOOST_CHECK_EQUAL(empty.getSize(), 0);
  BOOST_CHECK_EQUAL((short*)empty, (short*)0);
}
