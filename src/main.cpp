#include <array>
#include <iostream>
#include <string>
#include <string_view>
#include <tuple>

template<typename M, typename T>
struct BINDING {
    M T::* member;
    std::string_view name;
};

template<typename M, typename T>
constexpr BINDING<M, T> MakeBinding(M T::* member, std::string_view name) {
    return BINDING<M, T>{member, std::string_view{ name }};
}

///////////////////////

template <class T> struct LEXICOGRAPHICAL_EQUALITY {
    // Build lexigraphical equality operator from serialization metadata
    friend constexpr bool operator==(const T& lhs, const T& rhs) {
        auto CompareProperty = [&lhs, &rhs](auto &&...metadata) -> bool {
            return ((lhs.*metadata.member == rhs.*metadata.member) && ...);
            };
        return std::apply(CompareProperty, T::DefineMemberMapping());
    }

    // Build lexigraphical inequality operator from serialization metadata
    friend constexpr bool operator!=(const T& lhs, const T& rhs) { return !(lhs == rhs); }

private:
    friend T;
    LEXICOGRAPHICAL_EQUALITY() = default; // Protect against mismatched inheritance
};

///////////////////////

std::string ToString(int x) { // Cannot be constexpr until C++20
    return std::to_string(x);
}

constexpr std::string_view ToString(std::string_view sv) {
    return sv;
}

constexpr std::string_view ToString(const char& c) {
    return { &c, 1 };
}

template <class T> std::string serializeFromMetadata(const T& object) {
    auto result = std::string{ "{\n" };
    auto ConcatenateElement = [&result, &object](auto &&...element) {
        ((result.push_back('\t'),
            result.append(element.name),
            result.append(" : "),
            result.append(ToString(object.*(element.member))),
            result.append(",\n")),
            ...);
        };
    constexpr auto list = T::DefineMemberMapping(); // Create this separately to elide runtime call
    std::apply(ConcatenateElement, list);
    // std::apply(ConcatenateElement, T::DefineMemberMapping()); <= This form would make a runtime call on GCC 14
    result.push_back('}');
    return result;
}

template <class T> struct SERIALIZATION {
    [[nodiscard]] std::string serialize() const {
        return serializeFromMetadata(static_cast<const T&>(*this));
    }

    void deserialize(std::string_view input) {
        /// @todo
    }

private:
    friend T;
    SERIALIZATION() = default; // Protect against mismatched inheritance
};

///////////////////////

struct FOO : public SERIALIZATION<FOO>, LEXICOGRAPHICAL_EQUALITY<FOO> {
    int one;
    std::string_view two;
    char three;

    constexpr FOO(int one_, std::string_view two_, char three_) : one{ one_ }, two{ two_ }, three{ three_ } {}

    static constexpr auto DefineMemberMapping() {
        return std::make_tuple(MakeBinding(&FOO::one, "one"), MakeBinding(&FOO::two, "two"), MakeBinding(&FOO::three, "three"));
    }
};

int main() {
    /// Example of compile-time definition & usage of member <-> name mapping
    constexpr auto myVar = FOO{ 1, "abc", '-' };
    constexpr auto foo1 = FOO{ 1, "abc", '-' };
    constexpr auto foo2 = FOO{ 2, "abc", '-' };

    static_assert(myVar == foo1);
    static_assert(myVar != foo2);

    std::cout << myVar.serialize() << std::endl;
    return 0;
}
