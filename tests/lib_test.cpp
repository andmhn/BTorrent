#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "lib.hpp"

#include "doctest.h"

TEST_CASE("testing the greet function") {
  std::string dummyName = "Anand Mohan";
  CHECK(lib::greet(dummyName) == "Hello " + dummyName);
}