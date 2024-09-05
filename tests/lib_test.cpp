#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"
#include "lib.hpp"

TEST_CASE("testing the greet function") {
	std::string dummyName = "Anand Mohan";
	CHECK(lib::greet(dummyName) == "Hello " + dummyName);
}