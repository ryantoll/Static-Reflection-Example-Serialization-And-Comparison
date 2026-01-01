#ifndef SERIAL_CRTP_HPP
#define SERIAL_CRTP_HPP 1

#include <array>
#include <iostream>
#include <string>
#include <string_view>
#include <tuple>

template<typename M, typename T>
struct BINDING {
    M T::* member;
    std::string_view name;

    constexpr BINDING(M T::* member_, std::string_view name_) : member{ member_ }, name{ name_ } {}
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
        constexpr auto list = T::DefineMemberMapping();
        return std::apply(CompareProperty, list);
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
    static constexpr auto list = T::DefineMemberMapping(); // Create this separately to elide runtime call
    std::apply(ConcatenateElement, list);

    auto maybeComma = result.end() - 2;
    if (maybeComma > result.begin()) { result.erase(maybeComma); }
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

#endif // !SERIAL_CRTP_HPP
