#pragma once

#include <array>
#include <map>
#include <optional>
#include <set>
#include <vector>

namespace serializable::traits {

template <typename T> inline constexpr bool isOptional = false;
template <typename T> inline constexpr bool isOptional<std::optional<T>> = true;

template <typename T> inline constexpr bool isSmartPtr = false;
template <typename... Args> inline constexpr bool isSmartPtr<std::shared_ptr<Args...>> = true;
template <typename... Args> inline constexpr bool isSmartPtr<std::unique_ptr<Args...>> = true;

template <typename T> inline constexpr bool isStdArray = false;
template <typename T, std::size_t N> inline constexpr bool isStdArray<std::array<T, N>> = true;

template <typename T> inline constexpr bool isVector = false;
template <typename... Args> inline constexpr bool isVector<std::vector<Args...>> = true;

template <typename T> inline constexpr bool isSet = false;
template <typename... Args> inline constexpr bool isSet<std::set<Args...>> = true;

template <typename T> inline constexpr bool isMap = false;
template <typename... Args> inline constexpr bool isMap<std::map<Args...>> = true;

template <typename... Args>
inline constexpr bool isContainer = isStdArray<Args...> || isVector<Args...> || isSet<Args...> || isMap<Args...>;

// Indirection protects against confusing compilation errors when capabilities are missing
namespace hasSerializationImpl {
template <typename T, typename = void, typename = void, typename = void, typename = void>
struct hasSerialization : std::false_type { };
template <typename T>
struct hasSerialization<
    T,
    typename std::enable_if_t<std::is_invocable_r_v<std::string, decltype(&T::serialize), const T&>>,
    typename std::enable_if_t<std::is_invocable_r_v<T, decltype(T::deserialize), std::string_view>>
> : std::true_type { };
} // namespace hasSerializationImpl

template <class T>
inline constexpr bool hasSerializationInterface{ hasSerializationImpl::hasSerialization<std::decay_t<T>>::value };

} // namespace serializable::traits