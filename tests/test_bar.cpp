#include <cassert>
#include <iostream>
#include "qs/Bar.hpp"

int main()
{
    using namespace qs;
    Bar b;
    b.open = 1.0;
    b.high = 2.0;
    b.low = 0.5;
    b.close = 1.5;
    b.volume = 1000.0;

    assert(b.open == 1.0);
    assert(b.close == 1.5);
    assert(b.volume == 1000.0);

    std::cout << "test_bar: OK\n";
    return 0;
}
