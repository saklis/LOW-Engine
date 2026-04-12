#include <catch2/catch_test_macros.hpp>

// Smoke test — verifies the test target builds and links correctly.
TEST_CASE("Sanity check", "[smoke]") {
    REQUIRE(1 + 1 == 2);
}