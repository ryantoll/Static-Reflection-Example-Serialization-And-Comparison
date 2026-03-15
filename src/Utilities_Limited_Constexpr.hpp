#ifndef LIMITED_CONSTEXPR_UTILITIES_HPP
#define LIMITED_CONSTEXPR_UTILITIES_HPP 1

#include <optional>
#include <type_traits>

/// <summary>
/// A series of limited utilities meant to provide "good enough" constexpr capabilities to perform constexpr unit tests.
/// Ideally these would be superceeded by standard library facilities in C++20 and beyond.
/// </summary>
namespace limited_constexpr {

// C-string to integral with minor checking. Overflow may occur.
template<class INTEGRAL>
constexpr std::optional<INTEGRAL> FromChars(const char* begin, const char* end) {
	auto result = INTEGRAL{};
	if (end <= begin) { return std::nullopt; }

	constexpr auto base = 10; // Start by building base 10

	bool isNegative = false;

	if constexpr (std::is_signed_v<INTEGRAL>) {
		if (*begin == '-') {
			isNegative = true;
			++begin;
		}
	}

	while (begin < end) {
		if (*begin < '0' || *begin > '9') {
			return std::nullopt;
		}
		result = result * base + (*begin - '0');
		++begin;
	}

	return isNegative ? -1 * result : result;
}

}

#endif // !LIMITED_CONSTEXPR_UTILITIES_HPP