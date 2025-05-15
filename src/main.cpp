#include "Foo.hpp"

int main() {
    /// Example of compile-time definition & usage of member <-> name mapping
    static constexpr auto myVar = FOO{ 1, "abc", '-' };
    std::cout << myVar.serialize() << std::endl;
    return 0;
}
