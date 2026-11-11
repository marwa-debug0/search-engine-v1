#include <catch2/catch_test_macros.hpp>

int add(int a, int b) {
    return a + b;
}

TEST_CASE("Addition works correctly") {
    REQUIRE(add(2, 3) == 5);
    REQUIRE(add(0, 0) == 0);
    REQUIRE(add(-1, 1) == 0);
}
