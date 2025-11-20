#pragma once

#include <string>
#include "qs/Bar.hpp"
#include "qs/Order.hpp"

namespace qs
{

    struct Trade
    {
        std::string symbol;
        Side side{Side::Buy};
        int quantity{0};
        double price{0.0};
        Date date{};
        double commission{0.0};
        double slippage{0.0};
    };

} // namespace qs
