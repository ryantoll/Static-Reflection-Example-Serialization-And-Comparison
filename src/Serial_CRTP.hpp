#ifndef SERIAL_CRTP_HPP
#define SERIAL_CRTP_HPP 1

#include <array>
#include <charconv>
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

///////////////////////

template <class T>
constexpr T FromStringView(std::string_view sv); // Provide a static interface

template <>
constexpr std::string_view FromStringView(std::string_view sv) {
    return sv;
}

template <>
int FromStringView(std::string_view sv) {
    auto result = int{};
    auto retCode = std::from_chars(sv.data(), sv.data() + sv.size(), result);
    if (retCode.ec == std::errc{}) { return result; }
    else { throw std::runtime_error{ "Error parsing int." }; }
}

template <>
constexpr char FromStringView(std::string_view sv) {
    return sv.at(0);
}

template <class T> constexpr T DeserializeFromMetadata(std::string_view input) {
    auto result = T{ };
    static constexpr auto list = T::DefineMemberMapping(); // Create this separately to elide runtime call
    auto lines = std::array<std::string_view, std::tuple_size_v<decltype(list)>>{ };

    input.remove_prefix(1); // '{'
    input.remove_suffix(1); // '}'

    // Get text of each value
    auto beginPos = size_t{ 0 };
    for (auto& subView : lines) {
        auto colonPos = input.find(':', beginPos);
        auto endPos = input.find('\n', colonPos);

        beginPos = endPos + 1; // Set next iteration start point
        if (endPos < input.size() && input[endPos - 1] == ',') { --endPos; }
        auto startValuePos = colonPos + 2; // Advance past ':' and ' '

        subView = input.substr(startValuePos, endPos - startValuePos);
    }

    // Iterate over member variables and assign values assuming each name is present and in same order
    auto counter = 0;
    auto DeserializeElement = [&result, &lines, &counter](auto &&...element) {
        ((result.*(element.member) = FromStringView<std::decay_t<decltype(result.*(element.member))>>(lines[counter++])), ...);
    };
    std::apply(DeserializeElement, list);

    return result;
}

///////////////////////

template <class T> struct SERIALIZATION {
    [[nodiscard]] std::string serialize() const {
        return serializeFromMetadata(static_cast<const T&>(*this));
    }

    [[nodiscard]] static constexpr T deserialize(std::string_view input) {
        static_assert(std::is_default_constructible_v<T>);
        return DeserializeFromMetadata<T>(input);
    }

private:
    friend T;
    SERIALIZATION() = default; // Protect against mismatched inheritance
};

#endif // !SERIAL_CRTP_HPP
