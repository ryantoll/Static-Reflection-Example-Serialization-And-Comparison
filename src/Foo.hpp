#ifndef FOO_HPP
#define FOO_HPP 1

#include "Serial_CRTP.hpp"

struct FOO : public SERIALIZATION<FOO>, LEXICOGRAPHICAL_EQUALITY<FOO> {
    int one;
    std::string_view two;
    char three;

    constexpr FOO(int one_, std::string_view two_, char three_) : one{ one_ }, two{ two_ }, three{ three_ } {}

    static constexpr auto DefineMemberMapping() {
        return std::make_tuple(MakeBinding(&FOO::one, "one"), MakeBinding(&FOO::two, "two"), MakeBinding(&FOO::three, "three"));
    }
};

#endif // !FOO_HPP