#include "utils.hpp"
#include "doctest.h"

TEST_CASE("test HumanReadableSize conversion") {
    using namespace utils;
    CHECK(BytesToString(1) == "1 B");
    CHECK(BytesToString(1024) == "1 KB");
    CHECK(BytesToString(1024 * 1024) == "1 MB");
    CHECK(BytesToString(1024 * 1024 * 1024) == "1 GB");

    long long TB = 1099511627776;
    CHECK(BytesToString(TB) == "1 TB");
    CHECK(BytesToString(TB * 2) == "2 TB");

    CHECK(BytesToString(20) == "20 B");
    CHECK(BytesToString(1023) == "1023 B");

    CHECK(BytesToString(1825) == "1.78 KB");
    CHECK(BytesToString(1548576) == "1.47 MB");
}