#include "../src/Foo.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Constexpr (in)equality") {
    constexpr auto myVar = FOO{ 1, "abc", '-' };
    constexpr auto exactMatch = FOO{ 1, "abc", '-' };
    constexpr auto fieldOneMismatch = FOO{ 2, "abc", '-' };
    constexpr auto fieldTwoMismatch = FOO{ 1, "cba", '-' };
    constexpr auto fieldThreeMismatch = FOO{ 1, "abc", '*' };

    STATIC_REQUIRE(myVar == exactMatch);
    STATIC_REQUIRE(myVar != fieldOneMismatch);
    STATIC_REQUIRE(myVar != fieldTwoMismatch);
    STATIC_REQUIRE(myVar != fieldThreeMismatch);
}

// This struct has one field deliberately unmapped
struct BAR : public SERIALIZATION<BAR>, LEXICOGRAPHICAL_EQUALITY<BAR> {
    int one;
    std::string_view two;
    std::string_view ignoreMe;

    constexpr BAR(int one_, std::string_view two_, std::string_view ignoreMe_) : one{ one_ }, two{ two_ }, ignoreMe{ ignoreMe_ } {}

    static constexpr auto DefineMemberMapping() {
        return std::make_tuple(MakeBinding(&BAR::one, "one"), MakeBinding(&BAR::two, "two"));
    }
};

TEST_CASE("Equality only checks fields declared in mapping") {
    constexpr auto myVar = BAR{ 1, "abc", "Extra notes" };
    constexpr auto exactMatch = BAR{ 1, "abc", "Extra notes" };
    constexpr auto ignoredFieldMismatch = BAR{ 1, "abc", "Extra notes do not participate in equality check." };
    constexpr auto fieldOneMismatch = BAR{ 3, "abc", "N/A" };
    constexpr auto fieldTwoMismatch = BAR{ 1, "4", "N/A" };

    STATIC_REQUIRE(myVar == exactMatch); // Exact match
    STATIC_REQUIRE(myVar == ignoredFieldMismatch); // Field "ignoreMe" is mismatched, but is not mapped
    STATIC_REQUIRE(myVar != fieldOneMismatch); // Field "one" is mismatched
    STATIC_REQUIRE(myVar != fieldTwoMismatch); // Field "two" is mismatched
}
