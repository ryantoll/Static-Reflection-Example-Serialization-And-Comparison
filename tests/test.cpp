#include "../src/Foo.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Constexpr (in)equality") {
    constexpr auto myVar = FOO{ 1, "abc", '-' };
    constexpr auto foo1 = FOO{ 1, "abc", '-' };
    constexpr auto foo2 = FOO{ 2, "abc", '-' };

    STATIC_REQUIRE(myVar == foo1);
    STATIC_REQUIRE(myVar != foo2);
}