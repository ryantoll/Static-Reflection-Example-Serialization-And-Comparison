#include "Foo.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Constexpr (in)equality") {
    static constexpr auto myVar = FOO{ 1, "abc", '-' };
    static constexpr auto exactMatch = FOO{ 1, "abc", '-' };
    static constexpr auto fieldOneMismatch = FOO{ 2, "abc", '-' };
    static constexpr auto fieldTwoMismatch = FOO{ 1, "cba", '-' };
    static constexpr auto fieldThreeMismatch = FOO{ 1, "abc", '*' };

    STATIC_REQUIRE(myVar == exactMatch);
    STATIC_REQUIRE(myVar != fieldOneMismatch);
    STATIC_REQUIRE(myVar != fieldTwoMismatch);
    STATIC_REQUIRE(myVar != fieldThreeMismatch);
}

TEST_CASE("Serialization (in)equality") {
    static constexpr auto myVar = FOO{ 1, "abc", '-' };
    static constexpr auto exactMatch = FOO{ 1, "abc", '-' };
    static constexpr auto fieldOneMismatch = FOO{ 2, "abc", '-' };
    static constexpr auto fieldTwoMismatch = FOO{ 1, "cba", '-' };
    static constexpr auto fieldThreeMismatch = FOO{ 1, "abc", '*' };

    REQUIRE(myVar.serialize() == exactMatch.serialize());
    REQUIRE_FALSE(myVar.serialize() == fieldOneMismatch.serialize());
    REQUIRE_FALSE(myVar.serialize() == fieldTwoMismatch.serialize());
    REQUIRE_FALSE(myVar.serialize() == fieldThreeMismatch.serialize());
}

TEST_CASE("Expected Serialization/Deserialization Output Check : FOO") {
    static constexpr auto myVar = FOO{ 1, "abc", '-' };
    static constexpr auto serializationOutput = std::string_view{ "{\n\tone : 1,\n\ttwo : abc,\n\tthree : -\n}" };

    CHECK(myVar.serialize() == serializationOutput.data());

    // Verify round-trip
    static auto deserializationResult = FOO::deserialize(serializationOutput);
    CHECK(myVar == deserializationResult);

    // Reorder elements
    static constexpr auto reorderedInput = std::string_view{ "{\n\ttwo : abc,\n\tthree : -,\n\tone : 1\n}" };
    static auto reorderedResult = FOO::deserialize(reorderedInput);
    CHECK(reorderedResult == deserializationResult);

    // Change whitespace
    static constexpr auto lessWhiteSpaceinput = std::string_view{ "{two : abc,\nthree : -,\tone : 1}" };
    static auto lessWhiteSpaceResult = FOO::deserialize(lessWhiteSpaceinput);
    CHECK(lessWhiteSpaceResult == deserializationResult);
}

// This struct has one field deliberately unmapped
struct BAR : public SERIALIZATION<BAR>, LEXICOGRAPHICAL_EQUALITY<BAR> {
    int one{ 0 };
    std::string_view two;
    std::string_view ignoreMe;

    constexpr BAR() = default;

    constexpr BAR(int one_, std::string_view two_, std::string_view ignoreMe_) : one{ one_ }, two{ two_ }, ignoreMe{ ignoreMe_ } {}

    static constexpr auto DefineMemberMapping() {
        return std::make_tuple(MakeBinding(&BAR::one, "one"), MakeBinding(&BAR::two, "two"));
    }
};

TEST_CASE("Equality only checks fields declared in mapping") {
    static constexpr auto myVar = BAR{ 1, "abc", "Extra notes" };
    static constexpr auto exactMatch = BAR{ 1, "abc", "Extra notes" };
    static constexpr auto ignoredFieldMismatch = BAR{ 1, "abc", "Extra notes do not participate in equality check." };
    static constexpr auto fieldOneMismatch = BAR{ 3, "abc", "N/A" };
    static constexpr auto fieldTwoMismatch = BAR{ 1, "4", "N/A" };

    STATIC_REQUIRE(myVar == exactMatch); // Exact match
    STATIC_REQUIRE(myVar == ignoredFieldMismatch); // Field "ignoreMe" is mismatched, but is not mapped
    STATIC_REQUIRE(myVar != fieldOneMismatch); // Field "one" is mismatched
    STATIC_REQUIRE(myVar != fieldTwoMismatch); // Field "two" is mismatched
}

TEST_CASE("Serialization only includes fields declared in mapping") {
    static constexpr auto myVar = BAR{ 1, "abc", "Extra notes" };
    static constexpr auto exactMatch = BAR{ 1, "abc", "Extra notes" };
    static constexpr auto ignoredFieldMismatch = BAR{ 1, "abc", "Extra notes do not participate in equality check." };
    static constexpr auto fieldOneMismatch = BAR{ 3, "abc", "N/A" };
    static constexpr auto fieldTwoMismatch = BAR{ 1, "4", "N/A" };

    REQUIRE(myVar.serialize() == exactMatch.serialize()); // Exact match
    REQUIRE(myVar.serialize() == ignoredFieldMismatch.serialize()); // Field "ignoreMe" is mismatched, but is not mapped
    REQUIRE_FALSE(myVar.serialize() == fieldOneMismatch.serialize()); // Field "one" is mismatched
    REQUIRE_FALSE(myVar.serialize() == fieldTwoMismatch.serialize()); // Field "two" is mismatched
}

TEST_CASE("Constexpr Expected Serialization/Deserialization Output Check: BAR") {
    static constexpr auto myVar = BAR{ -42, "abc", "-" };
    static constexpr auto serializationOutput = std::string_view{ "{\n\tone : -42,\n\ttwo : abc\n}" };;

    CHECK(myVar.serialize() == serializationOutput.data());

    // Verify round-trip
    static auto deserializationResult = BAR::deserialize(serializationOutput);
    CHECK(myVar == deserializationResult);

    // Demonstrate constexpr capability
    static constexpr auto deserializationResultConstexpr = BAR::deserialize(serializationOutput);
    STATIC_CHECK(myVar == deserializationResultConstexpr);
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
    static constexpr auto myVar = BAZ{ };
    static constexpr auto exactMatch = BAZ{ };
    static constexpr auto irrelevantChange = BAZ{ '?' };

    STATIC_REQUIRE(myVar == exactMatch);
    STATIC_REQUIRE(myVar == irrelevantChange);
}

TEST_CASE("Objects with empty mapping always serialize as empty") {
    static constexpr auto myVar = BAZ{ };
    static constexpr auto exactMatch = BAZ{ };
    static constexpr auto irrelevantChange = BAZ{ '?' };

    REQUIRE(myVar.serialize() == "{\n}");
    REQUIRE(myVar.serialize() == exactMatch.serialize());
    REQUIRE(myVar.serialize() == irrelevantChange.serialize());
}

struct FOO_BAR : public SERIALIZATION<FOO_BAR>, LEXICOGRAPHICAL_EQUALITY<FOO_BAR> {
    FOO foo{};
    BAR bar{};

    FOO_BAR() = default;

    constexpr FOO_BAR(FOO foo_, BAR bar_) : foo{ foo_ }, bar{ bar_ } { }

    static constexpr auto DefineMemberMapping() {
        return std::make_tuple(MakeBinding(&FOO_BAR::foo, "foo"), MakeBinding(&FOO_BAR::bar, "bar"));
    }
};
static_assert(serializable::traits::hasSerializationInterface<FOO_BAR>, "FOO_BAR should evaluate as serializable");

TEST_CASE("Recursive Serialization of Serializable Objects") {
    // A serializable object may hold objects that are serializable themselves
    static constexpr auto myFoo = FOO{ 1, "abc", '-' };
    static constexpr auto myBar = BAR{ 1, "abc", "Extra notes" };
    static constexpr auto myVar = FOO_BAR{ myFoo, myBar };
    static constexpr auto serializationOutput = std::string_view{ "{\n\tfoo : {\n\tone : 1,\n\ttwo : abc,\n\tthree : -\n},\n\tbar : {\n\tone : 1,\n\ttwo : abc\n}\n}" };

    CHECK(myVar.serialize() == serializationOutput.data());
}

struct FOO_OPTIONAL_BAR : public SERIALIZATION<FOO_OPTIONAL_BAR>, LEXICOGRAPHICAL_EQUALITY<FOO_OPTIONAL_BAR> {
    FOO foo{};
    std::optional<BAR> bar{};

    FOO_OPTIONAL_BAR() = default;

    constexpr FOO_OPTIONAL_BAR(FOO foo_, std::optional<BAR> bar_) : foo{ foo_ }, bar{ bar_ } { }

    static constexpr auto DefineMemberMapping() {
        return std::make_tuple(MakeBinding(&FOO_OPTIONAL_BAR::foo, "foo"), MakeBinding(&FOO_OPTIONAL_BAR::bar, "bar"));
    }
};
static_assert(serializable::traits::hasSerializationInterface<FOO_OPTIONAL_BAR>, "FOO_OPTIONAL_BAR should evaluate as serializable");

TEST_CASE("Recursive Serialization of Optional Serializable Objects") {
    // Empty optional is entirely omitted: both name and value
    static constexpr auto myFoo = FOO{ 1, "abc", '-' };
    static constexpr auto myVar = FOO_OPTIONAL_BAR{ myFoo, std::nullopt };
    static constexpr auto serializationOutput = std::string_view{ "{\n\tfoo : {\n\tone : 1,\n\ttwo : abc,\n\tthree : -\n}\n}" };

    CHECK(myVar.serialize() == serializationOutput.data());

    // Present optional is serialized the same as a plain value when present
    static constexpr auto myBar = BAR{ 1, "abc", "Extra notes" };
    static constexpr auto myVar2 = FOO_BAR{ myFoo, myBar };
    static constexpr auto serializationOutput2 = std::string_view{ "{\n\tfoo : {\n\tone : 1,\n\ttwo : abc,\n\tthree : -\n},\n\tbar : {\n\tone : 1,\n\ttwo : abc\n}\n}" };

    CHECK(myVar2.serialize() == serializationOutput2.data());
}

struct FOO_STRING_VIEW : public SERIALIZATION<FOO_STRING_VIEW>, LEXICOGRAPHICAL_EQUALITY<FOO_STRING_VIEW> {
    FOO foo{};
    std::string_view text{};

    FOO_STRING_VIEW() = default;

    constexpr FOO_STRING_VIEW(FOO foo_, std::string_view text_) : foo{ foo_ }, text{ text_ } { }

    static constexpr auto DefineMemberMapping() {
        return std::make_tuple(MakeBinding(&FOO_STRING_VIEW::foo, "foo"), MakeBinding(&FOO_STRING_VIEW::text, "text"));
    }
};
static_assert(serializable::traits::hasSerializationInterface<FOO_STRING_VIEW>, "FOO_STRING should evaluate as serializable");

TEST_CASE("Recursive Serialization of Fundamental Types Mixed with Complex Types") {
    // Mixed levels of recursion operate seemlessly
    static constexpr auto myFoo = FOO{ 1, "abc", '-' };
    static constexpr auto myText = std::string_view{ "Extra notes" };
    static constexpr auto myVar = FOO_STRING_VIEW{ myFoo, myText };
    static constexpr auto serializationOutput = std::string_view{ "{\n\tfoo : {\n\tone : 1,\n\ttwo : abc,\n\tthree : -\n},\n\ttext : Extra notes\n}" };

    CHECK(myVar.serialize() == serializationOutput.data());
}
