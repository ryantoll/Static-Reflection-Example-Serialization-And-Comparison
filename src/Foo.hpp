#ifndef FOO_HPP
#define FOO_HPP 1

#include "Serial_CRTP.hpp"

struct FOO : public SERIALIZATION<FOO>, LEXICOGRAPHICAL_EQUALITY<FOO> {
    int one_{ 0 };
    std::string_view two_;
    char three_{ '\0' };

    FOO() = default;

    constexpr FOO(int one, std::string_view two, char three) : one_{ one }, two_{ two }, three_{ three } {}

    static constexpr auto DefineMemberMapping() {
        return std::make_tuple(MakeBinding(&FOO::one_, "one"), MakeBinding(&FOO::two_, "two"), MakeBinding(&FOO::three_, "three"));
    }
};

static_assert(serializable::traits::hasSerializationInterface<FOO>, "FOO should evaluate as serializable");

#endif // !FOO_HPP