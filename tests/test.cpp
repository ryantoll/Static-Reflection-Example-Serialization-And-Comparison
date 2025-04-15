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

TEST_CASE("Serialization (in)equality") {
    constexpr auto myVar = FOO{ 1, "abc", '-' };
    constexpr auto exactMatch = FOO{ 1, "abc", '-' };
    constexpr auto fieldOneMismatch = FOO{ 2, "abc", '-' };
    constexpr auto fieldTwoMismatch = FOO{ 1, "cba", '-' };
    constexpr auto fieldThreeMismatch = FOO{ 1, "abc", '*' };

    REQUIRE(myVar.serialize() == exactMatch.serialize());
    REQUIRE_FALSE(myVar.serialize() == fieldOneMismatch.serialize());
    REQUIRE_FALSE(myVar.serialize() == fieldTwoMismatch.serialize());
    REQUIRE_FALSE(myVar.serialize() == fieldThreeMismatch.serialize());
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

TEST_CASE("Serialization only includes fields declared in mapping") {
    constexpr auto myVar = BAR{ 1, "abc", "Extra notes" };
    constexpr auto exactMatch = BAR{ 1, "abc", "Extra notes" };
    constexpr auto ignoredFieldMismatch = BAR{ 1, "abc", "Extra notes do not participate in equality check." };
    constexpr auto fieldOneMismatch = BAR{ 3, "abc", "N/A" };
    constexpr auto fieldTwoMismatch = BAR{ 1, "4", "N/A" };

    REQUIRE(myVar.serialize() == exactMatch.serialize()); // Exact match
    REQUIRE(myVar.serialize() == ignoredFieldMismatch.serialize()); // Field "ignoreMe" is mismatched, but is not mapped
    REQUIRE_FALSE(myVar.serialize() == fieldOneMismatch.serialize()); // Field "one" is mismatched
    REQUIRE_FALSE(myVar.serialize() == fieldTwoMismatch.serialize()); // Field "two" is mismatched
}

// This struct has a trivial mapping
struct BAZ : public SERIALIZATION<BAZ>, LEXICOGRAPHICAL_EQUALITY<BAZ> {
    char c{' '};

    constexpr BAZ() = default;

    constexpr explicit BAZ(char c_) : c{ c_ } {}

    static constexpr auto DefineMemberMapping() {
        return std::make_tuple(); // Empty
    }
};

TEST_CASE("Objects with empty mapping always compare equal") {
    constexpr auto myVar = BAZ{ };
    constexpr auto exactMatch = BAZ{ };
    constexpr auto irrelevantChange = BAZ{ '?' };

    STATIC_REQUIRE(myVar == exactMatch);
    STATIC_REQUIRE(myVar == irrelevantChange);
}

TEST_CASE("Objects with empty mapping always serialize as empty") {
    constexpr auto myVar = BAZ{ };
    constexpr auto exactMatch = BAZ{ };
    constexpr auto irrelevantChange = BAZ{ '?' };

    REQUIRE(myVar.serialize() == "{\n}");
    REQUIRE(myVar.serialize() == exactMatch.serialize());
    REQUIRE(myVar.serialize() == irrelevantChange.serialize());
}
