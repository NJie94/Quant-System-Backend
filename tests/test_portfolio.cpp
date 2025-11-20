#include <cassert>
#include <iostream>

#include "qs/Portfolio.hpp"

int main()
{
    using namespace qs;
    using namespace std::chrono;

    Portfolio p(100000.0);

    Date d = sys_days{year{2020} / month{1} / day{2}};

    Trade buy{"AAPL", Side::Buy, 100, 100.0, d, 1.0, 0.0};
    p.applyTrade(buy);
    p.markToMarket([&](const std::string &)
                   { return 100.0; });

    assert(p.cash() < 100000.0);
    assert(p.positionQty("AAPL") == 100);

    Trade sell{"AAPL", Side::Sell, 100, 110.0, d, 1.0, 0.0};
    p.applyTrade(sell);
    p.markToMarket([&](const std::string &)
                   { return 110.0; });

    assert(p.positionQty("AAPL") == 0);
    assert(p.equity() > 100000.0);

    std::cout << "test_portfolio: OK\n";
    return 0;
}
